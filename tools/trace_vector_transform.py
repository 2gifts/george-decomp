"""Execute the two complete original VU/MMI transform bodies.

Reuse the reviewed bounded decoder from record_collision unchanged. Products
and accumulator additions round separately to binary32; finite normal/zero
fixtures do not claim VU extended ACC precision, FCR flags or timing. Original
fourth lanes are executed, but only three output words are observed by callers.
"""
import argparse
import hashlib
import json
from pathlib import Path
import random

from analyze import validated_elf
from trace_geometry import RETURN, is_control_transfer, word
from trace_record_collision import RecordTrace

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x2A1C60, 0x2A1CDC), (0x2A1D78, 0x2A1DEC))
ENTRIES = tuple(a for a, _ in RANGES)
BUFFER, END = 0x30000, 0x30100
MATRIX, INPUT, OUTPUT = BUFFER, BUFFER + 0x80, BUFFER + 0xC0
WORDS = (END - BUFFER) // 4


class VectorTrace(RecordTrace):
    def memory_check(self, address, size):
        if (size not in (4, 16) or address % size or
                not BUFFER <= address < address + size <= END or
                any(address + i not in self.memory for i in range(size))):
            raise ValueError('transform memory outside initialized aligned window')

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in RANGES):
            raise ValueError('unreviewed transform instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('transform instruction outside original')
        return int.from_bytes(self.original[offset:offset + 4], 'little')

    def library_call(self, target):
        raise ValueError('transform has no helper calls')

    def run(self, entry, stop=RETURN):
        body = next((r for r in RANGES if r[0] == entry), None)
        if body is None:
            raise ValueError('transform invocation requires complete approved entry')
        pc = entry
        while True:
            if not body[0] <= pc < body[1]:
                raise ValueError('transform transfer outside complete body')
            if self.instruction_count >= 64:
                raise ValueError('transform instruction bound exceeded')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if not annul:
                    if pc + 4 >= body[1]:
                        raise ValueError('transform delay outside complete body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('transform control transfer in delay')
                if instruction == 0x03E00008 and target == stop:
                    return
                raise ValueError('transform requires actual JR31 return only')
            pc += 4


def fixture(original, routine=0, matrix=None, point=(2, -3, 4),
            input_offset=0x80, output_offset=0xC0):
    matrix = matrix or [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 3, -2, 1, 1]
    trace = VectorTrace(original)
    for address in range(BUFFER, END):
        trace.memory[address] = 0
    for i, value in enumerate(matrix):
        trace.save(MATRIX + 4 * i, word(value), 4)
    # Sentinels ensure that exactly XYZ, rather than a fourth word, is stored.
    for i in range(16, WORDS):
        trace.save(BUFFER + 4 * i, word((i - 24) * 0.25), 4)
    for i, value in enumerate(point):
        trace.save(BUFFER + input_offset + 4 * i, word(value), 4)
    initial = [trace.load(BUFFER + 4 * i, 4) for i in range(WORDS)]
    trace.r[4:7] = [MATRIX, BUFFER + input_offset, BUFFER + output_offset]
    trace.r[31] = RETURN
    trace.run(ENTRIES[routine])
    return dict(routine=routine, input_offset=input_offset, output_offset=output_offset,
                initial=initial, expected=[trace.load(BUFFER + 4 * i, 4) for i in range(WORDS)],
                instruction_count=trace.instruction_count)


def fixtures(original):
    matrices = [
        [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 3, -2, 1, 1],
        [0, 1, 0, 2, -1, 0, 0, -3, 0, 0, 1, 4, -4, 2, 3, 5],
        [2, 0.5, -1, 2, -3, 1, 0.25, 0, 0.75, -2, 4, 1, 2, 3, -1, 2],
        [0] * 16,
        [-0.0] * 16,
        [-(1 + 2**-22)] * 4 + [1 + 2**-23] * 4 + [0] * 8,
    ]
    points = ((0, 0, 0), (-0.0, -0.0, -0.0), (1, 0, 0), (0, -1, 0),
              (0, 0, 1), (2, -3, 4), (1, 1 + 2**-23, 0), (0.125, 8, -2))
    outputs = (0, 4, 8, 12, 16, 28, 32, 44, 48, 52, 56, 60,
               0x7C, 0x80, 0x84, 0x88, 0x8C, 0xC0, 0xF4)
    cases = [fixture(original, routine, matrix, point, output_offset=output)
             for routine in range(2) for matrix in matrices for point in points for output in outputs]
    for routine in range(2):
        for offset in (0, 4, 16, 32, 44, 48, 52, 60, 0x80):
            for output in (0, 32, 48, offset, offset + 4, 0xC0):
                cases.append(fixture(original, routine, matrices[2], (1, 2, -3), offset, output))
    generator = random.Random(0x2A1C60)
    for i in range(128):
        matrix = [generator.randrange(-64, 65) / 16 for _ in range(16)]
        point = [generator.randrange(-128, 129) / 16 for _ in range(3)]
        cases.append(fixture(original, i & 1, matrix, point,
                             output_offset=generator.choice(outputs)))
    return cases


def golden_header(cases):
    lines = ['/* Synthetic fixture values only; no original instructions/assets. */',
             'struct TransformGolden { unsigned routine,input_offset,output_offset; u32 initial[64],expected[64]; };',
             'static const struct TransformGolden transform_golden[] = {']
    for case in cases:
        lines.append(' {%d,%d,%d,{%s},{%s}},' %
                     (case['routine'], case['input_offset'], case['output_offset'],
                      ','.join('0x%08Xu' % v for v in case['initial']),
                      ','.join('0x%08Xu' % v for v in case['expected'])))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'build/vector_transform/trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(ROOT / 'orig/SLUS_216.68')
    cases = fixtures(original)
    header = golden_header(cases)
    report = dict(fixture_count=len(cases), fixtures=cases,
                  executed_original_instructions=sum(c['instruction_count'] for c in cases),
                  maximum_instructions=max(c['instruction_count'] for c in cases),
                  input_sha256=hashlib.sha256(json.dumps([{k: v for k, v in c.items() if k != 'expected' and k != 'instruction_count'}
                                                         for c in cases], sort_keys=True).encode()).hexdigest(),
                  header_sha256_lf=hashlib.sha256(header.encode()).hexdigest(), limitation=__doc__)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(header)
    print('TRANSFORM FIXTURES', len(cases), report['executed_original_instructions'], report['maximum_instructions'])


if __name__ == '__main__':
    main()
