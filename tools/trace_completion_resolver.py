"""Strict initialized observations of complete resolver/container originals.

Actual selected instructions, allocator/refill/chunk/memmove/heap wrappers and
formatter wrapper execute. Allocation/release/format engines and one OOM
callback remain explicit supporting contracts. This is no general EE emulator,
class identity, heap implementation, format-capacity or original data award.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer
from trace_string_registry import RegistryTrace, sx32
from trace_buffer_completion import CompletionTrace

ROOT = Path(__file__).resolve().parents[1]
MASK = 0xFFFFFFFF
BASE, WORDS = 0x20000, 16384
END = BASE + WORDS * 4
RESOLVER, TABLE, VECTOR = 0x20100, 0x20540, 0x20600
BUCKETS, OLD, NEW = 0x20700, 0x20B00, 0x26000
NODES = (0x21100, 0x21120, 0x21140)
PAYLOADS = (0x21200, 0x21220, 0x21240)
DESCRIPTOR, ITERATOR, EXIT_SLOTS, VALUE, HEAP = 0x21300, 0x21400, 0x21500, 0x21600, 0x22C00
REG, SHUTDOWN, RANGE = 0x3F2C44, 0x3F2C9C, 0x46A0F0
HEADS, START, FINISH, HEAP_SIZE, HANDLER = 0x3F21B8, 0x3F21F8, 0x3F21FC, 0x3F2200, 0x3F21B0
HEAP_GLOBAL, REENT_GLOBAL = 0x3FD204, 0x405694
OOM_CALLBACK = 0xF0000010
STACK = (0x7D000, 0x81000)
SELECTED = ((0x2BF550,0x2BF580), (0x2BF580,0x2BF5E0), (0x2BF748,0x2BF964),
            (0x2BF0F0,0x2BF414), (0x2BEDF0,0x2BEF40), (0x2BF418,0x2BF4B4),
            (0x2BF4B8,0x2BF54C), (0x104B10,0x104B90), (0x2BD340,0x2BD388))
SUPPORT = ((0x252110,0x252164), (0x252168,0x2521F8), (0x2521F8,0x252294),
           (0x1005C8,0x1007DC), (0x2AF100,0x2AF120), (0x2AF120,0x2AF140),
           (0x2AF140,0x2AF1E8), (0x2AF1E8,0x2AF204), (0x3935A4,0x3936A0),
           (0x3952C8,0x395348))
RANGES = SELECTED + SUPPORT
GLOBALS = ((REG,4), (SHUTDOWN,4), (RANGE,12), (HEADS,64), (START,4),
           (FINISH,4), (HEAP_SIZE,4), (HANDLER,4), (HEAP_GLOBAL,4), (REENT_GLOBAL,4))
READONLY = ((0x4208A0,0x4208B0), (0x4200F8,0x420108), (0x447F48,0x447F63),
            (0x447F78,0x448058), (0x447238,0x447264))
initial_word = lambda i: (0xA5870301 ^ (i * 0x10103)) & MASK
KEYS = ('routine','size','capacity','position','count','alias_value','slot',
        'key','nodes','flags','mutation','pool','fail_first','exit_null')


class ResolverTrace(RegistryTrace):
    def __init__(self, original, parameters):
        Trace.__init__(self, original)
        self.p = parameters
        self.visited, self.pointer_cells, self.readonly_reads = set(), set(), set()
        self.events, self.branches = [], []
        self.invocations = [0] * len(RANGES)
        self.allocations = 0
        self.pc = None

    def memory_check(self, a, n):
        if n not in (1,2,4,8,16) or a % n or not (
                BASE <= a < a+n <= END or STACK[0] <= a < a+n <= STACK[1] or
                any(g <= a < a+n <= g+s for g,s in GLOBALS)):
            raise ValueError('unowned or unaligned resolver memory %#x/%d' % (a,n))

    def load(self, a, n):
        if any(lo <= a < a+n <= hi for lo,hi in READONLY):
            if a % n:
                raise ValueError('unaligned resolver readonly')
            self.readonly_reads.add((a,n))
            off = a - 0xFF000
            if off < 0 or off+n > len(self.original):
                raise ValueError('resolver readonly image bounds')
            return int.from_bytes(self.original[off:off+n], 'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):
            raise ValueError('uninitialized resolver memory')
        return Trace.load(self,a,n)

    def save(self, a, v, n):
        self.memory_check(a,n)
        if n == 4 and self.pc in (0x252260,0x25227C,0x1006C4):
            # Actual refill-node and leftover-chunk link stores, not value heuristics.
            self.pointer_cells.add(a)
        Trace.save(self,a,v,n)

    def pointer(self, a, v):
        self.pointer_cells.add(a)
        self.save(a,v,4)

    def fetch(self, pc):
        if pc & 3 or not any(a <= pc < b for a,b in RANGES):
            raise ValueError('unreviewed resolver instruction')
        off = pc - 0xFF000
        if off < 0 or off+4 > len(self.original):
            raise ValueError('resolver code image bounds')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self, w, pc):
        self.pc = pc
        return CompletionTrace.execute(self,w,pc)

    def text(self, a):
        result = bytearray()
        for i in range(96):
            b = self.load(a+i,1)
            if not b:
                return bytes(result)
            result.append(b)
        raise ValueError('resolver text observation bounds')

    def event(self, kind, a=0, b=0, c=0):
        if len(self.events) >= 80:
            raise ValueError('resolver event bound')
        self.events.append([kind,a&MASK,b&MASK,c&MASK])

    def external(self, target):
        a,b,c = (self.r[i]&MASK for i in (4,5,6))
        if target == 0x2ADF60:
            if a != HEAP or c != 3 or not 0 < b <= 0x1400:
                raise ValueError('unreviewed resolver allocation contract %#x/%#x/%#x' % (a,b,c))
            self.allocations += 1
            if self.allocations > 3:
                raise ValueError('resolver allocation attempt bound')
            failed = self.p['fail_first'] and self.allocations == 1
            value = 0 if failed else NEW + (self.allocations-1)*0x2000
            self.event(1,a,b,value)
            self.r[2] = value
            if self.p['mutation'] & 1 and self.p['routine'] in (0,2):
                table = RESOLVER+4 if self.p['routine']==0 else TABLE
                self.pointer(table+4,OLD)
                self.pointer(table+8,OLD+8)
                self.pointer(table+12,OLD+16)
        elif target == 0x2AE158:
            if not BASE <= a < END:
                raise ValueError('unreviewed resolver release contract')
            self.event(2,a,0,0)
            if self.p['mutation'] & 2 and a in PAYLOADS:
                self.pointer(NODES[0],0)
            if self.p['mutation'] & 4:
                self.pointer(REG,0)
        elif target == 0x394F68:
            if a != 0x447238 or b != 0 or self.r[7]&MASK != 3:
                raise ValueError('unreviewed resolver diagnostic contract')
            self.event(3,a,c,3)
            self.r[2] = 0
        elif target == OOM_CALLBACK:
            self.event(4,0,0,0)
            self.r[2] = 0
        elif target == 0x398F98:
            fmt = self.text(b)
            if fmt != b'0x%08x(reverse lookup n/a)' or self.load(a+12,2) != 0x208:
                raise ValueError('whole resolver formatter contract')
            if self.load(a+8,4) != 0x7FFFFFFF or self.load(a+20,4) != 0x7FFFFFFF:
                raise ValueError('actual resolver formatter wrapper fields')
            key = self.load(c,8)&MASK
            destination = self.load(a,4)
            self.event(5,destination,key,0)
            rendered = fmt.replace(b'%08x',('%08x'%key).encode())
            for i,v in enumerate(rendered):
                self.save(destination+i,v,1)
            self.save(a,destination+len(rendered),4)
            self.r[2] = len(rendered)
            if self.p['mutation'] & 8:
                self.save(RESOLVER+24,15,4)
        else:
            raise ValueError('unreviewed resolver external %#x' % target)

    def run(self, entry, stop=RETURN, depth=0):
        index = next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth > 14:
            raise ValueError('unreviewed resolver entry/depth')
        lo,hi = RANGES[index]
        self.invocations[index] += 1
        pc = lo
        while True:
            if not lo <= pc < hi:
                raise ValueError('resolver transfer outside complete body')
            w = self.fetch(pc)
            target,annul = self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:
                        raise ValueError('resolver annul taken')
                    pc += 8
                    continue
                if pc+4 >= hi:
                    raise ValueError('resolver missing delay')
                delay = self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                    raise ValueError('resolver control in delay')
                if w == 0x03E00008:
                    if target != stop:
                        raise ValueError('resolver JR31/stop')
                    return
                if w>>26==3 or w>>26==0 and w&63==9:
                    continuation = self.r[31]&MASK
                    if target in [a for a,b in RANGES]:
                        self.run(target,continuation,depth+1)
                    else:
                        self.external(target)
                    pc = continuation
                elif w>>26==2 and target in [a for a,b in RANGES]:
                    self.run(target,stop,depth+1)
                    return
                else:
                    pc = target if target is not None else pc+8
            else:
                if target is not None or annul:
                    raise ValueError('resolver unexpected transfer')
                pc += 4


def fixture(original, routine=3, size=4, capacity=8, position=2, count=2,
            alias_value=0, slot=0, key=0x89ABCDEF, nodes=3, flags=3,
            mutation=0, pool=0, fail_first=0, exit_null=0):
    p = dict(zip(KEYS,(routine,size,capacity,position,count,alias_value,slot,key,
                       nodes,flags,mutation,pool,fail_first,exit_null)))
    if any(type(v) is not int for v in p.values()) or routine not in range(9):
        raise ValueError('resolver fixture integral/routine domain')
    if not 0 <= size <= capacity <= 40 or not 0 <= position <= size or not 0 <= count <= 40:
        raise ValueError('resolver initialized vector domain')
    if alias_value not in (0,1) or not 0 <= slot < 16 or not 0 <= key <= MASK or nodes not in range(4):
        raise ValueError('resolver value/format/node domain')
    if not 0 <= flags <= MASK or not 0 <= mutation < 16 or pool not in range(3) or fail_first not in (0,1) or exit_null not in (0,1):
        raise ValueError('resolver controlled contract domain')
    t = ResolverTrace(original,p)
    for i in range(WORDS):
        t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):
        t.save(a,0xA5030201,4)
    for a,n in GLOBALS:
        for i in range(0,n,4):
            t.save(a+i,0,4)
    for a in (REG,RANGE,RANGE+4,RANGE+8,START,FINISH,HANDLER,HEAP_GLOBAL,REENT_GLOBAL):
        t.pointer_cells.add(a)
    for i in range(16):
        t.pointer(HEADS+4*i,0)
    t.pointer(HEAP_GLOBAL,HEAP)
    t.pointer(HANDLER,OOM_CALLBACK)
    t.save(HEAP+24,0,4)
    t.save(HEAP+36,0x10000,4)
    t.save(HEAP+40,0,4)
    table = RESOLVER+4 if routine in (0,1,4,7,8) else TABLE
    for table_address in (TABLE,RESOLVER+4):
        t.pointer(table_address+4,BUCKETS)
        t.pointer(table_address+8,BUCKETS+20)
        t.pointer(table_address+12,BUCKETS+20)
        t.save(table_address+16,nodes,4)
    for i in range(193):
        t.pointer(BUCKETS+4*i,0)
    for i,n in enumerate(NODES):
        t.pointer(n,NODES[i+1] if i+1<nodes else 0)
        t.save(n+4,1 if i<2 else 4,4)
        t.pointer(n+8,PAYLOADS[i] if i != 1 else 0)
    if nodes:
        t.pointer(BUCKETS+4,NODES[0])
    if nodes==3:
        t.pointer(NODES[1],0)
        t.pointer(BUCKETS+16,NODES[2])
    for i in range(80):
        t.pointer(OLD+4*i,PAYLOADS[i%3])
    t.pointer(VECTOR,OLD)
    t.pointer(VECTOR+4,OLD+size*4)
    t.pointer(VECTOR+8,OLD+capacity*4)
    t.pointer(VALUE,PAYLOADS[2])
    t.save(RESOLVER+24,slot,4)
    t.pointer(ITERATOR,NODES[0])
    t.pointer(ITERATOR+4,table)
    t.pointer(DESCRIPTOR+4,0x4208A0)
    t.pointer(EXIT_SLOTS,0 if exit_null else DESCRIPTOR)
    t.pointer(RANGE,EXIT_SLOTS)
    t.pointer(RANGE+4,EXIT_SLOTS+4)
    t.pointer(RANGE+8,EXIT_SLOTS+16)
    t.pointer(REG,RESOLVER if nodes else 0)
    if pool:
        t.pointer(START,NEW)
        t.pointer(FINISH,NEW+(320 if pool==1 else 24))
    t.initial_memory = dict(t.memory)
    t.initial_pointer_cells = sorted(t.pointer_cells)
    args = {0:(RESOLVER,),1:(RESOLVER,key),2:(TABLE,),
            3:(VECTOR,OLD+position*4,count,OLD if alias_value else VALUE),
            4:(RESOLVER,flags),5:(TABLE,),6:(ITERATOR,),7:(DESCRIPTOR,flags),8:()}
    t.r[29],t.r[31] = 0x80000,RETURN
    for r,v in enumerate(args[routine],4):
        t.r[r] = sx32(v)
    t.run(SELECTED[routine][0])
    # Pointer-array outputs are genuine initialized elements only. Capacity
    # padding remains numerical initialized scratch, not an invented pointer.
    if routine in (0,2):
        t.pointer_cells.update(range(t.load(table+4,4),t.load(table+8,4),4))
    if routine==3:
        t.pointer_cells.update(range(t.load(VECTOR,4),t.load(VECTOR+4,4),4))
    diffs = []
    for a in range(BASE,END,4):
        old = int.from_bytes(bytes(t.initial_memory[a+i] for i in range(4)),'little')
        now = t.load(a,4)
        if now != old:
            diffs.append([a-BASE,now])
    global_values = [[a,t.load(a,4)] for g,n in GLOBALS for a in range(g,g+n,4)]
    return dict(parameters=p, differences=diffs, initial_pointer_cells=t.initial_pointer_cells, pointer_cells=sorted(t.pointer_cells),
                globals=global_values, events=t.events,
                return_value=t.r[2]&MASK if routine in (0,1,2,6) else None,
                instructions=t.instruction_count, visited=sorted(t.visited),
                invocations=t.invocations, readonly_reads=sorted(t.readonly_reads))


def parameters():
    result=[]
    for routine in (0,2):
        for mutation in (0,1):
            for fail in (0,1):
                result.append(dict(routine=routine,mutation=mutation,fail_first=fail))
    for slot in (0,1,7,15):
        for key in (0,1,0x80000000,0xFFFFFFFF,0x89ABCDEF):
            for mutation in (0,8):
                result.append(dict(routine=1,slot=slot,key=key,mutation=mutation))
    for size,cap in ((0,0),(0,8),(1,1),(3,4),(4,8),(7,8),(12,40),(32,40)):
        for position in sorted({0,size//2,size}):
            for count in (0,1,2,5,12,33):
                for alias in ((0,1) if size else (0,)):
                    result.append(dict(routine=3,size=size,capacity=cap,position=position,
                                       count=count,alias_value=alias))
    for nodes in range(4):
        for flags in (0,1,2,3,0x80000000,0xFFFFFFFF):
            for mutation in (0,2,4,6):
                result.append(dict(routine=4,nodes=nodes,flags=flags,mutation=mutation))
    for routine in (5,6):
        for nodes in (1,2,3):
            result.append(dict(routine=routine,nodes=nodes))
    for routine in (7,8):
        for nodes in range(4):
            for flags in (0,1,2,3,0xFFFFFFFF):
                for mutation in (0,2,4):
                    result.append(dict(routine=routine,nodes=nodes,flags=flags,mutation=mutation))
    for pool in (0,1,2):
        for fail in (0,1):
            result.append(dict(routine=3,size=3,capacity=4,position=1,count=12,pool=pool,fail_first=fail))
    result.append(dict(routine=8,exit_null=1))
    return result


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--output',type=Path,default=ROOT/'build/completion_resolver/trace.json')
    args=parser.parse_args()
    _,original=validated_elf(ROOT/'orig/SLUS_216.68')
    inputs=parameters()
    observations=[]
    for i,p in enumerate(inputs):
        try:
            observations.append(fixture(original,**p))
        except Exception as error:
            raise ValueError('resolver fixture %d %r' % (i,p)) from error
    visited=sorted({pc for row in observations for pc in row['visited']})
    report=dict(schema_version=1,fixtures=len(observations),
                input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),
                total_instructions=sum(x['instructions'] for x in observations),
                maximum_instructions=max(x['instructions'] for x in observations),
                visited=visited,observations=observations,
                qualification='Initialized finite valid object/storage graph, complete actual original execution plus disclosed controlled engines. No uninitialized prior byte, corrupt C++ object alias, format capacity, EE timing/FCR or helper/data award.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(report,indent=2)+'\n')
    print(report['fixtures'],report['total_instructions'],report['maximum_instructions'])

if __name__=='__main__':
    main()
