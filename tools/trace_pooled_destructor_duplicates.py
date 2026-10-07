"""Actual five destructor PCs with controlled base/release effects.

The unchanged RegistryTrace integer decoder is reused, without PC spoofing.
Initialized aligned prefix carriers are synthetic; no allocator/class identity.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace
ROOT=Path(__file__).resolve().parents[1]
ENTRIES=(0x19A3D0,0x19D3F0,0x1F0120,0x22CB40,0x276C60)
OBJECT,MANAGER0,MANAGER1,GLOBAL=0x20100,0x20300,0x20400,0x4961F4
STACK=(0x7FF00,0x80100);MASK=0xFFFFFFFF;U128=(1<<128)-1
KEYS=('routine','mode','size','mutation','replacement','manager','offset')
class PooledTrace(Trace):
    def __init__(self,original,routine):
        if type(routine)is not int or routine not in range(5):raise ValueError('pooled routine domain')
        super().__init__(original);self.entry=ENTRIES[routine];self.stop=self.entry+84
        self.lo=self.hi=0;self.visited=set();self.events=[];self.branches=[];self.initializing=True;self.accesses=[];self.p=None
    def check(self,a,n):
        if n not in(1,2,4,8,16)or a%n or not (OBJECT<=a<a+n<=OBJECT+64 or GLOBAL<=a<a+n<=GLOBAL+4 or STACK[0]<=a<a+n<=STACK[1]):raise ValueError('unowned/unaligned pooled memory')
    def load(self,a,n):
        self.check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized pooled memory')
        v=Trace.load(self,a,n)
        if not self.initializing:self.accesses.append(['read',a,n,v])
        return v
    def save(self,a,v,n):
        self.check(a,n);Trace.save(self,a,v,n)
        if not self.initializing:self.accesses.append(['write',a,n,v&((1<<(8*n))-1)])
    def fetch(self,pc):
        if pc&3 or not self.entry<=pc<self.stop:raise ValueError('unreviewed pooled PC')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('pooled image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def execute(self,w,pc):
        if pc&3 or not self.entry<=pc<self.stop:raise ValueError('unreviewed pooled execution PC')
        if self.instruction_count>=100:raise ValueError('pooled instruction bound')
        return RegistryTrace.execute(self,w,pc)
    def external(self,target):
        if target not in(0x3064F0,0x2E2CD8):raise ValueError('unreviewed pooled external')
        obj=OBJECT+self.p['offset']
        if target==0x3064F0:
            args=[self.r[x]&MASK for x in(4,5)];assert args==[obj,0]
            self.events.append([target,*args,0,0])
            if self.p['mutation']&1:self.save(obj+4,self.p['replacement'],2)
            if self.p['mutation']&2:self.save(GLOBAL,MANAGER1,4)
            if self.p['mutation']&4:self.save(obj,0xD5A30102,4)
        elif target==0x2E2CD8:
            args=[self.r[x]&MASK for x in(4,5,6,7)];assert args[1]==obj and args[3]==0x27
            self.events.append([target,*args])
        else:raise ValueError('unreviewed pooled external')
        # Deliberately clobber caller-saved scalar GPRs, preserving s16/s17.
        for r in range(2,16):self.r[r]=0xC0DE0000+r
        self.r[24]=0xC0DE0018;self.r[25]=0xC0DE0019
    def run(self):
        pc=self.entry
        while True:
            if not self.entry<=pc<self.stop:raise ValueError('pooled transfer outside body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('pooled taken annul')
                    pc+=8;continue
                delay=self.fetch(pc+4)
                if is_control_transfer(delay)or self.execute(delay,pc+4)!=(None,False):raise ValueError('pooled control in delay')
                if w==0x03E00008:
                    if pc!=self.stop-8 or target!=RETURN:raise ValueError('pooled actual terminal JR31')
                    return
                if w>>26==3:
                    continuation=self.r[31]&MASK;self.external(target);pc=continuation
                else:pc=pc+8 if target is None else target
            else:
                if target is not None or annul:raise ValueError('pooled noncontrol transfer')
                pc+=4
def parameters():
    modes=(0,1,2,3,0x80000000,0x80000001,0xFFFFFFFF)
    sizes=(0,1,0x27,0x7FFF,0x8000,0xFFFF)
    return [dict(zip(KEYS,(r,m,s,mut,(s^0xBEEF)&0xFFFF,manager,offset)))for r in range(5)for m in modes for s in sizes for mut in range(8)for manager in(0,1,2)for offset in(0,16)]
def initialize(t,p):
    if set(p)!=set(KEYS)or any(type(p[k])is not int for k in KEYS)or p['routine']not in range(5)or not 0<=p['mode']<=MASK or not 0<=p['size']<=65535 or not 0<=p['replacement']<=65535 or p['mutation']not in range(8)or p['manager']not in(0,1,2)or p['offset']not in(0,16):raise ValueError('pooled fixture domain')
    t.p=p
    for i in range(64):t.save(OBJECT+i,(0xA5+17*i)&255,1)
    t.save(OBJECT+p['offset']+4,p['size'],2);t.save(GLOBAL,(0,MANAGER0,MANAGER1)[p['manager']],4)
    t.r[4]=OBJECT+p['offset'];t.r[5]=p['mode'];t.r[29]=0x80000;t.r[31]=RETURN
    t.r[16]=0x123456789ABCDEF00FEDCBA987654321;t.r[17]=0xDEADBEEF012345677654321001234567
    t.initializing=False
def fixture(original,p):
    t=PooledTrace(original,p['routine']);initialize(t,p);preserved=(t.r[16],t.r[17],t.r[29],t.r[31]);t.run()
    assert tuple(t.r[x]for x in(16,17,29,31))==preserved
    expected=[Trace.load(t,OBJECT+4*i,4)for i in range(16)]+[Trace.load(t,GLOBAL,4)]
    return dict(parameters=p,expected=expected,events=t.events,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,accesses=t.accesses,restored128_s16_s17=True)
def golden_header(rows):
    lines=['/* Authored initialized carriers and controlled callbacks; no original bytes. */','struct PooledGolden { u32 routine,mode,size,mutation,replacement,manager,offset;u32 expected[17];u32 event_count;u32 events[2][5]; };','static const struct PooledGolden pooled_golden[] = {']
    for x in rows:
        p=x['parameters'];events=x['events']+[[0]*5]*(2-len(x['events']))
        lines.append('{%s,{%s},%du,{%s}},'%(','.join('0x%08Xu'%p[k]for k in KEYS),','.join('0x%08Xu'%v for v in x['expected']),len(x['events']),','.join('{%s}'%','.join('0x%08Xu'%v for v in e)for e in events)))
    return '\n'.join(lines+['};',''])
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68');p.add_argument('--output',type=Path,default=ROOT/'build/pooled_destructor_duplicates/trace.json');p.add_argument('--golden-header',type=Path);a=p.parse_args();_,original=validated_elf(a.elf)
    inputs=parameters();rows=[fixture(original,x)for x in inputs];visited=sorted({pc for x in rows for pc in x['visited']});assert visited==[pc for e in ENTRIES for pc in range(e,e+84,4)]
    for e in ENTRIES:assert {taken for x in rows for pc,taken in x['branches']if pc==e+36}=={False,True}
    q=dict(fixtures=len(rows),new_actual_function_runs=5,selected_coverage=105,visited=visited,input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),total_instructions=sum(x['instructions']for x in rows),maximum_instructions=max(x['instructions']for x in rows),observations=rows,qualification='Five actual entry-PC ranges, unchanged published integer decoder; base3064F0/release2E2CD8 deliberately controlled, no heap/pool/SDK/class/vtable/data/return identity. Aligned initialized synthetic carriers; low32 pointer translation only. s16/s17 original full128 restores checked only in original observer, not host ABI.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(q,indent=2)+'\n',newline='\n')
    if a.golden_header:a.golden_header.parent.mkdir(parents=True,exist_ok=True);a.golden_header.write_text(golden_header(rows),newline='\n')
    print('Pooled fixtures',len(rows),'instructions',q['total_instructions'],'max',q['maximum_instructions'])
if __name__=='__main__':main()
