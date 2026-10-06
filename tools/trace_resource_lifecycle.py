"""Four complete resource ownership bodies with authored caller observations.

Reuse the reviewed scalar decoder and byte/halfword stores. Execute the actual
release wrapper recursively; other engine/virtual calls have explicit authored
effects. Only synthetic memory and events are exported, without retail tables.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_property_lifecycle import LifecycleTrace
from trace_geometry import RETURN

ROOT = Path(__file__).resolve().parents[1]
BUFFER, END = 0x20000, 0x20800
RECORD, CHILD, OTHER = 0x20100, 0x20200, 0x20240
HOLDER, OTHER_HOLDER, TABLE = 0x20300, 0x20340, 0x20400
VIRTUAL = 0xF00000C0
RANGES = ((0x20EF60, 0x20EFC8), (0x20EFC8, 0x20F004),
          (0x20F008, 0x20F070), (0x20F070, 0x20F128))
ENTRIES = tuple(a for a, b in RANGES)
CALLS = (0x226D78, 0x226ED8, VIRTUAL, 0x21C508, 0x21C5D8, 0x2267D0)


class ResourceLifecycleTrace(LifecycleTrace):
    def __init__(self, original, mutation=0, adjustment=0):
        super().__init__(original)
        self.mutation, self.adjustment = mutation, adjustment
        self.calls = [0] * len(CALLS)
        self.events = []

    def fetch(self, pc):
        if pc % 4 or not any(a <= pc < b for a, b in RANGES):
            raise ValueError('unreviewed resource lifecycle instruction')
        offset = pc - 0xFF000
        if offset < 0 or offset + 4 > len(self.original):
            raise ValueError('resource instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    def load(self, address, size):
        if not any(a <= address and address + size <= b
                   for a, b in ((BUFFER, END), (0x7F000, 0x81000))):
            raise ValueError('resource load outside authored memory')
        # Call the inherited strict-memory implementation after our narrower gate.
        return super().load(address, size)

    def save(self, address, value, size):
        if not any(a <= address and address + size <= b
                   for a, b in ((BUFFER, END), (0x7F000, 0x81000))):
            raise ValueError('resource store outside authored memory')
        return super().save(address, value, size)

    def execute(self, instruction, pc):
        if instruction >> 26 == 0x21:
            rs, rt = (instruction >> 21) & 31, (instruction >> 16) & 31
            offset = instruction & 65535
            if offset & 32768:
                offset -= 65536
            address = (self.r[rs] + offset) & 0xFFFFFFFF
            if address % 2:
                raise ValueError('unaligned resource LH outside model')
            value = self.load(address, 2)
            self.r[rt] = (value - 65536 if value & 32768 else value) & 0xFFFFFFFF
            self.r[0] = 0
            self.instruction_count += 1
            if self.instruction_count > 3000:
                raise ValueError('resource trace exceeded instruction bound')
            return None, False
        return super().execute(instruction, pc)

    def library_call(self, target):
        if target not in CALLS:
            raise ValueError('unknown resource lifecycle call')
        i = CALLS.index(target)
        a, b = self.r[4] & 0xFFFFFFFF, self.r[5] & 0xFFFFFFFF
        if i == 0 and a != CHILD:
            raise ValueError('unreviewed resource activation argument')
        if i == 1 and (a != CHILD or b != RECORD):
            raise ValueError('unreviewed resource detach arguments')
        if i == 2 and (a not in (CHILD + self.adjustment, OTHER + self.adjustment) or b):
            raise ValueError('unreviewed virtual release arguments')
        if i in (3, 4) and a not in (HOLDER, OTHER_HOLDER):
            raise ValueError('unreviewed resource holder argument')
        if i == 5 and a != RECORD:
            raise ValueError('unreviewed base release argument')
        self.calls[i] += 1
        self.events.extend((i, a, b if i in (1, 2, 5) else 0,
                            self.load(RECORD + 2, 1), self.load(RECORD + 0x20, 4)))
        if self.mutation == 1 and i == 0:
            self.save(RECORD + 0x10, OTHER_HOLDER, 4)
        if self.mutation == 2 and i == 1:
            self.save(RECORD + 0x24, OTHER, 4)
            self.save(RECORD + 2, self.load(RECORD + 2, 1) | 0x20, 1)
            self.save(RECORD + 0x10, OTHER_HOLDER, 4)
        if self.mutation == 3 and i == 2:
            self.save(RECORD + 2, self.load(RECORD + 2, 1) ^ 0x20, 1)
            self.save(RECORD + 0x10, OTHER_HOLDER, 4)
        # Actual callers ignore these arbitrary result words.
        self.r[2] = 0x87654321

    def run(self, entry, stop=RETURN):
        body = next((r for r in RANGES if r[0] == entry), None)
        if body is None:
            raise ValueError('unsupported resource lifecycle entry')
        pc = entry
        while pc != stop:
            if not body[0] <= pc < body[1]:
                raise ValueError('cross-body resource transfer')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            op = instruction >> 26
            if op == 0 and instruction & 63 == 8 and (instruction != 0x03E00008 or target != stop):
                raise ValueError('unreviewed resource return')
            branch = op in (1, 4, 5, 6, 7, 0x14, 0x15)
            if target is not None or branch and not annul:
                if pc + 4 >= body[1]:
                    raise ValueError('resource delay outside complete body')
                if self.execute(self.fetch(pc + 4), pc + 4) != (None, False):
                    raise ValueError('resource transfer in delay slot')
                if target is None:
                    pc += 8
                elif op == 3 or op == 0 and instruction & 63 == 9:
                    if target in ENTRIES:
                        self.run(target, self.r[31])
                    else:
                        self.library_call(target)
                    pc = self.r[31]
                else:
                    if not body[0] <= target < body[1] and (instruction != 0x03E00008 or target != stop):
                        raise ValueError('unreviewed resource return or transfer')
                    pc = target
            else:
                pc += 8 if annul else 4


def make_fixture(original, routine, flags, child_flags, count, present=1,
                 holder_flag=1, mutation=0, adjustment=-4, mode=0xFEDCBA98):
    t = ResourceLifecycleTrace(original, mutation, adjustment)
    for a, b in ((BUFFER, END), (0x7F000, 0x81000)):
        for p in range(a, b):
            t.memory[p] = 0x5A
    t.save(RECORD + 2, flags, 1)
    t.save(RECORD + 0x10, HOLDER, 4)
    t.save(RECORD + 0x20, TABLE, 4)
    t.save(RECORD + 0x24, CHILD if present else 0, 4)
    for child in (CHILD, OTHER):
        t.save(child, count, 2)
        t.save(child + 2, child_flags, 1)
        t.save(child + 0x20, TABLE, 4)
    t.save(HOLDER + 2, holder_flag, 1)
    t.save(OTHER_HOLDER + 2, 1, 1)
    t.save(TABLE + 0x10, adjustment, 2)
    t.save(TABLE + 0x14, VIRTUAL, 4)
    initial = [t.load(BUFFER + i * 4, 4) for i in range(512)]
    t.r[29], t.r[31], t.r[4], t.r[5] = 0x80000, RETURN, RECORD, mode
    t.run(ENTRIES[routine])
    return dict(routine=routine, flags=flags, child_flags=child_flags, count=count,
                present=present, holder_flag=holder_flag, mutation=mutation,
                adjustment=adjustment, mode=mode, initial=initial,
                expected=[t.load(BUFFER + i * 4, 4) for i in range(512)],
                calls=t.calls, events=t.events, instruction_count=t.instruction_count)


def fixtures(original):
    cases = []
    for routine in range(4):
        for flags in (0, 4, 0x20, 0x24, 0x80, 0x84, 0xA0, 0xA4, 0xFF):
            for child_flags in (0, 0x80):
                for count in (0, 0xFFFF):
                    cases.append(make_fixture(original, routine, flags, child_flags, count))
        for flags in (0, 4, 0x20, 0x24):
            cases.append(make_fixture(original, routine, flags, 0, 9, holder_flag=0))
        for mutation in (1, 2, 3):
            cases.append(make_fixture(original, routine, 4, 0x80, 9, mutation=mutation, adjustment=12))
        if routine != 0:
            cases.append(make_fixture(original, routine, 4, 0, 9, present=0))
    return cases


def input_hash(cases):
    keys = ('routine', 'flags', 'child_flags', 'count', 'present', 'holder_flag',
            'mutation', 'adjustment', 'mode', 'initial')
    return hashlib.sha256(json.dumps([{k: c[k] for k in keys} for c in cases],
                                    sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    n = max(len(c['events']) for c in cases)
    lines = ['/* Authored resource ownership fixtures; no original code/table arrays. */',
             'struct ResourceLifecycleGolden { u32 routine,flags,child_flags,count,present,holder_flag,mutation,adjustment,mode,initial[512],expected[512],calls[6],event_count,events[%d]; };' % n,
             'static const struct ResourceLifecycleGolden resource_lifecycle_golden[] = {']
    for c in cases:
        fields = ','.join('0x%08Xu' % (c[k] & 0xFFFFFFFF) for k in
                          ('routine', 'flags', 'child_flags', 'count', 'present', 'holder_flag', 'mutation', 'adjustment', 'mode'))
        array = lambda k: ','.join('0x%08Xu' % v for v in c[k])
        lines.append('    {%s,{%s},{%s},{%s},%du,{%s}},' %
                     (fields, array('initial'), array('expected'), array('calls'), len(c['events']), array('events')))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/resource_lifecycle_trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(dict(limitation=__doc__, input_sha256=input_hash(cases), cases=cases), indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(golden_header(cases))
    print('%d resource ownership fixtures; %d original instructions; maximum %d; input SHA256 %s' %
          (len(cases), sum(c['instruction_count'] for c in cases), max(c['instruction_count'] for c in cases), input_hash(cases)))


if __name__ == '__main__':
    main()
