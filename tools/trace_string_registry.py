"""Strict initialized registry/string observations, not an EE/SDK emulator.
CPU/MMI decoder portions reused from the isolated UI draft snapshot; scoped
PSUBW lane subtraction added for actual strcmp. Core allocation/free/diagnostic
contracts are controlled. Optimized reads require explicit initialized lanes.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
ROOT=Path(__file__).resolve().parents[1]
MASK,U64,U128=0xFFFFFFFF,(1<<64)-1,(1<<128)-1
BASE,WORDS=0x20000,2048;END=BASE+WORDS*4
TABLE,ALT_TABLE,NEW_TABLE=0x20100,0x20180,0x20200
KEY,PATH,NEW_KEY,NEW_PATH=0x20401,0x20481,0x20600,0x20700
OUTPUT,HEAP=0x20800,0x20A00
OLD_KEYS,OLD_PATHS=(0x20B00,0x20B80,0x20C00),(0x20D00,0x20D80,0x20E00)
COUNT,CAP,TABLE_GLOBAL,HEAP_GLOBAL=0x3FD1D8,0x3FD1DC,0x3FD1E0,0x3FD204
TEXT_GLOBALS=(0x469A00,0x469A80,0x469B00);STACK=(0x7D000,0x81000)
GLOBALS=(COUNT,CAP,TABLE_GLOBAL,HEAP_GLOBAL)
SELECTED=((0x2AB790,0x2AB9A8),(0x2ABAE8,0x2ABDE8),(0x2ABDE8,0x2ABE18),(0x2ABEF8,0x2ABF60))
SUPPORT=((0x2AEC28,0x2AECD0),(0x2AEE40,0x2AEE5C),(0x295050,0x295080),
 (0x3934F8,0x3935A4),(0x393758,0x393888),(0x393A28,0x393B74),(0x393B74,0x393C8C),
 (0x3984D8,0x398554),(0x398628,0x39869C),(0x39CA58,0x39CA78))
RANGES=SELECTED+SUPPORT
READONLY=((0x446FE0,0x447010),(0x456118,0x456219))
signed=lambda x:(x&MASK)-0x100000000 if x&0x80000000 else x&MASK
signed64=lambda x:(x&U64)-(1<<64) if x&(1<<63) else x&U64
sx32=lambda x:signed(x)&U64
initial_word=lambda i:(0x8D4A0301^(i*0x10203))&MASK

class RegistryTrace(Trace):
    def __init__(self,original,parameters):
        super().__init__(original);self.p=parameters;self.lo=self.hi=0
        self.visited=set();self.events=[];self.branches=[];self.invocations=[0]*len(RANGES)
        self.allocations=0;self.stack_base=None;self.readonly_reads=set()
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
          BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or
          any(g<=a<a+n<=g+4 for g in GLOBALS) or
          any(g<=a<a+n<=g+128 for g in TEXT_GLOBALS)):
            raise ValueError('unowned or unaligned registry memory %#x/%d'%(a,n))
    def load(self,a,n):
        if any(lo<=a<a+n<=hi for lo,hi in READONLY):
            if a%n:raise ValueError('unaligned registry readonly')
            self.readonly_reads.add((a,n));off=a-0xFF000
            if off<0 or off+n>len(self.original):raise ValueError('registry readonly image bound')
            return int.from_bytes(self.original[off:off+n],'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized registry read')
        return super().load(a,n)
    def save(self,a,v,n):self.memory_check(a,n);super().save(a,v,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed registry instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('registry code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def text(self,a):
        result=bytearray()
        for i in range(96):
            x=self.load((a+i)&MASK,1)
            if not x:return bytes(result)
            result.append(x)
        raise ValueError('registry string observer bound')
    def put(self,a,b):
        for i,x in enumerate(b+b'\0'):self.save(a+i,x,1)
    def event(self,kind,*args):
        if len(self.events)>=80:raise ValueError('registry event bound')
        self.events.append([kind,*[v&MASK for v in args]])
    def execute(self,w,pc):
        if self.instruction_count>=30000:raise ValueError('registry instruction bound')
        self.instruction_count+=1;self.visited.add(pc)
        op,rs,rt,rd,sh,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
        im=w&65535;si=im-65536 if im&32768 else im;target=None;annul=False
        a,b=self.r[rs],self.r[rt]
        if w==0:pass
        elif op==0:
            if fn in (0x21,0x23) and sh==0:self.r[rd]=sx32(a+b if fn==0x21 else a-b)
            elif fn in (0x2D,0x2F) and sh==0:self.r[rd]=(a+b if fn==0x2D else a-b)&U64
            elif fn in (0,2,3) and rs==0:
                self.r[rd]=sx32((b<<sh) if fn==0 else (b&MASK)>>sh if fn==2 else signed(b)>>sh)
            elif fn in (4,6,7) and sh==0:
                n=a&31;self.r[rd]=sx32(b<<n if fn==4 else (b&MASK)>>n if fn==6 else signed(b)>>n)
            elif fn in (0x14,0x16,0x17) and sh==0:
                n=a&63;self.r[rd]=((b<<n) if fn==0x14 else (b&U64)>>n if fn==0x16 else signed64(b)>>n)&U64
            elif fn in (0x38,0x3A,0x3B,0x3C,0x3E,0x3F) and rs==0:
                n=sh+(32 if fn>=0x3C else 0)
                self.r[rd]=((b<<n) if fn in (0x38,0x3C) else (b&U64)>>n if fn in (0x3A,0x3E) else signed64(b)>>n)&U64
            elif fn in (0x24,0x25,0x26,0x27) and sh==0:
                self.r[rd]=(a&b if fn==0x24 else a|b if fn==0x25 else a^b if fn==0x26 else ~(a|b))&U64
            elif fn in (0x2A,0x2B) and sh==0:
                self.r[rd]=int(signed64(a)<signed64(b)) if fn==0x2A else int((a&U64)<(b&U64))
            elif fn in (0x0A,0x0B) and sh==0:
                if (b==0 if fn==0x0A else b!=0):self.r[rd]=a
            elif fn in (0x10,0x12) and rs==rt==sh==0:self.r[rd]=sx32(self.hi if fn==0x10 else self.lo)
            elif fn==0x1B and rd==sh==0:
                dividend,divisor=a&MASK,b&MASK
                if not divisor:raise ValueError('registry unsigned division by zero')
                self.lo,self.hi=dividend//divisor,dividend%divisor
            elif fn==0x18 and sh==0:
                product=signed(a)*signed(b);self.lo=product&MASK;self.hi=(product>>32)&MASK
                if rd:self.r[rd]=sx32(self.lo)
            elif fn==0x0D:raise ValueError('original registry BREAK trap')
            elif fn==8 and rt==rd==sh==0:target=a&MASK
            elif fn==9 and rt==sh==0 and rd==31:target=a&MASK;self.r[31]=pc+8
            else:raise ValueError('unsupported registry SPECIAL operands %#x at%#x'%(w,pc))
        elif op in (9,25):self.r[rt]=sx32(a+si) if op==9 else (a+si)&U64
        elif op in (10,11):self.r[rt]=int(signed64(a)<si) if op==10 else int((a&U64)<(si&U64))
        elif op in (12,13,14):self.r[rt]=(a&im if op==12 else a|im if op==13 else a^im)&U64
        elif op==15 and rs==0:self.r[rt]=sx32(im<<16)
        elif op in (2,3):
            if op==3:self.r[31]=pc+8
            target=((pc+4)&0xF0000000)|((w&0x3FFFFFF)<<2)
        elif op==1 and rt in (0,1,2,3):
            take=signed64(a)<0 if rt in (0,2) else signed64(a)>=0
            if take:target=(pc+4+4*si)&MASK
            elif rt in (2,3):annul=True
        elif op in (4,5,6,7,20,21,22,23):
            if op in (6,7,22,23) and rt:raise ValueError('reserved registry zero branch')
            take=(a==b if op in (4,20) else a!=b if op in (5,21) else signed64(a)<=0 if op in (6,22) else signed64(a)>0)
            if take:target=(pc+4+4*si)&MASK
            elif op>=20:annul=True
        elif op in (30,32,33,35,36,37,55):
            n={30:16,32:1,33:2,35:4,36:1,37:2,55:8}[op];v=self.load((a+si)&MASK,n)
            if op in (32,33):
                if v&(1<<(8*n-1)):v-=1<<(8*n)
                v&=U64
            elif op==35:v=sx32(v)
            self.r[rt]=v
        elif op in (31,40,41,43,63):self.save((a+si)&MASK,b,{31:16,40:1,41:2,43:4,63:8}[op])
        elif op in (26,27,44,45):
            address=(a+si)&MASK;base=address&~7;k=address&7
            if op==26:
                count=k+1;v=int.from_bytes(bytes(self.load(base+i,1) for i in range(count)),'little')
                shift=(8-count)*8;mask=((1<<(count*8))-1)<<shift
                self.r[rt]=((b&~mask)|(v<<shift))&U64
            elif op==27:
                count=8-k;v=int.from_bytes(bytes(self.load(address+i,1) for i in range(count)),'little')
                mask=(1<<(count*8))-1;self.r[rt]=((b&~mask)|v)&U64
            elif op==44:
                count=k+1;v=(b&U64)>>((8-count)*8)
                for i in range(count):self.save(base+i,v>>(8*i),1)
            else:
                count=8-k
                for i in range(count):self.save(address+i,b>>(8*i),1)
        elif op==28:
            if fn==9 and sh==0x0E: # PCPYLD
                self.r[rd]=((a&U64)<<64)|(b&U64)
            elif fn==0x29 and sh==0x0E: # PCPYUD: high rs -> low, high rt -> high
                self.r[rd]=((b>>64)<<64)|(a>>64)
            elif fn==8 and sh in (1,9): # PSUBW/PSUBB
                n=4 if sh==1 else 1;mask=(1<<(8*n))-1
                self.r[rd]=sum((((a>>(8*n*i)&mask)-(b>>(8*n*i)&mask))&mask)<<(8*n*i) for i in range(16//n))
            elif (fn,sh) in ((9,0x12),(0x29,0x13)):
                self.r[rd]=(a&b if sh==0x12 else ~(a|b))&U128
            else:raise ValueError('unsupported registry MMI operands %#x at%#x'%(w,pc))
        else:raise ValueError('unsupported registry opcode %#x at%#x'%(w,pc))
        self.r[0]=0;return target,annul

    def external(self,target):
        if target==0x2ADF60:
            heap,n,align=(self.r[r]&MASK for r in (4,5,6))
            if heap!=HEAP or align!=3:raise ValueError('unreviewed registry core allocation')
            self.allocations+=1;self.event(1,heap,n,align)
            failure=bool(self.p['failure']&(1<<(self.allocations-1)))
            if n>=80:result=NEW_TABLE
            else:result=NEW_KEY
            # The destination order follows actual allocation calls, not contents.
            if n<80:
                self.small_allocations=getattr(self,'small_allocations',0)+1
                result=NEW_KEY if self.small_allocations==1 else NEW_PATH
            if self.p['mutation']&1 and n>=80:
                self.save(COUNT,2,4);self.save(CAP,7,4);self.save(TABLE_GLOBAL,ALT_TABLE,4)
            self.r[2]=0 if failure else result
        elif target==0x2AE158:
            self.event(2,self.r[4],self.load(COUNT,4),self.load(CAP,4))
            if self.p['mutation']&2:self.save(CAP,3,4)
        elif target==0x394F68:
            if self.r[4]&MASK!=0x447238:raise ValueError('unreviewed registry allocation diagnostic')
            self.event(3,self.r[5],self.r[6],self.r[7]);self.r[2]=0
        else:raise ValueError('unreviewed registry external')
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>5:raise ValueError('unreviewed registry entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo
        if entry==0x3934F8:
            d,s,n=(self.r[r]&MASK for r in (4,5,6))
            if n>64 or n and not(d+n<=s or s+n<=d):raise ValueError('registry copy observer domain')
        while True:
            if not lo<=pc<hi:raise ValueError('registry transfer outside full body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('registry annul taken')
                    pc+=8;continue
                delay=self.fetch(pc+4)
                if pc+4>=hi or is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('registry control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('registry actual JR31/stop')
                    if index<4 and pc!=hi-8:raise ValueError('registry actual terminal JR31')
                    return
                if w>>26==3:
                    continuation=self.r[31]&MASK
                    if target in [a for a,b in SUPPORT]:self.run(target,continuation,depth+1)
                    else:self.external(target)
                    pc=continuation
                elif w>>26==2 and target in [a for a,b in SUPPORT]:
                    self.run(target,stop,depth+1);return
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('registry unexpected transfer')
                pc+=4

KEYS=('routine','key','path','input','count','capacity','table','failure','mutation','output_alias','setter_alias')
def fixture(original,routine=0,key='Tag',path='Folder',input='TAG:File/One',count=1,capacity=10,table=1,failure=0,mutation=0,output_alias=0,setter_alias=0):
    p=dict(zip(KEYS,(routine,key,path,input,count,capacity,table,failure,mutation,output_alias,setter_alias)))
    if type(routine) is not int or routine not in range(4) or table not in (0,1) or not 0<=failure<16 or not 0<=mutation<4 or output_alias not in (0,1) or setter_alias not in (0,1,2):raise ValueError('registry fixture domain')
    if not 0<=count<=MASK or not 0<=capacity<=MASK:raise ValueError('registry fixture word')
    for s in (key,path,input):
        if not isinstance(s,str) or len(s)>64 or any(ord(c)>127 or ord(c)==0 for c in s):raise ValueError('registry supported byte string domain')
    if routine==1 and (not input or input.startswith(':') and ':' not in input[1:]):raise ValueError('registry prior-stack/leading-colon domain')
    t=RegistryTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xA5030201,4)
    for g in TEXT_GLOBALS:
        for i in range(128):t.save(g+i,0xA5,1)
    for a in (KEY-1,PATH-1,NEW_KEY,NEW_PATH,OUTPUT):
        for i in range(96):t.save(a+i,0xA5,1)
    t.put(KEY,key.encode());t.put(PATH,path.encode());t.put(OUTPUT,b'untouched-output')
    t.save(KEY-1,ord(':'),1);t.save(PATH-1,ord('/'),1)
    t.put(TEXT_GLOBALS[0],b'BASE/');t.put(TEXT_GLOBALS[1],b'PREFIX/');t.put(TEXT_GLOBALS[2],b'SUFFIX/')
    for i,(k,v) in enumerate(zip(OLD_KEYS,OLD_PATHS)):
        t.put(k,('tag:' if i==0 else 'other%d:'%i).encode());t.put(v,(path+'/' if i==0 else 'alt/').encode())
        for tab in (TABLE,ALT_TABLE):t.save(tab+i*8,k,4);t.save(tab+i*8+4,v,4)
    t.save(COUNT,count,4);t.save(CAP,capacity,4);t.save(TABLE_GLOBAL,TABLE if table else 0,4);t.save(HEAP_GLOBAL,HEAP,4)
    t.save(HEAP+0x18,0,4);t.save(HEAP+0x24,0xFFFFFFFF,4);t.save(HEAP+0x28,0,4)
    if routine==1:t.put(KEY,input.encode())
    arena_initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    texts_initial=[[t.load(g+i,1) for i in range(128)] for g in TEXT_GLOBALS]
    globals_initial=[t.load(g,4) for g in GLOBALS]
    out=KEY if output_alias else OUTPUT
    args=(KEY,PATH) if routine==0 else (KEY,out) if routine==1 else (0 if setter_alias==2 else TEXT_GLOBALS[0] if setter_alias else KEY,) if routine==2 else ()
    for r,a in enumerate(args,4):t.r[r]=a
    t.r[29]=0x80000;t.r[31]=RETURN
    preserved=[0xCAFE0000000000000000000000000000+i for i in range(16,24)]+[0x123456789ABCDEF0123456789ABCDEF0]
    for r,v in zip(list(range(16,24))+[30],preserved):t.r[r]=v
    t.run(SELECTED[routine][0]);assert t.r[29]==0x80000
    assert [t.r[r] for r in list(range(16,24))+[30]]==preserved
    expected=[t.load(BASE+4*i,4) for i in range(WORDS)]
    return dict(p,result=t.r[2]&MASK if routine==0 else 0,
      initial=[[i,v] for i,v in enumerate(arena_initial) if v!=initial_word(i)],
      changes=[[i,v] for i,v in enumerate(expected) if v!=arena_initial[i]],
      globals_initial=globals_initial,globals_expected=[t.load(g,4) for g in GLOBALS],
      texts_initial=texts_initial,texts_expected=[[t.load(g+i,1) for i in range(128)] for g in TEXT_GLOBALS],
      events=t.events,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,invocations=t.invocations,
      readonly_reads=sorted(t.readonly_reads))

def fixtures(raw,rejected=None):
    result=[]
    for key in ('Tag','TAG:','Mixed_Name',''):
        for path in ('Folder','X:/Dir','Path\\',''):
            for fail in (0,1,2,3):result.append(fixture(raw,key=key,path=path,failure=fail))
    for table in (0,1):
        for count,cap in ((0,10),(1,1),(2,2),(0,0),(0x80000000,10)):
            for fail in (0,1,2,3,4,7):
                for mutation in (0,1,2,3):
                    try:result.append(fixture(raw,count=count,capacity=cap,table=table,failure=fail,mutation=mutation))
                    except ValueError as e:
                        if not any(x in str(e) for x in ('unowned','copy observer')):raise
                        if rejected is not None:rejected.append(dict(routine=0,count=count,capacity=cap,table=table,failure=fail,mutation=mutation,reason=str(e)))
    for inp in ('TAG:File/One','Other1:Two','Missing:File','C:Dir/File','host0:File/Dir','path/noalias',':TAG:Tail','a','/ROOT\\File'):
        for count in (0,1,2,3,0x80000000):
            for table in (0,1):
                for alias in (0,1):result.append(fixture(raw,1,input=inp,count=count,table=table,output_alias=alias))
    for alias in (0,1,2):
        for key in ('Base','Mixed/Path',''):result.append(fixture(raw,2,key=key,setter_alias=alias))
    for table in (0,1):
        for fail in (0,1):
            for count in (0,3,0xFFFFFFFF):result.append(fixture(raw,3,table=table,failure=fail,count=count))
    return result

def golden(cases):
    mi=max(len(c['initial']) for c in cases);mc=max(len(c['changes']) for c in cases);me=max(len(c['events']) for c in cases)
    lines=['/* Authored initialized registry observations; no original code arrays. */',
      '#define STRING_REGISTRY_WORDS %du'%WORDS,'#define STRING_REGISTRY_INITIAL %du'%mi,'#define STRING_REGISTRY_CHANGES %du'%mc,'#define STRING_REGISTRY_EVENTS %du'%me,
      'struct RegistryPair{u32 index,value;};',
      'struct RegistryGolden{u32 routine,failure,mutation,output_alias,setter_alias,result,initial_count,change_count,event_count;struct RegistryPair initial[STRING_REGISTRY_INITIAL],changes[STRING_REGISTRY_CHANGES];u32 globals_initial[4],globals_expected[4];u8 texts_initial[3][128],texts_expected[3][128];u32 events[STRING_REGISTRY_EVENTS][4];};',
      'static const struct RegistryGolden registry_golden[]={']
    for c in cases:
        pair=lambda values:'{'+','.join('{%du,0x%08Xu}'%(i,v) for i,v in values)+'}'
        rows=lambda values:'{'+','.join('{'+','.join('0x%Xu'%v for v in row)+'}' for row in values)+'}'
        scalar=[c[k] for k in ('routine','failure','mutation','output_alias','setter_alias','result')]+[len(c['initial']),len(c['changes']),len(c['events'])]
        lines.append('{'+','.join('%du'%v for v in scalar)+','+pair(c['initial'])+','+pair(c['changes'])+',{'+','.join('0x%08Xu'%v for v in c['globals_initial'])+'},{'+','.join('0x%08Xu'%v for v in c['globals_expected'])+'},'+rows(c['texts_initial'])+','+rows(c['texts_expected'])+','+rows(c['events'])+'},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/string_registry/trace.json');p.add_argument('--header',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');rejected=[];cases=fixtures(raw,rejected)
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases),input_sha256=hashlib.sha256(json.dumps([{k:c[k] for k in KEYS} for c in cases],sort_keys=True).encode()).hexdigest(),coverage=[len({pc for c in cases for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],cases=cases,excluded_domain_cases=rejected)
    if a.header:a.header.write_text(golden(cases),newline='\n')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n');print(report['fixtures'],report['instructions'],report['coverage'])
if __name__=='__main__':main()
