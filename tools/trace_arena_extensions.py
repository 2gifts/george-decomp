"""Bounded original arena-extension fixtures with authored memory and calls.

The five complete scalar bodies execute. Numeric memset has a bounded zeroing,
caller-clobber and exposed-state mutation contract. This does not execute the
MMI memset body or model hardware, callback invocation, capacity safety or
malformed pointers. No original instruction/data arrays are exported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from analyze import validated_elf
from trace_arena_buffers import ArenaTrace,BUFFER,END,LOCAL,BASE,GLOBAL,MEMORY_RANGES,ZERO,RETURN

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2B8C00,0x2B8D7C),(0x2B8D80,0x2B8D9C),(0x2B8DA0,0x2B8DAC),
        (0x2B8DB0,0x2B8DD0),(0x2B8DD0,0x2B8DFC))
ENTRIES=tuple(a for a,b in RANGES)


class ArenaExtensionTrace(ArenaTrace):
    def __init__(self,original,mutation=0,outputs=(BUFFER,BUFFER+4,BUFFER+8,BUFFER+12)):
        super().__init__(original,mutation,outputs)

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed arena extension instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('arena extension instruction outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('unsupported arena extension entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body arena extension transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc)
            op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('unsupported arena extension return')
            branch=op in (4,5,20,21)
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('arena extension delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('arena extension transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif op==3:self.library_call(target);pc=self.r[31]
                else:
                    if not body[0]<=target<body[1]:raise ValueError('unreviewed arena extension transfer')
                    pc=target
            else:pc+=8 if annul else 4

    def library_call(self,target):
        if target!=ZERO:raise ValueError('unknown arena extension controlled call')
        pointer,value,length=(self.r[i]&0xFFFFFFFF for i in (4,5,6))
        if value!=0 or length>36 or not BUFFER<=pointer<=END-length:
            raise ValueError('arena extension zero outside authored contract')
        self.calls+=1
        self.events.extend((pointer,value,length,*[self.load(GLOBAL+i*4,4) for i in range(4)],
                            *[self.load(p,4) for p in self.outputs]))
        for i in range(length):self.save(pointer+i,0,1)
        if self.arena_mutation==1:
            self.save(GLOBAL,BASE+128,4);self.save(GLOBAL+8,BASE+132,4)
            self.save(GLOBAL+12,BASE+136,4);self.save(self.outputs[0],BASE+140,4)
        elif self.arena_mutation==2:
            self.save(self.outputs[1],BASE+144,4);self.save(self.outputs[2],BASE+148,4)
        elif self.arena_mutation==3:
            self.save(self.outputs[3],BASE+152,4);self.save(GLOBAL+16,ENTRIES[3],4)
            self.save(GLOBAL+20,BASE+156,4)
        for i in range(4,16):self.r[i]=0xDEADBEEF
        self.r[2]=pointer


def make_fixture(original,routine=0,argument=3,exponent=4,mutation=0,alias=0,
                 base=BASE,cursor=BASE+17,capacity=512,high=BASE,descriptor_global=0,raw_addresses=0,
                 callback=ENTRIES[3],context=BUFFER+32):
    outputs=((BUFFER,BUFFER+4,BUFFER+8,BUFFER+12),(BUFFER,BUFFER,BUFFER,BUFFER),
             (GLOBAL+8,GLOBAL+12,GLOBAL,GLOBAL+4),(GLOBAL,GLOBAL+4,GLOBAL+8,GLOBAL+12),
             (base+(argument*12&0xFFFFFFFF),BUFFER+4,BUFFER+8,BUFFER+12),
             (LOCAL,LOCAL+8,LOCAL+12,LOCAL+20),(BUFFER+12,BUFFER+8,BUFFER+4,BUFFER),
             (GLOBAL+16,GLOBAL+20,GLOBAL+8,GLOBAL),
             (BUFFER,BUFFER+4,BUFFER+8,base+(argument*12&0xFFFFFFFF)))[alias]
    outputs=tuple(v&0xFFFFFFFF for v in outputs)
    t=ArenaExtensionTrace(original,mutation,outputs)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0x5A
    for p in (LOCAL,GLOBAL):
        for i,value in enumerate((base,capacity,cursor,high,ENTRIES[3],BUFFER+32)):t.save(p+i*4,value,4)
    initial=[t.load(BUFFER+i*4,4) for i in range(512)]
    global_initial=[t.load(GLOBAL+i*4,4) for i in range(6)]
    t.r[29],t.r[31]=0x80000,RETURN
    descriptor=GLOBAL if descriptor_global else LOCAL
    if routine==0:
        t.r[4],t.r[5]=argument,exponent
        for i,p in enumerate(outputs):t.r[6+i]=p
    elif routine==1:t.r[4],t.r[5],t.r[6]=descriptor,base,capacity
    elif routine==2:t.r[4],t.r[5],t.r[6]=descriptor,callback,context
    elif routine==3:t.r[4]=descriptor
    elif routine==4:t.r[4],t.r[5]=descriptor,exponent
    else:raise ValueError('unsupported fixture routine')
    t.run(ENTRIES[routine])
    return dict(routine=routine,argument=argument&0xFFFFFFFF,exponent=exponent&0xFFFFFFFF,mutation=mutation,alias=alias,
                descriptor_global=descriptor_global,raw_addresses=raw_addresses,callback=callback,context=context,
                outputs=outputs,initial=initial,global_initial=global_initial,
                expected=[t.load(BUFFER+i*4,4) for i in range(512)],
                global_expected=[t.load(GLOBAL+i*4,4) for i in range(6)],calls=t.calls,events=t.events,
                instruction_count=t.instruction_count)


def fixtures(original):
    cases=[]
    for count in (0,1,2,3,4,5,7,8,0x7FFFFFFF,0x80000000,0x80000001,0xFFFFFFFD,0xFFFFFFFF):
        for exponent in (0,1,4,5,32,36,63):
            if exponent==63 and count not in (0,4,8,0x7FFFFFFF,0x80000000):continue
            for capacity in (0,120,512):cases.append(make_fixture(original,argument=count,exponent=exponent,capacity=capacity))
    for alias in (1,2,3,5,6,7):
        for mutation in (0,1,2,3):
            for count in (0,1,3,4):cases.append(make_fixture(original,argument=count,alias=alias,mutation=mutation))
    for alias in (4,8):
        for mutation in (0,1,2,3):
            for count in (0,1,2,3):cases.append(make_fixture(original,argument=count,exponent=0,alias=alias,mutation=mutation,base=BASE,cursor=BASE))
    for base,cursor,high in ((BASE+1,BASE+1,BASE+64),(BASE+7,BASE+511,BASE),(BASE,BASE+16,BASE+96)):
        cases.append(make_fixture(original,argument=3,base=base,cursor=cursor,high=high,capacity=32))
    for routine in (1,2,3,4):
        for descriptor_global in (0,1):
            for exponent in (0,1,4,5,31,32,36,63):
                for cursor,high in ((BASE,BASE),(BASE+17,BASE),(BASE+16,BASE+32)):
                    cases.append(make_fixture(original,routine,exponent=exponent,descriptor_global=descriptor_global,cursor=cursor,high=high))
    for descriptor_global in (0,1):
        for callback,context in ((0,0),(ENTRIES[3],LOCAL),(0xF00000C0,GLOBAL),(0xDEADBEEF,0x89ABCDEF)):
            cases.append(make_fixture(original,2,descriptor_global=descriptor_global,callback=callback,context=context))
    for routine in (0,1,3,4):
        for base,cursor,capacity,high in ((0xFFFFFFE0,0xFFFFFFF8,64,0xFFFFFFE0),
                                        (0x80000000,0x7FFFFFFF,32,0x80000010),
                                        (0,0xFFFFFFF0,0xFFFFFFFF,0),
                                        (0xFFFFFFF0,0,32,0xFFFFFFF4)):
            for exponent in (0,4,31):
                cases.append(make_fixture(original,routine,4,exponent,base=base,cursor=cursor,capacity=capacity,high=high,raw_addresses=1))
    return cases


def input_hash(cases):
    keys=('routine','argument','exponent','mutation','alias','descriptor_global','raw_addresses','callback','context','outputs','initial','global_initial')
    return hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()


def golden_header(cases):
    lines=['/* Lossless authored arena-extension memory/events; no original code/data arrays. */',
           'struct ArenaExtensionGolden { u32 routine,argument,exponent,mutation,alias,descriptor_global,raw_addresses,callback,context,outputs[4],local_initial[6],global_initial[6],change_count,changes[32][2],global_expected[6],calls,event_count,events[44]; };',
           'static const struct ArenaExtensionGolden arena_extension_golden[] = {']
    for c in cases:
        local=(LOCAL-BUFFER)//4
        assert len(c['initial'])==512 and all(v==0x5A5A5A5A for i,v in enumerate(c['initial']) if not local<=i<local+6)
        changes=[(i,v) for i,v in enumerate(c['expected']) if v!=c['initial'][i]]
        assert len(changes)<=32
        local_initial=','.join('0x%08Xu'%v for v in c['initial'][local:local+6])
        patch=','.join('{%du,0x%08Xu}'%(i,v) for i,v in changes) or '{0u,0u}'
        array=lambda key:','.join('0x%08Xu'%v for v in c[key])
        prefix=','.join('%du'%c[k] for k in ('routine','argument','exponent','mutation','alias','descriptor_global','raw_addresses','callback','context'))
        lines.append('    {%s,{%s},{%s},{%s},%du,{%s},{%s},%du,%du,{%s}},'%(prefix,array('outputs'),local_initial,array('global_initial'),len(changes),patch,array('global_expected'),c['calls'],len(c['events']),array('events')))
    return '\n'.join(lines+['};',''])


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/arena_extensions_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d bounded arena-extension fixtures; %d original instructions; maximum %d'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases)))
    print('input SHA256 '+input_hash(cases))


if __name__=='__main__':main()
