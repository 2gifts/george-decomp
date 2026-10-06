"""Generate finite synthetic aliases for three reviewed scalar matrix bodies.

Reuses trace_geometry's bounded instruction subset and excludes EE exceptional
values, FCR behavior, timing and precision quirks. Reads the local validated
ELF; only synthetic input/output words are exported.
"""
import argparse
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x2A1CE0, 0x2A1D74), (0x2A2278, 0x2A232C),
          (0x2A2330, 0x2A2370))


class MatrixTrace(Trace):
    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError('unreviewed matrix instruction: %#x' % pc)
        return struct.unpack_from('<I', self.original, pc - 0xFF000)[0]

    def execute(self, instruction, pc):
        # The transpose loop is the only additional opcode in this scope.
        if instruction >> 26 == 1 and (instruction >> 16) & 31 == 1:
            self.instruction_count += 1
            if self.instruction_count > 3000:
                raise ValueError('instruction trace exceeded its bound')
            rs = (instruction >> 21) & 31
            imm = instruction & 0xFFFF
            imm = imm - 0x10000 if imm & 0x8000 else imm
            target = None
            if not self.r[rs] & 0x80000000:
                target = (pc + 4 + (imm << 2)) & 0xFFFFFFFF
            return target, False
        return super().execute(instruction, pc)


def fixtures(original):
    cases = []
    specs = []
    for source in (0, 4, 12, 16, 17, 24):
        for destination in (0, 1, 4, 8, 12, 16, 17, 24):
            specs.append((0, destination, 0, source))
    for destination in (0, 4, 7, 8, 9, 10, 12, 16, 20, 24):
        specs.append((2, destination, 8, 0))
    specs.extend(((1, 0, 0, 0), (1, 8, 8, 0)))
    for routine, output, matrix, source in specs:
        trace = MatrixTrace(original)
        base = 0x10000
        for index in range(48):
            trace.single(base + index * 4, (index % 17) - 8)
        trace.r[29], trace.r[31] = 0x80000, RETURN
        trace.f[12], trace.f[13], trace.f[14] = 0.25, -0.5, 2.0
        if routine == 0:
            trace.r[4], trace.r[5], trace.r[6] = base + matrix * 4, base + source * 4, base + output * 4
            entry = 0x2A1CE0
        else:
            trace.r[4], trace.r[5] = base + output * 4, base + matrix * 4
            entry = 0x2A2278 if routine == 1 else 0x2A2330
        initial = [trace.load(base + index * 4, 4) for index in range(48)]
        trace.run(entry)
        cases.append({'routine': routine, 'output': output, 'matrix': matrix,
                      'input': source, 'initial': initial,
                      'expected': [trace.load(base + index * 4, 4) for index in range(48)],
                      'instruction_count': trace.instruction_count})
    return cases


def golden_header(cases):
    lines = ['/* Finite instruction-derived synthetic aliases; no original code. */',
             'struct MatrixGolden { int routine,output,matrix,input; u32 initial[48],expected[48]; };',
             'static const struct MatrixGolden matrix_golden[] = {']
    for case in cases:
        initial = ','.join('0x%08Xu' % value for value in case['initial'])
        expected = ','.join('0x%08Xu' % value for value in case['expected'])
        lines.append('    {%d,%d,%d,%d,{%s},{%s}},' %
                     (case['routine'], case['output'], case['matrix'], case['input'], initial, expected))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/matrix_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'limitation': __doc__, 'cases': cases}, indent=2) + '\n', encoding='utf-8')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases), encoding='utf-8')
    print('%d finite original-instruction alias fixtures' % len(cases))


if __name__ == '__main__':
    main()
