"""Bounded road-query originals with real provider/bounds/registry/GNU runtime.

Finite normal/zero separately rounded binary32 arithmetic only; no EE FCR,
exceptional/subnormal arithmetic, timing or memory-fault equivalence claim.
Output bytes and caller scratch remain unknown until actually initialized.
There are no callback/library substitutes or fabricated query outputs.
"""
import argparse
import hashlib
import json
from pathlib import Path
import random
import struct

from analyze import validated_elf
from trace_geometry import Trace, RETURN, word, scalar, is_control_transfer
from trace_actor_collision import CollisionTrace
from trace_spatial_queries import signed

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x1CC710,0x1CC7B4),(0x1CE620,0x1CEA80),(0x1CEA80,0x1CEDA8))
HELPERS=((0x1CCA28,0x1CCAA4),(0x2A8250,0x2A8308),(0x2A00A0,0x2A0138),
         (0x37AF00,0x37AFD4),(0x374748,0x3747E0),(0x373DB8,0x373E48))
INTERVALS=(*RANGES,*HELPERS)
ENTRIES=tuple(a for a,b in RANGES)
BUFFER,END=0x30000,0x35000
MANAGER,PROVIDERS,HEADERS,ROADS,RECORDS,ALT_RECORDS=(BUFFER+p for p in (0,0x40,0x100,0x200,0x434,0x700))
INDEX,CELL,OUTPUT,POINT,NEIGHBORS=(BUFFER+p for p in (0xA00,0xB00,0xC00,0xD00,0xE00))
POSITIONS=BUFFER+0x1000
FAR_RECORD=RECORDS+0xFFFF*0x34
GLOBAL=0x3F8C28
SNAPSHOT=tuple(range(BUFFER,END,4))+tuple(range(FAR_RECORD,FAR_RECORD+0x34,4))
MEMORY_RANGES=((BUFFER,END),(FAR_RECORD,FAR_RECORD+0x34),(GLOBAL,GLOBAL+4),(0x7E000,0x81000))
POINTER_CELLS=tuple([MANAGER+8+i*4 for i in range(3)]+
 [PROVIDERS+i*0x20+0x10 for i in range(3)]+
 [HEADERS+i*0x40+0x2C for i in range(3)]+
 [ROADS+i*0x60+p for i in range(3) for p in (0x24,0x30,0x34,0x38,0x48)]+[CELL+4,CELL+8,OUTPUT])

