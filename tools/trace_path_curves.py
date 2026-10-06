"""Bounded finite curve fixtures with complete selected and vector originals.

Seven selected bodies and the published normalization/spherical interpolation
helpers execute real locally validated instructions. Format getters, callbacks,
query/projection and trigonometric/remainder calls have authored observation
contracts. This excludes arbitrary formats, invalid record counts, exceptional
EE/FCR behavior, full callback algorithms and cycle or stack-placement claims.
Only authored memory and observations are exported.
"""
import argparse, hashlib, json, math, struct
from pathlib import Path
from analyze import validated_elf
from trace_resource_base import ResourceBaseTrace
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN, rounded, scalar, word, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x295EB8,0x29655C),(0x296560,0x2966EC),(0x2966F0,0x296B50),
          (0x297178,0x297208),(0x297208,0x297290),(0x297290,0x297318),
          (0x297318,0x297384))
HELPERS = ((0x2A3390,0x2A3538),(0x2A3538,0x2A35C0))
ENTRIES = tuple(a for a,b in RANGES)
CALLS = (0x2C24B8,0x2C24D8,0x2C24F8,0x2C2518,0x37B238,
         0x29DD60,0x29CF28,0x29C230,0x29C090)
CURVE, PROJECTION = 0xF0000100,0xF0000200
BUFFER, END, DATA, REFERENCE, OUTPUT, POSITION, FRACTION, INDEX = (
    0x20000,0x20800,0x20080,0x20280,0x202C0,0x20300,0x20304,0x20308)
FIRST, SECOND, WORDS = 0x20310,0x20314,512
MEMORY_RANGES = ((BUFFER,END),(0x7F000,0x81000))

