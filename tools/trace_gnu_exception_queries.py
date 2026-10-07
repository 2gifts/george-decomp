"""Strict initialized observations of two GNU queries and the genuine getter.

This executes only the three actual bounded original bodies. The mutable
context provider is a controlled typed callback, not an exception/OS model.
Integer/control decoding is delegated unchanged to RegistryTrace.
"""
import argparse,hashlib,itertools,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace
ROOT=Path(__file__).resolve().parents[1]
ENTRIES=(0x3735B0,0x373B28);SIZES=(36,52);HELPER=(0x370468,0x370490)
GLOBAL=0x4021B4;CONTEXTS=(0x20200,0x20220);RECORDS=(0x20300,0x20340)
PROVIDER,ALTERNATE=0x0F000004,0x0F000008;STACK=(0x70000,128)
MASK=0xFFFFFFFF
REGIONS=((GLOBAL,4),*((a,16) for a in CONTEXTS),*((a,48) for a in RECORDS),STACK)
CODE=((ENTRIES[0],ENTRIES[0]+SIZES[0]),(ENTRIES[1],ENTRIES[1]+SIZES[1]),HELPER)
def record_token(i):return 0 if i==2 else RECORDS[i]

def parameters():
    rows=[]
    for query,pair,context,new,mutation,caught in itertools.product(range(2),((0,1),(1,0),(2,2),(0,2),(2,1)),range(2),range(3),range(8),range(4)):
        final=new if mutation&1 else pair[context]
        if query==0 and final==2:continue
        rows.append(dict(query=query,initial0=pair[0],initial1=pair[1],context=context,new_record=new,mutation=mutation,caught=caught))
    return rows

