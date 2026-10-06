"""Bounded ownership fixtures through six complete validated original bodies.

Execute reviewed list helper bodies as well; the preserved VU initializer has
only a caller-visible identity-store contract. Authored post-call hooks separately
stress scalar reload/store order beyond actual helper/VU effects. Free/callback
substitutes retain dead storage. No heap/VU/register/FCR/timing fidelity is claimed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_property_records import PropertyTrace
from trace_geometry import RETURN,scalar,word

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20A00
NODES=(0x20100,0x20200,0x20300)
RECORDS,TEXT,NEXT_RECORDS=0x20400,(0x20500,0x20600),0x20700
PAYLOADS,NAMES=(0x20800,0x20820,0x20840),(0x20900,0x20920)
RANGES=((0x2B9440,0x2B9530),(0x2B9660,0x2B9720),(0x2B9720,0x2B9748),
        (0x2B9AF8,0x2B9B2C),(0x2B9B30,0x2B9B68),(0x2BA1D0,0x2BA268))
HELPER_RANGES=((0x2AD9A8,0x2AD9BC),(0x2ADAE0,0x2ADB04))
ENTRIES=tuple(r[0] for r in RANGES)
CALLBACKS=(0xF00000A0,0xF00000B0)
CALLS=(0x2AD9A8,0x2A1C30,0x2ADAE0,0x2AEE40,*CALLBACKS)


class LifecycleTrace(PropertyTrace):
    def __init__(self,original,mutation=0):
        super().__init__(original)
        self.lifecycle_mutation=mutation
        self.calls=[0]*len(CALLS);self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in (*RANGES,*HELPER_RANGES)):
            raise ValueError('unreviewed lifecycle instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('lifecycle instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if not any(a<=address and address+size<=b for a,b in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('lifecycle load outside authored memory')
        return super().load(address,size)

    def save(self,address,value,size):
        if not any(a<=address and address+size<=b for a,b in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('lifecycle store outside authored memory')
        return super().save(address,value,size)

    def execute(self,instruction,pc):
        op=instruction>>26
        if op in (0x28,0x29):
            rs,rt=(instruction>>21)&31,(instruction>>16)&31
            imm=instruction&65535
            if imm&32768:imm-=65536
            address=(self.r[rs]+imm)&0xFFFFFFFF
            if op==0x29 and address%2:raise ValueError('unaligned lifecycle SH outside model')
            self.save(address,self.r[rt],1 if op==0x28 else 2)
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('lifecycle trace exceeded instruction bound')
            return None,False
        return super().execute(instruction,pc)

    def initialize_list(self,pointer):
        self.save(pointer+4,0,4)
        self.save(pointer,pointer+4,4)
        self.save(pointer+8,pointer,4)

    def event(self,index,*values):
        self.events.extend((index,*[v&0xFFFFFFFF for v in values]))

    def mutate(self,target,pointer):
        m=self.lifecycle_mutation;root,first,second=NODES
        if target==CALLS[0] and m==1:
            self.save(root,BUFFER+4,4);self.save(root+0x60,second,4)
            self.single(root+0x2C,111);self.save(root+0xC,777,4)
            self.save(root+0x1C,0xBEEF,2);self.single(root+0x48,21)
        elif target==CALLS[1] and m==2:
            self.save(root+0x1C,0xF123,2);self.single(root+0x2C,-77)
            self.save(root+0xC,1234,4);self.single(root+0x48,333)
            self.save(root+0xB0,NAMES[1],4);self.save(root+0xB4,NEXT_RECORDS,4)
        elif target==CALLS[2]:
            if m==7 and pointer==root:self.save(root+0x24,CALLBACKS[1],4)
            if m==8 and pointer==TEXT[0]:self.save(TEXT[0]+0x90,PAYLOADS[2],4)
            if m==13 and pointer==root:self.save(root+8,second,4)
        elif target==CALLS[3]:
            if m==3 and pointer==PAYLOADS[0]:
                self.save(root+0xB0,NAMES[1],4);self.initialize_list(root+0x60)
                for node in (first,second):self.save(node,0,4);self.save(node+4,0,4)
            if m==4 and pointer==NAMES[0]:
                self.save(root+0x60,second,4);self.save(second+4,root+0x60,4)
                self.save(first,0,4);self.save(first+4,0,4)
            if m==9 and pointer==RECORDS:self.save(root+0xB4,NEXT_RECORDS,4)
            if m==10 and pointer==PAYLOADS[0]:
                self.initialize_list(RECORDS);self.save(TEXT[1],0,4);self.save(TEXT[1]+4,0,4)
            if m==11 and pointer==NAMES[0]:self.save(root+0xB4,NEXT_RECORDS,4)
            if m==12 and pointer==TEXT[0]:self.save(root+0xB4,NEXT_RECORDS,4)
        elif target in CALLBACKS and pointer==first:
            if m==5:
                self.initialize_list(root+0x60);self.save(second,0,4);self.save(second+4,0,4)
            if m==6:self.save(first,first+0x64,4)

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown lifecycle controlled call')
        index=CALLS.index(target);self.calls[index]+=1;pointer=self.r[4]&0xFFFFFFFF
        if target==CALLS[0]:
            if pointer!=NODES[0]+0x60:raise ValueError('unreviewed lifecycle list initialization')
            self.event(index,pointer,*[self.load(NODES[0]+i,4) for i in (0,4,8)])
            self.run(target,self.r[31])
        elif target==CALLS[1]:
            if pointer!=NODES[0]+0x70:raise ValueError('unreviewed lifecycle VU initializer argument')
            self.event(index,pointer,self.load(NODES[0]+0x1C,2),
                       *[self.load(NODES[0]+i,4) for i in (0x2C,0x30,0x34,0x38,0x3C,0x40,0x44,0x48,0xB4,0xB0)])
            # Observed VF0-based caller-visible stores: final row, then XYZ rows.
            # This is a fixed identity output contract, not VU instruction execution.
            for row in (3,0,1,2):
                for col in range(4):self.save(pointer+(row*4+col)*4,0x3F800000 if row==col else 0,4)
        elif target==CALLS[2]:
            if pointer not in (*NODES,*TEXT):raise ValueError('unreviewed lifecycle unlink argument')
            self.event(index,pointer,self.load(pointer,4),self.load(pointer+4,4))
            self.run(target,self.r[31])
        elif target==CALLS[3]:
            if pointer not in (*NODES,*TEXT,RECORDS,*PAYLOADS,*NAMES):raise ValueError('unreviewed lifecycle free argument')
            self.event(index,pointer)
        else:
            if pointer not in NODES:raise ValueError('unreviewed lifecycle destructor argument')
            self.event(index,pointer,*[self.load(pointer+i,4) for i in (8,0xB0,0xB4,0,4,0x60)])
        self.mutate(target,pointer)

    def run(self,entry,stop=RETURN):
        body=next((r for r in (*RANGES,*HELPER_RANGES) if r[0]==entry),None)
        if body is None:raise ValueError('unsupported lifecycle entry')
        pc=entry
        while pc!=stop:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body lifecycle transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            if target is not None:
                if pc+4>=body[1]:raise ValueError('lifecycle delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('lifecycle transfer in delay slot')
                if instruction>>26==3 or instruction>>26==0 and instruction&63==9:
                    if target in ENTRIES:self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1] and (instruction!=0x03E00008 or target!=stop):
                        raise ValueError('unreviewed lifecycle return or transfer')
                    pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine=1,mode=31,mutation=0,replacement=0,coordinates=0):
    t=LifecycleTrace(original,mutation)
    for a,b in ((BUFFER,END),(0x7F000,0x81000)):
        for p in range(a,b):t.memory[p]=0x5A
    t.initialize_list(BUFFER);t.initialize_list(RECORDS);t.initialize_list(NEXT_RECORDS)
    for node in NODES:
        t.save(node,0,4);t.save(node+4,0,4);t.save(node+8,0x12345678,4)
        t.save(node+0x24,0,4);t.save(node+0xB0,0,4);t.save(node+0xB4,0,4);t.initialize_list(node+0x60)
    root,first,second=NODES
    if mode&1:
        t.save(RECORDS,TEXT[0],4);t.save(RECORDS+8,TEXT[1],4)
        for i,node in enumerate(TEXT):
            t.save(node,TEXT[1] if i==0 else RECORDS+4,4)
            t.save(node+4,RECORDS if i==0 else TEXT[0],4)
            t.save(node+0x90,PAYLOADS[i] if mode&16 else 0,4)
        t.save(root+0xB4,RECORDS,4)
    elif mode&32:t.save(root+0xB4,RECORDS,4)
    if mode&2:t.save(root+0xB0,NAMES[0],4)
    if mode&4:
        t.save(root+0x60,first,4);t.save(root+0x68,second,4)
        t.save(first,second,4);t.save(first+4,root+0x60,4)
        t.save(second,root+0x64,4);t.save(second+4,first,4)
        for node in (first,second):t.save(node+0x24,CALLBACKS[0],4)
    if mode&8:
        t.save(BUFFER,root,4);t.save(BUFFER+8,root,4)
        t.save(root,BUFFER+4,4);t.save(root+4,BUFFER,4)
    if mode&64:t.save(root+0x24,CALLBACKS[0],4)
    coords=((word(1.25),word(-2.5),word(3.75)),(0x80000000,0,word(-4.5)),
            (word(-8),word(16),word(0.25)))[coordinates]
    initial=[t.load(BUFFER+i*4,4) for i in range(640)]
    t.r[29],t.r[31]=0x80000,RETURN;t.r[4]=RECORDS if routine==5 else root
    new_records=(NEXT_RECORDS,RECORDS,0)[replacement];t.r[5]=new_records
    t.f[12],t.f[13],t.f[14]=map(scalar,coords)
    t.run(ENTRIES[routine])
    return {'routine':routine,'mode':mode,'mutation':mutation,'replacement':replacement,'coordinates':coordinates,
            'float_words':coords,'initial':initial,'expected':[t.load(BUFFER+i*4,4) for i in range(640)],
            'calls':t.calls,'events':t.events,'instruction_count':t.instruction_count}


def fixtures(original):
    cases=[]
    for coordinates in range(3):
        for mutation in (0,1,2):cases.append(make_fixture(original,0,mutation=mutation,coordinates=coordinates))
    for mode in range(32):cases.append(make_fixture(original,1,mode=mode))
    for mode in (32,63,64,95,127):cases.append(make_fixture(original,1,mode=mode))
    for mutation in (3,4,5,6,7,8,9,10,11,12):cases.append(make_fixture(original,1,mode=127,mutation=mutation))
    for mutation in (0,7,13):cases.append(make_fixture(original,2,mutation=mutation))
    for routine in (3,4):
        for mode in (0,1,17,32):
            for replacement in (0,1,2):cases.append(make_fixture(original,routine,mode=mode,replacement=replacement))
        cases.append(make_fixture(original,routine,mutation=9))
        cases.append(make_fixture(original,routine,mutation=12,replacement=1))
    for mode in (0,1,17):cases.append(make_fixture(original,5,mode=mode))
    for mutation in (8,10,12):cases.append(make_fixture(original,5,mutation=mutation))
    return cases


def input_hash(cases):
    keys=('routine','mode','mutation','replacement','coordinates','float_words','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    events=max(len(c['events']) for c in cases)
    lines=['/* Authored ownership fixtures, no original code/data arrays. */',
           'struct LifecycleGolden { u32 routine,mode,mutation,replacement,coordinates,float_words[3],initial[640],expected[640],calls[6],event_count,events[%d]; };'%events,
           'static const struct LifecycleGolden lifecycle_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join(str(c[k])+'u' for k in ('routine','mode','mutation','replacement','coordinates'))
        lines.append('    {%s,{%s},{%s},{%s},{%s},%du,{%s}},'%(prefix,array('float_words'),array('initial'),array('expected'),array('calls'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/property_lifecycle_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded ownership fixtures; %d original instructions; maximum%d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
