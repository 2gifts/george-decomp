"""Strict initialized timer observations, with deliberate native Count control.
Actual selected and supporting original instructions execute. No clock rate,
SDK identity, arbitrary allocation failure or hardware exception claim.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,sx32

ROOT=Path(__file__).resolve().parents[1]
BASE,WORDS=0x20000,6144;END=BASE+WORDS*4
TIMER,RATE,ALT,RECORD,SLOTS,DESC=0x20080,0x20100,0x20140,0x20180,0x20300,0x20500
POOL,FREE,HEAP,REENT,ATNEW=0x21000,0x23800,0x24000,0x24800,0x25000
RATEGLOBAL,RANGE,HEAPGLOBAL,REENTGLOBAL=0x3FD4D8,0x46A0F0,0x3FD204,0x405694
HANDLER,HEADS,START,FINISH,HEAPSIZE=0x3F21B0,0x3F21B8,0x3F21F8,0x3F21FC,0x3F2200
GLOBALS=((RATEGLOBAL,4),(RANGE,12),(HEAPGLOBAL,4),(REENTGLOBAL,4),(HANDLER,4),(HEADS,64),(START,4),(FINISH,4),(HEAPSIZE,4))
STACK=(0x7D000,0x81000)
SELECTED=((0x2BD4F8,0x2BD614),(0x2BDC18,0x2BDD50),(0x2BDD50,0x2BDE6C))
SUPPORT=((0x2AEE60,0x2AEF08),(0x2AF140,0x2AF1E8),(0x100C30,0x100CCC),(0x100AA8,0x100AB8),
 (0x1007E0,0x100AA4),(0x1005C8,0x1007DC),(0x2521F8,0x252294),(0x252168,0x2521F8),
 (0x3935A4,0x3936A0),(0x396260,0x3962FC),(0x2AF1E8,0x2AF204))
RANGES=SELECTED+SUPPORT
READONLY=((0x447AA0,0x447AB0),(0x447238,0x447264))
KEYS=('routine','lazy','count','spare','current','previous','reset','divider','mutation','timer_alias','cache','exit_count','exit_failure')
initial_word=lambda i:(0x8C370501^(i*0x10203))&MASK

class TimerTrace(RegistryTrace):
    def __init__(self,original,p):
        Trace.__init__(self,original);self.p=p;self.lo=self.hi=0
        self.visited=set();self.events=[];self.branches=[];self.invocations=[0]*len(RANGES)
        self.allocations=0;self.counter_reads=0;self.readonly_reads=set();self.active_entries=[]
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
          BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or any(g<=a<a+n<=g+s for g,s in GLOBALS)):
            raise ValueError('unowned or unaligned timer memory %#x/%d'%(a,n))
    def load(self,a,n):
        if any(lo<=a<a+n<=hi for lo,hi in READONLY):
            if a%n:raise ValueError('unaligned timer readonly')
            self.readonly_reads.add((a,n));off=a-0xFF000
            if off<0 or off+n>len(self.original):raise ValueError('timer readonly image bound')
            return int.from_bytes(self.original[off:off+n],'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized timer read')
        return Trace.load(self,a,n)
    def save(self,a,v,n):self.memory_check(a,n);Trace.save(self,a,v,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed timer instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('timer code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def event(self,k,a=0,b=0,c=0):
        if len(self.events)>=64:raise ValueError('timer event bound')
        self.events.append([k,a&MASK,b&MASK,c&MASK])
    def count_read(self):
        self.counter_reads+=1
        if self.counter_reads!=1:raise ValueError('timer controlled Count count')
        self.event(3,self.p['current'],self.p['mutation'],self.p['timer_alias'])
        timer=(RATE+8 if self.p['timer_alias'] else TIMER)
        if self.p['mutation']&2:self.save(timer,(self.p['previous']+17)&MASK,4)
        if self.p['mutation']&4:self.save(RATEGLOBAL,ALT,4)
        if self.p['mutation']&8:
            captured=ALT if self.p['lazy'] and self.p['mutation']&1 else RATE
            self.save(captured+8,7,4)
        return self.p['current']
    def execute(self,w,pc):
        # The only local decoder extension: exact reviewed MFC0 v0,Count.
        # Manufacturer gate is recorded separately; this supplies an explicit
        # observation value, never a clock/frequency/privilege simulation.
        if w==0x40024800:
            if pc not in (0x2BD5F0,):raise ValueError('unreviewed Count location')
            if self.instruction_count>=30000:raise ValueError('timer instruction bound before Count')
            self.instruction_count+=1;self.visited.add(pc)
            self.r[2]=sx32(self.count_read());self.r[0]=0
            return None,False
        return RegistryTrace.execute(self,w,pc)
    def external(self,target):
        a,b,c=(self.r[r]&MASK for r in (4,5,6))
        if target==0x2ADF60:
            if a!=HEAP or c not in (3,4) or not 0<b<=6000:raise ValueError('timer core allocation domain')
            if c==4:
                self.allocations+=1
                if b!=12 or self.allocations>2:raise ValueError('timer descriptor allocation count')
                self.event(1,a,b,self.allocations)
                self.r[2]=RATE if self.allocations==1 else RECORD
                if self.allocations==2 and self.p['mutation']&1:self.save(RATEGLOBAL,ALT,4)
            elif 0x396260 in self.active_entries:
                if b!=0x88:raise ValueError('timer atexit allocation width')
                self.event(5,a,b,self.p['exit_failure']);self.r[2]=0 if self.p['exit_failure'] else ATNEW
            else:
                self.event(2,a,b,0);self.r[2]=POOL
        elif target==0x2AE158:
            if a!=SLOTS:raise ValueError('timer unreviewed release')
            self.event(6,a,self.load(RANGE+4,4),self.load(RANGE+8,4))
        elif target==0x394F68:
            # Real allocation wrapper's diagnostic on the controlled atexit
            # allocation-failure path. Reporting/abort/OS behavior is not
            # implemented or claimed; this observer deliberately returns.
            if a!=0x447238 or b or c!=0x88 or self.r[7]&MASK!=3:raise ValueError('timer diagnostic observer contract')
            self.event(7,a,b,c);self.r[2]=0
        else:raise ValueError('unreviewed timer external %#x'%target)
        for r in range(3,16):self.r[r]=sx32(0xA13E0000+r)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>10:raise ValueError('unreviewed timer entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo;self.active_entries.append(entry)
        if entry==0x3935A4:
            d,s,n=(self.r[r]&MASK for r in (4,5,6))
            if n>256:raise ValueError('timer memmove observer count')
            self.event(4,d,s,n)
        try:
            while True:
                if not lo<=pc<hi:raise ValueError('timer transfer outside full body')
                w=self.fetch(pc);target,annul=self.execute(w,pc)
                if is_control_transfer(w):
                    self.branches.append([pc,target is not None])
                    if annul:
                        if target is not None:raise ValueError('timer annul taken')
                        pc+=8;continue
                    if pc+4>=hi:raise ValueError('timer missing delay')
                    delay=self.fetch(pc+4)
                    if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('timer control in delay')
                    if w==0x03E00008:
                        if target!=stop:raise ValueError('timer JR31/stop')
                        if index<3 and pc!=hi-8:raise ValueError('timer selected terminal JR31')
                        return
                    if w>>26==3 or w>>26==0 and w&63==9:
                        continuation=self.r[31]&MASK
                        if target in [a for a,b in RANGES]:self.run(target,continuation,depth+1)
                        else:self.external(target)
                        pc=continuation
                    elif w>>26==2 and target in [a for a,b in RANGES]:self.run(target,stop,depth+1);return
                    else:pc=target if target is not None else pc+8
                else:
                    if target is not None or annul:raise ValueError('timer unexpected transfer')
                    pc+=4
        finally:self.active_entries.pop()

def fixture(original,routine=0,lazy=0,count=2,spare=2,current=0x12345678,previous=0xFFFFFFFA,reset=0,divider=17,mutation=0,timer_alias=0,cache=0,exit_count=0,exit_failure=0):
    p=dict(zip(KEYS,(routine,lazy,count,spare,current,previous,reset,divider,mutation,timer_alias,cache,exit_count,exit_failure)))
    if routine not in range(3) or lazy not in (0,1) or not 0<=count<=12 or not 0<=spare<=3 or mutation not in range(32) or timer_alias not in (0,1) or cache not in range(3) or exit_count not in (0,1,31,32) or exit_failure not in (0,1):raise ValueError('timer fixture parameter domain')
    if mutation&16 and (not lazy or count or spare!=1):raise ValueError('timer range-end alias domain')
    if not all(type(v) is int and 0<=v<=MASK for v in (current,previous,divider)) or not -0x80000000<=reset<=0x7FFFFFFF:raise ValueError('timer fixture word domain')
    if not divider or timer_alias:raise ValueError('timer zero-divider excluded native domain')
    t=TimerTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xC0370501,4)
    for a,n in GLOBALS:
        for at in range(a,a+n,4):t.save(at,0,4)
    t.save(HEAPGLOBAL,HEAP,4);t.save(REENTGLOBAL,REENT,4)
    for a,v in ((HEAP+0x18,0),(HEAP+0x24,0x100000),(HEAP+0x28,0),(RATE+4,0x1193FF10),(RATE+8,divider),(ALT+4,0xFE123456),(ALT+8,31)):
        t.save(a,v,4)
    t.save(RATEGLOBAL,0 if lazy else RATE,4)
    total=count+spare;begin=RANGE+4 if mutation&16 else SLOTS if total else 0
    t.save(RANGE,begin,4);t.save(RANGE+4,begin+4*count if begin else 0,4);t.save(RANGE+8,begin+4*total if begin else 0,4)
    for i in range(count):
        t.save(SLOTS+4*i,DESC+12*i,4);t.save(DESC+12*i,i*7,4);t.save(DESC+12*i+4,0,4);t.save(DESC+12*i+8,0,4)
    timer=RATE+8 if timer_alias else TIMER;t.save(timer,previous,4)
    # An aliased elapsed timer shares the divider word; require initialized
    # nonzero data for the scaled path before any native division.
    if routine==2 and timer_alias and not previous:raise ValueError('timer aliased initial divider zero')
    t.save(REENT+0x148,REENT+0x14C if exit_count else 0,4);t.save(REENT+0x150,exit_count,4)
    for at in range(REENT+0x14C,REENT+0x1D4,4):t.save(at,0,4)
    t.save(REENT+0x150,exit_count,4)
    request=4*(2*count if count else 1);block=(request+7)&~7
    if cache==1:
        t.save(HEADS+4*(block//8-1),POOL,4);t.save(POOL,0,4)
    elif cache==2:
        t.save(START,POOL,4);t.save(FINISH,POOL+block*20,4)
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    global_cells=[a+i for a,n in GLOBALS for i in range(0,n,4)];before=[t.load(a,4) for a in global_cells]
    t.r[4]=sx32(reset);t.r[5]=sx32(previous);t.r[29]=0x80000;t.r[31]=RETURN
    saved=[0xF00D1234567890000000000000000000+i for i in range(16,24)]+[0xABCDEF01234567890123456789ABCDE0]
    for r,v in zip([*range(16,24),30],saved):t.r[r]=v
    t.run(SELECTED[routine][0]);assert t.r[29]==0x80000 and [t.r[r] for r in [*range(16,24),30]]==saved
    after=[t.load(BASE+4*i,4) for i in range(WORDS)]
    pointers={SLOTS+4*i for i in range(count)}|{DESC+12*i+4 for i in range(count)}|{DESC+12*i+8 for i in range(count)}
    if lazy:pointers|={RECORD+4,RECORD+8}
    if lazy and spare and not mutation&16:pointers.add(SLOTS+4*count) # newly live appended void* slot
    if lazy and not spare and not (count==0 and total==0):
        pointers|={SLOTS} # genuine old storage becomes a free-list node
    if lazy and not spare:
        pointers|={POOL+4*i for i in range(count+1)}
        # A cached single block runs no refill. Other fixture modes publish
        # nineteen genuine free-list links after taking the first block.
        if cache!=1:
            for a in range(POOL+block,POOL+block*20,block):pointers.add(a)
    for a in (REENT+0x148,REENT+0x14C):pointers.add(a)
    if lazy and exit_count>=32 and not exit_failure:pointers|={ATNEW,ATNEW+8}
    elif lazy and exit_count<32:pointers.add(REENT+0x154+4*exit_count)
    # Actual initialized argument lanes, not a claimed original prototype.
    # Only routine1 consumes GPR5; GPR4's original type/use is unresolved.
    return dict(parameters=p,args=[reset&MASK,previous&MASK],initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],changes=[[i,v] for i,v in enumerate(after) if v!=initial[i]],
        globals_initial=before,globals_expected=[t.load(a,4) for a in global_cells],global_cells=global_cells,
        pointer_cells=sorted((a-BASE)//4 for a in pointers),events=t.events,result=t.r[2]&MASK,visited=sorted(t.visited),branches=t.branches,
        instructions=t.instruction_count,invocations=t.invocations,counter_reads=t.counter_reads)

def cases(original):
    out=[]
    for routine in range(3):
        for lazy in (0,1):
            for count,spare in ((0,0),(0,1),(1,0),(2,0),(4,0),(8,0),(12,0),(1,2),(4,2)):
                for cache in (0,1,2) if lazy and not spare else (0,):
                    out.append(fixture(original,routine=routine,lazy=lazy,count=count,spare=spare,cache=cache))
    for routine in range(3):
        for lazy in (0,1):
            for mutation in range(16):
                out.append(fixture(original,routine=routine,lazy=lazy,mutation=mutation))
    for value in (0,1,16,17,18,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF):
        for divider in (1,3,17,0x7FFFFFFF,0x80000000,0xFFFFFFFF):
            for unused in (0,-1,0x7FFFFFFF,-0x80000000):
                out.append(fixture(original,routine=1,previous=value,divider=divider,reset=unused))
    for current in (0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFF):
        for lazy in (0,1):
            out.append(fixture(original,current=current,lazy=lazy,mutation=14))
    for routine in range(3):
        for exit_count in (1,31,32):
            for failed in (0,1):
                out.append(fixture(original,routine=routine,lazy=1,exit_count=exit_count,exit_failure=failed))
        out.append(fixture(original,routine=routine,lazy=1,count=0,spare=1,mutation=16))
    return out

def header(records):
    lines=['/* Initialized synthetic observations only; no original instructions. */',
      'struct TimerPair { unsigned int index,value; };',
      'struct TimerCase { unsigned int p[13],initial_count,change_count,pointer_count,event_count,result,global_count; const struct TimerPair *initial,*changes; const unsigned int *pointers,*events,*globals_initial,*globals_expected; };']
    for i,c in enumerate(records):
        for label in ('initial','changes'):
            lines.append('static const struct TimerPair timer_%s_%d[]={%s};'%(label,i,','.join('{%du,0x%08Xu}'%(a,b) for a,b in c[label]) or '{0,0}'))
        for label,rows in [('pointers',c['pointer_cells']),('events',[v for row in c['events'] for v in row]),('globals_initial',c['globals_initial']),('globals_expected',c['globals_expected'])]:
            lines.append('static const unsigned int timer_%s_%d[]={%s};'%(label,i,','.join('0x%08Xu'%v for v in rows) or '0'))
    lines.append('static const struct TimerCase timer_cases[]={')
    for i,c in enumerate(records):
        p=','.join('0x%08Xu'%(c['parameters'][k]&MASK) for k in KEYS)
        lines.append('{{%s},%d,%d,%d,%d,0x%08Xu,%d,timer_initial_%d,timer_changes_%d,timer_pointers_%d,timer_events_%d,timer_globals_initial_%d,timer_globals_expected_%d},'%(p,len(c['initial']),len(c['changes']),len(c['pointer_cells']),len(c['events'])*4,c['result'],len(c['global_cells']),i,i,i,i,i,i))
    lines.append('};\n');return '\n'.join(lines)

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/timer_integer/trace.json');p.add_argument('--header',type=Path,default=ROOT/'tests/native/timer_integer_golden.h');a=p.parse_args()
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');records=cases(original)
    packet=dict(fixtures=len(records),instructions=sum(c['instructions'] for c in records),maximum=max(c['instructions'] for c in records),coverage=[len({pc for c in records for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],input_sha256=hashlib.sha256(json.dumps([c['parameters'] for c in records],sort_keys=True).encode()).hexdigest(),cases=records,
        limits='Count is explicit control; no hardware clock/SDK/time-unit/exception model. Initialized finite objects and bounded legitimate pointer domains; no invented production guard or capacity. Actual supporting originals are observations, not additional source recovery.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(packet,indent=2)+'\n');a.header.parent.mkdir(parents=True,exist_ok=True);a.header.write_text(header(records),newline='\n')
    print(packet['fixtures'],packet['instructions'],packet['maximum'],packet['coverage'])
if __name__=='__main__':main()
