"""Finite fixtures from the two complete original query instruction bodies.

Reuse the reviewed scalar decoder with no controlled callees. Only synthetic
memory and results are exported. Host IEEE rounding/SQRT are bounded models;
EE exceptional values, FCR flags, cycles and denormal behavior are excluded.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN, Trace, rounded, scalar, word

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x29CF28, 0x29D044), (0x29DD60, 0x29DE44))
ENTRIES = tuple(start for start, _ in RANGES)
BUFFER, END = 0x20000, 0x20100
MEMORY_RANGES = ((BUFFER, END), (0x7F000, 0x81000))


class QueryTrace(CameraTrace):
    def fetch(self, pc):
        if pc & 3 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError('unreviewed curve-query instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('curve-query instruction outside original image')
        return struct.unpack_from('<I', self.original, offset)[0]

    def memory_check(self, address, size):
        if (size != 4 or address & 3
                or not any(start <= address and address + size <= end
                           for start, end in MEMORY_RANGES)
                or any(address + i not in self.memory for i in range(size))):
            raise ValueError('curve-query memory outside initialized aligned windows')

    def load(self, address, size):
        self.memory_check(address, size)
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        return Trace.save(self, address, value, size)

    def execute(self, instruction, pc):
        op, rs, rt = instruction >> 26, (instruction >> 21) & 31, (instruction >> 16) & 31
        fs, fn = (instruction >> 11) & 31, instruction & 63
        supported = instruction == 0
        if op == 0:
            supported |= fn in (0x2D, 8)
        elif op in (9, 4, 21, 0x31, 0x39):
            supported = True
        elif op == 15:
            supported = rs == 0
        elif op == 17:
            supported = rs == 4 or (rs == 8 and rt in (0, 1, 2, 3))
            if rs == 16:
                supported = fn in (0, 1, 2, 3, 4, 6, 0x34)
                if fn == 3 and self.f[rt] == 0:
                    raise ValueError('curve-query zero denominator outside finite model')
                if fn == 4 and self.f[rt] < 0:
                    raise ValueError('curve-query negative square root outside finite model')
        if not supported:
            raise ValueError('unsupported curve-query instruction encoding')
        if self.instruction_count >= 512:
            raise ValueError('curve-query instruction bound exceeded')
        if op == 21:  # The optional output uses a genuine BNEL delay store.
            self.instruction_count += 1
            immediate = instruction & 0xFFFF
            if immediate & 0x8000:
                immediate -= 0x10000
            taken = self.r[rs] != self.r[rt]
            self.r[0] = 0
            return ((pc + 4 + (immediate << 2)) & 0xFFFFFFFF, False) if taken else (None, True)
        return super().execute(instruction, pc)

    def library_call(self, target):
        raise ValueError('curve queries contain no library calls')

    def run(self, entry):
        if entry not in ENTRIES:
            raise ValueError('curve-query run requires an owned function entry')
        start, end = RANGES[ENTRIES.index(entry)]
        pc = entry
        while True:
            if not start <= pc < end:
                raise ValueError('curve-query control transfer outside owned body')
            instruction = self.fetch(pc)
            op, rs = instruction >> 26, (instruction >> 21) & 31
            branch = op in (4, 21) or (op == 17 and rs == 8)
            returning = op == 0 and instruction & 63 == 8
            target, annul = self.execute(instruction, pc)
            if branch or returning:
                if not annul:
                    if pc + 4 >= end:
                        raise ValueError('curve-query delay outside owned body')
                    delay = self.fetch(pc + 4)
                    delay_op, delay_rs = delay >> 26, (delay >> 21) & 31
                    if (delay_op in (4, 21) or (delay_op == 17 and delay_rs == 8)
                            or (delay_op == 0 and delay & 63 == 8)):
                        raise ValueError('curve-query control transfer in delay')
                    if self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('curve-query control transfer in delay')
                if returning:
                    if instruction != 0x03E00008 or target != RETURN:
                        raise ValueError('curve-query return requires actual JR31 to selected stop')
                    return
                pc = pc + 8 if target is None else target
            else:
                if target is not None or annul:
                    raise ValueError('unreviewed curve-query transfer')
                pc += 4


def initialized_trace(original, seed):
    trace = QueryTrace(original)
    for start, end in MEMORY_RANGES:
        for address in range(start, end):
            trace.memory[address] = 0
    state = seed
    for index in range(64):
        state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
        trace.single(BUFFER + index * 4, ((state % 65) - 32) / 8)
    trace.r[29], trace.r[31] = 0x80000, RETURN
    return trace


def make_fixture(original, routine, seed, offsets, values):
    trace = initialized_trace(original, seed)
    a, b, c, fraction = offsets
    for offset, sequence in zip((a, b, c), values):
        for i, value in enumerate(sequence):
            trace.single(BUFFER + (offset + i) * 4, value)
    initial = [trace.load(BUFFER + i * 4, 4) for i in range(64)]
    trace.r[4], trace.r[5] = BUFFER + a * 4, BUFFER + b * 4
    if routine == 0:
        trace.r[6], trace.r[7] = BUFFER + c * 4, 0 if fraction < 0 else BUFFER + fraction * 4
    trace.run(ENTRIES[routine])
    return dict(routine=routine, a=a, b=b, c=c, fraction=fraction, initial=initial,
                expected=[trace.load(BUFFER + i * 4, 4) for i in range(64)],
                result=word(trace.f[0]) if routine == 0 else trace.r[2],
                instruction_count=trace.instruction_count)


def fixtures(original):
    cases = []
    first, second = (1, -2, 3), (5, -2, 3)
    points = ((-1, -2, 3), first, (2, -2, 3), (3, -2, 3), second,
              (7, -2, 3), (3, 0, 3), (1, 0, 3), (5, 0, 3), (3, -2, 5),
              (-1, 4, -2), (7, -3, 0))
    for degenerate in (False, True):
        for point in points:
            for fraction in (-1, 44, 4, 5, 6, 12, 13, 14, 20, 21, 22):
                cases.append(make_fixture(original, 0, 1, (4, 12, 20, fraction),
                                          (first, first if degenerate else second, point)))
    layouts = ((4, 12, 20), (4, 4, 20), (4, 12, 4), (4, 12, 12),
               (4, 5, 6), (6, 5, 4), (4, 8, 5), (5, 8, 4),
               (4, 12, 11), (4, 11, 12), (12, 4, 11), (11, 12, 4))
    for seed in range(1, 9):
        first = (seed / 2, -seed / 4, 3)
        second = (-seed / 4, 2, seed / 8)
        point = (seed / 8, 1, -seed / 2)
        for a, b, c in layouts:
            for fraction in (-1, 44, a, b + 1, c + 2):
                cases.append(make_fixture(original, 0, seed, (a, b, c, fraction),
                                          (first, second, point)))
    # Strict contain/tangent/end comparisons, signs, zero/negative extent and
    # deliberately unnormalized directions; the original applies no guard.
    queries = ((0, 0, 0), (0.5, 0, 0), (1, 0, 0), (-1, 0, 0), (-2, 0, 0),
               (2, 0, 0), (3, 0, 0), (4, 0, 0), (2, 1, 0), (2, 1.25, 0),
               (2, 0, 1), (0, 1, 0), (0, 1.25, 0))
    for point in queries:
        for radius in (0, 1, -1):
            for length in (-1, 0, 1, 2, 3):
                cases.append(make_fixture(original, 1, 1, (4, 12, 0, -1),
                                          ((*point, radius), (0, 0, 0, 1, 0, 0, length))))
    for seed in range(1, 9):
        for a, b in ((4, 12), (4, 4), (4, 5), (5, 4), (4, 7), (7, 4)):
            for direction in ((0, 0, 0), (2, 0, 0), (0.5, -1, 0.25)):
                query = (seed / 2, -seed / 4, 3, seed / 4)
                ray = (1, -2, seed / 8, *direction, seed / 2 - 2)
                cases.append(make_fixture(original, 1, seed, (a, b, 0, -1), (query, ray)))
    return cases


def input_hash(cases):
    data = [{key: case[key] for key in ('routine', 'a', 'b', 'c', 'fraction', 'initial')}
            for case in cases]
    return hashlib.sha256(json.dumps(data, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic finite instruction-derived outputs; no original code/assets. */',
             'struct CurveQueryGolden { u32 routine,a,b,c; s32 fraction; u32 initial[64],expected[64],result; };',
             'static const struct CurveQueryGolden curve_query_golden[] = {']
    for case in cases:
        arrays = [','.join('0x%08Xu' % value for value in case[key]) for key in ('initial', 'expected')]
        lines.append('    {%du,%du,%du,%du,%d,{%s},{%s},0x%08Xu},' %
                     (case['routine'], case['a'], case['b'], case['c'], case['fraction'], *arrays, case['result']))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/curve_query_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(dict(limitation=__doc__, input_sha256=input_hash(cases), cases=cases), indent=2) + '\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases))
    print('%d finite fixtures; %d original instructions; max %d; input SHA256 %s' %
          (len(cases), sum(case['instruction_count'] for case in cases),
           max(case['instruction_count'] for case in cases), input_hash(cases)))


if __name__ == '__main__':
    main()
