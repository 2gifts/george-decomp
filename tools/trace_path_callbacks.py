"""Finite callback fixtures using complete validated original instructions.

Fifteen callbacks and the published quaternion interpolation helper execute
original instructions. Projection and trigonometric calls use authored finite
observation/mutation contracts. This excludes exceptional EE/FCR arithmetic,
unknown input formats/capacities, actual trigonometric approximation and cycle or
stack-placement claims. Only synthetic memory and observations are exported.
"""
import argparse,hashlib,json,math,struct
from pathlib import Path
from analyze import validated_elf
from trace_resource_base import ResourceBaseTrace
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN,rounded,scalar,word, is_control_transfer
ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2C03E8,0x2C055C),(0x2C0560,0x2C06DC),(0x2C0C18,0x2C0D80),
        (0x2C19A8,0x2C19EC),(0x2C1AD0,0x2C1BAC),(0x2C1BB0,0x2C1C9C),
        (0x2C1E20,0x2C1EF4),(0x2C1EF8,0x2C1F4C),(0x2C1F50,0x2C1F8C),
        (0x2C1F90,0x2C2088),(0x2C2088,0x2C2180),(0x2C21B0,0x2C2258),
        (0x2C2300,0x2C238C),(0x2C23C8,0x2C2440),(0x2C2440,0x2C24B8))
