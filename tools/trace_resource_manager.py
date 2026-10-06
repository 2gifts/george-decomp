"""Bounded resource-manager fixtures, with original switch and pool execution.

Four selected complete bodies, a readonly provider query and the published pool
primitive execute from the validated local ELF. Resolution, heap, queue and map
calls have explicit authored observation contracts. Those contracts stress
capture/reload behavior; they do not model the full filesystem/queue/heap graph.
Only synthetic memory and events are exported; no original code/table arrays.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_resource_base import ResourceBaseTrace
from trace_geometry import Trace,RETURN

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x24000
WORDS=(END-BUFFER)//4
ALLOCATORS=(0x20000,0x21000)
MANAGERS=(0x22000,0x22200)
PROVIDERS=(0x22400,0x22420,0x22440)
OBJECTS=(0x22500,0x22800)
RECORD,OTHER=0x22B00,0x22B40
POOLS=(0x22C00,0x22C20)
SLOTS=0x22D00
MAPS=(0x22E00,0x22E20)
QUEUE,PAYLOAD,CURSOR,KEY=0x22F00,0x22F80,0x23000,0x13579BDF
GLOBAL=0x3F960C
TABLE,TABLE_SIZE=0x43B0E0,28
TABLE_HASH='432edbed9f952c1a4eaabbf1f6fcfc33c6b122edbed52ab78a85dd8991aaf911'
COMPLETION,OUTPUT_ROLE=0x225D38,0xF0000100
RANGES=((0x224AB8,0x224EC8),(0x225C28,0x225C84),
        (0x224750,0x224824),(0x224678,0x22474C))
HELPERS=((0x229FC8,0x229FEC),(0x2AD700,0x2AD748))
ENTRIES=tuple(a for a,b in RANGES)
CALLS=(0x229FC8,0x229FF0,0x229020,0x2AD700,0x20E598,0x21ABD8,
       0x217F78,0x218020,0x217FB0,0x217FE8,0x21A0B8,0x21A190)
ARG_COUNTS=(1,5,4,3,1,9,1,1,1,1,3,2)
MEMORY_RANGES=((BUFFER,END),(GLOBAL,GLOBAL+4),(0x7F000,0x81000))

class ManagerTrace(ResourceBaseTrace):
    def __init__(self,original,record=RECORD,mutation=0,resolution=1,
                 lookup=1,heap_result=PAYLOAD,remove_result=1):
        super().__init__(original)
        self.record,self.mutation,self.resolution=record,mutation,resolution
        self.lookup,self.heap_result,self.remove_result=lookup,heap_result,remove_result
        self.calls=[0]*len(CALLS);self.events=[]
        data=original[TABLE-0xFF000:TABLE-0xFF000+TABLE_SIZE]
        if hashlib.sha256(data).hexdigest()!=TABLE_HASH:raise ValueError('wrong full manager switch identity')
        self.switch_targets=struct.unpack('<7I',data)
        for i,b in enumerate(data):self.memory[TABLE+i]=b

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+HELPERS):raise ValueError('unreviewed manager instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('manager instruction outside image')
        return struct.unpack_from('<I',self.original,offset)[0]

    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(a<=address and address+size<=b for a,b in MEMORY_RANGES+((TABLE,TABLE+TABLE_SIZE),)):
            raise ValueError('manager memory outside aligned initialized windows')
        if any(address+i not in self.memory for i in range(size)):raise ValueError('uninitialized manager memory')

    def save(self,address,value,size):
        self.memory_check(address,size)
        if TABLE<=address<TABLE+TABLE_SIZE:raise ValueError('readonly manager switch store')
        return Trace.save(self,address,value,size)

    def execute(self,instruction,pc):
        op,rs,rt=instruction>>26,instruction>>21&31,instruction>>16&31
        imm=instruction&65535;simm=imm-65536 if imm&32768 else imm
        if op==0 and instruction&63==9:raise ValueError('unowned manager JALR')
        if op in (0xA,0xB,0xE):
            value=self.r[rs]&0xFFFFFFFF
            if op==0xA:self.r[rt]=int((value-0x100000000 if value&0x80000000 else value)<simm)
            elif op==0xB:self.r[rt]=int(value<(simm&0xFFFFFFFF))
            else:self.r[rt]=(value^imm)&0xFFFFFFFF
            self.r[0]=0;self.instruction_count+=1
            if self.instruction_count>3000:raise ValueError('manager instruction bound exceeded')
            return None,False
        return super().execute(instruction,pc)

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES+HELPERS if r[0]==entry),None)
        if body is None:raise ValueError('unsupported manager entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body manager transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc);op=instruction>>26
            jump=op==0 and instruction&63==8
            if jump:
                if instruction==0x03E00008:
                    if target!=stop:raise ValueError('manager return misses selected stop')
                elif pc!=0x224B70 or instruction!=0x00800008 or target not in self.switch_targets:
                    raise ValueError('unsupported manager indirect jump')
            branch=op in (4,5,20,21);call=op==3
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('manager delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('manager transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif call:
                    continuation=self.r[31]
                    if target in ENTRIES:self.run(target,continuation)
                    else:self.library_call(target)
                    if self.r[31]!=continuation:raise ValueError('manager call corrupted link register')
                    pc=continuation
                else:
                    if not body[0]<=target<body[1]:raise ValueError('manager local transfer outside owned body')
                    pc=target
            else:pc+=8 if annul else 4

    def event(self,index,args):
        self.calls[index]+=1;record=self.record
        manager=self.load(GLOBAL,4);kind=self.load(record+3,1)
        self.events.extend((index,*args,*([0]*(9-len(args))),manager,
            *[self.load(record+i,4) for i in (0,4,8,12,16,20,28)],
            self.load(manager+0x98+kind*4,4) if kind<8 else 0,
            self.load(manager+0xB8+kind*4,4) if kind<8 else 0))
        if len(self.events)>400:raise ValueError('manager event bound exceeded')

    def library_call(self,target):
        if target not in CALLS:raise ValueError('unknown manager controlled call')
        index=CALLS.index(target);argc=ARG_COUNTS[index]
        args=[self.r[4+i]&0xFFFFFFFF for i in range(min(argc,8))]
        if argc==9:args.append(self.load(self.r[29],4))
        record=self.record;m=self.mutation
        if index in (1,2):
            output=3 if index==1 else 2
            if args[0] not in OBJECTS or args[output]!=self.r[29]+0x10 or args[output+1]!=record+12:
                raise ValueError('unsupported manager resolution argument contract')
            if index==1 and args[1] not in range(6):raise ValueError('unsupported manager resolution command')
            event_args=args[:];event_args[output]=OUTPUT_ROLE;self.event(index,event_args)
            result=self.resolution
            if result:
                self.save(args[output],0xA1234567,4)
                self.save(args[output+1],self.load(record+12,4),4)
            if m==1:
                self.save(GLOBAL,MANAGERS[1],4)
                self.save(PROVIDERS[0],9,4);self.save(PROVIDERS[0]+16,OBJECTS[1],4)
            if m==2:
                self.save(record+3,3,1);self.save(record+2,0x40,1)
                self.save(record+4,KEY+1,4)
            if m==8:self.nested(ENTRIES[3],(0,1,record))
        elif index==0:
            if args[0] not in OBJECTS:raise ValueError('unknown manager query object')
            self.event(index,args);self.run(target,self.r[31]);result=self.r[2]
        elif index==3:
            if args[0] not in POOLS or args[2]!=1:raise ValueError('unsupported manager pool lanes')
            self.event(index,args)
            if m==3:
                self.save(GLOBAL,MANAGERS[1],4);self.save(record+12,257,4)
                self.save(record+8,0xB7654321,4)
            self.run(target,self.r[31]);result=self.r[2]
        elif index==4:
            if self.heap_result!=0 and not BUFFER<=self.heap_result<END:raise ValueError('unknown manager heap result')
            self.event(index,args);result=self.heap_result
            if m==4:
                self.save(record+12,511,4);self.save(GLOBAL,MANAGERS[1],4)
        elif index==5:
            if args[0]!=QUEUE or args[6]!=COMPLETION or args[7]!=record or args[8] not in (0,1):
                raise ValueError('unsupported nine-lane manager request')
            self.event(index,args);result=0x76543210
            if m==5:
                self.save(GLOBAL,MANAGERS[1],4);self.save(record+3,6,1)
                self.save(record+4,KEY+2,4);self.save(record+12,513,4)
        elif index in (6,7,8,9):
            self.event(index,args);result=PAYLOAD if self.lookup else 0
            if m==5:self.save(record+3,4,1);self.save(record+2,0x81,1)
        elif index==10:
            if args[0] not in MAPS or args[2]!=record:raise ValueError('unsupported manager map publication')
            self.event(index,args);result=0x11223344
            if m==6:
                self.save(GLOBAL,MANAGERS[1],4);self.save(record+3,1,1)
                self.save(MANAGERS[1]+0x98+6*4,0xFFFFFFFF,4)
        else:
            if args[0] not in MAPS:raise ValueError('unsupported manager map removal')
            self.event(index,args);result=self.remove_result
            if m==7:self.save(GLOBAL,MANAGERS[1],4)
        for r in range(3,16):self.r[r]=0xDEADBEEF
        self.r[2]=result&0xFFFFFFFF

def make_fixture(original,routine=0,kind=0,flags=0,size=129,provider=1,
                 allocator=0,mutation=0,resolution=1,lookup=1,heap_result=PAYLOAD,
                 pool_case=0,first=1,second=1,alias=0,remove_result=1,field1C=1,counter_case=0):
    record=RECORD if alias==0 else MANAGERS[0]+(0x20,0x24,0x28)[alias-1]
    t=ManagerTrace(original,record,mutation,resolution,lookup,heap_result,remove_result)
    for a,b in MEMORY_RANGES:
        for p in range(a,b):t.memory[p]=0x5A
    t.save(GLOBAL,MANAGERS[0],4)
    for manager,allocator_ptr in zip(MANAGERS,ALLOCATORS):
        for offset,value in ((0x24,10),(0x28,20),(0x30,0x123),(0x34,0x456),(0xFC,0),(0x164,QUEUE),(0x168,allocator_ptr)):
            t.save(manager+offset,value,4)
        for k in range(8):
            for off,value in ((0x38,0x222+k),(0x58,5+k),(0x78,MAPS[int(manager==MANAGERS[1])]),(0x98,8+k),(0xB8,9+k)):
                t.save(manager+off+k*4,value,4)
        for k,p in enumerate(PROVIDERS):t.save(manager+0x100+k*4,p,4)
    for a in ALLOCATORS:
        for k in range(1000):t.save(a+k*4,0,4)
        for k in (0,1,2,3,999):t.save(a+k*4,POOLS[0],4)
        for off,value in ((0xFA0,PAYLOAD),(0xFA4,PAYLOAD+32),(0xFB8,512),(0xFBC,0),(0xFC4,POOLS[1]),(0xFC8,0)):
            t.save(a+off,value,4)
    for pool in POOLS:
        for off,value in ((0,8),(4,32),(8,SLOTS),(12,0),(20,2)):t.save(pool+off,value,4)
        for off,value in ((16,0),(24,0),(26,0)):t.save(pool+off,value,2)
    t.save(SLOTS,1,2);t.save(SLOTS+32,0xFFFF,2)
    if counter_case:
        count,peak=((0xFFFFFFFF,0),(0x7FFFFFFF,0),(0,0xFFFFFFFF),(0xFFFFFFFF,0xFFFFFFFF))[counter_case-1]
        for manager in MANAGERS:
            for k in range(8):
                t.save(manager+0x98+k*4,count,4);t.save(manager+0xB8+k*4,peak,4)
    for p in PROVIDERS:
        for off,value in ((0,kind),(4,1),(16,OBJECTS[0])):t.save(p+off,value,4)
    for i,o in enumerate(OBJECTS):
        for off,value in ((0x98,1),(0x1B8,1),(0xA8,0xABC00000+i),(0x244,0xDEF00000+i)):
            t.save(o+off,value,4)
    if provider:
        t.save(MANAGERS[0]+0xFC,1 if provider<7 else 2,4)
        if provider==2:t.save(PROVIDERS[0],(kind+1)&255,4)
        if provider==3:resolution=0;t.resolution=0
        if provider in (4,5,6):t.save(PROVIDERS[0],9,4)
        if provider==4:t.save(OBJECTS[0]+0x98,0,4)
        if provider==6:resolution=0;t.resolution=0
        if provider==7:t.save(PROVIDERS[0]+4,0,4)
        if provider==8:t.save(PROVIDERS[0],(kind+1)&255,4)
    a=ALLOCATORS[0]
    if allocator in (1,2,3,4,5):
        t.save(a+0xFBC,CURSOR,4);t.save(a+0xFC8,1,4)
        if allocator in (2,3,4):t.save(a+0xFC4,0,4)
        if allocator==3:t.save(a+0xFB8,128,4)
        if allocator==4:t.save(a+0xFB8,0,4)
        if allocator==5:t.save(a+0xFC8,2,4)
    if allocator==6:
        for k in range(1000):t.save(a+k*4,0,4)
    if pool_case==1:t.save(POOLS[0]+20,0,4);t.save(POOLS[1]+20,0,4)
    if pool_case==2:t.save(POOLS[0]+26,1,2);t.save(POOLS[1]+26,1,2)
    t.save(record,1,2);t.save(record+2,flags,1);t.save(record+3,kind,1)
    for off,value in ((4,KEY),(8,0x11112222),(12,size),(16,PAYLOAD),(20,0x33334444),(24,0),(28,field1C),(32,0)):
        t.save(record+off,value,4)
    if routine in (2,3):
        if allocator==1:t.save(a+0xFA0,PAYLOAD+1,4)
        if allocator==2:t.save(a+0xFA4,PAYLOAD-1,4)
        if allocator==3:t.save(record+16,PAYLOAD+32,4)
        if allocator==4:t.save(record+16,0xFFFFFFFF,4)
        if allocator==5:t.save(record+16,0,4)
    initial=[t.load(BUFFER+i*4,4) for i in range(WORDS)];global_initial=t.load(GLOBAL,4)
    t.r[29],t.r[31]=0x80000,RETURN
    if routine==0:t.r[4]=record
    elif routine==1:t.r[4],t.r[5]=KEY,kind
    elif routine in (2,3):t.r[4:7]=[first,second,record]
    else:raise ValueError('unsupported manager fixture')
    t.run(ENTRIES[routine])
    return dict(routine=routine,kind=kind,flags=flags,size=size,provider=provider,allocator=allocator,
        mutation=mutation,resolution=resolution,lookup=lookup,heap_result=heap_result,pool_case=pool_case,
        first=first,second=second,alias=alias,remove_result=remove_result,field1C=field1C,counter_case=counter_case,record=record,
        initial=initial,global_initial=global_initial,
        expected=[t.load(BUFFER+i*4,4) for i in range(WORDS)],global_expected=t.load(GLOBAL,4),
        calls=t.calls,events=t.events,instruction_count=t.instruction_count)

def fixtures(original):
    cases=[]
    for kind in range(8):
        for provider in range(9):cases.append(make_fixture(original,kind=kind,provider=provider))
    for size in (0,1,127,128,129,256,127872,128000,0x7FFFFFFF,0xFFFFFFFF):
        for flags in (0,0x40):
            for allocator in range(7):cases.append(make_fixture(original,flags=flags,size=size,allocator=allocator))
    for allocator in (0,1,5):
        for pool_case in (1,2):cases.append(make_fixture(original,flags=0x40,allocator=allocator,pool_case=pool_case))
    for provider in (0,1,5,7,8):
        for mutation in (1,2,3,4,5,6,8):cases.append(make_fixture(original,provider=provider,mutation=mutation,flags=0x40))
    for kind in (0,1,3,6):
        for flags in (0,0xFF):cases.append(make_fixture(original,kind=kind,provider=0,flags=flags,lookup=0))
    for field1C in (0,2,0xFFFFFFFF):cases.append(make_fixture(original,field1C=field1C))
    for allocator in (0,1,2,3,4,5,6):cases.append(make_fixture(original,allocator=allocator,heap_result=0))
    for counter_case in (1,2,3,4):
        cases.append(make_fixture(original,counter_case=counter_case))
        cases.append(make_fixture(original,1,counter_case=counter_case))
    for kind in (0,3,7,0x40000000):
        for remove_result in (0,1,0x80000000):
            for mutation in (0,7):cases.append(make_fixture(original,1,kind=kind,remove_result=remove_result,mutation=mutation))
    for routine in (2,3):
        for first,second in ((0,0),(0,1),(1,0),(1,1),(0x80000000,0xFFFFFFFF)):
            for size in (0,1,129,0xFFFFFFFF):
                for allocator in range(6):cases.append(make_fixture(original,routine,size=size,allocator=allocator,first=first,second=second))
        for alias in (1,2,3):
            for kind in (0,3,7):cases.append(make_fixture(original,routine,kind=kind,alias=alias))
    return cases

def input_hash(cases):
    return hashlib.sha256(json.dumps([{k:v for k,v in c.items() if k not in ('expected','global_expected','calls','events','instruction_count')} for c in cases],sort_keys=True,separators=(',',':')).encode()).hexdigest()

def golden_header(cases):
    fields=('routine','kind','flags','size','provider','allocator','mutation','resolution','lookup','heap_result','pool_case','first','second','alias','remove_result','field1C','counter_case','record')
    def runs(words):
        result=[];start=0
        while start<len(words):
            end=start+1
            while end<len(words) and words[end]==words[start]:end+=1
            result.extend((start,end-start,words[start]));start=end
        return result
    initial=[runs(c['initial']) for c in cases]
    changes=[[v for i,(a,b) in enumerate(zip(c['initial'],c['expected'])) if a!=b for v in (i,b)] for c in cases]
    nr,nc,ne=max(map(len,initial)),max(map(len,changes)),max(len(c['events']) for c in cases)
    lines=['/* Complete authored input runs and output patches; no original table/code arrays. */',
        'struct ResourceManagerGolden { u32 '+','.join(fields)+',global_initial,global_expected,calls[12],initial_run_count,initial_runs[%d],change_count,changes[%d],event_count,events[%d]; };'%(nr,nc,ne),
        'static const struct ResourceManagerGolden resource_manager_golden[] = {']
    array=lambda values:'{'+','.join('0x%08Xu'%v for v in values)+'}'
    for c,r,p in zip(cases,initial,changes):
        lines.append(' {%s,0x%08Xu,0x%08Xu,%s,%du,%s,%du,%s,%du,%s},'%(','.join('0x%08Xu'%c[k] for k in fields),c['global_initial'],c['global_expected'],array(c['calls']),len(r)//3,array(r),len(p)//2,array(p),len(c['events']),array(c['events'])))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/resource_manager_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,raw=validated_elf(args.elf);cases=fixtures(raw)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps({'limitation':__doc__,'input_sha256':input_hash(cases),'cases':cases},indent=2)+'\n')
    if args.golden_header:args.golden_header.write_text(golden_header(cases))
    print('%d fixtures; %d instructions; max%d; input %s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),input_hash(cases)))

if __name__=='__main__':main()
