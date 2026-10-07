"""Bounded initialized archive lifecycle observations, not SDK emulation.

All selected bodies and listed string/map/heap-wrapper originals execute.
Only the deeper allocator, free, SDK, module and scheduler effects are controls.
PCPYH follows EE Core Instruction Set Manual v6 page189; no shared decoder edit.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,U64,U128,sx32,signed,signed64
ROOT=Path(__file__).resolve().parents[1]
BASE,WORDS=0x20000,8192;END=BASE+WORDS*4
CTX,ALTCTX,PATH,KEYOUT=0x20000,0x20080,0x20101,0x20200
MAP,ALTMAP,BUCKETS,ALTBUCKETS,NODE,ALTNODE,PAYLOAD=0x20400,0x20440,0x20500,0x20540,0x20600,0x20640,0x20700
BUFFER,BUFFER2,HEAP,MAPALLOC,ENTRY,NODEALLOC=0x21000,0x21800,0x22000,0x23000,0x25000,0x25800
GLOBALS=(0x3FD23C,0x3FD240,0x3FD244,0x3FD200,0x3FD204)
SLOTS,OVERRIDES,STACK=0x469BD0,0x469B80,(0x7B000,0x81000)
SELECTED=((0x2B15C8,0x2B1900),(0x2B1900,0x2B1AE4),(0x2B1D30,0x2B1E88),
 (0x2B1E88,0x2B1EDC),(0x2B0860,0x2B08F8),(0x2A7E68,0x2A7F1C),
 (0x2A7F58,0x2A7FC4),(0x2A7FC8,0x2A8070))
SUPPORT=((0x2AE830,0x2AE8D0),(0x2AEAF0,0x2AEB5C),(0x2AEB60,0x2AEC28),
 (0x2AEC28,0x2AECD0),(0x2AEE40,0x2AEE5C),(0x295050,0x295080),
 (0x29C648,0x29C6C0),(0x2A7CD0,0x2A7DBC),(0x3934F8,0x3935A4),
 (0x3936A0,0x393758),(0x393758,0x393888),(0x393B74,0x393C8C),
 (0x393C90,0x393E44),(0x393E48,0x394010),(0x394010,0x3941D8),
 (0x3984D8,0x398554),(0x39CA58,0x39CA78))
RANGES=SELECTED+SUPPORT
READONLY=((0x445650,0x445A50),(0x4471F8,0x447250),(0x447398,0x447440),(0x456118,0x456219))
initial_word=lambda i:(0xA14D0301^(i*0x10203))&MASK

def directory(records):
    data=bytearray(2048);offset=0
    for name,extent,size,flags in records:
        n=len(name);stride=(33+n+3)&~3
        if offset+stride+2>len(data):raise ValueError('authored sector bound')
        struct.pack_into('<H',data,offset,stride)
        struct.pack_into('<I',data,offset+2,extent);struct.pack_into('<I',data,offset+10,size)
        data[offset+25]=flags;data[offset+32]=n;data[offset+33:offset+33+n]=name
        offset+=stride
    return bytes(data)

def sectors(scenario,prefix):
    root=[(b'\0',10,4096 if scenario==4 else 2048,2)]
    if scenario==1:root += [(b'A.TXT;1',30,123,0)]
    if scenario==2:root += [(b'\1',0,0,0),(b'',31,7,0),(b'XY',32,8,0)]
    if scenario==3:root += [(b'DIR1',20,2048,2),(b'B.TXT;1',33,9,0),(b'DIR2',21,2048,2)]
    pvd=bytearray(2048);pvd[0]=2 if scenario==5 else 1
    pvd[1:6]=b'WRONG' if scenario==6 else b'CD001'
    pvd[40:40+len(prefix)]=prefix.encode();
    if scenario==7:pvd[40]=ord('Z')
    struct.pack_into('<H',pvd,128,1024 if scenario==8 else 2048)
    struct.pack_into('<I',pvd,140,9)
    return {16:bytes(pvd),9:directory([(b'\0',10,2048,2)]),10:directory(root),
      11:directory([(b'C2.TXT;1',34,11,0)]),20:directory([(b'\0',20,2048,2),(b'C.DAT;1',35,12,0)]),
      21:directory([(b'\0',21,2048,2),(b'D.DAT;1',36,13,0)])}

class LifecycleTrace(RegistryTrace):
    def __init__(self,original,parameters):
        super().__init__(original,parameters)
        self.invocations=[0]*len(RANGES);self.read_attempts={};self.sync_attempts=0
        self.allocations=0;self.small_allocations=0;self.buffers=0;self.frees=0
        self.pages=sectors(parameters['scenario'],parameters['prefix'])
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
          BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or
          any(g<=a<a+n<=g+4 for g in GLOBALS) or
          SLOTS<=a<a+n<=SLOTS+560 or OVERRIDES<=a<a+n<=OVERRIDES+80):
            raise ValueError('unowned or unaligned lifecycle memory %#x/%d'%(a,n))
    def load(self,a,n):
        if any(lo<=a<a+n<=hi for lo,hi in READONLY):
            if a%n:raise ValueError('unaligned lifecycle readonly')
            self.readonly_reads.add((a,n));off=a-0xFF000
            if off<0 or off+n>len(self.original):raise ValueError('lifecycle readonly image bound')
            return int.from_bytes(self.original[off:off+n],'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized lifecycle read %#x/%d'%(a,n))
        return Trace.load(self,a,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed lifecycle instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('lifecycle code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def execute(self,w,pc):
        if w>>26==28 and w&63==0x29 and w>>6&31==0x1B:
            if w>>21&31:raise ValueError('reserved PCPYH rs')
            if self.instruction_count>=250000:raise ValueError('lifecycle instruction bound')
            self.instruction_count+=1;self.visited.add(pc)
            value=self.r[w>>16&31];low=value&65535;high=value>>64&65535
            self.r[w>>11&31]=sum(low<<(16*i) for i in range(4))|sum(high<<(64+16*i) for i in range(4))
            self.r[0]=0;return None,False
        return self._execute_published(w,pc)
    def _execute_published(self,w,pc):
        if self.instruction_count>=250000:raise ValueError('registry instruction bound')
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

    def event(self,kind,a=0,b=0,c=0,d=0):
        if len(self.events)>=100:raise ValueError('lifecycle event bound')
        self.events.append([kind,a&MASK,b&MASK,c&MASK,d&MASK,
          self.load(CTX+0x20,4),self.load(CTX+0x24,4),self.load(CTX+0x50,4),self.load(CTX+0x54,4),
          self.load(GLOBALS[3],4),self.load(GLOBALS[0],4)])
    def external(self,target):
        a,b,c,d=(self.r[i]&MASK for i in range(4,8));result=0
        if target==0x2ADF60:
            if a!=HEAP or c not in (3,7):raise ValueError('lifecycle core allocator lanes')
            self.allocations+=1;self.event(1,a,b,c)
            if b==88:result=CTX
            elif b==0x3C000:result=HEAP
            elif b==8108 or self.p['routine']==6:result=MAPALLOC
            elif b==2048:
                result=BUFFER+2048*self.buffers;self.buffers+=1
                if self.buffers>4:raise ValueError('lifecycle buffer contract bound')
            elif b==12:
                result=(ENTRY if self.small_allocations%2==0 else NODEALLOC)+(self.small_allocations//2)*32
                self.small_allocations+=1
            else:raise ValueError('lifecycle unreviewed allocation size %d'%b)
            if self.p['failure']&(1<<(self.allocations-1)):result=0
            if self.p['mutation']&1 and b==12:self.save(CTX+0x50,ALTMAP,4)
        elif target==0x2AE158:
            self.event(2,a);self.frees+=1
            if self.p['mutation']&2 and self.frees==1:
                self.save(MAP+12,ALTBUCKETS,4);self.save(MAP+8,7,4)
            if self.p['mutation']&4:self.save(CTX+0x50,ALTMAP,4)
        elif target==0x394F68:
            if a!=0x447238:raise ValueError('lifecycle diagnostic literal')
            self.event(3,b,c,d)
        elif target==0x377820:
            if a!=1:raise ValueError('lifecycle sync mode')
            self.event(4,a);self.sync_attempts+=1
            result=1 if self.p['retry'] and self.sync_attempts%2 else 0
        elif target==0x378038:
            if a:raise ValueError('lifecycle readiness mode')
            self.event(5,a)
        elif target==0x378258:
            if b!=1 or d!=CTX+0x28 or a not in self.pages:raise ValueError('lifecycle read lanes/sector')
            self.event(6,a,b,c,d);attempt=self.read_attempts.get(a,0);self.read_attempts[a]=attempt+1
            for i,v in enumerate(self.pages[a]):self.save(c+i,v,1)
            loader=any(x==0x2B1900 for x in getattr(self,'active',[])) and self.active[-1]==0x2B1900
            result=int(bool(self.p['retry']) and attempt==0) if loader else int(not self.p['retry'] or attempt>0)
            if self.p['mutation']&8 and a==16:self.save(CTX+0x54,HEAP,4)
        elif target==0x295D30:
            if a not in (0x4473F8,0x447410,0x447428):raise ValueError('lifecycle module literal')
            self.event(7,a)
        elif target==0x2C3BC8:
            if a:raise ValueError('lifecycle control0');
            self.event(8,a)
        elif target==0x2C8C38:
            if (a,b)!=(3,7):raise ValueError('lifecycle control3/7')
            self.event(9,a,b)
        elif target==0x2C35B0:
            if (a,b,c,d)!=(400,16,self.p['arg5'],self.p['arg6']):raise ValueError('lifecycle scheduler captured lanes')
            self.event(10,a,b,c,d)
        else:raise ValueError('unreviewed lifecycle external %#x'%target)
        for register in range(3,16):self.r[register]=sx32(0xCAFEBABE)
        self.r[2]=sx32(result)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>20:raise ValueError('unreviewed lifecycle entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo
        if not hasattr(self,'active'):self.active=[]
        self.active.append(entry)
        try:
            while True:
                if not lo<=pc<hi:raise ValueError('lifecycle transfer outside full body')
                w=self.fetch(pc);target,annul=self.execute(w,pc)
                if is_control_transfer(w):
                    self.branches.append([pc,target is not None])
                    if annul:
                        if target is not None:raise ValueError('lifecycle annul taken')
                        pc+=8;continue
                    if pc+4>=hi:raise ValueError('lifecycle delay outside body')
                    delay=self.fetch(pc+4)
                    if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('lifecycle control in delay')
                    if w==0x03E00008:
                        if target!=stop:raise ValueError('lifecycle actual JR31/stop')
                        if index<8 and pc!=hi-8:raise ValueError('lifecycle terminal JR31')
                        return
                    if w>>26==3:
                        continuation=self.r[31]&MASK
                        if target in [a for a,b in RANGES]:self.run(target,continuation,depth+1)
                        else:self.external(target)
                        pc=continuation
                    elif w>>26==2 and target in [a for a,b in RANGES]:self.run(target,stop,depth+1);return
                    else:pc=target if target is not None else pc+8
                else:
                    if target is not None or annul:raise ValueError('lifecycle unexpected transfer')
                    pc+=4
        finally:self.active.pop()

KEYS=('routine','scenario','failure','mutation','retry','depth','path','prefix','bucket_count','map_count','key_alias','key_output','count','arg5','arg6')
def fixture(original,routine=0,scenario=1,failure=0,mutation=0,retry=0,depth=0,path='ROOT\\',prefix='VOL',bucket_count=4,map_count=1,key_alias=0,key_output=1,count=MASK,arg5=0x12345678,arg6=0x87654321):
    values=(routine,scenario,failure,mutation,retry,depth,path,prefix,bucket_count,map_count,key_alias,key_output,count,arg5,arg6)
    p=dict(zip(KEYS,values))
    if routine not in range(8) or scenario not in range(9) or mutation not in range(16) or retry not in (0,1) or key_alias not in range(6):raise ValueError('lifecycle fixture selector')
    if any(type(x) is not int or not 0<=x<=MASK for x in (failure,depth,bucket_count,map_count,count,arg5,arg6)):raise ValueError('lifecycle fixture word')
    if bucket_count>8 and routine!=6:raise ValueError('lifecycle explicit readable bucket domain')
    if any(not isinstance(s,str) or len(s)>24 or any(ord(c)==0 or ord(c)>127 for c in s) for s in (path,prefix)):raise ValueError('lifecycle initialized string domain')
    t=LifecycleTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xA5030201,4)
    for i,g in enumerate(GLOBALS):t.save(g,(count,arg5,ALTCTX,0,HEAP)[i],4)
    for i in range(140):t.save(SLOTS+4*i,(0x59170001+i*0x10203)&MASK,4)
    for i in range(20):t.save(OVERRIDES+4*i,HEAP,4)
    for h in (HEAP,):
        t.save(h+0x18,0,4);t.save(h+0x24,MASK,4);t.save(h+0x28,0,4)
    for ctx in (CTX,ALTCTX):
        t.put(ctx,prefix.encode());t.save(ctx+0x24,2048,4);t.save(ctx+0x50,MAP if ctx==CTX else ALTMAP,4);t.save(ctx+0x54,HEAP,4)
    t.put(PATH,(prefix if routine==2 else path).encode());t.save(PATH-1,ord('Q'),1)
    for m,b,n in ((MAP,BUCKETS,NODE),(ALTMAP,ALTBUCKETS,ALTNODE)):
        t.save(m,0,4);t.save(m+4,bucket_count,4);t.save(m+8,map_count,4);t.save(m+12,b,4)
        for i in range(8):t.save(b+i*4,0,4)
        t.save(b+(1 if bucket_count>1 else 0)*4,n,4)
        t.save(n,0,4);t.save(n+4,0x12345678,4);t.save(n+8,PAYLOAD,4)
    out=0 if not key_output else (KEYOUT,NODE,NODE+8,MAP+12,MAP+8,NODE+4)[key_alias]
    if key_alias==3:t.save(NODE+4,ALTBUCKETS,4)
    t.save(KEYOUT,0xFFEEDDCC,4)
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)];gi=[t.load(g,4) for g in GLOBALS]
    si=[t.load(SLOTS+4*i,4) for i in range(140)];oi=[t.load(OVERRIDES+4*i,4) for i in range(20)]
    args=((CTX,10,PATH,depth),(CTX,),(PATH,arg5,arg6),(CTX,),(arg5,arg6),(MAP,out),(bucket_count,),(MAP,))[routine]
    for r,v in enumerate(args,4):t.r[r]=sx32(v)
    t.r[29]=0x80000;t.r[31]=RETURN
    preserved=[0xCAFE0000000000000000000000000000+i for i in range(16,24)]+[0x123456789ABCDEF0123456789ABCDEF0]
    for r,v in zip(list(range(16,24))+[30],preserved):t.r[r]=v
    t.run(SELECTED[routine][0]);assert t.r[29]==0x80000
    assert [t.r[r] for r in list(range(16,24))+[30]]==preserved
    expected=[t.load(BASE+4*i,4) for i in range(WORDS)]
    return dict(p,args=list(args),result=t.r[2]&MASK if routine in (0,1,2,5,6) else 0,
      initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],changes=[[i,v] for i,v in enumerate(expected) if v!=initial[i]],
      globals_initial=gi,globals_expected=[t.load(g,4) for g in GLOBALS],slots_initial=si,slots_expected=[t.load(SLOTS+4*i,4) for i in range(140)],
      overrides_initial=oi,overrides_expected=[t.load(OVERRIDES+4*i,4) for i in range(20)],events=t.events,
      instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,invocations=t.invocations,readonly_reads=sorted(t.readonly_reads))

def fixtures(raw):
    cases=[]
    for routine in (0,1):
        for scenario in range(9):
            for retry in (0,1):
                for mutation in (0,1,4,8):cases.append(fixture(raw,routine,scenario,mutation=mutation,retry=retry))
    for depth in (0,1,2,MASK):
        for path in ('','ROOT','ROOT\\','A\\B','A\\B\\'):
            for failure in (0,1):cases.append(fixture(raw,0,depth=depth,path=path,failure=failure))
    for routine in (2,4):
        for scenario in (0,1,3,5,7,8):
            for retry in (0,1):cases.append(fixture(raw,routine,scenario,retry=retry,prefix=''))
        for count in (0,1,0x80000000,MASK):cases.append(fixture(raw,routine,failure=1,count=count))
    for routine in (3,5,7):
        for bucket_count in (0,1,4,8):
            for map_count in (0,1,7,MASK):
                for mutation in (0,2,4):cases.append(fixture(raw,routine,bucket_count=bucket_count,map_count=map_count,mutation=mutation))
    for alias in range(6):
        for mutation in (0,2):cases.append(fixture(raw,5,key_alias=alias,mutation=mutation))
    for count in (0,1,4,2023,0x40000000):
        for failure in (0,1):cases.append(fixture(raw,6,bucket_count=count,failure=failure))
    return cases

def golden(cases):
    initial=[];changes=[];events=[];rows=[]
    for c in cases:
        ii,ci,ei=len(initial),len(changes),len(events)
        initial.extend(c['initial']);changes.extend(c['changes']);events.extend(c['events'])
        fields=[c[k] for k in ('routine','scenario','failure','mutation','retry','arg5','arg6','result')]
        fields += [ii,len(c['initial']),ci,len(c['changes']),ei,len(c['events'])]
        array=lambda values:'{'+','.join('0x%08Xu'%v for v in values)+'}'
        rows.append('{'+','.join('%du'%v for v in fields)+','+array(c['args']+[0]*(4-len(c['args'])))+','+
          ','.join(array(c[k]) for k in ('globals_initial','globals_expected','slots_initial','slots_expected','overrides_initial','overrides_expected'))+
          ','+json.dumps(c['prefix'])+'},')
    lines=['/* Authored initialized fixtures, no original instruction/asset arrays. */',
      '#define LIFECYCLE_WORDS %du'%WORDS,'struct LifecyclePair{u32 index,value;};',
      'struct LifecycleGolden{u32 routine,scenario,failure,mutation,retry,arg5,arg6,result,initial_index,initial_count,change_index,change_count,event_index,event_count;u32 args[4],globals_initial[5],globals_expected[5],slots_initial[140],slots_expected[140],overrides_initial[20],overrides_expected[20];char prefix[25];};']
    for name,values in (('initial',initial),('changes',changes)):
        lines+=['static const struct LifecyclePair lifecycle_'+name+'[]={']+['{%du,0x%08Xu},'%(i,v) for i,v in values]+['};']
    lines+=['static const u32 lifecycle_events[][11]={']+['{'+','.join('0x%08Xu'%v for v in row)+'},' for row in events]+['};',
      'static const struct LifecycleGolden lifecycle_golden[]={']+rows+['};','']
    return '\n'.join(lines)

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/file_archive_lifecycle/trace.json');p.add_argument('--header',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases),
      input_sha256=hashlib.sha256(json.dumps([{k:c[k] for k in KEYS} for c in cases],sort_keys=True).encode()).hexdigest(),
      coverage=[len({pc for c in cases for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],cases=cases)
    if a.header:a.header.write_text(golden(cases),newline='\n')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n');print(report['fixtures'],report['instructions'],report['maximum'],report['coverage'])
if __name__=='__main__':main()
