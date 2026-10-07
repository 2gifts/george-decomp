"""Bounded finite six-face fixtures; all five original normalizers execute.

Only synthetic memory and call arguments are exported. This is a strict host
binary32 instruction observer, excluding EE exceptional/subnormal arithmetic,
FCR flags, timing and caller/class identity. No original instruction arrays
or assets are embedded. The original is read only after its complete hash check.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import Trace, RETURN, scalar, word, is_control_transfer
from trace_segment_distance import normal_single
from trace_segment_intersection import IntersectionTrace

ROOT = Path(__file__).resolve().parents[1]
ENTRY, END = 0x2A3DF8, 0x2A4520
NORMALIZE, NORMALIZE_END = 0x2A3538, 0x2A35C0
RANGES = ((ENTRY, END), (NORMALIZE, NORMALIZE_END))
BUFFER, WORDS = 0x20000, 128
MEMORY_RANGES = ((BUFFER, BUFFER + WORDS * 4), (0x7F000, 0x81000))


class FrustumTrace(IntersectionTrace):
    def __init__(self, original):
        super().__init__(original)
        self.calls = 0
        self.events = []
        self.output = BUFFER

    def fetch(self, pc):
        if pc & 3 or not any(start <= pc < end for start, end in RANGES):
            raise ValueError('unreviewed frustum instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('frustum instruction outside original image')
        return struct.unpack_from('<I', self.original, offset)[0]

    def memory_check(self, address, size):
        if (size not in (4, 8, 16) or address % size
                or not any(start <= address and address + size <= end for start, end in MEMORY_RANGES)
                or any(address + i not in self.memory for i in range(size))):
            raise ValueError('frustum memory outside initialized aligned windows')

    def execute(self, instruction, pc):
        op, rs, rt, fn = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31, instruction & 63
        allowed = instruction == 0 or (op == 0 and fn in (8, 0x2D))
        allowed |= op in (3, 4, 9, 13, 0x1E, 0x1F, 0x23, 0x2B, 0x31, 0x37, 0x39, 0x3F)
        allowed |= op == 15 and rs == 0
        if op == 17:
            allowed = rs == 4 or rs == 8 and rt in (0, 1, 2, 3) or rs == 16 and fn in (0, 1, 2, 3, 4, 6, 7, 0x32)
        if not allowed:
            raise ValueError('unsupported frustum instruction encoding')
        if self.instruction_count >= 1000:
            raise ValueError('frustum instruction bound exceeded')
        if op in (0x23, 0x2B):
            return CameraTrace.execute(self, instruction, pc)
        return super().execute(instruction, pc)

    def record_call(self, target):
        if target != NORMALIZE or self.calls >= 5:
            raise ValueError('unreviewed frustum normalization call')
        pointer = self.r[4]
        if pointer != self.output + self.calls * 28:
            raise ValueError('frustum normalization pointer/order mismatch')
        self.events.extend([(pointer - BUFFER) // 4, *[self.load(pointer + i * 4, 4) for i in range(3)]])
        self.calls += 1

    def library_call(self, target):
        raise ValueError('frustum has no controlled library calls')

    def run(self, entry):
        if entry != ENTRY:
            raise ValueError('frustum run requires owned constructor entry')
        pc, helper_return = entry, None
        while True:
            start, end = (NORMALIZE, NORMALIZE_END) if helper_return is not None else (ENTRY, END)
            if not start <= pc < end:
                raise ValueError('frustum transfer outside owned body')
            instruction = self.fetch(pc)
            op = instruction >> 26
            returning = op == 0 and instruction & 63 == 8
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if op != 3 and not returning:
                    self.branch_outcomes.append([pc, target is not None, annul])
                if not annul:
                    if pc + 4 >= end:
                        raise ValueError('frustum delay outside owned body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('frustum control transfer in delay')
                if returning:
                    stop = helper_return if helper_return is not None else RETURN
                    if instruction != 0x03E00008 or target != stop:
                        raise ValueError('frustum return requires actual JR31 to selected stop')
                    if helper_return is None:
                        return
                    pc, helper_return = helper_return, None
                elif op == 3:
                    if helper_return is not None or target != NORMALIZE:
                        raise ValueError('unreviewed frustum call or nested helper')
                    self.record_call(target)
                    helper_return, pc = self.r[31], target
                else:
                    pc = pc + 8 if target is None else target
            else:
                if target is not None or annul:
                    raise ValueError('unreviewed frustum transfer')
                pc += 4


def make_fixture(original, output, frame_offset, frame, dimensions, seed=1):
    if (isinstance(output, bool) or isinstance(frame_offset, bool) or not isinstance(output, int)
            or not isinstance(frame_offset, int) or not 0 <= output <= WORDS - 42
            or not 0 <= frame_offset <= WORDS - 16 or len(frame) != 16 or len(dimensions) != 4):
        raise ValueError('unreviewed frustum fixture layout')
    trace = FrustumTrace(original)
    for start, end in MEMORY_RANGES:
        for address in range(start, end):
            trace.memory[address] = 0
    for i in range(WORDS):
        trace.single(BUFFER + i * 4, float((i * 7 + seed) % 17 - 8))
    for i, value in enumerate(frame):
        trace.single(BUFFER + (frame_offset + i) * 4, normal_single(value))
    trace.output = BUFFER + output * 4
    trace.r[4], trace.r[5] = trace.output, BUFFER + frame_offset * 4
    for reg, value in enumerate(dimensions, 12):
        trace.f[reg] = normal_single(value)
    trace.r[29], trace.r[31] = 0x80000, RETURN
    initial = [trace.load(BUFFER + i * 4, 4) for i in range(WORDS)]
    trace.run(ENTRY)
    if trace.calls != 5:
        raise ValueError('original constructor did not execute five normalizers')
    return dict(output=output, frame=frame_offset, dimensions=[word(v) for v in dimensions], initial=initial,
                expected=[trace.load(BUFFER + i * 4, 4) for i in range(WORDS)], events=trace.events,
                instruction_count=trace.instruction_count, branch_outcomes=trace.branch_outcomes)


def fixtures(original):
    frames = (
        (1, 0, 0, 90, 0, 1, 0, 91, 0, 0, 1, 92, 3, -2, 5, 93),
        (2, 0, 0, 90, 0, 3, 0, 91, 0, 0, 4, 92, 0, 0, 0, 93),
        (0, 1, 0, 90, 0, 0, 1, 91, 1, 0, 0, 92, -4, 7, 2, 93),
        (1, 2, 3, 90, -2, 1, 4, 91, 3, -1, 2, 92, 4, -3, 1, 93),
        (0, 0, 0, 90, 0, 0, 0, 91, 0, 0, 0, 92, 1, 2, 3, 93),
        (1, 1, 1, 90, 2, 2, 2, 91, 3, 3, 3, 92, 0, 0, 0, 93),
        (1, -0.0, 0, 90, 0, 1, -0.0, 91, -0.0, 0, 1, 92, -0.0, 0, -0.0, 93),
        (-1, 0, 0, 90, 0, 1, 0, 91, 0, 0, -1, 92, 2, 3, 4, 93),
    )
    dimensions = ((1, 4, 2, 3), (0, 4, 2, 3), (-1, 4, 2, 3), (4, 1, 2, 3),
                  (1, 0, 2, 3), (1, 4, 0, 3), (1, 4, 2, 0), (0, 0, 0, 0),
                  (-0.0, -0.0, -0.0, -0.0), (0.5, -2, -1, 0.25))
    cases = [make_fixture(original, 24, 88, frame, dims) for frame in frames for dims in dimensions]
    # Every four-byte shifted overlap relative to the output, including both
    # edges, sees fresh position/axis loads and temporary sixth-vertex stores.
    for offset in range(8, 68):
        for frame in (frames[0], frames[3], frames[6]):
            for dims in (dimensions[0], dimensions[7]):
                cases.append(make_fixture(original, 24, offset, frame, dims))
    state = 0x63FACED1
    for seed in range(96):
        frame = []
        for i in range(16):
            state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
            frame.append(((state >> 16) % 33 - 16) / 4)
        dims = [((seed + i * 3) % 13 - 6) / 2 for i in range(4)]
        for offset in (88, 24, 24 + seed % 42):
            cases.append(make_fixture(original, 24, offset, frame, dims, seed))
    return cases


def input_hash(cases):
    inputs = [{key: c[key] for key in ('output', 'frame', 'dimensions', 'initial')} for c in cases]
    return hashlib.sha256(json.dumps(inputs, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic finite original outputs and actual-normalizer arguments; no game bytes. */',
             'struct FrustumGolden { u32 output,frame,dimensions[4],initial[128],expected[128],events[20]; };',
             'static const struct FrustumGolden frustum_golden[] = {']
    for c in cases:
        arrays = [','.join('0x%08Xu' % w for w in c[k]) for k in ('dimensions', 'initial', 'expected', 'events')]
        lines.append('    {%du,%du,{%s},{%s},{%s},{%s}},' % (c['output'], c['frame'], *arrays))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/geometry_frustum_trace.json')
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