HELPERS=((0x2A2780,0x2A298C),)
ENTRIES=tuple(a for a,b in RANGES)
BUFFER,END,WORDS=0x20000,0x20200,128
FIRST,SECOND,OUTPUT,REFERENCE,DISTANCE,POSITION=(0x20040,0x200C0,0x20140,0x20160,0x20170,0x20174)
MEMORY_RANGES=((BUFFER,END),(0x7F000,0x81000))
class CallbackTrace(ResourceBaseTrace):
    def __init__(self,original,mode=0):
        super().__init__(original)
        self.mode,self.events,self.callback_calls = mode,[],0

    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(
                a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('callback memory outside aligned authored scope')
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized callback memory')

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+HELPERS):
            raise ValueError('unreviewed callback instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('callback instruction outside original')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        op,rs,rt,rd,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w&63
        if op==17 and rs==4:
            # Signed integer conversion legitimately transfers non-float bits;
            # actual scalar uses still pass their separate finite guards.
            if w&0x7FF:raise ValueError('reserved callback MTC1 operands')
            self.f[rd]=scalar(self.r[rt]&0xFFFFFFFF)
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('callback instruction bound exceeded')
            return None,False
        if op==0 and fn in (3,0x25) or op in (1,0x20):
            target=None
            sh=w>>6&31
            if op==0:
                if fn==3:
                    if rs:raise ValueError('reserved callback arithmetic shift source')
                    value=self.r[rt]&0xFFFFFFFF
                    value-=0x100000000 if value&0x80000000 else 0
                    self.r[rd]=(value>>sh)&0xFFFFFFFF
                else:
                    if sh:raise ValueError('reserved callback OR shift field')
                    self.r[rd]=(self.r[rs]|self.r[rt])&0xFFFFFFFF
            else:
                imm=w&65535;imm-=65536 if imm&32768 else 0
                if op==1:
                    if rt!=1:raise ValueError('unsupported callback REGIMM form')
                    value=self.r[rs]&0xFFFFFFFF
                    value-=0x100000000 if value&0x80000000 else 0
                    if value>=0:target=(pc+4+imm*4)&0xFFFFFFFF
                else:
                    value=self.load((self.r[rs]+imm)&0xFFFFFFFF,1)
                    value-=256 if value&128 else 0
                    self.r[rt]=value&0xFFFFFFFF
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('callback instruction bound exceeded')
            return target,False
        if op==17 and rs==16 and fn==0x29:
            left,right=word(self.f[rd]),word(self.f[rt])
            if not math.isfinite(self.f[rd]) or not math.isfinite(self.f[rt]):
                raise ValueError('callback minimum outside finite model')
            sl=left-0x100000000 if left&0x80000000 else left
            sr=right-0x100000000 if right&0x80000000 else right
            take_left=sl>sr if left&right&0x80000000 else sl<sr
            self.f[w>>6&31]=self.f[rd] if take_left else self.f[rt]
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('callback instruction bound exceeded')
            return None,False
        if op==0 and fn in (10,11,42) or op==6:
            if w>>6&31 and op==0:raise ValueError('reserved callback SPECIAL operand')
            target=None
            if op==6:
                if rt:raise ValueError('reserved callback BLEZ operand')
                n=self.r[rs]&0xFFFFFFFF;n-=0x100000000 if n&0x80000000 else 0
                imm=w&65535;imm-=65536 if imm&32768 else 0
                if n<=0:target=(pc+4+4*imm)&0xFFFFFFFF
            elif fn in (10,11):
                condition=self.r[rt]&0xFFFFFFFF
                if (condition==0)==(fn==10):self.r[rd]=self.r[rs]
            else:
                a,b=self.r[rs]&0xFFFFFFFF,self.r[rt]&0xFFFFFFFF
                a-=0x100000000 if a&0x80000000 else 0
                b-=0x100000000 if b&0x80000000 else 0
                self.r[rd]=int(a<b)
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('callback instruction bound exceeded')
            return target,False
        if op==17 and rs==20:
            if fn!=32 or rt:raise ValueError('unsupported callback CVT.S.W operands')
            n=word(self.f[rd]);n-=0x100000000 if n&0x80000000 else 0
            self.f[w>>6&31]=rounded(float(n));self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('callback instruction bound exceeded')
            return None,False
        if op in (0x31,0x39,17):
            if op==17 and not(rs in (0,4,8) or rs==16 and fn in (0,1,2,3,4,6,7,0x32,0x34,0x36)):
                raise ValueError('unsupported callback COP1 opcode')
            if op==17 and rs==16 and fn==3 and self.f[rt]==0:
                raise ValueError('callback zero denominator outside finite fixtures')
            return CameraTrace.execute(self,w,pc)
        return super().execute(w,pc)

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES+HELPERS if r[0]==entry),None)
        if body is None:raise ValueError('unsupported callback entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body callback transfer')
            w=self.fetch(pc);target,annul=self.execute(w,pc);op=w>>26
            branch=op in (1,4,5,6,20,21) or op==17 and w>>21&31==8
            call=op==3 or op==0 and w&63==9
            if op==0 and w&63==8 and (w!=0x03E00008 or target!=stop):
                raise ValueError('callback must return through actual JR31')
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('callback delay outside complete body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay):raise ValueError('callback transfer in delay')
                if self.execute(delay,pc+4)!=(None,False):raise ValueError('callback transfer in delay')
                if w==0x03E00008:return
                if target is None:pc+=8
                elif call:
                    continuation=self.r[31]
                    if target in ENTRIES or target in tuple(a for a,b in HELPERS):self.run(target,continuation)
                    else:self.library_call(target)
                    if self.r[31]!=continuation:raise ValueError('callback callback corrupted link register')
                    pc=continuation
                else:
                    if not body[0]<=target<body[1]:raise ValueError('callback local transfer outside owned body')
                    pc=target
            else:pc+=8 if annul else 4

    def event(self,*values):
        assert len(values)==8
        self.events.extend(values)

    def library_call(self,target):
        a,b,c,d=[self.r[i]&0xFFFFFFFF for i in range(4,8)]
        if target in (0x29C230,0x29C090):
            x=self.f[12]
            if not math.isfinite(x):raise ValueError('nonfinite controlled callback angle')
            self.event(0 if target==0x29C230 else 1,word(x),0,0,0,0,0,0)
            self.f[0]=(0.0 if x==1.0 else scalar(0x40490FDB) if x==-1.0 else 1.0) if target==0x29C230 else (1.0 if x==1.0 else 0.5)
            if self.mode==1:
                for j in range(7):
                    self.single(FIRST+4+4*j,float(2+j))
                    self.single(SECOND+4+4*j,float(3-j))
        elif target==0x29CF28:
            left=[self.load(a+4*i,4) for i in range(3)]
            right=[self.load(b+4*i,4) for i in range(3)]
            query=[self.load(c+4*i,4) for i in range(3)]
            self.event(2,*left,*right,self.mode)
            self.event(3,*query,d,0,0,0)
            self.single(d,0.25);self.f[0]=3.5
            if self.mode==2:
                self.single(FIRST,20.0);self.single(SECOND,40.0)
        else:raise ValueError('unknown controlled callback call')
        for i in range(3,16):self.r[i]=0xDEADBEEF


def fixture(original,function,time=0.25,alias=0,mode=0,pattern=0):
    t=CallbackTrace(original,mode)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0
    for i in range(WORDS):t.single(BUFFER+4*i,float((i%23)-11)*0.25)
    for base,k in ((FIRST,1),(SECOND,-1)):
        t.single(base,10.0 if k==1 else 22.0)
        for i in range(1,16):t.single(base+i*4,float(k*((i%5)-2))*0.25)
    if pattern>=100:
        seed=pattern
        for base in (FIRST,SECOND):
            for i in range(16):
                seed=(1664525*seed+1013904223)&0xFFFFFFFF
                t.single(base+4*i,float((seed>>16)%201-100)/7.0)
    for i in range(3):t.single(REFERENCE+4*i,float(i-1))
    t.single(DISTANCE,8.0);t.single(POSITION,0.75)
    if function in (2,9,10):
        values=((-32768,-1,0,32767),(32767,-32768,1,-1),(0,0,0,0),
                (16384,8192,-16384,4096),(1,2,3,4))
        if function==10:
            values=((-128,-1,0,127),(127,-128,1,-1),(0,0,0,0),
                    (64,32,-64,16),(1,2,3,4))
        chosen=values[pattern%len(values)]
        if function==2:
            for base,k in ((FIRST,0),(SECOND,1)):
                for i in range(3):t.save(base+i*2,chosen[(i+k)%4]&65535,2)
                t.save(base+6,(0,127,128,255,64)[pattern%5],1)
                t.save(base+7,77,1)
        else:
            width=2 if function==9 else 1
            for base,k in ((FIRST,0),(SECOND,1)):
                for i in range(4):t.save(base+4+i*width,chosen[(i+k)%4]&((1<<(8*width))-1),width)
    elif function==11:
        quats=((1,0,0,0),(0,1,0,0),(-1,0,0,0),(2,0,0,0),(0.5,0.5,0.5,0.5),(-1.0000001192092896,0,0,0))
        left=(1,0,0,0);right=quats[pattern%len(quats)]
        for base,q in ((FIRST,left),(SECOND,right)):
            for i,x in enumerate(q):t.single(base+4+4*i,x)
    out=OUTPUT;distance=DISTANCE;position=POSITION;first=FIRST;second=SECOND
    if alias==1:out=FIRST
    elif alias==2:out=FIRST+4
    elif alias==3:out=FIRST+8
    elif alias==4:out=FIRST+0x10
    elif alias==5:out=FIRST+0x1C
    elif alias==6:out=SECOND+4
    elif alias==7:out=SECOND+0x10
    elif alias==8:out=SECOND+0x1C
    elif alias==9:second=FIRST+4
    elif alias==10:distance=position
    elif alias==11:distance=FIRST
    elif alias==12:position=SECOND
    elif alias==13:position=FIRST;distance=SECOND
    elif alias==14:distance=REFERENCE;position=REFERENCE+4
    if function<13:args=(first,second,out,0,0)
    else:args=(REFERENCE,first,second,distance,position)
    initial=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    t.r[4:9]=args;t.r[29],t.r[31]=0x80000,RETURN;t.f[12]=time
    t.run(ENTRIES[function])
    return dict(function=function,args=list(args),time=word(time),mode=mode,pattern=pattern,alias=alias,
                initial=initial,expected=[t.load(BUFFER+4*i,4) for i in range(WORDS)],
                events=t.events,instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for function in range(15):
        times=(-1.0,0.0,0.25,0.5,0.9999999403953552,1.0,1.25)
        for time in times:
            for alias in (0,1,2,3,4,5,6,7,8,9) if function<13 else (0,10,11,12,13,14):
                cases.append(fixture(original,function,time,alias))
    for function in (2,9,10,11):
        for pattern in range(6 if function==11 else 5):
            for alias in (0,1,2,6,9):
                for mode in (0,1):cases.append(fixture(original,function,0.375,alias,mode,pattern))
    for function in (13,14):
        for alias in (0,10,11,12,13,14):cases.append(fixture(original,function,0.25,alias,2))
    for function in (0,1,4,5,6,7,8,12):
        for pattern in range(100,140):
            cases.append(fixture(original,function,scalar(0x3DFCD6DE),pattern%10,0,pattern))
    return cases


def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:v for k,v in c.items() if k not in ('expected','events','instruction_count')} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    n=max(len(c['events']) for c in cases);ar=lambda v:'{'+','.join('0x%08Xu'%x for x in v)+'}'
    lines=['/* Authored finite/controlled fixtures; no original code/data arrays. */',
           'struct CallbackGolden { u32 function,args[5],time,mode,event_count,events[%d],initial[%d],expected[%d]; };'%(n,WORDS,WORDS),
           'static const struct CallbackGolden callback_golden[] = {']
    for c in cases:lines.append(' {%du,%s,0x%08Xu,%du,%du,%s,%s,%s},'%(c['function'],ar(c['args']),c['time'],c['mode'],len(c['events']),ar(c['events']),ar(c['initial']),ar(c['expected'])))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/path_callbacks/trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitations':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d finite callback fixtures; %d original instructions; max%d; input%s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))

if __name__=='__main__':main()
