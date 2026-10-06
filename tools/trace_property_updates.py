"""Scoped scalar hierarchy updates with reviewed ownership-body execution.

Only authored word/call observations are exported. The preserved VU transform
has an explicit identity-quaternion output contract, not a VU emulator. Callbacks
are one-GPR node inputs and may clobber f12; no incidental floating ABI is invented.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_property_lifecycle import LifecycleTrace,RANGES as LIFE_RANGES,HELPER_RANGES,CALLS as LIFE_CALLS
from trace_geometry import RETURN,scalar,word

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20A00
NODES=(0x20100,0x20200,0x20300)
RANGES=((0x2B9628,0x2B965C),(0x2B9748,0x2B9804),(0x2B9898,0x2B9928),
        (0x2B9C08,0x2B9CE0),(0x2B9D28,0x2B9D88))
OWNERSHIP_RANGES=(LIFE_RANGES[1],LIFE_RANGES[4],LIFE_RANGES[5],HELPER_RANGES[1])
ALL_RANGES=(*RANGES,*OWNERSHIP_RANGES)
ENTRIES=tuple(r[0] for r in RANGES)
TRANSFORM=0x2A1F18
CALLBACKS=(0xF00000C0,0xF00000D0)


class UpdateTrace(LifecycleTrace):
    def __init__(self,original,mutation=0):
        super().__init__(original,mutation=0)
        self.update_mutation=mutation;self.calls=[0]*9;self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in ALL_RANGES):raise ValueError('unreviewed property-update instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('property-update instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def run(self,entry,stop=RETURN):
        body=next((r for r in ALL_RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported property-update entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body property-update transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unreviewed property-update return')
            branch=op in (1,4,5,6,7,20,21,22,23)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('property-update delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('property-update transfer in delay slot')
                if instruction==0x03E00008:
                    return
                if target is None:
                    pc+=8
                elif op==3 or op==0 and instruction&63==9:
                    if target in (0x2B9628,0x2B9748,0x2B9898,0x2B9660,0x2B9B30,0x2BA1D0):self.run(target,self.r[31])
                    else:self.library_call(target)
                    pc=self.r[31]
                else:
                    if not body[0]<=target<body[1] and (instruction!=0x03E00008 or target!=stop):
                        raise ValueError('unreviewed property-update return or transfer')
                    pc=target
            else:pc+=8 if annul else 4

    def controlled_call(self,entry,node,time=None):
        link=self.r[31];self.r[4]=node
        if time is not None:self.f[12]=time
        if entry==0x2ADAE0:self.library_call(entry)
        else:self.run(entry,link)
        self.r[31]=link

    def library_call(self,target):
        if target==TRANSFORM:
            output,rotation,position=(self.r[i]&0xFFFFFFFF for i in (4,5,6))
            node=output-0x70
            if node not in NODES or rotation!=node+0x38 or position!=node+0x2C:
                raise ValueError('unreviewed VU-transform pointer ABI')
            q=[self.load(rotation+i*4,4) for i in range(4)]
            if q!=[0,0,0,0x3F800000]:raise ValueError('VU transform outside identity-quaternion contract')
            self.calls[6]+=1;self.event(6,output,rotation,position,*q,*[self.load(position+i*4,4) for i in range(3)])
            for row in (3,0,1,2):
                for col in range(4):self.save(output+(row*4+col)*4,0x3F800000 if row==col else 0,4)
            self.save(output+48,self.load(position,4),4)
            self.save(output+52,self.load(position+4,4),4)
            z=self.load(position+8,4)
            self.save(output+60,0x3F800000,4);self.save(output+56,z,4)
        elif target in CALLBACKS:
            node=self.r[4]&0xFFFFFFFF
            if node not in NODES:raise ValueError('unreviewed property-update callback argument')
            index=7+CALLBACKS.index(target);self.calls[index]+=1
            self.event(index,node,self.load(node+0x18,4),self.load(node+0x1C,2),self.load(node+0x20,4),self.load(node+0x60,4))
            m=self.update_mutation;root,first,second=NODES
            if node==root:
                if m in (1,13):self.controlled_call(0x2B9628,node)
                if m==2:self.save(node+0x1C,0x4020,2)
                if m in (3,13):self.save(node+0x1C,0x8010,2)
                if m==4:self.save(first+0x20,0,4)
                if m==5:
                    self.controlled_call(0x2ADAE0,first)
                if m==8:self.single(node+0x18,99)
                if m==9:self.controlled_call(0x2B9748,first,99)
                if m==11:
                    for i,value in enumerate((7,-8,9)):self.single(node+0x2C+i*4,value)
                    self.save(node+0x1C,0x10,2)
                if m==14:self.save(node+0x1C,0xFFFF,2)
            if node==first:
                if m==6:self.controlled_call(0x2ADAE0,first)
                if m==7:self.controlled_call(0x2ADAE0,second)
                if m==12:self.save(second+0x20,CALLBACKS[1],4)
            # Normal callback ABI permits f12 clobbering. Child time comes from
            # saved f20, and no f12 value is included in the callback contract.
            self.f[12]=123.0
        else:return super().library_call(target)


def make_fixture(original,routine=1,flags=0xC000,callback_mask=7,destructor_mask=0,graph=0,mutation=0,time=0):
    t=UpdateTrace(original,mutation)
    for a,b in ((BUFFER,END),(0x7F000,0x81000)):
        for p in range(a,b):t.memory[p]=0x5A
    t.initialize_list(BUFFER)
    for i,node in enumerate(NODES):
        t.save(node,0,4);t.save(node+4,0,4);t.save(node+8,0x12345678,4)
        t.save(node+0x1C,flags,2);t.save(node+0x20,CALLBACKS[0] if callback_mask&(1<<i) else 0,4)
        t.save(node+0x24,LIFE_CALLS[4] if destructor_mask&(1<<i) else 0,4)
        t.save(node+0xB0,0,4);t.save(node+0xB4,0,4);t.initialize_list(node+0x60)
        t.single(node+0x18,-25)
        for j,value in enumerate((i+1,-i-2,i+3)):t.single(node+0x2C+j*4,value)
        for j in range(4):t.save(node+0x38+j*4,0x3F800000 if j==3 else 0,4)
        for j in range(16):t.single(node+0x70+j*4,(j%7)-3)
    def attach(list_address,nodes):
        t.save(list_address,nodes[0],4);t.save(list_address+8,nodes[-1],4)
        for i,node in enumerate(nodes):
            t.save(node,nodes[i+1] if i+1<len(nodes) else list_address+4,4)
            t.save(node+4,nodes[i-1] if i else list_address,4)
    root,first,second=NODES
    if graph==0:attach(BUFFER,(root,second));attach(root+0x60,(first,))
    if graph==1:attach(BUFFER,NODES)
    if graph==2:attach(BUFFER,(root,));attach(root+0x60,(first,second))
    if graph==3:attach(BUFFER,(root,));attach(root+0x60,(first,));attach(first+0x60,(second,))
    time_bits=(word(1.25),0x80000000,word(-4.5))[time]
    initial=[t.load(BUFFER+i*4,4) for i in range(640)]
    t.r[29],t.r[31]=0x80000,RETURN;t.r[4]=BUFFER if routine in (3,4) else root;t.f[12]=scalar(time_bits)
    t.run(ENTRIES[routine])
    return {'routine':routine,'flags':flags,'callback_mask':callback_mask,'destructor_mask':destructor_mask,
            'graph':graph,'mutation':mutation,'time':time,'time_bits':time_bits,'initial':initial,
            'expected':[t.load(BUFFER+i*4,4) for i in range(640)],'calls':t.calls,'events':t.events,'instruction_count':t.instruction_count}


def fixtures(original):
    cases=[]
    flag_cases=(0,1,0x10,0x20,0x30,0x4000,0x4010,0x4020,0x4030,0x8000,0x8010,0x8020,0xC000,0xC010,0xC020,0xFFFF)
    for flags in (0,0x10,0x20,0x30,0x8010,0xFFFF):
        for destructor_mask in (0,7):cases.append(make_fixture(original,0,flags=flags,destructor_mask=destructor_mask,graph=2))
    for flags in flag_cases:
        for callback_mask in (0,1,7):
            for graph in (0,2):cases.append(make_fixture(original,1,flags=flags,callback_mask=callback_mask,graph=graph))
    for mode in (0,0x4000,0x8000,0xC000):
        for low in (0,1,0x10,0x20,0x30,0x3FFF):cases.append(make_fixture(original,2,flags=mode|low))
    for flags in (0,0x10,0x20,0x30,0x8000,0xC000,0xC010,0xFFFF):
        for callback_mask in (0,7):
            for graph in range(5):cases.append(make_fixture(original,3,flags=flags,callback_mask=callback_mask,graph=graph))
    for flags in (0,0x10,0x20,0x30,0xC000,0xFFFF):
        for graph in range(5):cases.append(make_fixture(original,4,flags=flags,graph=graph))
    for routine in (1,3):
        for mutation in (1,2,3,4,5,6,7,8,9,11,12,13,14):
            cases.append(make_fixture(original,routine,mutation=mutation,graph=2 if mutation in (5,6,7,12) else 0))
        for time in (1,2):cases.append(make_fixture(original,routine,time=time,graph=3))
    return cases


def input_hash(cases):
    keys=('routine','flags','callback_mask','destructor_mask','graph','mutation','time','time_bits','initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    events=max(len(c['events']) for c in cases)
    lines=['/* Authored scalar update fixtures; no original code/table arrays. */',
           'struct PropertyUpdateGolden { u32 routine,flags,callback_mask,destructor_mask,graph,mutation,time,time_bits,initial[640],expected[640],calls[9],event_count,events[%d]; };'%events,
           'static const struct PropertyUpdateGolden property_update_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('0x%08Xu'%c[k] for k in ('routine','flags','callback_mask','destructor_mask','graph','mutation','time','time_bits'))
        lines.append('    {%s,{%s},{%s},{%s},%du,{%s}},'%(prefix,array('initial'),array('expected'),array('calls'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/property_updates_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded scalar-update fixtures; %d original instructions; maximum%d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
