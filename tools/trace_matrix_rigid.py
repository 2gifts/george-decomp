"""Finite overlap fixtures for the complete scalar rigid inverse body.

Reuse the reviewed scalar decoder without adding instruction forms. Read the
original code from the validated local ELF; export only synthetic words. This
host IEEE subset omits EE FCR, nonfinite arithmetic, timing and engine behavior.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN

ROOT = Path(__file__).resolve().parents[1]
ENTRY, END = 0x2A1098, 0x2A11A4
BUFFER, INPUT = 0x10000, 0x10080


class MatrixRigidTrace(CameraTrace):
    def fetch(self, pc):
        if pc % 4 or not ENTRY <= pc < END:
            raise ValueError('unreviewed rigid inverse instruction')
        offset = pc - 0xFF000
        if offset < 0 or offset + 4 > len(self.original):
            raise ValueError('rigid inverse instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    def library_call(self, target):
        raise ValueError('rigid inverse has no controlled library calls')

    def run(self, entry=ENTRY):
        pc = entry
        while pc != RETURN:
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            if annul:
                raise ValueError('unreviewed rigid inverse likely branch')
            if target is not None:
                if instruction != 0x03E00008 or target != RETURN:
                    raise ValueError('unreviewed rigid inverse control transfer')
                if self.execute(self.fetch(pc + 4), pc + 4) != (None, False):
                    raise ValueError('control transfer in rigid inverse delay')
                pc = target
            else:
                pc += 4


def make_fixture(original, seed, output):
    trace = MatrixRigidTrace(original)
    for address in range(BUFFER, BUFFER + 256):
        trace.memory[address] = 0
    state = seed
    for index in range(64):
        state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
        trace.single(BUFFER + index * 4, ((state % 41) - 20) / 8)
    initial = [trace.load(BUFFER + index * 4, 4) for index in range(64)]
    trace.r[4], trace.r[5], trace.r[31] = BUFFER + output, INPUT, RETURN
    trace.run()
    if trace.instruction_count != 67:
        raise ValueError('rigid inverse did not execute its complete 67 instructions')
    return {'seed': seed, 'output': output, 'initial': initial,
            'expected': [trace.load(BUFFER + index * 4, 4) for index in range(64)],
            'instruction_count': trace.instruction_count}


def fixtures(original):
    return [make_fixture(original, seed, output)
            for seed in range(1, 17) for output in (0, *range(68, 189, 4))]


def input_hash(cases):
    data = [{key: case[key] for key in ('seed', 'output', 'initial')} for case in cases]
    return hashlib.sha256(json.dumps(data, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic bounded finite inputs/outputs; no original code or assets. */',
             'struct MatrixRigidGolden { u32 output,initial[64],expected[64]; };',
             'static const struct MatrixRigidGolden matrix_rigid_golden[] = {']
    for case in cases:
        arrays = [','.join('0x%08Xu' % value for value in case[key]) for key in ('initial', 'expected')]
        lines.append('    {%du,{%s},{%s}},' % (case['output'], *arrays))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/matrix_rigid_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'limitation': __doc__, 'input_sha256': input_hash(cases), 'cases': cases}, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases))
    print('%d finite overlap fixtures; %d original instructions; input SHA256 %s' %
          (len(cases), sum(case['instruction_count'] for case in cases), input_hash(cases)))


if __name__ == '__main__':
    main()
