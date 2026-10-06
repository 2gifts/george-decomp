"""Bounded render-record/arena ownership fixtures from the validated ELF.

All five selected bodies plus two published arena helpers execute. Heap calls
are authored allocation/free observation contracts with explicit mutations and
nested recovered calls; they do not execute the heap graph. COP1 is restricted
to finite raw scalar moves/loads/stores, not arithmetic or hardware emulation.
Only synthetic memory and events are exported.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_arena_buffers import ArenaTrace
from trace_geometry import Trace,RETURN,scalar

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END,RECORD,SECOND=0x20000,0x20400,0x20040,0x20100
OWNER,CAPACITY,ARENA=0x3FD3E0,0x3FD3E4,0x46A0D8
RANGES=((0x2B8A68,0x2B8AF0),(0x2B8AF0,0x2B8B48),(0x2B8B68,0x2B8BA8),
        (0x2B8BA8,0x2B8BDC),(0x2B8BE0,0x2B8BFC))
HELPERS=((0x2B8EA8,0x2B8EE8),(0x2B8EE8,0x2B8EF0))
ENTRIES=tuple(a for a,b in RANGES)
ALLOCATE,FREE=0x2AEC28,0x2AEE40
RESET=0x2B8E88
MEMORY_RANGES=((BUFFER,END),(OWNER,CAPACITY+4),(ARENA,ARENA+24),(0x7F000,0x81000))


class OwnershipTrace(ArenaTrace):
    def __init__(self,original,allocation=RECORD,mutation=0):
        super().__init__(original)
        self.fp=[0x3E800000+i for i in range(32)]
        self.allocation=allocation;self.mutation=mutation
        self.counts=[0,0];self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+HELPERS):raise ValueError('unreviewed ownership instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('ownership instruction outside image')
        return struct.unpack_from('<I',self.original,offset)[0]

    def memory_check(self,address,size):
        if size not in (1,4,8,16) or not any(a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('ownership memory outside authored windows')
        if size!=1 and address%size:raise ValueError('unaligned ownership memory')
        if any(address+i not in self.memory for i in range(size)):raise ValueError('uninitialized ownership memory')

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,instruction>>21&31,instruction>>16&31,instruction>>11&31,instruction&63
        simm=instruction&65535;simm-=65536 if simm&32768 else 0
        custom=True
        if op==0xD:self.r[rt]=(self.r[rs]|(instruction&65535))&0xFFFFFFFF
        elif op==0x31:
            value=self.load((self.r[rs]+simm)&0xFFFFFFFF,4)
            if not math.isfinite(scalar(value)):raise ValueError('ownership scalar outside finite move model')
            self.fp[rt]=value
        elif op==0x39:
            if not math.isfinite(scalar(self.fp[rt])):raise ValueError('ownership scalar outside finite move model')
            self.save((self.r[rs]+simm)&0xFFFFFFFF,self.fp[rt],4)
        elif op==0x11 and rs==4:
            if instruction&0x7FF:raise ValueError('reserved ownership MTC1 operands')
            value=self.r[rt]&0xFFFFFFFF
            if not math.isfinite(scalar(value)):raise ValueError('ownership scalar outside finite move model')
            self.fp[rd]=value
        elif op==0x11 and rs==16 and fn==6:
            if rt:raise ValueError('reserved ownership MOV.S operands')
            if not math.isfinite(scalar(self.fp[rd])):raise ValueError('ownership scalar outside finite move model')
            self.fp[instruction>>6&31]=self.fp[rd]
        else:custom=False
        if not custom:return super().execute(instruction,pc)
        self.r[0]=0;self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('ownership instruction bound exceeded')
        return None,False

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES+HELPERS if r[0]==entry),None)
        if body is None:raise ValueError('unsupported ownership entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body ownership transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc);op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):raise ValueError('unsupported ownership return')
            branch=op in (4,5,20,21)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('ownership delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('ownership transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif op==3:
                    if target in (ENTRIES[1],HELPERS[0][0],HELPERS[1][0]):self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed ownership transfer')
                    pc=target
            else:pc+=8 if annul else 4

    def nested(self,entry):
        registers,fp=self.r[:],self.fp[:]
        self.r[29]-=0x100;self.r[31]=RETURN
        self.run(entry)
        self.r,self.fp=registers,fp

    def library_call(self,target):
        if target not in (ALLOCATE,FREE):raise ValueError('unknown ownership controlled call')
        if target==ALLOCATE and self.allocation!=0 and (self.allocation%4 or not BUFFER<=self.allocation<=END-48):
            raise ValueError('allocation result outside authored contract')
        argument=self.r[4]&0xFFFFFFFF;kind=int(target==FREE)
        self.counts[kind]+=1
        self.events.extend((kind,argument,self.load(OWNER,4),self.load(CAPACITY,4),
                            *[self.load(ARENA+i*4,4) for i in range(6)]))
        if target==ALLOCATE:
            if self.mutation==1:
                self.save(CAPACITY,argument+17&0xFFFFFFFF,4);self.save(OWNER,SECOND,4)
            elif self.mutation==2:
                self.save(RECORD+0x2C,0x89ABCDEF,4);self.save(RECORD+0x20,SECOND,4)
                self.save(CAPACITY,0x80000001,4)
            elif self.mutation==3:
                for i in range(6):self.save(ARENA+i*4,0x11000000+i,4)
                self.save(CAPACITY,0xFFFFFFFF,4)
            elif self.mutation==4:self.nested(ENTRIES[4]);self.save(CAPACITY,19,4)
            elif self.mutation==5:self.nested(ENTRIES[3])
            result=self.allocation
        else:
            if self.mutation==1:self.save(OWNER,SECOND,4);self.save(CAPACITY,99,4)
            elif self.mutation==2:self.save(ARENA+8,SECOND+4,4);self.save(ARENA+12,SECOND+8,4)
            elif self.mutation==3 and BUFFER<=argument<=END-4:self.save(argument,0x12345678,4)
            result=0xAABBCCDD
        for i in range(4,16):self.r[i]=0xDEADBEEF
        for i in range(20):self.fp[i]=0x3F400000
        self.r[2]=result


def make_fixture(original,routine=0,argument20=0x12345678,argument24=0x89ABCDEF,
                 floats=(0,0,0),allocation=RECORD,mutation=0,record=RECORD,
                 capacity=0x25800,owner=SECOND):
    t=OwnershipTrace(original,allocation,mutation)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0x5A
    t.save(OWNER,owner,4);t.save(CAPACITY,capacity,4)
    for i,v in enumerate((SECOND,256,SECOND+4,SECOND+8,RESET,RECORD)):t.save(ARENA+i*4,v,4)
    initial=[t.load(BUFFER+i*4,4) for i in range(256)]
    global_initial=[t.load(OWNER,4),t.load(CAPACITY,4)]
    arena_initial=[t.load(ARENA+i*4,4) for i in range(6)]
    t.r[29],t.r[31]=0x80000,RETURN
    t.fp[12:15]=floats
    if routine==0:t.r[4],t.r[5]=argument20,argument24
    elif routine==1:t.r[4],t.r[5],t.r[6]=record,argument20,argument24
    elif routine==2:t.r[4]=capacity
    elif routine not in (3,4):raise ValueError('unsupported ownership fixture')
    t.run(ENTRIES[routine])
    return dict(routine=routine,argument20=argument20,argument24=argument24,floats=floats,
                allocation=allocation,mutation=mutation,record=record,capacity=capacity,owner=owner,
                initial=initial,global_initial=global_initial,arena_initial=arena_initial,
                expected=[t.load(BUFFER+i*4,4) for i in range(256)],
                global_expected=[t.load(OWNER,4),t.load(CAPACITY,4)],
                arena_expected=[t.load(ARENA+i*4,4) for i in range(6)],
                result=t.r[2]&0xFFFFFFFF if routine==0 else 0,counts=t.counts,events=t.events,
                instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    values=((0,0,0),(0x80000000,0,0x80000000),(0x3F800000,0xC0000000,0x40400000),
            (1,0x007FFFFF,0x80000001),(0x7F7FFFFF,0xFF7FFFFF,0x00800000),
            (0x3F123456,0xBE654321,0x4B000001))
    arguments=((0,0),(0xFFFFFFFF,0x80000000),(RECORD,RECORD+0x2C),(RECORD+4,SECOND))
    for routine in (0,1):
        for floats in values:
            for a,b in arguments:
                for offset in (0,4,16,64):
                    if routine==1:cases.append(make_fixture(original,routine,a,b,floats,record=RECORD+offset))
                    else:
                        for mutation in range(6):cases.append(make_fixture(original,routine,a,b,floats,allocation=RECORD+offset,mutation=mutation))
        if routine==0:
            for floats in values:
                for mutation in range(6):cases.append(make_fixture(original,routine,floats=floats,allocation=0,mutation=mutation))
    for capacity in (0,1,48,120,0x25800,0x80000000,0xFFFFFFFF):
        for allocation in (0,RECORD,SECOND):
            for mutation in range(6):cases.append(make_fixture(original,2,capacity=capacity,allocation=allocation,mutation=mutation))
    for owner in (0,RECORD,SECOND,0xDEADBEEF):
        for capacity in (0,48,0xFFFFFFFF):
            for mutation in range(4):cases.append(make_fixture(original,3,owner=owner,capacity=capacity,mutation=mutation))
    for capacity in (0,1,48,0xFFFFFFFF):cases.append(make_fixture(original,4,capacity=capacity))
    return cases


def input_hash(cases):
    keys=('routine','argument20','argument24','floats','allocation','mutation','record','capacity','owner','initial','global_initial','arena_initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    lines=['/* Lossless authored record/arena fixtures; no original code/data arrays. */',
           'struct OwnershipGolden { u32 routine,argument20,argument24,floats[3],allocation,mutation,record,capacity,owner,global_initial[2],arena_initial[6],change_count,changes[16][2],global_expected[2],arena_expected[6],result,counts[2],event_count,events[20]; };',
           'static const struct OwnershipGolden ownership_golden[] = {']
    for c in cases:
        assert c['initial']==[0x5A5A5A5A]*256
        changes=[(i,v) for i,v in enumerate(c['expected']) if v!=0x5A5A5A5A];assert len(changes)<=16
        array=lambda k:','.join('0x%08Xu'%v for v in c[k])
        patch=','.join('{%du,0x%08Xu}'%(i,v) for i,v in changes) or '{0u,0u}'
        lines.append('    {%du,0x%08Xu,0x%08Xu,{%s},0x%08Xu,%du,0x%08Xu,0x%08Xu,0x%08Xu,{%s},{%s},%du,{%s},{%s},{%s},0x%08Xu,{%s},%du,{%s}},'%(c['routine'],c['argument20'],c['argument24'],array('floats'),c['allocation'],c['mutation'],c['record'],c['capacity'],c['owner'],array('global_initial'),array('arena_initial'),len(changes),patch,array('global_expected'),array('arena_expected'),c['result'],array('counts'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68');p.add_argument('--output',type=Path,default=ROOT/'build/arena_ownership_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d ownership fixtures; %d original instructions; maximum %d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)));print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
