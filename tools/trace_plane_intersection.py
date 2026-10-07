"""Bounded finite original plane-intersection fixtures and call observations.

Only synthetic memory/results are exported. The complete selected originals
execute; the reviewed 12-byte zero-fill and three pure soft-double entries
have controlled contracts. This model excludes EE exceptional/subnormal
arithmetic, FCR, helper timing and unknown original caller reachability.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import Trace, RETURN, scalar, word, is_control_transfer
from trace_segment_intersection import IntersectionTrace
from trace_segment_distance import normal_single, double_bits, double_value, MASK64

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x29F370, 0x29F62C), (0x29F630, 0x29F7C4))
ENTRIES = tuple(start for start, _ in RANGES)
BUFFER, BUFFER_END = 0x20000, 0x20100
MEMORY_RANGES = ((BUFFER, BUFFER_END), (0x7F000, 0x81000))
CALLS = (0x374848, 0x373250, 0x372CC0, 0x3936A0)


class PlaneTrace(IntersectionTrace):
    def __init__(self, original):
        super().__init__(original)
        self.outputs = (0, 0)

    def fetch(self, pc):
        if pc & 3 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError('unreviewed plane instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('plane instruction outside original image')
        return struct.unpack_from('<I', self.original, offset)[0]

    def execute(self, instruction, pc):
        op, rs, rt = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31
        if self.instruction_count >= 500:
            raise ValueError('plane instruction bound exceeded')
        if op == 20 or op == 1 and rt not in (0, 1):
            raise ValueError('unsupported plane branch encoding')
        if op == 17:
            if rs == 8 and rt not in (0, 1) or rs == 16 and instruction & 63 == 4:
                raise ValueError('unsupported plane COP1 encoding')
        if op == 10:
            immediate = instruction & 0xFFFF
            if immediate & 0x8000:
                immediate -= 0x10000
            value = self.r[rs] & MASK64
            if value & (1 << 63):
                value -= 1 << 64
            self.r[rt] = int(value < immediate)
            self.r[0] = 0
            self.instruction_count += 1
            return None, False
        if op == 0x2B:
            return CameraTrace.execute(self, instruction, pc)
        return super().execute(instruction, pc)

    def record_call(self, target):
        if target not in CALLS:
            raise ValueError('unknown plane call')
        index = CALLS.index(target)
        self.calls[index] += 1
        output = [self.load(address + i * 4, 4) if address else 0
                  for address in self.outputs for i in range(3)]
        a, b = self.r[4] & MASK64, self.r[5] & MASK64
        if target == 0x3936A0:
            # The stack pointer itself is deliberately not exported. The
            # reviewed original contract is a distinct initialized 12-byte
            # local, filled with zero; its target address has no native ABI.
            args = [0, self.r[6] & 0xFFFFFFFF, 0, 0, 0]
        else:
            if target == 0x374848:
                a = b = 0
            args = [a & 0xFFFFFFFF, a >> 32, b & 0xFFFFFFFF, b >> 32,
                    word(self.f[12]) if target == 0x374848 else 0]
        self.events.extend([index, *args, *output])
        if len(self.events) > 60:
            raise ValueError('plane call event bound exceeded')

    def library_call(self, target):
        if target not in CALLS:
            raise ValueError('unknown plane library call')
        if target == 0x3936A0:
            destination = self.r[4]
            if (self.r[5] != 0 or self.r[6] != 12 or destination & 3
                    or not 0x7F000 <= destination <= 0x81000 - 12):
                raise ValueError('unreviewed plane zero-fill arguments')
            self.record_call(target)
            for i in range(3):
                self.save(destination + i * 4, 0, 4)
            self.r[2] = destination
            return
        self.record_call(target)
        a, b = self.r[4] & MASK64, self.r[5] & MASK64
        if target == 0x374848:
            self.r[2] = double_bits(normal_single(self.f[12]))
        elif target == 0x373250:
            first, second = double_value(a), double_value(b)
            self.r[2] = ((first > second) - (first < second)) & MASK64
        else:
            self.r[2] = double_bits(double_value(a) - double_value(b))

    def run(self, entry):
        if entry not in ENTRIES:
            raise ValueError('plane run requires owned entry')
        start, end = RANGES[ENTRIES.index(entry)]
        pc = entry
        while True:
            if not start <= pc < end:
                raise ValueError('plane transfer outside owned body')
            instruction = self.fetch(pc)
            op = instruction >> 26
            returning = op == 0 and instruction & 63 == 8
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if op != 3 and not returning:
                    self.branch_outcomes.append([pc, target is not None, annul])
                if not annul:
                    if pc + 4 >= end:
                        raise ValueError('plane delay outside owned body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('plane control transfer in delay')
                if returning:
                    if instruction != 0x03E00008 or target != RETURN:
                        raise ValueError('plane return requires actual JR31 to selected stop')
                    return
                if op == 3:
                    self.library_call(target)
                    pc = self.r[31]
                else:
                    pc = pc + 8 if target is None else target
            else:
                if target is not None or annul:
                    raise ValueError('unreviewed plane transfer')
                pc += 4


def make_fixture(original, routine, offsets, values, seed=1):
    lengths = (4, 4) if routine == 0 else (3, 3, 4)
    if (routine not in (0, 1) or len(offsets) != 4
            or any(type(x) is not int or not 0 <= x <= 60 for x in offsets)
            or len(values) != len(lengths)
            or any(len(vector) != size for vector, size in zip(values, lengths))):
        raise ValueError('invalid bounded plane fixture layout')
    trace = PlaneTrace(original)
    for start, end in MEMORY_RANGES:
        for address in range(start, end):
            trace.memory[address] = 0
    for index in range(64):
        trace.single(BUFFER + index * 4, float((index * 7 + seed) % 17 - 8))
    for offset, vector in zip(offsets[:2 if routine == 0 else 3], values):
        for index, value in enumerate(vector):
            trace.single(BUFFER + (offset + index) * 4, normal_single(value))
    initial = [trace.load(BUFFER + i * 4, 4) for i in range(64)]
    for reg, offset in enumerate(offsets, 4):
        trace.r[reg] = BUFFER + offset * 4
    trace.outputs = ((trace.r[6], trace.r[7]) if routine == 0 else (trace.r[7], 0))
    trace.r[29], trace.r[31] = 0x80000, RETURN
    trace.run(ENTRIES[routine])
    return dict(routine=routine, offsets=offsets, initial=initial,
                expected=[trace.load(BUFFER + i * 4, 4) for i in range(64)],
                result=trace.r[2], calls=trace.calls, events=trace.events,
                instruction_count=trace.instruction_count, branch_outcomes=trace.branch_outcomes)


def fixtures(original):
    cases = []
    normals = (((1, 0, 0), (0, 1, 0)), ((0, 1, 0), (0, 0, 1)),
               ((0, 0, 1), (1, 0, 0)), ((1, 2, 3), (2, -1, 4)),
               ((1, 0, 0), (0, 2, 2)), ((0, 1, 0), (2, 0, 2)),
               ((1, 1, 0), (0, 0, 2)), ((1, 2, 3), (2, 4, 6)),
               ((0, 0, 0), (1, 2, 3)), ((-1, -2, -3), (2, -1, 4)))
    for a, b in normals:
        for first_w, second_w in ((0, 0), (1, 2), (-3, 4), (5, -2)):
            values = ((*a, first_w), (*b, second_w))
            for first_out, second_out in ((36, 44), (36, 36), (36, 37), (37, 36),
                                          (4, 44), (5, 12), (12, 4), (13, 5)):
                cases.append(make_fixture(original, 0, (4, 12, first_out, second_out), values))
    for bits in (0x38D1B716, 0x38D1B717, 0x38D1B718):
        for sign in (1, -1):
            for output in ((36, 44), (36, 37), (5, 13)):
                cases.append(make_fixture(original, 0, (4, 12, *output),
                                          ((0, 1, 0, 2), (0, 0, sign * scalar(bits), 3))))
    for first, second in ((4, 5), (5, 4), (4, 4), (4, 7), (7, 4)):
        for output in ((36, 44), (4, 5), (5, 4), (first + 1, second + 2)):
            cases.append(make_fixture(original, 0, (first, second, *output),
                                      ((1, 2, 3, 4), (3, 1, 2, -2))))
    planes = ((0, 0, 1, 0), (0, 0, -2, 3), (1, 2, 3, -4),
              (0, 0, 0, 4), (1, 0, 0, -2), (0, -0.0, 1, 2))
    endpoints = (((1, 2, -3), (1, 2, 5)), ((1, 2, 5), (1, 2, -3)),
                 ((-2, 1, 0), (3, -4, 2)), ((1, 2, 3), (1, 2, 3)),
                 ((1, 2, 3), (5, 2, 3)), ((-0.0, 2, -0.0), (0.0, 2, 0.0)))
    for plane in planes:
        for first, second in endpoints:
            for output in (40, 4, 5, 6, 12, 13, 20, 21, 23):
                cases.append(make_fixture(original, 1, (4, 12, 20, output), (first, second, plane)))
    for bits in (0x38D1B716, 0x38D1B717, 0x38D1B718):
        for sign in (1, -1):
            for output in (40, 4, 5, 20):
                cases.append(make_fixture(original, 1, (4, 12, 20, output),
                                          ((0, 0, 0), (0, 0, sign * scalar(bits)), (0, 0, 1, 1))))
    for offsets in ((4, 5, 20), (5, 4, 20), (4, 12, 4), (4, 12, 5),
                    (4, 12, 12), (4, 4, 20), (20, 12, 4)):
        for output in (40, *offsets, offsets[0] + 1):
            cases.append(make_fixture(original, 1, (*offsets, output),
                                      ((1, 2, 3), (4, 6, 9), (2, 3, 4, 5))))
    state = 0x504C414E
    for seed in range(96):
        values = []
        for _ in range(3):
            vector = []
            for _ in range(4):
                state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
                vector.append(((state >> 16) % 33 - 16) / 4)
            values.append(tuple(vector))
        for output in ((36, 44), (36, 37), (5, 13)):
            cases.append(make_fixture(original, 0, (4, 12, *output), values[:2], seed))
        for output in (40, 5, 21):
            cases.append(make_fixture(original, 1, (4, 12, 20, output),
                                      (values[0][:3], values[1][:3], values[2]), seed))
    return cases


def input_hash(cases):
    inputs = [{key: case[key] for key in ('routine', 'offsets', 'initial')} for case in cases]
    return hashlib.sha256(json.dumps(inputs, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic finite original outputs; no original code/assets. */',
             'struct PlaneGolden { u32 routine; s32 offsets[4]; u32 initial[64],expected[64],calls[4],event_count,events[60],result; };',
             'static const struct PlaneGolden plane_golden[] = {']
    for case in cases:
        arrays = [','.join('0x%08Xu' % value for value in case[key]) for key in ('initial', 'expected', 'calls', 'events')]
        lines.append('    {%du,{%s},{%s},{%s},{%s},%du,{%s},0x%08Xu},' %
                     (case['routine'], ','.join(map(str, case['offsets'])), *arrays[:3], len(case['events']), arrays[3], case['result']))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/plane_intersection_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(dict(limits=__doc__, input_sha256=input_hash(cases), cases=cases), indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(golden_header(cases), newline='\n')
    print('%d fixtures; %d original instructions; maximum %d' %
          (len(cases), sum(c['instruction_count'] for c in cases), max(c['instruction_count'] for c in cases)))
    print('input SHA256', input_hash(cases))


if __name__ == '__main__':
    main()
