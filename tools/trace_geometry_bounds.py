"""Trace finite bounds and affine inverse fixtures through local original words.

Reuses the geometry tracer's bounded host-IEEE subset. Synthetic words alone
are exported; EE exceptional arithmetic, FCR effects and timing are excluded.
"""
import argparse
import json
from pathlib import Path
import struct
from analyze import validated_elf
from trace_geometry import Trace, RETURN

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2A0048,0x2A009C),(0x2A00A0,0x2A0138),
        (0x2A0138,0x2A01D8),(0x2A0248,0x2A02DC),
        (0x2A02E0,0x2A0370),(0x2A0390,0x2A040C),
        (0x2A0E20,0x2A1098))
ENTRIES=(0x2A0048,0x2A00A0,0x2A0138,0x2A0248,0x2A02E0,0x2A0390,0x2A0E20)


def signed(value):
    return value-0x100000000 if value&0x80000000 else value


class BoundsTrace(Trace):
    def fetch(self,pc):
        if pc%4 or not any(start<=pc<end for start,end in RANGES):
            raise ValueError('unreviewed bounds instruction: %#x'%pc)
        return struct.unpack_from('<I',self.original,pc-0xFF000)[0]

    def execute(self,instruction,pc):
        op=instruction>>26;rs=(instruction>>21)&31;rt=(instruction>>16)&31
        rd=(instruction>>11)&31;fn=instruction&63
        imm=instruction&65535;imm=imm-65536 if imm&32768 else imm
        extra=(op==0 and fn in (0x2A,0x2B)) or op==5 or (op==6 and rt==0) or (op==0x11 and rs==16 and fn in (0x34,0x36))
        if not extra:return super().execute(instruction,pc)
        self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('instruction trace exceeded its bound')
        target=None
        if op==0:
            left,right=self.r[rs],self.r[rt]
            if fn==0x2A:left,right=signed(left),signed(right)
            self.r[rd]=int(left<right)
        elif op==5:
            if self.r[rs]!=self.r[rt]:target=(pc+4+(imm<<2))&0xFFFFFFFF
        elif op==6:
            if signed(self.r[rs])<=0:target=(pc+4+(imm<<2))&0xFFFFFFFF
        else:
            self.condition=self.f[rd]<self.f[rt] if fn==0x34 else self.f[rd]<=self.f[rt]
        self.r[0]=0
        return target,False


def fixtures(original):
    cases=[]
    def add(routine,a,b,c,values,writes=(),parameter=0.0):
        trace=BoundsTrace(original);base=0x10000
        for i,value in enumerate(values):trace.single(base+4*i,value)
        for index,value in writes:trace.save(base+index*4,value,4)
        initial=[trace.load(base+4*i,4) for i in range(64)]
        trace.r[4],trace.r[5],trace.r[6]=base+a*4,base+b*4,base+c*4
        if routine==3:trace.r[5]=b
        if routine==5:trace.r[6]=c&0xFFFFFFFF
        trace.r[29],trace.r[31]=0x80000,RETURN;trace.f[12]=parameter
        trace.run(ENTRIES[routine])
        cases.append({'routine':routine,'a':a,'b':b,'c':c,'parameter':parameter,
            'initial':initial,'expected':[trace.load(base+4*i,4) for i in range(64)],
            'result':trace.r[2],'instruction_count':trace.instruction_count})
    for amount in (-3,0,0.5,2):
        values=[(i%13)-6 for i in range(64)]
        add(0,8,0,0,values,parameter=amount)
    for routine in (1,2):
        for shift in (0,1,3,4,6,8,12,16,20):
            values=[(i%11)-5 for i in range(64)]
            values[8:14]=[-2,-3,-4,2,3,4]
            add(routine,8,shift,0,values)
    for count in (0,1,3):
        for plane_count in (0,1,3):
            for x in (-2,0,2):
                values=[0.0]*64
                values[1:13]=[1,0,0,-1,0,1,0,-1,0,0,1,-1]
                values[24:33]=[x,0,0,0,x,0,0,0,x]
                add(3,0,count,24,values,writes=((0,plane_count),))
    for radius in (-1,0,0.5,1,2):
        for x in (-2,-1,0,1,2):
            values=[0.0]*64
            values[1:13]=[1,0,0,-1,0,1,0,-1,0,0,1,-1]
            values[24:28]=[x,0,0,radius]
            add(4,0,24,0,values,writes=((0,3),))
    for count in (-2,-1,0,1,2,3):
        for x in (-2,0,1,2):
            values=[0.0]*64
            values[0:12]=[1,0,0,1,0,1,0,1,0,0,1,1]
            values[24:27]=[x,0,0]
            add(5,24,0,count,values)
    for output in (0,1,4,8,9,12,15,16,17,20,24,32):
        for diagonal in ((1,1,1),(2,4,8),(0,0,0)):
            values=[(i%9)-4 for i in range(64)]
            values[8:24]=[diagonal[0],0,0,0,0,diagonal[1],0,0,0,0,diagonal[2],0,2,-3,4,1]
            add(6,output,8,0,values)
    return cases


def golden_header(cases):
    lines=['/* Finite instruction-derived synthetic fixtures; no original code. */',
        'struct BoundsGolden { int routine,a,b,c; float parameter; u32 initial[64],expected[64],result; };',
        'static const struct BoundsGolden bounds_golden[] = {']
    for c in cases:
        words=lambda name:','.join('0x%08Xu'%x for x in c[name])
        lines.append('    {%d,%d,%d,%d,%.8ff,{%s},{%s},0x%08Xu},'%(c['routine'],c['a'],c['b'],c['c'],c['parameter'],words('initial'),words('expected'),c['result']))
    return '\n'.join(lines+['};',''])


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/bounds_trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps({'limitation':__doc__,'cases':cases},indent=2)+'\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True)
        args.golden_header.write_text(golden_header(cases))
    print('%d finite original-instruction bounds fixtures'%len(cases))


if __name__=='__main__':main()