class CurveTrace(ResourceBaseTrace):
    def __init__(self,original,mode=0):
        super().__init__(original)
        self.mode,self.events,self.callback_calls = mode,[],0

    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(
                a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('curve memory outside aligned authored scope')
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized curve memory')

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+HELPERS):
            raise ValueError('unreviewed curve instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('curve instruction outside original')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        op,rs,rt,rd,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w&63
        if op==0 and fn in (10,11,42) or op==6:
            if w>>6&31 and op==0:raise ValueError('reserved curve SPECIAL operand')
            target=None
            if op==6:
                if rt:raise ValueError('reserved curve BLEZ operand')
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
            if self.instruction_count>3000:raise ValueError('curve instruction bound exceeded')
            return target,False
        if op==17 and rs==20:
            if fn!=32 or rt:raise ValueError('unsupported curve CVT.S.W operands')
            n=word(self.f[rd]);n-=0x100000000 if n&0x80000000 else 0
            self.f[w>>6&31]=rounded(float(n));self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('curve instruction bound exceeded')
            return None,False
        if op in (0x31,0x39,17):
            if op==17 and not(rs in (0,4,8) or rs==16 and fn in (0,1,2,3,4,6,7,0x32,0x34,0x36)):
                raise ValueError('unsupported curve COP1 opcode')
            if op==17 and rs==16 and fn==3 and self.f[rt]==0:
                raise ValueError('curve zero denominator outside finite fixtures')
            return CameraTrace.execute(self,w,pc)
        return super().execute(w,pc)

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES+HELPERS if r[0]==entry),None)
        if body is None:raise ValueError('unsupported curve entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body curve transfer')
            w=self.fetch(pc);target,annul=self.execute(w,pc);op=w>>26
            branch=op in (4,5,6,20,21) or op==17 and w>>21&31==8
            call=op==3 or op==0 and w&63==9
            if op==0 and w&63==8 and (w!=0x03E00008 or target!=stop):
                raise ValueError('curve must return through actual JR31')
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('curve delay outside complete body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay):raise ValueError('curve transfer in delay')
                if self.execute(delay,pc+4)!=(None,False):raise ValueError('curve transfer in delay')
                if w==0x03E00008:return
                if target is None:pc+=8
                elif call:
                    continuation=self.r[31]
                    if target in ENTRIES or target in tuple(a for a,b in HELPERS):self.run(target,continuation)
                    else:self.library_call(target)
                    if self.r[31]!=continuation:raise ValueError('curve callback corrupted link register')
                    pc=continuation
                else:
                    if not body[0]<=target<body[1]:raise ValueError('curve local transfer outside owned body')
                    pc=target
            else:pc+=8 if annul else 4

    def event(self,*values):
        assert len(values)==8
        self.events.extend(values)

    def library_call(self,target):
        a,b,c,d,e=[self.r[i]&0xFFFFFFFF for i in range(4,9)]
        if target in CALLS[:4]:
            self.event(CALLS.index(target),a,0,0,0,0,0,0)
            self.r[2]=PROJECTION if target==0x2C2518 else CURVE+(CALLS.index(target)*4)
        elif target==0x37B238:
            x,y=self.f[12],self.f[13]
            if y==0:raise ValueError('zero curve remainder denominator')
            self.event(4,word(x),word(y),0,0,0,0,0)
            self.f[0]=rounded(math.fmod(x,y))
            if self.mode==7:self.save(DATA+2,0x20,1)
        elif target==0x29DD60:
            v=[self.load(b+4*i,4) for i in range(7)]
            self.event(5,*v)
            self.callback_calls+=1;self.r[2]=int(self.mode!=3 and (self.mode!=2 or self.callback_calls==2))
            if self.mode==5:self.single(DATA+8,33.0)
        elif target==0x29CF28:
            v=[self.load(a+4*i,4) for i in range(3)]
            q=[self.load(c+4*i,4) for i in range(3)]
            self.event(6,*v,*q,self.mode)
            self.single(d,0.25);self.f[0]=3.5
            if self.mode==6:self.single(DATA+8,44.0)
        elif target in (0x29C230,0x29C090):
            x=self.f[12];self.event(7 if target==0x29C230 else 8,word(x),0,0,0,0,0,0)
            # Deterministic controlled engine values, not libm/retail approximations.
            self.f[0]=(0.0 if x==1.0 else scalar(0x40490FDB) if x==-1.0 else 1.0) if target==0x29C230 else (1.0 if x==1.0 else 0.5)
        elif target in (CURVE,CURVE+4,CURVE+8):
            values=[self.load(a+4*i,4) for i in range(3)]
            self.event(9+(target-CURVE)//4,a,b,c,word(self.f[12]),*values)
            self.single(c,self.f[12]);self.single(c+4,scalar(values[0]));self.single(c+8,scalar(values[1]))
            if self.mode==5:self.save(INDEX,55,2)
        elif target==PROJECTION:
            left=self.load(b,4);right=self.load(c,4)
            v=[self.load(a+4*i,4) for i in range(3)]
            self.event(12,b,c,left,right,*v)
            self.callback_calls+=1
            if self.mode!=4:
                distance=100000000.0 if self.mode==3 else 10.0-self.callback_calls
                p=scalar(left) if self.mode in (1,5,6) else scalar(right) if self.mode==2 else rounded((scalar(left)+scalar(right))*0.5)
                self.single(d,distance);self.single(e,p)
            if self.mode==5:self.single(REFERENCE,-8.0)
            if self.mode==6:self.save(DATA+1,0,1)
        else:raise ValueError('unknown controlled curve call')
        for i in range(3,16):self.r[i]=0xDEADBEEF

def fixture(original,function=0,count=4,kind=9,flags=0,time=15.0,index=0,mode=0,
            search_range=0,reference_x=1.0,alias=0,force_nonnull=False):
    t=CurveTrace(original,mode)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0
    for i in range(WORDS):t.save(BUFFER+4*i,0xCCCC0000+i,4)
    stride=16
    t.save(DATA,kind,1);t.save(DATA+1,stride,1);t.save(DATA+2,flags,1);t.save(DATA+4,count,2)
    for i in range(5):
        base=DATA+8+i*stride*4
        t.single(base,float(i*10+10));t.single(base+4,float(i*2));t.single(base+8,float(i));t.single(base+12,0.0)
        if kind==16:t.save(base+7,i+1,1)
        t.single(base+0x28,1.0 if i%2==0 else 0.0);t.single(base+0x2C,0.0 if i%2==0 else 1.0);t.single(base+0x30,0.0)
    t.single(REFERENCE,reference_x);t.single(REFERENCE+4,0.0);t.single(REFERENCE+8,0.0);t.single(REFERENCE+12,2.0)
    t.single(POSITION,time);t.single(FRACTION,0.75);t.save(INDEX,index,2)
    pos=POSITION;distance=FRACTION;out=OUTPUT;ip=INDEX;first=FIRST;second=SECOND
    if alias==1:pos=FRACTION
    elif alias==2:out=DATA+12
    elif alias==3:pos=REFERENCE
    elif alias==4:distance=REFERENCE
    elif alias==5:ip=DATA+4
    elif alias==6:out=INDEX
    elif alias==7:first=second=FIRST
    if function==0:args=(DATA,index,pos,FRACTION,first,second)
    elif function==1:args=(DATA,REFERENCE,pos,distance,0,0)
    elif function==2:args=(DATA,REFERENCE,ip if index!=65535 or force_nonnull else 0,search_range&0xFFFFFFFF,pos,distance)
    elif function==3:args=(DATA,ip,pos,out,0,0)
    else:args=(DATA,ip,out,0,0,0)
    initial=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    t.r[4:10]=args;t.r[29],t.r[31]=0x80000,RETURN;t.f[12]=time
    t.run(ENTRIES[function])
    return dict(function=function,args=list(args),time=word(time),mode=mode,initial=initial,
                expected=[t.load(BUFFER+4*i,4) for i in range(WORDS)],events=t.events,
                result=word(t.f[0]) if function>=3 else t.r[2]&0xFFFFFFFF,
                instruction_count=t.instruction_count,count=count,kind=kind,flags=flags,index=index,search_range=search_range,alias=alias,force_nonnull=force_nonnull)

def fixtures(original):
    cases=[]
    for count in (1,2,4):
        for kind in (9,16):
            scale=16.0 if kind==16 else 1.0
            for flags in (0,3,12,15,32,47):
                for time in (0,10,15,20,35,40,55):
                    cases.append(fixture(original,0,count,kind,flags,time*scale,index=2 if count==4 else 0))
    for function in (3,4,5,6):
        for count in (1,2,4):
            for index in (0,1,65535):
                for mode in (0,5):cases.append(fixture(original,function,count=count,index=index,mode=mode))
    for function in (1,2):
        for count in (1,2,4):
            for mode in range(7):
                for x in (-8,0,1,8):cases.append(fixture(original,function,count=count,mode=mode,reference_x=x))
    for rng in (-1,0,1,2,100):
        for index in (0,1,3,65535):cases.append(fixture(original,2,index=index,search_range=rng,mode=1))
    for function in (0,1,2,3,4,5,6):
        for alias in ((1,3) if function==0 else (1,2,3,4,5,6)):
            cases.append(fixture(original,function,alias=alias))
    cases.append(fixture(original,0,count=1,alias=7))
    cases.append(fixture(original,0,count=4,flags=3,time=0,mode=7))
    for function in (0,3,4,5,6):
        for index in (0,1,2,3,65535):
            for time in (-45,10,20,30,40,75):
                cases.append(fixture(original,function,index=index,time=time,flags=47))
    for count in (0,1,4):
        for index in (0,1,6,65535):
            for rng in (-2147483648,-1,0,1,2147483647):
                if count==0 and rng>0 and (rng>1 or index!=0):continue
                cases.append(fixture(original,2,count=count,index=index,search_range=rng,force_nonnull=True,mode=1))
    return cases

def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:v for k,v in c.items() if k not in ('expected','events','result','instruction_count')} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()

def golden_header(cases):
    n=max(len(c['events']) for c in cases);ar=lambda v:'{'+','.join('0x%08Xu'%x for x in v)+'}'
    lines=['/* Authored finite/controlled fixtures; no original code/data arrays. */',
           'struct CurveGolden { u32 function,args[6],time,mode,result,event_count,events[%d],initial[%d],expected[%d]; };'%(n,WORDS,WORDS),
           'static const struct CurveGolden curve_golden[] = {']
    for c in cases:lines.append(' {%du,%s,0x%08Xu,%du,0x%08Xu,%du,%s,%s,%s},'%(c['function'],ar(c['args']),c['time'],c['mode'],c['result'],len(c['events']),ar(c['events']),ar(c['initial']),ar(c['expected'])))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/path_curves/trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitations':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d finite curve fixtures; %d original instructions; max%d; input%s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))

if __name__=='__main__':main()
