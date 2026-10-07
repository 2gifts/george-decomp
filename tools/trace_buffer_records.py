"""Bounded initialized buffer/navigation observations from original instructions.

Only low-word integer, delayed-control and unsigned byte-copy paths are modeled.
The original length helper runs in full. The original optimized strncpy runs
only its alignment gate and byte/padding path; aligned bulk overreads, EE upper
registers, traps, thread behavior and malformed/capacity contracts are excluded.
Exported arrays contain authored inputs/results/events, never original code.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2A5230,0x2A528C),(0x2A5290,0x2A5314),(0x2A5398,0x2A53C0),
        (0x2A53D0,0x2A53E8),(0x2A53E8,0x2A53F4),(0x2A53F8,0x2A5430),
        (0x295050,0x295080),(0x394010,0x3941D8))
BUFFER,WORDS=0x20000,512
END=BUFFER+WORDS*4
OWNER=BUFFER+0x100;CHILD=BUFFER+0x180;ALT=BUFFER+0x1A0
TABLE=BUFFER+0x240;HISTORY=BUFFER+0x280;DEST=BUFFER+0x400;NEW_TEXT=BUFFER+0x501
TEXTS=tuple(BUFFER+0x301+i*0x40 for i in range(4))
STACK=(0x7F000,0x81000);EMPTY_TOKEN=0xF0000001;MASK=0xFFFFFFFF
POINTER_CELLS=tuple(sorted({OWNER+0x0C,OWNER+0x10,OWNER+0x28,CHILD+8,ALT+8,
    *(TABLE+4*i for i in range(-2,5)),*(HISTORY+4*i for i in range(-2,5))}))

def signed(x):return (x&MASK)-0x100000000 if x&0x80000000 else x&MASK
def initial_word(index):return (0x5A5A0001^(index*0x10203))&MASK

class BufferTrace(Trace):
    def __init__(self,original):
        super().__init__(original);self.visited=set();self.branches=[];self.events=[];self.invocations=[0]*8

    def memory_check(self,address,size):
        if size not in (1,4,8,16) or address%size or not (
                BUFFER<=address<address+size<=END or STACK[0]<=address<address+size<=STACK[1]):
            raise ValueError('unowned/unaligned buffer memory')

    def load(self,address,size):
        self.memory_check(address,size)
        if not all(address+i in self.memory for i in range(size)):raise ValueError('uninitialized buffer memory')
        return super().load(address,size)

    def save(self,address,value,size):
        self.memory_check(address,size);super().save(address,value,size)

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed buffer instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('buffer instruction outside original')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        if self.instruction_count>=1600:raise ValueError('buffer instruction bound')
        op,rs,rt,rd,sh,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
        im=w&65535;si=im-65536 if im&32768 else im;target=None
        self.instruction_count+=1;self.visited.add(pc)
        if w==0:pass
        elif op==0:
            if fn in (0x21,0x2D) and sh==0:self.r[rd]=(self.r[rs]+self.r[rt])&MASK
            elif fn==0 and rs==0:self.r[rd]=(self.r[rt]<<sh)&MASK
            elif fn==0x25 and sh==0:self.r[rd]=(self.r[rs]|self.r[rt])&MASK
            elif fn in (0x2A,0x2B) and sh==0:
                self.r[rd]=int(signed(self.r[rs])<signed(self.r[rt])) if fn==0x2A else int((self.r[rs]&MASK)<(self.r[rt]&MASK))
            elif fn==0x0A and sh==0:
                if self.r[rt]&MASK==0:self.r[rd]=self.r[rs]
            elif w==0x03E00008:target=self.r[31]&MASK
            else:raise ValueError('unsupported buffer SPECIAL or bulk encoding')
        elif op==9:self.r[rt]=(self.r[rs]+si)&MASK
        elif op==12:self.r[rt]=self.r[rs]&im
        elif op==3:self.r[31]=pc+8;target=((pc+4)&0xF0000000)|((w&0x3FFFFFF)<<2)
        elif op in (4,5,6):
            if op==6 and rt:raise ValueError('reserved buffer branch register')
            take=(self.r[rs]==self.r[rt] if op==4 else self.r[rs]!=self.r[rt] if op==5 else signed(self.r[rs])<=0)
            if take:target=(pc+4+4*si)&MASK
        elif op in (30,32,35,36,55):
            n={30:16,32:1,35:4,36:1,55:8}[op];v=self.load((self.r[rs]+si)&MASK,n)
            if op==32 and v&128:v-=256
            self.r[rt]=v&MASK if n<=4 else v
        elif op in (31,40,43,63):self.save((self.r[rs]+si)&MASK,self.r[rt],{31:16,40:1,43:4,63:8}[op])
        else:raise ValueError('unsupported buffer instruction encoding')
        self.r[0]=0
        return target,False

    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,p in enumerate(RANGES) if p[0]==entry),None)
        if index is None or depth>2:raise ValueError('unreviewed buffer entry/depth')
        a,b=RANGES[index];self.invocations[index]+=1;pc=a
        if index==6:self.events.extend((2,self.r[4]&MASK,0,0))
        if index==7:
            d,s,n=(self.r[i]&MASK for i in (4,5,6))
            if n>32 or n>=8 and (d|s)&7==0:raise ValueError('aligned bulk strncpy outside byte observer')
            self.events.extend((1,d,EMPTY_TOKEN if STACK[0]<=s<STACK[1] else s,n))
        if len(self.events)>8:raise ValueError('buffer event bound')
        while True:
            if not a<=pc<b:raise ValueError('buffer transfer outside complete body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if annul:raise ValueError('unreviewed buffer annul')
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                delay=self.fetch(pc+4)
                if pc+4>=b or is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                    raise ValueError('buffer control in delay')
                if w==0x03E00008:
                    if pc!=b-8 or target!=stop:raise ValueError('buffer actual terminal JR31/stop required')
                    return
                if w>>26==3:
                    if target not in (RANGES[6][0],RANGES[7][0]):raise ValueError('unreviewed buffer call')
                    continuation=self.r[31];self.run(target,continuation,depth+1);pc=continuation
                else:pc=target if target is not None else pc+8
            else:
                if target is not None:raise ValueError('unexpected buffer transfer')
                pc+=4

def fixture(original,routine=0,index=1,end=3,bound=8,count=3,shift=1,source=0,child=0,alias=0):
    values=(routine,index,end,bound,count,shift,source,child,alias)
    if any(type(x) is not int for x in values) or routine not in range(6) or not 0<=index<=MASK or not 0<=end<=MASK or not 0<=count<=MASK:
        raise ValueError('invalid buffer fixture type/word')
    if bound not in (0,1,2,4,8,16) or shift not in range(4) or source not in range(4) or child not in (0,1) or alias not in range(4):
        raise ValueError('invalid buffer fixture domain')
    if alias and (alias==1 and routine!=2 or alias==2 and (routine!=2 or count not in (1,2)) or alias==3 and (routine!=5 or bound!=4)):
        raise ValueError('unsupported buffer alias domain')
    t=BufferTrace(original)
    for i in range(WORDS):t.save(BUFFER+4*i,initial_word(i),4)
    selected=ALT if child else CHILD
    t.save(OWNER+0x0C,TABLE,4);t.save(OWNER+0x10,selected,4);t.save(OWNER+0x20,index,4);t.save(OWNER+0x24,end,4);t.save(OWNER+0x28,HISTORY,4)
    for address in (CHILD,ALT):
        t.save(address,bound,4);t.save(address+4,count,4);t.save(address+8,DEST+shift+(0x40 if address==ALT else 0),4)
    for i in range(-2,5):
        t.save(TABLE+4*i,ALT if i&1 else CHILD,4);t.save(HISTORY+4*i,TEXTS[i%4],4)
    for k,p in enumerate(TEXTS):
        for j,x in enumerate(([65+k,0x80+k,67+k,0] if k else [0])):t.save(p+j,x,1)
    for p in (DEST+shift,DEST+0x40+shift):
        for j in range(33):t.save(p+j,0 if j==32 else 97+j%26,1)
    for j,x in enumerate(b'new\0'):t.save(NEW_TEXT+j,x,1)
    argument=TEXTS[source]
    if alias==1:t.save(selected+8,selected+4,4)
    if alias==2:t.save(selected+8,selected+8,4)
    if alias==3:
        argument=BUFFER+0x601;t.save(selected+8,selected+8,4);t.save(argument,NEW_TEXT,4) if argument%4==0 else None
        for j,x in enumerate(struct.pack('<I',NEW_TEXT)):t.save(argument+j,x,1)
    # Every array offset that will actually be read must stay in owned storage.
    if routine==0:
        actual=(index-1)&MASK if signed(index)>0 else index
        p=(HISTORY+((actual<<2)&MASK))&MASK
        if not HISTORY-8<=p<=HISTORY+16:raise ValueError('history pointer outside selected fixture array')
    if routine==1 and signed(index)<signed(end) and signed((index+1)&MASK)<signed(end):
        p=(HISTORY+(((index+1)<<2)&MASK))&MASK
        if not HISTORY-8<=p<=HISTORY+16:raise ValueError('next history pointer outside selected fixture array')
    if routine==2 and signed(count)>0 and count>32:raise ValueError('deletion outside bounded readable buffer')
    if routine==3 and not TABLE-8<=(TABLE+((index<<2)&MASK))&MASK<=TABLE+16:raise ValueError('record index outside fixture array')
    initial=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    t.r[4]=OWNER;t.r[5]=index if routine==3 else argument;t.r[29]=0x80000;t.r[31]=RETURN
    saved=0xA5A5A5A5112233445566778899AABBCC;t.r[16]=saved
    t.run(RANGES[routine][0]);assert t.r[29]==0x80000 and t.r[16]==saved
    expected=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    return dict(routine=routine,index=index,end=end,bound=bound,count=count,shift=shift,source=source,child=child,alias=alias,
        argument=index if routine==3 else argument,result=t.r[2]&MASK if routine in (3,4) else 0,
        initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],
        changes=[[i,v] for i,v in enumerate(expected) if v!=initial[i]],events=t.events,
        instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,invocations=t.invocations)

def fixtures(original):
    cases=[]
    for i in (0,1,2,MASK,0x80000000,0x40000000,0x7FFFFFFF):
        for bound in (0,1,2,4,8,16):
            for shift in range(4):cases.append(fixture(original,0,i,bound=bound,shift=shift))
    for i,e in ((0,0),(0,1),(0,3),(1,3),(2,3),(4,3),(MASK,0),(MASK,2),(0x80000000,0x80000002),(0x80000001,0x80000002),(0x7FFFFFFE,0x7FFFFFFF)):
        for bound in (0,1,2,4,8,16):
            for shift in (1,2,3):cases.append(fixture(original,1,i,e,bound=bound,shift=shift))
    for count in (0,1,2,3,4,16,MASK,0x80000000):
        for shift in range(4):
            for child in (0,1):cases.append(fixture(original,2,count=count,shift=shift,child=child))
    for index in (0,1,2,MASK,MASK-1,0x80000000,0x40000001):cases.append(fixture(original,3,index))
    for child in (0,1):
        for shift in range(4):cases.append(fixture(original,4,shift=shift,child=child))
    for bound in (0,1,2,4,8,16):
        for shift in range(4):
            for source in range(4):cases.append(fixture(original,5,bound=bound,shift=shift,source=source))
    for count in (1,2,3,4):cases.append(fixture(original,2,count=count,alias=1))
    for count in (1,2):cases.append(fixture(original,2,count=count,alias=2))
    cases.append(fixture(original,5,bound=4,alias=3))
    return cases

def golden(cases):
    maxinitial=max(len(c['initial']) for c in cases);maxchanges=max(len(c['changes']) for c in cases)
    lines=['/* Authored bounded navigation/byte-copy observations, no original words. */',
        '#define BUFFER_RECORD_WORDS %du'%WORDS,'#define BUFFER_RECORD_INITIAL %du'%maxinitial,'#define BUFFER_RECORD_CHANGES %du'%maxchanges,
        'struct BufferPair {u32 index,value;};',
        'struct BufferGolden {u32 routine,argument,result,initial_count,change_count,event_count;struct BufferPair initial[BUFFER_RECORD_INITIAL],changes[BUFFER_RECORD_CHANGES];u32 events[8];};',
        'static const u32 buffer_pointer_cells[]={'+','.join('%du'%((p-BUFFER)//4) for p in POINTER_CELLS)+'};',
        'static const struct BufferGolden buffer_golden[]={']
    for c in cases:
        pair=lambda v:'{'+','.join('{%du,0x%08Xu}'%(i,x) for i,x in v)+'}'
        vals=[c[k] for k in ('routine','argument','result')]+[len(c['initial']),len(c['changes']),len(c['events'])]
        lines.append('{'+','.join('0x%08Xu'%v for v in vals)+','+pair(c['initial'])+','+pair(c['changes'])+',{' +','.join('0x%08Xu'%v for v in c['events'])+'}},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/buffer_records/trace.json');p.add_argument('--header',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    keys=('routine','index','end','bound','count','shift','source','child','alias')
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases),
        input_sha256=hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True).encode()).hexdigest(),
        coverage=[len({pc for c in cases for pc in c['visited'] if a<=pc<b}) for a,b in RANGES],cases=cases)
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n')
    if a.header:a.header.write_text(golden(cases),newline='\n')
    print(report['fixtures'],'buffer fixtures;',report['instructions'],'instructions;',report['coverage'],'coverage')

if __name__=='__main__':main()
