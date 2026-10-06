"""Finite original triangle/plane/projection/bounds instruction fixtures.

Seven complete selected bodies and the published normalization helper execute
their original instructions. No numerical or callback substitutions occur.
Only synthetic inputs/outputs are exported. Binary32 host arithmetic/SQRT is a
bounded model, excluding EE exceptional/subnormal arithmetic, FCR and cycles.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_resource_base import ResourceBaseTrace
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN, word, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x29C6C0, 0x29C7AC), (0x29C7B0, 0x29C85C),
          (0x29CA28, 0x29CB20), (0x29CB20, 0x29CD50),
          (0x29D048, 0x29D0F0), (0x29E448, 0x29E510),
          (0x29E720, 0x29E7EC))
HELPER = (0x2A3538, 0x2A35C0)
ENTRIES = tuple(a for a, b in RANGES)
BUFFER, END, WORDS = 0x20000, 0x20200, 128
MEMORY_RANGES = ((BUFFER, END), (0x7F000, 0x81000))


class PlaneTrace(ResourceBaseTrace):
    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in (*RANGES, HELPER)):
            raise ValueError('unreviewed plane geometry instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('plane geometry instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    def memory_check(self, address, size):
        if (size not in (4, 8, 16) or address % size
                or not any(a <= address and address + size <= b for a, b in MEMORY_RANGES)
                or any(address + i not in self.memory for i in range(size))):
            raise ValueError('plane memory outside initialized aligned windows')

    def execute(self, instruction, pc):
        op, rs, rt, fn = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31, instruction & 63
        if self.instruction_count >= 3000:
            raise ValueError('plane instruction bound exceeded')
        allowed = instruction == 0
        if op == 0:
            allowed |= fn in (8, 0x2D)
        elif op in (3, 4, 9, 0x1E, 0x1F, 0x37, 0x3F, 0x31, 0x39):
            allowed = True
        elif op == 15:
            allowed = rs == 0
        elif op == 17:
            allowed = rs == 4 or rs == 8 and rt in (0, 1, 2, 3)
            if rs == 16:
                allowed = fn in (0, 1, 2, 3, 4, 6, 7, 0x32, 0x34, 0x36)
                if fn == 3 and self.f[rt] == 0:
                    raise ValueError('plane zero denominator outside finite model')
                if fn == 4 and self.f[rt] < 0:
                    raise ValueError('plane negative square root outside finite model')
        if not allowed:
            raise ValueError('unsupported plane instruction encoding')
        if op in (0x31, 0x39, 17):
            return CameraTrace.execute(self, instruction, pc)
        return super().execute(instruction, pc)

    def library_call(self, target):
        raise ValueError('plane geometry has no controlled calls')

    def run(self, entry, stop=RETURN):
        body = next((r for r in (*RANGES, HELPER) if r[0] == entry), None)
        if body is None:
            raise ValueError('plane invocation requires a complete owned entry')
        pc = entry
        while True:
            if not body[0] <= pc < body[1]:
                raise ValueError('plane control transfer outside owned body')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            op = instruction >> 26
            branch = op == 4 or op == 17 and instruction >> 21 & 31 == 8
            returning = op == 0 and instruction & 63 == 8
            if returning and (instruction != 0x03E00008 or target != stop):
                raise ValueError('plane return requires actual JR31 to selected stop')
            if target is not None or branch and not annul:
                if pc + 4 >= body[1]:
                    raise ValueError('plane delay outside complete body')
                delay = self.fetch(pc + 4)
                if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                    raise ValueError('plane control transfer in delay')
                if returning:
                    return
                if target is None:
                    pc += 8
                elif op == 3:
                    continuation = self.r[31]
                    if target not in (ENTRIES[1], HELPER[0]):
                        raise ValueError('plane unreviewed helper call')
                    self.run(target, continuation)
                    if self.r[31] != continuation:
                        raise ValueError('plane helper corrupted link register')
                    pc = continuation
                else:
                    if not body[0] <= target < body[1]:
                        raise ValueError('plane local transfer outside owned body')
                    pc = target
            else:
                pc += 8 if annul else 4


def fixture(original, routine, offsets, values, seed=1, radius=0):
    t = PlaneTrace(original)
    for a, b in MEMORY_RANGES:
        for p in range(a, b):
            t.memory[p] = 0
    state = seed
    for i in range(WORDS):
        state = (state * 1664525 + 1013904223) & 0xFFFFFFFF
        t.single(BUFFER + i * 4, ((state % 65) - 32) / 8)
    for offset, data in values:
        for i, v in enumerate(data):
            t.single(BUFFER + (offset + i) * 4, v)
    initial = [t.load(BUFFER + i * 4, 4) for i in range(WORDS)]
    t.r[4:8] = [0 if i < 0 else BUFFER + i * 4 for i in offsets]
    t.r[29], t.r[31], t.f[12] = 0x80000, RETURN, radius
    t.run(ENTRIES[routine])
    result = word(t.f[0]) if routine == 4 else t.r[2] & 0xFFFFFFFF if routine in (2, 3, 6) else 0
    return dict(routine=routine, offsets=offsets, radius=word(radius), initial=initial,
                expected=[t.load(BUFFER + i * 4, 4) for i in range(WORDS)],
                result=result, instruction_count=t.instruction_count)


def fixtures(original):
    cases = []
    triangles = (((0, 0, 0), (4, 0, 0), (0, 4, 0)),
                 ((1, -2, 3), (3, 1, 0), (-2, 3, 4)),
                 ((1, 2, 3), (1, 2, 3), (1, 2, 3)),
                 ((0, 0, 0), (1, 1, 1), (2, 2, 2)),
                 ((0, 0, 2), (0, 4, 2), (4, 0, 2)))
    for routine in (0, 1):
        for triangle in triangles:
            for out in (64, 8, 9, 10, 11, 16, 17, 18, 24, 25, 26):
                cases.append(fixture(original, routine, (out, 8, 16, 24), list(zip((8, 16, 24), triangle))))
    for radius in (-2, -1, 0, 0.5, 1, 2, 4):
        for query in ((-2, 0, 0), (0, 0, 0), (1, 0, 0), (2, 1, 0), (4, 0, 0), (0, 1, 0), (3, 4, 0)):
            for second in ((4, 0, 0), (0, 0, 0), (1, 2, 3)):
                cases.append(fixture(original, 2, (8, 16, 24, -1), ((8, query), (16, (0, 0, 0)), (24, second)), radius=radius))
    for triangle in triangles:
        for query in ((1, 1, 0), (0, 0, 0), (2, 2, 0), (4, 0, 0), (5, 1, 0), (-1, 1, 0), (1, 1, 2)):
            for point in (64, 8, 9, 11, 13, 16):
                cases.append(fixture(original, 3, (8, point, -1, -1), ((8, sum(triangle, ())), (point, query))))
    for seed in range(1, 9):
        for origin, direction, query in ((8, 16, 24), (8, 8, 24), (8, 9, 10), (10, 9, 8), (8, 12, 11)):
            for parameter in (-1, 64, origin, direction + 1, query + 2):
                cases.append(fixture(original, 4, (origin, direction, query, parameter),
                    ((origin, (seed / 2, -2, 3)), (direction, (seed / 4, -1, 0.5)), (query, (-seed / 2, 1, 2))), seed))
    for lower, upper in (((-1, -2, -3), (1, 2, 3)), ((0, 0, 0), (0, 0, 0)), ((3, 2, 1), (-3, -2, -1))):
        for point in ((0, 0, 0), (-4, -5, -6), (4, 5, 6), (-1, 2, 3), (2, -3, 4)):
            for point_offset in (64, 8, 9, 10, 11, 12, 13, 14):
                cases.append(fixture(original, 5, (8, point_offset, -1, -1), ((8, (*lower, *upper)), (point_offset, point))))
    for point in ((0, 0, 0), (1, 2, 3), (-1, -2, -3), (2, 2, 3), (2, 3, 3), (2, 3, 4), (0, 0, 4), (-2, -3, -4)):
        for radius in (0, 1, -1, 1.5, 2, 3):
            for sphere in (64, 8, 9, 10, 11, 12, 13):
                cases.append(fixture(original, 6, (8, sphere, -1, -1), ((8, (-1, -2, -3, 1, 2, 3)), (sphere, (*point, radius)))))
    return cases


def input_hash(cases):
    keys = ('routine', 'offsets', 'radius', 'initial')
    return hashlib.sha256(json.dumps([{k: c[k] for k in keys} for c in cases], sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    ar = lambda a: '{' + ','.join('0x%08Xu' % v for v in a) + '}'
    lines = ['/* Synthetic finite outputs only; no original code/data arrays. */',
             'struct PlaneGeometryGolden { u32 routine; s32 offsets[4]; u32 radius,result,initial[128],expected[128]; };',
             'static const struct PlaneGeometryGolden plane_geometry_golden[] = {']
    for c in cases:
        offsets = '{' + ','.join(str(v) for v in c['offsets']) + '}'
        lines.append(' {%du,%s,0x%08Xu,0x%08Xu,%s,%s},' % (c['routine'], offsets, c['radius'], c['result'], ar(c['initial']), ar(c['expected'])))
    return '\n'.join(lines + ['};', ''])


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    p.add_argument('--output', type=Path, default=ROOT / 'build/plane_geometry/trace.json')
    p.add_argument('--golden-header', type=Path)
    args = p.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(dict(limitation=__doc__, input_sha256=input_hash(cases), cases=cases), indent=2) + '\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases))
    print('%d fixtures; %d original instructions; max %d; input %s' % (len(cases), sum(c['instruction_count'] for c in cases), max(c['instruction_count'] for c in cases), input_hash(cases)))


if __name__ == '__main__':
    main()
