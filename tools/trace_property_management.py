"""Bounded property-management fixtures from complete original scalar bodies.

Reuses the reviewed finite scalar decoder. Actual constructor/list/length bodies
and the original byte-copy branch execute; aligned strcpy MMI paths are rejected.
Allocations, free and VU identity initialization have explicit authored contracts.
Retained dead storage and post-call mutations stress observations, not heap/VU/
register/FCR/timing fidelity or valid behavior for dangling/overlapping strings.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_property_updates import UpdateTrace
from trace_geometry import RETURN,scalar,word

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20A00
NODE,RECORD,LIST,NEW,OLD,INPUT,ALTERNATE=0x20100,0x20500,0x20700,0x20801,0x20901,0x20941,0x20981
RANGES=((0x2B9530,0x2B95B4),(0x2B95B8,0x2B9624),(0x2BA110,0x2BA16C),
        (0x2BA170,0x2BA1AC),(0x2BA1B0,0x2BA1CC),(0x2BA268,0x2BA27C))
HELPER_RANGES=((0x2B9440,0x2B9530),(0x2AD9A8,0x2AD9BC),(0x2AD9C0,0x2AD9E0),
               (0x2ADAE0,0x2ADB04),(0x295050,0x295080),(0x393B74,0x393C8C))
ALL_RANGES=(*RANGES,*HELPER_RANGES)
ENTRIES=tuple(a for a,b in RANGES)
CALLS=(0x2AEB60,0x2AEC28,0x2AD9A8,0x2A1C30,0x2AEE40,0x295050,0x393B74,0x2ADAE0,0x2AD9C0)


class ManagementTrace(UpdateTrace):
    def __init__(self,original,routine=0,failure=0,mutation=0):
        super().__init__(original)
        self.routine,self.failure,self.management_mutation=routine,failure,mutation
        self.calls=[0]*len(CALLS);self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in ALL_RANGES):raise ValueError('unreviewed property-management instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('property-management instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,instruction>>21&31,instruction>>16&31,instruction>>11&31,instruction&63
        shamt=instruction>>6&31
        if op==0x20:
            imm=instruction&65535;imm-=65536 if imm&32768 else 0
            value=self.load((self.r[rs]+imm)&0xFFFFFFFF,1)
            self.r[rt]=(value-256 if value&128 else value)&0xFFFFFFFF
        elif op==0 and fn==0x26:
            if shamt:raise ValueError('reserved property-management XOR operand')
            self.r[rd]=(self.r[rs]^self.r[rt])&0xFFFFFFFF
        else:return super().execute(instruction,pc)
        self.r[0]=0;self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('property-management instruction bound exceeded')
        return None,False

    def run(self,entry,stop=RETURN):
        body=next((r for r in ALL_RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported property-management entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body property-management transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unreviewed property-management return')
            branch=op in (1,4,5,6,7,20,21,22,23)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('property-management delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('property-management transfer in delay slot')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif op==3 or op==0 and instruction&63==9:
                    if target in (0x2B9440,0x2B95B8):self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed property-management transfer')
                    pc=target
            else:pc+=8 if annul else 4

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown property-management controlled call')
        index=CALLS.index(target);self.calls[index]+=1;pointer=self.r[4]&0xFFFFFFFF
        m=self.management_mutation
        if target==CALLS[0]:
            if pointer!=0xC0 or self.r[5]!=4:raise ValueError('unreviewed property-node allocator arguments')
            self.event(index,pointer,self.r[5]);self.r[2]=0 if self.failure==1 else NODE
            if m==1:self.save(INPUT,ord('M'),1)
            # Real calls may clobber argument registers; the factory must retain
            # its incoming name and coordinates independently of those values.
            for i in range(4,16):self.r[i]=0xDEADBEEF
            self.f[12],self.f[13],self.f[14]=77.0,88.0,99.0
        elif target==CALLS[1]:
            self.event(index,pointer,self.load(NODE+0xB0,4))
            if self.routine==3:
                if pointer!=12:raise ValueError('unreviewed list allocation size')
                self.r[2]=0 if self.failure==1 else LIST
            else:
                if pointer>64:raise ValueError('name allocation outside controlled capacity')
                self.r[2]=0 if self.failure==2 else NEW
                if m==3:self.save(NODE+0xB0,ALTERNATE,4);self.save(INPUT,ord('N'),1)
        elif target==CALLS[2]:
            if pointer not in (NODE+0x60,LIST):raise ValueError('unreviewed management list initialization')
            self.event(index,pointer,self.load(NODE,4),self.load(NODE+4,4),self.load(NODE+8,4))
            self.run(target,self.r[31])
            # Authored post-helper hook beyond the pure helper's own effects.
            if m==5:self.save(NODE+8,RECORD,4);self.save(NODE+0x48,0x41424344,4)
            self.r[2]=0x12345678
        elif target==CALLS[3]:
            if pointer!=NODE+0x70:raise ValueError('unreviewed management VU initializer')
            self.event(index,pointer,self.load(NODE+0x1C,2),*[self.load(NODE+i,4) for i in (0x2C,0x30,0x34,0x38,0x3C,0x40,0x44,0x48)])
            for row in (3,0,1,2):
                for col in range(4):self.save(pointer+(row*4+col)*4,0x3F800000 if row==col else 0,4)
            self.r[2]=0x87654321
        elif target==CALLS[4]:
            if not BUFFER<=pointer<END:raise ValueError('free outside authored storage')
            self.event(index,pointer,self.load(NODE+0xB0,4),self.load(RECORD+0x90,4))
            if m==2 and pointer==OLD:self.save(NODE+0xB0,ALTERNATE,4);self.save(INPUT,ord('Q'),1)
            if m==6 and pointer!=RECORD:self.save(RECORD+0x90,ALTERNATE,4)
        elif target==CALLS[5]:
            if not BUFFER<=pointer<END:raise ValueError('length outside authored storage')
            self.event(index,pointer,self.load(NODE+0xB0,4));self.run(target,self.r[31])
            self.event(index,self.r[2])
        elif target==CALLS[6]:
            source=self.r[5]&0xFFFFFFFF
            # Execute only the authentic scalar copy branch. Aligned MMI/64-bit
            # paths are outside this decoder; silently substituting is forbidden.
            if not (pointer|source)&7:raise ValueError('aligned strcpy outside scalar branch')
            self.event(index,pointer,source,self.load(NODE+0xB0,4))
            self.run(target,self.r[31])
        elif target==CALLS[7]:
            if pointer!=RECORD:raise ValueError('unreviewed management unlink')
            self.event(index,pointer,self.load(pointer,4),self.load(pointer+4,4),self.load(pointer+0x90,4))
            self.run(target,self.r[31])
            if m==4:self.save(RECORD+0x90,ALTERNATE,4)
        else:
            if pointer!=LIST or self.r[5] not in (NODE,RECORD):raise ValueError('unreviewed management tail append')
            self.event(index,pointer,self.r[5],self.load(LIST,4),self.load(LIST+8,4))
            self.run(target,self.r[31])


def make_fixture(original,routine=0,failure=0,mutation=0,name=1,previous=0,attached=0,payload=1,coordinates=0):
    t=ManagementTrace(original,routine,failure,mutation)
    for a,b in ((BUFFER,END),(0x7F000,0x81000)):
        for p in range(a,b):t.memory[p]=0x5A
    for p in (NODE,RECORD):t.save(p,0,4);t.save(p+4,0,4)
    t.initialize_list(LIST);t.initialize_list(NODE+0x60)
    t.save(NODE+0xB0,OLD if previous else 0,4);t.save(NODE+0xB4,0,4)
    t.save(RECORD+0x90,OLD if payload else 0,4)
    for address,data in ((OLD,b'old-name\0'),(INPUT,b'new-\x80name\0'),(INPUT+32,b'\0'),(ALTERNATE,b'other-name\0'),(NODE+0x48,b'field-alias\0')):
        for i,value in enumerate(data):t.save(address+i,value,1)
    if attached:
        t.save(LIST,RECORD,4);t.save(LIST+8,RECORD,4)
        t.save(RECORD,LIST+4,4);t.save(RECORD+4,LIST,4)
    if routine==5:
        t.save(LIST,(LIST+4,RECORD,0,LIST)[attached],4)
    names=(0,INPUT,OLD,OLD+1,NODE+0x48,INPUT+32)
    floats=((word(1.25),word(-2.5),word(3.75)),(0x80000000,0,word(-4.5)),(word(-8),word(16),word(0.25)))[coordinates]
    initial=[t.load(BUFFER+i*4,4) for i in range(640)]
    t.r[29],t.r[31]=0x80000,RETURN
    t.r[4]=names[name] if routine==0 else NODE if routine==1 else RECORD if routine==2 else LIST
    t.r[5]=names[name] if routine==1 else NODE
    t.f[12],t.f[13],t.f[14]=map(scalar,floats)
    t.run(ENTRIES[routine])
    result=t.r[2]&0xFFFFFFFF if routine in (0,3,5) else 0
    return dict(routine=routine,failure=failure,mutation=mutation,name=name,previous=previous,attached=attached,payload=payload,
                coordinates=coordinates,float_words=floats,initial=initial,expected=[t.load(BUFFER+i*4,4) for i in range(640)],
                result=result,calls=t.calls,events=t.events,instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for coordinates in range(3):
        for name in (0,1,4,5):
            for failure in (0,1,2):cases.append(make_fixture(original,coordinates=coordinates,name=name,failure=failure))
    for mutation in (1,3,5):cases.append(make_fixture(original,mutation=mutation))
    for previous in (0,1):
        for name in range(6):
            for failure in (0,2):cases.append(make_fixture(original,1,previous=previous,name=name,failure=failure))
    for mutation in (2,3):
        for name in (0,1,2):cases.append(make_fixture(original,1,previous=1,name=name,mutation=mutation))
    for attached in (0,1):
        for payload in (0,1):
            for mutation in (0,4,6):cases.append(make_fixture(original,2,attached=attached,payload=payload,mutation=mutation))
    for failure in (0,1):
        for mutation in (0,5):cases.append(make_fixture(original,3,failure=failure,mutation=mutation))
    for attached in (0,1):cases.append(make_fixture(original,4,attached=attached))
    for attached in range(4):cases.append(make_fixture(original,5,attached=attached))
    return cases


def input_hash(cases):
    keys=('routine','failure','mutation','name','previous','attached','payload','coordinates','float_words','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    n=max(len(c['events']) for c in cases)
    lines=['/* Authored management buffers/events; no original code/data arrays. */',
           'struct ManagementGolden { u32 routine,failure,mutation,name,previous,attached,payload,coordinates,float_words[3],initial[640],expected[640],result,calls[9],event_count,events[%d]; };'%n,
           'static const struct ManagementGolden management_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('%du'%c[k] for k in ('routine','failure','mutation','name','previous','attached','payload','coordinates'))
        lines.append('    {%s,{%s},{%s},{%s},0x%08Xu,{%s},%du,{%s}},'%(prefix,array('float_words'),array('initial'),array('expected'),c['result'],array('calls'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/property_management_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded management fixtures; %d original instructions; maximum %d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
