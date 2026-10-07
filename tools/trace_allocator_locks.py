"""Bounded allocator hooks plus actual syscall trampolines, with authored hooks.

Low32 words, call order and fresh global accesses only. No kernel/thread safety,
upper-register, exception, timing or allocator behavior is modeled or exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x3966E8, 0x396748), (0x396748, 0x396788))
STUBS = ((0x363750, 47), (0x3638A0, 68), (0x363880, 66))
SEMAPHORE, OWNER, COUNT = 0x400BC0, 0x4059F8, 0x4059FC
GLOBALS = (SEMAPHORE, OWNER, COUNT)
STACK = (0x7F000, 0x81000)
MASK = 0xFFFFFFFF


class AllocatorLockTrace(Trace):
    def __init__(self, original, routine=0, thread=0, mutation=0):
        super().__init__(original)
        if (type(routine) is not int or routine not in (0, 1) or
                type(thread) is not int or not 0 <= thread <= MASK or
                type(mutation) is not int or not 0 <= mutation < 16):
            raise ValueError('invalid allocator lock observer input')
        self.routine, self.thread, self.mutation = routine, thread & MASK, mutation
        self.events, self.visited, self.branches = [], set(), []
        self.calls = []

    def fetch(self, pc):
        intervals = (RANGES[self.routine], *((a, a + 16) for a, _ in STUBS))
        if pc % 4 or not any(a <= pc < b for a, b in intervals):
            raise ValueError('unreviewed allocator lock instruction')
        offset = pc - 0xFF000
        if offset < 0 or offset + 4 > len(self.original):
            raise ValueError('allocator lock instruction outside original')
        return struct.unpack_from('<I', self.original, offset)[0]

    @staticmethod
    def permitted(address, size):
        return (address in GLOBALS and size == 4 or
                STACK[0] <= address and address + size <= STACK[1])

    def load(self, address, size):
        if not self.permitted(address, size) or address % size:
            raise ValueError('unowned or unaligned allocator lock load')
        if not all(address + i in self.memory for i in range(size)):
            raise ValueError('uninitialized allocator lock load')
        return super().load(address, size)

    def save(self, address, value, size):
        if not self.permitted(address, size) or address % size:
            raise ValueError('unowned or unaligned allocator lock store')
        super().save(address, value, size)

    def syscall(self, pc):
        kinds = {a + 4: (kind, number) for kind, (a, number) in enumerate(STUBS)}
        if pc not in kinds:
            raise ValueError('unreviewed syscall instruction')
        kind, number = kinds[pc]
        if self.r[3] & MASK != number:
            raise ValueError('wrong syscall number')
        if ((self.routine == 0 and self.calls not in ([], [0])) or
                (self.routine == 1 and (self.calls or kind != 2)) or
                (self.routine == 0 and kind != (0 if not self.calls else 1))):
            raise ValueError('unexpected syscall order')
        argument = 0 if kind == 0 else self.r[4] & MASK
        self.events.extend((kind, argument, self.load(COUNT, 4), self.load(OWNER, 4), self.load(SEMAPHORE, 4)))
        if kind == 0:
            if self.mutation & 1:
                self.save(OWNER, self.thread ^ 1, 4)
                self.save(COUNT, 0x7FFFFFFF, 4)
                self.save(SEMAPHORE, 0x80000003, 4)
            if self.mutation & 2:
                self.save(OWNER, self.thread, 4)
                self.save(COUNT, 0xFFFFFFFF, 4)
                self.save(SEMAPHORE, 0xFFFF0001, 4)
        elif kind == 1 and self.mutation & 4:
            self.save(COUNT, MASK, 4)
            self.save(OWNER, 0xDEADBEEF, 4)
            self.save(SEMAPHORE, 0x12345678, 4)
        elif kind == 2 and self.mutation & 8:
            self.save(COUNT, 0x80000000, 4)
            self.save(OWNER, 0x55667788, 4)
            self.save(SEMAPHORE, 0x87654321, 4)
        self.r[2] = self.thread if kind == 0 else 0xA5A5A5A5
        for register in range(4, 12):
            self.r[register] = 0xCAFEBABE
        self.calls.append(kind)

    def execute(self, instruction, pc):
        op, rs, rt = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31
        rd, shamt, fn, imm = instruction >> 11 & 31, instruction >> 6 & 31, instruction & 63, instruction & 65535
        if instruction == 0:
            pass
        elif instruction == 12:
            self.instruction_count += 1
            self.visited.add(pc)
            self.syscall(pc)
            return None, False
        elif op == 0 and fn == 0x2D:
            if rt or shamt or (rs, rd) != (2, 16):
                raise ValueError('unsupported allocator lock DADDU')
        elif instruction == 0x03E00008:
            pass
        elif op == 9:
            permitted = ((rs, rt, imm) in ((29, 29, 0xFFD0), (29, 29, 0x30),
                (29, 29, 0xFFF0), (29, 29, 0x10), (2, 2, 1), (2, 2, 0xFFFF), (0, 3, 0xFFFF)) or
                rs == 0 and rt == 3 and imm in (47, 68, 66))
            if not permitted:
                raise ValueError('unsupported allocator lock ADDIU')
        elif op == 15:
            if rs or rt not in (2, 3, 5, 17) or imm != 0x40:
                raise ValueError('unsupported allocator lock global base')
        elif op == 35:
            if (rs, rt, imm) not in ((17, 2, 0x59F8), (2, 4, 0x0BC0), (3, 2, 0x59FC)):
                raise ValueError('unsupported allocator lock global load')
        elif op == 43:
            if (rs, rt, imm) not in ((17, 16, 0x59F8), (3, 2, 0x59FC), (5, 3, 0x59F8)):
                raise ValueError('unsupported allocator lock global store')
        elif op in (30, 31):
            if rs != 29 or (rt, imm) not in ((16, 32), (17, 16)):
                raise ValueError('unsupported allocator lock quadword save/restore')
        elif op in (55, 63):
            if rs != 29 or rt != 31 or imm:
                raise ValueError('unsupported allocator lock return save/restore')
        elif op == 3:
            target = ((pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)
            if target not in tuple(a for a, _ in STUBS):
                raise ValueError('unsupported allocator lock syscall target')
        elif op in (4, 5):
            if (op == 4 and (rs, rt) != (0, 0) or
                    op == 5 and (rs, rt) not in ((2, 16), (2, 0))):
                raise ValueError('unsupported allocator lock branch')
            signed = imm - 65536 if imm & 32768 else imm
            taken = (self.r[rs] & MASK == self.r[rt] & MASK) == (op == 4)
            self.instruction_count += 1
            self.visited.add(pc)
            self.branches.append((pc, taken))
            return (pc + 4 + signed * 4 if taken else None), False
        else:
            raise ValueError('unsupported allocator lock encoding')
        self.visited.add(pc)
        return super().execute(instruction, pc)

    def run(self, entry, stop=RETURN):
        if entry != RANGES[self.routine][0] or stop != RETURN:
            raise ValueError('unsupported allocator lock entry/stop')
        pc = entry
        a, b = RANGES[self.routine]
        stub_returns = {address + 8 for address, _ in STUBS}
        while True:
            if self.instruction_count > 128:
                raise ValueError('allocator lock instruction bound')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            if annul:
                raise ValueError('unreviewed allocator lock annul')
            if is_control_transfer(instruction):
                delay = self.fetch(pc + 4)
                if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                    raise ValueError('allocator lock control in delay')
                if instruction == 0x03E00008:
                    if pc == b - 8:
                        if target != stop:
                            raise ValueError('allocator lock terminal JR31/stop')
                        break
                    if pc not in stub_returns or not a <= target < b:
                        raise ValueError('allocator lock stub return outside caller')
                elif instruction >> 26 != 3 and target is not None and not a <= target < b:
                    raise ValueError('allocator lock branch outside body')
                pc = target if target is not None else pc + 8
            else:
                if target is not None:
                    raise ValueError('allocator lock unexpected transfer')
                pc += 4
        if self.calls not in (([0], [0, 1]) if self.routine == 0 else ([], [2])):
            raise ValueError('allocator lock incomplete callback sequence')


def make_fixture(original, routine, thread, owner, count, semaphore, mutation):
    if any(type(value) is not int or not 0 <= value <= MASK
           for value in (thread, owner, count, semaphore)):
        raise ValueError('invalid allocator lock fixture word')
    t = AllocatorLockTrace(original, routine, thread, mutation)
    for address, value in ((OWNER, owner), (COUNT, count), (SEMAPHORE, semaphore)):
        t.save(address, value, 4)
    t.r[29], t.r[31], t.r[4] = 0x80000, RETURN, 0x81234567
    saved = (0x0123456789ABCDEF0123456789ABCDEF, 0xFEDCBA9876543210FEDCBA9876543210)
    t.r[16:18] = saved
    t.run(RANGES[routine][0])
    assert t.r[29] == 0x80000 and t.r[31] == RETURN and tuple(t.r[16:18]) == saved
    return dict(routine=routine, thread=thread, owner=owner, count=count, semaphore=semaphore, mutation=mutation,
        expected_owner=t.load(OWNER, 4), expected_count=t.load(COUNT, 4), expected_semaphore=t.load(SEMAPHORE, 4),
        event_words=len(t.events), events=t.events + [0] * (10 - len(t.events)),
        instruction_count=t.instruction_count, visited=sorted(t.visited), branches=t.branches)


def fixtures(original):
    return [make_fixture(original, routine, thread, owner, count, semaphore, mutation)
        for routine in (0, 1) for thread in (0, 0xFFFFFFFF, 0x80000001, 0x7FFFFFFF)
        for owner in (thread, thread ^ 1, 0xFFFFFFFF)
        for count in (0, 1, 2, 0x80000000, 0xFFFFFFFF)
        for semaphore in (0, 0xFFFFFFFF, 0x80000002) for mutation in range(16)]


def golden_header(cases):
    keys = ('routine', 'thread', 'owner', 'count', 'semaphore', 'mutation',
            'expected_owner', 'expected_count', 'expected_semaphore', 'event_words')
    lines = ['/* Synthetic ordered syscall/global observations; no original code/data arrays. */',
        'struct AllocatorLockGolden { unsigned int ' + ','.join(keys) + ',events[10]; };',
        'static const struct AllocatorLockGolden allocator_lock_golden[] = {']
    for c in cases:
        lines.append(' {%s,{%s}},' % (','.join('0x%08Xu' % c[k] for k in keys),
                                    ','.join('0x%08Xu' % x for x in c['events'])))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=ROOT / 'build/reuse/allocator_locks/trace.json')
    parser.add_argument('--header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(ROOT / 'orig/SLUS_216.68')
    cases = fixtures(original)
    keys = ('routine', 'thread', 'owner', 'count', 'semaphore', 'mutation')
    report = dict(fixtures=len(cases), instructions=sum(c['instruction_count'] for c in cases),
        max_instructions=max(c['instruction_count'] for c in cases),
        input_sha256=hashlib.sha256(json.dumps([{k: c[k] for k in keys} for c in cases], sort_keys=True).encode()).hexdigest(),
        selected_instruction_coverage=[len({pc for c in cases for pc in c['visited'] if a <= pc < b}) for a, b in RANGES],
        syscall_stub_instruction_coverage=[len({pc for c in cases for pc in c['visited'] if a <= pc < a + 16}) for a, _ in STUBS],
        cases=cases)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    if args.header:
        args.header.write_text(golden_header(cases), newline='\n')
    print('allocator locks:', report['fixtures'], 'fixtures;', report['instructions'], 'instructions;', report['selected_instruction_coverage'], 'coverage')


if __name__ == '__main__':
    main()
