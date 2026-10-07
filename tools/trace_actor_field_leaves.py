"""Bounded scalar field effects, delegating the unchanged registry decoder.

Only lower64 results and initialized scalar memory are observed. Original
class/prototype, upper128, exception/timing and concurrency are not modeled.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN
from trace_string_registry import RegistryTrace

ROOT = Path(__file__).resolve().parents[1]
MASK, U64 = 0xFFFFFFFF, (1 << 64) - 1
BASE, BYTES, WORDS = 0x20000, 0x368, 0x368 // 4
RANGES = ((0x18FC88, 0x18FC98), (0x18FCC8, 0x18FCD8),
          (0x18FCD8, 0x18FCEC), (0x18FCF0, 0x18FD00),
          (0x191BD0, 0x191BE0), (0x191BE0, 0x191BF0),
          (0x1F6138, 0x1F6148), (0x1F6148, 0x1F6158),
          (0x1BA5A0, 0x1BA5B0), (0x1BA5B0, 0x1BA5C4),
          (0x1BA5C8, 0x1BA5D4))
FIELD_OFFSETS = (0x198, 0x198, 0x198, 0x190, 0x360, 0x360,
                 0x38, 0x38, 0x148, 0x148, 0x148)
NARROW = (0, 1, 0x10, 0x80000000, 0x80000001, 0x7FFFFFFF,
          0xFFFFFFFF, 0xF0, 0x40000000, 0x10000)
HIGH = (0, 0x80000000, 0xFFFFFFFF, 1, 0x7FFFFFFF)
MASKS = (0, 1, 0x10, 0x80000000, 0xFFFFFFFF, 0x8000000000000000,
         U64, 0x100000000, 0xFFFFFFFF00000000, 0x7FFFFFFF80000000,
         0x8000000100000001)


def initial_word(index, salt):
    return (0xC63A8107 ^ ((index * 0x1020311) & MASK) ^ salt) & MASK


class ActorFieldTrace(Trace):
    def __init__(self, original, routine=0):
        super().__init__(original)
        if type(routine) is not int or not 0 <= routine < len(RANGES):
            raise ValueError('unreviewed field entry')
        self.routine = routine
        self.visited = set()
        a, b = RANGES[routine]
        self.expected_words = {}
        for pc in range(a, b, 4):
            offset = pc - 0xFF000
            if offset >= 0 and offset + 4 <= len(original):
                self.expected_words[pc] = struct.unpack_from('<I', original, offset)[0]

    @staticmethod
    def memory_check(address, size):
        if (size not in (4, 8) or address % size or
                not BASE <= address < address + size <= BASE + BYTES):
            raise ValueError('unowned or unaligned field memory')

    def initialized_check(self, address, size):
        self.memory_check(address, size)
        if not all(address + i in self.memory for i in range(size)):
            raise ValueError('uninitialized field memory')

    def load(self, address, size):
        self.initialized_check(address, size)
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        Trace.save(self, address, value, size)

    def fetch(self, pc):
        if pc not in self.expected_words:
            raise ValueError('unreviewed field PC')
        offset = pc - 0xFF000
        word = struct.unpack_from('<I', self.original, offset)[0]
        if word != self.expected_words[pc]:
            raise ValueError('changed field instruction')
        return word

    def execute(self, word, pc):
        # All rejection checks precede the shared decoder's count/visited write.
        if pc not in self.expected_words or word != self.expected_words[pc]:
            raise ValueError('unreviewed field PC/word')
        if self.instruction_count >= 8:
            raise ValueError('field instruction budget')
        op, rs, rt = word >> 26, word >> 21 & 31, word >> 16 & 31
        rd, sh, fn, imm = word >> 11 & 31, word >> 6 & 31, word & 63, word & 65535
        if op in (35, 55, 43):
            offset = FIELD_OFFSETS[self.routine]
            if (rs != 4 or rt != 2 or imm != offset or
                    op == 55 and self.routine != 3 or
                    op == 35 and self.routine == 3 or
                    op == 43 and self.routine in (0, 3, 10)):
                raise ValueError('reserved field memory operands')
            address = ((self.r[rs] & MASK) + imm) & MASK
            self.initialized_check(address, 8 if op == 55 else 4)
        elif op == 9:
            if (rs, rt) != (2, 2) or self.routine not in (4, 5, 6, 7) or imm != (
                    1 if self.routine in (4, 6) else 65535):
                raise ValueError('reserved field ADDIU operands')
        elif op == 0:
            permitted = (sh == 0 and (
                fn in (0x24, 0x25) and (rs, rt, rd) == (2, 5, 2) or
                fn == 0x27 and (rs, rt, rd) == (0, 5, 5) or
                fn == 0x2B and (rs, rt, rd) == (0, 2, 2) or
                fn == 8 and (rs, rt, rd) == (31, 0, 0)))
            if not permitted:
                raise ValueError('reserved field SPECIAL operands')
            if fn == 8 and self.r[31] & MASK != RETURN:
                raise ValueError('unexpected field JR31 target')
        else:
            raise ValueError('unsupported field family')
        return RegistryTrace.execute(self, word, pc)

    def run(self, entry=None):
        if entry is None:
            entry = RANGES[self.routine][0]
        if entry != RANGES[self.routine][0] or self.r[31] & MASK != RETURN:
            raise ValueError('unreviewed field entry/return')
        return Trace.run(self, entry)


def make_fixture(original, routine, word, high, mask, salt):
    if (type(routine) is not int or not 0 <= routine < len(RANGES) or
            any(type(v) is not int or not 0 <= v <= MASK for v in (word, high, salt)) or
            type(mask) is not int or not 0 <= mask <= U64):
        raise ValueError('invalid field fixture input')
    trace = ActorFieldTrace(original, routine)
    for i in range(WORDS):
        trace.save(BASE + 4 * i, initial_word(i, salt), 4)
    for offset in (0x38, 0x148, 0x198, 0x360):
        trace.save(BASE + offset, word, 4)
    trace.save(BASE + 0x190, (high << 32) | word, 8)
    initial = [trace.load(BASE + 4 * i, 4) for i in range(WORDS)]
    trace.r[4], trace.r[5], trace.r[31] = BASE, mask, RETURN
    trace.run()
    return dict(routine=routine, word=word, high=high, mask=mask, salt=salt,
                initial=initial, expected=[trace.load(BASE + 4 * i, 4) for i in range(WORDS)],
                expected_result=trace.r[2] & U64, instructions=trace.instruction_count,
                visited=sorted(trace.visited))


def fixtures(original):
    parameters = []
    for routine in range(len(RANGES)):
        if routine == 3:
            parameters.extend((routine, word, high, mask) for word in NARROW
                              for high in HIGH for mask in MASKS)
        elif routine in (4, 5, 6, 7):
            parameters.extend((routine, word, 0, 0) for word in NARROW)
        else:
            parameters.extend((routine, word, 0, mask) for word in NARROW for mask in MASKS)
    return [make_fixture(original, r, w, h, m, (0x59A62F01 * (i + 1)) & MASK)
            for i, (r, w, h, m) in enumerate(parameters)]


def golden_header(cases):
    lines = ['/* Synthetic full-memory/lower64 observations; no original code arrays. */',
             'struct ActorFieldGolden { u32 routine,word,high,salt; GeorgeActorBits64 mask,result; u32 expected[%d]; };' % WORDS,
             'static const struct ActorFieldGolden actor_field_golden[] = {']
    for case in cases:
        values = ','.join('0x%08Xu' % case[k] for k in ('routine', 'word', 'high', 'salt'))
        lines.append(' {%s,0x%016XULL,0x%016XULL,{%s}},' % (
            values, case['mask'], case['expected_result'],
            ','.join('0x%08Xu' % w for w in case['expected'])))
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=ROOT / 'build/actor_field_leaves/trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(ROOT / 'orig/SLUS_216.68')
    cases = fixtures(original)
    inputs = [{k: c[k] for k in ('routine', 'word', 'high', 'mask', 'salt')} for c in cases]
    report = dict(fixtures=len(cases), instructions=sum(c['instructions'] for c in cases),
                  max_instructions=max(c['instructions'] for c in cases),
                  coverage=[len({pc for c in cases for pc in c['visited'] if a <= pc < b}) for a, b in RANGES],
                  input_sha256=hashlib.sha256(json.dumps(inputs, sort_keys=True).encode()).hexdigest(),
                  limitation='Bounded initialized scalar memory and lower64 result only; no original class, upper128, exception/timing, concurrency or hardware equality.',
                  cases=cases)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        args.golden_header.write_text(golden_header(cases), newline='\n')
    print('actor fields: %d fixtures, %d instructions, coverage %s' % (
        report['fixtures'], report['instructions'], report['coverage']))


if __name__ == '__main__':
    main()
