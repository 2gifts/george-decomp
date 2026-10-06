"""Bounded scalar arena fixtures read from the locally hash-validated ELF.

Only the eleven reviewed complete bodies execute. The numeric memset callee has
an authored bounded zero-store/caller-clobber contract; it is not an MMI/heap/EE
emulator. Synthetic buffers, numeric wrap cases and explicit post-call mutations
test caller observations. No original instruction or data arrays are exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import Trace,RETURN

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20800
LOCAL,BASE,GLOBAL=0x20040,0x20440,0x46A0D8
RANGES=((0x2B8E00,0x2B8E84),(0x2B8E88,0x2B8EA8),(0x2B8EA8,0x2B8EE8),
        (0x2B8EE8,0x2B8EF0),(0x2B8EF0,0x2B8F24),(0x2B8F28,0x2B8FB4),
        (0x2B8FB8,0x2B9040),(0x2B9040,0x2B911C),(0x2B9120,0x2B9234),
        (0x2B9238,0x2B9388),(0x2B9388,0x2B9440))
ENTRIES=tuple(a for a,b in RANGES)
MEMORY_RANGES=((BUFFER,END),(GLOBAL,GLOBAL+24),(0x7F000,0x81000))
ZERO=0x3936A0


def signed32(value):
    value&=0xFFFFFFFF
    return value if value<0x80000000 else value-0x100000000


class ArenaTrace(CameraTrace):
    def __init__(self,original,mutation=0,outputs=(BUFFER,BUFFER+4,BUFFER+8)):
        super().__init__(original)
        self.arena_mutation=mutation;self.outputs=outputs
        self.lo=None;self.calls=0;self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed arena instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('arena instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def memory_check(self,address,size):
        if size not in (1,4,8,16) or not any(a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('arena memory outside authored windows')
        if size!=1 and address%size:raise ValueError('unaligned arena memory outside model')
        if any(address+i not in self.memory for i in range(size)):raise ValueError('uninitialized arena memory')

    def load(self,address,size):
        self.memory_check(address,size)
        return Trace.load(self,address,size)

    def save(self,address,value,size):
        self.memory_check(address,size)
        return Trace.save(self,address,value,size)

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,instruction>>21&31,instruction>>16&31,instruction>>11&31,instruction&63
        shamt=instruction>>6&31;imm=instruction&65535
        simm=imm-65536 if imm&32768 else imm
        target,annul=None,False
        if op==0 and fn in (0,4,0x12,0x18,0x23,0x2A,0x2B):
            if fn==0:
                if rs:raise ValueError('reserved arena SLL source')
                self.r[rd]=(self.r[rt]<<shamt)&0xFFFFFFFF
            else:
                if shamt:raise ValueError('reserved arena integer shift field')
                if fn==4:self.r[rd]=(self.r[rt]<<(self.r[rs]&31))&0xFFFFFFFF
                elif fn==0x12:
                    if rs or rt:raise ValueError('reserved arena MFLO source')
                    if self.lo is None:raise ValueError('arena MFLO before reviewed MULT')
                    self.r[rd]=self.lo
                elif fn==0x18:
                    # The observed R5900 MULT destination also writes rd.
                    self.lo=(signed32(self.r[rs])*signed32(self.r[rt]))&0xFFFFFFFF
                    self.r[rd]=self.lo
                elif fn==0x23:self.r[rd]=(self.r[rs]-self.r[rt])&0xFFFFFFFF
                elif fn==0x2A:self.r[rd]=int(signed32(self.r[rs])<signed32(self.r[rt]))
                else:self.r[rd]=int((self.r[rs]&0xFFFFFFFF)<(self.r[rt]&0xFFFFFFFF))
        elif op in (0x14,0x15):
            taken=self.r[rs]==self.r[rt]
            if op==0x15:taken=not taken
            if taken:target=(pc+4+simm*4)&0xFFFFFFFF
            else:annul=True
        elif op==0xC:self.r[rt]=self.r[rs]&imm
        else:
            allowed=op in (3,4,5,9,0xF,0x23,0x37,0x1E,0x2B,0x3F,0x1F) or op==0 and fn in (8,0x21,0x24,0x2D)
            if not allowed or op==0xF and rs:raise ValueError('unsupported arena opcode or operand')
            return super().execute(instruction,pc)
        self.r[0]=0;self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('arena instruction bound exceeded')
        return target,annul

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported arena entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body arena transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unsupported arena return')
            branch=op in (4,5,20,21)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('arena delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('arena transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif op==3:
                    if target==ENTRIES[3]:self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed arena transfer')
                    pc=target
            else:pc+=8 if annul else 4

    def library_call(self,target):
        if target!=ZERO:raise ValueError('unknown arena controlled call')
        pointer,value,length=(self.r[i]&0xFFFFFFFF for i in (4,5,6))
        if value!=0 or length>36 or not BUFFER<=pointer<=END-length:
            raise ValueError('arena zero call outside authored contract')
        self.calls+=1
        self.events.extend((pointer,value,length,*[self.load(GLOBAL+i*4,4) for i in range(4)],
                            *[self.load(p,4) for p in self.outputs]))
        for i in range(length):self.save(pointer+i,0,1)
        if self.arena_mutation==1:
            self.save(GLOBAL,BASE+128,4);self.save(GLOBAL+8,BASE+132,4)
            self.save(GLOBAL+12,BASE+136,4);self.save(self.outputs[0],BASE+140,4)
        elif self.arena_mutation==2:
            self.save(self.outputs[1],BASE+144,4);self.save(self.outputs[2],BASE+148,4)
        # Normal ABI clobbers; subsequent calls retain bases/offsets in saved
        # registers in retail, and in ordinary C locals in the reconstruction.
        for i in range(4,16):self.r[i]=0xDEADBEEF
        self.r[2]=pointer


def make_fixture(original,routine=0,argument=12,exponent=4,mutation=0,alias=0,
                 base=BASE,cursor=BASE+17,capacity=512,high=BASE,descriptor_global=0,raw_addresses=0):
    outputs=((BUFFER,BUFFER+4,BUFFER+8),(BUFFER,BUFFER,BUFFER),
             (GLOBAL+8,GLOBAL+12,GLOBAL),(GLOBAL,GLOBAL+4,GLOBAL+8),
             (base+(argument*12&0xFFFFFFFF),BUFFER+4,BUFFER+8),
             (LOCAL,LOCAL+8,LOCAL+12),(BUFFER+8,BUFFER+4,BUFFER))[alias]
    outputs=tuple(v&0xFFFFFFFF for v in outputs)
    t=ArenaTrace(original,mutation,outputs)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0x5A
    initial_descriptor=(base,capacity,cursor,high,ENTRIES[1],BUFFER+32)
    for p in (LOCAL,GLOBAL):
        for i,value in enumerate(initial_descriptor):t.save(p+i*4,value,4)
    initial=[t.load(BUFFER+i*4,4) for i in range(512)]
    global_initial=[t.load(GLOBAL+i*4,4) for i in range(6)]
    t.r[29],t.r[31]=0x80000,RETURN
    descriptor=GLOBAL if descriptor_global else LOCAL
    if routine==0:t.r[4],t.r[5],t.r[6]=descriptor,argument,exponent
    elif routine==1:t.r[4]=descriptor
    elif routine==2:t.r[4],t.r[5]=base,capacity
    elif routine==4:t.r[4]=exponent
    else:
        t.r[4],t.r[5]=argument,exponent
        for i,p in enumerate(outputs):t.r[6+i]=p
    t.run(ENTRIES[routine])
    return dict(routine=routine,argument=argument&0xFFFFFFFF,exponent=exponent&0xFFFFFFFF,mutation=mutation,alias=alias,
                descriptor_global=descriptor_global,raw_addresses=raw_addresses,outputs=outputs,initial=initial,
                global_initial=global_initial,expected=[t.load(BUFFER+i*4,4) for i in range(512)],
                global_expected=[t.load(GLOBAL+i*4,4) for i in range(6)],
                result=t.r[2]&0xFFFFFFFF if routine in (0,5,6) else 0,
                calls=t.calls,events=t.events,instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for routine in (0,5,6):
        for exponent in (0,1,4,5,32,36,63):
            for argument in (0,12,32,64):
                for capacity in (64,512):cases.append(make_fixture(original,routine,argument,exponent,capacity=capacity))
        for base,cursor,high in ((BASE+1,BASE+1,BASE+64),(BASE+7,BASE+511,BASE),(BASE,BASE+16,BASE+96)):
            cases.append(make_fixture(original,routine,16,4,base=base,cursor=cursor,high=high,capacity=32))
    for routine in (0,1):
        for descriptor_global in (0,1):
            for cursor,high in ((BASE,BASE),(BASE+32,BASE),(BASE+16,BASE+32)):
                cases.append(make_fixture(original,routine,descriptor_global=descriptor_global,cursor=cursor,high=high))
    for routine in (2,3,4):
        for exponent in (0,4,31,32,63):cases.append(make_fixture(original,routine,exponent=exponent,base=BASE+1))
    for routine in (7,8,9,10):
        for argument in (0,1,3,4,5,7,8,0x7FFFFFFF,0x80000000,0x80000001,0xFFFFFFFD,0xFFFFFFFF):
            for exponent in (0,4,5):cases.append(make_fixture(original,routine,argument,exponent))
        for alias in (1,2,3,5,6):
            for mutation in (0,1,2):cases.append(make_fixture(original,routine,3,4,alias=alias,mutation=mutation))
        for mutation in (0,1,2):
            cases.append(make_fixture(original,routine,3,0,alias=4,mutation=mutation,base=BASE,cursor=BASE))
        for capacity in (0,48,96):cases.append(make_fixture(original,routine,4,4,capacity=capacity))
    for routine in (0,1,4,5,6):
        for base,cursor,capacity,high in ((0xFFFFFFE0,0xFFFFFFF8,64,0xFFFFFFE0),
                                        (0x80000000,0x7FFFFFFF,32,0x80000010),
                                        (0,0xFFFFFFF0,0xFFFFFFFF,0),
                                        (0xFFFFFFF0,0,32,0xFFFFFFF4)):
            cases.append(make_fixture(original,routine,32,4,base=base,cursor=cursor,capacity=capacity,high=high,raw_addresses=1))
    return cases


def input_hash(cases):
    keys=('routine','argument','exponent','mutation','alias','descriptor_global','raw_addresses','outputs','initial','global_initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    lines=['/* Authored arena buffers/events only; no original code/data arrays. */',
           'struct ArenaGolden { u32 routine,argument,exponent,mutation,alias,descriptor_global,raw_addresses,outputs[3],initial[512],global_initial[6],expected[512],global_expected[6],result,calls,event_count,events[30]; };',
           'static const struct ArenaGolden arena_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('%du'%c[k] for k in ('routine','argument','exponent','mutation','alias','descriptor_global','raw_addresses'))
        lines.append('    {%s,{%s},{%s},{%s},{%s},{%s},0x%08Xu,%du,%du,{%s}},'%(prefix,array('outputs'),array('initial'),array('global_initial'),array('expected'),array('global_expected'),c['result'],c['calls'],len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/arena_buffers_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded arena fixtures; %d original instructions; maximum %d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
