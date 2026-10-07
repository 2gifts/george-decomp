"""Bounded original provider/road queries with real bounds and VU transform.

All four selected bodies and both complete original helper bodies execute.
Memory reads require actual initialized bytes, including local scratch: a
later zero-vertex record may reuse Y written by an earlier record. First-zero
uninitialized reads and writes overlapping original saved stack are observer
limits, not fabricated production guards or discovered asset capacities.
Finite normal/zero binary32 products/adds reuse the reviewed VU decoder; no
extended VU ACC precision, FCR, exceptional/subnormal, timing or stack-layout
equivalence claim. Exported fixtures contain synthetic data only.
"""
import argparse
import hashlib
import json
from pathlib import Path
import random
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer, word
from trace_record_collision import RecordTrace

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x1CC960, 0x1CCA28), (0x1CCA28, 0x1CCAA4),
          (0x1CF340, 0x1CF584), (0x1CF588, 0x1CF7A0))
HELPERS = ((0x2A00A0, 0x2A0138), (0x2A1C60, 0x2A1CDC))
ENTRIES = tuple(a for a, _ in RANGES)
BUFFER, END = 0x30000, 0x30A00
MANAGER, PROVIDERS, HEADERS, ROADS, RECORDS, BLOBS, POINT = (
    BUFFER, BUFFER + 0x40, BUFFER + 0x100, BUFFER + 0x200,
    BUFFER + 0x400, BUFFER + 0x700, BUFFER + 0x9E0)
GLOBAL = 0x3F8C28
WORDS = (END - BUFFER) // 4
POINTER_CELLS = tuple([MANAGER + 8 + 4 * i for i in range(4)] +
                      [PROVIDERS + 0x20 * i + 0x10 for i in range(4)] +
                      [HEADERS + 0x40 * i + 0x2C for i in range(4)] +
                      [ROADS + 0x50 * i + p for i in range(4) for p in (0x3C, 0x40)])
MEMORY_RANGES = ((BUFFER, END), (GLOBAL, GLOBAL + 4), (0x7E000, 0x81000))


def signed(value):
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