class RoadTrace(CollisionTrace):
    def __init__(self,original):
        super().__init__(original)
        self.invocations=[0]*len(INTERVALS)
        self.store_events=[]
    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('road memory outside aligned observer windows')
    def load(self,address,size):
        self.memory_check(address,size)
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized road memory/output read')
        return Trace.load(self,address,size)
    def save(self,address,value,size):
        self.memory_check(address,size)
        if self.frames and any(a<=address< b for a,b in MEMORY_RANGES[:2]):
            self.store_events.extend((address,size,value&((1<<(size*8))-1)))
        return Trace.save(self,address,value,size)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in INTERVALS):
            raise ValueError('unreviewed road instruction or excluded neighboring negate')
        offset=pc-0xFF000
        if not 0<=offset<=len(self.original)-4:raise ValueError('road instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]
    def execute(self,w,pc):
        op,rs,rt,rd,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w&63
        sh=w>>6&31;imm=w&65535;simm=imm-65536 if imm&32768 else imm
        self.visited.add(pc)
        if self.instruction_count>=150000:raise ValueError('road instruction bound')
        if op==0 and fn in (4,6,7,0x27):
            if sh:raise ValueError('reserved road variable shift/NOR encoding')
            if fn==4:self.r[rd]=(self.r[rt]<< (self.r[rs]&31))&0xFFFFFFFF
            elif fn==6:self.r[rd]=(self.r[rt]&0xFFFFFFFF)>>(self.r[rs]&31)
            elif fn==7:self.r[rd]=(signed(self.r[rt])>>(self.r[rs]&31))&0xFFFFFFFF
            else:self.r[rd]=~(self.r[rs]|self.r[rt])&0xFFFFFFFF
            self.instruction_count+=1;self.r[0]=0;return None,False
        if op==1 and rt in (2,3):
            taken=signed(self.r[rs])<0 if rt==2 else signed(self.r[rs])>=0
            self.instruction_count+=1;self.r[0]=0
            return ((pc+4+simm*4)&0xFFFFFFFF,False) if taken else (None,True)
        if op==17 and rs==0:
            if w&0x7FF:raise ValueError('reserved road MFC1 encoding')
            self.r[rt]=word(self.f[rd]);self.instruction_count+=1;self.r[0]=0;return None,False
        return super().execute(w,pc)
    def library_call(self,target):
        raise ValueError('road helpers require complete original execution')
    def run(self,entry,stop=RETURN):
        body=next((p for p in INTERVALS if p[0]==entry),None)
        if body is None or len(self.frames)>=16:raise ValueError('road requires approved whole entry')
        self.invocations[tuple(a for a,b in INTERVALS).index(entry)]+=1
        self.frames.append(entry);pc=entry
        try:
            while True:
                if not body[0]<=pc<body[1]:raise ValueError('road transfer outside complete body')
                w=self.fetch(pc);target,annul=self.execute(w,pc)
                if is_control_transfer(w):
                    if not annul:
                        if pc+4>=body[1]:raise ValueError('road delay outside body')
                        delay=self.fetch(pc+4)
                        if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                            raise ValueError('road control transfer in delay')
                    if w>>26==0 and w&63==8:
                        if w!=0x03E00008 or target!=stop:raise ValueError('road requires actual JR31 return')
                        return
                    if w>>26==3:
                        continuation=self.r[31]
                        self.run(target,continuation)
                        if self.r[31]!=continuation:raise ValueError('road callee changed RA')
                        pc=continuation
                    else:
                        if target is not None and not body[0]<=target<body[1]:raise ValueError('road branch crosses body')
                        pc=target if target is not None else pc+8
                else:pc+=4
        finally:self.frames.pop()


def scene(original,routine=2,point=(0,0,0),radius=0.0,threshold=0.0,index=0,count=2,
          providers=1,disabled=0,missing=0,road_zero=False,cell_count=1,lazy=False,
          registry=True,neighbors=(),numbers=(0,),vertex_output=OUTPUT+4,
          mode_output=OUTPUT+8,road_output=OUTPUT,shape=0,span=80.0,shift_records=False,run=True):
    t=RoadTrace(original)
    for a,b in MEMORY_RANGES[:3]:
        for p in range(a,b):t.memory[p]=0
    t.save(GLOBAL,MANAGER,4);t.save(MANAGER,providers,1)
    for i in range(3):
        provider,header,road=PROVIDERS+i*0x20,HEADERS+i*0x40,ROADS+i*0x60
        t.save(MANAGER+8+i*4,provider,4);t.save(provider+2,0 if disabled>>i&1 else 4,1)
        t.save(provider+0x10,header,4);t.save(header,i<<16,4)
        for j,v in enumerate((-40,-40,-40,span-40,40,40)):t.single(header+4+j*4,v)
        t.save(header+0x2C,0 if missing>>i&1 else road,4)
        t.save(road,0 if road_zero else 1,4)
        t.save(road+0x24,NEIGHBORS,4);t.save(road+0x30,CELL,4)
        t.save(road+0x34,INDEX,4);t.save(road+0x38,RECORDS if i==0 else ALT_RECORDS,4)
        t.save(road+0x48,POSITIONS,4)
    for i in range(-1,5):
        for records in (RECORDS,ALT_RECORDS):
            record=records+i*0x34
            t.save(record,i&65535,2);t.save(record+5,2,1)
            for off,base in ((8,0),(12,256),(10,512),(14,768)):
                t.save(record+off,base+(1 if shift_records and i==1 else 0),2)
    t.save(FAR_RECORD,0xFFFF,2);t.save(FAR_RECORD+5,2,1)
    for i in range(256):
        x=-2+i*4
        for base,z in ((0,-2),(256,2),(512,-1),(768,1)):
            if shape==1:z=0
            if shape==2:x=0
            if shape==3:x=x*0.01;z=z*0.01
            p=POSITIONS+(base+i)*12
            for j,v in enumerate((x,0,z)):t.single(p+j*4,v)
    for j,v in enumerate(point):t.single(POINT+j*4,v)
    t.save(INDEX,1,4);t.save(INDEX+4,1 if registry else 0,4)
    t.save(INDEX+8,0x20,4);t.save(INDEX+12,0x40,4);t.save(INDEX+16,12,4)
    t.save(INDEX+0x20,0,4)
    # Calculate only the authored registry key for synthetic input construction;
    # the selected execution still runs genuine ceil/fptoui/lookup originals.
    import math
    columns=(int(math.ceil(span))//40)+(int(math.ceil(span))%40!=0) if span>0 else 0
    x=max(0,int(point[0]+40))//40;z=max(0,int(point[2]+40))//40
    t.save(INDEX+0x40,(x+z*columns)&0xFFFFFFFF,4);t.save(INDEX+0x44,0,4)
    t.save(CELL,cell_count,2);t.save(CELL+0xC,not lazy,1)
    t.save(CELL+4,CELL+0x10,4);t.save(CELL+8,CELL+0x10+(cell_count+1)*2,4)
    for i,n in enumerate(numbers):
        t.save(CELL+0x10+2*i,2*i,2)
        t.save(CELL+0x10+(cell_count+1)*2+2*i,n,2)
    t.save(ROADS+6,len(neighbors),2)
    for i,key in enumerate(neighbors):t.save(NEIGHBORS+i*4,key,4)
    t.save(OUTPUT,0xCCCCCCCC,4);t.save(OUTPUT+4,0xAB,1);t.save(OUTPUT+8,0x12345678,4)
    args=[POINT,ROADS,index&0xFFFFFFFF,count&0xFFFFFFFF,vertex_output,mode_output]
    if routine==0:args=[POINT,0,0,0,0,0]
    if routine==2:args=[POINT,road_output,vertex_output,mode_output,0,0]
    t.r[4:10]=args;t.f[12]=radius if routine==2 else threshold
    t.r[29],t.r[31]=0x80000,RETURN
    initial=[t.load(a,4) for a in SNAPSHOT]
    if not run:return t
    t.run(ENTRIES[routine])
    return {'routine':routine,'args':args,'scalar':word(radius if routine==2 else threshold),
            'initial':initial,'expected':[t.load(a,4) for a in SNAPSHOT],
            'result':word(t.f[0]) if routine==1 else t.r[2]&0xFFFFFFFF,
            'invocations':t.invocations,'stores':t.store_events,
            'instructions':t.instruction_count,'visited':sorted(t.visited)}


def fixtures(original):
    cases=[]
    for providers in (0,1,2,3):
        for disabled in (0,1,3,7):
            for p in ((0,0,0),(-40,-40,-40),(40,40,40),(41,0,0)):
                cases.append(scene(original,routine=0,providers=providers,disabled=disabled,point=p))
    for count in (0,1,2,3,5,65536,65537,65538,-1,-32768):
        for index in (0,1,65536,65535):
            for threshold in (-20,0,20):
                cases.append(scene(original,routine=1,index=index,count=count,threshold=threshold))
    for p in ((0,0,0),(-2,0,0),(-1.9,3,1.9),(2,0,2),(4,0,0),(-3,0,0)):
        for threshold in (-8,-7,0,1,20):
            for shape in (0,1,2):cases.append(scene(original,routine=1,point=p,threshold=threshold,shape=shape,count=3))
    for lazy in (False,True):
        for count in (0,1,2):
            for registry in (False,True):
                for mode in (0,OUTPUT+8):
                    cases.append(scene(original,cell_count=count,lazy=lazy,registry=registry,mode_output=mode,numbers=(0,1),neighbors=(0x10000,0x10001)))
    for radius in (-2,-0.0,0,0.25,2,40):
        for p in ((-40,0,-40),(-2,0,-2),(0,0,0),(39.9,0,39.9)):
            for span in (39.1,40,40.01,80):cases.append(scene(original,radius=radius,point=p,span=span))
    for n in ((),(0,),(0,1,2),(0x10000,0x10001,0x20000),(0x50000,0x50001,0x60000),(0x1FFFF,0x1FFFF)):
        for providers in (1,3):cases.append(scene(original,registry=False,neighbors=n,providers=providers))
    for mode in (OUTPUT+8,0):
        for shift in range(4):cases.append(scene(original,routine=1,vertex_output=OUTPUT+8+shift,mode_output=mode,threshold=0))
    # Actual store alias cases: output pointers are caller-supplied and trusted.
    for routine,roadout,vertex,mode in ((1,OUTPUT,OUTPUT+4,ROADS+0x38),
             (1,OUTPUT,CELL,OUTPUT+8),(1,OUTPUT,OUTPUT+4,POINT),
             (2,OUTPUT,CELL,OUTPUT+8),(2,CELL,OUTPUT+4,OUTPUT+8),
             (2,OUTPUT,OUTPUT+8+2,OUTPUT+8),(2,OUTPUT,OUTPUT+4,ROADS+0x38)):
        cases.append(scene(original,routine=routine,road_output=roadout,vertex_output=vertex,mode_output=mode))
    for lazy in (False,True):
        cases.append(scene(original,point=(4,0,0),cell_count=2,numbers=(0,1),
                           lazy=lazy,shift_records=True))
    for threshold in (-1,0,1):
        cases.append(scene(original,routine=1,threshold=threshold,shape=3))
    # Full unsigned returned record differs from signed16 lower-helper address.
    cases.append(scene(original,numbers=(0xFFFF,)))
    return cases


def golden_header(cases):
    lines=['/* Authored fixtures only; no original instructions/assets. */',
           'struct RoadWord { unsigned index,value; };',
           'struct RoadGolden { unsigned routine,args[6],scalar,result,initial_count,changed_count; const struct RoadWord *initial,*changed; unsigned calls[9]; };']
    for i,c in enumerate(cases):
        for n,data in (('initial',[(j,v) for j,v in enumerate(c['initial']) if v]),
                       ('changed',[(j,v) for j,(a,v) in enumerate(zip(c['initial'],c['expected'])) if a!=v])):
            lines.append('static const struct RoadWord r%d_%s[]={%s};'%(i,n,','.join('{%d,0x%08Xu}'%p for p in data) or '{0,0}'))
    lines.append('static const struct RoadGolden road_golden[]={')
    for i,c in enumerate(cases):
        lines.append('{%d,{%s},0x%08Xu,0x%08Xu,%d,%d,r%d_initial,r%d_changed,{%s}},'%(c['routine'],','.join('0x%Xu'%a for a in c['args']),c['scalar'],c['result'],sum(bool(v) for v in c['initial']),sum(a!=b for a,b in zip(c['initial'],c['expected'])),i,i,','.join(str(v) for v in c['invocations'])))
    return '\n'.join(lines+['};',''])


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/road_queries/trace.json')
    parser.add_argument('--golden-header',type=Path,default=ROOT/'tests/native/road_queries_golden.h')
    a=parser.parse_args();_,original=validated_elf(a.elf);cases=fixtures(original)
    inputs=[{k:c[k] for k in ('routine','args','scalar','initial')} for c in cases]
    summary={'fixtures':len(cases),'instructions':sum(c['instructions'] for c in cases),
             'maximum':max(c['instructions'] for c in cases),'visited':sorted(set().union(*(set(c['visited']) for c in cases))),
             'input_sha256':hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),
             'golden_sha256_lf':hashlib.sha256(golden_header(cases).encode()).hexdigest()}
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(summary,indent=2)+'\n')
    a.golden_header.write_text(golden_header(cases));print(json.dumps({k:v for k,v in summary.items() if k!='visited'}))
if __name__=='__main__':main()
