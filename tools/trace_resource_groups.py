"""Bounded integer resource-group fixtures from ten complete original bodies.

Execute the five connected relocation/enumeration/publication helpers directly.
Other engine and virtual calls have explicit authored effects and mutation hooks;
they do not model the complete engine, allocation failure, MMIO or EE timing.
Only synthetic input/output/event data is exported. The local validated ELF
supplies instructions and the checked 16-byte prefix of the group virtual table.
Counts must fit the original caller's two 16-entry arrays and stack buffers.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_resource_lifecycle import ResourceLifecycleTrace
from trace_geometry import Trace, RETURN

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20B00
WORDS=(END-BUFFER)//4
GROUP,ALLOCATED=0x20000,0x20100
HOLDER,OTHER_HOLDER=0x20200,0x20240
CHILDREN=(0x20300,0x20340,0x20380,0x203C0)
PAYLOADS=(0x20400,0x20440,0x20480,0x204C0)
TABLE=0x20500
KEYS=(0x20600,0x20610,0x20620,0x20630)
PRIMARY,OTHER_PRIMARY=0x20700,0x20800
SECONDARY,OTHER_SECONDARY=0x20900,0x20940
OUTPUT=0x20A00
VIRTUAL=0xF00000C0
ORIGINAL_TABLE,TABLE_END=0x43A458,0x43A468
TABLE_SHA256='06986c747b09f427fb8efc89ec51179d0529db337d018816f972a2ea66af3d5d'
FALLBACK_GLOBAL=0x4682DC
RANGES=((0x20F128,0x20F340),(0x20F340,0x20F440),(0x20F440,0x20F5D8),
        (0x20F5D8,0x20F6F8),(0x20F6F8,0x20F854),(0x2B6DC0,0x2B6DF8),
        (0x2B7088,0x2B70DC),(0x2B70E0,0x2B714C),(0x2B6F28,0x2B6F44),
        (0x2B6F48,0x2B6F5C))
ENTRIES=tuple(a for a,b in RANGES)
CALLS=(0x20FD48,0x226E38,0x226D78,0x227B68,0x226F40,0x227B98,
       VIRTUAL,0x226ED8,0x225E80,0x2AEE60,0x226BC0,0x224678,0x20F9B0)


class GroupTrace(ResourceLifecycleTrace):
    def __init__(self,original,mutation=0,adjustment=-4):
        super().__init__(original)
        self.mutation,self.adjustment=mutation,adjustment
        self.calls=[0]*len(CALLS);self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):
            raise ValueError('unreviewed group instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):
            raise ValueError('group instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if ORIGINAL_TABLE<=address<TABLE_END:
            if size not in (2,4) or address%size or address+size>TABLE_END:
                raise ValueError('invalid group table prefix load')
            data=self.original[ORIGINAL_TABLE-0xFF000:TABLE_END-0xFF000]
            if hashlib.sha256(data).hexdigest()!=TABLE_SHA256:
                raise ValueError('incorrect group table prefix')
            return int.from_bytes(data[address-ORIGINAL_TABLE:address-ORIGINAL_TABLE+size],'little')
        if address==FALLBACK_GLOBAL and size==4:
            # Authored replacement for the actual global pointer identity.
            return PAYLOADS[3]
        self.memory_scope(address,size)
        return Trace.load(self,address,size)

    def save(self,address,value,size):
        self.memory_scope(address,size)
        return Trace.save(self,address,value,size)

    @staticmethod
    def memory_scope(address,size):
        if size not in (1,2,4,8,16) or address%size or not any(
                a<=address and address+size<=b for a,b in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('group memory outside aligned authored scope: %#x/%d'%(address,size))

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown group controlled call')
        i=CALLS.index(target)
        a,b,c,d,e=[self.r[r]&0xFFFFFFFF for r in range(4,9)]
        argc=(3,4,1,1,1,1,2,2,2,1,5,3,2)[i]
        args=[a,b,c,d,e][:argc]+[0]*(5-argc)
        if i==0 and (a not in KEYS or b or c not in (0x1234ABCD,0x13579BDF)):
            raise ValueError('unreviewed group child lookup')
        if i==1 and (a not in CHILDREN or (b,c,d)!=(ENTRIES[2],GROUP,GROUP)):
            raise ValueError('unreviewed group ready callback')
        if i==2 and a not in (*CHILDREN,GROUP,ALLOCATED):
            raise ValueError('unreviewed group activation')
        if i in (3,5) and a not in (HOLDER,OTHER_HOLDER):
            raise ValueError('unreviewed group holder notification')
        if i==4 and a!=GROUP:raise ValueError('unreviewed group completion')
        if i==6 and (a not in tuple(p+self.adjustment for p in CHILDREN) or b):
            raise ValueError('unreviewed group virtual release')
        if i==7 and (a not in (0,*CHILDREN) or b!=GROUP):
            raise ValueError('unreviewed group detach')
        if i==8 and (a!=KEYS[0] or b!=2):raise ValueError('unreviewed group lookup')
        if i==9 and a!=0xE8:raise ValueError('unreviewed group allocation')
        if i==10 and ((a,b,c)!=(ALLOCATED,KEYS[0],2) or d not in (0,1,0xFEDCBA98) or e!=0x1234ABCD):
            raise ValueError('unreviewed group initialization')
        if i==11 and ((a,b)!=(1,0) or c not in (GROUP,ALLOCATED)):
            raise ValueError('unreviewed group request')
        if i==12 and (a!=ALLOCATED or b!=3):raise ValueError('unreviewed group destruction')
        self.calls[i]+=1
        self.events.extend((i,*args,self.load(GROUP+2,1),self.load(GROUP+0x24,2),
                            self.load(GROUP+0x26,2),self.load(GROUP+0x10,4),self.load(GROUP+0x28,4)))
        result=0x87654321
        if i==0:result=self.load(a,4)
        if i==8:result=self.load(KEYS[0]+4,4)
        if i==9:result=ALLOCATED
        if i==10:
            # Explicit authored constructor observations, without heap fidelity.
            self.save(ALLOCATED,1,2);self.save(ALLOCATED+0x10,HOLDER,4)
            self.save(ALLOCATED+0x1C,e,4);self.save(ALLOCATED+0x20,TABLE,4)
            self.save(ALLOCATED+0x24,3,2);self.save(ALLOCATED+0x26,2,2)
        self.mutate(i)
        self.r[2]=result

    def mutate(self,i):
        if self.calls[i]!=1:return
        m=self.mutation
        if m==1 and i==0:
            self.save(GROUP+0x10,OTHER_HOLDER,4);self.save(GROUP+0x1C,0x13579BDF,4)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==2 and i==1:
            self.save(GROUP+0x28,CHILDREN[3],4);self.save(GROUP+2,self.load(GROUP+2,1)|0x20,1)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==3 and i==2:
            self.save(GROUP+0x10,OTHER_HOLDER,4)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==4 and i==4:
            self.save(GROUP+0x10,OTHER_HOLDER,4);self.save(GROUP+0x26,0,2)
            self.save(GROUP+2,self.load(GROUP+2,1)^4,1)
        if m==5 and i==5:
            self.save(GROUP+2,self.load(GROUP+2,1)|0x20,1)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==6 and i==6:
            self.save(GROUP+2,self.load(GROUP+2,1)|0x20,1)
            self.save(GROUP+0x28,CHILDREN[3],4);self.save(GROUP+0x68,CHILDREN[3],4)
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
        if m==7 and i==7:
            self.save(GROUP+0x24,1,2);self.save(GROUP+0x26,1,2)
            self.save(GROUP+0x10,OTHER_HOLDER,4)
        if m==8 and i==11:
            self.save(KEYS[0]+4,0,4)
            self.save(GROUP+2,self.load(GROUP+2,1)^0x80,1)

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported group entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body group transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26
            returning=op==0 and instruction&63==8
            if returning and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unreviewed group return')
            branch=op in (1,4,5,6,7,0x14,0x15)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('group delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):
                    raise ValueError('group transfer in delay slot')
                if returning:return
                if target is None:pc+=8
                elif op==3 or op==0 and instruction&63==9:
                    if target in ENTRIES:self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed group branch')
                    pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine,flags=4,child_flags=0x20,count=0xFFFF,
                 first=2,second=2,pattern=0,mutation=0,adjustment=-4,
                 mode=0,lookup=1,init_flags=0,alias=0):
    t=GroupTrace(original,mutation,adjustment)
    for a,b in ((BUFFER,END),(0x7F000,0x81000)):
        for p in range(a,b):t.memory[p]=0x5A
    t.save(GROUP,count,2);t.save(GROUP+2,flags,1)
    t.save(GROUP+0x10,HOLDER,4);t.save(GROUP+0x1C,0x1234ABCD,4);t.save(GROUP+0x20,TABLE,4)
    t.save(GROUP+0x24,first,2);t.save(GROUP+0x26,second,2)
    for i in range(4):
        child=CHILDREN[i]
        t.save(child,count,2);t.save(child+2,child_flags,1)
        t.save(child+0x10,PAYLOADS[i],4);t.save(child+0x20,TABLE,4)
        t.save(KEYS[i],child,4);t.save(KEYS[i]+4,GROUP if lookup else 0,4)
        t.save(GROUP+0x28+i*4,child,4);t.save(GROUP+0x68+i*4,child,4)
    # A later ready child and null child make the first unready exit observable.
    if pattern in (1,3):
        t.save(GROUP+0x28,0,4);t.save(GROUP+0x68,0,4)
        t.save(KEYS[0],0,4)
    if pattern in (2,3):
        t.save(CHILDREN[0]+2,0,1);t.save(CHILDREN[1]+2,0x20,1)
        t.save(GROUP+0x30,0,4);t.save(GROUP+0x70,0,4)
    if pattern==4:
        t.save(GROUP+0x28,CHILDREN[1],4);t.save(GROUP+0x68,CHILDREN[0],4)
        t.save(CHILDREN[0]+2,0,1);t.save(CHILDREN[1]+2,0x20,1)
        t.save(GROUP+0x70,0,4)
    t.save(TABLE+0x10,adjustment,2);t.save(TABLE+0x14,VIRTUAL,4)
    t.save(ALLOCATED+2,init_flags,1)
    for holder,primary,secondary in ((HOLDER,PRIMARY,SECONDARY),(OTHER_HOLDER,OTHER_PRIMARY,OTHER_SECONDARY)):
        relative=routine in (0,5) and holder==HOLDER
        t.save(holder,0x123,4);t.save(holder+8,0x10 if relative else holder+0x10,4)
        t.save(holder+0x1C,first,2)
        t.save(holder+0x20,primary-holder if relative else primary,4)
        t.save(holder+0x24,secondary-holder if relative else secondary,4)
        for i in range(4):
            t.save(primary+i*0x24,KEYS[i],4)
            t.save(primary+i*0x24+0x1D,second if i==0 else 0,1)
            t.save(secondary+i*8+4,KEYS[i],4)
    if routine==0 and mode!=0:
        # The reload path expects already resolved arrays, but relocates holder.
        pass
    if routine==5 and alias==1:
        t.save(HOLDER+0x20,0,4);t.save(HOLDER+0x24,0,4);t.save(HOLDER+8,0,4)
    if routine==7:
        if alias==1:t.save(HOLDER+0x20,0,4)
        if alias==2:t.save(HOLDER+0x24,0,4)
    args=[GROUP,mode,0x1234ABCD]
    if routine==2:args=[mode,GROUP,0]
    if routine==4:args=[KEYS[0],mode,0x1234ABCD]
    if routine==5:args=[HOLDER,0,0]
    if routine==6:args=[HOLDER,mode,HOLDER+0x1C if alias else OUTPUT]
    if routine==7:args=[HOLDER,SECONDARY+4 if alias==3 else OUTPUT,0]
    if routine in (8,9):args=[HOLDER,mode,PAYLOADS[0]]
    initial=[t.load(BUFFER+i*4,4) for i in range(WORDS)]
    t.r[29],t.r[31]=0x80000,RETURN
    t.r[4:7]=args
    t.run(ENTRIES[routine])
    return dict(routine=routine,mutation=mutation,adjustment=adjustment,args=args,initial=initial,
                expected=[t.load(BUFFER+i*4,4) for i in range(WORDS)],
                calls=t.calls,events=t.events,result=t.r[2]&0xFFFFFFFF,
                instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for routine in range(5):
        for flags in (0,4,0x20,0x24,0x40,0x80,0x84,0xFF):
            for child_flags in (0,0x20,0x80):
                cases.append(make_fixture(original,routine,flags=flags,child_flags=child_flags))
        for first,second in ((0,0),(0,2),(2,0),(3,3)):
            cases.append(make_fixture(original,routine,first=first,second=second,count=7))
        for pattern in (1,2,3,4):
            cases.append(make_fixture(original,routine,first=3,second=3,pattern=pattern,count=0))
    for mode in (1,0xFEDCBA98,0xFFFFFFFF):
        for flags in (0,4,0x24):
            cases.append(make_fixture(original,0,flags=flags,mode=mode))
            cases.append(make_fixture(original,2,flags=flags,mode=mode))
    for routine,mutations in ((0,(1,2,4)),(1,(3,)),(2,(4,)),(3,(5,6,7)),(4,(8,))):
        for mutation in mutations:
            cases.append(make_fixture(original,routine,mutation=mutation,child_flags=0 if mutation==2 else 0x80 if mutation==3 else 0x20,adjustment=12,mode=1 if routine==4 else 0))
    for mode in (0,1,0xFEDCBA98):
        for flags in (0,8,0x10,0x18,0xFF):
            cases.append(make_fixture(original,4,lookup=0,init_flags=flags,mode=mode))
    for alias in (0,1):cases.append(make_fixture(original,5,alias=alias))
    for limit in (0,1,2,4,16):
        cases.append(make_fixture(original,6,mode=limit,first=3))
    cases.append(make_fixture(original,6,mode=2,first=1,alias=1))
    for alias in (0,1,2,3):cases.append(make_fixture(original,7,alias=alias))
    for routine in (8,9):
        for index in (0,1,3):cases.append(make_fixture(original,routine,mode=index))
    return cases


def input_hash(cases):
    keys=('routine','mutation','adjustment','args','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    n=max(len(c['events']) for c in cases)
    lines=['/* Authored resource-group observations; no original instructions or table byte arrays. */',
           'struct ResourceGroupGolden { u32 routine,mutation,adjustment,args[3],initial[%d],expected[%d],calls[%d],event_count,events[%d],result; };'%(WORDS,WORDS,len(CALLS),n),
           'static const struct ResourceGroupGolden resource_group_golden[] = {']
    for c in cases:
        array=lambda k:','.join('0x%08Xu'%(v&0xFFFFFFFF) for v in c[k])
        lines.append('    {%du,%du,0x%08Xu,{%s},{%s},{%s},{%s},%du,{%s},0x%08Xu},'%(c['routine'],c['mutation'],c['adjustment']&0xFFFFFFFF,array('args'),array('initial'),array('expected'),array('calls'),len(c['events']),array('events'),c['result']))
    return '\n'.join(lines+['};',''])


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/resource_groups_trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(args.elf)
    cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(dict(limitation=__doc__,input_sha256=input_hash(cases),cases=cases),indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d resource-group fixtures; %d original instructions; maximum %d; input SHA256 %s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))


if __name__=='__main__':main()
