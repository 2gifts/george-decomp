"""Three complete route originals with genuine road/goal supporting execution.

The finite normal/zero binary32 model reuses the reviewed road decoder unchanged.
Virtual frame/position queries have explicit controlled one-GPR observation
contracts. Getter and seven-GPR setup execute their complete originals here;
native bridges remain supporting-only observers, not additional recovered C.
No original instruction, asset or table arrays are exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_geometry import Trace,RETURN,word,scalar,is_control_transfer
from trace_road_queries import RoadTrace,scene as road_scene,INTERVALS as ROAD_INTERVALS
from trace_road_queries import BUFFER,ROADS,RECORDS,ALT_RECORDS,MANAGER,PROVIDERS,POINT,POSITIONS,GLOBAL

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x1B7AB8,0x1B7CA0),(0x1E2D88,0x1E2EC8),(0x1CC628,0x1CC68C))
SUPPORT=((0x1CFBF8,0x1CFD20),(0x1CAFE0,0x1CB074),(0x1BA610,0x1BA618),(0x20D080,0x20D190))
INTERVALS=(*RANGES,*ROAD_INTERVALS,*SUPPORT)
ENTRIES=tuple(a for a,b in RANGES)
END=0x3A000
ACTOR,STATE,SETUP,ENTITY,VOBJECT,TABLE,FRAME,REFERENCE,POSITION,OTHER_MANAGER=(
 BUFFER+p for p in (0x6000,0x6200,0x7000,0x8200,0x8800,0x8900,0x8A00,0x8B00,0x8C00,0x9000))
FRAME_CALL,POSITION_CALL=0x60000000,0x60000004
WORDS=(END-BUFFER)//4
MEMORY_RANGES=((BUFFER,END),(GLOBAL,GLOBAL+4),(0x7C000,0x81000))

class RouteTrace(RoadTrace):
    def __init__(self,original,mutation=0,adjustment=0):
        super().__init__(original)
        self.mutation,self.adjustment=mutation,adjustment
        self.invocations=[0]*len(INTERVALS)
        self.calls=[0,0,0];self.events=[]
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in INTERVALS):
            raise ValueError('unreviewed actor route instruction')
        offset=pc-0xFF000
        if not 0<=offset<=len(self.original)-4:raise ValueError('actor route image bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(
                a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('actor route outside aligned observer memory')
    def load(self,address,size):
        self.memory_check(address,size)
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized actor route read')
        return Trace.load(self,address,size)
    def save(self,address,value,size):
        self.memory_check(address,size)
        return Trace.save(self,address,value,size)
    def execute(self,w,pc):
        op,rs,fn=w>>26,w>>21&31,w&63
        allowed=(0,1,3,4,5,6,9,10,11,12,13,14,15,17,20,21,30,31,32,33,35,36,37,40,41,43,49,55,57,63)
        special=(0,2,3,4,6,7,8,9,11,13,16,18,24,26,27,33,35,36,37,39,42,43,45)
        if op not in allowed or op==0 and fn not in special:
            raise ValueError('unsupported actor route opcode')
        if op==17 and (rs not in (0,4,8,16) or rs==16 and fn not in (0,1,2,6,52,54)):
            raise ValueError('unsupported actor route COP1 form')
        return super().execute(w,pc)
    def library_call(self,target):
        if target not in (FRAME_CALL,POSITION_CALL,0x120B68):
            raise ValueError('unknown actor route controlled callee')
        i=(FRAME_CALL,POSITION_CALL,0x120B68).index(target)
        receiver=self.r[4]&0xFFFFFFFF
        if target==FRAME_CALL and receiver!=VOBJECT+self.adjustment:
            raise ValueError('route frame signed adjustment/one-GPR contract')
        if target==POSITION_CALL and receiver!=VOBJECT+self.adjustment:
            raise ValueError('route position signed adjustment/one-GPR contract')
        if target==0x120B68 and receiver!=VOBJECT:
            raise ValueError('route numeric position input contract')
        self.calls[i]+=1;self.events.extend((i,receiver))
        if target==FRAME_CALL:
            normal=self.load(ROADS+0x48,4)+self.load(RECORDS+0x10,2)*12
            if self.mutation in (1,2):
                for j,v in enumerate(((0,1,0) if self.mutation==1 else (-1,2,3))):
                    self.single(normal+j*4,v)
            if self.mutation==2:self.save(ROADS+0x48,POSITIONS+12,4)
            if self.mutation==3:self.save(ACTOR+0x38,0,4)
            if self.mutation==4:self.save(RECORDS,self.load(RECORDS,4)|0x10000,4)
            if self.mutation==5:self.save(GLOBAL,OTHER_MANAGER,4)
            self.r[2]=FRAME
        else:self.r[2]=POSITION
        for register in range(3,16):self.r[register]=0xDEADBEEF
    def run(self,entry,stop=RETURN):
        body=next((p for p in INTERVALS if p[0]==entry),None)
        if body is None or len(self.frames)>=20:raise ValueError('route requires reviewed whole entry')
        self.invocations[tuple(a for a,b in INTERVALS).index(entry)]+=1
        if entry==0x20D080:
            args=[self.r[r]&0xFFFFFFFF for r in range(4,11)]
            if args[1]!=STATE+0x14 or args[2]!=5 or args[5] not in (POINT,STATE+0x114):
                raise ValueError('route setup genuine seven-GPR contract')
            self.events.extend((3,*args))
        self.frames.append(entry);pc=entry
        try:
            while True:
                if not body[0]<=pc<body[1]:raise ValueError('route control outside complete body')
                w=self.fetch(pc);target,annul=self.execute(w,pc)
                if is_control_transfer(w):
                    if not annul:
                        if pc+4>=body[1]:raise ValueError('route delay outside body')
                        delay=self.fetch(pc+4)
                        if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                            raise ValueError('actor route control transfer in delay')
                    if w>>26==0 and w&63==8:
                        if w!=0x03E00008 or target!=stop:
                            raise ValueError('actor route requires actual JR31 to selected stop')
                        return
                    call=w>>26==3 or w>>26==0 and w&63==9
                    if call:
                        continuation=self.r[31]
                        if target in tuple(a for a,b in INTERVALS):self.run(target,continuation)
                        else:self.library_call(target)
                        if self.r[31]!=continuation:raise ValueError('route callee changed RA')
                        pc=continuation
                    else:
                        if target is not None and not body[0]<=target<body[1]:raise ValueError('route branch crosses body')
                        pc=target if target is not None else pc+8
                else:pc+=4
        finally:self.frames.pop()


def snapshot(t):
    return [t.load(BUFFER+i*4,4) for i in range(WORDS)]+[t.load(GLOBAL,4)]

def make_scene(original,routine=0,missing=False,virtual=True,mutation=0,adjustment=0,
               direction=(1,0,0),active=0,tag_alias=0,count=0,marker=0,queue_alias=False,
               point_alias=False,copy_shift=0,query_failure=0):
    base=road_scene(original,run=False,cell_count=2,numbers=(0,1),shift_records=True,
                    point=(0,0,0),providers=2,missing=3 if missing else 0,mode_output=0)
    t=RouteTrace(original,mutation,adjustment)
    t.memory=base.memory
    for a in range(0x35000,END):t.memory[a]=0
    for i in range(0x100):t.save(OTHER_MANAGER+i,t.load(MANAGER+i,1),1)
    t.save(MANAGER+1,count,1)
    t.save(ACTOR+0x38,VOBJECT if virtual else 0,4);t.save(ACTOR+0x3C,ENTITY,4)
    t.save(VOBJECT+4,TABLE,4)
    t.save(TABLE+0x98,adjustment&65535,2);t.save(TABLE+0x9C,FRAME_CALL,4)
    t.save(TABLE+0x28,adjustment&65535,2);t.save(TABLE+0x2C,POSITION_CALL,4)
    for i,v in enumerate(direction):
        t.single(FRAME+0x20+i*4,v);t.single(ENTITY+0xD0+i*4,v)
        t.single(ACTOR+0x70+i*4,(-2,3,4)[i])
    for p in (ACTOR+0x48,ACTOR+0x50,ACTOR+0xB4,ACTOR+0xB8,ACTOR+0xBC,ACTOR+0xC0,
              STATE+0x3C,STATE+0x44,STATE+0x48,STATE+0x4C):t.save(p,0x5A5A5A5A,4)
    for i,v in enumerate((0,0,0)):t.single(POINT+i*4,v)
    t.save(STATE+0x10,SETUP,4)
    for i,v in enumerate((4,0,0)):t.single(STATE+0x114+i*4,v)
    t.save(STATE+0x110,0 if active==0 else REFERENCE,4)
    t.save(REFERENCE+4,3 if active==1 else 1 if active==2 else 2 if active==3 else 5,1)
    t.save(REFERENCE+6,0x40 if active==1 else 0,1)
    t.save(REFERENCE+0x20,VOBJECT if active>=3 else STATE+0x114+copy_shift-0x40,4)
    for i,v in enumerate((4,0,0)):
        t.single(POSITION+i*4,v)
        if active==1:t.single(REFERENCE+0x20+i*4,v)
    if copy_shift:
        for i,v in enumerate((4,0,0)):t.single(STATE+0x114+copy_shift+i*4,v)
    if tag_alias:
        new=STATE+(0x4C if tag_alias==1 else 0x14)
        data=bytes(t.memory[RECORDS+i] for i in range(0xA0))
        for i,v in enumerate(data):t.memory[new+i]=v
        t.save(ROADS+0x38,new,4)
    if query_failure:
        if routine!=1 or query_failure not in (1,2):raise ValueError('invalid authored query failure')
        t.single(POINT if query_failure==1 else STATE+0x114,1000.0)
    args=[ACTOR,ACTOR+0xB4 if point_alias else POINT] if routine==0 else [STATE,STATE+0x114 if point_alias else POINT]
    if point_alias and routine==0:
        for i,v in enumerate((0,0,0)):t.single(ACTOR+0xB4+i*4,v)
    if routine==2:
        object=MANAGER-2 if queue_alias else SETUP
        t.save(object+3,marker,1);args=[object,0]
    t.r[4:6]=args;t.r[29],t.r[31]=0x80000,RETURN
    return t,args

def make_fixture(original,**parameters):
    t,args=make_scene(original,**parameters)
    initial=snapshot(t)
    t.run(ENTRIES[parameters.get('routine',0)])
    final=snapshot(t)
    return dict(parameters=parameters,args=args,
        initial=[[i,v] for i,v in enumerate(initial) if v],
        changed=[[i,v] for i,v in enumerate(final) if v!=initial[i]],
        result=t.r[2]&0xFFFFFFFF if parameters.get('routine',0)==2 else 0,
        calls=t.calls,events=t.events,invocations=t.invocations,
        instruction_count=t.instruction_count,visited=sorted(t.visited))

def fixtures(original):
    cases=[]
    for missing in (False,True):
        for virtual in (False,True):
            for direction in ((1,0,0),(-1,0,0),(0,1,0),(0,0,0)):
                for adjustment in ((-4,0,4) if virtual and not missing else (0,)):
                    cases.append(make_fixture(original,missing=missing,virtual=virtual,
                                              direction=direction,adjustment=adjustment))
    for mutation in range(1,6):
        for adjustment in (-4,4):cases.append(make_fixture(original,mutation=mutation,adjustment=adjustment))
    for missing in (False,True):
        for point_alias in (False,True):cases.append(make_fixture(original,missing=missing,point_alias=point_alias))
    for active in range(5):
        for missing in (False,True):
            for alias in (0,1,2):
                cases.append(make_fixture(original,routine=1,active=active,missing=missing,tag_alias=alias))
    for shift in (4,8):cases.append(make_fixture(original,routine=1,active=2,copy_shift=shift))
    for point_alias in (False,True):cases.append(make_fixture(original,routine=1,point_alias=point_alias))
    for which in (1,2):cases.append(make_fixture(original,routine=1,query_failure=which))
    for count in (0,1,28,29,30,31,255):
        for marker in (0,1,255):cases.append(make_fixture(original,routine=2,count=count,marker=marker))
    cases.append(make_fixture(original,routine=2,count=0,queue_alias=True))
    return cases

def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:c[k] for k in ('parameters','args','initial')} for c in cases],
        sort_keys=True,separators=(',',':')).encode()).hexdigest()

def golden_header(cases):
    lines=['/* Synthetic bounded original outputs; no game instructions/assets. */',
        'struct RoutePair { u32 index,value; };',
        'struct ActorRouteGolden { u32 routine,mutation; s32 adjustment; u32 args[2],result,calls[3],initial_count,changed_count,event_count; const struct RoutePair *initial,*changed; const u32 *events; };']
    for n,c in enumerate(cases):
        for key in ('initial','changed'):
            pairs=','.join('{%du,0x%08Xu}'%tuple(p) for p in c[key]) or '{0,0}'
            lines.append('static const struct RoutePair route_%s_%d[]={%s};'%(key,n,pairs))
        lines.append('static const u32 route_events_%d[]={%s};'%(n,','.join('0x%08Xu'%v for v in c['events']) or '0'))
    lines.append('static const struct ActorRouteGolden actor_route_golden[]={')
    for n,c in enumerate(cases):
        p=c['parameters']
        lines.append(' {%du,%du,%d,{0x%08Xu,0x%08Xu},0x%08Xu,{%du,%du,%du},%du,%du,%du,route_initial_%d,route_changed_%d,route_events_%d},'%(
            p.get('routine',0),p.get('mutation',0),p.get('adjustment',0),*c['args'],c['result'],*c['calls'],
            len(c['initial']),len(c['changed']),len(c['events']),n,n,n))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/actor_route/trace.json');p.add_argument('--golden-header',type=Path)
    a=p.parse_args();_,raw=validated_elf(a.elf);cases=fixtures(raw)
    a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(dict(limitation=__doc__,input_sha256=input_hash(cases),cases=cases),indent=2)+'\n')
    if a.golden_header:a.golden_header.write_text(golden_header(cases))
    print(len(cases),'fixtures;',sum(c['instruction_count'] for c in cases),'instructions; max',max(c['instruction_count'] for c in cases))
if __name__=='__main__':main()
