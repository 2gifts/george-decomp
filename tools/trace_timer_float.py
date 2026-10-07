"""Original timer float observations in a bounded mathematical exact domain.

The immutable published integer/Count/registry observer supplies its complete
initialized memory, callback and scalar decoder contracts. Only the approved
22 original COP1 PC/word pairs are handled locally. No general EE rounding,
FCR31, hardware clock, original prototype or invocation equivalence is claimed.
"""
import argparse,hashlib,json,struct
from fractions import Fraction
from pathlib import Path
from analyze import validated_elf
from trace_geometry import RETURN,is_control_transfer
from trace_string_registry import MASK,sx32
from trace_timer_registry import (TimerTrace as PublishedTimerTrace,BASE,WORDS,END,
 TIMER,RATE,ALT,RECORD,SLOTS,DESC,POOL,FREE,HEAP,REENT,ATNEW,RATEGLOBAL,RANGE,
 HEAPGLOBAL,REENTGLOBAL,HANDLER,HEADS,START,FINISH,HEAPSIZE,GLOBALS,STACK,
 SUPPORT,READONLY,initial_word,header)

ROOT=Path(__file__).resolve().parents[1]
SELECTED=((0x2BD770,0x2BD924),(0x2BDA78,0x2BDC14))
RANGES=SELECTED+((0x2BD618,0x2BD76C),)+SUPPORT
KEYS=('routine','lazy','count','spare','current','previous','reset','divider',
 'mutation','timer_alias','cache','exit_count','exit_failure')
COP1={0x2BD894:0x44820000,0x2BD89C:0x46800020,0x2BD8B4:0x44820000,
 0x2BD8BC:0x46800020,0x2BD8C0:0x46000000,0x2BD8CC:0x44840800,
 0x2BD8D4:0x46800860,0x2BD8E8:0x44820800,0x2BD8F0:0x46800860,
 0x2BD8F4:0x46010840,0x2BD900:0x46010003,0x2BDB88:0x44910000,
 0x2BDB90:0x46800020,0x2BDBA8:0x44820000,0x2BDBB0:0x46800020,
 0x2BDBB4:0x46000000,0x2BDBC0:0x44840800,0x2BDBC8:0x46800860,
 0x2BDBDC:0x44820800,0x2BDBE4:0x46800860,0x2BDBE8:0x46010840,
 0x2BDBF4:0x46010003}

def exact_u32(n):
    return type(n) is int and 0<=n<=MASK and (not n or n% (1<<max(0,n.bit_length()-24))==0)
def power_divisor(n):return type(n) is int and 0<n<=0x80000000 and not n&(n-1)
def encode_exact(value):
    """Positive/zero exact binary24 normal value -> raw32 without host float."""
    value=Fraction(value)
    if value<0:raise ValueError('negative exact timer value')
    if not value:return 0
    n,d=value.numerator,value.denominator
    if d&(d-1):raise ValueError('nonbinary exact timer value')
    exponent=n.bit_length()-1-(d.bit_length()-1)
    if not -31<=exponent<=31:raise ValueError('timer normal quotient bound')
    shift=n.bit_length()-24
    if shift>0:
        if n%(1<<shift):raise ValueError('inexact binary24 timer value')
        significand=n>>shift
    else:significand=n<<-shift
    return ((exponent+127)<<23)|(significand&0x7fffff)
def decode_exact(bits):
    if bits is None:raise ValueError('uninitialized timer FPR')
    if bits==0:return Fraction(0)
    exponent=(bits>>23)&255
    if bits>>31 or not 96<=exponent<=158:raise ValueError('outside timer exact normal FPR domain')
    value=Fraction((bits&0x7fffff)|0x800000)
    power=exponent-127-23
    return value*(1<<power) if power>=0 else value/Fraction(1<<-power)

