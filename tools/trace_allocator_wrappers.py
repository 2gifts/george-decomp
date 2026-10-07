"""Trace two complete retail wrappers with authored lock/core/unlock observers.

Only low 32-bit caller-visible pointer/size lanes are modeled. Actual allocation,
thread/semaphore behavior, upper registers, exceptions and timing are not modeled.
Original instructions come from the validated local ELF; exported fixtures contain
only authored inputs, outputs and observer events. The three callbacks can replace
the global reent pointer independently, testing the actual fresh-load contract.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import RETURN, Trace, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x396348, 0x3963A8), (0x3971A8, 0x397208))
ENTRIES = tuple(a for a, _ in RANGES)
IMPURE = 0x405694
STACK = (0x7F000, 0x81000)
LOCK, UNLOCK = 0x3966E8, 0x396748
CORES = (0x39C620, 0x39C0A0)
REENTS = (0x20000, 0x20004, 0x20008, 0x2000C)


class AllocatorWrapperTrace(Trace):
    def __init__(self, original, routine=0, mutation=0, result=0x20040):
        super().__init__(original)
        if routine not in (0, 1) or not 0 <= mutation < 8:
            raise ValueError('unsupported allocator observer inputs')
        self.routine, self.mutation, self.result = routine, mutation, result
        self.events = []
        self.stage = 0
        self.visited = set()

    def fetch(self, pc):
        if pc % 4 or not RANGES[self.routine][0] <= pc < RANGES[self.routine][1]:
            raise ValueError('unreviewed allocator wrapper instruction')
        off = pc - 0xFF000
        if off < 0 or off + 4 > len(self.original):
            raise ValueError('allocator instruction outside original')
        return struct.unpack_from('<I', self.original, off)[0]

    @staticmethod
    def permitted(address, size):
        return (address == IMPURE and size == 4 or
                STACK[0] <= address and address + size <= STACK[1])

    def load(self, address, size):
        if not self.permitted(address, size) or address % size:
            raise ValueError('unowned or unaligned allocator memory load')
        return super().load(address, size)

    def save(self, address, value, size):
        if not self.permitted(address, size) or address % size:
            raise ValueError('unowned or unaligned allocator memory store')
        super().save(address, value, size)

    def execute(self, instruction, pc):
        op, rs, rt = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31
        rd, shamt, fn = instruction >> 11 & 31, instruction >> 6 & 31, instruction & 63
        imm = instruction & 65535
        if op == 0 and fn == 0x2D:
            if rt or shamt or (rs, rd) not in ((4, 18), (5, 17), (18, 5), (17, 6), (2, 17), (17, 2)):
                raise ValueError('unsupported allocator DADDU operand')
        elif op == 9:
            if rs != 29 or rt != 29 or imm not in (0xFFC0, 0x40):
                raise ValueError('unsupported allocator stack adjustment')
        elif op == 15:
            if rs or rt != 16 or imm != 0x40:
                raise ValueError('unsupported allocator global base')
        elif op == 35:
            if rs != 16 or rt != 4 or imm != 0x5694:
                raise ValueError('unsupported allocator global load')
        elif op in (30, 31):
            if rs != 29 or (rt, imm) not in ((16, 48), (17, 32), (18, 16)):
                raise ValueError('unsupported allocator quadword save/restore')
        elif op in (55, 63):
            if rs != 29 or rt != 31 or imm:
                raise ValueError('unsupported allocator return save/restore')
        elif op == 3:
            pass  # Exact target and stage are checked by library_call.
        elif instruction == 0x03E00008:
            pass
        else:
            raise ValueError('unsupported allocator instruction or control transfer')
        self.visited.add(pc)
        return super().execute(instruction, pc)

    def library_call(self, target):
        expected = (LOCK, CORES[self.routine], UNLOCK)
        if self.stage >= 3 or target != expected[self.stage]:
            raise ValueError('unexpected allocator observer target/order')
        current = self.load(IMPURE, 4)
        if self.r[4] & 0xFFFFFFFF != current or current not in REENTS:
            raise ValueError('allocator call did not receive fresh reent')
        a1, a2 = (self.r[5] & 0xFFFFFFFF, self.r[6] & 0xFFFFFFFF) if self.stage == 1 else (0, 0)
        self.events.extend((self.stage, current, a1, a2, current))
        if self.mutation & (1 << self.stage):
            self.save(IMPURE, REENTS[self.stage + 1], 4)
        self.r[2] = self.result if self.stage == 1 else 0xA5A5A5A5
        # Caller-saved arguments are deliberately destroyed after observation.
        for register in range(4, 12):
            self.r[register] = 0xDEADBEEF
        self.stage += 1

    def run(self, entry, stop=RETURN):
        if entry != ENTRIES[self.routine] or stop != RETURN:
            raise ValueError('unsupported allocator entry/stop')
        start, end = RANGES[self.routine]
        pc = entry
        while pc != stop:
            if not start <= pc < end:
                raise ValueError('cross-body allocator control transfer')
            instruction = self.fetch(pc)
            target, annul = self.execute(instruction, pc)
            if annul:
                raise ValueError('unexpected allocator annulled delay')
            if is_control_transfer(instruction):
                if pc + 4 >= end or is_control_transfer(self.fetch(pc + 4)):
                    raise ValueError('allocator control transfer in/outside delay slot')
                if self.execute(self.fetch(pc + 4), pc + 4) != (None, False):
                    raise ValueError('allocator delay produced control transfer')
                if instruction >> 26 == 3:
                    self.library_call(target)
                    pc = self.r[31]
                elif instruction == 0x03E00008 and pc == end - 8 and target == stop:
                    pc = target
                else:
                    raise ValueError('allocator return must be terminal JRRA')
            else:
                if target is not None:
                    raise ValueError('unreviewed allocator control result')
                pc += 4
        if self.stage != 3:
            raise ValueError('incomplete allocator observer sequence')


def make_fixture(original, routine, arg1, nbytes, mutation, result):
    t = AllocatorWrapperTrace(original, routine, mutation, result)
    t.save(IMPURE, REENTS[0], 4)
    t.r[29], t.r[31], t.r[4], t.r[5] = 0x80000, RETURN, arg1, nbytes
    saved = (0x0123456789ABCDEF0123456789ABCDEF,
             0xFEDCBA9876543210FEDCBA9876543210,
             0x12345678123456781234567812345678)
    t.r[16:19] = saved
    t.run(ENTRIES[routine])
    assert t.r[29] == 0x80000 and t.r[31] == RETURN and tuple(t.r[16:19]) == saved
    assert t.instruction_count == 24 and t.visited == set(range(*RANGES[routine], 4))
    return dict(routine=routine, arg1=arg1, nbytes=nbytes, mutation=mutation, result=result,
                expected_return=t.r[2] & 0xFFFFFFFF, expected_reent=t.load(IMPURE, 4),
                events=t.events, instruction_count=t.instruction_count)


def fixtures(original):
    return [make_fixture(original, routine, arg, count, mutation, result)
            for routine in (0, 1)
            for arg in ((4, 16, 0x80000020, 0xFFFFFFFF) if routine == 0 else (0, 0x20020, 0x81234567, 0xFFFFFFFF))
            for count in (0, 1, 16, 0x80000005, 0xFFFFFFFF)
            for mutation in range(8) for result in (0, 0x20040, 0xFFFFFFFF)]


def input_hash(cases):
    keys = ('routine', 'arg1', 'nbytes', 'mutation', 'result')
    return hashlib.sha256(json.dumps([{k: c[k] for k in keys} for c in cases],
                                     sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def golden_header(cases):
    fields = ('routine', 'arg1', 'nbytes', 'mutation', 'result', 'expected_return', 'expected_reent')
    lines = ['/* Authored allocator wrapper observations; no original instruction/data arrays. */',
             'struct AllocatorWrapperGolden { unsigned int ' + ','.join(fields) + ',events[15]; };',
             'static const struct AllocatorWrapperGolden allocator_wrapper_golden[] = {']
    for c in cases:
        lines.append('    {%s,{%s}},' % (','.join('0x%08Xu' % c[k] for k in fields),
                                       ','.join('0x%08Xu' % v for v in c['events'])))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/reuse/allocator_wrappers/trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.elf)
    cases = fixtures(original)
    report = dict(limit=__doc__, cases=cases, input_sha256=input_hash(cases),
                  instructions=sum(c['instruction_count'] for c in cases),
                  maximum=max(c['instruction_count'] for c in cases), all_selected_instructions_covered=48)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.write_text(golden_header(cases), newline='\n')
    print('%d fixtures; %d original instructions; maximum24; input SHA %s' %
          (len(cases), report['instructions'], report['input_sha256']))


if __name__ == '__main__':
    main()
