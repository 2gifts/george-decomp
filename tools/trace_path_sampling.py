"""Bounded finite path-helper fixtures with genuine selected and inverse code.

The five selected bodies and published rigid inverse execute their original
instructions. Curve evaluation/projection and the VU point transform have
explicit authored contracts. This is not curve, VU exceptional/FCR, allocator,
full runtime or cycle validation. No original code/data arrays are exported.
"""
import argparse, hashlib, json, struct
from pathlib import Path
from analyze import validated_elf
from trace_resource_base import ResourceBaseTrace
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN, rounded, scalar, word, is_control_transfer

ROOT = Path(__file__).resolve().parents[1]
RANGES = ((0x135D10,0x135D88),(0x135D88,0x135E88),
          (0x135E88,0x135F8C),(0x135F90,0x13601C),(0x136020,0x136078))
HELPER = (0x2A1098,0x2A11A4)
ENTRIES = tuple(a for a,b in RANGES)
CALLS = (0x2A1C60,0x2966F0,0x297178)
BUFFER,END = 0x20000,0x20800
PATH,OWNER,OTHER_OWNER,DATA,OTHER_DATA = 0x20040,0x20100,0x20300,0x20500,0x20600
OUTPUT,REFERENCE = 0x20090,0x200B0
WORDS = (END-BUFFER)//4
MEMORY_RANGES = ((BUFFER,END),(0x7F000,0x81000))

