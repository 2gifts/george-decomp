"""Strict original store/copy fixtures for two connected route setup entries.

Reuse the reviewed integer/delay observer. FPR accesses here only move raw
words; no floating arithmetic, substituted helper, original code or assets
are exported. EE hardware exception/timing behavior is outside this model.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer, word
from trace_actor_collision import CollisionTrace

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x20BEA8, 0x20BFB0), (0x20D080, 0x20D190))
BUFFER, WORDS, OWNER = 0x20000, 1536, 64
END = BUFFER + WORDS * 4


class SetupTrace(CollisionTrace):
    def __init__(self, original):
        super().__init__(original)
        self.fbits = [0] * 32

    def memory_check(self, address, size):
        if (size not in (1, 2, 4) or address % size
                or not BUFFER <= address < address + size <= END):
            raise ValueError('route setup memory outside aligned arena')

    def load(self, address, size):
        self.memory_check(address, size)
        if any(address + i not in self.memory for i in range(size)):
            raise ValueError('uninitialized route setup memory')
        return Trace.load(self, address, size)

    def save(self, address, value, size):
        self.memory_check(address, size)
        return Trace.save(self, address, value, size)

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in RANGES):
            raise ValueError('unreviewed route setup instruction')
        return struct.unpack_from('<I', self.original, pc - 0xFF000)[0]

    def execute(self, w, pc):
        op, rs, rt, rd, fn = w >> 26, w >> 21 & 31, w >> 16 & 31, w >> 11 & 31, w & 63
        imm = w & 65535
        simm = imm - 65536 if imm & 32768 else imm
        allowed = w == 0 or w == 0x03E00008 or op == 0 and fn == 0x2D and w & 0x7C0 == 0
        allowed |= op in (4, 5, 9, 13, 21, 0x23, 0x28, 0x29, 0x2B, 0x31, 0x39)
        allowed |= op == 15 and rs == 0
        allowed |= op == 17 and rs == 4 and w & 0x7FF == 0
        if not allowed:
            raise ValueError('unsupported route setup instruction encoding')
        if self.instruction_count >= 100:
            raise ValueError('route setup instruction bound')
        if op in (0x31, 0x39) or op == 17:
            self.visited.add(pc)
            self.instruction_count += 1
            if op == 0x31:
                self.fbits[rt] = self.load((self.r[rs] + simm) & 0xFFFFFFFF, 4)
            elif op == 0x39:
                self.save((self.r[rs] + simm) & 0xFFFFFFFF, self.fbits[rt], 4)
            else:
                self.fbits[rd] = self.r[rt] & 0xFFFFFFFF
            return None, False
        return super().execute(w, pc)

    def run(self, entry):
        body = next((p for p in RANGES if p[0] == entry), None)
        if body is None:
            raise ValueError('route setup requires approved entry')
        pc = entry
        while True:
            if not body[0] <= pc < body[1]:
                raise ValueError('route setup transfer outside whole body')
            w = self.fetch(pc)
            target, annul = self.execute(w, pc)
            if is_control_transfer(w):
                self.branch_outcomes.append([pc, target is not None, annul])
                if not annul:
                    if pc + 4 >= body[1]:
                        raise ValueError('route setup delay outside whole body')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                        raise ValueError('route setup control transfer in delay')
                if w >> 26 == 0 and w & 63 == 8:
                    if w != 0x03E00008 or target != RETURN:
                        raise ValueError('route setup requires actual JR31 return')
                    return
                pc = pc + 8 if target is None else target
            else:
                if target is not None or annul:
                    raise ValueError('unexpected route setup transfer')
                pc += 4


def initial_word(index, seed):
    value = (index * 7 + seed) % 17 - 8
    return 0x80000000 if value == 0 and index & 1 else word(float(value))


def make_fixture(original, entry_index, point, target, first, second, kind, seed=1):
    if (entry_index not in (0, 1) or isinstance(entry_index, bool)
            or any(isinstance(v, bool) or not isinstance(v, int) for v in (point, target, first, second, kind, seed))
            or not 0 <= point <= WORDS - 3 or not 0 <= target <= WORDS - 3
            or any(not 0 <= v <= 0xFFFFFFFF for v in (first, second, kind, seed))):
        raise ValueError('unreviewed route setup fixture layout')
    trace = SetupTrace(original)
    for i in range(WORDS):
        Trace.save(trace, BUFFER + 4 * i, initial_word(i, seed), 4)
    opaque_state = (0x91827364 + seed) & 0xFFFFFFFF
    trace.r[4:11] = [BUFFER + OWNER * 4, opaque_state, kind, first, second,
                     BUFFER + point * 4, BUFFER + target * 4]
    trace.r[31] = RETURN
    trace.run(RANGES[entry_index][0])
    base, count = (0xECC, 26) if entry_index == 0 else (0xCB0, 23)
    output_start = OWNER + base // 4
    result = dict(entry=entry_index, point=point, target=target, first=first, second=second,
                  kind=kind, seed=seed, opaque_state=opaque_state,
                  expected_status=trace.load(BUFFER + OWNER * 4, 4),
                  expected=[trace.load(BUFFER + 4 * (output_start + i), 4) for i in range(count)],
                  instruction_count=trace.instruction_count, visited=sorted(trace.visited),
                  branches=trace.branch_outcomes)
    # Verify the compact golden covers every changed word in the whole arena.
    for i in range(WORDS):
        if i != OWNER and not output_start <= i < output_start + count:
            if trace.load(BUFFER + i * 4, 4) != initial_word(i, seed):
                raise ValueError('unexpected store outside published output view')
    return result


def fixtures(original):
    cases = []
    tags = ((1, 2), (3, 3), (0xFFFFFFFF, 4), (5, 0xFFFFFFFF),
            (0xFFFFFFFF, 0xFFFFFFFF), (0x80000001, 0x80000002))
    for entry in (0, 1):
        base = OWNER + (0xECC if entry == 0 else 0xCB0) // 4
        points = [1200, OWNER, *range(base - 3, base + 29)]
        for point in points:
            for first, second in tags:
                for target in (1208, point, point + 1):
                    cases.append(make_fixture(original, entry, point, target, first, second,
                                              (0, 0x1234000C, 0xFFFFFFFF)[len(cases) % 3], len(cases) + 1))
    return cases


def golden_header(cases):
    lines = ['/* Synthetic whole-original store/copy outputs; no game code or assets. */',
             'struct SetupGolden { u32 entry,point,target,first,second,kind,seed,state,status; u32 expected[26]; };',
             'static const struct SetupGolden setup_golden[] = {']
    for c in cases:
        values = [c[k] for k in ('entry', 'point', 'target', 'first', 'second', 'kind', 'seed', 'opaque_state', 'expected_status')]
        lines.append(' {' + ','.join('0x%08Xu' % v for v in values) + ',{' +
                     ','.join('0x%08Xu' % v for v in c['expected']) + '}},')
    return '\n'.join(lines + ['};', ''])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=ROOT / 'build/route_setup/trace.json')
    parser.add_argument('--header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(ROOT / 'orig/SLUS_216.68')
    cases = fixtures(original)
    inputs = [{k: c[k] for k in ('entry', 'point', 'target', 'first', 'second', 'kind', 'seed', 'opaque_state')} for c in cases]
    report = dict(fixtures=len(cases), input_sha256=hashlib.sha256(json.dumps(inputs, sort_keys=True).encode()).hexdigest(),
                  instructions=sum(c['instruction_count'] for c in cases),
                  max_instructions=max(c['instruction_count'] for c in cases),
                  selected_instruction_coverage=[len({pc for c in cases for pc in c['visited'] if a <= pc < b}) for a, b in RANGES],
                  cases=cases)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    if args.header:
        args.header.parent.mkdir(parents=True, exist_ok=True)
        args.header.write_text(golden_header(cases))
    print('route setup:', len(cases), 'fixtures;', report['instructions'], 'original instructions;', report['selected_instruction_coverage'], 'coverage')


if __name__ == '__main__':
    main()
