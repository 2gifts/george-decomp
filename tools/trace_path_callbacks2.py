"""Finite normal/zero fixtures for five original table-backed curve callbacks.

All selected bodies and the published quaternion interpolation helper execute
validated original instructions. ACC is initialized only by original MULA.S;
MADDA.S adds a rounded product and MADD.S writes its sum without changing ACC.
This is an independently expressed, bounded host-binary32 operation model,
not an EE/FCR/exceptional/subnormal/precision-quirk emulator. Trigonometric
calls retain authored finite observation/mutation contracts. Only synthetic
memory and observations are exported; no original code/data arrays.
"""
import argparse,hashlib,json,struct,math
from pathlib import Path
import trace_path_callbacks as parent
from trace_geometry import RETURN,rounded,scalar,word,is_control_transfer
from analyze import validated_elf
ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2C19F0,0x2C1AD0),(0x2C1CA0,0x2C1D5C),(0x2C1D60,0x2C1E20),(0x2C2180,0x2C21AC),(0x2C2390,0x2C23BC))
HELPERS=parent.HELPERS
ENTRIES=tuple(a for a,b in RANGES)
class AccTrace(parent.CallbackTrace):
    def __init__(self,original,mode=0):
        super().__init__(original,mode)
        self.accumulator=None
    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+HELPERS):raise ValueError('unreviewed accumulator instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('accumulator instruction outside original')
        return struct.unpack_from('<I',self.original,off)[0]
    @staticmethod
    def normal(value):
        bits=word(value)
        if not math.isfinite(value) or bits&0x7FFFFFFF and not bits&0x7F800000:raise ValueError('outside normal/zero accumulator domain')
        return value
    def execute(self,w,pc):
        if w>>26==17 and w>>21&31==16 and w&63 in (26,28,30):
            ft,fs,fd,fn=w>>16&31,w>>11&31,w>>6&31,w&63
            if fn in (26,30) and fd:raise ValueError('reserved accumulator destination')
            product=self.normal(rounded(self.normal(self.f[fs])*self.normal(self.f[ft])))
            if fn==26:self.accumulator=product
            else:
                if self.accumulator is None:raise ValueError('uninitialized accumulator')
                result=self.normal(rounded(self.accumulator+product))
                if fn==30:self.accumulator=result
                else:self.f[fd]=result
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('accumulator instruction bound exceeded')
            return None,False
        op,fmt=w>>26,w>>21&31
        if op==17 and fmt==16:
            fs,ft,fn=w>>11&31,w>>16&31,w&63
            if fn in (0,1,2,3,0x29,0x32,0x34,0x36):
                self.normal(self.f[fs]);self.normal(self.f[ft])
            elif fn in (6,7):self.normal(self.f[fs])
            elif fn==4:self.normal(self.f[ft])
        result=super().execute(w,pc)
        if op==0x31:self.normal(self.f[w>>16&31])
        elif op==17 and fmt==16 and w&63 in (0,1,2,3,4,6,7,0x29):self.normal(self.f[w>>6&31])
        return result
    def library_call(self,target):
        if target not in (0x29C230,0x29C090):raise ValueError('unknown accumulator controlled call')
        return super().library_call(target)

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



def fixture(raw,function,time,output,pattern,mode):
    t=AccTrace(raw,mode)
    for a,b in parent.MEMORY_RANGES:
        for q in range(a,b):t.memory[q]=0
    for i in range(parent.WORDS):t.single(parent.BUFFER+4*i,float((i%23)-11)*0.25)
    seed=pattern+19
    for base,k in ((parent.FIRST,1),(parent.SECOND,-1)):
        for i in range(16):
            seed=(1664525*seed+1013904223)&0xFFFFFFFF
            t.single(base+4*i,float((seed>>16)%201-100)/7.0 if pattern==2 else float(k*((i%5)-2))*0.25)
        t.single(base,10.0 if k==1 else 22.0)
    if function in (3,4):
        shift=4 if function==3 else 24
        quats=(((0,0,0,1),(0,0,0,1)),((0,0,1,0),(0,0,-1,0)),((0,1,0,0),(1,0,0,0)))
        for base,values in zip((parent.FIRST,parent.SECOND),quats[pattern]):
            for i,v in enumerate(values):t.single(base+shift+4*i,float(v))
    args=[parent.FIRST,parent.SECOND,output,0,0]
    initial=[t.load(parent.BUFFER+4*i,4) for i in range(parent.WORDS)]
    for i,a in enumerate(args):t.r[4+i]=a
    t.r[29],t.r[31]=0x80000,RETURN;t.f[12]=time
    t.run(ENTRIES[function])
    return dict(function=function,time=word(time),args=args,mode=mode,initial=initial,
                expected=[t.load(parent.BUFFER+4*i,4) for i in range(parent.WORDS)],
                events=t.events,event_count=len(t.events),instruction_count=t.instruction_count)

def fixtures(raw):
    times=(-1.0,-0.25,0.0,0.25,0.5,0.75,1.0,1.5)
    outputs=(parent.OUTPUT,parent.FIRST,parent.FIRST+4,parent.FIRST+12,parent.FIRST+24,
             parent.SECOND,parent.SECOND+4,parent.SECOND+12,parent.FIRST+28)
    return [fixture(raw,fn,t,out,pattern,mode) for fn in range(5) for t in times
            for out in outputs for pattern in range(3) for mode in ((0,1) if fn>=3 else (0,))]

def golden_header(cases):
    text=parent.golden_header(cases)
    return text.replace('CallbackGolden','AccumulatorGolden').replace('callback_golden','accumulator_golden')

def input_hash(cases):
    return parent.input_hash(cases)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output',type=Path,default=ROOT/'build/path_callbacks2/trace.json')
    p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps({'limitations':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d finite accumulator fixtures; %d original instructions; max%d; input%s' %
          (len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))

if __name__=='__main__':main()
