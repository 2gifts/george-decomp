"""Bounded route initialization originals and genuinely executed helper paths.

Finite normal/zero, separately rounded binary32 only; EE FCR, exceptional
arithmetic and fault behavior are outside the observer. The published movement
body executes original instructions on its early-positive and simple-state
paths; all other movement states fail explicitly. Its 42-word dispatch is read
from the local original, never exported. Tagged virtual/numeric position calls
alone use a controlled one-GPR observation. No query output is fabricated.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace,RETURN,word,scalar,is_control_transfer
from trace_road_queries import RoadTrace

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x1CD578,0x1CD724),(0x1DB250,0x1DB3DC),(0x20D190,0x20D1CC))
SUPPORT=((0x1CC710,0x1CC7B4),(0x2A00A0,0x2A0138),(0x2A0390,0x2A040C),
         (0x1CAFE0,0x1CB074),(0x177E48,0x1784D0),(0x20BEA8,0x20BFB0),(0x1CC628,0x1CC68C))
INTERVALS=(*RANGES,*SUPPORT)
ENTRIES=tuple(a for a,b in RANGES)
BUFFER,END=0x2F000,0x37000
MANAGER,PROVIDER,HEADER,COLLECTION,OFFSETS,RECORDS,PLANES,POINT,OUTPUT=(
    0x30000,0x30040,0x30100,0x30200,0x30300,0x30400,0x30800,0x30A00,0x30B00)
STATE,OWNER,ENTITY,DATA,SETUP,REFERENCE,VOBJECT,TABLE,POSITION,OTHER_OWNER=(
    0x31000,0x31200,0x31400,0x32000,0x33000,0x35000,0x35100,0x35200,0x35300,0x35400)
GLOBAL=0x3F8C28
POSITION_CALL=0x60000000
TABLE_ADDRESS,TABLE_SIZE=0x42C530,168
WORDS=(END-BUFFER)//4
MEMORY_RANGES=((BUFFER,END),(GLOBAL,GLOBAL+4),(0x7C000,0x81000))
SIMPLE_STATES=(3,4,27,29,30,10,14,26,41,1,6,7,8,9,11,13,15,16,17,18,19,20,21,22,24,25,28,33,34,35,36,37,38,39,40)

class InitTrace(RoadTrace):
    def __init__(self,original,mutation=0,adjustment=0):
        super().__init__(original)
        self.mutation,self.adjustment=mutation,adjustment
        self.invocations=[0]*len(INTERVALS)
        self.calls=[0,0];self.events=[]
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in INTERVALS):
            raise ValueError('unreviewed route-init instruction')
        offset=pc-0xFF000
        if not 0<=offset<=len(self.original)-4:raise ValueError('route-init original bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(
                a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('route-init outside aligned observer memory')
    def load(self,address,size):
        if TABLE_ADDRESS<=address< TABLE_ADDRESS+TABLE_SIZE:
            if size!=4 or address&3 or address+size>TABLE_ADDRESS+TABLE_SIZE:
                raise ValueError('route-init dispatch-table geometry')
            offset=address-0xFF000
            if not 0<=offset<=len(self.original)-4:raise ValueError('route-init table image bound')
            value=struct.unpack_from('<I',self.original,offset)[0]
            if value&3 or not 0x177E48<=value<0x1784D0:
                raise ValueError('route-init original dispatch target')
            return value
        self.memory_check(address,size)
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized route-init read')
        return Trace.load(self,address,size)
    def save(self,address,value,size):
        self.memory_check(address,size)
        return Trace.save(self,address,value,size)
    def execute(self,w,pc):
        op,rs,rt,rd,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w&63
        allowed=(0,1,3,4,5,6,9,10,11,12,13,15,17,20,21,30,31,32,33,35,36,37,40,41,43,49,55,57,63)
        special=(0,2,3,8,9,33,35,36,37,42,43,45)
        if op not in allowed or op==0 and fn not in special:
            raise ValueError('unsupported route-init opcode')
        if op==17 and (rs not in (0,4,8,16) or rs==16 and fn not in (0,1,2,4,6,52,54)):
            raise ValueError('unsupported route-init COP1')
        if op==9:
            imm=w&65535;imm=imm-65536 if imm&32768 else imm
            value=(self.r[rs]+imm)&0xFFFFFFFF
            self.r[rt]=value|0xFFFFFFFF00000000 if value&0x80000000 else value
            self.visited.add(pc);self.instruction_count+=1;self.r[0]=0
            if self.instruction_count>150000:raise ValueError('route-init instruction bound')
            return None,False
        if op==0 and fn==36:
            if w>>6&31:raise ValueError('reserved route-init AND')
            # ADDIU's simple-state -0x1001 masks are sign-extended on the EE.
            # Preserve the complete LD/AND/SD 64-bit flags, rather than truncate.
            self.r[rd]=(self.r[rs]&self.r[rt])&0xFFFFFFFFFFFFFFFF
            self.visited.add(pc);self.instruction_count+=1;self.r[0]=0
            if self.instruction_count>150000:raise ValueError('route-init instruction bound')
            return None,False
        return super().execute(w,pc)
    def library_call(self,target):
        if target not in (POSITION_CALL,0x120B68):
            raise ValueError('unknown route-init controlled callee')
        receiver=self.r[4]&0xFFFFFFFF
        wanted=VOBJECT+self.adjustment if target==POSITION_CALL else VOBJECT
        if receiver!=wanted:raise ValueError('route-init one-GPR position receiver')
        index=0 if target==POSITION_CALL else 1
        self.calls[index]+=1;self.events.extend((index,receiver))
        if self.mutation==1:self.save(STATE,OTHER_OWNER,4)
        if self.mutation==2:self.save(STATE+0xC0,0,4)
        if self.mutation==3:self.save(GLOBAL,MANAGER+0x100,4)
        self.r[2]=POSITION
        for register in range(3,16):self.r[register]=0xDEADBEEF
    def run(self,entry,stop=RETURN):
        body=next((p for p in INTERVALS if p[0]==entry),None)
        if body is None or len(self.frames)>=20:raise ValueError('route-init requires whole entry')
        if entry==0x177E48:
            entity=self.r[4]&0xFFFFFFFF
            if scalar(self.load(entity+0x438,4))<=0 and self.load(entity+0xC,4) not in SIMPLE_STATES:
                raise ValueError('movement path outside reviewed execution subset')
        self.invocations[tuple(a for a,b in INTERVALS).index(entry)]+=1
        if entry==0x20BEA8:
            args=[self.r[r]&0xFFFFFFFF for r in range(4,11)]
            if args[1]!=STATE+0x14 or args[2]!=12 or args[6]!=STATE+0xC4:
                raise ValueError('route-init seven-GPR setup contract')
            self.events.extend((2,*args))
        self.frames.append(entry);pc=entry
        try:
            while True:
                if not body[0]<=pc<body[1]:raise ValueError('route-init control outside body')
                w=self.fetch(pc);target,annul=self.execute(w,pc)
                if is_control_transfer(w):
                    if not annul:
                        if pc+4>=body[1]:raise ValueError('route-init delay outside body')
                        delay=self.fetch(pc+4)
                        if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                            raise ValueError('route-init control transfer in delay')
                    if w>>26==0 and w&63==8:
                        if w==0x03E00008:
                            if target!=stop:raise ValueError('route-init actual JR31 stop')
                            return
                        if pc!=0x177EAC or w!=0x00800008 or target not in tuple(
                                self.load(TABLE_ADDRESS+i*4,4) for i in range(42)):
                            raise ValueError('route-init unproven computed jump')
                        pc=target;continue
                    call=w>>26==3 or w>>26==0 and w&63==9
                    if call:
                        continuation=self.r[31]
                        if target in tuple(a for a,b in INTERVALS):self.run(target,continuation)
                        else:self.library_call(target)
                        if self.r[31]!=continuation:raise ValueError('route-init callee changed RA')
                        pc=continuation
                    else:
                        if target is not None and not body[0]<=target<body[1]:raise ValueError('route-init branch crosses body')
                        pc=target if target is not None else pc+8
                else:pc+=4
        finally:self.frames.pop()

def snapshot(t):
    return [t.load(BUFFER+i*4,4) for i in range(WORDS)]+[t.load(GLOBAL,4)]

def scene(original,routine=0,count=3,kind=1,plane_count=0,inside=False,
          point=(0,0,0),centers=None,missing=0,disabled=False,alias=0,
          active=0,mutation=0,adjustment=0,copy_shift=0,owner_alias=0,
          owner_tag=0x44000000,move_state=3,positive=False,queue_count=0,
          setup_alias=False,flags=4,step=7,output_alias=0,flag_bits=0x1020304050607080,
          plane_distance=None):
    t=InitTrace(original,mutation,adjustment)
    for a,b in MEMORY_RANGES[:2]:
        for p in range(a,b):t.memory[p]=0
    t.save(GLOBAL,MANAGER,4);t.save(MANAGER,0 if missing==1 else 1,1)
    t.save(MANAGER+1,queue_count,1);t.save(MANAGER+8,PROVIDER,4)
    t.save(PROVIDER+2,0 if disabled else 4,1);t.save(PROVIDER+0x10,HEADER,4)
    t.save(HEADER,0x44000000,4);t.save(HEADER+0x24,0 if missing==2 else COLLECTION,4)
    for i,v in enumerate((-2e9,-2e9,-2e9,2e9,2e9,2e9)):t.single(HEADER+4+i*4,v)
    t.save(COLLECTION,count,4);t.save(COLLECTION+0x20,OFFSETS,4);t.save(COLLECTION+0x24,RECORDS,4)
    if centers is None:centers=((3,0,0),(1,0,0),(2,0,0))
    for i in range(3):
        record=RECORDS+i*32
        t.save(OFFSETS+i*4,i*32,4)
        for j,v in enumerate(centers[i]):t.single(record+4+j*4,v)
        t.save(record+0x14,kind,1);t.save(record+0x17,plane_count,1);t.save(record+0x18,PLANES,4)
    for j,v in enumerate((1,0,0,(100 if inside else -100) if plane_distance is None else plane_distance)):
        t.single(PLANES+j*4,v)
    for j,v in enumerate((0,1,0,100 if inside else -100)):t.single(PLANES+16+j*4,v)
    for j,v in enumerate(point):t.single(POINT+j*4,v);t.single(STATE+0xC4+j*4,v)
    for p in (OUTPUT,OUTPUT+4,OUTPUT+8):t.save(p,0x5A5A5A5A,4)
    tag,recordout,collectionout=OUTPUT,OUTPUT+4,OUTPUT+8
    if alias==1:recordout=HEADER
    if alias==2:recordout=collectionout
    if alias==3:collectionout=HEADER+0x24
    if alias==4:collectionout=COLLECTION
    if alias==5:tag=HEADER
    if alias==6:recordout=RECORDS+4
    if alias==7:recordout=COLLECTION
    if alias==8:tag=COLLECTION
    if alias==9:recordout=HEADER+0x24
    args=[POINT,tag,recordout,collectionout]
    if routine==1:
        t.save(STATE,OWNER,4);t.save(STATE+0x10,SETUP,4)
        t.save(OWNER+8,ENTITY,4);t.save(OWNER+0xC,owner_tag,4)
        t.save(OTHER_OWNER+8,ENTITY,4);t.save(OTHER_OWNER+0xC,0x44000001,4)
        t.save(ENTITY+0xC,move_state,4);t.save(ENTITY+0x18,DATA,4)
        t.single(ENTITY+0x438,1.0 if positive else 0.0)
        t.save(ENTITY+0x190,flag_bits,8);t.single(DATA+0x10,2.0)
        for j,v in enumerate((4,-2,3)):
            t.single(ENTITY+0xD0+j*4,v);t.single(ENTITY+0x40+j*4,v)
            t.single(STATE+0x98+j*4,9+j);t.single(STATE+0xA4+j*4,3+j)
        for off in (0x74,0x75,0x76,0x77,0x80,0x81,0x82):t.save(STATE+off,0xA5,1)
        t.save(STATE+0x84,0xA5A5A5A5,4);t.save(STATE+0x88,0xA5A5A5A5,4)
        t.save(STATE+0xC0,0 if active==0 else REFERENCE,4)
        t.save(REFERENCE+4,3 if active==1 else 1 if active==2 else 2 if active==3 else 5,1)
        t.save(REFERENCE+6,0x40 if active==1 else 0,1)
        t.save(REFERENCE+0x20,VOBJECT if active>=3 else STATE+0xC4+copy_shift-0x40,4)
        t.save(VOBJECT+4,TABLE,4);t.save(TABLE+0x28,adjustment&65535,2)
        t.save(TABLE+0x2C,POSITION_CALL,4)
        for j,v in enumerate(point):
            t.single(POSITION+j*4,v)
            if active==1:t.single(REFERENCE+0x20+j*4,v)
            if copy_shift:t.single(STATE+0xC4+copy_shift+j*4,v)
        if owner_alias:
            owner=STATE+(0x74 if owner_alias==1 else 0x8C)
            t.save(STATE,owner,4);t.save(owner+8,ENTITY,4);t.save(owner+0xC,owner_tag,4)
        if setup_alias:t.save(STATE+0x10,STATE-0xEBC,4)
        args=[STATE,0,0,0]
    if routine==2:
        t.save(SETUP,flags,1);t.save(SETUP+0xCB4,step,1)
        output=OUTPUT if output_alias==0 else SETUP if output_alias==1 else SETUP+0xCB4
        args=[SETUP,output,0,0]
    pointers=[MANAGER+8,PROVIDER+0x10,HEADER+0x24,COLLECTION+0x20,COLLECTION+0x24,
              *[RECORDS+i*32+0x18 for i in range(3)],STATE,STATE+0x10,STATE+0xC0,
              OWNER+8,OTHER_OWNER+8,ENTITY+0x18,VOBJECT+4,TABLE+0x2C,REFERENCE+0x20]
    if owner_alias:pointers.append(STATE+(0x7C if owner_alias==1 else 0x94))
    pointers.extend((recordout,collectionout,MANAGER+0x6C+queue_count*4,SETUP+0xECC))
    if alias==1:pointers.append(tag)
    if setup_alias:pointers.extend((STATE+0x10,STATE+0x14))
    t.r[4:8]=args;t.r[29],t.r[31]=0x80000,RETURN
    return t,args,sorted(set(pointers))

def make_fixture(original,**parameters):
    t,args,pointers=scene(original,**parameters)
    before=snapshot(t);routine=parameters.get('routine',0)
    t.run(ENTRIES[routine]);after=snapshot(t)
    return dict(parameters=parameters,args=args,pointer_cells=pointers,
        initial=[[i,v] for i,v in enumerate(before) if v],
        changed=[[i,v] for i,v in enumerate(after) if v!=before[i]],
        result=t.r[2]&0xFFFFFFFF if routine!=1 else 0,calls=t.calls,events=t.events,
        invocations=t.invocations,instruction_count=t.instruction_count,visited=sorted(t.visited))

def fixtures(original):
    cases=[]
    for count in (0,1,3):
        for kind in (0,1,2,3,255):
            for n in (0,1,2):
                for inside in (False,True):
                    cases.append(make_fixture(original,count=count,kind=kind,plane_count=n,inside=inside))
    for missing in (1,2):cases.append(make_fixture(original,missing=missing))
    cases.append(make_fixture(original,disabled=True))
    for centers in (((3,0,0),(3,0,0),(-3,0,0)),((0,0,0),(0,0,0),(0,0,0)),
                    tuple((scalar(w),0,0) for w in (0x4E6E6B28,0x4E6E6B29,0x4E6E6B27))):
        cases.append(make_fixture(original,centers=centers))
    for alias in range(1,10):
        cases.append(make_fixture(original,alias=alias,kind=0,plane_count=1,inside=True))
        if alias in (1,2,3,5,9):cases.append(make_fixture(original,alias=alias))
    for point in ((-3,4,5),(3,0,0),(0,-0.0,0)):
        cases.append(make_fixture(original,point=point))
    for x in (0x3727C5AB,0x3727C5AC,0x3727C5AD):
        cases.append(make_fixture(original,kind=0,plane_count=1,plane_distance=0,point=(scalar(x),0,0)))
    for count in (0x80000000,0xFFFFFFFF):
        cases.append(make_fixture(original,count=count,kind=0,plane_count=1,inside=True))
    for active in range(5):
        for missing in (0,1,2):
            for inside in (False,True):
                cases.append(make_fixture(original,routine=1,active=active,missing=missing,
                                          kind=0,plane_count=1,inside=inside))
    for state in SIMPLE_STATES:
        cases.append(make_fixture(original,routine=1,move_state=state))
    for bits in (0x80001000,0xFFFFFFFFFFFFFFFF,0x100000000,0):
        cases.append(make_fixture(original,routine=1,flag_bits=bits))
    for positive in (False,True):
        for tag in (0xFFFFFFFF,0x44000000,0x44000001):
            cases.append(make_fixture(original,routine=1,positive=positive,owner_tag=tag))
    cases.append(make_fixture(original,routine=1,kind=0,plane_count=1,inside=True,owner_tag=0x44000001))
    for mutation in (0,1,2,3):
        for adjustment in (-4,0,4):
            cases.append(make_fixture(original,routine=1,active=3,mutation=mutation,adjustment=adjustment))
    for shift in (4,8):cases.append(make_fixture(original,routine=1,active=2,copy_shift=shift,point=(1,2,3)))
    for owner_alias in (1,2):cases.append(make_fixture(original,routine=1,owner_alias=owner_alias))
    for queue in (0,1,29,30,255):cases.append(make_fixture(original,routine=1,queue_count=queue))
    cases.append(make_fixture(original,routine=1,setup_alias=True))
    for flags in range(256):
        for output_alias in (0,1,2):
            cases.append(make_fixture(original,routine=2,flags=flags,output_alias=output_alias,step=flags^0xA5))
    return cases

def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:c[k] for k in ('parameters','args','pointer_cells','initial')} for c in cases],
        sort_keys=True,separators=(',',':')).encode()).hexdigest()

def golden_header(cases):
    lines=['/* Synthetic bounded original results; no game instructions/assets. */',
        'struct InitPair { u32 index,value; };',
        'struct InitGolden { u32 routine,mutation; s32 adjustment; u32 args[4],result,calls[2],movement_calls,setup_calls,initial_count,changed_count,event_count,pointer_count; const struct InitPair *initial,*changed; const u32 *events,*pointers; };']
    for i,c in enumerate(cases):
        for key in ('initial','changed'):
            pairs=','.join('{%du,0x%08Xu}'%tuple(p) for p in c[key]) or '{0,0}'
            lines.append('static const struct InitPair init_%s_%d[]={%s};'%(key,i,pairs))
        for key,value in (('events',c['events']),('pointers',c['pointer_cells'])):
            lines.append('static const u32 init_%s_%d[]={%s};'%(key,i,','.join('0x%08Xu'%v for v in value) or '0'))
    lines.append('static const struct InitGolden init_golden[]={')
    for i,c in enumerate(cases):
        p=c['parameters'];numbers=(p.get('routine',0),p.get('mutation',0),p.get('adjustment',0),*c['args'],c['result'],*c['calls'],
            c['invocations'][7],c['invocations'][8],
            len(c['initial']),len(c['changed']),len(c['events']),len(c['pointer_cells']),i,i,i,i)
        lines.append(' {%du,%du,%d,{0x%08Xu,0x%08Xu,0x%08Xu,0x%08Xu},0x%08Xu,{%du,%du},%du,%du,%du,%du,%du,%du,init_initial_%d,init_changed_%d,init_events_%d,init_pointers_%d},'%numbers)
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/actor_route_init/trace.json')
    p.add_argument('--golden-header',type=Path)
    a=p.parse_args();_,raw=validated_elf(a.elf);cases=fixtures(raw)
    a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(dict(limitation=__doc__,input_sha256=input_hash(cases),cases=cases),indent=2)+'\n')
    if a.golden_header:a.golden_header.write_text(golden_header(cases))
    print(len(cases),'fixtures;',sum(c['instruction_count'] for c in cases),'instructions; max',max(c['instruction_count'] for c in cases))
if __name__=='__main__':main()
