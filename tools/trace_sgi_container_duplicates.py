"""Actual fourteen clone PCs with unchanged reviewed scalar/allocator decoder.

The SGI constructed native pair is a bounded semantic counterpart. Original
clone nodes have only observed next0/raw-u32-keybits4; their class/size/lifetime
and pointer-like payload semantics remain unknown. No instruction PC spoofing.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

import trace_completion_resolver as inherited
from analyze import validated_elf
from trace_geometry import RETURN, Trace, is_control_transfer
from trace_string_registry import sx32

ROOT = Path(__file__).resolve().parents[1]
MASK = inherited.MASK
BASE, WORDS, END = inherited.BASE, inherited.WORDS, inherited.END
TABLE, VECTOR = inherited.TABLE, inherited.VECTOR
BUCKETS, OLD, NEW = inherited.BUCKETS, inherited.OLD, inherited.NEW
NODES, PAYLOADS = inherited.NODES, inherited.PAYLOADS
ITERATOR, VALUE, HEAP = inherited.ITERATOR, inherited.VALUE, inherited.HEAP
HEADS, START, FINISH = inherited.HEADS, inherited.START, inherited.FINISH
HEAP_SIZE, HANDLER = inherited.HEAP_SIZE, inherited.HANDLER
HEAP_GLOBAL, REENT_GLOBAL = inherited.HEAP_GLOBAL, inherited.REENT_GLOBAL
GLOBALS, STACK = inherited.GLOBALS, inherited.STACK
initial_word = inherited.initial_word
SELECTED = tuple((a, a + 804) for a in (0x24B0E8, 0x24B4A8, 0x24B870,
                 0x24BC30, 0x24BFF8, 0x24C3C0, 0x24C788, 0x24CB50, 0x2BBDA0)) + tuple(
                 (a, a + 148) for a in (0x24D160, 0x24D1F8, 0x24D290, 0x24D3E0, 0x24DE80))
SUPPORT = inherited.SUPPORT[1:9]  # Complete OOM/refill/chunk/heap/memmove bodies.
RANGES = SELECTED + SUPPORT
KEYS = inherited.KEYS + ('entry', 'bucket_count', 'iterator_path')


class CloneTrace(inherited.ResolverTrace):
    """Local control scopes; inherited strict initialized decoder unchanged."""
    def __init__(self, original, parameters):
        super().__init__(original, parameters)
        self.invocations = [0] * len(RANGES)

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a, b in RANGES):
            raise ValueError('unreviewed clone instruction')
        off = pc - 0xFF000
        if off < 0 or off + 4 > len(self.original):
            raise ValueError('clone code image bounds')
        return struct.unpack_from('<I', self.original, off)[0]

    def run(self, entry, stop=RETURN, depth=0):
        # This complete control loop follows the reviewed ResolverTrace loop,
        # with local concrete scopes and a selected terminal-JR31 check.
        index = next((i for i, (a, b) in enumerate(RANGES) if a == entry), None)
        if index is None or depth > 14:
            raise ValueError('unreviewed clone entry/depth')
        lo, hi = RANGES[index]
        self.invocations[index] += 1
        pc = lo
        while True:
            if not lo <= pc < hi:
                raise ValueError('clone transfer outside complete body')
            w = self.fetch(pc)
            target, annul = self.execute(w, pc)
            if is_control_transfer(w):
                self.branches.append([pc, target is not None])
                if annul:
                    if target is not None:
                        raise ValueError('clone annul taken')
                    pc += 8
                    continue
                if pc + 4 >= hi:
                    raise ValueError('clone missing delay')
                delay = self.fetch(pc + 4)
                if is_control_transfer(delay) or self.execute(delay, pc + 4) != (None, False):
                    raise ValueError('clone control in delay')
                if w == 0x03E00008:
                    if target != stop or index < len(SELECTED) and pc != hi - 8:
                        raise ValueError('clone selected terminal JR31/stop')
                    return
                if w >> 26 == 3 or w >> 26 == 0 and w & 63 == 9:
                    continuation = self.r[31] & MASK
                    if target in [a for a, b in RANGES]:
                        self.run(target, continuation, depth + 1)
                    else:
                        self.external(target)
                    pc = continuation
                elif w >> 26 == 2 and target in [a for a, b in RANGES]:
                    self.run(target, stop, depth + 1)
                    return
                else:
                    pc = target if target is not None else pc + 8
            else:
                if target is not None or annul:
                    raise ValueError('clone unexpected transfer')
                pc += 4


def fixture(original, entry=0, size=4, capacity=8, position=2, count=2,
            alias_value=0, pool=0, fail_first=0, key=0x89ABCDEF,
            bucket_count=5, iterator_path=0):
    values = (entry, size, capacity, position, count, alias_value, pool,
              fail_first, key, bucket_count, iterator_path)
    if any(type(v) is not int for v in values) or not 0 <= entry < 14:
        raise ValueError('clone integral entry domain')
    if not 0 <= size <= capacity <= 40 or not 0 <= position <= size or not 0 <= count <= 40:
        raise ValueError('clone initialized vector domain')
    if alias_value not in (0, 1) or pool not in (0, 1, 2, 3) or fail_first not in (0, 1):
        raise ValueError('clone allocation/value domain')
    if not 0 <= key <= MASK or bucket_count not in (1, 2, 3, 5, 8, 16) or iterator_path not in (0, 1, 2):
        raise ValueError('clone nonzero bucket/key/path domain')
    routine = 3 if entry < 9 else 6
    p = dict(zip(KEYS, (routine, size, capacity, position, count, alias_value,
                       0, key, 3, 3, 0, pool, fail_first, 0, entry, bucket_count, iterator_path)))
    t = CloneTrace(original, p)
    for i in range(WORDS):
        t.save(BASE + 4*i, initial_word(i), 4)
    for a in range(STACK[0], STACK[1], 4):
        t.save(a, 0xA5030201, 4)
    for a, n in GLOBALS:
        for i in range(0, n, 4):
            t.save(a+i, 0, 4)
    for a in (inherited.REG, inherited.RANGE, inherited.RANGE+4, inherited.RANGE+8,
              START, FINISH, HANDLER, HEAP_GLOBAL, REENT_GLOBAL):
        t.pointer_cells.add(a)
    for i in range(16):
        t.pointer(HEADS+4*i, 0)
    t.pointer(HEAP_GLOBAL, HEAP)
    t.pointer(HANDLER, inherited.OOM_CALLBACK)
    t.save(HEAP+24, 0, 4)
    t.save(HEAP+36, 0x10000, 4)
    t.save(HEAP+40, 0, 4)
    for table in (TABLE, inherited.RESOLVER+4):
        t.pointer(table+4, BUCKETS)
        t.pointer(table+8, BUCKETS+bucket_count*4)
        t.pointer(table+12, BUCKETS+bucket_count*4)
        t.save(table+16, 3, 4)
    for i in range(193):
        t.pointer(BUCKETS+4*i, 0)
    for i, n in enumerate(NODES):
        t.pointer(n, NODES[1] if i == 0 and iterator_path == 0 else 0)
        t.save(n+4, key if i == 0 else i, 4)
        t.pointer(n+8, PAYLOADS[i] if i != 1 else 0)
    slot = key % bucket_count
    t.pointer(BUCKETS+slot*4, NODES[0])
    if iterator_path == 1 and slot + 1 < bucket_count:
        t.pointer(BUCKETS+(bucket_count-1)*4, NODES[2])
    for i in range(80):
        t.pointer(OLD+4*i, PAYLOADS[i % 3])
    t.pointer(VECTOR, OLD)
    t.pointer(VECTOR+4, OLD+size*4)
    t.pointer(VECTOR+8, OLD+capacity*4)
    t.pointer(VALUE, PAYLOADS[2])
    t.save(inherited.RESOLVER+24, 0, 4)
    t.pointer(ITERATOR, NODES[0])
    t.pointer(ITERATOR+4, TABLE)
    t.pointer(inherited.DESCRIPTOR+4, 0x4208A0)
    t.pointer(inherited.EXIT_SLOTS, inherited.DESCRIPTOR)
    t.pointer(inherited.RANGE, inherited.EXIT_SLOTS)
    t.pointer(inherited.RANGE+4, inherited.EXIT_SLOTS+4)
    t.pointer(inherited.RANGE+8, inherited.EXIT_SLOTS+16)
    t.pointer(inherited.REG, inherited.RESOLVER)
    # Initialized allocator-link carrier, not a claimed clone node object.
    t.pointer(NEW, 0)
    if pool in (1, 2):
        t.pointer(START, NEW)
        t.pointer(FINISH, NEW+(320 if pool == 1 else 24))
    elif pool == 3:
        n = (size + max(size, count))*4
        if not 0 < n <= 128:
            raise ValueError('clone preloaded free-list size domain')
        t.pointer(HEADS+(((n+7)//8)-1)*4, NEW)
    t.initial_memory = dict(t.memory)
    t.initial_pointer_cells = sorted(t.pointer_cells)
    args = (VECTOR, OLD+position*4, count, OLD if alias_value else VALUE) if entry < 9 else (ITERATOR,)
    t.r[29], t.r[31] = 0x80000, RETURN
    for r, v in enumerate(args, 4):
        t.r[r] = sx32(v)
    t.run(SELECTED[entry][0])
    if entry < 9:
        t.pointer_cells.update(range(t.load(VECTOR, 4), t.load(VECTOR+4, 4), 4))
    diffs = []
    for a in range(BASE, END, 4):
        old = int.from_bytes(bytes(t.initial_memory[a+i] for i in range(4)), 'little')
        now = t.load(a, 4)
        if now != old:
            diffs.append([a-BASE, now])
    return dict(parameters=p, differences=diffs, initial_pointer_cells=t.initial_pointer_cells,
                pointer_cells=sorted(t.pointer_cells), globals=[[a, t.load(a, 4)] for g, n in GLOBALS for a in range(g, g+n, 4)],
                events=t.events, return_value=t.r[2] & MASK if entry >= 9 else None,
                instructions=t.instruction_count, visited=sorted(t.visited), invocations=t.invocations,
                readonly_reads=sorted(t.readonly_reads), branches=t.branches)


def parameters():
    result = []
    for entry in range(9):
        for size, capacity in ((0, 0), (0, 8), (1, 1), (3, 4), (4, 8), (7, 8), (12, 40), (32, 40)):
            for position in sorted({0, size//2, size}):
                for count in (0, 1, 5, 12, 33):
                    for alias in ((0, 1) if size else (0,)):
                        result.append(dict(entry=entry, size=size, capacity=capacity, position=position,
                                           count=count, alias_value=alias))
        for pool in (0, 1, 2):
            for fail in (0, 1):
                result.append(dict(entry=entry, size=3, capacity=4, position=1, count=12,
                                   pool=pool, fail_first=fail))
        result.append(dict(entry=entry,size=3,capacity=4,position=1,count=33,fail_first=1))
        result.append(dict(entry=entry,size=3,capacity=4,position=1,count=12,pool=3))
    for entry in range(9, 14):
        for buckets in (1, 2, 3, 5, 8, 16):
            for key in (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x89ABCDEF):
                for path in (0, 1, 2):
                    result.append(dict(entry=entry, bucket_count=buckets, key=key, iterator_path=path))
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=ROOT/'build/sgi_container_duplicates/trace.json')
    args = parser.parse_args()
    _, original = validated_elf(ROOT/'orig/SLUS_216.68')
    inputs = parameters()
    observations = [fixture(original, **p) for p in inputs]
    report = dict(schema_version=1, fixtures=len(observations),
                  input_sha256=hashlib.sha256(json.dumps(inputs, sort_keys=True).encode()).hexdigest(),
                  total_instructions=sum(x['instructions'] for x in observations),
                  maximum_instructions=max(x['instructions'] for x in observations),
                  visited=sorted({pc for x in observations for pc in x['visited']}), observations=observations,
                  qualification='Actual fourteen original entry PCs; unchanged inherited initialized scalar/allocator decoder. Finite nonzero buckets/raw-u32 key counterparts, valid disjoint vector storage and caller views only; no original clone node size/class/pair/lifetime, hardware timing, malformed trap or helper/data award. Old493/native8094485 are inherited, not these counts.')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2)+'\n', newline='\n')
    print(report['fixtures'], report['total_instructions'], report['maximum_instructions'])


if __name__ == '__main__':
    main()
