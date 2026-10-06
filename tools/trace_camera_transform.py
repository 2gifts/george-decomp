"""Finite controlled-call fixtures for the five reviewed camera transform bodies.

Reuses CameraTrace's strict bounded instruction decoder unchanged. The transform
cache, rotation construction, inverse and trig calls are controlled substitutes;
no full EE engine/FCR/special/timing behavior is claimed. Only synthetic words
are exported, with original instructions read from the locally validated ELF.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_motion import CameraTrace
from trace_geometry import RETURN,rounded,scalar,word

ROOT=Path(__file__).resolve().parents[1]
BUFFER,TRANSFORM=0x20000,0x20040
RANGES=((0x29A100,0x29A168),(0x29A188,0x29A260),(0x29A308,0x29A3A0),
        (0x29A3A0,0x29A498),(0x29A498,0x29A504))
CALLS=(0x2AEC28,0x299BE0,0x2A1E78,0x2A0E20,0x299D68,0x29C168,0x29C090)


class CameraTransformTrace(CameraTrace):
    def __init__(self,original,mutation=0,rebuild=1,allocation_fail=0):
        super().__init__(original)
        self.mutation=mutation
        self.rebuild=rebuild
        self.allocation_fail=allocation_fail
        self.calls=[0]*len(CALLS)
        self.angle=None

    def fetch(self,pc):
        if pc%4 or not any(start<=pc<end for start,end in RANGES):
            raise ValueError('unreviewed camera transform instruction: %#x'%pc)
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):
            raise ValueError('camera transform instruction outside original image')
        return struct.unpack_from('<I',self.original,offset)[0]

    def library_call(self,target):
        if target not in CALLS:
            raise ValueError('unreviewed controlled transform call: %#x'%target)
        self.calls[CALLS.index(target)]+=1
        if target==0x2AEC28:
            if self.r[4]!=0x68:raise ValueError('incorrect transform allocation size')
            self.r[2]=0 if self.allocation_fail else TRANSFORM
            return
        if target==0x299BE0:
            if self.r[4]!=TRANSFORM:raise ValueError('uncaptured transform cache argument')
            if self.mutation==1:
                for offset,value in ((0x10,7),(0x14,8),(0x18,9)):
                    self.single(TRANSFORM+offset,value)
                for offset in (0x50,0x64):self.save(TRANSFORM+offset,0x12345678,4)
            self.r[2]=self.rebuild
        elif target==0x2A1E78:
            if self.r[5]!=TRANSFORM+0x1C:raise ValueError('incorrect transform rotation argument')
            for index in range(16):self.single(self.r[4]+index*4,float(index+1))
        elif target==0x2A0E20:
            values=[scalar(self.load(self.r[5]+index*4,4)) for index in range(16)]
            for index,value in enumerate(values):self.single(self.r[4]+index*4,-value)
        elif target==0x299D68:
            if self.r[4]!=TRANSFORM:raise ValueError('incorrect fallback transform')
            for index in range(16):self.single(self.r[6]+index*4,float(index+21))
            for index in range(16):self.single(self.r[5]+index*4,float(-index-21))
        elif target==0x29C168:
            self.angle=self.f[12]
            self.f[0]=1.0
            if self.mutation==1:
                for offset,value in ((0x3C,9),(0x40,90),(0x44,17)):
                    self.single(TRANSFORM+offset,value)
        elif target==0x29C090:
            if self.angle is None or word(self.f[12])!=word(self.angle):
                raise ValueError('uncaptured projection angle')
            self.f[0]=0.5
            if self.mutation==2:self.single(TRANSFORM+0x48,3.0)

    def run(self,entry):
        pc=entry
        while pc!=RETURN:
            target,annul=self.execute(self.fetch(pc),pc)
            if target is not None:
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):
                    raise ValueError('control transfer in transform delay slot')
                if target in CALLS:
                    self.library_call(target)
                    pc=self.r[31]
                else:pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine,mutation=0,rebuild=1,output=128,inverse=192,delta=0,allocation_fail=0):
    trace=CameraTransformTrace(original,mutation,rebuild,allocation_fail)
    for start,end in ((0x20000,0x21000),(0x7F000,0x81000)):
        for address in range(start,end):trace.memory[address]=0x5A
    for index in range(64):trace.single(BUFFER+index*4,float(index%13-6))
    for offset,value in ((0,0),(4,3),(8,-2),(12,5),(0x10,7),(0x14,8),(0x18,9),
                         (0x30,1000),(0x34,-0.25),(0x38,0.75),(0x3C,1),
                         (0x40,4000),(0x44,34.51599884033203),(0x48,1.3333330154418945)):
        trace.single(TRANSFORM+offset,float(value))
    initial=[trace.load(BUFFER+index*4,4) for index in range(64)]
    entries=(0x29A100,0x29A188,0x29A308,0x29A3A0,0x29A498)
    trace.r[4],trace.r[29],trace.r[31]=TRANSFORM,0x80000,RETURN
    trace.f[12],trace.f[13],trace.f[14]=3,-2,5
    if routine==2:trace.r[5],trace.r[6]=BUFFER+inverse,BUFFER+output
    if routine==3:trace.r[5]=BUFFER+output
    if routine==4:trace.r[5]=TRANSFORM+delta
    trace.run(entries[routine])
    return {'routine':routine,'mutation':mutation,'rebuild':rebuild,'output':output,'inverse':inverse,'delta':delta,
            'allocation_fail':allocation_fail,'initial':initial,'expected':[trace.load(BUFFER+index*4,4) for index in range(64)],
            'result':trace.r[2] if routine==0 else 0,'calls':trace.calls,'instruction_count':trace.instruction_count}


def fixtures(original):
    cases=[]
    for routine in (0,1):
        for mutation in (0,1):
            for failed in ((0,1) if routine==0 else (0,)):
                cases.append(make_fixture(original,routine,mutation,allocation_fail=failed))
    for output in (0,32,48,64,68,72,80,96,112,128):
        for inverse in (128,160,192):
            for rebuild in (0,1):
                for mutation in (0,1):cases.append(make_fixture(original,2,mutation,rebuild,output,inverse))
    for output in (0,32,48,64,68,72,80,96,112,128):
        for mutation in (0,1,2):cases.append(make_fixture(original,3,mutation,output=output))
    for delta in (0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76):
        cases.append(make_fixture(original,4,delta=delta))
    return cases


def input_hash(cases):
    inputs=[{key:case[key] for key in ('routine','mutation','rebuild','output','inverse','delta','allocation_fail','initial')}
            for case in cases]
    return hashlib.sha256(json.dumps(inputs,sort_keys=True,separators=(',',':')).encode('utf-8')).hexdigest()


def golden_header(cases):
    lines=['/* Synthetic finite controlled-call fixture words; no original code/assets. */',
           'struct CameraTransformGolden { u32 routine,mutation,rebuild,output,inverse,delta,allocation_fail,initial[64],expected[64],result,calls[7]; };',
           'static const struct CameraTransformGolden camera_transform_golden[] = {']
    for case in cases:
        array=lambda key:','.join('0x%08Xu'%value for value in case[key])
        prefix=','.join(str(case[key]) for key in ('routine','mutation','rebuild','output','inverse','delta','allocation_fail'))
        lines.append('    {%s,{%s},{%s},0x%08Xu,{%s}},'%(prefix,array('initial'),array('expected'),case['result'],array('calls')))
    return '\n'.join(lines+['};',''])


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    parser.add_argument('--output',type=Path,default=ROOT/'build/camera_transform_trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(cases))
    print('%d finite controlled-call fixtures; %d instructions; maximum%d'%(len(cases),sum(case['instruction_count'] for case in cases),max(case['instruction_count'] for case in cases)))
    print('synthetic input SHA-256 '+input_hash(cases))


if __name__=='__main__':main()
