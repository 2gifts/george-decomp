"""Original stream-close/copy fixtures with real flush/init/read instruction paths.

This bounded low-word observer models initialized memory and actual delayed
control transfers. Write/close callbacks are controlled observations, not host
filesystem, allocation, concurrency, EE upper-register or exception behavior.
Only synthetic inputs/results are exported; original words stay in the local ELF.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x3941D8,0x39428C),(0x3947D8,0x394808),(0x394290,0x39439C),
        (0x3945A8,0x394634),(0x3943F8,0x394450),(0x394808,0x39491C))
BUFFER,WORDS=0x20000,2048
END=BUFFER+WORDS*4
FILE,CTX0,CTX1=0x20200,0x21000,0x21400
DATA,UB,LB,PAYLOAD,ALT=0x20400,0x20500,0x20580,0x20600,0x20700
IMPURE=0x405694
WRITE,CLOSE,CLOSE_ALT=0x60000000,0x60000004,0x60000008
STD_READ,STD_WRITE,STD_SEEK,STD_CLOSE=0x3953E8,0x395450,0x3954D0,0x395538
STACK=(0x7F000,0x81000)
MASK=0xFFFFFFFF

def signed(v):return (v&MASK)-0x100000000 if v&0x80000000 else v&MASK

class StdioTrace(Trace):
    def __init__(self,original,selected_file=FILE,writeret=2,closeret=0,mutation=0):
        super().__init__(original)
        if any(type(v) is not int for v in (selected_file,writeret,closeret,mutation)) or not 0<=mutation<64:
            raise ValueError('unreviewed stdio observer parameters')
        self.selected_file=selected_file;self.writeret=writeret&MASK;self.closeret=closeret&MASK;self.mutation=mutation
        self.visited=set();self.events=[];self.branches=[];self.invocations=[0]*len(RANGES)
        self.active=[]

    def memory_check(self,address,size):
        valid=(BUFFER<=address<address+size<=END or address==IMPURE and size==4 or
               STACK[0]<=address<address+size<=STACK[1])
        if size not in (1,2,4,8,16) or address%size or not valid:
            raise ValueError('unowned/unaligned stdio memory')

    def load(self,address,size):
        self.memory_check(address,size)
        if not all(address+i in self.memory for i in range(size)):
            raise ValueError('uninitialized stdio memory')
        return super().load(address,size)

    def save(self,address,value,size):
        self.memory_check(address,size);super().save(address,value,size)

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed stdio instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('stdio instruction outside original')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        op,rs,rt,rd,sh,fn=w>>26,(w>>21)&31,(w>>16)&31,(w>>11)&31,(w>>6)&31,w&63
        im=w&65535;si=im-65536 if im&32768 else im
        if self.instruction_count>=12000:raise ValueError('stdio instruction bound')
        self.instruction_count+=1;self.visited.add(pc);target=None;annul=False
        if w==0:pass
        elif op==0:
            if fn in (0x21,0x23,0x2D) and sh==0:
                self.r[rd]=(self.r[rs]+(-self.r[rt] if fn==0x23 else self.r[rt]))&MASK
            elif fn in (0x2A,0x2B) and sh==0:
                self.r[rd]=int(signed(self.r[rs])<signed(self.r[rt])) if fn==0x2A else int((self.r[rs]&MASK)<(self.r[rt]&MASK))
            elif fn==0x0A and sh==0:
                if self.r[rt]==0:self.r[rd]=self.r[rs]
            elif w==0x03E00008:target=self.r[31]&MASK
            elif fn==9 and rd==31 and rt==sh==0:
                target=self.r[rs]&MASK;self.r[31]=pc+8
            elif fn==0x18 and sh==0:
                value=signed(self.r[rs])*signed(self.r[rt]);self.lo=value&MASK;self.hi=(value>>32)&MASK
                if rd:self.r[rd]=self.lo
            else:raise ValueError('unsupported stdio SPECIAL encoding')
        elif op==9:self.r[rt]=(self.r[rs]+si)&MASK
        elif op==15 and rs==0:self.r[rt]=im<<16
        elif op==12:self.r[rt]=self.r[rs]&im
        elif op==13:self.r[rt]=self.r[rs]|im
        elif op==3:self.r[31]=pc+8;target=((pc+4)&0xF0000000)|((w&0x3FFFFFF)<<2)
        elif op in (4,5,6,7,20,21):
            if op in (6,7) and rt:raise ValueError('reserved stdio branch register')
            take=((self.r[rs]==self.r[rt]) if op in (4,20) else (self.r[rs]!=self.r[rt]) if op in (5,21) else
                  signed(self.r[rs])<=0 if op==6 else signed(self.r[rs])>0)
            if take:target=(pc+4+4*si)&MASK
            elif op in (20,21):annul=True
        elif op==1 and rt in (0,2):
            if signed(self.r[rs])<0:target=(pc+4+4*si)&MASK
            elif rt==2:annul=True
        elif op in (30,33,35,36,37,55):
            n={30:16,33:2,35:4,36:1,37:2,55:8}[op];v=self.load((self.r[rs]+si)&MASK,n)
            if op==33 and v&0x8000:v-=65536
            self.r[rt]=v&MASK if n<=4 else v
        elif op in (31,40,41,43,63):
            self.save((self.r[rs]+si)&MASK,self.r[rt],{31:16,40:1,41:2,43:4,63:8}[op])
        else:raise ValueError('unsupported stdio encoding')
        self.r[0]=0
        return target,annul

    def callback(self,target):
        if target not in (WRITE,CLOSE,CLOSE_ALT,STD_CLOSE):raise ValueError('unreviewed stdio callback')
        f=self.selected_file
        if not BUFFER<=f<=END-88:raise ValueError('callback without selected FILE')
        writing=target==WRITE
        kind=0 if writing else 2 if target==STD_CLOSE else 1 if target==CLOSE else 3
        cookie=self.r[4]&MASK;buf=self.r[5]&MASK if writing else 0;count=self.r[6]&MASK if writing else 0
        self.events.extend([kind,cookie,buf,count,self.load(f+12,2),self.load(f+44,4),self.load(f+84,4)])
        if len(self.events)>140:raise ValueError('stdio callback event bound')
        if writing:
            if self.mutation&1:self.save(f+44,CLOSE_ALT,4);self.save(f+28,BUFFER+0x60,4)
            if self.mutation&2:self.save(f+44,0,4)
            if self.mutation&4:self.save(f+84,CTX1,4);self.save(f+12,self.load(f+12,2)|0x40,2)
            if self.mutation&32:self.save(IMPURE,CTX1,4)
            result=self.writeret
        else:
            if self.mutation&8:
                for offset in (16,48,68):self.save(f+offset,ALT,4)
                self.save(f+12,0x7FFF,2);self.save(f+84,CTX1,4)
            if self.mutation&16:self.save(IMPURE,CTX1,4)
            result=self.closeret
        self.r[2]=result
        for reg in range(4,12):self.r[reg]=0xDEADBEEF

    def run(self,entry,stop=RETURN):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None:raise ValueError('unreviewed stdio entry')
        a,b=RANGES[index];self.invocations[index]+=1;self.active.append(index);pc=entry
        if len(self.active)>8:raise ValueError('stdio call depth')
        try:
            while True:
                if not a<=pc<b:raise ValueError('stdio transfer outside body')
                w=self.fetch(pc);target,annul=self.execute(w,pc)
                if is_control_transfer(w):
                    self.branches.append([pc,target is not None,annul])
                    if not annul:
                        if pc+4>=b:raise ValueError('stdio delay outside body')
                        delay=self.fetch(pc+4)
                        if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('stdio control in delay')
                    if w==0x03E00008:
                        if target!=stop or pc!=b-8:raise ValueError('stdio nonterminal/unexpected return')
                        return
                    if w>>26==3 or w>>26==0 and w&63==9:
                        continuation=self.r[31]
                        if any(target==lo for lo,hi in RANGES):self.run(target,continuation)
                        else:self.callback(target)
                        if self.r[31]!=continuation:raise ValueError('stdio callback corrupted RA')
                        pc=continuation
                    else:pc=target if target is not None else pc+8
                else:
                    if target is not None or annul:raise ValueError('unexpected stdio transfer')
                    pc+=4
        finally:self.active.pop()

def pointers():
    # Exact typed pointer cells in authored FILE/reent views only.
    cells=[]
    for f in (FILE,*[base+0x1E4+i*88 for base in (CTX0,CTX1) for i in range(3)]):
        cells.extend(f+o for o in (0,16,28,32,36,40,44,48,56,68,84))
    for d in (CTX0,CTX1):cells.extend(d+o for o in (4,8,12,60,0x1E0))
    return sorted(set(cells))

POINTER_CELLS=pointers()
# Packed signed-short flags and descriptor can numerically resemble an address.
# They remain scalars and must never undergo native pointer translation.
PACKED_SHORT_CELLS={f+12 for f in (FILE,*[base+0x1E4+i*88 for base in (CTX0,CTX1) for i in range(3)])}

def fixture(original,routine=0,mode=1,flags=9,close=1,writeret=2,closeret=0,mutation=0,count=8,source=PAYLOAD,destination=PAYLOAD+64):
    if any(type(v) is not int for v in (routine,mode,flags,close,writeret,closeret,mutation,count,source,destination)):
        raise ValueError('unreviewed stdio fixture type')
    if routine not in (0,1,2) or mode not in range(7) or not 0<=flags<=65535 or close not in (0,1) or not 0<=mutation<64 or not 0<=count<=64:
        raise ValueError('unreviewed stdio fixture domain')
    if routine in (1,2) and (not BUFFER<=source<=END-count or not BUFFER<=destination<=END-count):
        raise ValueError('unreviewed copy domain')
    f=0 if mode==0 else CTX0+0x1E4+(mode-4)*88 if mode>=4 else FILE
    t=StdioTrace(original,f,writeret,closeret,mutation)
    for p in range(BUFFER,END):t.memory[p]=0
    for d in (CTX0,CTX1):
        t.save(d+56,1 if d==CTX1 or mode in (0,1,2) else 0,4)
        for off,i in ((4,0),(8,1),(12,2)):t.save(d+off,d+0x1E4+i*88,4)
    t.save(IMPURE,CTX0,4)
    if f:
        values={0:DATA+5,4:32,8:7,16:DATA,20:16,24:0x12345678,28:BUFFER+0x40,
                32:STD_READ,36:WRITE,40:STD_SEEK,44:CLOSE if close else 0,
                48:UB,52:13,56:DATA+11,60:9,64:0x11223344,68:LB,72:17,76:21,80:25,
                84:0 if mode==2 else CTX0}
        for off,value in values.items():t.save(f+off,value,4)
        t.save(f+12,flags,2);t.save(f+14,7,2)
    for i in range(96):t.save(PAYLOAD+i,(i*7+3)&255,1)
    if routine==2:
        if not f or count>32:raise ValueError('unreviewed fread fixture')
        t.save(f,PAYLOAD,4);t.save(f+4,32,4)
    initial=[[i,t.load(BUFFER+i*4,4)] for i in range(WORDS) if t.load(BUFFER+i*4,4)]
    t.r[29],t.r[31]=0x80000,RETURN
    if routine==0:t.r[4]=f;entry=RANGES[0][0]
    elif routine==1:t.r[4:7]=[destination,source,count];entry=RANGES[1][0]
    else:t.r[4:8]=[destination,1,count,f];entry=RANGES[5][0]
    t.run(entry)
    expected=[[i,t.load(BUFFER+i*4,4)] for i in range(WORDS) if t.load(BUFFER+i*4,4)]
    # No address-valued scalar inference: all such words must be typed cells.
    for pairs in (initial,expected):
        for i,v in pairs:
            if BUFFER<=v<END or v in (WRITE,CLOSE,CLOSE_ALT,STD_READ,STD_WRITE,STD_SEEK,STD_CLOSE,0x394748):
                if BUFFER+i*4 not in POINTER_CELLS and BUFFER+i*4 not in PACKED_SHORT_CELLS:
                    raise ValueError('undesignated pointer-valued scalar')
    return dict(routine=routine,mode=mode,flags=flags,close=close,writeret=writeret&MASK,closeret=closeret&MASK,mutation=mutation,
        count=count,source=source,destination=destination,selected_file=f,initial=initial,expected=expected,
        result=t.r[2]&MASK if routine!=1 else 0,impure=t.load(IMPURE,4),events=t.events,
        instructions=t.instruction_count,visited=sorted(t.visited),invocations=t.invocations,branches=t.branches)

def fixtures(original):
    cases=[]
    for mode in range(7):
        for flags in (0,4,8,9,0x8000,0x88,0xFFFF):
            for close in (0,1):
                for result in (0,-1,-2147483648):cases.append(fixture(original,mode=mode,flags=flags,close=close,closeret=result))
    for mutation in range(64):
        for w in (0,2,-1):cases.append(fixture(original,writeret=w,closeret=(-1 if mutation&1 else 2147483647),mutation=mutation))
    for count in range(17):
        for shift in range(-3,4):
            for align in (0,1):cases.append(fixture(original,routine=1,count=count,source=PAYLOAD+8+align,destination=PAYLOAD+8+align+shift))
    for count in (1,2,3,8,16,24,32):cases.append(fixture(original,routine=2,flags=4,count=count,destination=PAYLOAD+100))
    return cases

def input_hash(cases):
    keys=('routine','mode','flags','close','writeret','closeret','mutation','count','source','destination')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()

def golden_header(cases):
    fields=('routine','mode','flags','close','writeret','closeret','mutation','count','source','destination','selected_file','result','impure')
    maxpairs=max(max(len(c['initial']),len(c['expected'])) for c in cases)
    lines=['/* Authored initialized stdio observations, no original words or assets. */',
        '#define STDIO_WORDS %du'%WORDS,'#define STDIO_MAX_PAIRS %du'%maxpairs,
        'struct StdioPair {u32 index,value;};',
        'struct StdioGolden {u32 '+','.join(fields)+',initial_count,expected_count,event_count;struct StdioPair initial[STDIO_MAX_PAIRS],expected[STDIO_MAX_PAIRS];u32 events[140];};',
        'static const u32 stdio_pointer_cells[]={'+','.join('0x%08Xu'%a for a in POINTER_CELLS)+'};',
        'static const struct StdioGolden stdio_golden[]={']
    for c in cases:
        values=[c[k] for k in fields]+[len(c['initial']),len(c['expected']),len(c['events'])]
        pair=lambda v:'{'+','.join('{%du,0x%08Xu}'%(i,w) for i,w in v)+'}'
        lines.append('{'+','.join('0x%08Xu'%v for v in values)+','+pair(c['initial'])+','+pair(c['expected'])+',{' +','.join('0x%08Xu'%v for v in c['events'])+'}},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/reuse/stdio_close/trace.json');p.add_argument('--golden-header',type=Path)
    a=p.parse_args();_,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    report=dict(limit=__doc__,cases=cases,input_sha256=input_hash(cases),fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases))
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n')
    if a.golden_header:a.golden_header.write_text(golden_header(cases),newline='\n')
    print('%d stdio fixtures;%d instructions;maximum%d;input%s'%(len(cases),report['instructions'],report['maximum'],report['input_sha256']))

if __name__=='__main__':main()