class QueryTrace(Trace):
    def __init__(self,original,p):
        super().__init__(original);self.p=p;self.visited=set();self.events=[];self.branches=[];self.accesses=[];self.initializing=True
    def check(self,a,n):
        if n not in (4,8) or a%n or not any(lo<=a<a+n<=lo+size for lo,size in REGIONS):raise ValueError('query memory ownership/alignment')
    def load(self,a,n):
        self.check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized query memory')
        value=Trace.load(self,a,n)
        if not self.initializing:self.accesses.append(['read',a,n,value])
        return value
    def save(self,a,v,n):
        self.check(a,n);Trace.save(self,a,v,n)
        if not self.initializing:self.accesses.append(['write',a,n,v&((1<<(8*n))-1)])
    def fetch(self,pc):
        if pc&3 or not any(lo<=pc<hi for lo,hi in CODE):raise ValueError('unreviewed query instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('query original bounds')
        return struct.unpack_from('<I',self.original,off)[0]
    def execute(self,w,pc):
        if self.instruction_count>=96:raise ValueError('query pre-state instruction budget')
        op=w>>26;rs=w>>21&31;rt=w>>16&31;rd=w>>11&31;sh=w>>6&31;fn=w&63
        allowed=(w==0 or op in (9,11,35,55,63,3,4) or op==15 and rs==0 or op==0 and (fn==0x2D and sh==0 or w==0x03E00008 or fn==9 and rt==sh==0 and rd==31))
        if not allowed:raise ValueError('unsupported query instruction operands')
        return RegistryTrace.execute(self,w,pc)
    def external(self,target):
        if self.instruction_count>=96:raise ValueError('query pre-callback budget')
        if target!=PROVIDER or len(self.events):raise ValueError('unreviewed query provider/repetition')
        p=self.p;ctx=CONTEXTS[p['context']]
        before=self.load(ctx+8,4)
        if p['mutation']&1:self.save(ctx+8,record_token(p['new_record']),4)
        record=self.load(ctx+8,4)
        if p['mutation']&2 and record:self.save(record+20,1-self.load(record+20,4),4)
        if p['mutation']&4:
            if record:self.save(record+8,0x21030,4)
            self.save(GLOBAL,ALTERNATE,4)
        self.events.append([before,record,ctx,p['mutation']]);self.r[2]=ctx;self.r[0]=0
    def run(self):
        pc=ENTRIES[self.p['query']]
        while True:
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:raise ValueError('unobserved query likely annul')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('query control in delay')
                if target==RETURN:
                    if w!=0x03E00008 or pc!=ENTRIES[self.p['query']]+SIZES[self.p['query']]-8:raise ValueError('query terminal return')
                    return
                if target in (PROVIDER,ALTERNATE):
                    if pc!=0x370478 or w!=0x0040F809:raise ValueError('query provider call PC/word')
                    self.external(target);pc=self.r[31]&MASK
                else:pc=pc+8 if target is None else target
            else:
                if target is not None or annul:raise ValueError('query noncontrol transfer')
                pc+=4

def initialize(t,p):
    if p not in parameters():raise ValueError('query initialized source contract')
    for lo,size in REGIONS:
        for a in range(lo,lo+size,4):t.save(a,0xA5C30000+(a&65535),4)
    t.save(GLOBAL,PROVIDER,4)
    for i,ctx in enumerate(CONTEXTS):t.save(ctx+8,record_token(p['initial%d'%i]),4)
    for i,a in enumerate(RECORDS):
        t.save(a,0,4);t.save(a+4,0x00200010+i*0x00010001,4)
        t.save(a+8,0x21000+16*i,4);t.save(a+12,0x21100+16*i,4);t.save(a+16,0,4)
        t.save(a+20,p['caught']>>i&1,4);t.save(a+24,RECORDS[1-i],4)
        t.save(a+32,0x12340+i,8);t.save(a+40,0x21200+16*i,4)
    t.r[29]=STACK[0]+STACK[1]-16;t.r[31]=RETURN;t.r[0]=0;t.initializing=False

def fixture(original,p):
    t=QueryTrace(original,p);initialize(t,p)
    initial={a:t.memory[a] for a in t.memory};t.run();result=t.r[2]&MASK
    final_record=t.load(CONTEXTS[p['context']]+8,4)
    if p['query']==0:
        assert final_record and result==final_record+8
        pointed=t.load(result,4)
    else:
        assert result==int(bool(final_record) and t.load(final_record+20,4)==0);pointed=0
    assert len(t.events)==1 and t.r[0]==0 and t.r[29]==STACK[0]+STACK[1]-16
    changed={a for a,v in t.memory.items() if v!=initial[a]}
    permitted=set(range(STACK[0],STACK[0]+STACK[1]))
    for a in (CONTEXTS[p['context']]+8,GLOBAL):permitted.update(range(a,a+4))
    if final_record:
        for a in (final_record+8,final_record+20):permitted.update(range(a,a+4))
    assert changed<=permitted
    output=[result,pointed,len(t.events),p['context'],t.load(GLOBAL,4),*(t.load(a+8,4) for a in CONTEXTS)]
    for a in RECORDS:output.extend(t.load(a+off,4) for off in (4,8,12,16,20,24,32,40))
    return dict(parameters=p,expected=output,event=t.events[0],instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,accesses=t.accesses)

def golden_header(rows):
    keys=('query','initial0','initial1','context','new_record','mutation','caught')
    lines=['/* Synthetic initialized logical fields; no original code/data arrays. */','struct QueryGolden { unsigned p[7], expected[23], event[4]; };','static const QueryGolden query_golden[] = {']
    for row in rows:lines.append('{{%s},{%s},{%s}},'%(','.join(str(row['parameters'][k])+'u' for k in keys),','.join('0x%08Xu'%x for x in row['expected']),','.join('0x%08Xu'%x for x in row['event'])))
    return '\n'.join(lines+['};',''])

def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68');parser.add_argument('--output',type=Path,default=ROOT/'build/gnu_exception_queries/trace.json');parser.add_argument('--golden-header',type=Path);args=parser.parse_args()
    _,raw=validated_elf(args.elf);ps=parameters();rows=[fixture(raw,p) for p in ps]
    visited=sorted({pc for row in rows for pc in row['visited']});assert visited==sorted(pc for lo,hi in CODE for pc in range(lo,hi,4))
    out=dict(fixtures=len(rows),input_sha256=hashlib.sha256(json.dumps(ps,sort_keys=True).encode()).hexdigest(),total_instructions=sum(x['instructions'] for x in rows),maximum_instructions=max(x['instructions'] for x in rows),visited=visited,observations=rows,limits='Initialized live slot/context objects; old query only live nonnull current record, predicate null or semantic bool0/1. Controlled provider can switch context/info, mutate value/caught and provider word. Unchanged RegistryTrace subset, no exception/OS/thread/hardware/upper128 claim.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(out,indent=2)+'\n',newline='\n')
    if args.golden_header:args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(rows),newline='\n')
    print('GNU queries',len(rows),'fixtures',out['total_instructions'],'original instructions')
if __name__=='__main__':main()
