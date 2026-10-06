"""Bounded property hierarchy fixtures from complete validated original bodies.

Execute actual recursive bodies and delays, exporting authored integer memory
and controlled comparator/callback events only. This is not a full runtime or
hardware model; capacity-overshoot paths receive sufficient authored storage.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_property_records import PropertyTrace
from trace_geometry import RETURN

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20A00
NODES=tuple(0x20100+i*0x100 for i in range(5))
OUTPUT,NAMES,KEYS,DATA=0x20600,0x20700,0x20800,0x20900
RANGES=((0x2B9928,0x2B99AC),(0x2B99B0,0x2B9A54),(0x2B9A58,0x2B9AF4),
        (0x2B9B68,0x2B9C04),(0x2B9D88,0x2B9E34),(0x2B9E38,0x2B9F34))
ENTRIES=tuple(r[0] for r in RANGES)
CALLBACK=0xF00000A0
CALLS=(0x393A28,0x398628,CALLBACK)


class HierarchyTrace(PropertyTrace):
    def __init__(self,original,mutation=0,stop=0xFFFFFFFF,callback_value=7):
        super().__init__(original)
        self.hierarchy_mutation,self.stop,self.callback_value=mutation,stop,callback_value
        self.calls=[0]*3;self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(start<=pc<end for start,end in RANGES):raise ValueError('unreviewed hierarchy instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('hierarchy instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if not any(start<=address and address+size<=end for start,end in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('hierarchy load outside authored memory')
        return super().load(address,size)

    def save(self,address,value,size):
        if not any(start<=address and address+size<=end for start,end in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('hierarchy store outside authored memory')
        return super().save(address,value,size)

    def execute(self,instruction,pc):
        if instruction>>26==0 and instruction&63==0x23:
            if (instruction>>6)&31:raise ValueError('reserved hierarchy SUBU operand')
            rs,rt,rd=(instruction>>21)&31,(instruction>>16)&31,(instruction>>11)&31
            self.r[rd]=(self.r[rs]-self.r[rt])&0xFFFFFFFF;self.r[0]=0
            self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('hierarchy trace exceeded instruction bound')
            return None,False
        return super().execute(instruction,pc)

    def bytestring(self,pointer):
        result=[]
        for i in range(64):
            value=self.load(pointer+i,1)
            if not value:return bytes(result)
            result.append(value)
        raise ValueError('unterminated hierarchy controlled string')

    def mutate(self,index,callback):
        if callback:
            if self.hierarchy_mutation==1 and index==0:self.save(NODES[0]+0x60,NODES[2],4)
            if self.hierarchy_mutation==2 and index==1:self.save(NODES[1],0,4)
            if self.hierarchy_mutation==3 and index==1:self.save(NODES[2],0,4)
        else:
            if self.hierarchy_mutation==4 and index==0:self.save(NODES[0]+0x60,NODES[2],4)
            if self.hierarchy_mutation==5 and index==1:self.save(NODES[1],0,4)

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown hierarchy controlled call')
        index=CALLS.index(target);self.calls[index]+=1;self.events.append(index)
        if index<2:
            first,second=self.r[4]&0xFFFFFFFF,self.r[5]&0xFFFFFFFF
            self.events.extend((first-BUFFER,second-BUFFER))
            a,b=self.bytestring(first),self.bytestring(second)
            if index==0:self.r[2]=((a>b)-(a<b))&0xFFFFFFFF
            else:
                found=a.find(b);self.r[2]=first+found if found>=0 else 0
            node_index=(first-NAMES)//32
            if not 0<=node_index<5:raise ValueError('unreviewed hierarchy name pointer')
            self.mutate(node_index,False)
        else:
            node,data=self.r[4]&0xFFFFFFFF,self.r[5]&0xFFFFFFFF
            if node not in NODES or data!=DATA:raise ValueError('invalid hierarchy controlled callback arguments')
            node_index=NODES.index(node);self.events.extend((node-BUFFER,data-BUFFER))
            self.mutate(node_index,True)
            self.r[2]=0 if node_index==self.stop else self.callback_value&0xFFFFFFFF

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported hierarchy entry')
        pc=entry
        while pc!=stop:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body hierarchy transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            if target is not None:
                if pc+4>=body[1]:raise ValueError('hierarchy delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('hierarchy transfer in delay slot')
                if instruction>>26==3 or instruction>>26==0 and instruction&63==9:
                    if target in ENTRIES:self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1] and (instruction!=0x03E00008 or target!=stop):
                        raise ValueError('unreviewed hierarchy return or transfer')
                    pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine=0,key=0,shape=0,mutation=0,alias=0,flags=3,remaining=2,stop=0xFFFFFFFF,callback_value=7):
    t=HierarchyTrace(original,mutation,stop,callback_value)
    for start,end in ((BUFFER,END),(0x7F000,0x81000)):
        for address in range(start,end):t.memory[address]=0x5A
    t.save(BUFFER,NODES[0],4);t.save(BUFFER+4,0,4);t.save(BUFFER+8,NODES[4],4)
    names=(b'root',b'alpha',b'beta-alpha',b'gamma',b'delta')
    for index,node in enumerate(NODES):
        t.save(node,BUFFER+4,4);t.save(node+4,BUFFER,4);t.save(node+0xC,(1,2,1,1,2)[index],4)
        t.save(node+0x60,node+0x64,4);t.save(node+0x64,0,4);t.save(node+0x68,node+0x60,4)
        t.save(node+0xB0,0 if shape==3 or shape==2 and index==0 else NAMES+index*32,4)
        for offset,value in enumerate(names[index]+b'\0'):t.save(NAMES+index*32+offset,value,1)
    t.save(NODES[0],NODES[4],4)
    if shape not in (1,4):
        t.save(NODES[0]+0x60,NODES[1],4);t.save(NODES[0]+0x68,NODES[2],4)
        t.save(NODES[1],NODES[2],4);t.save(NODES[2],NODES[0]+0x64,4)
        t.save(NODES[1]+0x60,NODES[3],4);t.save(NODES[1]+0x68,NODES[3],4);t.save(NODES[3],NODES[1]+0x64,4)
    if shape==4:t.save(BUFFER,BUFFER+4,4);t.save(BUFFER+8,BUFFER,4)
    for index,text in enumerate((b'alpha',b'root',b'missing',b'',b'delta')):
        for offset,value in enumerate(text+b'\0'):t.save(KEYS+index*16+offset,value,1)
    output=(OUTPUT,NODES[2]+0xC,NODES[0]+0x68,NODES[0]+0xC)[alias]
    initial=[t.load(BUFFER+i*4,4) for i in range(640)]
    t.r[29],t.r[31]=0x80000,RETURN
    t.r[4]=BUFFER if routine in (4,5) else NODES[0]
    t.r[5]=KEYS+key*16 if routine in (0,1,4) else key if routine in (2,5) else flags
    if routine==1:t.r[6]=output
    if routine in (2,5):t.r[6],t.r[7]=remaining,output
    if routine==3:t.r[6],t.r[7]=CALLBACK,DATA
    t.run(ENTRIES[routine]);result=t.r[2]&0xFFFFFFFF
    if routine in (0,4) and result:result-=BUFFER
    return {'routine':routine,'key':key,'shape':shape,'mutation':mutation,'alias':alias,'flags':flags,'remaining':remaining,'stop':stop,'callback_value':callback_value,
            'initial':initial,'expected':[t.load(BUFFER+i*4,4) for i in range(640)],'result':result,'calls':t.calls,'events':t.events,'instruction_count':t.instruction_count}


def fixtures(original):
    cases=[]
    for routine in (0,1,4):
        for key in range(5):
            for shape in (0,1,2,3,4):cases.append(make_fixture(original,routine,key=key,shape=shape))
    for routine in (2,5):
        for key in (1,2,3,0xFFFFFFFF):
            for remaining in (0,1,2,10):cases.append(make_fixture(original,routine,key=key,remaining=remaining))
        for alias in (1,2,3):cases.append(make_fixture(original,routine,key=1,remaining=0,alias=alias))
    for flags in (0,1,2,3,0xFFFFFFFF):
        for stop in (0xFFFFFFFF,0,1,2,3,4):cases.append(make_fixture(original,3,flags=flags,stop=stop))
    for mutation in (1,2,3):cases.append(make_fixture(original,3,mutation=mutation))
    for routine in (0,1,4):
        for mutation in (4,5):cases.append(make_fixture(original,routine,key=2,mutation=mutation))
    cases.append(make_fixture(original,3,callback_value=0xFFFFFFFD))
    return cases


def input_hash(cases):
    keys=('routine','key','shape','mutation','alias','flags','remaining','stop','callback_value','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    maximum=max(len(c['events']) for c in cases)
    lines=['/* Authored bounded hierarchy words; no original code/table/assets. */',
           'struct HierarchyGolden { u32 routine,key,shape,mutation,alias,flags,remaining,stop,callback_value,initial[640],expected[640],result,calls[3],event_count,events[%d]; };'%maximum,
           'static const struct HierarchyGolden hierarchy_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('0x%08Xu'%c[k] for k in ('routine','key','shape','mutation','alias','flags','remaining','stop','callback_value'))
        lines.append('    {%s,{%s},{%s},0x%08Xu,{%s},%d,{%s}},'%(prefix,array('initial'),array('expected'),c['result'],array('calls'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/property_hierarchy_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded hierarchy fixtures; %d instructions; maximum%d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
