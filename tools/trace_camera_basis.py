"""Bounded finite camera-basis trace with explicit controlled numeric callees.

Only two reviewed bodies and their exact five-word switch table are fetched from
the locally validated ELF. Synthetic outputs contain no original code/table
bytes. Normalization reuses the reviewed finite CameraTrace call model; explicit
mutations and threshold fixtures test caller observations, not EE special/FCR/
VU/cycle behavior or a complete original engine execution.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN,rounded,scalar,word

ROOT=Path(__file__).resolve().parents[1]
BUFFER,POSE=0x20000,0x20040
RANGES=((0x299BE0,0x299D68),(0x299D68,0x29A100))
TABLE,TABLE_END=0x4455A0,0x4455B4
TABLE_SHA256='1af4994c85ca60c5868e1399b54e143dfc1e3cf03f82a7af29aa5ee36468aed3'
CALLS=(0x2A3538,0x29C168,0x29C090,0x374848,0x373250,0x372CC0,0x29B940,0x2A1098)
MASK64=0xFFFFFFFFFFFFFFFF


def double_bits(value):
    if not math.isfinite(value):raise ValueError('nonfinite controlled double')
    return struct.unpack('<Q',struct.pack('<d',value))[0]


def double_value(value):
    result=struct.unpack('<d',struct.pack('<Q',value&MASK64))[0]
    if not math.isfinite(result):raise ValueError('nonfinite controlled double')
    return result


class CameraBasisTrace(CameraTrace):
    def __init__(self,original,mutation=0,direction=0):
        # Parent normalization is reused with its unrelated mutations disabled.
        super().__init__(original,mutation=0)
        self.basis_mutation=mutation
        self.direction_case=direction
        self.calls=[0]*len(CALLS)
        self.events=[]
        self.trig_calls=0
        self.direction_pointer=self.up_pointer=None

    def fetch(self,pc):
        if pc%4 or not any(start<=pc<end for start,end in RANGES):
            raise ValueError('unreviewed camera basis instruction: %#x'%pc)
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):
            raise ValueError('camera basis instruction outside original image')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if TABLE<=address<TABLE_END:
            if size!=4 or address%4 or address+size>TABLE_END:
                raise ValueError('unsupported switch-table load')
            offset=TABLE-0xFF000
            data=self.original[offset:offset+20]
            if hashlib.sha256(data).hexdigest()!=TABLE_SHA256:
                raise ValueError('incorrect complete original switch table')
            return struct.unpack_from('<I',data,address-TABLE)[0]
        return super().load(address,size)

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,(instruction>>21)&31,(instruction>>16)&31,(instruction>>11)&31,instruction&63
        shamt=(instruction>>6)&31
        target=None
        if op==0 and fn in (0,0x38):
            if rs!=0:raise ValueError('unsupported camera shift source encoding')
            if fn==0:
                value=(self.r[rt]<<shamt)&0xFFFFFFFF
                self.r[rd]=value if value<0x80000000 else value|0xFFFFFFFF00000000
            else:self.r[rd]=(self.r[rt]<<shamt)&MASK64
        elif op==0 and fn==0x2D:
            if shamt:raise ValueError('unsupported camera DADDU encoding')
            self.r[rd]=(self.r[rs]+self.r[rt])&MASK64
        elif op==0xB:
            imm=instruction&0xFFFF
            if imm&0x8000:imm-=0x10000
            self.r[rt]=int((self.r[rs]&MASK64)<(imm&MASK64))
        elif op in (1,6):
            if (op==1 and rt!=1) or (op==6 and rt!=0):
                raise ValueError('unsupported camera signed branch encoding')
            value=self.r[rs]&MASK64
            value=value if value<0x8000000000000000 else value-(1<<64)
            taken=value>=0 if op==1 else value<=0
            if taken:
                imm=instruction&0xFFFF
                if imm&0x8000:imm-=0x10000
                target=(pc+4+(imm<<2))&0xFFFFFFFF
        else:return super().execute(instruction,pc)
        self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('instruction trace exceeded its bound')
        self.r[0]=0
        return target,False

    def event(self,index,*values):
        self.events.extend([index,*[value&0xFFFFFFFF for value in values]])

    def mutate_vector(self,pointer,values):
        for index,value in enumerate(values):self.single(pointer+index*4,value)

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown controlled camera basis call')
        index=CALLS.index(target)
        self.calls[index]+=1
        if target==0x2A3538:
            pointer=self.r[4]
            self.event(index,*[self.load(pointer+i*4,4) for i in range(3)])
            CameraTrace.library_call(self,target)
            n=self.calls[0]
            if n==1:
                self.direction_pointer=pointer
                if self.direction_case in (3,4,5):
                    self.single(pointer+4,scalar(0x3F7D70A3+self.direction_case-3))
            if n==2:self.up_pointer=pointer
            if self.basis_mutation==2:
                if n==1:
                    self.save(POSE,2,4)
                    self.mutate_vector(POSE+0x10,(4,5,6))
                    self.single(POSE+0x2C,0.5)
                elif n==2:self.mutate_vector(self.direction_pointer,(0.25,-0.5,0.75))
                elif n==3:self.mutate_vector(self.up_pointer,(2,-3,4))
            elif self.basis_mutation==3 and n==4:
                self.save(POSE,2,4)
                self.mutate_vector(POSE+0x10,(7,8,9))
            elif self.basis_mutation==5:self.f[0]=float(10+n)
        elif target in (0x29C168,0x29C090):
            angle=self.f[12]
            self.event(index,word(angle))
            self.f[0]=rounded(0.8-angle*0.05) if target==0x29C168 else rounded(0.6+angle*0.1)
            self.trig_calls+=1
            if self.basis_mutation==1:
                if self.trig_calls==1:
                    self.single(POSE+0x38,7.0);self.single(POSE+0x2C,1.75)
                elif self.trig_calls==2:self.single(POSE+0x34,-0.5)
                elif self.trig_calls==4:
                    self.single(POSE+0x30,3.0)
                    self.mutate_vector(POSE+4,(8,-9,10))
                    self.save(POSE,3,4)
        elif target==0x374848:
            self.event(index,word(self.f[12]))
            self.r[2]=double_bits(self.f[12])
        elif target in (0x373250,0x372CC0):
            a,b=self.r[4]&MASK64,self.r[5]&MASK64
            self.event(index,a,a>>32,b,b>>32)
            first,second=double_value(a),double_value(b)
            if target==0x373250:self.r[2]=((first>second)-(first<second))&MASK64
            else:self.r[2]=double_bits(first-second)
        elif target==0x29B940:
            first,second=self.f[12],self.f[13]
            self.event(index,word(first),word(second))
            self.f[0]=rounded(first+second*0.25)
            if self.basis_mutation==4:
                if self.calls[index]==1:self.mutate_vector(self.up_pointer,(0.3,0.4,0.5))
                else:self.single(POSE+0x34,-0.75)
        elif target==0x2A1098:
            values=[self.load(self.r[5]+i*4,4) for i in range(16)]
            self.event(index,*values)
            for i,value in enumerate(values):self.single(self.r[4]+i*4,-scalar(value))

    def run(self,entry):
        pc=entry
        while pc!=RETURN:
            instruction=self.fetch(pc)
            target,annul=self.execute(instruction,pc)
            if target is not None:
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):
                    raise ValueError('control transfer in camera basis delay slot')
                if instruction>>26==3:
                    self.library_call(target);pc=self.r[31]
                else:pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine,mode,mutation=0,direction=0,output=128,inverse=192):
    trace=CameraBasisTrace(original,mutation,direction)
    for start,end in ((0x20000,0x21000),(0x7F000,0x81000)):
        for address in range(start,end):trace.memory[address]=0x5A
    for i in range(64):trace.single(BUFFER+i*4,float(i%13-6))
    y=scalar(0x3F7D70A3+max(0,direction-3)) if direction in (3,4,5) else 0
    vectors=((1,0,0),(0,1,0),(0,-1,0),(math.sqrt(1-y*y),y,0),
             (math.sqrt(1-y*y),y,0),(math.sqrt(1-y*y),y,0),(0,0,0),(2,-1,3))
    delta=vectors[direction]
    trace.save(POSE,mode,4)
    for i,value in enumerate((2,-3,4)):
        trace.single(POSE+4+i*4,value)
        trace.single(POSE+0x10+i*4,rounded(value+delta[i]))
    for offset,value in ((0x2C,0.25),(0x30,5),(0x34,-0.25),(0x38,0.75)):
        trace.single(POSE+offset,value)
    initial=[trace.load(BUFFER+i*4,4) for i in range(64)]
    trace.r[4],trace.r[5],trace.r[6]=POSE,BUFFER+inverse,BUFFER+output
    trace.r[29],trace.r[31]=0x80000,RETURN
    trace.run(0x299BE0 if routine==0 else 0x299D68)
    return {'routine':routine,'mode':mode,'mutation':mutation,'direction':direction,
            'output':output,'inverse':inverse,'initial':initial,
            'expected':[trace.load(BUFFER+i*4,4) for i in range(64)],
            'calls':trace.calls,'events':trace.events,'result':trace.r[2] if routine==0 else 0,
            'instruction_count':trace.instruction_count}


def fixtures(original):
    cases=[]
    for mode in (0,1,2,3,4,5,0xFFFFFFFF):
        for mutation in (0,1):cases.append(make_fixture(original,0,mode,mutation))
    for mode in (0,1,2,3,4,0xFFFFFFFF):
        for direction in range(8):
            for mutation in range(5):cases.append(make_fixture(original,1,mode,mutation,direction))
    for output in (0,32,48,64,68,80,96,128):
        for inverse in (128,160,192):
            for direction in (0,1,7):
                for mutation in (0,3,4):
                    cases.append(make_fixture(original,1,2,mutation,direction,output,inverse))
    for direction in (1,2,7):cases.append(make_fixture(original,1,2,5,direction))
    return cases


def input_hash(cases):
    keys=('routine','mode','mutation','direction','output','inverse','initial')
    inputs=[{key:case[key] for key in keys} for case in cases]
    return hashlib.sha256(json.dumps(inputs,sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    maximum=max(len(case['events']) for case in cases)
    lines=['/* Authored finite controlled-call words; no original code/table/assets. */',
           'struct CameraBasisGolden { u32 routine,mode,mutation,direction,output,inverse,initial[64],expected[64],calls[8],result,event_count,events[%d]; };'%maximum,
           'static const struct CameraBasisGolden camera_basis_golden[] = {']
    for case in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in case[key])
        prefix=','.join('0x%08Xu'%case[k] for k in ('routine','mode','mutation','direction','output','inverse'))
        lines.append('    {%s,{%s},{%s},{%s},0x%08Xu,%d,{%s}},'%(prefix,array('initial'),array('expected'),array('calls'),case['result'],len(case['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/camera_basis_trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True)
        args.golden_header.write_text(golden_header(cases))
    print('%d controlled finite fixtures; %d instructions; maximum%d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('synthetic input SHA-256 '+input_hash(cases))


if __name__=='__main__':main()
