"""Strict initialized packet observations; no network/class or SDK identity.

All five entries and listed SGI/string/heap-wrapper originals execute. Only
heap core, diagnostics and an OOM handler are controlled effects. Count=0
permits unused null addresses. Positive fills and string/copy ranges must be
readable initialized storage; no overlap claim is added to memcpy.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,sx32
ROOT=Path(__file__).resolve().parents[1]
BASE,WORDS=0x20000,4096;END=BASE+WORDS*4
HEAP,HOLDER,RANGE,DEST=0x20040,0x20100,0x20140,0x20200
FIRST,SECOND,PAYLOAD,VALUE=0x20600,0x20700,0x20800,0x20900
POOL,NODE,NODE2=0x21000,0x21c00,0x21d00
STACK=(0x7b000,0x81000)
HEADS,BEGIN,FINISH,HEAPSIZE,HANDLER,HEAPGLOBAL=0x3f21b8,0x3f21f8,0x3f21fc,0x3f2200,0x3f21b0,0x3fd204
HANDLER_TARGET=0xf1100000
GLOBALS=tuple(range(HEADS,HEADS+64,4))+(BEGIN,FINISH,HEAPSIZE,HANDLER,HEAPGLOBAL)
SELECTED=((0x2bcb50,0x2bcc9c),(0x2bcca0,0x2bcdf8),(0x2bcfa8,0x2bd000),(0x2bd000,0x2bd0c0),(0x2bd1f0,0x2bd218))
SUPPORT=((0x295050,0x295080),(0x3934f8,0x3935a4),(0x2aee60,0x2aef08),(0x2af140,0x2af1e8),(0x252168,0x2521f8),(0x2521f8,0x252294),(0x1005c8,0x1007dc))
RANGES=SELECTED+SUPPORT
initial_word=lambda i:(0xe25a8301^(i*0x10203))&MASK
KEYS=('routine','length','capacity','allocator','mutation','failure','alias','first_length','second_length','value')
class PacketTrace(RegistryTrace):
    def __init__(self,original,parameters):
        super().__init__(original,parameters);self.invocations=[0]*len(RANGES)
        self.allocations=0;self.failed=0;self.current_pc=0;self.pointer_words=set();self.fill_reads=0
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or any(g<=a<a+n<=g+4 for g in GLOBALS)):
            raise ValueError('unowned or unaligned packet memory %#x/%d pc%#x parameters%s'%(a,n,self.current_pc,self.p))
    def load(self,a,n):
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized packet read %#x/%d'%(a,n))
        if self.current_pc==0x2bd1f8:self.fill_reads+=1
        return Trace.load(self,a,n)
    def save(self,a,v,n):
        self.memory_check(a,n)
        # A typed pointer ledger, never translate coincidental arbitrary words.
        for slot in tuple(self.pointer_words):
            if a<slot+4 and slot<a+n:self.pointer_words.remove(slot)
        if n==4 and self.current_pc in (0x2bcc24,0x2bcc28,0x2bcc30,0x2bcc48,0x2bcc68,0x2bcd88,0x2bcd8c,0x2bcd94,0x2bcdac,0x2bcdc8,0x252250,0x252260,0x25227c,0x100634,0x10063c,0x100640,0x100648,0x10067c,0x1006c4,0x1006cc,0x1006dc,0x100728,0x100788,0x1007a0):
            self.pointer_words.add(a)
        Trace.save(self,a,v,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed packet instruction')
        offset=pc-0xff000
        if offset<0 or offset+4>len(self.original):raise ValueError('packet image bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def execute(self,w,pc):
        self.current_pc=pc
        return super().execute(w,pc)
    def event(self,kind,*args):
        if len(self.events)>=32:raise ValueError('packet event bound')
        self.events.append([kind,*[x&MASK for x in args]])
    def put(self,address,value):
        for i,x in enumerate(value+b'\0'):self.save(address+i,x,1)
    def mutate(self):
        mutation=self.p['mutation']
        if mutation==1:self.put(FIRST,b'X');self.put(SECOND,b'Y')
        elif mutation==2:self.put(FIRST,b'LongerFirstInput');self.put(SECOND,b'LongerSecondInput')
        elif mutation==3:self.put(FIRST,b'');self.put(SECOND,b'')
    def external(self,target):
        a,b,c=[self.r[r]&MASK for r in (4,5,6)];result=0
        if target==0x2adf60:
            if a!=HEAP or c not in (3,4):raise ValueError('packet heap core observed arguments')
            self.allocations+=1;self.event(1,b,c,self.allocations)
            if self.allocations==1:
                if b!=12 or c!=4:raise ValueError('packet range allocation contract')
                result=RANGE;self.mutate()
            elif self.p['failure'] and not self.failed:
                self.failed+=1;result=0
            else:result=POOL
        elif target==0x394f68:
            if a!=0x447238:raise ValueError('packet allocation diagnostic')
            self.event(2,b,c,self.r[7]&MASK)
        elif target==HANDLER_TARGET:
            self.event(3,self.failed,0,0);self.mutate()
        else:raise ValueError('unreviewed packet external %#x'%target)
        for r in range(3,16):self.r[r]=sx32(0xcafebabe)
        self.r[2]=sx32(result)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>20:raise ValueError('packet entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo
        while True:
            if not lo<=pc<hi:raise ValueError('packet transfer outside body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('packet annul taken')
                    pc+=8;continue
                if pc+4>=hi:raise ValueError('packet delay outside body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('packet control in delay')
                if w==0x03e00008:
                    if target!=stop:raise ValueError('packet JR31 return')
                    if index<5 and pc!=hi-8:raise ValueError('packet terminal return')
                    return
                if w>>26==3 or w>>26==0 and w&63==9:
                    continuation=self.r[31]&MASK
                    if target in [a for a,b in RANGES]:self.run(target,continuation,depth+1)
                    else:self.external(target)
                    pc=continuation
                elif w>>26==2 and target in [a for a,b in RANGES]:self.run(target,stop,depth+1);return
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('packet unexpected transfer')
                pc+=4

def fixture(raw,routine=0,length=9,capacity=64,allocator=0,mutation=0,failure=0,alias=0,first_length=4,second_length=7,value=0xa5):
    parameters=dict(zip(KEYS,(routine,length,capacity,allocator,mutation,failure,alias,first_length,second_length,value)))
    if routine not in range(5) or allocator not in range(4) or mutation not in range(4) or alias not in range(5) or max(length,first_length,second_length)>256:raise ValueError('packet fixture parameters')
    t=PacketTrace(raw,parameters)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for address in GLOBALS:t.save(address,0,4)
    for address,v in ((HEAPGLOBAL,HEAP),(HANDLER,HANDLER_TARGET),(HEAP+24,0),(HEAP+36,0x100000),(HEAP+40,0),(NODE,NODE2),(NODE2,0)):
        t.save(address,v,4)
        if address not in (HANDLER,HEAP+36,HEAP+40):t.pointer_words.add(address)
    t.put(FIRST,b'A'*first_length);t.put(SECOND,b'B'*second_length)
    for i in range(256):t.save(PAYLOAD+i,(37*i+11)&255,1)
    t.save(VALUE,value&255,1)
    bytes_needed=length+8 if routine==0 else first_length+second_length+6
    rounded=(bytes_needed+7)&~7
    if allocator==1 and 0<bytes_needed<=128:t.save(HEADS+4*((rounded>>3)-1),NODE,4);t.pointer_words.add(HEADS+4*((rounded>>3)-1))
    if allocator==2:
        t.save(BEGIN,POOL,4);t.save(FINISH,POOL+1280,4);t.pointer_words.update((BEGIN,FINISH))
    if allocator==3:
        t.save(BEGIN,POOL,4);t.save(FINISH,POOL+8,4);t.save(HEAPSIZE,128,4);t.pointer_words.update((BEGIN,FINISH))
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)];global_initial=[t.load(g,4) for g in GLOBALS];pointer_initial=sorted(t.pointer_words)
    t.r[29]=0x80000;t.r[31]=RETURN;t.current_pc=0
    holder=HOLDER if alias==0 else RANGE if alias==1 else POOL if alias==2 else FIRST if alias==3 else SECOND
    destination=DEST+16*alias
    if routine==0:args=[holder,0xabcdef12,0x98765434,PAYLOAD,length]
    elif routine==1:args=[holder,FIRST,SECOND,0x89abcdef]
    elif routine==2:args=[destination,capacity,0xabcdef12,0x98765434,destination if alias==4 and length<=4 else PAYLOAD,length]
    elif routine==3:args=[FIRST if alias==4 and first_length<=2 else destination&~3,capacity,FIRST,SECOND,0x89abcdef]
    else:args=[DEST+(alias&3),length,VALUE if alias==0 else DEST+(alias&3)+(0 if alias==1 else length//2 if alias==2 else length if alias==3 else 3)]
    if routine==4:t.save(args[2],value&255,1);initial=[t.load(BASE+4*i,4)for i in range(WORDS)]
    for register,argument in enumerate(args,4):t.r[register]=sx32(argument)
    t.run(SELECTED[routine][0]);expected=[t.load(BASE+4*i,4)for i in range(WORDS)];global_expected=[t.load(g,4)for g in GLOBALS]
    if routine==4 and t.fill_reads!=length:raise ValueError('fill source-reference load count')
    return dict(p=list(parameters.values()),args=args,result=t.r[2]&MASK,initial=[[i,v]for i,v in enumerate(initial)if v!=initial_word(i)],changes=[[i,v]for i,v in enumerate(expected)if v!=initial[i]],global_initial=global_initial,global_expected=global_expected,pointer_initial=[(x-BASE)//4 for x in pointer_initial if BASE<=x<END],pointer_expected=[(x-BASE)//4 for x in sorted(t.pointer_words)if BASE<=x<END],events=t.events,invocations=t.invocations,fill_reads=t.fill_reads,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches)

def fixtures(raw):
    cases=[]
    for routine in range(5):
        for n in (0,1,2,3,7,8,9,15,16,17,31,32,63,64,65,120,121,128,129,255,256):
            if routine==4:
                for alias in range(5):cases.append(fixture(raw,routine=routine,length=n,alias=alias,value=(n*29)&255))
            elif routine==2:
                for cap in sorted({0,max(0,n+7),n+8,n+9,0xffffffff}):
                    for alias in (0,1,2,3,4):cases.append(fixture(raw,routine=routine,length=n,capacity=cap,alias=alias))
            elif routine==3:
                for cap in (0,13,14,15,64,0xffffffff):cases.append(fixture(raw,routine=routine,capacity=cap,first_length=n,second_length=n//2))
            else:
                for allocator in range(4):
                    for mutation in range(4) if routine==1 else (0,):
                        cases.append(fixture(raw,routine=routine,length=n,allocator=allocator,mutation=mutation,first_length=n,second_length=n//2))
                for failure in (0,1):cases.append(fixture(raw,routine=routine,length=n,failure=failure,first_length=n,second_length=n//2))
        if routine in (0,1):
            for alias in range(5):cases.append(fixture(raw,routine=routine,alias=alias))
    for n in (0,1,2):cases.append(fixture(raw,routine=3,first_length=n,second_length=n,alias=4))
    return cases

def golden(cases):
    out=['/* Synthetic initialized observations, no original code/data bytes. */','#ifndef PACKET_CONSTRUCTION_GOLDEN_H','#define PACKET_CONSTRUCTION_GOLDEN_H',
      'typedef struct { unsigned int index,value; } PacketWord;',
      'typedef struct { unsigned int p[10],args[6],result; unsigned int initial_count,change_count; const PacketWord *initial,*changes; unsigned int globals_initial[21],globals_expected[21]; unsigned int pointer_initial_count,pointer_expected_count; const unsigned int *pointer_initial,*pointer_expected; unsigned int event_count; const unsigned int *events; } PacketCase;']
    word=lambda v:'0x%08Xu'%v
    for i,c in enumerate(cases):
        for kind in ('initial','changes'):
            out.append('static const PacketWord pc_%s_%d[]={%s};'%(kind,i,','.join('{%s,%s}'%(word(a),word(b))for a,b in c[kind])or'{0,0}'))
        for kind in ('pointer_initial','pointer_expected'):
            out.append('static const unsigned int pc_%s_%d[]={%s};'%(kind,i,','.join(word(v)for v in c[kind])or'0'))
        events=[x for e in c['events']for x in e];assert all(len(e)==4 for e in c['events'])
        out.append('static const unsigned int pc_events_%d[]={%s};'%(i,','.join(word(v)for v in events)or'0'))
    out.append('static const PacketCase packet_cases[]={')
    for i,c in enumerate(cases):
        out.append('{{%s},{%s},%s,%d,%d,pc_initial_%d,pc_changes_%d,{%s},{%s},%d,%d,pc_pointer_initial_%d,pc_pointer_expected_%d,%d,pc_events_%d},'%(','.join(word(v)for v in c['p']),','.join(word(v)for v in c['args']+[0]*(6-len(c['args']))),word(c['result']),len(c['initial']),len(c['changes']),i,i,','.join(word(v)for v in c['global_initial']),','.join(word(v)for v in c['global_expected']),len(c['pointer_initial']),len(c['pointer_expected']),i,i,len(c['events'])*4,i))
    out+=['};','#endif',''];return '\n'.join(out)

def main():
    p=argparse.ArgumentParser();p.add_argument('--golden-header',type=Path);p.add_argument('--output',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    report=dict(fixtures=len(cases),instructions=sum(c['instructions']for c in cases),maximum_instructions=max(c['instructions']for c in cases),visited_selected=sum(len({pc for c in cases for pc in c['visited']if lo<=pc<hi})for lo,hi in SELECTED),input_sha256=hashlib.sha256(json.dumps(cases,sort_keys=True,separators=(',',':')).encode()).hexdigest(),cases=cases)
    if a.golden_header:a.golden_header.write_text(golden(cases),newline='\n')
    if a.output:a.output.write_text(json.dumps(report,indent=2)+'\n')
    print({k:v for k,v in report.items()if k!='cases'})
if __name__=='__main__':main()
