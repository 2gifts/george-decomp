"""Initialized low-word array observations with actual published heap wrappers
and memcpy instructions. Core heap/free and sort operations are controlled
caller-observation hooks, not allocator/OS or whole qsort emulation. No original
instruction arrays are exported. Malformed/overlapping-copy domains are excluded.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_buffer_records import BufferTrace
from trace_geometry import RETURN,is_control_transfer

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2AAF50,0x2AAF88),(0x2AAF98,0x2AAFD8),(0x2AAFD8,0x2AAFE0),
        (0x2AAFE0,0x2AB020),(0x2AB020,0x2AB068),(0x2AB068,0x2AB10C),
        (0x2AB110,0x2AB204),(0x2AB330,0x2AB3C4),(0x2AB3C8,0x2AB3F8),
        (0x3934F8,0x3935A4),(0x2AEB60,0x2AEC28),(0x2AEC28,0x2AECD0),(0x2AEE40,0x2AEE5C))
BASE,WORDS=0x20000,1024;END=BASE+WORDS*4
ARRAY=BASE+0x100;HEADER=BASE+0x180;OLD=BASE+0x400;ALT=BASE+0x500
NEW=BASE+0x600;ELEMENT=BASE+0x900;HEAP=BASE+0xA00
GLOBAL=0x3FD204;COMPARE=0xF1230000;STACK=(0x7F000,0x81000);MASK=0xFFFFFFFF
POINTER_CELLS=(ARRAY+12,HEADER+12,HEAP+0x18)
KEYS=('routine','index','stride','capacity','used','data','success','mutation','alias')
signed=lambda v:(v&MASK)-0x100000000 if v&0x80000000 else v&MASK
initial_word=lambda i:(0x5A3C0001^(i*0x10203))&MASK

class ArrayTrace(BufferTrace):
    def __init__(self,original,parameters):
        super().__init__(original);self.parameters=parameters;self.lo=0;self.copy_calls=0
        self.invocations=[0]*len(RANGES)

    def memory_check(self,address,size):
        if size not in (1,4,8,16) or address%size or not (
            BASE<=address and address+size<=END or STACK[0]<=address and address+size<=STACK[1]
            or address==GLOBAL and size==4):raise ValueError('unowned/unaligned array memory')

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed array instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('array instruction outside original')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        op,rs,rt,rd,sh,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
        si=(w&65535)-(65536 if w&32768 else 0)
        extra=op in (1,11,15,20,21) or op==0 and fn in (4,0x12,0x18,0x23)
        if not extra:return super().execute(w,pc)
        if self.instruction_count>=1600:raise ValueError('array instruction bound')
        self.instruction_count+=1;self.visited.add(pc);target=None;annul=False
        if op==15:
            if rs:raise ValueError('reserved array LUI')
            self.r[rt]=(w&65535)<<16
        elif op==11:self.r[rt]=int((self.r[rs]&MASK)<(si&MASK))
        elif op in (20,21):
            take=(self.r[rs]&MASK)==(self.r[rt]&MASK)
            if op==21:take=not take
            if take:target=(pc+4+4*si)&MASK
            else:annul=True
        elif op==1:
            if rt not in (0,1,2,3):raise ValueError('unsupported array REGIMM')
            take=signed(self.r[rs])<0 if rt in (0,2) else signed(self.r[rs])>=0
            if take:target=(pc+4+4*si)&MASK
            elif rt in (2,3):annul=True
        elif fn==4:
            if sh:raise ValueError('reserved array SLLV')
            self.r[rd]=(self.r[rt]<<(self.r[rs]&31))&MASK
        elif fn==0x12:
            if rs or rt or sh:raise ValueError('reserved array MFLO')
            self.r[rd]=self.lo
        elif fn==0x18:
            if sh:raise ValueError('reserved array MULT')
            self.lo=(signed(self.r[rs])*signed(self.r[rt]))&MASK
            if rd:self.r[rd]=self.lo
        elif fn==0x23:
            if sh:raise ValueError('reserved array SUBU')
            self.r[rd]=(self.r[rs]-self.r[rt])&MASK
        self.r[0]=0;return target,annul

    def event(self,kind,a=0,b=0,c=0):
        if len(self.events)>=48:raise ValueError('array event bound')
        self.events.extend((kind,a&MASK,b&MASK,c&MASK))

    def mutation(self,phase):
        mode=self.parameters['mutation'];routine=self.parameters['routine']
        if phase==1 and mode&1 and routine in (5,6):
            self.save(ARRAY,8,4);self.save(ARRAY+8,2,4);self.save(ARRAY+12,ALT,4)
            self.save(ARRAY+4,99,4)
        if phase==2 and mode&2 and routine in (5,6) and self.copy_calls==1:
            self.save(ARRAY+12,ALT,4);self.save(ARRAY+8,1,4);self.save(ARRAY,4,4)
        if phase==3 and mode&4 and routine in (3,4,5,6):
            self.save(ARRAY+8,2,4);self.save(ARRAY,4,4)
        if phase==2 and mode&8 and routine==6 and self.copy_calls==2:
            self.save(ARRAY+8,5,4)

    def external(self,target):
        if target==0x2ADF60:
            heap,n,align=(self.r[i]&MASK for i in (4,5,6))
            if heap!=HEAP or align not in (3,4):raise ValueError('unreviewed allocator caller')
            self.event(1,heap,n,align);self.mutation(1)
            self.r[2]=(HEADER if self.parameters['routine']==1 else NEW) if self.parameters['success'] else 0
        elif target==0x2AE158:
            p=self.r[4]&MASK;self.event(2,p,self.load(ARRAY+8,4),self.load(ARRAY+4,4));self.mutation(3)
        elif target==0x394F68:
            if self.r[4]&MASK!=0x447238:raise ValueError('unreviewed allocation diagnostic')
            self.event(4,self.r[5],self.r[6],self.r[7]);self.r[2]=0
        elif target==0x396788:
            data,n,size,compare=(self.r[i]&MASK for i in (4,5,6,7))
            if n>12 or size!=4 or compare!=COMPARE:raise ValueError('sort outside controlled domain')
            self.event(5,data,n,size)
            values=sorted((self.load(data+4*i,4) for i in range(n)),key=signed)
            for i,v in enumerate(values):self.save(data+4*i,v,4)
            if n>1 and self.parameters['mutation']&16:self.save(ARRAY+8,31,4)
            self.r[2]=0
        else:raise ValueError('unreviewed array external call')

    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,p in enumerate(RANGES) if p[0]==entry),None)
        if index is None or depth>3:raise ValueError('unreviewed array entry/depth')
        a,b=RANGES[index];self.invocations[index]+=1;pc=a
        if index==9:
            d,s,n=(self.r[i]&MASK for i in (4,5,6))
            if n>64 or n and not (d+n<=s or s+n<=d):raise ValueError('copy outside bounded nonoverlap domain')
            self.copy_calls+=1;self.event(3,d,s,n)
        while True:
            if not a<=pc<b:raise ValueError('array transfer outside complete body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('annul with taken array branch')
                    pc+=8;continue
                delay=self.fetch(pc+4)
                if pc+4>=b or is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('array control in delay')
                if w==0x03E00008:
                    legal={0x2AAF58,0x2AAF78,0x2AAF80} if index==0 else {b-8}
                    if pc not in legal or target!=stop:raise ValueError('array actual JR31/stop required')
                    if index==9:self.mutation(2)
                    return
                if w>>26==3:
                    continuation=self.r[31]&MASK
                    if target in (p[0] for p in RANGES[9:]):self.run(target,continuation,depth+1)
                    else:self.external(target)
                    pc=continuation
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('unexpected array transfer')
                pc+=4

def fixture(original,routine=0,index=1,stride=4,capacity=4,used=3,data=1,success=1,mutation=0,alias=0):
    parameters=dict(zip(KEYS,(routine,index,stride,capacity,used,data,success,mutation,alias)))
    if any(type(v) is not int for v in parameters.values()) or routine not in range(9) or data not in (0,1) or success not in (0,1) or not 0<=mutation<32 or alias not in (0,1,2):raise ValueError('invalid array fixture domain')
    if any(not 0<=v<=MASK for v in (index,stride,capacity,used)):raise ValueError('invalid array word')
    if alias and (routine!=6 or used!=0 or stride!=4 or capacity!=4 or mutation):raise ValueError('invalid array alias domain')
    t=ArrayTrace(original,parameters)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    t.save(ARRAY,stride,4);t.save(ARRAY+4,capacity,4);t.save(ARRAY+8,used,4)
    t.save(ARRAY+12,(ARRAY+(4 if alias==1 else 8)) if alias else OLD if data else 0,4)
    t.save(HEADER+12,0,4);t.save(HEAP+0x18,0,4);t.save(HEAP+0x24,0xFFFFFFFF,4);t.save(HEAP+0x28,0,4)
    t.save(GLOBAL,HEAP,4);t.save(ELEMENT,3,4)
    if routine==8:
        if used>12 or stride!=4 or not data:raise ValueError('invalid sort fixture')
        for i in range(used):t.save(OLD+4*i,((used-i)%5)-2,4)
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    args=(ARRAY,index)
    if routine==1:args=(stride,)
    if routine in (2,3,4):args=(ARRAY,)
    if routine==6:args=(ARRAY,ELEMENT)
    if routine==8:args=(ARRAY,COMPARE)
    for r,v in enumerate(args,4):t.r[r]=v
    t.r[29]=0x80000;t.r[31]=RETURN
    preserved=tuple(0xA5A50000000000000000000000000000+i for i in range(16,21))
    for r,v in enumerate(preserved,16):t.r[r]=v
    t.run(RANGES[routine][0]);assert t.r[29]==0x80000
    assert tuple(t.r[16:21])==preserved
    expected=[t.load(BASE+4*i,4) for i in range(WORDS)]
    result=t.r[2]&MASK if routine in (0,1,5,6,7) else 0
    return dict(parameters,result=result,initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],
                changes=[[i,v] for i,v in enumerate(expected) if v!=initial[i]],events=t.events,
                instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,invocations=t.invocations)

def fixtures(raw):
    cases=[]
    for used in (0,1,3,5,0xFFFFFFFF,0x80000000):
        for index in (0,1,4,0xFFFFFFFF,0x80000000):
            for stride in (0,1,4,0x40000000):cases.append(fixture(raw,0,index,stride,used=used))
    for stride in (0,1,4,0xFFFFFFFF):
        for success in (0,1):cases.append(fixture(raw,1,stride=stride,success=success))
    for routine in (2,3,4):
        for used in (0,3,0xFFFFFFFF,0x80000000):
            for data in (0,1):
                for mutation in (0,4):cases.append(fixture(raw,routine,used=used,data=data,mutation=mutation))
    for additional in (0,1,2,0xFFFFFFFF,0x80000000):
        for data in (0,1):
            for success in (0,1):
                for mutation in (0,1,2,4,7):cases.append(fixture(raw,5,additional,used=2,data=data,success=success,mutation=mutation))
    for capacity,used in ((4,0),(4,3),(4,4),(0,0),(0xFFFFFFFF,0),(0x80000000,0),(0xFFFFFFFF,0xFFFFFFFF)):
        for data in (0,1):
            for success in (0,1):
                for mutation in (0,1,2,4,7,15):
                    # NULL data without allocation cannot support a nonzero copy.
                    if not data and signed(used)<signed(capacity):continue
                    if used&0x80000000:continue
                    try:cases.append(fixture(raw,6,capacity=capacity,used=used,data=data,success=success,mutation=mutation))
                    except ValueError as e:
                        if 'unowned' not in str(e):raise
    for used in (0,1,2,4):
        for index in (0,1,3,0xFFFFFFFF):
            for stride in (0,4):
                if stride and not (used and (index==used-1 or index<used)):continue
                cases.append(fixture(raw,7,index,stride,used=used))
    for used in (0,1,2,3,6,12):
        for mutation in (0,16):cases.append(fixture(raw,8,used=used,mutation=mutation))
    for alias in (1,2):cases.append(fixture(raw,6,used=0,alias=alias))
    # Full aligned quadword and remainder paths in genuine memcpy, bounded by
    # independent old/new storage and modeled 64-byte readable capacities.
    for used in (4,5,8,9,10,12,16):
        cases.append(fixture(raw,5,1,capacity=used,used=used))
        cases.append(fixture(raw,6,capacity=used,used=used))
    # Wrapped allocation request and count decrement, without oversized copies.
    cases.extend((fixture(raw,5,1,stride=0,capacity=MASK,used=MASK),fixture(raw,6,stride=0,capacity=0x80000000,used=MASK),fixture(raw,7,MASK,stride=0,used=0)))
    return cases

def golden(cases):
    mi=max(len(c['initial']) for c in cases);mc=max(len(c['changes']) for c in cases)
    lines=['/* Authored initialized integer observations, no original code. */','#define ARRAY_RECORD_WORDS %du'%WORDS,
      '#define ARRAY_RECORD_INITIAL %du'%mi,'#define ARRAY_RECORD_CHANGES %du'%mc,
      'struct ArrayPair{u32 index,value;};',
      'struct ArrayGolden{u32 routine,index,stride,capacity,used,data,success,mutation,alias,result,initial_count,change_count,event_count;struct ArrayPair initial[ARRAY_RECORD_INITIAL],changes[ARRAY_RECORD_CHANGES];u32 events[48];};',
      'static const u32 array_pointer_cells[]={'+','.join('%du'%((p-BASE)//4) for p in POINTER_CELLS)+'};',
      'static const struct ArrayGolden array_golden[]={']
    for c in cases:
        pair=lambda values:'{'+','.join('{%du,0x%08Xu}'%(i,v) for i,v in values)+'}'
        scalars=[c[k] for k in KEYS]+[c['result'],len(c['initial']),len(c['changes']),len(c['events'])]
        lines.append('{'+','.join('0x%08Xu'%v for v in scalars)+','+pair(c['initial'])+','+pair(c['changes'])+',{'+','.join('0x%08Xu'%v for v in c['events'])+'}},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/array_records/trace.json');p.add_argument('--header',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases),
      input_sha256=hashlib.sha256(json.dumps([{k:c[k] for k in KEYS} for c in cases],sort_keys=True).encode()).hexdigest(),
      coverage=[len({pc for c in cases for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],cases=cases)
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n')
    if a.header:a.header.write_text(golden(cases),newline='\n')
    print(report['fixtures'],'array fixtures;',report['instructions'],'instructions;',report['coverage'],'coverage')

if __name__=='__main__':main()
