"""Bounded initialized ordinary-memory observations of five original leaves.

Generic licensed C is compared only on initialized padded/disjoint fixtures.
This is not a complete EE emulator, memory-fault/timing/MMIO model or SDK claim.
The published lifecycle decoder is inherited unchanged; local PXOR follows the
manufacturer EE Core Instruction Set Manual v6 p285 and its reviewed encoding.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,U128,sx32
from trace_file_archive_lifecycle import LifecycleTrace
ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x393378,0x393464),(0x3936A0,0x393758),(0x393C90,0x393E44),
        (0x393E48,0x394010),(0x394010,0x3941D8))
BASES=(0x20000,0x20400,0x20800);SIZE=512
KEYS=('routine','count','character','offset1','offset2','offsetd','length1','length2','dest_length','difference','difference_value','pattern')

class RuntimeStringsTrace(LifecycleTrace):
    def __init__(self,original):
        RegistryTrace.__init__(self,original,{})
        self.reads=[];self.writes=[]
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not any(base<=a<a+n<=base+SIZE for base in BASES):
            raise ValueError('runtime strings unowned or unaligned memory %#x/%d'%(a,n))
    def load(self,a,n):
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('runtime strings uninitialized read')
        self.reads.append((a,n));return Trace.load(self,a,n)
    def save(self,a,v,n):
        self.memory_check(a,n);self.writes.append((a,n));return Trace.save(self,a,v,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('runtime strings unreviewed instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('runtime strings code image bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def execute(self,w,pc):
        if w&0xFC0007FF==0x700004C9:
            if self.instruction_count>=250000:raise ValueError('runtime strings instruction bound')
            self.instruction_count+=1;self.visited.add(pc)
            rs,rt,rd=w>>21&31,w>>16&31,w>>11&31
            a,b=self.r[rs],self.r[rt]
            self.r[rd]=(a^b)&U128
            self.r[0]=0;return None,False
        return LifecycleTrace.execute(self,w,pc)
    def run(self,entry,stop=RETURN):
        interval=next((r for r in RANGES if r[0]==entry),None)
        if interval is None:raise ValueError('runtime strings unreviewed entry')
        lo,hi=interval;pc=lo
        while True:
            if not lo<=pc<hi:raise ValueError('runtime strings transfer outside full body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('runtime strings taken annul')
                    pc+=8;continue
                if pc+4>=hi:raise ValueError('runtime strings delay outside body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('runtime strings control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('runtime strings actual JR31/stop')
                    return
                if w>>26 in (2,3):raise ValueError('runtime strings unreviewed leaf call')
                pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('runtime strings unexpected transfer')
                pc+=4

def payload(pattern,i):
    return (65,255,128,(127,128,255,1)[i&3],(1,128,254,127)[i&3])[pattern]

def initial_buffers(p):
    routine,count,c,o1,o2,od,l1,l2,dl,diff,dv,pat=[p[k] for k in KEYS]
    if type(routine) is not int or routine not in range(5) or type(count) is not int or not 0<=count<=256:raise ValueError('runtime strings fixture routine/count')
    if any(type(v) is not int or not 0<=v<16 for v in (o1,o2,od)) or pat not in range(5):raise ValueError('runtime strings fixture alignment/pattern')
    if any(type(v) is not int or not -1<=v<=256 for v in (l1,l2,diff)) or type(dl) is not int or not 0<=dl<=96 or type(c) is not int or not -(1<<31)<=c<1<<31 or not 0<=dv<=255:raise ValueError('runtime strings fixture byte/count domain')
    result=[bytearray(((i*37+buffer*59)^0xA5)&255 for i in range(SIZE)) for buffer in range(3)]
    if routine==0:
        fill=0x5A if c&255==0xA5 else 0xA5
        for i in range(288):result[0][o1+i]=fill
        if l1>=0:result[0][o1+l1]=c&255
    elif routine in (2,3,4):
        for buffer,offset,length in ((0,o1,l1),(1,o2,l2)):
            for i in range(288):result[buffer][offset+i]=payload(pat,i)
            if length>=0:result[buffer][offset+length]=0
        if diff>=0:result[1][o2+diff]=dv
        if routine==2:
            for i in range(dl):result[2][od+i]=payload(pat,i)
            result[2][od+dl]=0
    return result

def defined_subtraction_subset(p,buffers,width):
    # Arithmetic-only subset; aligned long casts still use the explicit GNU
    # aliasing contract. This is not universal ISO-C char-array type validity.
    if p['routine']!=4 or (p['offset1']|p['offsetd'])&(width-1):return True
    remaining=p['count'];offset=p['offset1'];ones=int.from_bytes(b'\1'*width,'little');limit=1<<(8*width-1)
    while remaining>=width:
        block=buffers[0][offset:offset+width];x=int.from_bytes(block,'little',signed=True)
        if not -limit<=x-ones<limit:return False
        if 0 in block:return True
        remaining-=width;offset+=width
    return True

def logical_byte_reference(p,buffers):
    # Independent ordinary-memory value specification, never substitutes for
    # the original instruction execution used by fixture().
    out=[bytearray(b) for b in buffers];n=p['count'];o1,o2,od=(p[k] for k in ('offset1','offset2','offsetd'));routine=p['routine'];result=0
    if routine==0:
        for i in range(n):
            if out[0][o1+i]==p['character']&255:result=BASES[0]+o1+i;break
    elif routine==1:
        out[2][od:od+n]=bytes([p['character']&255])*n;result=BASES[2]+od
    elif routine==2:
        end=od
        while out[2][end]:end+=1
        for i in range(n):
            v=out[1][o2+i];out[2][end+i]=v
            if not v:break
        else:
            if n:out[2][end+n]=0
        result=BASES[2]+od
    elif routine==3:
        for i in range(n):
            a,b=out[0][o1+i],out[1][o2+i]
            if a!=b:result=(a-b)&MASK;break
            if not a:break
    else:
        zero=False
        for i in range(n):
            v=0 if zero else out[0][o1+i];out[2][od+i]=v
            if not v:zero=True
        result=BASES[2]+od
    return out,result

def fixture(raw,**arguments):
    p=dict(zip(KEYS,(0,0,0,0,0,0,-1,-1,0,-1,0,0)));p.update(arguments)
    if set(p)!=set(KEYS):raise ValueError('runtime strings fixture keys')
    buffers=initial_buffers(p);t=RuntimeStringsTrace(raw)
    for base,buf in zip(BASES,buffers):
        for i,v in enumerate(buf):t.save(base+i,v,1)
    t.writes.clear();t.reads.clear();routine=p['routine'];o1,o2,od=(p[k] for k in ('offset1','offset2','offsetd'))
    if routine==0:args=(BASES[0]+o1,p['character'],p['count'])
    elif routine==1:args=(BASES[2]+od,p['character'],p['count'])
    elif routine==2:args=(BASES[2]+od,BASES[1]+o2,p['count'])
    elif routine==3:args=(BASES[0]+o1,BASES[1]+o2,p['count'])
    else:args=(BASES[2]+od,BASES[0]+o1,p['count'])
    for r,a in enumerate(args,4):t.r[r]=sx32(a)
    t.r[29]=0x80000;t.r[31]=RETURN
    preserved=[0xAABBCCDD000000001122334400000000+i for i in range(16,24)]
    for r,v in zip(range(16,24),preserved):t.r[r]=v
    t.run(RANGES[routine][0]);assert t.r[29]==0x80000 and [t.r[r] for r in range(16,24)]==preserved and t.r[31]==RETURN
    actual=[bytearray(t.memory[base+i] for i in range(SIZE)) for base in BASES]
    reference,result=logical_byte_reference(p,buffers)
    assert actual==reference and t.r[2]&MASK==result,(p,t.r[2]&MASK,result)
    changes=[]
    for buffer,(initial,final) in enumerate(zip(buffers,actual)):
        i=0
        while i<SIZE:
            if initial[i]==final[i]:i+=1;continue
            start=i;v=final[i];i+=1
            while i<SIZE and initial[i]!=final[i] and final[i]==v:i+=1
            changes.append([buffer*SIZE+start,i-start,v])
    return dict(parameters=p,result=result,changes=changes,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,reads=t.reads,writes=t.writes,
                target_defined_signed_subtraction=defined_subtraction_subset(p,buffers,8),native32_defined_signed_subtraction=defined_subtraction_subset(p,buffers,4),
                initial_sha256=hashlib.sha256(b''.join(buffers)).hexdigest(),expected_sha256=hashlib.sha256(b''.join(actual)).hexdigest())

def fixtures(raw):
    result=[];seen=set()
    def add(**kwargs):
        p=dict(zip(KEYS,(0,0,0,0,0,0,-1,-1,0,-1,0,0)));p.update(kwargs);key=tuple(p[k] for k in KEYS)
        if key not in seen:seen.add(key);result.append(fixture(raw,**p))
    counts=(0,1,7,8,9,15,16,17,31,32,33,63,64,65,127,128,129,255,256)
    for offset in range(16):
        for n in counts:
            for c in (0,128,255):
                for pos in (-1,0,n//2,n-1):add(routine=0,count=n,character=c,offset1=offset,length1=pos)
            for c in (0,128,255):add(routine=1,count=n,character=c,offsetd=offset)
    for n in (0,7,8,16,17,32,65,256):
        for c in (1,127,-1,0x1234,-2147483648,2147483647):
            add(routine=0,count=n,character=c,length1=n//2)
            add(routine=1,count=n,character=c)
    for offset in range(16):
        for n in (0,1,8,16,17,33,127):
            for pat in range(3):
                for length in (0,1,n//2,n+1):
                    add(routine=2,count=n,offset2=(offset*7)&15,offsetd=offset,length2=length,dest_length=(n*3)%65,pattern=pat)
    for offset in range(16):
        for n in (0,1,7,8,15,16,17,31,32,33,65,256):
            for pat in ((offset+n)%5,):
                for pos in (-1,0,n//2,n-1):
                    add(routine=3,count=n,offset1=offset,offset2=(offset*7)&15,length1=pos,length2=pos,pattern=pat)
                    add(routine=3,count=n,offset1=offset,offset2=(offset*7)&15,difference=pos,difference_value=0 if pat==0 else 65,pattern=pat)
                    add(routine=4,count=n,offset1=offset,offsetd=(offset*7)&15,length1=pos,pattern=pat)
    for dest_length in (0,1,7,8,15,16,17,31,32,63):
        for offset in (0,8,1):
            add(routine=2,count=17,offset2=offset,offsetd=offset,length2=9,dest_length=dest_length)
    # Distinct simultaneous long/quad alignment and count edges across both
    # source/destination operands, plus each NUL position in two full blocks.
    for o1 in (0,8,1):
        for o2 in (0,8,1):
            for pos in range(33):
                for pat in (0,1,2):
                    add(routine=3,count=65,offset1=o1,offset2=o2,length1=pos,length2=pos,pattern=pat)
                    add(routine=4,count=65,offset1=o1,offsetd=o2,length1=pos,pattern=pat)
    return result

def header(rows):
    runs=[];records=[]
    for row in rows:
        start=len(runs);runs.extend(row['changes']);params=[row['parameters'][k] for k in KEYS]
        records.append('{{%s},0x%08Xu,%du,%du,%du,%du}'%(','.join('(-2147483647-1)' if x==-2147483648 else str(x) for x in params),row['result'],start,len(row['changes']),row['target_defined_signed_subtraction'],row['native32_defined_signed_subtraction']))
    return ('/* Generated only from synthetic initialized/padded ordinary-memory inputs. */\n'
            '#ifndef GEORGE_RUNTIME_STRINGS_GOLDEN_H\n#define GEORGE_RUNTIME_STRINGS_GOLDEN_H\n'
            'typedef struct { unsigned short offset,length; unsigned char value; } StringsRun;\n'
            'typedef struct { int p[12]; unsigned int result,start,count,target_defined,native_defined; } StringsGolden;\n'
            'static const StringsRun strings_runs[]={\n'+',\n'.join('{%d,%d,%d}'%tuple(r) for r in runs)+'\n};\n'
            'static const StringsGolden strings_golden[]={\n'+',\n'.join(records)+'\n};\n'
            '#endif\n')

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--golden-header',type=Path);parser.add_argument('--output',type=Path,default=ROOT/'build/runtime_strings/trace.json');args=parser.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');rows=fixtures(raw)
    visited=sorted({pc for row in rows for pc in row['visited']});owned={pc for a,b in RANGES for pc in range(a,b,4)}
    result=dict(fixtures=rows,summary=dict(count=len(rows),instructions=sum(x['instructions'] for x in rows),max_instructions=max(x['instructions'] for x in rows),all_selected_instructions=len(owned),executed_selected_instructions=len(visited),unexecuted_selected=sorted(owned-set(visited)),target_arithmetic_subset=sum(x['target_defined_signed_subtraction'] for x in rows),native32_arithmetic_subset=sum(x['native32_defined_signed_subtraction'] for x in rows),input_sha256=hashlib.sha256(json.dumps([x['parameters'] for x in rows],sort_keys=True,separators=(',',':')).encode()).hexdigest(),limitations='Finite initialized padded/disjoint ordinary memory. GNU native -fwrapv/-fno-strict-aliasing compiler contract; separate arithmetic-defined subset, no EE timing/page-fault/MMIO/concurrency or exact C compiler identity.'))
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(result,indent=2)+'\n',newline='\n')
    if args.golden_header:args.golden_header.write_text(header(rows),newline='\n')
    print(json.dumps(result['summary'],indent=2))
if __name__=='__main__':main()
