"""Bounded complete resource bounds/group bodies and connected real helpers.

Reuse the strict scalar decoder. Execute the existing group release body and
primary publisher directly, and execute the actual bounds destructor selected
by its checked original table pair. The projection helper has an explicit
authored finite output contract; its engine implementation is not executed.
Other calls use authored observations/mutations, without heap or EE fidelity.
Only synthetic memory/events are exported. Counts and indices fit four slots.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_resource_groups import (GroupTrace, BUFFER, END, WORDS, GROUP,
    ALLOCATED, HOLDER, OTHER_HOLDER, CHILDREN, PAYLOADS, TABLE, KEYS, PRIMARY,
    OTHER_PRIMARY, SECONDARY, OTHER_SECONDARY, OUTPUT)
from trace_geometry import Trace, RETURN, word
from trace_property_records import signed32

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x20F858,0x20F860),(0x20F860,0x20F870),
        (0x20F870,0x20F8D0),(0x20F8D0,0x20F98C),
        (0x20F990,0x20F9AC),(0x20F9B0,0x20FA08),
        (0x20FA08,0x20FA88),(0x20FA88,0x20FC14),
        (0x20FC18,0x20FC20),(0x20FC20,0x20FC28),
        (0x20FC28,0x20FC30),(0x20FC30,0x20FCC0),
        (0x20FCC0,0x20FD48),(0x2B6F60,0x2B6FC8))
HELPER_RANGES=((0x20F5D8,0x20F6F8),(0x2B6F28,0x2B6F44))
ENTRIES=tuple(a for a,b in (*RANGES,*HELPER_RANGES))
CHILD_VIRTUAL,PAYLOAD_VIRTUAL=0xF00000C0,0xF00000D0
CALLS=(0x225E80,0x2AEE60,0x226BC0,0x224678,0x226D78,
       0x227B98,CHILD_VIRTUAL,0x226ED8,0x2267D0,0x22B360,PAYLOAD_VIRTUAL)
TABLES={0x43A458:'06986c747b09f427fb8efc89ec51179d0529db337d018816f972a2ea66af3d5d',
        0x43A4B0:'815d883e31d7e3e94279da5563413547f6ce0641c9cfb60f42cbeb1ec7b47149'}
POINT=tuple(word(v) for v in (-0.0,2.5,7.0,1.0))
FRAME=tuple(word(float(i+1)) for i in range(12))


class BoundsTrace(GroupTrace):
    def __init__(self,original,mutation=0,adjustment=-4):
        super().__init__(original,mutation,adjustment)
        self.calls=[0]*len(CALLS);self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in (*RANGES,*HELPER_RANGES)):
            raise ValueError('unreviewed bounds instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):
            raise ValueError('bounds instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        for base,sha in TABLES.items():
            if base<=address<base+16:
                if size not in (2,4) or address%size or address+size>base+16:
                    raise ValueError('invalid bounds readonly prefix load')
                data=self.original[base-0xFF000:base+16-0xFF000]
                if hashlib.sha256(data).hexdigest()!=sha:
                    raise ValueError('incorrect bounds readonly prefix')
                return int.from_bytes(data[address-base:address-base+size],'little')
        self.memory_scope(address,size)
        return Trace.load(self,address,size)

    def execute(self,instruction,pc):
        if instruction>>26==0 and instruction&63==0x2A:
            rs,rt,rd=(instruction>>21)&31,(instruction>>16)&31,(instruction>>11)&31
            if (instruction>>6)&31:raise ValueError('reserved bounds SLT operand')
            # The only owned SLT compares the incremented nonnegative index
            # with a freshly loaded unsigned halfword count.
            if not all(0<=self.r[r]<=65536 for r in (rs,rt)):
                raise ValueError('bounds SLT outside halfword loop domain')
            self.r[rd]=int(signed32(self.r[rs])<signed32(self.r[rt]))
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('bounds trace exceeded bound')
            return None,False
        return super().execute(instruction,pc)

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown bounds controlled call')
        i=CALLS.index(target)
        a,b,c,d,e=[self.r[r]&0xFFFFFFFF for r in range(4,9)]
        argc=(2,1,5,3,1,1,2,2,2,3,2)[i]
        args=[a,b,c,d,e][:argc]+[0]*(5-argc)
        if i==0 and (a!=KEYS[0] or b!=6):raise ValueError('invalid bounds lookup')
        if i==1 and a!=0x40:raise ValueError('invalid bounds allocation')
        if i==2 and (a not in (GROUP,ALLOCATED) or b!=KEYS[0] or c not in (2,6) or d not in (0,1,0xFEDCBA98) or e!=0x1234ABCD):
            raise ValueError('invalid bounds base constructor')
        if i==3 and ((a,b)!=(1,0) or c not in (GROUP,ALLOCATED)):
            raise ValueError('invalid bounds request')
        if i==4 and a not in (GROUP,ALLOCATED):raise ValueError('invalid bounds activation')
        if i==5 and a not in (HOLDER,OTHER_HOLDER):raise ValueError('invalid bounds holder release')
        if i==6 and (a not in tuple(p+self.adjustment for p in CHILDREN) or b):
            raise ValueError('invalid bounds child release')
        if i==7 and (a not in (0,*CHILDREN) or b!=GROUP):raise ValueError('invalid bounds detach')
        if i==8 and (a not in (GROUP,ALLOCATED) or b not in (0,3,0xFEDCBA98)):
            raise ValueError('invalid bounds base destruction')
        if i==9:
            if a not in (HOLDER,OTHER_HOLDER) or b%16 or c%16 or not all(0x7F000<=p and p+n<=0x81000 for p,n in ((b,16),(c,48))) or not (b+16<=c or c+48<=b):
                raise ValueError('invalid bounds projection output contract')
            # Local stack addresses differ between host C and the original.
            args[1],args[2]=0xF00000E0,0xF00000F0
        if i==10 and (a not in tuple(p+self.adjustment for p in PAYLOADS) or b!=3):
            raise ValueError('invalid bounds payload release')
        self.calls[i]+=1
        self.events.extend((i,*args,self.load(GROUP+2,1),self.load(GROUP+0x20,4),
                            self.load(GROUP+0x10,4),self.load(GROUP+0x28,4),self.load(GROUP,2)))
        result=0x87654321
        if i==0:result=self.load(KEYS[0]+4,4)
        if i==1:result=ALLOCATED
        if i==2:
            self.save(a,1,2);self.save(a+0x10,HOLDER,4)
            self.save(a+0x1C,e,4);self.save(a+0x20,TABLE,4)
            if c==2:self.save(a+0x24,3,2);self.save(a+0x26,2,2)
            else:self.save(a+0x24,PAYLOADS[2],4)
        if i==9:
            for index,value in enumerate(POINT):self.save(b+index*4,value,4)
            for index,value in enumerate(FRAME):self.save(c+index*4,value,4)
            result=PAYLOADS[0]
        self.mutate(i)
        self.r[2]=result

    def mutate(self,i):
        if self.calls[i]!=1:return
        m=self.mutation
        if m==1 and i==9:
            self.save(GROUP+0x10,OTHER_HOLDER,4);self.save(GROUP+0x24,PAYLOADS[1],4)
            self.save(GROUP+2,self.load(GROUP+2,1)^8,1)
            # Fresh factory flags are tested after the controlled projection.
            self.save(ALLOCATED+2,self.load(ALLOCATED+2,1)^8,1)
        if m==2 and i==3:
            self.save(KEYS[0]+4,0,4);self.save(GROUP+2,self.load(GROUP+2,1)^0x80,1)
        if m==3 and i==5:
            self.save(GROUP+2,self.load(GROUP+2,1)|0x20,1)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==4 and i==6:
            self.save(GROUP+0x28,CHILDREN[3],4);self.save(GROUP+0x68,CHILDREN[3],4)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==5 and i==7:
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==6 and i==10:self.save(GROUP+0x24,PAYLOADS[1],4)
        if m==7 and i==8:self.save(GROUP+0x10,OTHER_HOLDER,4)
        if m==8 and i==2:
            self.save(GROUP+2,self.load(GROUP+2,1)^0x10,1)
            self.save(ALLOCATED+2,self.load(ALLOCATED+2,1)^0x10,1)

    def run(self,entry,stop=RETURN):
        body=next((r for r in (*RANGES,*HELPER_RANGES) if r[0]==entry),None)
        if body is None:raise ValueError('unsupported bounds entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body bounds transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26;returning=op==0 and instruction&63==8
            if returning and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unreviewed bounds return at %#x: %#x/%#x'%(pc,target,stop))
            branch=op in (1,4,5,6,7,0x14,0x15)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('bounds delay outside body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):
                    raise ValueError('bounds transfer in delay')
                if returning:return
                if target is None:pc+=8
                elif op==3 or op==0 and instruction&63==9:
                    if target in ENTRIES:self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed bounds branch')
                    pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine,flags=4,child_flags=4,count=0xFFFF,
                 first=3,second=2,present=1,mutation=0,adjustment=-4,
                 mode=0,lookup=1,init_flags=0,index=1,alias=0,owned=1):
    t=BoundsTrace(original,mutation,adjustment)
    for a,b in ((BUFFER,END),(0x7F000,0x81000)):
        for p in range(a,b):t.memory[p]=0x5A
    t.save(GROUP,count,2);t.save(GROUP+2,flags,1)
    t.save(ALLOCATED+2,init_flags,1)
    t.save(GROUP+0x10,HOLDER,4);t.save(GROUP+0x1C,0x1234ABCD,4);t.save(GROUP+0x20,TABLE,4)
    is_group=routine in (0,1,2,3,4,5,13)
    if is_group:t.save(GROUP+0x24,first,2);t.save(GROUP+0x26,second,2)
    else:t.save(GROUP+0x24,PAYLOADS[0] if present else 0,4)
    for i in range(4):
        t.save(CHILDREN[i]+2,child_flags if i!=2 else 0,1)
        t.save(CHILDREN[i]+0x10,PAYLOADS[i],4);t.save(CHILDREN[i]+0x20,TABLE,4)
        t.save(GROUP+0x28+i*4,CHILDREN[i] if i!=1 or present else 0,4)
        t.save(GROUP+0x68+i*4,CHILDREN[i] if i!=1 or present else 0,4)
        t.save(PAYLOADS[i],TABLE,4);t.save(PAYLOADS[i]+4,owned,2);t.save(PAYLOADS[i]+6,count,2)
        t.save(KEYS[i]+4,GROUP if lookup else 0,4)
    t.save(TABLE+8,adjustment&65535,2);t.save(TABLE+12,PAYLOAD_VIRTUAL,4)
    t.save(TABLE+16,adjustment&65535,2);t.save(TABLE+20,CHILD_VIRTUAL,4)
    for holder,primary in ((HOLDER,PRIMARY),(OTHER_HOLDER,OTHER_PRIMARY)):
        t.save(holder+0x1C,first,2);t.save(holder+0x20,primary,4)
        for i in range(4):t.save(primary+i*0x24,KEYS[i],4)
    if not is_group:
        for i in range(6):t.save(GROUP+0x28+i*4,word(float(i+21)),4)
    args=[GROUP,mode,0x1234ABCD,index]
    if routine==1:args=[GROUP,index,0,0]
    if routine in (2,11):args=[GROUP,KEYS[0],mode,0x1234ABCD]
    if routine==3:args=[GROUP,CHILDREN[0] if present else 0,index,0]
    if routine==7:args=[KEYS[0],mode,0x1234ABCD,0]
    if routine==13:
        payload=HOLDER+0xC if alias==1 else PRIMARY if alias==2 else PAYLOADS[0]
        args=[HOLDER,payload,index,0]
    initial=[t.load(BUFFER+i*4,4) for i in range(WORDS)]
    t.r[29],t.r[31]=0x80000,RETURN;t.r[4:8]=args
    t.run(RANGES[routine][0])
    return dict(routine=routine,mutation=mutation,adjustment=adjustment,args=args,
                initial=initial,expected=[t.load(BUFFER+i*4,4) for i in range(WORDS)],
                calls=t.calls,events=t.events,result=t.r[2]&0xFFFFFFFF,
                instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for routine in range(14):
        for flags in (0,4,8,0x10,0x18,0x24,0x40,0x80,0x84,0xFF):
            cases.append(make_fixture(original,routine,flags=flags))
    for routine in (0,1,3,4,5,13):
        for first,second in ((0,0),(0,2),(3,0),(3,3)):
            cases.append(make_fixture(original,routine,first=first,second=second,present=0))
    for routine in (3,13):
        for index in (0,1,3,0xFFFFFFFF):
            cases.append(make_fixture(original,routine,index=index))
        for child_flags in (0,4,0x20,0x24):
            cases.append(make_fixture(original,routine,child_flags=child_flags,present=0))
    for routine in (2,6,7,11):
        for mode in (1,0xFEDCBA98):
            cases.append(make_fixture(original,routine,mode=mode))
    for mode in (0,1,0xFEDCBA98):
        for flags in (0,8,0x10,0x18,0xFF):
            cases.append(make_fixture(original,7,lookup=0,init_flags=flags,mode=mode))
    for routine in (2,11):
        for flags in (0,0x10,0x18):
            cases.append(make_fixture(original,routine,flags=flags,mutation=8))
    for routine,mutations in ((6,(1,)),(7,(1,2)),(4,(3,4,5)),(5,(3,4,5,7)),(12,(6,7))):
        for mutation in mutations:
            cases.append(make_fixture(original,routine,mutation=mutation,count=1,
                flags=4,mode=1 if routine==7 else 0xFEDCBA98 if routine in (5,12) else 0,
                init_flags=0x10,lookup=0 if mutation==1 and routine==7 else 1,adjustment=12))
    for count in (0,1,2,0xFFFF):
        for owned in (0,1):
            for present in (0,1):cases.append(make_fixture(original,12,count=count,owned=owned,present=present))
    for alias in (1,2):cases.append(make_fixture(original,13,index=0xFFFFFFFF,alias=alias))
    return cases


def input_hash(cases):
    keys=('routine','mutation','adjustment','args','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    n=max(len(c['events']) for c in cases)
    lines=['/* Authored bounds observations; no original instruction or table arrays. */',
           'struct ResourceBoundsGolden { u32 routine,mutation,adjustment,args[4],initial[%d],expected[%d],calls[%d],event_count,events[%d],result; };'%(WORDS,WORDS,len(CALLS),n),
           'static const struct ResourceBoundsGolden resource_bounds_golden[] = {']
    for c in cases:
        array=lambda k:','.join('0x%08Xu'%(v&0xFFFFFFFF) for v in c[k])
        lines.append('    {%du,%du,0x%08Xu,{%s},{%s},{%s},{%s},%du,{%s},0x%08Xu},'%(c['routine'],c['mutation'],c['adjustment']&0xFFFFFFFF,array('args'),array('initial'),array('expected'),array('calls'),len(c['events']),array('events'),c['result']))
    return '\n'.join(lines+['};',''])


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/resource_bounds_trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(args.elf)
    cases=fixtures(original);args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(dict(limitation=__doc__,input_sha256=input_hash(cases),cases=cases),indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounds fixtures; %d original instructions; maximum %d; input SHA256 %s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))


if __name__=='__main__':main()