class PathTrace(ResourceBaseTrace):
    def __init__(self, original, mode=0):
        super().__init__(original)
        self.mode,self.events,self.matrix_calls = mode,[],0

    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('path memory outside aligned authored scope')
        if any(address+i not in self.memory for i in range(size)):
            raise ValueError('uninitialized path memory')

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+(HELPER,)):
            raise ValueError('unreviewed path instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('path instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,instruction>>21&31,instruction>>16&31,instruction>>11&31,instruction&63
        if op==0 and fn==9:raise ValueError('unowned path JALR')
        if op==0x11 and rs==20:
            if fn!=32 or rt:raise ValueError('unsupported path CVT.S.W operands')
            bits=word(self.f[rd]);signed=bits-0x100000000 if bits&0x80000000 else bits
            self.f[instruction>>6&31]=rounded(float(signed))
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('path instruction bound exceeded')
            return None,False
        if op in (0x31,0x39,0x11):
            if op==0x11 and not (rs in (0,4,8) or rs==16 and fn in (0,1,2,3,6,7,0x32)):
                raise ValueError('unsupported path COP1 opcode')
            if op==0x11 and rs==16 and fn==3 and self.f[rt]==0.0:
                raise ValueError('path zero denominator outside finite fixtures')
            return CameraTrace.execute(self,instruction,pc)
        return super().execute(instruction,pc)

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES+(HELPER,) if r[0]==entry),None)
        if body is None:raise ValueError('unsupported path entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body path transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26;branch=op in (4,5,6,20,21) or op==0x11 and instruction>>21&31==8
            call=op==3
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('path invocation must return by actual JR31 to selected stop')
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('path delay outside complete body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay):raise ValueError('path transfer in delay')
                if self.execute(delay,pc+4)!=(None,False):raise ValueError('path transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif call:
                    continuation=self.r[31]
                    if target in ENTRIES or target==HELPER[0]:self.run(target,continuation)
                    else:self.library_call(target)
                    if self.r[31]!=continuation:raise ValueError('path call corrupted link register')
                    pc=continuation
                else:
                    if not body[0]<=target<body[1]:raise ValueError('path local transfer outside owned body')
                    pc=target
            else:pc+=8 if annul else 4

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown controlled path call')
        a,b,c,d,e,f=[self.r[i]&0xFFFFFFFF for i in range(4,10)]
        if target==0x2A1C60:
            inputs=[self.load(b+4*i,4) for i in range(3)]
            matrix=[self.load(a+4*i,4) for i in range(16)]
            self.events.extend((0,*inputs,*matrix[12:15],self.load(PATH+0x0C,4)))
            values=[scalar(v) for v in inputs];m=[scalar(v) for v in matrix]
            result=[rounded(rounded(rounded(rounded(m[i]*values[0])+rounded(m[4+i]*values[1]))+rounded(m[8+i]*values[2]))+m[12+i]) for i in range(3)]
            # The actual VU body captures inputs/basis before Z, X, Y stores.
            for i in (2,0,1):self.single(c+4*i,result[i])
            self.matrix_calls+=1
            if self.matrix_calls==1 and self.mode==2:self.save(PATH+0x34,OTHER_DATA,4)
            if self.matrix_calls==1 and self.mode==6:self.save(PATH+0x0C,OTHER_OWNER,4)
        elif target==0x2966F0:
            if c or d:raise ValueError('unsupported selected projection index/count')
            inputs=[self.load(b+4*i,4) for i in range(3)]
            self.events.extend((1,*inputs,a,c,d,self.load(PATH+0x44,4)))
            self.single(f,100000000.0);self.single(e,0.0)
            self.single(e,25.0);self.single(f,3.5)
            if self.mode==3:self.single(PATH+0x44,-1.0)
            if self.mode==4:
                self.save(PATH+0x34,OTHER_DATA,4);self.save(PATH+0x0C,OTHER_OWNER,4)
            if self.mode==5:
                self.single(PATH+0x48,50.0);self.single(PATH+0x44,7.0)
            self.r[2]=int(self.mode!=7)
        else:
            position=self.load(c,4);index=self.load(b,2);fmt=self.load(a,1)
            self.events.extend((2,position,index,a,self.load(PATH+0x0C,4),fmt,self.load(PATH+0x44,4),self.load(PATH+0x48,4)))
            self.save(b,(index+3)&65535,2)
            self.single(c,rounded(scalar(position)+0.25))
            self.single(d,scalar(position));self.single(d+4,float((index&7)-3));self.single(d+8,3.0 if fmt==16 else 4.0)
            if self.mode==1:self.save(PATH+0x0C,OTHER_OWNER,4)
            self.f[0]=0.125
        for i in range(3,16):self.r[i]=0xDEADBEEF

def fixture(original,function,format=0,count=3,stride=4,current=-1.0,fraction=0.5,mode=0,output=OUTPUT,reference=REFERENCE,distance_output=None,position_arg=None,index_arg=None):
    t=PathTrace(original,mode)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0
    for i in range(WORDS):t.save(BUFFER+i*4,0xCCCC0000+i,4)
    t.save(PATH+0x0C,OWNER,4);t.save(PATH+0x34,DATA,4);t.save(PATH+0x38,0xFFFE,2)
    t.single(PATH+0x44,current);t.single(PATH+0x48,12.0)
    for address,translation in ((OWNER,(2.0,-3.0,4.0)),(OTHER_OWNER,(-4.0,5.0,2.0))):
        for i,v in enumerate((1,0,0,0,0,1,0,0,0,0,1,0,*translation,1)):t.single(address+0x50+4*i,v)
    for data,fmt in ((DATA,format),(OTHER_DATA,16 if format==0 else 0)):
        t.save(data,fmt,1);t.save(data+1,stride,1);t.save(data+4,count,2)
        t.single(data+8,12.5);t.save(data+15,3,1)
        offset=(((count-1)&0xFFFFFFFF)*stride*4)&0xFFFFFFFF
        t.single((data+offset+8)&0xFFFFFFFF,27.5);t.save((data+offset+15)&0xFFFFFFFF,8,1)
    for i,v in enumerate((5.0,6.0,-7.0)):t.single(REFERENCE+4*i,v)
    if distance_output is None:distance_output=output+4
    if position_arg is None:position_arg=PATH+0x44
    if index_arg is None:index_arg=PATH+0x38
    initial=[t.load(BUFFER+4*i,4) for i in range(WORDS)]
    args=(PATH,output,reference,0) if function==0 else (PATH,reference,0,0) if function==1 else (PATH,output,0,0) if function==2 else (PATH,reference,output,distance_output) if function==3 else (PATH,position_arg,index_arg,output)
    t.r[4:8]=args;t.f[12]=fraction;t.r[29],t.r[31]=0x80000,RETURN
    t.run(ENTRIES[function])
    return {'function':function,'args':list(args),'mode':mode,'fraction':word(fraction),'initial':initial,
            'expected':[t.load(BUFFER+4*i,4) for i in range(WORDS)],'events':t.events,
            'result':word(t.f[0]) if function==1 else t.r[2]&0xFFFFFFFF if function==3 else 0,
            'instruction_count':t.instruction_count,'format':format,'count':count,'stride':stride,'current':word(current),'output':output,'reference':reference,'distance_output':distance_output}

def fixtures(original):
    cases=[]
    for fn in range(5):
        for fmt in (0,16):
            for current in (-1.0,0.0,10.0,25.0):
                for mode in range(8):cases.append(fixture(original,fn,fmt,current=current,mode=mode))
    for fn in (0,2,4):
        for fmt in (0,16):
            for output in (PATH+0x34,PATH+0x44,REFERENCE,OWNER+0x58):
                cases.append(fixture(original,fn,fmt,output=output,mode=1))
    for fn in (0,1,3):
        for reference in (PATH+0x44,OWNER+0x80):cases.append(fixture(original,fn,reference=reference))
    for fmt in (0,16):
        for output in (PATH+0x44,PATH+0x0C,REFERENCE):
            for shared in (False,True):cases.append(fixture(original,3,fmt,output=output,distance_output=output if shared else output+4))
    for fmt in (0,16):
        for count in (0,1,2,4):
            for stride in (1,3,5):
                for fn in (1,2):
                    if fn==1 and count==1:continue
                    cases.append(fixture(original,fn,fmt,count=count,stride=stride,current=10.0,fraction=-0.25))
    for fraction in (-2.0,-0.0,0.0,1.0,2.5):
        for fmt in (0,16):cases.append(fixture(original,2,fmt,fraction=fraction))
    for position,index in ((PATH+0x38,PATH+0x38),(PATH+0x44,PATH+0x44),(DATA+8,DATA+4),(DATA+8,DATA)):
        for mode in (0,1):cases.append(fixture(original,4,position_arg=position,index_arg=index,mode=mode))
    return cases

def input_hash(cases):
    keys=('function','args','mode','fraction','initial','format','count','stride','current','output','reference','distance_output')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()

def golden_header(cases):
    n=max(len(c['events']) for c in cases)
    lines=['/* Authored finite/control fixtures only; no original code/data arrays. */',
           'struct PathGolden { u32 function,args[4],mode,fraction,result,event_count,events[%d],initial[%d],expected[%d]; };'%(n,WORDS,WORDS),
           'static const struct PathGolden path_golden[] = {']
    ar=lambda a:'{'+','.join('0x%08Xu'%v for v in a)+'}'
    for c in cases:
        lines.append(' {%du,%s,%du,0x%08Xu,0x%08Xu,%du,%s,%s,%s},'%(c['function'],ar(c['args']),c['mode'],c['fraction'],c['result'],len(c['events']),ar(c['events']),ar(c['initial']),ar(c['expected'])))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/path_sampling/trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitations':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(cases))
    print('%d finite path cases; %d original instructions; max%d; input%s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))

if __name__=='__main__':main()
