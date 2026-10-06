"""Scoped property-record instruction trace with authored finite call substitutes.

Reads reviewed bodies and complete switch data only from the validated local ELF.
Exports synthetic buffers/events, never original code/table bytes. Inherits the
reviewed camera decoder and adds only observed integer/CVT.S.W/JALR forms; finite
host arithmetic excludes EE special/FCR/cycle behavior.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_camera_basis import CameraBasisTrace
from trace_geometry import RETURN,rounded,scalar,word

ROOT=Path(__file__).resolve().parents[1]
BUFFER,RECORD,DESCRIPTORS=0x20000,0x20100,0x20180
CALLBACK=0xF0000080
TABLE,TABLE_END=0x4455D0,0x445610
TABLE_SHA256='1bf06df5949b582209f0b84f3e61a8045478fe9a1f2c41e83f3b72a649f48889'
RANGES=((0x29A508,0x29A888),(0x29A890,0x29A8B4),(0x29A8B8,0x29A8E4))
CALLS=(0x393B74,0x29C168,CALLBACK,0x2B2448)


def signed32(value):
    value&=0xFFFFFFFF
    return value if value<0x80000000 else value-0x100000000


class PropertyTrace(CameraBasisTrace):
    def __init__(self,original,mutation=0):
        super().__init__(original)
        self.property_mutation=mutation
        self.calls=[0]*4
        self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(start<=pc<end for start,end in RANGES):
            raise ValueError('unreviewed property-record instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('property instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if TABLE<=address<TABLE_END:
            if size!=4 or address%4 or address+size>TABLE_END:raise ValueError('invalid property switch load')
            data=self.original[TABLE-0xFF000:TABLE_END-0xFF000]
            if hashlib.sha256(data).hexdigest()!=TABLE_SHA256:raise ValueError('incorrect entire property switch')
            return struct.unpack_from('<I',data,address-TABLE)[0]
        # Do not inherit a second subsystem's readonly table permission.
        if 0x4455A0<=address<0x4455B4:raise ValueError('unowned readonly table')
        return super().load(address,size)

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,(instruction>>21)&31,(instruction>>16)&31,(instruction>>11)&31,instruction&63
        shamt=(instruction>>6)&31;imm=instruction&0xFFFF
        simm=imm-0x10000 if imm&0x8000 else imm
        target,annul=None,False
        if op in (0x14,0x15):
            taken=self.r[rs]==self.r[rt]
            if op==0x15:taken=not taken
            if taken:target=(pc+4+simm*4)&0xFFFFFFFF
            else:annul=True
        elif op==1 and rt==0:
            if signed32(self.r[rs])<0:target=(pc+4+simm*4)&0xFFFFFFFF
        elif op==0xC:self.r[rt]=self.r[rs]&imm
        elif op==0x24:self.r[rt]=self.load((self.r[rs]+simm)&0xFFFFFFFF,1)
        elif op==0x25:self.r[rt]=self.load((self.r[rs]+simm)&0xFFFFFFFF,2)
        elif op==0 and fn in (2,3):
            if rs:raise ValueError('reserved property shift source')
            value=(self.r[rt]&0xFFFFFFFF)>>shamt if fn==2 else signed32(self.r[rt])>>shamt
            self.r[rd]=value&0xFFFFFFFF
        elif op==0 and fn in (0x25,0x27,0x2B):
            if shamt:raise ValueError('reserved property integer operand')
            self.r[rd]=(self.r[rs]|self.r[rt])&0xFFFFFFFF if fn==0x25 else (~(self.r[rs]|self.r[rt]))&0xFFFFFFFF if fn==0x27 else int((self.r[rs]&0xFFFFFFFF)<(self.r[rt]&0xFFFFFFFF))
        elif op==0 and fn==0x18:
            if shamt:raise ValueError('reserved property MULT operand')
            self.r[rd]=(signed32(self.r[rs])*signed32(self.r[rt]))&0xFFFFFFFF
        elif op==0 and fn==9:
            if rt or shamt or rd!=31:raise ValueError('unsupported property JALR operand')
            target=self.r[rs]&0xFFFFFFFF;self.r[31]=pc+8
        elif op==17 and rs==20:
            if rt or fn!=0x20:raise ValueError('unsupported property COP1.W operand')
            bits=word(self.f[rd])
            if bits>255:raise ValueError('property CVT.S.W outside reviewed byte input scope')
            self.f[shamt]=float(bits)
        else:return super().execute(instruction,pc)
        self.instruction_count+=1
        if self.instruction_count>3000:raise ValueError('property trace exceeded instruction bound')
        self.r[0]=0
        return target,annul

    def bytestring(self,pointer):
        data=[]
        for i in range(128):
            value=self.load(pointer+i,1)
            if not value:return bytes(data)
            data.append(value)
        raise ValueError('unterminated controlled property string')

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown controlled property call')
        index=CALLS.index(target);self.calls[index]+=1;self.events.append(index)
        if target==0x393B74:
            self.events.extend([self.r[4]-BUFFER,self.r[5]-BUFFER])
            data=self.bytestring(self.r[5])+b'\0'
            for i,value in enumerate(data):self.save(self.r[4]+i,value,1)
            self.r[2]=self.r[4]
        elif target==0x29C168:
            self.events.append(word(self.f[12]));self.f[0]=rounded(0.8-self.f[12]*0.05)
        elif target==CALLBACK:
            if self.r[4] not in (RECORD+8,RECORD+48,RECORD+88):
                raise ValueError('incorrect property callback payload')
            self.events.append(self.load(self.r[4],4));self.r[2]=0x12345678
        else:
            value=self.r[4]&0xFFFFFFFF;self.events.append(value)
            self.r[2]=int.from_bytes(value.to_bytes(4,'little'),'big')
        if target in (0x393B74,0x29C168,CALLBACK):
            if self.property_mutation==1:self.save(DESCRIPTORS+8,64,4)
            elif self.property_mutation==2:self.save(RECORD+5,0x80,1)
            elif self.property_mutation==3:self.save(RECORD+6,80,2)

    def run(self,entry):
        pc=entry
        while pc!=RETURN:
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            if target is not None:
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('property transfer in delay slot')
                if instruction>>26==3 or instruction>>26==0 and instruction&63==9:
                    self.library_call(target);pc=self.r[31]
                else:pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,routine=0,kind=1,mutation=0,alias=0,count=1,search=0,packed=0xABCD1234,empty=0):
    trace=PropertyTrace(original,mutation)
    for start,end in ((0x20000,0x21000),(0x7F000,0x81000)):
        for address in range(start,end):trace.memory[address]=0x5A
    for i in range(128):trace.save(BUFFER+i*4,0x5A5A5A5A,4)
    for record in range(3):
        p=RECORD+record*40
        trace.save(p,0x11223344,4);trace.save(p+4,1,1)
        trace.save(p+5,0x80 if record==2 else 0,1);trace.save(p+6,40,2)
        for i,value in enumerate((1.25,-2.5,3.75,4.5,-5.25,6.5,7.75)):trace.single(p+8+i*4,value)
    if kind in (1,2,4,10,14,15,16,17,0xFFFFFFFF):trace.save(RECORD+8,packed,4)
    if kind==5:
        for i,value in enumerate(b'authored\0'):trace.save(RECORD+8+i,value,1)
    for i in range(4):
        p=DESCRIPTORS+i*24
        trace.save(p,0x11223344 if i>=search else 0x99887766,4)
        trace.save(p+4,kind if i==search else 1,4)
        trace.save(p+8,16,4);trace.save(p+12,0xC030,4);trace.save(p+16,CALLBACK,4)
        trace.save(p+20,0x76543210,4)
    if alias==1:trace.save(DESCRIPTORS+8,264,4)
    elif alias==2:trace.save(DESCRIPTORS+8,268,4)
    elif alias==3:
        trace.save(DESCRIPTORS+8,392,4)
        if kind in (3,7,8):trace.save(RECORD+8,64,4)
        elif kind in (14,15):trace.save(RECORD+8,0xA1B2C300,4)
    if routine==1:
        trace.save(RECORD+5,mutation,1);trace.save(RECORD+6,packed&65535,2)
    if routine==2:trace.save(RECORD,packed,4)
    if empty==2:trace.save(RECORD+4,0,1)
    if routine==0 and alias==0 and mutation==0 and search==0:trace.save(RECORD+5,0x80,1)
    initial=[trace.load(BUFFER+i*4,4) for i in range(128)]
    trace.r[29],trace.r[31]=0x80000,RETURN
    if routine==0:trace.r[4],trace.r[5],trace.r[6],trace.r[7]=BUFFER,0 if empty==1 else RECORD,count,DESCRIPTORS
    else:trace.r[4]=RECORD
    trace.run((0x29A508,0x29A890,0x29A8B8)[routine])
    result=trace.r[2] if routine else 0
    if routine==1 and result:result-=BUFFER
    return {'routine':routine,'kind':kind,'mutation':mutation,'alias':alias,'count':count,'search':search,
            'packed':packed,'empty':empty,'initial':initial,'expected':[trace.load(BUFFER+i*4,4) for i in range(128)],
            'result':result,'calls':trace.calls,'events':trace.events,'instruction_count':trace.instruction_count}


def fixtures(original):
    cases=[]
    for kind in (*range(18),0xFFFFFFFF):
        for count,search in ((0,0),(1,0),(4,0),(4,2),(1,3)):
            cases.append(make_fixture(original,kind=kind,count=count,search=search))
    for kind in (3,7,8,14,15):
        for alias in (1,2,3):cases.append(make_fixture(original,kind=kind,alias=alias))
    for kind in (5,12,13,16):
        for mutation in (1,2,3):cases.append(make_fixture(original,kind=kind,mutation=mutation))
    for packed in (0,1,0xFFFFFFFF,0x80010203,0xFE80FF00,0x12345678):
        for kind in (10,14,15):cases.append(make_fixture(original,kind=kind,packed=packed))
    for flags in (0,0x7F,0x80,0xFF):
        for step in (0,8,40,65535):cases.append(make_fixture(original,1,mutation=flags,packed=step))
    for packed in (0,1,0xFFFFFFFF,0x12345678,0x80000001):cases.append(make_fixture(original,2,packed=packed))
    for empty in (1,2):cases.append(make_fixture(original,empty=empty,count=0xFFFFFFFF))
    return cases


def input_hash(cases):
    keys=('routine','kind','mutation','alias','count','search','packed','empty','initial')
    data=[{k:c[k] for k in keys} for c in cases]
    return hashlib.sha256(json.dumps(data,sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    maximum=max(len(c['events']) for c in cases)
    lines=['/* Authored finite property fixture words; no original code/table/assets. */',
           'struct PropertyGolden { u32 routine,kind,mutation,alias,count,search,packed,empty,initial[128],expected[128],result,calls[4],event_count,events[%d]; };'%maximum,
           'static const struct PropertyGolden property_golden[] = {']
    for case in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in case[key])
        prefix=','.join('0x%08Xu'%case[k] for k in ('routine','kind','mutation','alias','count','search','packed','empty'))
        lines.append('    {%s,{%s},{%s},0x%08Xu,{%s},%d,{%s}},'%(prefix,array('initial'),array('expected'),case['result'],array('calls'),len(case['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68');p.add_argument('--output',type=Path,default=ROOT/'build/property_records_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d controlled property fixtures; %d instructions; maximum%d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA-256 '+input_hash(cases))


if __name__=='__main__':main()