class SpatialTrace(RecordTrace):
    def __init__(self, original):
        super().__init__(original)
        self.scratch_frames = []
        self.invocations = [0] * 6
        self.visited = set()

    def memory_check(self, address, size):
        if (size not in (1, 2, 4, 8, 16) or address % size or
                not any(a <= address and address + size <= b for a, b in MEMORY_RANGES)):
            raise ValueError('spatial memory outside aligned observer windows')

    def load(self, address, size):
        self.memory_check(address, size)
        if any(address + i not in self.memory for i in range(size)):
            raise ValueError('uninitialized spatial memory read, including first scratch Y')
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        return Trace.save(self, address, value, size)

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in (*RANGES, *HELPERS)):
            raise ValueError('unreviewed spatial instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('spatial instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    def execute(self, instruction, pc):
        self.visited.add(pc)
        op, rs, rt, rd, fn = (instruction >> 26, instruction >> 21 & 31,
                              instruction >> 16 & 31, instruction >> 11 & 31, instruction & 63)
        sh = instruction >> 6 & 31
        if op == 0 and fn in (0x1A, 0x2A):
            if sh or fn == 0x1A and rd:
                raise ValueError('reserved spatial signed divide/compare encoding')
            a, b = signed(self.r[rs]), signed(self.r[rt])
            if fn == 0x2A:
                self.r[rd] = int(a < b)
            else:
                if b == 0 or a == -0x80000000 and b == -1:
                    raise ValueError('spatial signed division outside observer contract')
                quotient = abs(a) // abs(b)
                if (a < 0) != (b < 0):
                    quotient = -quotient
                self.lo, self.hi = quotient & 0xFFFFFFFF, (a - quotient * b) & 0xFFFFFFFF
            self.instruction_count += 1
            self.r[0] = 0
            return None, False
        if op == 6:
            if rt:
                raise ValueError('reserved spatial BLEZ operand')
            immediate = signed((instruction & 65535) | (0xFFFF0000 if instruction & 32768 else 0))
            self.instruction_count += 1
            return (pc + 4 + immediate * 4, False) if signed(self.r[rs]) <= 0 else (None, False)
        if op == 0 and fn == 0x0D:
            raise ValueError('actual spatial division BREAK trap reached')
        return super().execute(instruction, pc)

    def library_call(self, target):
        raise ValueError('spatial helpers require genuine complete original execution')

    def run(self, entry, stop=RETURN):
        intervals = (*RANGES, *HELPERS)
        body = next((r for r in intervals if r[0] == entry), None)
        if body is None:
            raise ValueError('spatial invocation requires complete approved entry')
        self.invocations[tuple(a for a, _ in intervals).index(entry)] += 1
        scratch = entry in (ENTRIES[2], ENTRIES[3])
        if scratch:
            base = (self.r[29] - (0xD0 if entry == ENTRIES[2] else 0xB0)) & 0xFFFFFFFF
            self.scratch_frames.append((base, base + 0x40))
        pc = entry
        while True:
            if not body[0] <= pc < body[1] or self.instruction_count >= 3000:
                raise ValueError('spatial transfer/body instruction bound')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            if is_control_transfer(instruction):
                if not annul:
                    if pc + 4 >= body[1]:
                        raise ValueError('spatial delay outside complete body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('spatial control transfer in delay')
                if instruction >> 26 == 0 and instruction & 63 == 8:
                    if instruction != 0x03E00008 or target != stop:
                        raise ValueError('spatial return requires actual JR31 to stop')
                    if scratch:
                        self.scratch_frames.pop()
                    return
                if instruction >> 26 == 3:
                    continuation = self.r[31]
                    if target not in tuple(a for a, _ in intervals):
                        raise ValueError('unreviewed spatial callee')
                    if target == 0x2A1C60:
                        if not self.scratch_frames:
                            raise ValueError('point transform without selected scratch frame')
                        first, limit = self.scratch_frames[-1]
                        output = self.r[6] & 0xFFFFFFFF
                        if not first <= output < output + 12 <= limit:
                            raise ValueError('spatial saved-stack overlap outside observer limit')
                    self.run(target, continuation)
                    if self.r[31] != continuation:
                        raise ValueError('spatial callee link register mismatch')
                    pc = continuation
                elif annul:
                    pc += 8
                else:
                    pc = target if target is not None else pc + 8
            else:
                pc += 4


def prepare(original, routine=0, point=(0, 0, 0), provider_count=1,
            disabled=0, missing=0, invalid=0, key=0x11110000,
            record_count=1, polygon_count=4, shape=0, later_zero=False,
            point_offset=POINT - BUFFER):
    t = SpatialTrace(original)
    # No initial stack bytes are injected: prologues and transforms must write
    # every subsequently read stack value themselves.
    for address in range(BUFFER, END):
        t.memory[address] = 0
    for address in range(GLOBAL, GLOBAL + 4):
        t.memory[address] = 0
    t.save(GLOBAL, MANAGER, 4)
    t.save(MANAGER, provider_count, 1)
    shape_points = [
        [(-1, 0, -1), (-1, 0, 1), (1, 0, 1), (1, 0, -1), (-1, 0, -1)],
        [(0, 0, 0), (0.05, 0, 0.05), (0.075, 0, 0.075), (0, 0, 0), (0, 0, 0)],
        [(-1, 0, -1), (1, 0, -1), (1, 0, 1), (-1, 0, 1), (-1, 0, -1)],
        [(-1, 1, -1), (-1, 1, 1), (1, 1, 1), (1.025, 1, 0.975), (1, 1, -1)],
    ][shape]
    for i in range(4):
        provider, header, road = PROVIDERS + i * 0x20, HEADERS + i * 0x40, ROADS + i * 0x50
        record_base, blob = RECORDS + i * 0xC0, BLOBS + i * 0xB0
        t.save(MANAGER + 8 + i * 4, provider, 4)
        t.save(provider + 2, 0 if disabled & (1 << i) else 4, 1)
        t.save(provider + 0x10, header, 4)
        t.save(header, 0x11110000 + i * 0x10000, 4)
        for p, v in zip((4, 8, 12, 16, 20, 24), (-100, -100, -100, 100, 100, 100)):
            t.save(header + p, word(v), 4)
        t.save(header + 0x2C, 0 if missing & (1 << i) else road, 4)
        t.save(road, 0 if invalid & (1 << i) else 1, 4)
        t.save(road + 4, record_count, 2)
        t.save(road + 0x3C, record_base, 4)
        t.save(road + 0x40, blob, 4)
        for j in range(2):
            record = record_base + j * 0x60
            matrix = [1,0,0,0, 0,1,0,0, 0,0,1,0, j * 4, i * 8, 0,1]
            for p, v in enumerate(matrix):
                t.save(record + p * 4, word(v), 4)
            t.save(record + 0x40, 0xABC00000 + i * 2 + j, 4)
            t.save(record + 0x44, j * 0x50, 4)
            polygon = blob + j * 0x50
            t.save(polygon + 4, 0 if later_zero and j == 1 else polygon_count, 1)
            for k, vertex in enumerate(shape_points):
                for p, value in enumerate(vertex):
                    t.save(polygon + 8 + k * 12 + p * 4, word(value), 4)
    for p, value in enumerate(point):
        t.save(BUFFER + point_offset + p * 4, word(value), 4)
    t.r[4] = key if routine == 1 else BUFFER + point_offset
    t.r[5] = key
    t.r[29], t.r[31] = 0x80000, RETURN
    return t


def fixture(original, **kwargs):
    routine = kwargs.get('routine', 0)
    t = prepare(original, **kwargs)
    initial = [t.load(BUFFER + i * 4, 4) for i in range(WORDS)]
    t.run(ENTRIES[routine])
    return dict(routine=routine, key=kwargs.get('key', 0x11110000),
                point_offset=kwargs.get('point_offset', POINT - BUFFER),
                initial=initial, expected=[t.load(BUFFER + i * 4, 4) for i in range(WORDS)],
                result=t.r[2] & 0xFFFFFFFF, invocations=t.invocations,
                instruction_count=t.instruction_count)


def fixtures(original):
    cases = []
    for routine in range(4):
        for count in (0, 1, 2, 4):
            for flags in (0, 1, 3, 15):
                for point in ((0,0,0), (101,0,0), (0,-100,0), (0,0,100), (100,100,100)):
                    cases.append(fixture(original, routine=routine, provider_count=count,
                                         disabled=flags, point=point))
        for mask in (0, 1, 15):
            for key in (0x11110000, 0x11110001, 0x11120000, 0x91120000, 0xFFFFFFFF):
                cases.append(fixture(original, routine=routine, provider_count=4,
                                     missing=mask, key=key))
                cases.append(fixture(original, routine=routine, provider_count=4,
                                     invalid=mask, key=key))
    points = ((0,0,0), (-1,0,-1), (1,0,1), (-1.0001,0,0), (1.0001,0,0),
              (0,2,0), (0,-2,0), (0,1.9999,0), (0,-1.9999,0), (4,0,0),
              (0,3,0), (0,2.9999,0), (0,1,0))
    for routine in (2,3):
        for polygon_count in (1,2,3,4,5):
            for shape in range(4):
                for point in points:
                    cases.append(fixture(original, routine=routine, record_count=2,
                                         polygon_count=polygon_count, shape=shape, point=point))
        for point in ((3,0,0), (3,2,0), (3,-2,0), (3,1.9999,0), (3,-1.9999,0)):
            if routine == 2:
                cases.append(fixture(original, routine=2, record_count=2, later_zero=True, point=point))
        if routine == 2:
            for point in ((3,1,0),(3,3,0),(3,-1,0),(3,2.9999,0),(3,-0.9999,0)):
                cases.append(fixture(original,routine=2,record_count=2,later_zero=True,
                                     shape=3,point=point))
        for point in ((0,0,0), (4,0,0), (0,1,0), (1,0,-1)):
            cases.append(fixture(original, routine=routine, point=point,
                                 point_offset=RECORDS - BUFFER + 0x30))
    generator = random.Random(0x1CF340)
    for i in range(96):
        cases.append(fixture(original, routine=2 + i % 2, provider_count=generator.randrange(1,5),
                             record_count=generator.randrange(1,3), polygon_count=generator.randrange(1,6),
                             shape=generator.randrange(4), point=tuple(generator.randrange(-48,49)/8 for _ in range(3)),
                             key=(0x11110000 + generator.randrange(4)*0x10000) | generator.randrange(2)))
    return cases


def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:v for k,v in c.items() if k not in
                                       ('expected','result','invocations','instruction_count')}
                                      for c in cases], sort_keys=True).encode()).hexdigest()


