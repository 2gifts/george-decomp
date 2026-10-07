"""Initialized completion observations through actual bounded original bodies.
Reuses the published strict integer/MMI decoder. Unknown constructor/insertion,
core allocation and formatter engine are controlled supporting contracts.
Actual wrappers, search, strings, append, traversal and atexit execute; no
capacity/class/OS/EE-overread identity or helper recovery is awarded.
"""
import argparse,hashlib,json,struct,math
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer,word
from trace_string_registry import RegistryTrace,sx32
ROOT=Path(__file__).resolve().parents[1]
MASK=0xFFFFFFFF
BASE,WORDS=0x20000,3072;END=BASE+4*WORDS
UI,ALT=0x21100,0x20044
CAND,RESOLVER,RECORD,SLOTS,OLD=0x21800,0x21A00,0x21F00,0x20300,0x20200
MANAGERS=(0x20400,0x20440);LINES=(0x20500,0x20520)
CHILDREN=(0x20600,0x20620,0x20640,0x20660)
TEXTS=(0x20800,0x20900,0x20A00,0x20B00)
HEAP,REENT=0x20E00,0x22A00
REG,UI_GLOBAL,RANGE,HEAP_GLOBAL,REENT_GLOBAL,MAP_GLOBAL=0x3F2C44,0x3F9408,0x46A0F0,0x3FD204,0x405694,0x481760
DIRECT,DBUCKETS,DNODES,GMAP,GBUCKETS,GNODES,VALUE=0x22000,0x22080,(0x22100,0x22120),0x22200,0x22240,(0x22300,0x22320),0x22400
GLOBALS=((REG,4),(UI_GLOBAL,4),(RANGE,12),(HEAP_GLOBAL,4),(REENT_GLOBAL,4),(MAP_GLOBAL,4))
STACK=(0x7D000,0x81000)
SELECTED=((0x2162D0,0x2163E4),(0x217598,0x2176CC))
SUPPORT=((0x100AA8,0x100AB8),(0x100C30,0x100CCC),(0x2AEE60,0x2AEF08),
 (0x2BF550,0x2BF580),(0x2BF580,0x2BF5E0),(0x3952C8,0x395348),
 (0x2A4DB8,0x2A4EF0),(0x295050,0x295080),(0x393B74,0x393C8C),(0x393E48,0x394010),
 (0x396260,0x3962FC),(0x2CDA20,0x2CDAB0),(0x2CDDB0,0x2CDDE4),
 (0x2CDD88,0x2CDDB0),(0x2A8130,0x2A81D0))
RANGES=SELECTED+SUPPORT
READONLY=((0x4208A0,0x4208B0),(0x43A690,0x43A692),(0x447F48,0x447F63))
initial_word=lambda i:(0xA5870301^(i*0x10103))&MASK