class FloatTimerTrace(PublishedTimerTrace):
    def __init__(self,original,p):
        super().__init__(original,p);self.invocations=[0]*len(RANGES)
        self.fraw=[None]*32;self.cop1_steps=[]
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed float timer instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('float timer code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def count_read(self):
        # Genuine elapsed body invokes this once. Controlled mutation occurs
        # before its fresh timer read and before the caller's fresh +4 load.
        if self.counter_reads:raise ValueError('timer controlled Count count')
        self.counter_reads+=1;self.event(3,self.p['current'],self.p['mutation'],self.p['timer_alias'])
        timer=RATE+4 if self.p['timer_alias'] else TIMER
        if self.p['mutation']&2:self.save(timer,(self.p['previous']+17)&MASK,4)
        if self.p['mutation']&4:self.save(RATEGLOBAL,ALT,4)
        if self.p['mutation']&8:
            captured=ALT if self.p['lazy'] and self.p['mutation']&1 else RATE
            self.save(captured+4,0x40000000,4)
        return self.p['current']
    def external(self,target):
        a,b,c=(self.r[r]&MASK for r in (4,5,6))
        super().external(target)
        if target==0x2ADF60 and c==4 and self.allocations==2 and self.p['routine']==1 and self.p['mutation']&8:
            self.save(RATE+4,0x40000000,4)
    def execute(self,w,pc):
        if w==0x40024800:
            if pc!=0x2BD720:raise ValueError('unreviewed float timer Count location')
            if self.counter_reads:raise ValueError('timer controlled Count count')
        if w>>26!=17:return super().execute(w,pc)
        # Reject all unsupported encodings, positions, domains and budgets
        # before changing the FPRs/count/visited/r0 or executing callbacks.
        if COP1.get(pc)!=w:raise ValueError('unapproved float timer COP1 PC/word')
        if self.instruction_count>=30000:raise ValueError('float timer budget before COP1')
        fmt,ft,fs,fd,fn=w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
        if fmt==4:
            value=self.r[ft]&MASK
            if value>=0x80000000 or not exact_u32(value):raise ValueError('inexact signed conversion input')
            if fs==1 and not power_divisor(value):raise ValueError('unsupported divisor input')
            destination=fs;bits=value
        elif fmt==20 and fn==32:
            value=self.fraw[fs]
            if value is None or value>=0x80000000 or not exact_u32(value):raise ValueError('inexact signed conversion FPR')
            destination=fd;bits=encode_exact(value)
        elif fmt==16 and fn==0:
            value=decode_exact(self.fraw[fs])+decode_exact(self.fraw[ft])
            destination=fd;bits=encode_exact(value)
        elif fmt==16 and fn==3:
            numerator=decode_exact(self.fraw[fs]);denominator=decode_exact(self.fraw[ft])
            if denominator.denominator!=1 or not power_divisor(denominator.numerator):raise ValueError('unsupported divisor FPR')
            if numerator.denominator!=1 or not exact_u32(numerator.numerator):raise ValueError('unsupported numerator FPR')
            destination=fd;bits=encode_exact(numerator/denominator)
        else:raise ValueError('unapproved float timer COP1 operation')
        self.instruction_count+=1;self.visited.add(pc);self.fraw[destination]=bits;self.r[0]=0
        self.cop1_steps.append([pc,w,destination,bits]);return None,False
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>10:raise ValueError('unreviewed float timer entry/depth')
        if index<2 and self.r[5]!=sx32(self.r[5]&MASK):raise ValueError('incoming GPR5 is not signextended u32/reset')
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
                        if index<2 and pc!=hi-8:raise ValueError('timer selected terminal JR31')
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

def fixture(original,routine=0,lazy=0,count=2,spare=2,current=1024,previous=0,reset=0,divider=256,mutation=0,timer_alias=0,cache=0,exit_count=0,exit_failure=0):
    p=dict(zip(KEYS,(routine,lazy,count,spare,current,previous,reset,divider,mutation,timer_alias,cache,exit_count,exit_failure)))
    if routine not in (0,1) or lazy not in (0,1) or not 0<=count<=12 or not 0<=spare<=3 or mutation not in range(32) or timer_alias not in (0,1) or cache not in range(3) or exit_count not in (0,1,31,32) or exit_failure not in (0,1):raise ValueError('timer fixture parameter domain')
    if mutation&16 and (not lazy or count or spare!=1):raise ValueError('timer range-end alias domain')
    if not all(type(v) is int and 0<=v<=MASK for v in (current,previous,divider)) or not -0x80000000<=reset<=0x7fffffff:raise ValueError('timer fixture word domain')
    if not power_divisor(divider):raise ValueError('unsupported initial divisor')
    if routine==1 and timer_alias:raise ValueError('raw timer has no pointer argument')
    # Mathematical domain preflight, before initialized memory/callbacks/run.
    captured_alt=lazy and mutation&1
    denominator=0x80000000 if captured_alt else 0x1193ff10 if lazy else divider
    if routine==0:
        old=previous
        if timer_alias:old=previous
        if mutation&2:old=(previous+17)&MASK
        if mutation&8:
            denominator=0x40000000
            if timer_alias and not captured_alt:old=denominator
        if timer_alias and not captured_alt:
            denominator=current if reset else old
        numerator=(current-old)&MASK
    else:
        numerator=previous
        if lazy and mutation&8 and not captured_alt:denominator=0x40000000
    if not exact_u32(numerator) or not power_divisor(denominator):raise ValueError('unsupported exact numerical fixture')
    t=FloatTimerTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xC0370501,4)
    for a,n in GLOBALS:
        for at in range(a,a+n,4):t.save(at,0,4)
    t.save(HEAPGLOBAL,HEAP,4);t.save(REENTGLOBAL,REENT,4)
    for a,v in ((HEAP+0x18,0),(HEAP+0x24,0x100000),(HEAP+0x28,0),(RATE+4,divider),(RATE+8,17),(ALT+4,0x80000000),(ALT+8,31)):
        t.save(a,v,4)
    t.save(RATEGLOBAL,0 if lazy else RATE,4)
    total=count+spare;begin=RANGE+4 if mutation&16 else SLOTS if total else 0
    t.save(RANGE,begin,4);t.save(RANGE+4,begin+4*count if begin else 0,4);t.save(RANGE+8,begin+4*total if begin else 0,4)
    for i in range(count):
        t.save(SLOTS+4*i,DESC+12*i,4);t.save(DESC+12*i,i*7,4);t.save(DESC+12*i+4,0,4);t.save(DESC+12*i+8,0,4)
    timer=RATE+4 if timer_alias else TIMER;t.save(timer,previous,4)
    for at in range(REENT+0x14C,REENT+0x1D4,4):t.save(at,0,4)
    t.save(REENT+0x148,REENT+0x14C if exit_count else 0,4);t.save(REENT+0x150,exit_count,4)
    request=4*(2*count if count else 1);block=(request+7)&~7
    if cache==1:t.save(HEADS+4*(block//8-1),POOL,4);t.save(POOL,0,4)
    elif cache==2:t.save(START,POOL,4);t.save(FINISH,POOL+block*20,4)
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    global_cells=[a+i for a,n in GLOBALS for i in range(0,n,4)];before=[t.load(a,4) for a in global_cells]
    args=[timer,reset&MASK] if routine==0 else [reset&MASK,previous]
    t.r[4]=sx32(args[0]);t.r[5]=sx32(args[1]);t.r[29]=0x80000;t.r[31]=RETURN
    saved=[0xF00D1234567890000000000000000000+i for i in range(16,24)]+[0xABCDEF01234567890123456789ABCDE0]
    for r,v in zip([*range(16,24),30],saved):t.r[r]=v
    t.run(SELECTED[routine][0]);assert t.r[29]==0x80000 and [t.r[r] for r in [*range(16,24),30]]==saved
    result=encode_exact(Fraction(numerator,denominator));assert t.fraw[0]==result
    after=[t.load(BASE+4*i,4) for i in range(WORDS)]
    pointers={SLOTS+4*i for i in range(count)}|{DESC+12*i+4 for i in range(count)}|{DESC+12*i+8 for i in range(count)}
    if lazy:pointers|={RECORD+4,RECORD+8}
    if lazy and spare and not mutation&16:pointers.add(SLOTS+4*count)
    if lazy and not spare and not (count==0 and total==0):pointers.add(SLOTS)
    if lazy and not spare:
        pointers|={POOL+4*i for i in range(count+1)}
        if cache!=1:
            for a in range(POOL+block,POOL+block*20,block):pointers.add(a)
    pointers|={REENT+0x148,REENT+0x14C}
    if lazy and exit_count>=32 and not exit_failure:pointers|={ATNEW,ATNEW+8}
    elif lazy and exit_count<32:pointers.add(REENT+0x154+4*exit_count)
    return dict(parameters=p,args=args,initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],changes=[[i,v] for i,v in enumerate(after) if v!=initial[i]],
        globals_initial=before,globals_expected=[t.load(a,4) for a in global_cells],global_cells=global_cells,
        pointer_cells=sorted((a-BASE)//4 for a in pointers),events=t.events,result=result,numerator=numerator,denominator=denominator,
        visited=sorted(t.visited),branches=t.branches,instructions=t.instruction_count,invocations=t.invocations,counter_reads=t.counter_reads,cop1_steps=t.cop1_steps)

def cases(original):
    out=[]
    values=[0,1,3,0x7fffff,0x800000,0xffffff,0x1000000,0x1000002,0x7fffff80,0x80000000,0xffffff00]
    for value in values:
        for divider in (1,2,128,0x1000000,0x40000000,0x80000000):
            for unused in (0,-1,-0x80000000):out.append(fixture(original,routine=1,previous=value,divider=divider,reset=unused))
            out.append(fixture(original,current=value,previous=0,divider=divider))
    for routine in (0,1):
        for count,spare in ((0,0),(0,1),(1,0),(2,0),(4,0),(8,0),(12,0),(1,2),(4,2)):
            for cache in (0,1,2) if not spare else (0,):
                out.append(fixture(original,routine=routine,lazy=1,count=count,spare=spare,cache=cache,mutation=1))
                out.append(fixture(original,routine=routine,lazy=1,count=count,spare=spare,cache=cache,mutation=8))
    for mutation in (0,2,4,6,8,10,12,14):
        for reset in (0,1,-1):out.append(fixture(original,current=4096,previous=64,reset=reset,mutation=mutation))
    for reset in (0,1,-1):
        for current,previous in ((1024,256),(0x80000000,0x40000000),(256,1024)):
            out.append(fixture(original,current=current,previous=previous,reset=reset,divider=256,timer_alias=1))
    for routine in (0,1):
        for exit_count in (1,31,32):
            for failed in (0,1):out.append(fixture(original,routine=routine,lazy=1,mutation=1,exit_count=exit_count,exit_failure=failed))
        out.append(fixture(original,routine=routine,lazy=1,count=0,spare=1,mutation=17))
    return out

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/timer_float/trace.json');p.add_argument('--header',type=Path,default=ROOT/'tests/native/timer_float_golden.h');a=p.parse_args()
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');records=cases(original)
    packet=dict(fixtures=len(records),instructions=sum(c['instructions'] for c in records),maximum=max(c['instructions'] for c in records),coverage=[len({pc for c in records for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],input_sha256=hashlib.sha256(json.dumps([c['parameters'] for c in records],sort_keys=True).encode()).hexdigest(),cases=records,
        limits='New float selected observations only. Exact signextended-u32/binary24 numerator and positive power2 divisor; exact normal/zero quotient model, no general EE rounding/FCR31/hardware proof. Default294911760 divisor excluded unless controlled later mutation supplies supported data. Complete unchanged supporting observer/source get no new recovery/fixture credit. No production domain guards.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(packet,indent=2)+'\n');a.header.parent.mkdir(parents=True,exist_ok=True);a.header.write_text(header(records),newline='\n')
    print(packet['fixtures'],packet['instructions'],packet['maximum'],packet['coverage'])
if __name__=='__main__':main()