def golden_header(cases):
    lines = ['/* Synthetic original-derived fixture data only; no original instructions/assets. */',
             'struct SpatialGolden { unsigned routine,key,point_offset,result; u32 initial[640],expected[640]; };',
             'static const struct SpatialGolden spatial_golden[] = {']
    for c in cases:
        lines.append(' {%du,0x%08Xu,%du,0x%08Xu,{%s},{%s}},' %
                     (c['routine'], c['key'], c['point_offset'], c['result'],
                      ','.join('0x%08Xu' % v for v in c['initial']),
                      ','.join('0x%08Xu' % v for v in c['expected'])))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT/'build/spatial_queries/trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(ROOT/'orig/SLUS_216.68')
    cases = fixtures(original)
    header = golden_header(cases)
    report = dict(fixtures=cases, fixture_count=len(cases),
                  executed_original_instructions=sum(c['instruction_count'] for c in cases),
                  maximum_instructions=max(c['instruction_count'] for c in cases),
                  input_sha256=input_hash(cases), header_sha256_lf=hashlib.sha256(header.encode()).hexdigest(),
                  limitation=__doc__)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2)+'\n')
    if args.golden_header:
        args.golden_header.write_text(header)
    print('SPATIAL FIXTURES',len(cases),report['executed_original_instructions'],report['maximum_instructions'])


if __name__ == '__main__':
    main()