class CompletionTrace(RegistryTrace):
    def __init__(self,original,p):
        Trace.__init__(self,original);self.p=p;self.lo=self.hi=0
        self.visited=set();self.events=[];self.branches=[];self.allocations=0
        self.invocations=[0]*len(RANGES);self.readonly_reads=set();self.append_count=0
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
          BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or
          any(g<=a<a+n<=g+s for g,s in GLOBALS)):
            raise ValueError('unowned or unaligned completion memory %#x/%d'%(a,n))
    def load(self,a,n):
        if any(lo<=a<a+n<=hi for lo,hi in READONLY):
            if a%n:raise ValueError('unaligned completion readonly')
            self.readonly_reads.add((a,n));off=a-0xFF000
            if off<0 or off+n>len(self.original):raise ValueError('completion readonly image bound')
            return int.from_bytes(self.original[off:off+n],'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized completion read')
        return Trace.load(self,a,n)
    def save(self,a,v,n):self.memory_check(a,n);Trace.save(self,a,v,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed completion instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('completion code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def text(self,a):
        result=bytearray()
        for i in range(160):
            v=self.load((a+i)&MASK,1)
            if not v:return bytes(result)
            result.append(v)
        raise ValueError('completion string observer bound')
    def put(self,a,data):
        for i,b in enumerate(data+b'\0'):self.save(a+i,b,1)
    def event(self,k,*a):
        if len(self.events)>=80 or len(a)!=3:raise ValueError('completion event shape/bound')
        self.events.append([k,*[v&MASK for v in a]])
    def execute(self,w,pc):
        if w>>26==57: # Actual sprintf's four finite, unused FP vararg saves.
            self.instruction_count+=1;self.visited.add(pc)
            if self.instruction_count>30000:raise ValueError('completion instruction bound')
            rs,rt=w>>21&31,w>>16&31;im=w&65535;im=im-65536 if im&32768 else im
            if not math.isfinite(self.f[rt]):raise ValueError('nonfinite unused formatter vararg')
            self.save((self.r[rs]+im)&MASK,word(self.f[rt]),4);self.r[0]=0
            return None,False
        return RegistryTrace.execute(self,w,pc)
    def external(self,target):
        a,b,c=(self.r[r]&MASK for r in (4,5,6))
        if target==0x2ADF60:
            if a!=HEAP or c!=4 or b not in (0x41C,12):raise ValueError('unreviewed completion core allocation')
            self.allocations+=1;self.event(1,a,b,c)
            if self.allocations>2:raise ValueError('completion allocation count')
            self.r[2]=RESOLVER if b==0x41C else RECORD
            if self.p['mutation']&1 and b==12:self.save(REG,RESOLVER+0x100,4)
        elif target==0x2BF748:
            if a!=RESOLVER+4:raise ValueError('unreviewed nested resolver initialization')
            self.event(2,a,0,0);self.r[2]=a
        elif target==0x1007E0:
            if a!=RANGE:raise ValueError('unreviewed insertion range')
            value=self.load(c,4);self.event(3,a,b,value)
            end=self.load(RANGE+4,4)
            if not SLOTS<=end<SLOTS+32:raise ValueError('insertion controlled domain')
            self.save(end,value,4);self.save(RANGE+4,end+4,4)
        elif target==0x398F98:
            if self.load(a+12,2)!=0x208 or self.load(a+8,4)!=0x7FFFFFFF or self.load(a+20,4)!=0x7FFFFFFF:raise ValueError('actual formatter wrapper layout')
            fmt=self.text(b)
            if fmt!=self.original[0x447F48-0xFF000:0x447F63-0xFF000-1] or fmt.count(b'%08x')!=1:raise ValueError('whole completion format contract')
            key=self.load(c,8)&MASK;out=self.load(a,4)
            self.event(4,out,key,self.load(REG,4))
            rendered=fmt.replace(b'%08x',('%08x'%key).encode())
            for i,v in enumerate(rendered):self.save(out+i,v,1)
            self.save(a,out+len(rendered),4);self.r[2]=len(rendered)
            if self.p['mutation']&2:self.save(UI_GLOBAL,ALT,4)
            if self.p['mutation']&4:self.save(self.load(REG,4)+24,15,4)
            if self.p['mutation']&8:
                self.save(DNODES[0] if self.p['routine']==2 else GNODES[0],0,4)
        else:raise ValueError('unreviewed completion external %#x'%target)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>9:raise ValueError('unreviewed completion entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo
        if entry==0x2A4DB8:
            self.append_count+=1;self.event(5,self.r[4],self.r[5],self.load(UI_GLOBAL,4))
        while True:
            if not lo<=pc<hi:raise ValueError('completion transfer outside full body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('completion annul taken')
                    pc+=8;continue
                if pc+4>=hi:raise ValueError('completion missing delay')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('completion control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('completion JR31/stop')
                    if index<2 and pc!=hi-8:raise ValueError('completion selected terminal JR31')
                    return
                if w>>26==3 or w&63==9 and w>>26==0:
                    continuation=self.r[31]&MASK
                    if target in [a for a,b in RANGES]:self.run(target,continuation,depth+1)
                    else:self.external(target)
                    pc=continuation
                elif w>>26==2 and target in [a for a,b in RANGES]:self.run(target,stop,depth+1);return
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('completion unexpected transfer')
                pc+=4

KEYS=('routine','key','input','candidate','previous','count','common','registry','range_mode','mutation','alias','line_limit')
def fixture(original,routine=1,key=0x89ABCDEF,input='',candidate='alpha',previous='alpine',count=0,common=6,registry=0,range_mode=0,mutation=0,alias=0,line_limit=40):
    p=dict(zip(KEYS,(routine,key,input,candidate,previous,count,common,registry,range_mode,mutation,alias,line_limit)))
    if type(routine) is not int or routine not in (0,1,2,3) or registry not in (0,1) or range_mode not in range(4) or not 0<=mutation<16 or alias not in range(5) or not 1<=line_limit<=80:raise ValueError('completion fixture domain')
    for s in (input,candidate,previous):
        if not isinstance(s,str) or len(s)>80 or any(ord(c)>127 or ord(c)==0 for c in s):raise ValueError('completion initialized ASCII contract')
    if not all(type(v) is int and 0<=v<=MASK for v in (key,count,common)):raise ValueError('completion fixture word')
    t=CompletionTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xA5030201,4)
    for g,n in GLOBALS:
        for i in range(0,n,4):t.save(g+i,0,4)
    t.save(REG,RESOLVER if registry else 0,4);t.save(UI_GLOBAL,UI,4)
    t.save(HEAP_GLOBAL,HEAP,4);t.save(REENT_GLOBAL,REENT,4);t.save(MAP_GLOBAL,GMAP,4)
    t.save(HEAP+24,0,4);t.save(HEAP+36,0x10000,4);t.save(HEAP+40,0,4)
    t.save(REENT+0x148,0,4);t.save(REENT+0x14C,0,4);t.save(REENT+0x150,0,4)
    for u,m in zip((UI,ALT),MANAGERS):
        t.save(u,m,4);t.save(u+12,count,4);t.save(u+0x210,common,4)
        t.put(u+16,previous.encode());t.put(u+0x110,input.encode());t.put(u+0x214,b'>')
    t.put(CAND,candidate.encode());t.save(RESOLVER+24,0,4);t.save(RESOLVER+0x100+24,0,4)
    for i,(m,lines) in enumerate(zip(MANAGERS,LINES)):
        t.save(m+4,2,4);t.save(m+12,lines,4);t.save(m+16,CHILDREN[2*i],4)
        for j in range(2):
            child=CHILDREN[2*i+j];t.save(lines+4*j,child,4)
            t.save(child,line_limit,4);t.save(child+4,0,4);t.save(child+8,TEXTS[2*i+j],4);t.put(TEXTS[2*i+j],b'')
    t.save(OLD,5 if range_mode!=2 else 9,4)
    t.save(SLOTS,OLD,4)
    begin=SLOTS;end=SLOTS+(4 if range_mode==2 else 0);cap=SLOTS+16
    if range_mode==1:cap=end
    if range_mode==3:begin=end=RANGE+4;cap=RANGE+12
    t.save(RANGE,begin,4);t.save(RANGE+4,end,4);t.save(RANGE+8,cap,4)
    t.save(DIRECT+12,DBUCKETS,4)
    for i in range(17):t.save(DBUCKETS+4*i,DNODES[0] if i==0 else 0,4)
    t.save(GMAP+4,1,4);t.save(GMAP+12,GBUCKETS,4);t.save(GBUCKETS,GNODES[0],4)
    for nodes in (DNODES,GNODES):
        for i,node in enumerate(nodes):
            t.save(node,nodes[1] if i==0 else 0,4);t.save(node+4,key if i==0 else 0x12345678,4)
            t.save(node+8,0 if nodes==DNODES else VALUE,4)
    args=[key,0,0] if routine==0 else [DIRECT,0x2162D0,0] if routine==2 else [0x2162D0,0] if routine==3 else [UI,CAND]
    if routine==1 and alias==1:args[1]=UI+16
    if routine==1 and alias==2:args[1]=UI+0x110
    if routine==1 and alias==3:
        # Actual manager character/NUL stores change only the low pointer lanes.
        t.put(UI+0x214,b'D');t.save(CHILDREN[0]+8,UI_GLOBAL,4)
    if routine==1 and alias==4:
        # Destination precedes this aliased source; both actual and generic
        # byte copies observe the same initialized terminated text.
        args[1]=UI+17;t.put(UI+0x110,b'')
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    globals_initial=[t.load(g+i,4) for g,n in GLOBALS for i in range(0,n,4)]
    for r,a in enumerate(args,4):t.r[r]=a
    t.r[29]=0x80000;t.r[31]=RETURN
    preserved=[0xCAFE0000000000000000000000000000+i for i in range(16,24)]+[0x123456789ABCDEF0123456789ABCDEF0]
    for r,v in zip(list(range(16,24))+[30],preserved):t.r[r]=v
    t.run(SELECTED[routine][0] if routine<2 else 0x2CDA20 if routine==2 else 0x2CDDB0)
    if t.r[29]!=0x80000 or any(t.r[r]!=v for r,v in zip(list(range(16,24))+[30],preserved)):raise ValueError('completion callee preservation')
    final=[t.load(BASE+4*i,4) for i in range(WORDS)]
    globals_final=[t.load(g+i,4) for g,n in GLOBALS for i in range(0,n,4)]
    # Only declared typed cells may hold translated guest pointers.
    cells=[u for u in (UI,ALT)]+[HEAP+24,REENT+0x148,REENT+0x14C]
    cells+=[m+x for m in MANAGERS for x in (12,16)]+[a+4*j for a in LINES for j in range(2)]
    cells+=[a+8 for a in CHILDREN]+[SLOTS+4*j for j in range(4)]
    cells+=[RECORD+4,RECORD+8]+[REENT+0x154]
    cells+=[DIRECT+12,GMAP+12,GBUCKETS]+[DBUCKETS+4*i for i in range(17)]+list(DNODES)+list(GNODES)+[a+8 for a in GNODES]
    # SLOTS[1..3], descriptor fields and exit callback are pointers only when written.
    cells=[a for a in cells if any(v==0 or BASE<=v<END or v in (0x4208A0,0x2BD340,UI_GLOBAL) for v in (initial[(a-BASE)//4],final[(a-BASE)//4]))]
    return dict(parameters=p,args=args,initial=initial,final=final,globals_initial=globals_initial,globals_final=globals_final,pointer_cells=sorted(set(cells)),events=t.events,invocations=t.invocations,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,readonly_reads=sorted(t.readonly_reads))

def cases():
    result=[]
    for count in (0,1,0xFFFFFFFF,0x7FFFFFFF):
        for inp,cand,prev in (('', 'alpha','alpine'),('a','alpha','alpine'),('z','alpha','alpine'),('alpha','alpha','alpha'),('abc','ab','xyz'),('','x',''),('','','anything')):
            for common in (0,1,4,80):result.append(dict(count=count,input=inp,candidate=cand,previous=prev,common=common))
    for alias in (1,2,3,4):
        for count in (0,1,2):result.append(dict(alias=alias,count=count,input='',candidate='alpha',previous='alpine'))
    for limit in (1,3,40):result.append(dict(count=1,line_limit=limit,input='',candidate='zebra',previous='alpha'))
    for reg in (0,1):
        for mode in range(4):
            if reg and mode:continue
            for mut in range(8):
                for key in (0,0x89ABCDEF,0xFFFFFFFF):result.append(dict(routine=0,key=key,registry=reg,range_mode=mode,mutation=mut))
    for routine in (2,3):
        for registry in (0,1):
            for mutation in (0,2,8,10,12):result.append(dict(routine=routine,registry=registry,mutation=mutation,key=0x89ABCDEF))
    return result

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path,default=ROOT/'build/buffer_completion/trace.json');ap.add_argument('--header',type=Path,default=ROOT/'tests/native/buffer_completion_golden.h');a=ap.parse_args()
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');rows=[fixture(original,**p) for p in cases()]
    packet=dict(fixtures=rows,total_fixtures=len(rows),total_original_instructions=sum(r['instructions'] for r in rows),maximum_instructions=max(r['instructions'] for r in rows),input_sha256=hashlib.sha256(json.dumps([r['parameters'] for r in rows],sort_keys=True).encode()).hexdigest(),coverage=sorted(set(v for r in rows for v in r['visited'])),limits='Initialized authored ASCII arenas, complete executed original intervals; controlled unknown allocation/constructor/insertion/vfprintf engine, no EE bulk read/native stack identity or capacities.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(packet,indent=2)+'\n')
    out=['/* Authored initialized completion observations; no original instruction arrays. */','#define COMPLETION_WORDS %d'%WORDS,'struct CompletionGolden {unsigned routine,key,mutation,alias,args[3],initial_count,initial[800],change_count,changes[800],globals_initial[8],globals_final[8],pointer_count,pointer_cells[64],length_calls,event_count,events[320];};','static const struct CompletionGolden completion_golden[]={']
    arr=lambda x:'{'+','.join('0x%08Xu'%v for v in x)+'}'
    for r in rows:
        p=r['parameters'];assert len(r['pointer_cells'])<=64 and sum(v!=initial_word(i) for i,v in enumerate(r['initial']))*2<=800 and sum(a!=b for a,b in zip(r['initial'],r['final']))*2<=800
        out.append('{%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s},'%(str(p['routine'])+'u',str(p['key'])+'u',str(p['mutation'])+'u',str(p['alias'])+'u',arr(r['args']+[0]*(3-len(r['args']))),sum(v!=initial_word(i) for i,v in enumerate(r['initial'])),arr(sum(([i,v] for i,v in enumerate(r['initial']) if v!=initial_word(i)),[])),sum(a!=b for a,b in zip(r['initial'],r['final'])),arr(sum(([i,v] for i,v in enumerate(r['final']) if v!=r['initial'][i]),[])),arr(r['globals_initial']),arr(r['globals_final']),len(r['pointer_cells']),arr(r['pointer_cells']),r['invocations'][9],len(r['events'])*4,arr(sum(r['events'],[]))))
    out.append('};');a.header.write_text('\n'.join(out)+'\n',newline='\n')
    print('%d fixtures / %d original instructions / maximum %d'%(len(rows),packet['total_original_instructions'],packet['maximum_instructions']))
if __name__=='__main__':main()
