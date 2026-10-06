"""Bounded resource-base fixtures with genuine lookup and slot-pool bodies.

Eight complete resource entries and three existing helper bodies execute from
the locally hash-validated ELF. Other manager/heap/virtual calls have authored
observation contracts; callback mutation tests include real nested subscription
and removal. This is a low32 integer model, not a full EE/heap/runtime emulator.
No original code or table arrays are exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer
from trace_camera_motion import CameraTrace

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x22800
WORDS=(END-BUFFER)//4
ALLOCATORS=(0x20000,0x21000)
MANAGERS=(0x22000,0x22200)
RECORD,OTHER,TABLE=0x22400,0x22440,0x22480
POOLS=(0x22500,0x22520)
MAP,BUCKETS,MAP_NODES=0x22540,0x22560,(0x22580,0x225A0)
NODES=tuple(0x22600+i*16 for i in range(8))
ARGS=(0x22700,0x22710)
KEYS=(0x22740,0x22750)
PAYLOADS=(0x22780,0x227A0)
MANAGER_GLOBAL,POOL_GLOBAL=0x3F960C,0x3F9610
BASE_TABLE=0x43B1C8
BASE_TABLE_HASH='1325d67337d0356240635b3ed50730d3908bc32d86d933efcd87b4ed5aa39d41'
READY,DISPATCH,CALLBACK_A,CALLBACK_B=(0xF00000A0,0xF00000B0,0xF00000C0,0xF00000D0)
RANGES=((0x225E80,0x225EB0),(0x2267D0,0x22694C),(0x226BC0,0x226C60),
        (0x226D78,0x226DDC),(0x226E38,0x226ED4),(0x226ED8,0x226F40),
        (0x226F40,0x226FB0),(0x226FB0,0x22700C))
HELPERS=((0x219FF0,0x21A040),(0x2AD700,0x2AD748),(0x2AD748,0x2AD798))
ENTRIES=tuple(a for a,b in RANGES)
CALLS=(0x219FF0,0x224AB8,0x225C28,0x224750,0x2AD700,0x2AD748,
       0x20E5C0,0x2AF100,READY,DISPATCH,CALLBACK_A,CALLBACK_B)
ARG_COUNTS=(2,1,2,3,3,2,1,1,1,2,2,2)
MEMORY_RANGES=((BUFFER,END),(MANAGER_GLOBAL,POOL_GLOBAL+4),(0x7F000,0x81000))


class ResourceBaseTrace(CameraTrace):
    def __init__(self,original,mutation=0,adjustment=0,init_flags=0):
        super().__init__(original)
        self.mutation,self.adjustment,self.init_flags=mutation,adjustment,init_flags
        self.calls=[0]*len(CALLS);self.events=[];self.lo=self.hi=None

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in (*RANGES,*HELPERS)):
            raise ValueError('unreviewed resource base instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('resource instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(
                a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('resource memory outside aligned authored scope')
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized resource memory')

    def load(self,address,size):
        self.memory_check(address,size)
        return Trace.load(self,address,size)

    def save(self,address,value,size):
        self.memory_check(address,size)
        return Trace.save(self,address,value,size)

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,instruction>>21&31,instruction>>16&31,instruction>>11&31,instruction&63
        sh=instruction>>6&31;imm=instruction&65535;simm=imm-65536 if imm&32768 else imm
        target,annul=None,False
        if op==0 and fn in (0,2,9,0x10,0x12,0x18,0x1B,0x23,0x24,0x2B):
            if fn in (0,2):
                if rs:raise ValueError('reserved resource immediate shift source')
                self.r[rd]=((self.r[rt]<<sh) if fn==0 else (self.r[rt]&0xFFFFFFFF)>>sh)&0xFFFFFFFF
            else:
                if sh:raise ValueError('reserved resource SPECIAL shift field')
                if fn==9:
                    if rt or rd!=31:raise ValueError('unsupported resource JALR form')
                    target=self.r[rs]&0xFFFFFFFF;self.r[31]=pc+8
                elif fn in (0x10,0x12):
                    if rs or rt:raise ValueError('reserved resource MFHI/MFLO source')
                    value=self.hi if fn==0x10 else self.lo
                    if value is None:raise ValueError('resource HI/LO read without supported producer')
                    self.r[rd]=value
                elif fn in (0x18,0x1B):
                    a,b=self.r[rs]&0xFFFFFFFF,self.r[rt]&0xFFFFFFFF
                    if fn==0x1B:
                        if rd:raise ValueError('reserved resource DIVU destination')
                        if b==0:raise ValueError('resource zero division outside contract')
                        self.lo,self.hi=a//b,a%b
                    else:
                        sa=a-0x100000000 if a&0x80000000 else a
                        sb=b-0x100000000 if b&0x80000000 else b
                        result=sa*sb;self.lo=result&0xFFFFFFFF;self.hi=(result>>32)&0xFFFFFFFF
                        self.r[rd]=self.lo
                elif fn==0x23:self.r[rd]=(self.r[rs]-self.r[rt])&0xFFFFFFFF
                elif fn==0x24:self.r[rd]=(self.r[rs]&self.r[rt])&0xFFFFFFFF
                else:self.r[rd]=int((self.r[rs]&0xFFFFFFFF)<(self.r[rt]&0xFFFFFFFF))
        elif op in (0x21,0x24,0x25,0x28,0x29):
            address=(self.r[rs]+simm)&0xFFFFFFFF
            size=2 if op in (0x21,0x25,0x29) else 1
            if op in (0x28,0x29):self.save(address,self.r[rt],size)
            else:
                value=self.load(address,size)
                if op==0x21 and value&32768:value-=65536
                self.r[rt]=value&0xFFFFFFFF
        elif op in (0x14,0x15):
            taken=(self.r[rs]&0xFFFFFFFF)==(self.r[rt]&0xFFFFFFFF)
            if op==0x15:taken=not taken
            if taken:target=(pc+4+simm*4)&0xFFFFFFFF
            else:annul=True
        elif op==0xC:self.r[rt]=self.r[rs]&imm
        else:
            allowed=op in (3,4,5,9,0xD,0xF,0x23,0x37,0x1E,0x2B,0x3F,0x1F) or op==0 and fn in (8,0x21,0x2D)
            if not allowed or op==0xF and rs:raise ValueError('unsupported resource opcode or operands')
            return super().execute(instruction,pc)
        self.r[0]=0;self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('resource instruction bound exceeded')
        return target,annul

    def run(self,entry,stop=RETURN):
        body=next((r for r in (*RANGES,*HELPERS) if r[0]==entry),None)
        if body is None:raise ValueError('unsupported resource base entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body resource transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc);op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unsupported resource return')
            branch=op in (4,5,20,21);call=op==3 or op==0 and instruction&63==9
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('resource delay outside complete body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay):raise ValueError('resource transfer in delay')
                if self.execute(delay,pc+4)!=(None,False):raise ValueError('resource transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif call:
                    continuation=self.r[31]
                    if target in ENTRIES:self.run(target,continuation)
                    else:self.library_call(target)
                    if self.r[31]!=continuation:raise ValueError('resource callback corrupted link register')
                    pc=continuation
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed resource local transfer')
                    pc=target
            else:pc+=8 if annul else 4

    def nested(self,entry,args):
        saved=list(self.r);self.r[29]=(self.r[29]-0x100)&0xFFFFFFFF
        for i,value in enumerate(args):self.r[4+i]=value
        self.r[31]=RETURN;self.run(entry);self.r=saved

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown resource controlled call')
        i=CALLS.index(target);a,b,c=[self.r[r]&0xFFFFFFFF for r in (4,5,6)]
        argc=ARG_COUNTS[i];args=[a,b,c][:argc]+[0]*(3-argc)
        if i==0 and (a!=MAP or b!=0x13579BDF):raise ValueError('unsupported resource lookup arguments')
        if i==1 and a!=RECORD:raise ValueError('unsupported resource initialization argument')
        if i==2 and b>255:raise ValueError('unsupported resource kind byte')
        if i==3 and (a,b,c)!=(0,1,RECORD):raise ValueError('unsupported resource accounting arguments')
        if i==4 and (a not in POOLS or (b,c)!=(16,0)):raise ValueError('unsupported pool allocation lanes')
        if i==5 and (a not in POOLS or not BUFFER<=b<END):raise ValueError('unsupported slot release arguments')
        if i==6 and a!=PAYLOADS[0]:raise ValueError('unsupported resource generic free')
        if i==7 and a!=RECORD:raise ValueError('unsupported resource deletion argument')
        if i in (8,9) and (a!=RECORD+self.adjustment or i==9 and b):raise ValueError('unsupported resource virtual arguments')
        if i in (10,11) and (a!=RECORD or b not in ARGS):raise ValueError('unsupported resource callback arguments')
        self.calls[i]+=1
        self.events.extend((i,*args,self.load(RECORD,2),self.load(RECORD+2,1),
            self.load(RECORD+0x10,4),self.load(RECORD+0x18,4),
            self.load(POOL_GLOBAL,4),self.load(MANAGER_GLOBAL,4),self.load(RECORD+4,4)))
        m=self.mutation
        if i==4 and m==1:self.save(RECORD+0x18,NODES[3],4)
        if target in tuple(a for a,b in HELPERS):self.run(target,self.r[31])
        else:self.r[2]=0x87654321
        if i==5 and m==2 and self.calls[i]==1:
            self.save(POOL_GLOBAL,POOLS[1],4);self.save(RECORD+0x18,NODES[3],4)
        if i in (10,11) and b==ARGS[0]:
            if m==3:self.nested(ENTRIES[4],(RECORD,CALLBACK_B,ARGS[1],KEYS[1]))
            if m==4:self.nested(ENTRIES[5],(RECORD,KEYS[1]))
            if m==5:
                self.save(POOL_GLOBAL,POOLS[1],4)
                head=self.load(RECORD+0x18,4)
                if head:self.save(head,CALLBACK_B,4)
            if m==6:self.save(RECORD+0x18,0,4)
        if i==2 and m==7:
            self.save(RECORD+0x10,0,4);self.save(RECORD+2,0x10,1)
        if i==2 and m==8:
            self.save(RECORD+0x10,PAYLOADS[0]|0xF0000000,4)
            self.save(RECORD+2,0,1);self.save(RECORD+0xC,128,4)
            self.save(MANAGER_GLOBAL,MANAGERS[1],4)
        if i==3 and m==9:
            self.save(RECORD+0x10,0,4);self.save(MANAGER_GLOBAL,MANAGERS[1],4)
        if i==3 and m==10:
            self.save(MANAGER_GLOBAL,MANAGERS[1],4)
            self.save(RECORD+0x10,PAYLOADS[0]|0xF0000000,4)
            self.save(RECORD+0xC,0xFFFFFFFF,4)
        if i==1 and m in (11,12):
            self.save(RECORD+0x10,PAYLOADS[0] if m==11 else 0,4)
            self.save(RECORD+2,self.init_flags,1);self.save(RECORD+0xC,0x123,4)
        for register in range(3,16):self.r[register]=0xDEADBEEF


def make_fixture(original,routine=0,flags=0,count=0,graph=0,mutation=0,
                 adjustment=0,mode=0,payload=1,allocator_case=0,alias=0,kind=2):
    t=ResourceBaseTrace(original,mutation,adjustment,flags)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0x5A
    t.save(MANAGER_GLOBAL,MANAGERS[0],4);t.save(POOL_GLOBAL,POOLS[0],4)
    for manager,allocator in zip(MANAGERS,ALLOCATORS):
        t.save(manager+0x168,allocator,4)
        for k in range(8):t.save(manager+0x78+k*4,MAP,4)
    for allocator in ALLOCATORS:
        for k in range(1000):t.save(allocator+k*4,POOLS[0],4)
        t.save(allocator+0xFA0,PAYLOADS[0],4);t.save(allocator+0xFA4,PAYLOADS[0]+32,4)
        t.save(allocator+0xFBC,0,4);t.save(allocator+0xFC4,POOLS[1],4);t.save(allocator+0xFC8,0,4)
    for pool in POOLS:
        for off,value in ((0,32),(4,16),(8,NODES[0]),(12,0),(20,4)):t.save(pool+off,value,4)
        t.save(pool+16,4,2);t.save(pool+24,0,2);t.save(pool+26,0,2)
    for j,node in enumerate(NODES):
        for off,value in ((0,CALLBACK_A if j==0 else CALLBACK_B),(4,ARGS[0] if j==0 else ARGS[1]),
                          (8,KEYS[0] if j in (0,2) else KEYS[1]),(12,0)):t.save(node+off,value,4)
        if j>=4:t.save(node,j+1 if j<7 else 0xFFFF,2)
    head=0
    if graph:
        head=NODES[0]
        t.save(NODES[0]+12,NODES[1] if graph>=2 else 0,4)
        t.save(NODES[1]+12,NODES[2] if graph>=3 else 0,4)
    t.save(RECORD,count,2);t.save(RECORD+2,flags,1);t.save(RECORD+3,2,1)
    for off,value in ((4,KEYS[0]),(8,0x11223344),(12,128),(16,PAYLOADS[0] if payload else 0),
                      (20,0x55667788),(24,head),(28,0x1234ABCD),(32,TABLE)):
        t.save(RECORD+off,value,4)
    for i in range(12):t.save(TABLE+i*4,0,4)
    t.save(TABLE+0x18,adjustment,2);t.save(TABLE+0x1C,DISPATCH,4)
    t.save(TABLE+0x28,adjustment,2);t.save(TABLE+0x2C,READY,4)
    t.save(MAP+4,7,4);t.save(MAP+12,BUCKETS,4)
    for j in range(7):t.save(BUCKETS+j*4,0,4)
    key=0x13579BDF
    t.save(BUCKETS+(key%7)*4,MAP_NODES[0] if graph else 0,4)
    for node,next_,k,value in ((MAP_NODES[0],MAP_NODES[1],key+7,OTHER),
                               (MAP_NODES[1],0,key,RECORD)):
        t.save(node,next_,4);t.save(node+4,k,4);t.save(node+8,value,4)
    allocator=ALLOCATORS[0]
    if allocator_case in (1,2):
        t.save(allocator+0xFBC,PAYLOADS[1],4);t.save(allocator+0xFC8,1,4)
        if allocator_case==2:t.save(allocator+0xFC4,0,4)
    if allocator_case==3:t.save(allocator+0xFA0,PAYLOADS[0]+1,4)
    if allocator_case==4:t.save(allocator+0xFA4,PAYLOADS[0]-1,4)
    if allocator_case==5:t.save(allocator+0xFC8,2,4);t.save(allocator+0xFBC,PAYLOADS[1],4)
    if allocator_case==6:t.save(RECORD+0xC,0xFFFFFFFF,4)
    if allocator_case==7:t.save(RECORD+0x10,PAYLOADS[0]|0xF0000000,4)
    if allocator_case==8:
        t.save(allocator+0xFA0,PAYLOADS[0]-32,4);t.save(allocator+0xFA4,PAYLOADS[0],4)
    if alias==1:
        t.save(POOLS[0]+8,RECORD+8,4);t.save(POOLS[0]+16,0,2)
        t.save(POOLS[0]+20,1,4);t.save(RECORD+8,0xFFFF,2)
    if alias==2:t.save(POOLS[0]+20,0,4)
    if alias==3:t.save(POOLS[0]+26,1,2)
    initial=[t.load(BUFFER+i*4,4) for i in range(WORDS)]
    globals_=[t.load(a,4) for a in (MANAGER_GLOBAL,POOL_GLOBAL)]
    t.r[29],t.r[31]=0x80000,RETURN
    t.r[4]=RECORD
    if routine==0:t.r[4],t.r[5]=key,kind
    elif routine==1:t.r[5]=mode
    elif routine==2:t.r[5:9]=[KEYS[0],kind,mode,0x1234ABCD]
    elif routine==4:t.r[5:8]=[CALLBACK_A,ARGS[0],KEYS[0]]
    elif routine==5:t.r[5]=KEYS[1] if alias==1 else KEYS[0] if alias==2 else ARGS[0]
    t.run(ENTRIES[routine])
    return dict(routine=routine,flags=flags,count=count,graph=graph,mutation=mutation,
                adjustment=adjustment&0xFFFFFFFF,mode=mode,payload=payload,allocator_case=allocator_case,
                alias=alias,kind=kind,initial=initial,globals_initial=globals_,
                expected=[t.load(BUFFER+i*4,4) for i in range(WORDS)],
                globals_expected=[t.load(a,4) for a in (MANAGER_GLOBAL,POOL_GLOBAL)],
                result=(t.r[2]&0xFFFFFFFF) if routine in (0,2,4,5) else 0,
                calls=t.calls,events=t.events,instruction_count=t.instruction_count)


def fixtures(original):
    data=original[BASE_TABLE-0xFF000:BASE_TABLE-0xFF000+48]
    if hashlib.sha256(data).hexdigest()!=BASE_TABLE_HASH:
        raise ValueError('incorrect complete resource base table prefix')
    cases=[]
    for routine in range(8):
        for flags in (0,4,0x10,0x40,0x44,0x80,0xFF):
            for count in (0,1,0xFFFF):
                graph=3 if routine in (1,5,6) else 0
                cases.append(make_fixture(original,routine,flags,count,graph))
    for graph in range(4):
        cases.append(make_fixture(original,0,graph=graph,kind=0x40000000))
        for alias in (0,1,2):cases.append(make_fixture(original,5,graph=graph,alias=alias))
        cases.append(make_fixture(original,6,graph=graph))
    for case in range(9):
        for flags in (0,0x10):
            for mode in (0,1,0xFFFFFFFE):
                cases.append(make_fixture(original,1,flags,graph=2,allocator_case=case,mode=mode))
    for m in (2,7,8,9,10):cases.append(make_fixture(original,1,graph=2,mutation=m,mode=1))
    for flags in (0,0x10,0x80):
        for m in (0,11,12):
            for mode in (0,1,0xFFFFFFFF):cases.append(make_fixture(original,2,flags,mode=mode,mutation=m,kind=0x102))
    for alias in range(4):cases.append(make_fixture(original,4,graph=1,alias=alias,mutation=1))
    for routine in (3,7):
        for adjustment in (-4,12):cases.append(make_fixture(original,routine,4,count=1,adjustment=adjustment))
    for m in range(1,7):cases.append(make_fixture(original,6,graph=3,mutation=m))
    cases.append(make_fixture(original,4,4,graph=1,mutation=3))
    cases.append(make_fixture(original,1,payload=0,mode=1))
    return cases


def input_hash(cases):
    keys=('routine','flags','count','graph','mutation','adjustment','mode','payload','allocator_case','alias','kind','initial','globals_initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    fields=('routine','flags','count','graph','mutation','adjustment','mode','payload','allocator_case','alias','kind')
    n=max(map(lambda c:len(c['events']),cases))
    def runs(words):
        out=[];start=0
        while start<len(words):
            end=start+1
            while end<len(words) and words[end]==words[start]:end+=1
            out.extend((start,end-start,words[start]));start=end
        return out
    initial=[runs(c['initial']) for c in cases]
    changes=[[v for i,(a,b) in enumerate(zip(c['initial'],c['expected'])) if a!=b for v in (i,b)] for c in cases]
    nr,nc=max(map(len,initial)),max(map(len,changes))
    lines=['/* Authored bounded resource callback fixtures; no retail code/table arrays. */',
           '/* Runs encode the complete authored input; patches encode all changes. */',
           'struct ResourceBaseGolden { u32 '+','.join(fields)+',globals_initial[2],globals_expected[2],result,calls[12],initial_run_count,initial_runs[%d],change_count,changes[%d],event_count,events[%d]; };'%(nr,nc,n),
           'static const struct ResourceBaseGolden resource_base_golden[] = {']
    for c,runs_,changes_ in zip(cases,initial,changes):
        scalar=','.join('0x%08Xu'%c[k] for k in fields)
        array=lambda values:'{'+','.join('0x%08Xu'%v for v in values)+'}'
        lines.append(' {%s,%s,%s,0x%08Xu,%s,%du,%s,%du,%s,%du,%s},'%(scalar,array(c['globals_initial']),array(c['globals_expected']),c['result'],array(c['calls']),len(runs_)//3,array(runs_),len(changes_)//2,array(changes_),len(c['events']),array(c['events'])))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/resource_base_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,raw=validated_elf(args.elf);cases=fixtures(raw)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d fixtures; %d instructions; max%d; input %s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))


if __name__=='__main__':main()
