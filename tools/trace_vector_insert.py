"""Strict initialized integer observations of two insertion entries and real
 * supporting chunk/OOM/refill/memmove bodies. No generic C++/EE emulator,
 * allocator/kernel implementation or arbitrary object/storage alias model.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,sx32
ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x1007E0,0x100AA4),(0x100AA8,0x100AB8),(0x1005C8,0x1007DC),
        (0x252168,0x2521F8),(0x2521F8,0x252294),(0x3935A4,0x3936A0))
BASE,WORDS=0x20000,4096;END=BASE+WORDS*4
VECTOR=BASE;VALUE=BASE+0x80;OLD=BASE+0x200;POOL=BASE+0x1000;FREE=BASE+0x2800
STACK=(0x7D000,0x81000);H1,H2=0xF1200000,0xF1200010
GLOBALS=(0x3F21B0,*range(0x3F21B8,0x3F21F8,4),0x3F21F8,0x3F21FC,0x3F2200)
KEYS=('routine','count','capacity','position','value_index','cache','failures','mutation')
initial_word=lambda i:(0x5A830201^(i*0x10203))&MASK

class InsertTrace(RegistryTrace):
    def __init__(self,original,parameters):
        Trace.__init__(self,original);self.p=parameters;self.lo=self.hi=0
        self.visited=set();self.events=[];self.branches=[];self.invocations=[0]*len(RANGES)
        self.allocations=0;self.copies=0
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
            BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or n==4 and a in GLOBALS):
            raise ValueError('unowned/unaligned insertion memory')
    def load(self,a,n):
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized insertion memory')
        return Trace.load(self,a,n)
    def save(self,a,v,n):self.memory_check(a,n);Trace.save(self,a,v,n)
    def fetch(self,pc):
        if pc%4 or not any(lo<=pc<hi for lo,hi in RANGES):raise ValueError('unreviewed insertion instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('insertion code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def event(self,kind,a=0,b=0,c=0):
        if len(self.events)>=100:raise ValueError('insertion event bound')
        self.events.append([kind,a&MASK,b&MASK,c&MASK])
    def mutate_copy(self):
        if self.copies==1 and self.p['mutation']&2:
            self.save(VALUE,0x7F001234,4)
            if self.p['count'] and self.p['position']<self.p['count']:
                self.save(VECTOR+4,OLD+4*(self.p['count']-1),4)
    def external(self,target):
        if target==0x2AF140:
            size=self.r[4]&MASK
            if not 0<size<=6000:raise ValueError('allocation outside bounded insertion domain')
            self.allocations+=1;self.event(1,size,self.allocations)
            if self.p['mutation']&1:self.save(VALUE,0x6F005678,4)
            self.r[2]=0 if self.allocations<=self.p['failures'] else POOL
        elif target==0x2AF1E8:
            if self.r[4]&MASK!=OLD:raise ValueError('unknown insertion free')
            self.event(2,OLD,self.load(VECTOR+4,4),self.load(VECTOR+8,4))
        elif target in (H1,H2):
            self.event(3,1 if target==H1 else 2)
            if target==H1:self.save(GLOBALS[0],H2,4)
        else:raise ValueError('unsupported insertion external/stream/null-handler path')
        for r in range(3,16):self.r[r]=sx32(0xA17E0000+r)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>8:raise ValueError('unreviewed insertion entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo
        if index==5:
            d,s,n=(self.r[r]&MASK for r in (4,5,6))
            if n>256:raise ValueError('memmove insertion count bound')
            if n:
                self.memory_check(d,1);self.memory_check(d+n-1,1)
                self.memory_check(s,1);self.memory_check(s+n-1,1)
            self.copies+=1;self.event(4,d,s,n)
        while True:
            if not lo<=pc<hi:raise ValueError('insertion transfer outside whole body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('taken insertion annul')
                    pc+=8;continue
                delay=self.fetch(pc+4)
                if pc+4>=hi or is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('control in insertion delay')
                if w==0x03E00008:
                    if target!=stop or index!=5 and pc!=hi-8:raise ValueError('actual insertion JR31/stop required')
                    if index==5:self.mutate_copy()
                    return
                op=w>>26;fn=w&63
                if op==3 or op==0 and fn==9:
                    continuation=self.r[31]&MASK
                    if target in (a for a,b in RANGES):self.run(target,continuation,depth+1)
                    else:self.external(target)
                    pc=continuation
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('unexpected insertion transfer')
                pc+=4

def fixture(original,routine=0,count=3,capacity=8,position=1,value_index=-1,cache=0,failures=0,mutation=0):
    p=dict(zip(KEYS,(routine,count,capacity,position,value_index,cache,failures,mutation)))
    if routine not in (0,1) or not 0<=count<=64 or not count<=capacity<=66 or not 0<=position<=count or not -1<=value_index<=count or cache not in range(6) or failures not in range(5) or mutation not in range(4):raise ValueError('insertion fixture parameter domain')
    if count!=capacity and count==0:raise ValueError('spare empty vector source lifetime domain')
    if count!=capacity and position==count:raise ValueError('direct auxiliary spare insertion at end outside active valid source range')
    if count==capacity and cache==0 and 0<count<=16:raise ValueError('small growth requires explicit allocator cache domain')
    t=InsertTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xC5070301,4)
    for a in GLOBALS:t.save(a,0,4)
    t.save(GLOBALS[0],H1,4)
    for i in range(67):t.save(OLD+4*i,(0x43000000+i*0x11001)&MASK,4)
    t.save(VALUE,0x80000123,4)
    if routine==1:
        comparator_values=(0,0x7FFFFFFF,0x80000000,0xFFFFFFFF)
        t.save(OLD,comparator_values[count%4],4);t.save(VALUE,comparator_values[mutation],4)
    t.save(VECTOR,OLD if capacity else 0,4);t.save(VECTOR+4,OLD+4*count if capacity else 0,4);t.save(VECTOR+8,OLD+4*capacity if capacity else 0,4)
    request=4*(2*count if count else 1);block=(request+7)&~7
    if cache==1 and request<=128:
        t.save(GLOBALS[1]+4*(block//8-1),FREE,4);t.save(FREE,0,4)
    elif cache in (2,3) and request<=128:
        t.save(0x3F21F8,POOL,4);t.save(0x3F21FC,POOL+block*(20 if cache==2 else 3),4)
    elif cache==4 and request<=128:
        t.save(0x3F21F8,FREE,4);t.save(0x3F21FC,FREE+8,4)
        t.save(GLOBALS[1]+4*(15),POOL,4);t.save(POOL,0,4)
    before=[t.load(BASE+4*i,4) for i in range(WORDS)];gbefore=[t.load(a,4) for a in GLOBALS]
    value=VALUE if value_index<0 else OLD+4*value_index
    args=(VECTOR,OLD+4*position if capacity else 0,value) if routine==0 else (OLD,VALUE)
    for r,v in enumerate(args,4):t.r[r]=sx32(v)
    t.r[29]=0x80000;t.r[31]=RETURN
    saved=[0xCAFE1234567890000000000000000000+i for i in range(16,24)]+[0xABCDEF01234567890123456789ABCDE0]
    for r,v in zip([*range(16,24),30],saved):t.r[r]=v
    t.run(RANGES[routine][0]);assert t.r[29]==0x80000 and [t.r[r] for r in [*range(16,24),30]]==saved,(p,hex(t.r[29]),[(r,hex(t.r[r]),hex(v)) for r,v in zip([*range(16,24),30],saved) if t.r[r]!=v])
    after=[t.load(BASE+4*i,4) for i in range(WORDS)]
    return dict(parameters=p,args=args,initial=[[i,v] for i,v in enumerate(before) if v!=initial_word(i)],changes=[[i,v] for i,v in enumerate(after) if v!=before[i]],globals_initial=gbefore,globals_expected=[t.load(a,4) for a in GLOBALS],events=t.events,result=t.r[2]&MASK if routine==1 else 0,visited=sorted(t.visited),branches=t.branches,instructions=t.instruction_count,invocations=t.invocations)

def cases(original):
    result=[]
    for n in (1,2,3,4,8,16,17,32,40,64):
        for spare in (False,True):
            cap=n+2 if spare else n
            for pos in sorted({0,n//2,max(0,n-1)}):
                for vi in (-1,0,n-1,n):
                    modes=(0,) if spare or n>16 else (1,2,3,5)
                    for cache in modes:
                        result.append(fixture(original,count=n,capacity=cap,position=pos,value_index=vi,cache=cache))
    for n in (0,1,2,8,16,17,40):
        for failed in (0,1,3):
            for mutation in (0,1,2,3):
                result.append(fixture(original,count=n,capacity=n,position=0,cache=5,failures=failed,mutation=mutation))
    for n in (1,2,4,8):result.append(fixture(original,count=n,capacity=n,position=0,cache=4,failures=1))
    for left in range(4):
        for right in range(4):result.append(fixture(original,routine=1,count=left,capacity=left,position=0,cache=1,value_index=-1,mutation=right))
    return result

def header(records):
    lines=['/* Authored initialized inputs and observed outputs; no original instructions. */',
      'struct InsertPair { unsigned int index,value; };',
      'struct InsertCase { unsigned int p[8],initial_count,change_count,event_count,result; const InsertPair *initial,*changes; unsigned int globals_initial[20],globals_expected[20]; const unsigned int *events; };']
    for i,c in enumerate(records):
        for label in ('initial','changes'):
            rows=c[label];lines.append('static const InsertPair insert_%s_%d[]={%s};'%(label,i,','.join('{%du,0x%08Xu}'%(a,b) for a,b in rows) or '{0,0}'))
        lines.append('static const unsigned int insert_events_%d[]={%s};'%(i,','.join('0x%08Xu'%v for row in c['events'] for v in row) or '0'))
    lines.append('static const InsertCase insert_cases[]={')
    for i,c in enumerate(records):
        p=c['parameters'];vals=[p[k]&MASK for k in KEYS]
        lines.append('{{%s},%d,%d,%d,0x%08Xu,insert_initial_%d,insert_changes_%d,{%s},{%s},insert_events_%d},'%(','.join('%du'%v for v in vals),len(c['initial']),len(c['changes']),len(c['events'])*4,c['result'],i,i,','.join('0x%08Xu'%v for v in c['globals_initial']),','.join('0x%08Xu'%v for v in c['globals_expected']),i))
    lines.append('};\n');return '\n'.join(lines)

def main():
    a=argparse.ArgumentParser();a.add_argument('--output',type=Path,default=ROOT/'build/vector_insert/trace.json');a.add_argument('--header',type=Path,default=ROOT/'tests/native/vector_insert_golden.h');args=a.parse_args()
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');records=cases(original)
    packet=dict(fixtures=len(records),instructions=sum(c['instructions'] for c in records),maximum=max(c['instructions'] for c in records),coverage=[len({pc for c in records for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],input_sha256=hashlib.sha256(json.dumps([c['parameters'] for c in records],sort_keys=True).encode()).hexdigest(),cases=records)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(packet,indent=2)+'\n');args.header.parent.mkdir(parents=True,exist_ok=True);args.header.write_text(header(records),newline='\n')
    print(packet['fixtures'],packet['instructions'],packet['maximum'],packet['coverage'])
if __name__=='__main__':main()
