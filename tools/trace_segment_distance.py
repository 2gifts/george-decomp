"""Bounded original segment-distance fixtures with explicit soft-call contracts.

The complete selected body is read from the hash-validated local ELF. Only
synthetic memory, call operands and results are exported. Binary32 operations
use the reviewed scalar decoder. Four already source-identified soft-double
entries have finite normal/zero IEEE models; their exception/denormal behavior,
EE FCR/timing/precision quirks and arbitrary hardware memory are not modeled.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import Trace, RETURN, rounded, scalar, word, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
ENTRY, END = 0x29D5D0, 0x29DD5C
BUFFER, BUFFER_END = 0x20000, 0x20100
MEMORY_RANGES = ((BUFFER, BUFFER_END), (0x7F000, 0x81000))
CALLS = (0x374848, 0x373250, 0x372CC0, 0x3734F8)
MASK64 = (1 << 64) - 1
REGIONS = {0x29D7A8: 0, 0x29D898: 1, 0x29D8F8: 2, 0x29D7F8: 3,
           0x29DA50: 4, 0x29DA34: 5, 0x29DAAC: 6, 0x29D838: 7,
           0x29D984: 8, 0x29DB2C: 9, 0x29DB44: 10, 0x29DC20: 11}


def normal_single(value):
    bits = word(value)
    if not math.isfinite(value) or bits & 0x7FFFFFFF and not bits & 0x7F800000:
        raise ValueError('segment-distance operation outside finite normal/zero model')
    return value


def double_bits(value):
    if not math.isfinite(value):
        raise ValueError('nonfinite soft-double operand')
    return struct.unpack('<Q', struct.pack('<d', value))[0]


def double_value(bits):
    value = struct.unpack('<d', struct.pack('<Q', bits & MASK64))[0]
    if not math.isfinite(value) or bits & 0x7FFFFFFFFFFFFFFF and not bits & 0x7FF0000000000000:
        raise ValueError('soft-double operation outside finite normal/zero model')
    return value


class SegmentTrace(CameraTrace):
    def __init__(self, original):
        super().__init__(original)
        self.calls = [0] * len(CALLS)
        self.events = []
        self.regions = set()
        self.branch_outcomes = []
        self.fraction_pointers = (0, 0)

    def fetch(self, pc):
        if pc & 3 or not ENTRY <= pc < END:
            raise ValueError('unreviewed segment-distance instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('segment-distance instruction outside original image')
        return struct.unpack_from('<I', self.original, offset)[0]

    def memory_check(self, address, size):
        if (size not in (4, 8, 16) or address % size
                or not any(start <= address and address + size <= end for start, end in MEMORY_RANGES)
                or any(address + i not in self.memory for i in range(size))):
            raise ValueError('segment-distance memory outside initialized aligned windows')

    def load(self, address, size):
        self.memory_check(address, size)
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        return Trace.save(self, address, value, size)

    def execute(self, instruction, pc):
        op, rs, rt, rd, fn = (instruction >> 26, (instruction >> 21) & 31,
                              (instruction >> 16) & 31, (instruction >> 11) & 31, instruction & 63)
        allowed = instruction == 0
        if op == 0:
            allowed |= fn in (8, 0x2D)
        elif op in (3, 4, 9, 13, 21, 0x1E, 0x1F, 0x31, 0x37, 0x39, 0x3F):
            allowed = True
        elif op == 1:
            allowed = rt == 1  # BGEZ of signed software-comparison result.
        elif op == 15:
            allowed = rs == 0
        elif op == 17:
            allowed = rs == 4 or (rs == 8 and rt in (0, 1, 2, 3))
            if rs == 16:
                allowed = fn in (0, 1, 2, 3, 6, 7, 0x34, 0x36)
                for index in ((rd,) if fn in (6, 7) else (rd, rt)):
                    normal_single(self.f[index])
                if fn == 3 and self.f[rt] == 0:
                    raise ValueError('segment-distance zero denominator outside finite model')
        if not allowed:
            raise ValueError('unsupported segment-distance instruction encoding')
        if self.instruction_count >= 700:
            raise ValueError('segment-distance instruction bound exceeded')
        if pc in REGIONS:
            self.regions.add(REGIONS[pc])
        if op == 0 and fn == 0x2D:
            if instruction >> 6 & 31:
                raise ValueError('unsupported segment-distance DADDU operand form')
            self.r[rd] = (self.r[rs] + self.r[rt]) & MASK64
            self.r[0] = 0
            self.instruction_count += 1
            return None, False
        if op in (1, 21):
            self.instruction_count += 1
            immediate = instruction & 0xFFFF
            if immediate & 0x8000:
                immediate -= 0x10000
            signed = self.r[rs] & MASK64
            if signed & (1 << 63):
                signed -= 1 << 64
            taken = signed >= 0 if op == 1 else self.r[rs] != self.r[rt]
            self.r[0] = 0
            return (pc + 4 + immediate * 4, False) if taken else (None, op == 21)
        result = super().execute(instruction, pc)
        if op == 0x31 or op == 17 and rs in (4, 16) and fn not in (0x34, 0x36):
            index = rt if op == 0x31 else rd if rs == 4 else instruction >> 6 & 31
            normal_single(self.f[index])
        return result

    def library_call(self, target):
        if target not in CALLS:
            raise ValueError('unknown segment-distance soft call')
        index = CALLS.index(target)
        self.calls[index] += 1
        a, b = self.r[4] & MASK64, self.r[5] & MASK64
        # Fixed eight-word events include observed output memory at every call;
        # this independently verifies publication before the final absolute.
        output = [self.load(p, 4) if p else 0 for p in self.fraction_pointers]
        if target == 0x374848:
            a = b = 0  # Only f12 is an argument to this established entry.
        elif target == 0x3734F8:
            b = 0  # Only the first integer lane is consumed.
        self.events.extend([index, a & 0xFFFFFFFF, a >> 32,
                            b & 0xFFFFFFFF, b >> 32,
                            word(self.f[12]) if target == 0x374848 else 0, *output])
        if target == 0x374848:
            self.r[2] = double_bits(normal_single(self.f[12]))
        elif target == 0x373250:
            first, second = double_value(a), double_value(b)
            self.r[2] = ((first > second) - (first < second)) & MASK64
        elif target == 0x372CC0:
            self.r[2] = double_bits(double_value(a) - double_value(b))
        else:
            self.f[0] = normal_single(rounded(double_value(a)))

    def run(self, entry=ENTRY):
        if entry != ENTRY:
            raise ValueError('segment-distance run requires owned entry')
        pc = entry
        while True:
            if not ENTRY <= pc < END:
                raise ValueError('segment-distance transfer outside owned body')
            instruction = self.fetch(pc)
            op = instruction >> 26
            returning = op == 0 and instruction & 63 == 8
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if op != 3 and not returning:
                    self.branch_outcomes.append([pc, target is not None, annul])
                if not annul:
                    if pc + 4 >= END:
                        raise ValueError('segment-distance delay outside owned body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('segment-distance control transfer in delay')
                if returning:
                    if instruction != 0x03E00008 or target != RETURN:
                        raise ValueError('segment-distance return requires actual JR31 to selected stop')
                    return
                if op == 3:
                    self.library_call(target)
                    pc = self.r[31]
                else:
                    pc = pc + 8 if target is None else target
            else:
                if target is not None or annul:
                    raise ValueError('unreviewed segment-distance transfer')
                pc += 4


def make_fixture(original, values, offsets=(4, 12, 20, 28, 44, 45), seed=1):
    trace = SegmentTrace(original)
    for start, end in MEMORY_RANGES:
        for address in range(start, end):
            trace.memory[address] = 0
    for index in range(64):
        trace.single(BUFFER + index * 4, float((index * 7 + seed) % 17 - 8))
    for offset, vector in zip(offsets[:4], values):
        for index, value in enumerate(vector):
            trace.single(BUFFER + (offset + index) * 4, normal_single(value))
    initial = [trace.load(BUFFER + i * 4, 4) for i in range(64)]
    for reg, offset in enumerate(offsets, 4):
        trace.r[reg] = 0 if offset < 0 else BUFFER + offset * 4
    trace.fraction_pointers = tuple(trace.r[reg] for reg in (8, 9))
    trace.r[29], trace.r[31] = 0x80000, RETURN
    trace.run()
    return dict(offsets=offsets, initial=initial,
                expected=[trace.load(BUFFER + i * 4, 4) for i in range(64)],
                calls=trace.calls, events=trace.events, result=word(trace.f[0]),
                instruction_count=trace.instruction_count, regions=sorted(trace.regions),
                branch_outcomes=trace.branch_outcomes)


def fixtures(original):
    cases = []
    # Crossed axes independently exercise every region, with unclipped inputs.
    for x in (-4, -1, 0, 1, 2, 3, 6):
        for y in (-6, -3, -2, -1, 0, 1, 4):
            for z in (-1, 0, 2):
                values = ((0, 0, 0), (2, 0, 0), (x, y, z), (0, 2, 0))
                for outputs in ((44, 45), (-1, -1), (44, 44), (4, 28), (13, 21)):
                    cases.append(make_fixture(original, values, (4, 12, 20, 28, *outputs)))
    # Parallel and nearly parallel determinants on either side of the literal
    # tolerance; two opposing directions use distinct acute/obtuse branches.
    for direction in ((2, 0, 0), (-2, 0, 0), (1, 0.0005, 0),
                      (1, 0.001, 0), (1, 0.0015, 0), (0, 0, 0)):
        for x in (-4, -2, -1, 0, 1, 2, 4):
            for y in (0, 1, 3):
                for outputs in ((44, 45), (28, 20), (45, 45), (-1, 44), (44, -1)):
                    cases.append(make_fixture(original, ((0, 0, 0), (1, 0, 0), (x, y, 0), direction),
                                              (4, 12, 20, 28, *outputs)))
    # Shifted/partial vector overlap and output aliases, including a same-input
    # displacement pointer. Initial writes deliberately obey their listed order.
    layouts = ((4, 12, 20, 28), (4, 5, 6, 7), (7, 6, 5, 4), (4, 4, 20, 28),
               (4, 12, 4, 28), (4, 12, 20, 12), (4, 7, 8, 5), (5, 8, 7, 4))
    for seed in range(1, 17):
        values = ((seed / 4, -seed / 8, 1), (2, seed / 4, -1),
                  (-seed / 8, 1, seed / 4), (-1, seed / 2, 2))
        for layout in layouts:
            for outputs in ((44, 45), (layout[0] + 1, layout[3] + 2),
                            (layout[1], layout[1]), (-1, layout[2] + 1)):
                cases.append(make_fixture(original, values, (*layout, *outputs), seed))
    # Zero and signed-zero inputs preserve the compare/subtract absolute's
    # original signed-zero behavior; no general standard-fabs replacement.
    for zero in (0.0, -0.0):
        for direction0, direction1 in (((0, 0, 0), (0, 0, 0)), ((0, 0, 0), (1, 0, 0)),
                                       ((1, 0, 0), (0, 0, 0))):
            cases.append(make_fixture(original, ((zero, zero, zero), direction0,
                                                (zero, zero, zero), direction1)))
    # Reproducible uncorrelated directions/locations reach the secondary corner
    # clamps which an orthogonal-only geometric model cannot distinguish.
    state = 0x6D2B79F5
    for seed in range(256):
        values = []
        for vector in range(4):
            coordinates = []
            for component in range(3):
                state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
                coordinates.append(((state >> 16) % 65 - 32) / 4)
            values.append(tuple(coordinates))
        for outputs in ((44, 45), (12, 28), (44, 44)):
            cases.append(make_fixture(original, values, (4, 12, 20, 28, *outputs), seed))
    # Coincident interior points exercise cancellation and the final soft
    # subtract path when the rounded squared-distance expression is negative.
    for seed in range(1, 33):
        u, v = (1, seed / 4, -2), (-seed / 8, 2, 1)
        b = tuple(u[i] * 0.25 - v[i] * 0.75 for i in range(3))
        for outputs in ((44, 45), (4, 5), (-1, -1)):
            cases.append(make_fixture(original, ((0, 0, 0), u, b, v),
                                      (4, 12, 20, 28, *outputs), seed))
    assert set(range(12)) <= {r for case in cases for r in case['regions']}
    return cases


def input_hash(cases):
    data = [{key: case[key] for key in ('offsets', 'initial')} for case in cases]
    return hashlib.sha256(json.dumps(data, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic finite original-instruction outputs; no original code/assets. */',
             'struct SegmentDistanceGolden { s32 offsets[6]; u32 initial[64],expected[64],calls[4],event_count,events[64],result; };',
             'static const struct SegmentDistanceGolden segment_distance_golden[] = {']
    for case in cases:
        assert len(case['events']) <= 64
        arrays = [','.join('0x%08Xu' % value for value in case[key]) for key in ('initial', 'expected', 'calls', 'events')]
        offsets = ','.join(str(n) for n in case['offsets'])
        lines.append('    {{%s},{%s},{%s},{%s},%du,{%s},0x%08Xu},' %
                     (offsets, *arrays[:3], len(case['events']), arrays[3], case['result']))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/segment_distance_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(dict(limits=__doc__, input_sha256=input_hash(cases), cases=cases), indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(golden_header(cases), newline='\n')
    print('%d fixtures; %d original instructions; maximum %d; regions %s' %
          (len(cases), sum(c['instruction_count'] for c in cases), max(c['instruction_count'] for c in cases),
           sorted({r for c in cases for r in c['regions']})))
    print('input SHA256', input_hash(cases))


if __name__ == '__main__':
    main()
