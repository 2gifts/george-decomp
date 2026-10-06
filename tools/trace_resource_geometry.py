"""Bounded resource fixtures executing six complete validated retail bodies.

Only authored buffers and controlled engine-call substitutes are exported.
Neutral holder prefixes have a byte at +2 and a payload pointer at +8; this
does not identify their complete classes. All referenced pointers are valid,
non-null and backed by authored storage. Floating inputs are finite. Models
test caller captures/reloads, not substituted engine behavior or EE hardware.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_property_records import PropertyTrace, signed32
from trace_geometry import Trace, RETURN, word, scalar, rounded

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x20400
RECORD,OTHER,OTHER2=0x20040,0x20080,0x200C0
HOLDER,ALTHOLDER=0x20100,0x20120
PAYLOAD,PAYLOAD2=0x20180,0x20200
MATRIX,FACES,KEY=0x20280,0x20300,0x20380
TABLE,TABLE_END=0x43A408,0x43A418
TABLE_SHA256='c8a486a185341df4e5be1e23f5bae29bfb0860322bbc2ba72da61941e8e2fbc5'
RANGES=((0x20E920,0x20EAA0),(0x20EAA0,0x20EC04),(0x20EC10,0x20EC78),
        (0x20EC78,0x20ECA0),(0x20ECA0,0x20ED70),(0x20EEC8,0x20EF60))
ENTRIES=tuple(r[0] for r in RANGES)
CALLS=(0x225E80,0x2AEE60,0x226BC0,0x224678,0x226D78,0x226FB0,0x226E38,
       0x2B62E0,0x2B6308,0x2867E8,0x20F6F8,0x21C508,0x2A1C60,0x29FEF8,
       0x2A48F0,0x2A4808,0x20F070)
PARAMETERS=('routine','mode','flags','count','found','other_flags','payload_flags',
            'notify','mutation','classification','result','query_null','callback_result')


class ResourceTrace(PropertyTrace):
    def __init__(self,original,options=None):
        super().__init__(original)
        self.options=options or {}
        self.calls=[0]*len(CALLS);self.events=[]

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed resource instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('resource instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def load(self,address,size):
        if TABLE<=address<TABLE_END:
            if size not in (2,4) or address%size or address+size>TABLE_END:raise ValueError('invalid resource vtable load')
            data=self.original[TABLE-0xFF000:TABLE_END-0xFF000]
            if hashlib.sha256(data).hexdigest()!=TABLE_SHA256:raise ValueError('incorrect whole resource vtable')
            return int.from_bytes(data[address-TABLE:address-TABLE+size],'little')
        if not any(a<=address and address+size<=b for a,b in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('resource load outside authored memory')
        return Trace.load(self,address,size)

    def save(self,address,value,size):
        if not any(a<=address and address+size<=b for a,b in ((BUFFER,END),(0x7F000,0x81000))):
            raise ValueError('resource store outside authored memory')
        Trace.save(self,address,value,size)

    def execute(self,instruction,pc):
        op,rs,rt=instruction>>26,(instruction>>21)&31,(instruction>>16)&31
        imm=instruction&65535;simm=imm-65536 if imm&32768 else imm
        if op in (0x28,0x29,0x21,0xA):
            address=(self.r[rs]+simm)&0xFFFFFFFF
            if op in (0x28,0x29):self.save(address,self.r[rt],1 if op==0x28 else 2)
            elif op==0x21:
                value=self.load(address,2);self.r[rt]=(value-65536 if value&32768 else value)&0xFFFFFFFF
            else:self.r[rt]=int(signed32(self.r[rs])<simm)
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('resource trace exceeded bound')
            return None,False
        return super().execute(instruction,pc)

    def log(self,target,*args):
        if target not in CALLS:raise ValueError('unknown resource controlled call')
        index=CALLS.index(target);self.calls[index]+=1;self.events.extend((index,*args))

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown resource controlled call')
        a,b,c,d,e=(self.r[i]&0xFFFFFFFF for i in range(4,9))
        m=self.options.get('mutation',0)
        if target==0x225E80:
            if (a,b)!=(KEY,1):raise ValueError('invalid resource lookup ABI')
            self.log(target,a,b);self.r[2]=RECORD if self.options['found'] else 0
        elif target==0x2AEE60:
            if a!=40:raise ValueError('invalid resource allocation size')
            self.log(target,a);self.r[2]=RECORD
        elif target==0x226BC0:
            if (a,b,c)!=(RECORD,KEY,1):raise ValueError('invalid resource constructor ABI')
            self.log(target,a,b,c,d,e);self.save(RECORD+2,self.options['flags'],1)
        elif target==0x224678:
            if (a,b,c)!=(1,0,RECORD):raise ValueError('invalid resource mode request ABI')
            self.log(target,a,b,c)
        elif target in (0x226D78,0x226FB0):
            if a!=RECORD:raise ValueError('invalid resource record callback')
            self.log(target,a)
            if target==0x226FB0 and m==5:self.save(RECORD+2,8,1)
        elif target==0x226E38:
            if (a,b,c,d)!=(OTHER,0x20EEC8,RECORD,RECORD):raise ValueError('invalid resource deferred callback ABI')
            self.log(target,a,b,c,d)
            if m==8:
                link=self.r[31];self.r[4]=self.options['callback_result'];self.r[5]=RECORD
                self.run(0x20EEC8,link)
                if self.r[31]!=link:raise ValueError('resource callback link was not restored')
        elif target in (0x2B62E0,0x2B6308,0x21C508):
            if a not in (HOLDER,ALTHOLDER):raise ValueError('invalid neutral holder callback')
            self.log(target,a,self.load(a+8,4),self.load(RECORD+2,1))
            if target==0x2B62E0 and m==1:
                self.save(RECORD+0x10,ALTHOLDER,4);self.save(RECORD+2,0x80,1);self.save(RECORD+0x24,OTHER2,4)
            if target==0x2B6308 and m==2:
                self.save(a+8,PAYLOAD2,4);self.save(RECORD+2,1,1);self.save(RECORD+0x10,ALTHOLDER,4);self.save(RECORD+0x24,OTHER2,4)
        elif target==0x2867E8:
            if a not in (HOLDER,ALTHOLDER):raise ValueError('invalid payload handler holder')
            self.log(target,a,b)
            if m==3:self.save(a+2,0,1);self.save(RECORD+2,8,1)
        elif target==0x20F6F8:
            if a not in (PAYLOAD,PAYLOAD2) or b!=0 or c!=0x12345678:raise ValueError('invalid retained resource query ABI')
            self.log(target,a,b,c)
            if m==4:self.save(RECORD+0x10,ALTHOLDER,4)
            self.r[2]=0 if self.options['query_null'] else OTHER
        elif target==0x20F070:
            if (a,b)!=(RECORD,3):raise ValueError('invalid observed adjusted virtual pair ABI')
            self.log(target,a,b)
        elif target==0x2A1C60:
            if (a,b)!=(MATRIX,PAYLOAD+0x30) or not 0x7F000<=c<=0x81000-16:raise ValueError('invalid sphere transform ABI')
            source=[self.load(b+i*4,4) for i in range(4)]
            self.log(target,a,b,*source)
            for i in range(3):self.single(c+i*4,rounded(scalar(source[i])+10*(i+1)))
            if m==6:self.single(PAYLOAD+0x3C,99);self.save(RECORD+0x10,ALTHOLDER,4);self.save(HOLDER+8,PAYLOAD2,4)
        elif target==0x2A48F0:
            if a!=FACES or not 0x7F000<=b<=0x81000-16:raise ValueError('invalid sphere classifier ABI')
            self.log(target,a,*(self.load(b+i*4,4) for i in range(4)));self.r[2]=self.options['classification']
            if m==7:self.save(RECORD+0x10,ALTHOLDER,4);self.save(HOLDER+8,PAYLOAD2,4);self.save(PAYLOAD+0x40,0x87654321,4)
        elif target==0x29FEF8:
            if (b,c)!=(PAYLOAD+0x40,MATRIX) or not 0x7F000<=a<=0x81000-64:raise ValueError('invalid frame constructor ABI')
            self.log(target,b,c,self.load(b,4))
            for i in range(16):self.save(a+i*4,0x11000000+i,4)
        elif target==0x2A4808:
            if a!=FACES or not 0x7F000<=b<=0x81000-64:raise ValueError('invalid frame classifier ABI')
            self.log(target,a,*(self.load(b+i*4,4) for i in range(16)));self.r[2]=self.options['result']

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported resource entry')
        pc=entry
        while pc!=stop:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body resource transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26
            is_return=op==0 and instruction&63==8
            if is_return and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unreviewed resource return')
            # Ordinary untaken branches still have a delay instruction. Check
            # it explicitly, rather than allowing the next loop to transfer.
            branch=op in (1,4,5,6,7,0x14,0x15)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('resource delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('resource transfer in delay slot')
                if target is None:pc+=8
                elif op==3 or op==0 and instruction&63==9:
                    self.library_call(target);pc=self.r[31]
                else:
                    if not body[0]<=target<body[1] and (instruction!=0x03E00008 or target!=stop):
                        raise ValueError('unreviewed resource return or transfer')
                    pc=target
            else:pc+=8 if annul else 4


def make_fixture(original,**kwargs):
    options=dict(routine=0,mode=0,flags=0,count=1,found=1,other_flags=0x20,payload_flags=0x88000,
                 notify=1,mutation=0,classification=1,result=0xABCD1234,query_null=0,callback_result=0)
    options.update(kwargs);t=ResourceTrace(original,options)
    for a,b in ((BUFFER,END),(0x7F000,0x81000)):
        for address in range(a,b):t.memory[address]=0x5A
    for record,payload in ((RECORD,HOLDER),(OTHER,PAYLOAD),(OTHER2,PAYLOAD2)):
        t.save(record,options['count'],2);t.save(record+2,options['flags'] if record==RECORD else options['other_flags'],1)
        t.save(record+0x10,payload,4);t.save(record+0x1C,0x12345678,4);t.save(record+0x20,TABLE,4);t.save(record+0x24,OTHER,4)
    for holder,payload in ((HOLDER,PAYLOAD),(ALTHOLDER,PAYLOAD2)):
        t.save(holder+2,options['notify'],1);t.save(holder+8,payload,4)
    for index,payload in enumerate((PAYLOAD,PAYLOAD2)):
        t.save(payload,options['payload_flags'],4);t.save(payload+8,0x31415926+index,4)
        for i,f in enumerate((1.25,-2.5,3.75,4.5)):t.single(payload+0x30+i*4,f+index)
        t.save(payload+0x40,0x76543210+index,4)
    if options['query_null']==2:t.save(RECORD+0x24,0,4)
    initial=[t.load(BUFFER+i*4,4) for i in range(256)]
    routine=options['routine'];t.r[29],t.r[31]=0x80000,RETURN
    t.r[4],t.r[5],t.r[6],t.r[7]=RECORD,options['mode'],0xCAFE0123,0xCAFE0123
    if routine==1:t.r[4],t.r[6]=KEY,0xCAFE0123
    elif routine==2:t.r[5],t.r[6]=KEY,options['mode']
    elif routine==4:t.r[5],t.r[6]=MATRIX,FACES
    elif routine==5:t.r[4],t.r[5]=options['callback_result'],RECORD
    t.run(ENTRIES[routine]);result=t.r[2]&0xFFFFFFFF if routine in (1,2,3,4) else 0
    return dict(options,initial=initial,expected=[t.load(BUFFER+i*4,4) for i in range(256)],
                expected_result=result,calls=t.calls,events=t.events,instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for mode in (0,1,0xFFFFFFFF):
        for flags in (0,0x10,0x20,0x80,0xFF):
            for mutation in (0,1,2,3,4):cases.append(make_fixture(original,mode=mode,flags=flags,mutation=mutation))
    for other_flags in (0,0x20):
        for notify in (0,1):
            for payload_flags in (0,0x80000):cases.append(make_fixture(original,other_flags=other_flags,notify=notify,payload_flags=payload_flags))
    for query_null in (1,2):
        for mode in (0,1):cases.append(make_fixture(original,query_null=query_null,mode=mode))
    for result in (0,0xFFFFFFFF):cases.append(make_fixture(original,mutation=8,other_flags=0,callback_result=result))
    for found in (0,1):
        for mode in (0,1,0xFFFFFFFF):
            for flags in (0,8,0x10,0x40,0x80,0xFF):
                for count in (0,65535):cases.append(make_fixture(original,routine=1,found=found,mode=mode,flags=flags,count=count))
    cases.append(make_fixture(original,routine=1,found=0,flags=0x10,mutation=5))
    for flags in (0,8,0x10,0xFF):
        for mutation in (0,5):cases.append(make_fixture(original,routine=2,flags=flags,mutation=mutation))
    for flags in (0,0x20,0xFF):
        for payload_flags in (0,0x8000,0x80000,0xFFFFFFFF):cases.append(make_fixture(original,routine=3,flags=flags,payload_flags=payload_flags))
    for flags in (0,0x20):
        for classification in (0,1,2,3,0xFFFFFFFF,0x80000000,0x7FFFFFFF):
            for mutation in (0,6,7):cases.append(make_fixture(original,routine=4,flags=flags,classification=classification,mutation=mutation))
    for callback_result in (0,0xFFFFFFFF):
        for notify in (0,1):
            for mutation in (0,2,3):cases.append(make_fixture(original,routine=5,callback_result=callback_result,notify=notify,mutation=mutation))
    return cases


def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:c[k] for k in (*PARAMETERS,'initial')} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    maximum=max(len(c['events']) for c in cases)
    lines=['/* Authored resource buffers/events only; no original code/table/assets. */',
           'struct ResourceGolden { u32 '+','.join(PARAMETERS)+',initial[256],expected[256],expected_result,calls[17],event_count,events[%d]; };'%maximum,
           'static const struct ResourceGolden resource_golden[] = {']
    for c in cases:
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('0x%08Xu'%c[k] for k in PARAMETERS)
        lines.append('    {%s,{%s},{%s},0x%08Xu,{%s},%d,{%s}},'%(prefix,array('initial'),array('expected'),c['expected_result'],array('calls'),len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/reuse/resource_geometry/trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded resource fixtures; %d original instructions; input SHA256 %s'%(len(cases),sum(c['instruction_count'] for c in cases),input_hash(cases)))


if __name__=='__main__':main()
