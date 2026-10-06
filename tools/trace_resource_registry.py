"""Bounded low32 traces of complete resource-registry and map bodies.

All thirteen new functions and the existing scalar accessor execute their real
instructions. There are no callback or external-library substitutions. Input
memory, keys, links, indexes and aliases are authored fixtures; original class
names, allocator safety, full resource lifetime and EE timing are not modeled.
No original code/data arrays are exported.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_resource_base import ResourceBaseTrace
from trace_geometry import Trace,RETURN

ROOT=Path(__file__).resolve().parents[1]
BUFFER,END=0x20000,0x21000
WORDS=(END-BUFFER)//4
GLOBAL=0x3F9468
RANGES=((0x217F78,0x217FAC),(0x217FB0,0x217FE4),
        (0x217FE8,0x21801C),(0x218020,0x218054),
        (0x2193A0,0x219418),(0x21A0B8,0x21A18C),
        (0x21A190,0x21A238),(0x21A238,0x21A2E8),
        (0x229020,0x2290B8),(0x229FF0,0x22A0BC),
        (0x2A77F0,0x2A7824),(0x2AF9A0,0x2AFA9C),
        (0x2A8250,0x2A8308))
HELPERS=((0x2A8228,0x2A8230),)
ENTRIES=tuple(a for a,b in RANGES)
KINDS=(14,0,6,3)
MAP,POOL,SLOTS,BUCKETS,ALT_BUCKETS=0x20000,0x20020,0x20040,0x20080,0x20090
NODES=(0x20100,0x20110,0x20120,0x20130)
ALT_POOL,ALT_SLOTS=0x20200,0x20240
REGISTRY,CONTEXT=0x20000,0x20040
LINKS=(0x20100,0x20200,0x20300,0x203C0)
GROUP=0x20400
DATA,INDEX,ROWS,ORIGIN=0x20500,0x20600,0x20700,0x20800
PROVIDER,INDEXED_PROVIDER=0x20900,0x20A00
OUTPUT=0x20C00
KEY=7
MEMORY_RANGES=((BUFFER,END),(GLOBAL,GLOBAL+4),(0x7F000,0x81000))

class RegistryTrace(ResourceBaseTrace):
    def __init__(self,original):
        super().__init__(original)

    def fetch(self,pc):
        if pc%4 or not any(a<=pc<b for a,b in RANGES+HELPERS):
            raise ValueError('unreviewed registry instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('registry code outside original')
        return struct.unpack_from('<I',self.original,offset)[0]

    def memory_check(self,address,size):
        if size not in (1,2,4,8,16) or address%size or not any(a<=address and address+size<=b for a,b in MEMORY_RANGES):
            raise ValueError('registry memory outside aligned authored scope')
        if any(address+i not in self.memory for i in range(size)):raise ValueError('uninitialized registry memory')

    def execute(self,instruction,pc):
        op,rs,rt,rd,fn=instruction>>26,instruction>>21&31,instruction>>16&31,instruction>>11&31,instruction&63
        sh=instruction>>6&31;imm=instruction&65535;simm=imm-65536 if imm&32768 else imm
        if op==0 and fn==9:raise ValueError('unowned registry JALR')
        if op==0 and fn in (0x25,0x2A):
            if sh:raise ValueError('reserved registry SPECIAL shift')
            a,b=self.r[rs]&0xFFFFFFFF,self.r[rt]&0xFFFFFFFF
            if fn==0x25:self.r[rd]=a|b
            else:self.r[rd]=int((a-0x100000000 if a&0x80000000 else a)<(b-0x100000000 if b&0x80000000 else b))
            self.r[0]=0;self.instruction_count+=1;result=(None,False)
        elif op==0xE:
            self.r[rt]=(self.r[rs]^imm)&0xFFFFFFFF
            self.r[0]=0;self.instruction_count+=1;result=(None,False)
        elif op==6:
            if rt:raise ValueError('reserved registry BLEZ rt')
            a=self.r[rs]&0xFFFFFFFF;a=a-0x100000000 if a&0x80000000 else a
            self.instruction_count+=1;result=((pc+4+simm*4)&0xFFFFFFFF if a<=0 else None,False)
        else:return super().execute(instruction,pc)
        if self.instruction_count>3000:raise ValueError('registry instruction bound exceeded')
        return result

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES+HELPERS if r[0]==entry),None)
        if body is None:raise ValueError('unsupported registry entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:raise ValueError('cross-body registry transfer')
            instruction=self.fetch(pc);target,annul=self.execute(instruction,pc);op=instruction>>26
            if op==0 and instruction&63==8 and (instruction!=0x03E00008 or target!=stop):
                raise ValueError('registry invocation must return by actual JR31 to selected stop')
            branch=op in (4,5,6,20,21);call=op==3
            if target is not None or branch and not annul:
                if pc+4>=body[1]:raise ValueError('registry delay outside complete body')
                if self.execute(self.fetch(pc+4),pc+4)!=(None,False):raise ValueError('registry transfer in delay')
                if instruction==0x03E00008:return
                if target is None:pc+=8
                elif call:
                    continuation=self.r[31]
                    if target not in ENTRIES+tuple(a for a,b in HELPERS):raise ValueError('unknown registry call')
                    self.run(target,continuation)
                    if self.r[31]!=continuation:raise ValueError('registry call corrupted link register')
                    pc=continuation
                else:
                    if not body[0]<=target<body[1]:raise ValueError('registry local transfer outside owned body')
                    pc=target
            else:pc+=8 if annul else 4

def blank(original):
    t=RegistryTrace(original)
    for a,b in MEMORY_RANGES:
        for address in range(a,b):t.memory[address]=0
    for i in range(WORDS):t.save(BUFFER+4*i,0xCCCC0000+i,4)
    return t

def put(t,address,*values):
    for i,value in enumerate(values):t.save(address+4*i,value,4)

def map_scene(t,position,flags=0,index=2):
    put(t,MAP,flags,3,0xFFFFFFFF,BUCKETS,POOL)
    put(t,POOL,4,index,SLOTS,NODES[0]);put(t,ALT_POOL,4,2,ALT_SLOTS,NODES[0])
    put(t,SLOTS,0,0,NODES[3],0);put(t,ALT_SLOTS,0,0,0,0)
    put(t,BUCKETS,0,NODES[0] if position>=0 else 0,0)
    put(t,ALT_BUCKETS,0,0,0)
    for i,node in enumerate(NODES):put(t,node,NODES[i+1] if i<2 else 0,KEY if i==position else 1,0x11110000+i)

def data_scene(t,indexed,position,count=3,stride=12,bucket_count=3):
    put(t,DATA,0,4 if indexed else 0,count)
    put(t,DATA+0x1C,INDEX if indexed else ROWS,ORIGIN)
    put(t,INDEX,bucket_count,3,0x20,0x40,stride)
    put(t,INDEX+0x20,0,0,3)
    for i in range(3):
        row=INDEX+0x40+i*stride if indexed else ROWS+12*i
        put(t,row,KEY if i==position else 1,129+i,32+16*i)
    put(t,GROUP+12,*([DATA]*15))
    put(t,PROVIDER+0x10,DATA,0x12345);put(t,PROVIDER+0xA4,0xA)
    put(t,INDEXED_PROVIDER+0x88,0x12345);put(t,INDEXED_PROVIDER+0x98,*([INDEX]*6))
    put(t,INDEXED_PROVIDER+0x1C0,0xA)

def fixtures(original):
    cases=[]
    def add(t,function,args,label):
        initial=[t.load(BUFFER+4*i,4) for i in range(WORDS)];glob=t.load(GLOBAL,4)
        t.r[4:9]=list(args)+[0]*(5-len(args));t.r[29],t.r[31]=0x80000,RETURN
        t.run(ENTRIES[function])
        cases.append({'function':function,'args':list(args)+[0]*(5-len(args)),
                      'global':glob,'initial':initial,'expected':[t.load(BUFFER+4*i,4) for i in range(WORDS)],
                      'expected_global':t.load(GLOBAL,4),'result':t.r[2]&0xFFFFFFFF if function!=5 else 0,
                      'result_observed':function!=5,
                      'instruction_count':t.instruction_count,'label':label})
    for flags in (0,1,3):
        for position in (-1,0,1,2,3):
            for alias in range(5):
                t=blank(original);map_scene(t,position,flags)
                if alias==1:put(t,SLOTS+8,0)
                if alias==2:put(t,POOL+4,0,POOL+4)
                if alias==3:put(t,SLOTS+8,MAP)
                if alias==4:put(t,SLOTS+8,NODES[0])
                add(t,5,(MAP,KEY,0x2468ABCD),'insert flags%d position%d alias%d'%(flags,position,alias))
    for function in (6,7):
        for position in (-1,0,1,2,3):
            for alias in range(9 if function==7 else 2):
                t=blank(original);map_scene(t,position)
                output=OUTPUT
                if alias==1:put(t,POOL+4,0,SLOTS+4)
                if function==7:
                    if alias==2:output=MAP+16;put(t,NODES[max(position,0) if position<3 else 0]+8,ALT_POOL)
                    if alias==3:output=POOL+4;put(t,NODES[max(position,0) if position<3 else 0]+8,1)
                    if alias==4:output=POOL+8;put(t,NODES[max(position,0) if position<3 else 0]+8,ALT_SLOTS)
                    if alias==5:output=MAP+8
                    if alias==6:output=BUCKETS+4
                    if alias==7:output=NODES[1]+8
                    if alias==8:output=POOL+12
                add(t,function,(MAP,KEY,output) if function==7 else (MAP,KEY),'remove%d position%d alias%d'%(function,position,alias))
    for function in (5,6,7):
        t=blank(original);map_scene(t,0)
        for node in NODES[:3]:put(t,node+4,KEY)
        args=(MAP,KEY,0x2468ABCD) if function==5 else (MAP,KEY,OUTPUT) if function==7 else (MAP,KEY)
        add(t,function,args,'first duplicate map entry%d'%function)
    for indexed in (0,1):
        for position in (-1,0,1,2):
            for size in (0,OUTPUT,DATA+0x20,ROWS+4,INDEX+0x44,INDEX+0x48,DATA+8):
                t=blank(original);data_scene(t,indexed,position)
                add(t,11,(DATA,KEY,size),'data indexed%d position%d size%x'%(indexed,position,size))
        for count in (0,0xFFFFFFFF,0x80000000):
            t=blank(original);data_scene(t,indexed,0,count)
            add(t,11,(DATA,KEY,OUTPUT),'data count%x indexed%d'%(count,indexed))
    for stride in (12,16,28):
        for position in (-1,0,1,2):
            for bucket_count in (1,3):
                t=blank(original);data_scene(t,1,position,stride=stride,bucket_count=bucket_count)
                if bucket_count==1:put(t,INDEX+0x20,0)
                add(t,12,(INDEX,KEY),'index stride%d position%d buckets%d'%(stride,position,bucket_count))
    t=blank(original);data_scene(t,1,0);put(t,INDEX,0);add(t,12,(INDEX,KEY),'empty index')
    t=blank(original);data_scene(t,1,0);put(t,INDEX+0x20,0,3,3);add(t,12,(INDEX,KEY),'empty bucket span')
    t=blank(original);data_scene(t,1,0);put(t,INDEX,1,0x40000000);put(t,INDEX+0x20,0)
    add(t,12,(INDEX,KEY),'row endpoint multiplication wraps to empty range')
    t=blank(original);data_scene(t,1,-1);put(t,INDEX+12,0xFFFFFE00)
    for i in range(3):put(t,GROUP+12*i,KEY if i==2 else 1,101+i,32+i)
    add(t,12,(INDEX,KEY),'self relative row offset wraps to preceding authored window')
    for indexed in (0,1):
        t=blank(original);data_scene(t,indexed,0)
        for i in range(3):put(t,(INDEX+0x40 if indexed else ROWS)+12*i,KEY)
        add(t,11,(DATA,KEY,OUTPUT),'first duplicate data row indexed%d'%indexed)
    for kind in KINDS:
        for state in range(3):
            t=blank(original);data_scene(t,state==2,0 if state else -1)
            if state==1:put(t,GROUP+12+kind*4,0)
            add(t,10,(GROUP,kind,KEY),'group kind%d state%d'%(kind,state))
    for function in range(5):
        kind=KINDS[function] if function<4 else 0
        for disabled in (0,1,3,7):
            for match in (-1,0,1,2,3):
                t=blank(original);data_scene(t,0,0)
                put(t,REGISTRY,CONTEXT);put(t,CONTEXT+0x84,LINKS[0]);put(t,GLOBAL,REGISTRY)
                for i,node in enumerate(LINKS):
                    put(t,node,LINKS[i+1] if i<3 else 0);put(t,node+0x10,(disabled>>i)&1)
                    put(t,node+0x98,GROUP if i==match else GROUP+0x80)
                put(t,GROUP+0x80+12,*([0]*15))
                add(t,function,(KEY,) if function<4 else (CONTEXT,kind,KEY),'walk%d disabled%d match%d'%(function,disabled,match))
        if function<4:
            t=blank(original);put(t,GLOBAL,0);add(t,function,(KEY,),'null global%d'%function)
    for indexed in (0,1):
        for position in (-1,0,2):
            for alias in range(7):
                t=blank(original);data_scene(t,indexed,position)
                address,size=OUTPUT,OUTPUT+4
                if alias==1:address=size=OUTPUT
                if alias==2:address=DATA+0x20
                if alias==3:size=PROVIDER+0x14
                if alias==4:size=PROVIDER+0xA4
                if alias==5:size=DATA+8
                if alias==6:address=PROVIDER+0x10
                add(t,8,(PROVIDER,KEY,address,size),'provider indexed%d position%d alias%d'%(indexed,position,alias))
    for position in (-1,0,2):
        for alias in range(10):
            t=blank(original);data_scene(t,1,position)
            address,size=OUTPUT,OUTPUT+4
            if alias==1:address=0
            if alias==2:size=0
            if alias==3:address=size=0
            if alias==4:address=size=OUTPUT
            if alias==5:address=INDEX+0x48
            if alias==6:address=INDEXED_PROVIDER+0x1C0
            if alias==7:size=INDEXED_PROVIDER+0x88
            if alias==8:address=INDEXED_PROVIDER+0x98
            if alias==9:size=INDEX+0x44
            add(t,9,(INDEXED_PROVIDER,0,KEY,address,size),'indexed provider position%d alias%d'%(position,alias))
    t=blank(original);data_scene(t,1,0);put(t,INDEX,0);add(t,9,(INDEXED_PROVIDER,0,KEY,OUTPUT,OUTPUT+4),'indexed provider empty')
    return cases

def golden_header(cases):
    lines=['/* Authored low32 alias fixtures; no original code/data arrays. */',
           'struct RegistryGolden { u32 function,args[5],global,expected_global,result; u32 initial[%d],expected[%d]; };'%(WORDS,WORDS),
           'static const struct RegistryGolden registry_golden[] = {']
    for c in cases:
        words=lambda values:','.join('0x%08Xu'%v for v in values)
        lines.append(' {%du,{%s},0x%08Xu,0x%08Xu,0x%08Xu,{%s},{%s}},'%(c['function'],words(c['args']),c['global'],c['expected_global'],c['result'],words(c['initial']),words(c['expected'])))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/resource_registry_trace.json');p.add_argument('--golden-header',type=Path)
    args=p.parse_args();_,original=validated_elf(args.elf);cases=fixtures(original)
    inputs=[{k:c[k] for k in ('function','args','global','initial','label')} for c in cases]
    digest=hashlib.sha256(json.dumps(inputs,separators=(',',':')).encode()).hexdigest()
    report={'limitation':'Low32 scalar instructions and authored valid memory only; no full resource graph, allocator safety, original class identity or EE timing claim. All 13 production routines and existing accessor execute; no call substitutions.','synthetic_input_sha256':digest,'cases':cases}
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(cases))
    print('%d registry fixtures; %d instructions; max%d; input%s'%(len(cases),sum(c['instruction_count'] for c in cases),max(c['instruction_count'] for c in cases),digest))

if __name__=='__main__':main()
