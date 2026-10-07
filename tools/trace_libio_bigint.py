"""Strict initialized libio Bigint observations using the published scalar decoder.

The six original functions and their complete integer helpers execute at their
actual retail PCs. Engine allocation/free are controlled nonnull hooks. Packed
double fields use integer bits, with no COP1 or general floating arithmetic.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from analyze import validated_elf
from trace_geometry import RETURN, is_control_transfer
from trace_string_registry import RegistryTrace

ROOT = Path(__file__).resolve().parents[1]
MASK = 0xffffffff
BASE, WORDS = 0x20000, 2048
END = BASE + WORDS * 4
A, B, C, E, BITS, NEW = 0x20100, 0x20400, 0x20700, 0x20c00, 0x20c04, 0x21000
STACK = (0x7e000, 0x81000)
SELECTED = ((0x35e9e0, 116), (0x35ea58, 208), (0x35f090, 380),
            (0x35f278, 400), (0x35f4a0, 300), (0x35f5d0, 364))
SUPPORT = ((0x35e910, 96), (0x35e970, 36), (0x35e998, 68),
           (0x35ec60, 132), (0x35ece8, 192), (0x35f210, 104))
RANGES = SELECTED + SUPPORT
METHODS = ('Brealloc', 'multadd', 'lshift', 'diff', 'b2d', 'd2b')


def initial_word(index, seed):
    return (0x6a31c29d ^ (index * 0x10203) ^ (seed * 0x443)) & MASK


class LibioBigintTrace(RegistryTrace):
    def __init__(self, original, parameters):
        super().__init__(original, parameters)
        self.invocations = [0] * len(RANGES)
        self.events = []
        self.branches = []
        self.allocations = 0
        self.last_allocation = None

    def memory_check(self, address, size):
        if size not in (1, 2, 4, 8, 16) or address % size or not (
            BASE <= address < address + size <= END or
            STACK[0] <= address < address + size <= STACK[1]):
            raise ValueError('unowned or unaligned libio Bigint memory')

    def load(self, address, size):
        self.memory_check(address, size)
        if not all(address + i in self.memory for i in range(size)):
            raise ValueError('uninitialized libio Bigint read')
        return int.from_bytes(bytes(self.memory[address + i] for i in range(size)), 'little')

    def save(self, address, value, size):
        self.memory_check(address, size)
        value &= (1 << (8 * size)) - 1
        for i in range(size):
            self.memory[address + i] = value >> (8 * i) & 255

    def fetch(self, pc):
        if pc % 4 or not any(start <= pc < start + size for start, size in RANGES):
            raise ValueError('unreviewed libio Bigint instruction')
        offset = pc - 0xff000
        if not 0 <= offset <= len(self.original) - 4:
            raise ValueError('libio Bigint original image bounds')
        return struct.unpack_from('<I', self.original, offset)[0]

    def execute(self, word, pc):
        if self.instruction_count >= 20000:
            raise ValueError('libio Bigint instruction budget before mutation')
        if word != self.fetch(pc):
            raise ValueError('instruction differs from reviewed original word')
        return RegistryTrace.execute(self, word, pc)

    def external(self, target):
        if target == 0x2af140:
            size = self.r[4] & MASK
            if size not in (52, 84, 148):
                raise ValueError('allocation outside initialized Bigint domain')
            if self.allocations >= 2:
                raise ValueError('libio Bigint allocation event bound')
            result = NEW + self.allocations * 0x200
            self.allocations += 1
            self.last_allocation = result
            self.events.append([1, size, result - BASE])
            self.r[2] = result
        elif target == 0x2af1e8:
            pointer = self.r[4] & MASK
            if pointer not in (A, B, C):
                raise ValueError('unreviewed original Bigint free argument')
            self.events.append([2, pointer - BASE, self.load(pointer + 12, 2)])
            if self.p['mutation']:
                if self.last_allocation is None:
                    raise ValueError('mutation requires captured allocation')
                self.save(self.last_allocation + 16, 3, 4)
        else:
            raise ValueError('unreviewed libio Bigint external call')

    def run(self, entry, stop=RETURN, depth=0):
        index = next((i for i, (a, n) in enumerate(RANGES) if a == entry), None)
        if index is None or depth > 8:
            raise ValueError('unreviewed libio Bigint entry/depth')
        start, size = RANGES[index]
        end = start + size
        self.invocations[index] += 1
        pc = entry
        while True:
            if not start <= pc < end:
                raise ValueError('libio Bigint transfer outside complete body')
            word = self.fetch(pc)
            target, annul = self.execute(word, pc)
            if is_control_transfer(word):
                if word >> 26 in (1, 4, 5, 6, 7, 20, 21, 22, 23):
                    self.branches.append([pc, target is not None])
                if not annul:
                    if not start <= pc + 4 < end:
                        raise ValueError('missing libio Bigint delay instruction')
                    delay = self.fetch(pc + 4)
                    if is_control_transfer(delay):
                        raise ValueError('control transfer in libio Bigint delay')
                    t, an = self.execute(delay, pc + 4)
                    if t is not None or an:
                        raise ValueError('unexpected libio Bigint delay transfer')
                op, fn = word >> 26, word & 63
                if op == 0 and fn == 8:
                    if word != 0x03e00008 or target != stop:
                        raise ValueError('libio Bigint return must be actual JR31 to selected stop')
                    return
                if op == 3:
                    if target in (a for a, n in RANGES):
                        self.run(target, pc + 8, depth + 1)
                    else:
                        self.external(target)
                    pc += 8
                    continue
                if op == 2 and target == 0x2af1e8:
                    self.external(target)
                    if self.r[31] != stop:
                        raise ValueError('libio Bigint tail external return mismatch')
                    return
                if target is not None:
                    if not start <= target < end:
                        raise ValueError('libio Bigint branch outside selected body')
                    pc = target
                else:
                    pc += 8
            else:
                if target is not None or annul:
                    raise ValueError('unexpected libio Bigint transfer')
                pc += 4

    def initialize(self):
        for i in range(WORDS):
            self.save(BASE + i * 4, initial_word(i, self.p['seed']), 4)
        for address in range(STACK[0], STACK[1]):
            self.memory[address] = (address * 13 + self.p['seed'] * 7) & 255
        for address, key in ((A, 'a'), (B, 'b'), (C, 'c')):
            limbs = self.p[key]
            k = self.p[key + '_k']
            if not 3 <= k <= 5 or not 1 <= len(limbs) <= 1 << k:
                raise ValueError('invalid initialized Bigint capacity/word count')
            self.save(address + 4, k, 4)
            self.save(address + 8, 1 << k, 4)
            self.save(address + 12, self.p[key + '_stack'], 2)
            self.save(address + 14, self.p[key + '_sign'], 2)
            self.save(address + 16, len(limbs), 4)
            for i, value in enumerate(limbs):
                self.save(address + 20 + 4 * i, value, 4)
        self.r[29] = 0x80800
        self.r[31] = RETURN

    def observe(self):
        self.initialize()
        method = self.p['method']
        if method == 0:
            self.r[4], self.r[5] = (0 if self.p['null'] else A), self.p['argument']
        elif method == 1:
            self.r[4], self.r[5], self.r[6] = A, self.p['argument'], self.p['addend']
        elif method == 2:
            self.r[4], self.r[5] = A, self.p['argument']
        elif method == 3:
            self.r[4], self.r[5], self.r[6] = (0 if self.p['null'] else C), A, B
        elif method == 4:
            self.r[4], self.r[5] = A, E
        elif method == 5:
            self.r[4], self.r[5], self.r[6], self.r[7] = (0 if self.p['null'] else C), self.p['double_bits'], E, BITS
        else:
            raise ValueError('unreviewed Bigint method selector')
        self.run(SELECTED[method][0])
        result = self.r[2] if method == 4 else (self.r[2] & MASK) - BASE
        return dict(parameters=self.p, return_value=result,
                    memory=[self.load(BASE + i * 4, 4) for i in range(WORDS)],
                    events=self.events, instruction_count=self.instruction_count,
                    visited=sorted(self.visited), branches=self.branches,
                    invocations=self.invocations)


def fixtures():
    result = []
    def add(method, **kw):
        p = dict(method=method, seed=len(result), a=[1], b=[1], c=[0],
                 a_k=3, b_k=3, c_k=3, a_stack=0, b_stack=1, c_stack=0,
                 a_sign=1, b_sign=0, c_sign=7, argument=0, addend=0,
                 double_bits=0x3ff0000000000000, null=0, mutation=0)
        p.update(kw)
        result.append(p)
    for stack in (0, 1, 0x8000):
        for old_k in (3, 4):
            for requested in (0, 3, 4, 5):
                add(0, a=[0x80000001, 0xffffffff, 3], a_k=old_k,
                    a_stack=stack, argument=requested)
    for requested in (0, 1, 3, 4, 5):
        add(0, null=1, argument=requested)
    for limbs in ([0], [1], [0xffffffff], [0xffff, 0x10000, 0x80000000],
                  [0xffffffff] * 8, [0xffffffff] * 16):
        for multiplier, carry in ((0, 0), (0, 1), (1, 0), (2, 1),
                                   (10, 9), (0x10000, 0xffff), (0x10001, 0)):
            add(1, a=list(limbs), a_k=4 if len(limbs) > 8 else 3,
                argument=multiplier, addend=carry)
    # Real callback mutation checks the captured local wds across reallocation.
    add(1, a=[0xffffffff] * 8, argument=10, addend=9, mutation=1)
    for limbs in ([1], [0x80000000], [0xffffffff, 1],
                  [0xffffffff] * 8, [0xffffffff] * 16):
        for shift in (0, 1, 7, 31, 32, 33, 63, 64, 95, 127, 255, 511):
            add(2, a=list(limbs), a_k=4 if len(limbs) > 8 else 3, argument=shift)
    comparisons = [([0], [0]), ([1], [1]), ([2], [1]), ([1], [2]),
                   ([0x80000000], [0x7fffffff]), ([0xffffffff], [1]),
                   ([0, 1], [1]), ([1], [0, 1]), ([1, 1], [2, 1]),
                   ([0, 0, 1], [0xffffffff, 0xffffffff]),
                   ([0xffffffff] * 16, [1])]
    for aa, bb in comparisons:
        for null in (0, 1):
            add(3, a=aa, b=bb, a_k=4 if len(aa) > 8 else 3,
                b_k=4 if len(bb) > 8 else 3, null=null)
    for wds in (1, 2, 3, 8):
        for bit in range(32):
            for low in (0, 0xffffffff):
                limbs = [low] * (wds - 1) + [1 << bit]
                add(4, a=limbs)
    for exponent in (0, 1, 2, 512, 1023, 1024, 2046):
        for fraction in (1, 2, 4, 8, 1 << 20, 1 << 31, 1 << 32,
                         1 << 40, 1 << 51, (1 << 52) - 1, 0):
            if exponent == 0 and fraction == 0:
                continue
            for negative in (0, 1):
                add(5, double_bits=(negative << 63) | (exponent << 52) | fraction,
                    null=(len(result) & 1))
    return result


def generate(original):
    observations = [LibioBigintTrace(original, p).observe() for p in fixtures()]
    visited = set().union(*(set(o['visited']) for o in observations))
    selected_pcs = set(pc for start, size in SELECTED for pc in range(start, start + size, 4))
    branch_outcomes = {}
    for o in observations:
        for pc, taken in o['branches']:
            branch_outcomes.setdefault(pc, set()).add(taken)
    summary = dict(fixture_count=len(observations), original_instructions=sum(o['instruction_count'] for o in observations),
                   maximum_instructions=max(o['instruction_count'] for o in observations),
                   selected_visited_words=len(visited & selected_pcs), selected_total_words=len(selected_pcs),
                   unvisited_selected_pcs=['0x%08X' % pc for pc in sorted(selected_pcs - visited)],
                   branch_outcomes=[dict(pc='0x%08X' % pc, outcomes=sorted(v)) for pc, v in sorted(branch_outcomes.items())],
                   no_COP1_or_float_arithmetic=True, no_helper_or_data_award=True,
                   domain='Initialized nonnull valid capacities/word counts; normalized nonzero b2d, finite nonzero d2b packedbits, bounded positive shifts/products, disjoint input/output objects. Engine allocation/free controlled, not heap/OS fidelity.')
    return dict(summary=summary, observations=observations)


def golden(packet, path):
    def array(values):
        return '{' + ','.join('0x%08Xu' % v for v in values) + '}'
    lines = ['/* Authored initialized inputs and full original-observed outputs; no original instructions. */',
             '#define LIBIO_BIGINT_WORDS %d' % WORDS,
             'typedef struct { unsigned method,seed,a_k,b_k,c_k,a_stack,b_stack,c_stack,a_sign,b_sign,c_sign,argument,addend,is_null,mutation,a_n,b_n,c_n; unsigned a[32],b[32],c[32]; unsigned long long double_bits,result; unsigned event_n,events[2][3]; unsigned delta_n,deltas[256][2]; } LibioBigintFixture;',
             'static const LibioBigintFixture libio_bigint_fixtures[] = {']
    for observation in packet['observations']:
        p = observation['parameters']
        fields = [p[k] for k in ('method', 'seed', 'a_k', 'b_k', 'c_k', 'a_stack', 'b_stack', 'c_stack', 'a_sign', 'b_sign', 'c_sign', 'argument', 'addend', 'null', 'mutation')]
        fields += [len(p[k]) for k in ('a', 'b', 'c')]
        parts = [','.join(str(v) + 'u' for v in fields)]
        parts += [array(p[k]) for k in ('a', 'b', 'c')]
        deltas = [[i, value] for i, value in enumerate(observation['memory'])
                  if value != initial_word(i, p['seed'])]
        assert len(deltas) <= 256
        parts += ['0x%016XULL' % p['double_bits'], '0x%016XULL' % observation['return_value'],
                  str(len(observation['events'])) + 'u',
                  '{' + ','.join(array(e) for e in observation['events']) + '}', array(observation['memory'])]
        parts[-1:] = [str(len(deltas)) + 'u', '{' + ','.join(array(d) for d in deltas) + '}']
        lines.append('{' + ','.join(parts) + '},')
    lines.append('};')
    path.write_text('\n'.join(lines) + '\n', newline='\n')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original', type=Path, default=ROOT / 'orig/SLUS_216.68')
    parser.add_argument('--output', type=Path, default=ROOT / 'build/mprec_second/trace.json')
    parser.add_argument('--golden-header', type=Path)
    args = parser.parse_args()
    _, original = validated_elf(args.original)
    packet = generate(original)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(packet, separators=(',', ':')) + '\n', newline='\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True, exist_ok=True)
        golden(packet, args.golden_header)
    print(json.dumps({k: v for k, v in packet['summary'].items() if k != 'branch_outcomes'}, indent=2))
