"""Bounded collision-node originals with genuine published helper execution.

Only finite normal/zero scalar/VU arithmetic is observed. Two unrecovered
engine queries and virtual callbacks have explicit synthetic observation
contracts; their numerical implementations are not claimed. Stack bytes are
unknown until actually stored: an unwritten query-mode output is rejected.
No original instructions/assets are exported in the synthetic fixtures.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, rounded, scalar, word, is_control_transfer
from trace_record_collision import RecordTrace
from trace_spatial_queries import signed
from trace_segment_distance import SegmentTrace, CALLS as SOFT_CALLS, MASK64

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x1E2EC8, 0x1E4ADC), (0x1E52E8, 0x1E5308))
HELPERS = ((0x2AD700, 0x2AD748), (0x2A3538, 0x2A35C0),
           (0x29C230, 0x29C29C), (0x29C300, 0x29C41C),
           (0x1CAFE0, 0x1CB074), (0x1CCA28, 0x1CCAA4),
           (0x1CF340, 0x1CF584), (0x1CC960, 0x1CCA28),
           (0x2A00A0, 0x2A0138), (0x2A1C60, 0x2A1CDC),
           (0x29D5D0, 0x29DD5C))
ENTRIES = tuple(a for a, _ in RANGES)
INTERVALS = (*RANGES, *HELPERS)
BUFFER, END = 0x30000, 0x32800
STATE, POINT, DIRECTION, POOL_DATA = BUFFER, BUFFER + 0x800, BUFFER + 0x820, BUFFER + 0x900
MANAGER, PROVIDER, HEADER, ROAD, RECORDS, VERTICES = (BUFFER + p for p in (0xC00, 0xC40, 0xC80, 0xD00, 0xE00, 0x1000))
OWNER, OBJECT, OTHER, TABLE, POSITION, REFERENCE, HOST = (BUFFER + p for p in (0x1800, 0x1A00, 0x1C00, 0x1E00, 0x1F00, 0x2000, 0x2100))
POOL, GLOBAL, BROAD_GLOBAL, BASIS = 0x45C6A0, 0x3F8C28, 0x3F8B58, 0x437288
POSITION_CALL, RADIUS_CALL = 0x60000000, 0x60000004
WORDS = (END - BUFFER) // 4
MEMORY_RANGES = ((BUFFER, END), (POOL, POOL + 28), (GLOBAL, GLOBAL + 4),
                 (BROAD_GLOBAL, BROAD_GLOBAL + 4), (BASIS, BASIS + 12), (0x7C000, 0x81000))
POINTER_CELLS = (STATE, STATE + 0x64, STATE + 0x110, MANAGER + 8,
                 PROVIDER + 0x10, HEADER + 0x2C, ROAD + 0x38, ROAD + 0x48,
                 ROAD + 0x3C, ROAD + 0x40, OWNER + 4, OBJECT + 4, OTHER + 4,
                 HOST + 4, REFERENCE + 0x20,
                 *[STATE + 0x6C + i * 4 for i in range(256)],
                 *[POOL_DATA + i * 32 + p for i in range(16) for p in (0x14, 0x18, 0x1C)])


class CollisionTrace(RecordTrace):
    def __init__(self, original, objects=0, mutation=0, query=0):
        super().__init__(original)
        self.objects, self.mutation, self.query = objects, mutation, query
        self.position_count = self.radius_count = self.query_count = 0
        self.frames = []
        self.visited = set()
        self.invocations = [0] * len(INTERVALS)
        self.f_bits = [0] * 32
        self.object_output = None
        self.segment_decoder = CollisionSegmentTrace.__new__(CollisionSegmentTrace)
        self.segment_decoder.__dict__ = self.__dict__

    def memory_check(self, address, size):
        if size not in (1, 2, 4, 8, 16) or address % size or not any(
                a <= address and address + size <= b for a, b in MEMORY_RANGES):
            raise ValueError('collision memory outside aligned observer windows')

    def load(self, address, size):
        self.memory_check(address, size)
        if any(address + i not in self.memory for i in range(size)):
            raise ValueError('uninitialized collision read, including unwritten query mode')
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        return Trace.save(self, address, value, size)

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in INTERVALS):
            raise ValueError('unreviewed collision instruction')
        offset = pc - 0xFF000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('collision instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    def execute(self, instruction, pc):
        self.visited.add(pc)
        if self.instruction_count >= 100000:
            raise ValueError('collision whole observer instruction bound')
        op, rs, rt, rd, fn = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31, instruction >> 11 & 31, instruction & 63
        shift = instruction >> 6 & 31
        imm = instruction & 65535
        simm = imm - 65536 if imm & 32768 else imm
        custom, target = True, None
        if op == 0 and fn == 0x1A:
            if shift or rd: raise ValueError('reserved collision signed division')
            a, b = signed(self.r[rs]), signed(self.r[rt])
            if b == 0 or a == -0x80000000 and b == -1:
                raise ValueError('collision signed division outside observer contract')
            quotient = abs(a) // abs(b)
            if (a < 0) != (b < 0): quotient = -quotient
            self.lo, self.hi = quotient & 0xFFFFFFFF, (a - quotient * b) & 0xFFFFFFFF
        elif op == 0 and fn == 0x0D:
            raise ValueError('actual collision/spatial BREAK trap reached')
        elif op == 0 and fn in (3, 0x0A, 0x25, 0x2A, 0x2D):
            if fn == 3:
                if rs: raise ValueError('reserved collision SRA')
                self.r[rd] = (signed(self.r[rt]) >> shift) & 0xFFFFFFFF
            else:
                if shift: raise ValueError('reserved collision SPECIAL shift')
                if fn == 0x0A:
                    if self.r[rt] == 0: self.r[rd] = self.r[rs]
                elif fn == 0x25: self.r[rd] = (self.r[rs] | self.r[rt]) & MASK64
                elif fn == 0x2A: self.r[rd] = int(signed(self.r[rs]) < signed(self.r[rt]))
                else: self.r[rd] = (self.r[rs] + self.r[rt]) & MASK64
        elif op in (1, 6, 7):
            if op == 1 and rt not in (0, 1) or op in (6, 7) and rt:
                raise ValueError('unsupported collision integer branch')
            value = signed(self.r[rs])
            taken = (value < 0 if rt == 0 else value >= 0) if op == 1 else value <= 0 if op == 6 else value > 0
            target = (pc + 4 + simm * 4) & 0xFFFFFFFF if taken else None
        elif op in (10, 11):
            self.r[rt] = int(signed(self.r[rs]) < simm) if op == 10 else int((self.r[rs] & 0xFFFFFFFF) < (simm & 0xFFFFFFFF))
        elif op == 0x20:
            value = self.load((self.r[rs] + simm) & 0xFFFFFFFF, 1)
            self.r[rt] = (value - 256 if value & 128 else value) & 0xFFFFFFFF
        elif op == 17 and rs == 4:
            if instruction & 0x7FF: raise ValueError('reserved collision MTC1')
            self.f_bits[rd] = self.r[rt] & 0xFFFFFFFF
            self.f[rd] = scalar(self.f_bits[rd])
        elif op == 17 and rs == 20 and fn == 32:
            if rt: raise ValueError('reserved collision CVT.S.W')
            self.f[shift] = rounded(float(signed(self.f_bits[rd])))
            self.f_bits[shift] = word(self.f[shift])
        else:
            custom = False
        if custom:
            self.instruction_count += 1
            self.r[0] = 0
            return target, False
        count = self.instruction_count
        self.instruction_count = 0
        try:
            if self.segment_active:
                result = self.segment_decoder.execute(instruction, pc)
            else:
                result = RecordTrace.execute(self, instruction, pc)
        finally:
            self.instruction_count += count
        if op == 0x31:
            self.f_bits[rt] = word(self.f[rt])
        elif op == 17 and rs == 16 and fn not in (0x32, 0x34, 0x36):
            self.f_bits[shift] = word(self.f[shift])
        return result

    def vector_words(self, pointer):
        return [self.load(pointer + i * 4, 4) for i in range(3)]

    def library_call(self, target):
        if target in SOFT_CALLS:
            return RecordTrace.library_call(self, target)
        if target == 0x1C1540:
            if self.r[4] != HOST or self.r[8] != 32 or word(self.f[12]) != word(1.0):
                raise ValueError('collision broadphase argument contract')
            self.events.extend((10, *self.vector_words(self.r[5]), *self.vector_words(self.r[6]), self.r[8], word(self.f[12])))
            self.object_output = self.r[7]
            for i in range(self.objects): self.save(self.r[7] + i * 4, OBJECT if i == 0 else OTHER, 4)
            if self.mutation == 1: self.single(POINT, 0.125)
            self.r[2] = self.objects
        elif target in (POSITION_CALL, RADIUS_CALL):
            receiver = self.r[4] & 0xFFFFFFFF
            if receiver not in (OWNER + 4, OBJECT + 4, OTHER + 4):
                raise ValueError('collision virtual receiver/full signed adjustment')
            if target == POSITION_CALL:
                self.position_count += 1
                self.events.extend((11, receiver))
                if self.mutation == 2 and self.position_count == 2: self.single(POSITION + 4, 0.0625)
                if self.mutation == 2 and self.position_count == 3: self.single(POSITION + 8, 0.125)
                self.r[2] = POSITION
            else:
                self.radius_count += 1
                value = 0.125 if receiver != OWNER + 4 else 0.25
                self.events.extend((12, receiver, word(value)))
                if self.mutation == 3 and self.radius_count == 1: self.save(self.object_output, OTHER, 4)
                if self.mutation == 7 and receiver == OWNER + 4: self.save(STATE + 0x64, OTHER, 4)
                self.f[0] = value
        elif target == 0x1CEA80:
            self.query_count += 1
            self.events.extend((13, *self.vector_words(self.r[4]), word(self.f[12])))
            if self.query:
                self.save(self.r[5], ROAD, 4)
                self.save(self.r[6], 0, 1)
                self.save(self.r[7], self.query - 1, 4)
                self.r[2] = RECORDS + (0x34 if self.mutation == 5 or self.mutation == 8 and self.query_count > 1 else 0)
                if self.mutation == 4: self.save(STATE + 0x42, 5, 1)
            else:
                self.r[2] = 0
        else:
            raise ValueError('unknown collision controlled call')

    def run(self, entry, stop=RETURN):
        body = next((r for r in INTERVALS if r[0] == entry), None)
        if body is None or len(self.frames) >= 40:
            raise ValueError('collision invocation requires whole approved entry/bounded depth')
        index = tuple(a for a, _ in INTERVALS).index(entry)
        self.invocations[index] += 1
        self.frames.append(entry)
        segment = entry == 0x29D5D0
        parent_segment = self.segment_active
        self.segment_active = segment
        pc, count, start_count = entry, 0, self.instruction_count
        try:
            while True:
                count += 1
                if not body[0] <= pc < body[1] or count >= 12000 or segment and self.instruction_count - start_count >= 700:
                    raise ValueError('collision local transfer/instruction bound')
                instruction = self.fetch(pc)
                target, annul = self.execute(instruction, pc)
                if is_control_transfer(instruction):
                    if not annul:
                        if pc + 4 >= body[1]: raise ValueError('collision delay outside body')
                        delay = self.fetch(pc + 4)
                        if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                            raise ValueError('collision control transfer in delay')
                    op, fn = instruction >> 26, instruction & 63
                    if op == 0 and fn == 8:
                        if instruction != 0x03E00008 or target != stop:
                            raise ValueError('collision requires actual JR31 return')
                        return
                    if op == 3 or op == 0 and fn == 9:
                        continuation = self.r[31]
                        if target in tuple(a for a, _ in INTERVALS): self.run(target, continuation)
                        else: self.library_call(target)
                        self.segment_active = segment
                        if self.r[31] != continuation: raise ValueError('collision callee corrupted RA')
                        pc = continuation
                    else:
                        if target is not None and not body[0] <= target < body[1]:
                            raise ValueError('collision branch outside whole body')
                        pc = target if target is not None else pc + 8
                else: pc += 4
        finally:
            self.frames.pop()
            self.segment_active = parent_segment


class CollisionSegmentTrace(SegmentTrace):
    # Reuse the exact reviewed arithmetic decoder, with this observer's
    # genuinely initialized arena and caller-stack memory windows.
    memory_check = CollisionTrace.memory_check
    load = CollisionTrace.load
    save = CollisionTrace.save


def fixture(original, routine=0, control=0, depth=0, scalar0=0.0, scalar1=0.0,
            initial_count=0, road=True, slot=0, vertex=1, flag=0,
            orientation=0, records=1, objects=0, mutation=0, query=0,
            point=(0, 0, 0), target=(4, 0, 0), position=(0.75, 0, 0),
            point_pointer=POINT, object_type=1, reference=False, stop=255,
            polygon=False, query_upper=0, initial_key=0, run=True):
    t = CollisionTrace(original, objects, mutation, query)
    for p in range(BUFFER, END): t.memory[p] = 0
    for a, b in ((POOL, POOL + 28), (GLOBAL, GLOBAL + 4), (BROAD_GLOBAL, BROAD_GLOBAL + 4), (BASIS, BASIS + 12)):
        for p in range(a, b): t.memory[p] = 0
    t.save(GLOBAL, MANAGER, 4); t.save(BROAD_GLOBAL, HOST, 4)
    for i, v in enumerate((0, 1, 0)): t.single(BASIS + i * 4, v)
    t.save(MANAGER, 1 if road else 0, 1); t.save(MANAGER + 8, PROVIDER, 4)
    t.save(PROVIDER + 2, 4, 1); t.save(PROVIDER + 0x10, HEADER, 4)
    t.save(HEADER + 0x2C, ROAD, 4); t.save(ROAD, 1, 4)
    t.save(ROAD + 0x38, RECORDS, 4); t.save(ROAD + 0x48, VERTICES, 4)
    t.save(ROAD + 4, int(polygon), 2)
    if polygon:
        t.save(ROAD + 0x3C, BUFFER + 0x1400, 4)
        t.save(ROAD + 0x40, BUFFER + 0x1500, 4)
        for i, v in enumerate((-100, -100, -100, 100, 100, 100)): t.single(HEADER + 4 + i * 4, v)
        for i, v in enumerate((1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)): t.single(BUFFER + 0x1400 + i * 4, v)
        t.save(BUFFER + 0x1504, 4, 1)
        for i, values in enumerate(((-100, 0, -100), (100, 0, -100), (100, 0, 100), (-100, 0, 100))):
            for j, v in enumerate(values): t.single(BUFFER + 0x1508 + i * 12 + j * 4, v)
    for i in range(3):
        p = RECORDS + i * 0x34
        t.save(p, i, 2); t.save(p + 2, query_upper, 2); t.save(p + 5, records, 1)
        for off, base in ((8, 0), (10, 0), (12, 8), (14, 8), (0x12, 16)): t.save(p + off, base, 2)
    for i in range(8):
        for base, values in ((0, (3 + i, 0, -1)), (8, (3 + i, 0, 1)), (16, (0, 0, 1))):
            for j, v in enumerate(values): t.single(VERTICES + (base + i) * 12 + j * 4, v)
    t.save(STATE, HOST, 4); t.save(HOST + 4, OTHER, 4)
    t.save(STATE + 0x64, OWNER, 4); t.save(STATE + 0x42, initial_count, 1)
    t.save(STATE + 0x3C, 0, 1); t.save(STATE + 0x41, stop, 1)
    t.save(STATE + 0x14, initial_key, 4)
    for i in range(4): t.save(STATE + 0x18 + i * 8, orientation, 1)
    t.save(STATE + 0x110, REFERENCE if reference else 0, 4)
    t.save(REFERENCE + 4, 3, 1)
    for i, v in enumerate((10, 0, 0)): t.single(REFERENCE + 0x20 + i * 4, v)
    for obj in (OWNER, OBJECT, OTHER):
        t.save(obj + 4, TABLE, 4); t.save(obj + 0xC, object_type, 1)
    t.save(TABLE + 0x28, 4, 2); t.save(TABLE + 0x2C, POSITION_CALL, 4)
    t.save(TABLE + 0x40, 4, 2); t.save(TABLE + 0x44, RADIUS_CALL, 4)
    for ptr, values in ((point_pointer, point), (DIRECTION, (1, 0, 0)), (STATE + 0x114, target), (POSITION, position)):
        for i, v in enumerate(values): t.single(ptr + i * 4, v)
    t.save(POOL, 16, 4); t.save(POOL + 4, 32, 4); t.save(POOL + 8, POOL_DATA, 4)
    t.save(POOL + 0x14, 16, 4)
    for i in range(16): t.save(POOL_DATA + i * 32, i + 1, 2)
    initial = [t.load(BUFFER + i * 4, 4) for i in range(WORDS)]
    initial_pool = [t.load(POOL + i * 4, 4) for i in range(7)]
    args = (STATE, point_pointer, DIRECTION, depth, slot & 0xFFFFFFFF, vertex, flag & 0xFFFFFFFF, control & 0xFFFFFFFF)
    t.r[4:12] = args
    if routine == 1: t.r[5] = POOL_DATA
    t.f[12], t.f[13] = scalar0, scalar1
    t.r[29], t.r[31] = 0x80000, RETURN
    if not run: return t
    t.run(ENTRIES[routine])
    return dict(routine=routine, args=list(args), scalar=[word(scalar0), word(scalar1)],
                objects=objects, mutation=mutation, query=query,
                initial=initial, expected=[t.load(BUFFER + i * 4, 4) for i in range(WORDS)],
                initial_pool=initial_pool, pool=[t.load(POOL + i * 4, 4) for i in range(7)],
                result=t.r[2] & 0xFFFFFFFF if routine == 0 else 0,
                events=t.events, invocations=t.invocations, instruction_count=t.instruction_count,
                visited=sorted(t.visited))


def fixtures(original):
    cases = []
    for count in (0, 1, 4, 5, 6, 42, 128, 255):
        for control in (1, 256, -256, -1, 0x7FFFFFFF):
            cases.append(fixture(original, initial_count=count, control=control, scalar0=3.5, scalar1=-2.0))
    for scalar0 in (100, 101, 1000):
        for depth in (0, 3, 4, 0xFFFFFFFF): cases.append(fixture(original, scalar0=scalar0, depth=depth))
    for depth in (4, 5, 0xFFFFFFFF): cases.append(fixture(original, depth=depth))
    for road in (True, False):
        for slot in (0, 256): cases.append(fixture(original, road=road, control=1, slot=slot))
    cases.append(fixture(original, control=1, point_pointer=POOL_DATA + 8, point=(7, 8, 9), scalar1=-3.0))
    for count in (0, 1, 4, 5, 42, 128, 255): cases.append(fixture(original, routine=1, initial_count=count))
    for scalar0 in (0, 96, 99, -3):
        for flag in (0, 1, 256):
            for depth in (0, 2, 3): cases.append(fixture(original, scalar0=scalar0, flag=flag, depth=depth))
    for target in ((4, 0, 0), (2, 0.25, 0), (4, 1, 0.5)):
        for count in (0, 3, 4):
            for mutation in (0, 1): cases.append(fixture(original, target=target, initial_count=count, mutation=mutation))
    for records in (1, 2, 3):
        # Arbitrary multi-edge inputs can select an unset predecessor or a
        # zero arccos operand. These fixtures keep actual descriptor reads
        # initialized and every arithmetic operation inside the finite model.
        for vertex in ((0, 1, 2) if records == 1 else (records, records + 1)):
            for orientation in (0, 1):
                cases.append(fixture(original, records=records, vertex=vertex, orientation=orientation))
    for records in (2, 3, 4):
        for orientation in (0, 1):
            for flag in (0, 1, 256):
                cases.append(fixture(original, records=records, vertex=0, orientation=orientation, scalar0=99, flag=flag))
    for slot in (1, 2, 3):
        for orientation in (0, 1):
            cases.append(fixture(original, slot=slot, orientation=orientation, scalar0=99))
    for stop in (0, 1, 255): cases.append(fixture(original, stop=stop, scalar0=99))
    for position in ((-1, 0, 0), (5, 0, 0), (0.75, 10, 0), (1.5, 0, 0)):
        for mutation in (0, 2, 3): cases.append(fixture(original, objects=1, position=position, mutation=mutation))
    for object_type in (0, 1, 2):
        for reference in (False, True): cases.append(fixture(original, objects=1, object_type=object_type, reference=reference, position=(1.5, 0, 0)))
    for query in (0, 1, 2):
        for mutation in (0, 2, 3): cases.append(fixture(original, objects=1, query=query, mutation=mutation))
    for query in (1, 2):
        for mutation in (0, 2, 3, 4, 5, 7):
            cases.append(fixture(original, objects=1, query=query, mutation=mutation, scalar0=99))
    # Full LW route keys retain the upper half while edge indexing uses LHU.
    # Mutation8 returns a different complete record on the second side, so
    # both original reset/publication paths execute with nonzero upper bits.
    for initial_key in (0, 0x80000000):
        for mutation in (0, 5, 8):
            for orientation in (0, 1):
                cases.append(fixture(original, objects=1, query=1, mutation=mutation,
                                     scalar0=99, query_upper=0x8000,
                                     initial_key=initial_key, orientation=orientation))
    return cases


def golden_header(cases):
    lines = ['/* Synthetic fixtures only; no original code/assets. */',
             'struct CollisionWord { unsigned index,value; };',
             'struct CollisionGolden { unsigned routine,args[8],scalar[2],objects,mutation,query,result;',
             ' unsigned initial_count,changed_count,event_count; const struct CollisionWord *initial,*changed;',
             ' const unsigned *events; unsigned pool[7]; };']
    for i, c in enumerate(cases):
        for name, data in (('initial', [(j, v) for j, v in enumerate(c['initial']) if v]),
                           ('changed', [(j, v) for j, (a, v) in enumerate(zip(c['initial'], c['expected'])) if a != v])):
            lines.append('static const struct CollisionWord c%d_%s[] = {%s};' % (i, name, ','.join('{%d,0x%08Xu}' % p for p in data) or '{0,0}'))
        lines.append('static const unsigned c%d_events[] = {%s};' % (i, ','.join('0x%08Xu' % v for v in c['events']) or '0'))
    lines.append('static const struct CollisionGolden collision_golden[] = {')
    for i, c in enumerate(cases):
        values = [c['routine'], '{' + ','.join('0x%Xu' % v for v in c['args']) + '}',
                  '{' + ','.join('0x%Xu' % v for v in c['scalar']) + '}', c['objects'], c['mutation'], c['query'], '0x%Xu' % c['result'],
                  sum(bool(v) for v in c['initial']), sum(a != b for a, b in zip(c['initial'], c['expected'])), len(c['events']),
                  'c%d_initial' % i, 'c%d_changed' % i, 'c%d_events' % i,
                  '{' + ','.join('0x%Xu' % v for v in c['pool']) + '}']
        lines.append(' {' + ','.join(map(str, values)) + '},')
    lines.append('};')
    return '\n'.join(lines) + '\n'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', default='build/actor_collision/trace.json')
    parser.add_argument('--header', default='tests/native/actor_collision_golden.h')
    args = parser.parse_args()
    validated_elf(ROOT / 'orig/SLUS_216.68')
    original = (ROOT / 'orig/SLUS_216.68').read_bytes()
    cases = fixtures(original)
    header = golden_header(cases)
    output = dict(fixtures=len(cases), instruction_count=sum(c['instruction_count'] for c in cases),
                  maximum=max(c['instruction_count'] for c in cases),
                  input_sha256=hashlib.sha256(json.dumps([{k: c[k] for k in ('routine','args','scalar','objects','mutation','query','initial')} for c in cases], sort_keys=True).encode()).hexdigest(),
                  golden_sha256=hashlib.sha256(header.encode()).hexdigest(),
                  visited=sorted(set().union(*(set(c['visited']) for c in cases))))
    Path(args.output).parent.mkdir(parents=True, exist_ok=True)
    Path(args.output).write_text(json.dumps(output, indent=2) + '\n')
    Path(args.header).write_text(header, newline='\n')
    print(json.dumps({k: v for k, v in output.items() if k != 'visited'}))


if __name__ == '__main__': main()
