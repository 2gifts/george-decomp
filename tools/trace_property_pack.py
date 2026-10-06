"""Bounded integer record-production fixtures with controlled ownership calls.

Execute two complete validated original bodies plus the reviewed actual next-
record body. Export authored buffers/events only, never original bytes. Heap,
copy and CRC calls are explicit bounded models, not full runtime/hardware proof.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib

from analyze import validated_elf
from trace_property_records import PropertyTrace
from trace_geometry import RETURN

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20800
NODES=(0x20100,0x201A0,0x20240)
NAME,DATA,ALLOCATION,CREATED,PAYLOAD=0x20300,0x20340,0x20400,0x20600,0x20700
RANGES=((0x2B9F38,0x2BA070),(0x2BA070,0x2BA10C),(0x29A890,0x29A8B4))
CALLS=(0x2AEC28,0x29C648,0x393B74,0x3934F8,0x29A890)


class PropertyPackTrace(PropertyTrace):
    def __init__(self,original,routine,mutation,failure):
        super().__init__(original)
        self.routine,self.pack_mutation,self.failure=routine,mutation,failure
        self.calls=[0]*len(CALLS);self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(start<=pc<end for start,end in RANGES):
            raise ValueError('unreviewed property-pack instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('property-pack instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if not any(start<=address and address+size<=end for start,end in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('property-pack load outside authored memory')
        return super().load(address,size)

    def save(self,address,value,size):
        if not any(start<=address and address+size<=end for start,end in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('property-pack store outside authored memory')
        return super().save(address,value,size)

    def execute(self,instruction,pc):
        op=instruction>>26;rs=(instruction>>21)&31;rt=(instruction>>16)&31
        if op in (0x28,0x29):
            immediate=instruction&0xFFFF
            if immediate&0x8000:immediate-=0x10000
            self.save((self.r[rs]+immediate)&0xFFFFFFFF,self.r[rt],1 if op==0x28 else 2)
            self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('property-pack trace exceeded instruction bound')
            self.r[0]=0
            return None,False
        return super().execute(instruction,pc)

    def bytestring(self,pointer):
        data=[]
        for index in range(128):
            value=self.load(pointer+index,1)
            if not value:return bytes(data)
            data.append(value)
        raise ValueError('unterminated property-pack string')

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown property-pack call')
        index=CALLS.index(target);self.calls[index]+=1;self.events.append(index)
        if target==0x2AEC28:
            size=self.r[4]&0xFFFFFFFF;self.events.append(size)
            self.r[2]=0 if self.failure==self.calls[0] else (ALLOCATION if not self.routine else CREATED if self.calls[0]==1 else PAYLOAD)
            if not self.routine and self.pack_mutation==1 and self.calls[0]==1:
                self.save(BUFFER,NODES[1],4);self.save(NODES[1],BUFFER+4,4)
            if self.routine and self.pack_mutation==7 and self.calls[0]==2:
                self.save(CREATED+0x88,0xFEDCBA98,4);self.save(CREATED+0x8C,1,4)
        elif target==0x29C648:
            pointer=self.r[4]&0xFFFFFFFF;self.events.append(pointer-BUFFER)
            self.r[2]=zlib.crc32(self.bytestring(pointer))&0xFFFFFFFF
            if self.calls[1]==1:
                if self.pack_mutation==2:self.save(NODES[0],BUFFER+4,4)
                if self.pack_mutation==3:
                    self.save(NODES[0]+0x88,0x12345678,4);self.save(NODES[0]+0x8C,4,4);self.save(NODES[0]+0x90,DATA+16,4)
        elif target==0x393B74:
            dest,source=self.r[4]&0xFFFFFFFF,self.r[5]&0xFFFFFFFF
            self.events.extend((dest-BUFFER,source-BUFFER))
            data=self.bytestring(source)+b'\0'
            for index,value in enumerate(data):self.save(dest+index,value,1)
            self.r[2]=dest
            if self.pack_mutation==6:
                self.save(CREATED,DATA,4);self.save(CREATED+4,NAME,4)
                self.save(CREATED+0x88,0xCAFEBABE,4);self.save(CREATED+0x8C,99,4);self.save(CREATED+0x90,DATA,4)
        elif target==0x3934F8:
            dest,source,size=(self.r[i]&0xFFFFFFFF for i in (4,5,6))
            self.events.extend((dest-BUFFER,source-BUFFER,size))
            if size>128:raise ValueError('copy outside authored property-pack extent')
            # Controlled valid copies are disjoint, like their native memcpy.
            if size and max(dest,source)<min(dest+size,source+size):raise ValueError('overlapping controlled copy')
            data=[self.load(source+i,1) for i in range(size)]
            for index,value in enumerate(data):self.save(dest+index,value,1)
            self.r[2]=dest
            if not self.routine and self.calls[3]==1:
                if self.pack_mutation==4:self.save(dest-2,24,2)
                if self.pack_mutation==5:
                    self.save(dest-3,0x80,1);self.save(NODES[1],0,4)
            if self.routine and self.pack_mutation==8:self.save(CREATED+0x90,DATA,4)
        else:
            self.events.append((self.r[4]&0xFFFFFFFF)-BUFFER)
            self.run(0x29A890,self.r[31])

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported property-pack entry')
        pc=entry
        while pc!=stop:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body property-pack transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            if target is not None:
                if pc+4>=body[1]:raise ValueError('property-pack delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('property-pack transfer in delay slot')
                if instruction>>26==3:
                    self.library_call(target);pc=self.r[31]
                else:
                    if not body[0]<=target<body[1] and (instruction!=0x03E00008 or target!=stop):
                        raise ValueError('unreviewed property-pack return or cross-body transfer')
                    pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine=0,count=3,length=4,kind=0x12345678,mutation=0,failure=0,alias=0):
    t=PropertyPackTrace(original,routine,mutation,failure)
    for start,end in ((BUFFER,END),(0x7F000,0x81000)):
        for p in range(start,end):t.memory[p]=0x5A
    t.save(BUFFER,NODES[0] if count else BUFFER+4,4);t.save(BUFFER+4,0,4);t.save(BUFFER+8,NODES[min(max(count-1,0),2)] if count else BUFFER,4)
    for index,node in enumerate(NODES):
        next_node=NODES[index+1] if index+1<min(count,3) else BUFFER+4
        t.save(node,next_node,4);t.save(node+4,BUFFER if not index else NODES[index-1],4)
        for offset,value in enumerate(('Record%d'%index).encode()+b'\0'):t.save(node+8+offset,value,1)
        t.save(node+0x88,kind+index,4);t.save(node+0x8C,length,4);t.save(node+0x90,DATA+index*32,4)
    if count==4:t.save(NODES[0],0,4)  # Authored non-sentinel head with NULL next.
    for offset,value in enumerate(b'Created Name\0'):t.save(NAME+offset,value,1)
    for index in range(128):t.save(DATA+index,(index*17+3)&255,1)
    output_size=(BUFFER+16,NODES[0]+0x8C,NODES[1]+0x88,BUFFER+4)[alias]
    initial=[t.load(BUFFER+i*4,4) for i in range(512)]
    t.r[29],t.r[31]=0x80000,RETURN
    if routine:t.r[4],t.r[5],t.r[6],t.r[7]=NAME,kind,length,DATA
    else:t.r[4],t.r[5]=BUFFER,output_size
    t.run(0x2BA070 if routine else 0x2B9F38)
    return {'routine':routine,'count':count,'length':length,'kind':kind,'mutation':mutation,'failure':failure,'alias':alias,
            'initial':initial,'expected':[t.load(BUFFER+i*4,4) for i in range(512)],'result':(t.r[2]-BUFFER)&0xFFFFFFFF if t.r[2] else 0,
            'calls':t.calls,'events':t.events,'instruction_count':t.instruction_count}


def fixtures(original):
    cases=[]
    for count in range(5):
        for length in (0,4,12,28):
            for failure in (0,1):cases.append(make_fixture(original,count=count,length=length,failure=failure))
    for alias in range(1,4):
        for length in (4,12):cases.append(make_fixture(original,length=length,alias=alias))
    for mutation in range(1,6):cases.append(make_fixture(original,mutation=mutation))
    for count in (1,2,3):
        for length in (0xFFFFFFFF,0xFFFFFFF8,0x80000000):cases.append(make_fixture(original,count=count,length=length,failure=1))
    for length in (0,4,16,28):
        for kind in (0,0xFFFFFFFF,0x12345678):
            for failure in (0,1,2):cases.append(make_fixture(original,1,length=length,kind=kind,failure=failure))
    for mutation in (6,7,8):cases.append(make_fixture(original,1,mutation=mutation))
    for length in (0xFFFFFFFF,0x80000000):cases.append(make_fixture(original,1,length=length,failure=2))
    return cases


def input_hash(cases):
    keys=('routine','count','length','kind','mutation','failure','alias','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    maximum=max(len(c['events']) for c in cases)
    lines=['/* Authored integer property-pack words; no original code/table/assets. */',
           'struct PropertyPackGolden { u32 routine,count,length,kind,mutation,failure,alias,initial[512],expected[512],result,calls[5],event_count,events[%d]; };'%maximum,
           'static const struct PropertyPackGolden property_pack_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('0x%08Xu'%c[k] for k in ('routine','count','length','kind','mutation','failure','alias'))
        lines.append('    {%s,{%s},{%s},0x%08Xu,{%s},%d,{%s}},'%(prefix,array('initial'),array('expected'),c['result'],array('calls'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/property_pack_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d controlled property-pack fixtures; %d instructions; maximum%d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
