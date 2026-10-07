"""Seven-word actual-PC forwarding effects; helper is controlled/unawarded."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

R = Path.cwd().resolve()
sys.path.insert(0, str(R / 'tools'))
from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer
from trace_string_registry import RegistryTrace

MASK, U64 = 0xFFFFFFFF, (1 << 64) - 1
ARENA, WORDS, SP, TABLE = 0x20000, 32, 0x80000, 0x43A2B0
ENTRIES = (0x1DA5C8, 0x1DB910, 0x1DC580, 0x1DC630, 0x1DC6D8,
           0x1DC788, 0x1DFD18, 0x1E04A8, 0x1E1DD0, 0x1E1E58,
           0x1E5968, 0x1E6808, 0x1E7500, 0x1E87A0, 0x1E8930,
           0x1E8D78, 0x1E8E20)
BODY_SHA = '2fa583e293ca5e514a8703ed00e8b810c3ad087237dd5f4a580f3275624eca73'
FLAGS = (0, 1, 2, 3, 0x10000, 0x80000000, 0x80000001, MASK)


class ForwardTrace(Trace):
    def __init__(self, original, entry, budget=7):
        super().__init__(original)
        if entry not in ENTRIES or type(budget) is not int or not 0 <= budget <= 7:
            raise ValueError('unreviewed forwarding entry/budget')
        self.entry, self.budget = entry, budget
        self.visited, self.events, self.stores = set(), [], []
        b = original[entry-0xFF000:entry-0xFF000+28]
        if hashlib.sha256(b).hexdigest() != BODY_SHA:
            raise ValueError('changed complete original forwarding body')
        self.expected = dict(zip(range(entry, entry+28, 4), struct.unpack('<7I', b)))
        self.storage = None

    @staticmethod
    def memory_check(a, n):
        if n not in (4, 8) or a % n or not (
                ARENA <= a < a+n <= ARENA+WORDS*4 or SP-16 <= a < a+n <= SP):
            raise ValueError('unowned or unaligned forwarding memory')

    def initialized(self, a, n):
        self.memory_check(a, n)
        if not all(a+i in self.memory for i in range(n)):
            raise ValueError('uninitialized forwarding memory')

    def load(self, a, n):
        self.initialized(a, n)
        return Trace.load(self, a, n)

    def save(self, a, v, n):
        self.memory_check(a, n)
        Trace.save(self, a, v, n)
        self.stores.append([a, n, v & ((1 << (8*n))-1)])

    def fetch(self, pc):
        if pc not in self.expected:
            raise ValueError('unreviewed forwarding PC')
        w = struct.unpack_from('<I', self.original, pc-0xFF000)[0]
        if w != self.expected[pc]:
            raise ValueError('changed forwarding instruction')
        return w

    def execute(self, w, pc):
        if pc not in self.expected or w != self.expected[pc]:
            raise ValueError('unreviewed forwarding PC/word')
        if self.instruction_count >= self.budget:
            raise ValueError('forwarding budget before mutation')
        op = w >> 26
        if op in (55, 63):
            imm = w & 65535
            si = imm-65536 if imm & 32768 else imm
            a = (self.r[w >> 21 & 31]+si) & MASK
            self.initialized(a, 8)
        if is_control_transfer(w):
            delay = self.fetch(pc+4)
            if is_control_transfer(delay):
                raise ValueError('control in forwarding delay')
            if op == 3 and ((w & 0x3FFFFFF) << 2) != 0x20D2C0:
                raise ValueError('unreviewed forwarding helper')
            if op == 0 and (self.r[31] & MASK) != RETURN:
                raise ValueError('unreviewed forwarding return')
        return RegistryTrace.execute(self, w, pc)

    def controlled_helper(self, target):
        # This records only the declared finite effect seam, not helper execution.
        if target != 0x20D2C0 or self.instruction_count >= self.budget:
            raise ValueError('helper target/budget before mutation')
        storage, flags = self.r[4] & MASK, self.r[5] & MASK
        if storage not in (ARENA, ARENA+32):
            raise ValueError('untyped forwarding storage')
        self.initialized(storage+12, 4)
        self.events.append([1, storage, flags])
        self.save(storage+12, TABLE, 4)
        if flags & 1:
            self.events.append([2, storage, self.load(storage+12, 4)])
        # Incidental lower64 lane is observed only by the original observer.
        self.r[2] = 0x7654321089ABCDEF
        for r in range(3, 16): self.r[r] = 0xDEADBEEFDEADBEEF
        self.r[0] = 0

    def run(self):
        if self.r[31] & MASK != RETURN or self.r[29] != SP:
            raise ValueError('unreviewed forwarding stack/return')
        pc = self.entry
        while pc != RETURN:
            w = self.fetch(pc)
            target, annul = self.execute(w, pc)
            if annul: raise ValueError('no likely branch in selected forwarding body')
            if is_control_transfer(w):
                dw = self.fetch(pc+4)
                if is_control_transfer(dw): raise ValueError('control in forwarding delay')
                dt, da = self.execute(dw, pc+4)
                if dt is not None or da: raise ValueError('forwarding delay changed control')
                if w >> 26 == 3:
                    self.controlled_helper(target)
                    pc += 8
                else: pc = target
            else: pc += 4


def fixture(original, routine, offset, flags, salt):
    if routine not in range(17) or offset not in (0, 32) or flags not in FLAGS:
        raise ValueError('unreviewed fixture')
    t = ForwardTrace(original, ENTRIES[routine])
    for i in range(WORDS): t.save(ARENA+4*i, (0x572B1903 ^ (i*0x1020311) ^ salt) & MASK, 4)
    # Both actual pointer cells receive initialized observer tokens only.
    for off in (12, 44): t.save(ARENA+off, 0x43A000+(salt & 12), 4)
    for a in range(SP-16, SP, 8): t.save(a, 0, 8)
    initial = [t.load(ARENA+4*i, 4) for i in range(WORDS)]
    for i in range(16, 29): t.r[i] = 0xABCD000000000000+i
    saved = t.r[16:29]
    t.r[4], t.r[5], t.r[29], t.r[31] = ARENA+offset, flags, SP, RETURN
    t.stores.clear()
    t.run()
    assert t.r[16:29] == saved and t.r[29] == SP and t.r[31] == RETURN and t.r[0] == 0
    assert t.instruction_count == 7 and t.visited == set(range(t.entry, t.entry+28, 4))
    return dict(routine=routine,offset=offset,flags=flags,salt=salt,initial=initial,
        expected=[t.load(ARENA+4*i,4) for i in range(WORDS)],events=t.events,
        stores=t.stores,instructions=7,visited=sorted(t.visited),
        incidental_original_GPR2=t.r[2],native_void_result_compared=False)


def fixtures(original):
    return [fixture(original,r,o,f,s) for r in range(17) for o in (0,32)
            for f in FLAGS for s in (0,0x93A62018)]


def golden(cases):
    lines = ['/* Initialized synthetic forwarding state; no original code/table arrays. */',
        'struct GoalForwardGolden { u32 offset,flags,salt; u32 initial[32],expected[32]; };',
        'static const struct GoalForwardGolden goal_forward_golden[] = {']
    # The native source is shared across17 PCs; execute each distinct32 input once.
    for c in cases:
        if c['routine'] != 0: continue
        fmt = lambda words: ','.join('0x%08Xu' % v for v in words)
        lines.append(' {%du,0x%08Xu,0x%08Xu,{%s},{%s}},' %
            (c['offset'],c['flags'],c['salt'],fmt(c['initial']),fmt(c['expected'])))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True)
    p.add_argument('--golden-header',type=Path,required=True);a=p.parse_args()
    _,raw=validated_elf(R/'orig/SLUS_216.68');cases=fixtures(raw)
    a.output.write_bytes((json.dumps(dict(cases=cases,unique_native_inputs=32,
        actual_original_executions=len(cases),helper_controlled_unawarded=True),indent=2)+'\n').encode())
    a.golden_header.write_bytes(golden(cases).encode())

if __name__ == '__main__': main()
