"""Finite original intersection fixtures; actual published normalization executes.

Only bounded synthetic memory/results are exported. Three source-identified
soft-double callees have explicit normal/zero IEEE contracts. This scalar
model excludes EE exceptional/subnormal arithmetic, FCR and hardware timing.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import Trace, RETURN, scalar, word, is_control_transfer
from trace_segment_distance import normal_single, double_bits, double_value, MASK64

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x29EB98, 0x29F080), (0x29F080, 0x29F1DC))
ENTRIES = tuple(start for start, _ in RANGES)
NORMALIZE, NORMALIZE_END = 0x2A3538, 0x2A35C0
BUFFER, BUFFER_END = 0x20000, 0x20100
MEMORY_RANGES = ((BUFFER, BUFFER_END), (0x7F000, 0x81000))
CALLS = (0x374848, 0x373250, 0x372CC0, NORMALIZE)


class IntersectionTrace(CameraTrace):
    def __init__(self, original):
        super().__init__(original)
        self.calls = [0] * 4
        self.events = []
        self.output = 0
        self.output_size = 0
        self.branch_outcomes = []

    def fetch(self, pc):
        if pc & 3 or not any(start <= pc < end for start, end in (*RANGES, (NORMALIZE, NORMALIZE_END))):
            raise ValueError('unreviewed intersection instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('intersection instruction outside original image')
        return struct.unpack_from('<I', self.original, offset)[0]

    def memory_check(self, address, size):
        if (size not in (4, 8, 16) or address % size
                or not any(start <= address and address + size <= end for start, end in MEMORY_RANGES)
                or any(address + i not in self.memory for i in range(size))):
            raise ValueError('intersection memory outside initialized aligned windows')

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
            allowed |= fn in (8, 0x2D, 0x38)
        elif op in (3, 4, 9, 13, 20, 0x1E, 0x1F, 0x31, 0x37, 0x39, 0x3F):
            allowed = True
        elif op == 1:
            allowed = rt in (0, 1, 2)
        elif op == 15:
            allowed = rs == 0
        elif op == 17:
            allowed = rs == 4 or rs == 8 and rt in (0, 1, 2, 3)
            if rs == 16:
                allowed = fn in (0, 1, 2, 3, 4, 6, 7, 0x32, 0x34, 0x36)
                operands = (rt,) if fn == 4 else (rd,) if fn in (6, 7) else (rd, rt)
                for index in operands:
                    normal_single(self.f[index])
                if fn == 3 and self.f[rt] == 0:
                    raise ValueError('intersection zero denominator outside finite model')
                if fn == 4 and self.f[rt] < 0:
                    raise ValueError('intersection negative square root outside finite model')
        if not allowed:
            raise ValueError('unsupported intersection instruction encoding')
        if self.instruction_count >= 1200:
            raise ValueError('intersection instruction bound exceeded')
        if op == 0 and fn in (0x2D, 0x38):
            if fn == 0x2D and instruction >> 6 & 31 or fn == 0x38 and rs:
                raise ValueError('unsupported intersection 64-bit operand form')
            self.r[rd] = ((self.r[rs] + self.r[rt]) if fn == 0x2D else self.r[rt] << (instruction >> 6 & 31)) & MASK64
            self.r[0] = 0
            self.instruction_count += 1
            return None, False
        if op in (1, 20):
            self.instruction_count += 1
            immediate = instruction & 0xFFFF
            if immediate & 0x8000:
                immediate -= 0x10000
            signed = self.r[rs] & MASK64
            if signed & (1 << 63):
                signed -= 1 << 64
            taken = self.r[rs] == self.r[rt] if op == 20 else signed >= 0 if rt == 1 else signed < 0
            return (pc + 4 + immediate * 4, False) if taken else (None, op == 20 or rt == 2)
        result = super().execute(instruction, pc)
        if op == 0x31 or op == 17 and rs in (4, 16) and fn not in (0x32, 0x34, 0x36):
            index = rt if op == 0x31 else rd if rs == 4 else instruction >> 6 & 31
            normal_single(self.f[index])
        return result

    def record_call(self, target):
        if target not in CALLS:
            raise ValueError('unknown intersection call')
        index = CALLS.index(target)
        self.calls[index] += 1
        output = [self.load(self.output + i * 4, 4) if self.output and i < self.output_size else 0 for i in range(3)]
        a, b = self.r[4] & MASK64, self.r[5] & MASK64
        if target == NORMALIZE:
            coordinates = [self.load(a + i * 4, 4) for i in range(3)]
            self.events.extend([index, *coordinates, 0, 0, *output])
        else:
            if target == 0x374848:
                a = b = 0
            self.events.extend([index, a & 0xFFFFFFFF, a >> 32, b & 0xFFFFFFFF, b >> 32,
                                word(self.f[12]) if target == 0x374848 else 0, *output])
        if len(self.events) > 108:
            raise ValueError('intersection call event bound exceeded')

    def library_call(self, target):
        if target not in CALLS[:3]:
            raise ValueError('unknown intersection soft call')
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
            raise ValueError('intersection run requires owned entry')
        start, end = RANGES[ENTRIES.index(entry)]
        pc = entry
        helper_return = None
        while True:
            active_start, active_end = (NORMALIZE, NORMALIZE_END) if helper_return is not None else (start, end)
            if not active_start <= pc < active_end:
                raise ValueError('intersection transfer outside owned body')
            instruction = self.fetch(pc)
            op = instruction >> 26
            returning = op == 0 and instruction & 63 == 8
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if op != 3 and not returning:
                    self.branch_outcomes.append([pc, target is not None, annul])
                if not annul:
                    if pc + 4 >= active_end:
                        raise ValueError('intersection delay outside owned body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('intersection control transfer in delay')
                if returning:
                    stop = helper_return if helper_return is not None else RETURN
                    if instruction != 0x03E00008 or target != stop:
                        raise ValueError('intersection return requires actual JR31 to selected stop')
                    if helper_return is None:
                        return
                    pc, helper_return = helper_return, None
                elif op == 3:
                    if helper_return is not None:
                        raise ValueError('unreviewed nested normalization call')
                    if target == NORMALIZE:
                        self.record_call(target)
                        helper_return, pc = self.r[31], target
                    else:
                        self.library_call(target)
                        pc = self.r[31]
                else:
                    pc = pc + 8 if target is None else target
            else:
                if target is not None or annul:
                    raise ValueError('unreviewed intersection transfer')
                pc += 4


def make_fixture(original, routine, offsets, values, seed=1):
    trace = IntersectionTrace(original)
    for start, end in MEMORY_RANGES:
        for address in range(start, end):
            trace.memory[address] = 0
    for index in range(64):
        trace.single(BUFFER + index * 4, float((index * 7 + seed) % 17 - 8))
    count = 5 if routine == 0 else 2
    for offset, vector in zip(offsets[:count], values):
        for index, value in enumerate(vector):
            trace.single(BUFFER + (offset + index) * 4, normal_single(value))
    initial = [trace.load(BUFFER + i * 4, 4) for i in range(64)]
    for reg, offset in enumerate(offsets[:count], 4):
        trace.r[reg] = BUFFER + offset * 4
    trace.output = 0 if offsets[count] < 0 else BUFFER + offsets[count] * 4
    trace.output_size = 3 if routine == 0 else 1
    trace.r[9 if routine == 0 else 6] = trace.output
    trace.r[29], trace.r[31] = 0x80000, RETURN
    trace.run(ENTRIES[routine])
    return dict(routine=routine, offsets=offsets, initial=initial,
                expected=[trace.load(BUFFER + i * 4, 4) for i in range(64)],
                result=trace.r[2], calls=trace.calls, events=trace.events,
                instruction_count=trace.instruction_count, branch_outcomes=trace.branch_outcomes)


def fixtures(original):
    cases = []
    vertices = ((0, 0, 0), (4, 0, 0), (0, 4, 0))
    for x in (-1, 0, 0.5, 2, 5):
        for y in (-1, 0, 0.5, 2, 5):
            for z0, z1 in ((-2, 2), (2, -2), (0, 2), (-2, 0), (1, 2), (-2, -1), (0, 0)):
                values = ((x, y, z0), (x, y, z1), *vertices)
                for output in (-1, 44, 20, 21, 28, 36, 5, 13):
                    cases.append(make_fixture(original, 0, (4, 12, 20, 28, 36, output), values))
    for normal in ((0, 0, 1), (0, 0, -1), (1, 2, 3), (0, 0, 0)):
        for direction in ((0, 0, 1), (0, 0, -1), (0.5, 1, 2), (1, 0, 0)):
            for offset in (-1, 0, 1, 2, 3):
                for extent in (-2, 1, 2):
                    ray = (0.5, -1, 0, *direction, extent)
                    for output in (44, 4, 10, 20, 23):
                        cases.append(make_fixture(original, 1, (4, 20, output, -1, -1, -1), (ray, (*normal, offset))))
    # Explicit denominator neighbors exercise both full64 threshold comparisons.
    for bits in (0x3727C5AB, 0x3727C5AC, 0x3727C5AD):
        value = scalar(bits)
        cases.append(make_fixture(original, 0, (4, 12, 20, 28, 36, 44),
                                  ((0.5, 0.5, 0), (0.5, 0.5, value), *vertices)))
    for bits in (0x3A83126E, 0x3A83126F, 0x3A831270):
        value = scalar(bits)
        for sign in (1, -1):
            cases.append(make_fixture(original, 1, (4, 20, 44, -1, -1, -1),
                                      ((0, 0, 0, 0, 0, sign * value, 2), (0, 0, 1, sign * value))))
    # Shifted shared input windows, repeated vertices, degenerate triangles and
    # post-publication output aliases retain defined finite original behavior.
    layouts = ((4, 5, 20, 21, 22), (4, 12, 20, 20, 20), (4, 12, 20, 28, 20),
               (4, 12, 4, 28, 36), (4, 4, 20, 28, 36), (20, 12, 4, 5, 6))
    for layout in layouts:
        for output in (-1, 44, *layout, layout[2] + 1):
            cases.append(make_fixture(original, 0, (*layout, output), ((0.5, 0.5, -1), (0.5, 0.5, 1), *vertices)))
    for ray, plane in ((4, 4), (4, 5), (5, 4), (4, 7), (7, 4)):
        for output in (44, ray, ray + 6, plane, plane + 3):
            # Plane writes can change extent. Choose all-positive initialized
            # coordinates to keep every shared-window denominator nonzero.
            cases.append(make_fixture(original, 1, (ray, plane, output, -1, -1, -1),
                                      ((1, 2, 3, 2, 3, 4, 2), (1, 2, 3, 4))))
    # Non-axis geometry and both windings supplement the independent planar
    # grid; original arithmetic, rather than a mathematical rewrite, decides
    # cancellation and containment at shifted vertex/output aliases.
    triangles = (((1, -2, 3), (5, 0, 4), (0, 3, 5)),
                 ((1, -2, 3), (0, 3, 5), (5, 0, 4)),
                 ((0, 0, 0), (1, 1, 1), (2, 2, 2)))
    for triangle in triangles:
        for first, second in (((1, 0, -3), (1, 0, 9)), ((-4, 1, 2), (8, 1, 4)),
                              ((0, 0, 0), (1, 2, 3)), ((-2, 4, 8), (3, -2, -1))):
            for output in (-1, 44, 20, 21, 28, 29, 36, 37):
                cases.append(make_fixture(original, 0, (4, 12, 20, 28, 36, output), (first, second, *triangle)))
    for bits in (0x3F8020C4, 0x3F8020C5, 0x3F8020C6):
        value = scalar(bits)
        for output in (44, 10, 23):
            cases.append(make_fixture(original, 1, (4, 20, output, -1, -1, -1),
                                      ((0, 0, 0, 0, 0, 1, 1), (0, 0, 1, value))))
    state = 0x5E6A1B2D
    for seed in range(64):
        vectors = []
        for vector in range(5):
            coordinates = []
            for component in range(3):
                state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
                coordinates.append(((state >> 16) % 33 - 16) / 4)
            vectors.append(tuple(coordinates))
        for output in (-1, 44, 21, 29):
            cases.append(make_fixture(original, 0, (4, 12, 20, 28, 36, output), vectors, seed))
    return cases


def input_hash(cases):
    data = [{key: case[key] for key in ('routine', 'offsets', 'initial')} for case in cases]
    return hashlib.sha256(json.dumps(data, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic finite original outputs; no original code/assets. */',
             'struct IntersectionGolden { u32 routine; s32 offsets[6]; u32 initial[64],expected[64],calls[4],event_count,events[108],result; };',
             'static const struct IntersectionGolden intersection_golden[] = {']
    for case in cases:
        arrays = [','.join('0x%08Xu' % value for value in case[key]) for key in ('initial', 'expected', 'calls', 'events')]
        lines.append('    {%du,{%s},{%s},{%s},{%s},%du,{%s},0x%08Xu},' %
                     (case['routine'], ','.join(map(str, case['offsets'])), *arrays[:3], len(case['events']), arrays[3], case['result']))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/segment_intersection_trace.json')
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
