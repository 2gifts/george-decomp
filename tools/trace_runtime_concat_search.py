"""Initialized padded observations of actual strcat/strchr and full strcpy.

Reuse the reviewed runtime-strings execution decoder without new instructions.
Only strcat's actual JAL to strcpy is callable; this is no full EE emulator.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_runtime_strings import RuntimeStringsTrace,BASES,SIZE,payload,MASK,sx32
ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x393758,0x393888),(0x393888,0x393A28),(0x393B74,0x393C8C))
STACK=0x80000
KEYS=('routine','offset_source','offset_destination','length','dest_length','pattern','character','match')

class ConcatSearchTrace(RuntimeStringsTrace):
    def __init__(self,original):
        super().__init__(original);self.calls=[]
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (any(base<=a<a+n<=base+SIZE for base in BASES) or STACK-32<=a<a+n<=STACK):
            raise ValueError('concat/search unowned or unaligned memory %#x/%d'%(a,n))
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('concat/search unreviewed instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('concat/search code image bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def run(self,entry,stop=RETURN):
        interval=next((r for r in RANGES if r[0]==entry),None)
        if interval is None:raise ValueError('concat/search unreviewed entry')
        lo,hi=interval;pc=lo
        while True:
            if not lo<=pc<hi:raise ValueError('concat/search transfer outside full body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('concat/search taken annul')
                    pc+=8;continue
                if pc+4>=hi:raise ValueError('concat/search delay outside body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('concat/search control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('concat/search actual JR31/stop')
                    return
                if w>>26 in (2,3):
                    if w>>26!=3 or entry!=0x393758 or pc!=0x39386C or target!=0x393B74:raise ValueError('concat/search unreviewed call')
                    self.calls.append(dict(pc=pc,target=target,arguments=[self.r[4]&MASK,self.r[5]&MASK]))
                    self.run(target,pc+8);pc+=8;continue
                pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('concat/search unexpected transfer')
                pc+=4

def parameters(**arguments):
    p=dict(zip(KEYS,(0,0,0,0,0,0,0,-1)));p.update(arguments)
    if set(p)!=set(KEYS) or any(type(v) is not int for v in p.values()):raise ValueError('concat/search fixture keys/type')
    if p['routine'] not in (0,1) or not 0<=p['length']<=256 or not 0<=p['dest_length']<=128:raise ValueError('concat/search routine/length')
    if any(not 0<=p[k]<16 for k in ('offset_source','offset_destination')) or p['pattern'] not in range(5):raise ValueError('concat/search alignment/pattern')
    if not -(1<<31)<=p['character']<1<<31 or not -1<=p['match']<p['length']:raise ValueError('concat/search character/match')
    return p

def initial_buffers(p):
    b=[bytearray(((i*37+j*59)^0xA5)&255 for i in range(SIZE)) for j in range(3)]
    o=p['offset_source'];d=p['offset_destination']
    for i in range(288):b[0][o+i]=payload(p['pattern'],i)
    b[0][o+p['length']]=0
    if p['match']>=0:
        if not p['character']&255:raise ValueError('concat/search inserted match would shorten string')
        b[0][o+p['match']]=p['character']&255
    for i in range(p['dest_length']):b[2][d+i]=payload((p['pattern']+1)%5,i)
    b[2][d+p['dest_length']]=0
    return b

def logical_reference(p,b):
    out=[bytearray(x) for x in b];o=p['offset_source'];d=p['offset_destination'];result=0
    if p['routine']==0:
        for i in range(p['length']+1):out[2][d+p['dest_length']+i]=out[0][o+i]
        result=BASES[2]+d
    else:
        for i in range(p['length']+1):
            if out[0][o+i]==p['character']&255:result=BASES[0]+o+i;break
    return out,result

def fixture(raw,**arguments):
    p=parameters(**arguments);b=initial_buffers(p);t=ConcatSearchTrace(raw)
    for base,buf in zip(BASES,b):
        for i,v in enumerate(buf):t.save(base+i,v,1)
    for i in range(32):t.save(STACK-32+i,(i*17)^0xA5,1)
    t.reads.clear();t.writes.clear();t.r[29]=STACK;t.r[31]=RETURN
    preserved=[0xAABBCCDD000000001122334400000000+i for i in range(16,24)]
    for r,v in zip(range(16,24),preserved):t.r[r]=v
    args=(BASES[2]+p['offset_destination'],BASES[0]+p['offset_source']) if not p['routine'] else (BASES[0]+p['offset_source'],p['character'])
    for r,a in enumerate(args,4):t.r[r]=sx32(a)
    t.run(RANGES[p['routine']][0]);assert t.r[29]==STACK and t.r[31]==RETURN and [t.r[r] for r in range(16,24)]==preserved
    out=[bytearray(t.memory[base+i] for i in range(SIZE)) for base in BASES];reference,result=logical_reference(p,b)
    assert out==reference and t.r[2]&MASK==result,(p,result,t.r[2]&MASK)
    changes=[]
    for j,(a,z) in enumerate(zip(b,out)):
        i=0
        while i<SIZE:
            if a[i]==z[i]:i+=1;continue
            start=i;v=z[i];i+=1
            while i<SIZE and a[i]!=z[i] and z[i]==v:i+=1
            changes.append([j*SIZE+start,i-start,v])
    return dict(parameters=p,result=result,changes=changes,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,reads=t.reads,writes=t.writes,calls=t.calls,initial_sha256=hashlib.sha256(b''.join(b)).hexdigest(),expected_sha256=hashlib.sha256(b''.join(out)).hexdigest())

def fixtures(raw):
    rows=[];seen=set()
    def add(**kw):
        p=parameters(**kw);key=tuple(p[k] for k in KEYS)
        if key not in seen:seen.add(key);rows.append(fixture(raw,**p))
    lengths=(0,1,7,8,9,15,16,17,31,32,33,63,64,65,127,128,129,255,256)
    for o in range(16):
        for n in lengths:
            for pat in (0,1,2):
                add(offset_source=o,offset_destination=(o*7)&15,length=n,dest_length=(n*3)%129,pattern=pat)
                for c in (0,1,65,127,128,255):add(routine=1,offset_source=o,length=n,pattern=pat,character=c)
    for o in (0,8,1):
        for n in range(33):
            for m in range(-1,n):
                for c in (1,128,255):add(routine=1,offset_source=o,length=n,pattern=0,character=c,match=m)
        for od in (0,8,1):
            for dl in range(33):add(offset_source=o,offset_destination=od,length=33,dest_length=dl,pattern=4)
    for c in (-2147483648,-257,-256,-129,-1,256,257,383,0x1234,2147483647):
        for o in (0,8,1):add(routine=1,offset_source=o,length=65,pattern=3,character=c)
    return rows

def header(rows):
    runs=[];records=[]
    for row in rows:
        start=len(runs);runs.extend(row['changes']);p=[row['parameters'][k] for k in KEYS]
        records.append('{{%s},0x%08Xu,%du,%du}'%(','.join('(-2147483647-1)' if x==-2147483648 else str(x) for x in p),row['result'],start,len(row['changes'])))
    return ('/* Synthetic initialized padded inputs; original instruction outputs. */\n#ifndef GEORGE_CONCAT_SEARCH_GOLDEN_H\n#define GEORGE_CONCAT_SEARCH_GOLDEN_H\ntypedef struct { unsigned short offset,length; unsigned char value; } ConcatRun;\ntypedef struct { int p[8]; unsigned int result,start,count; } ConcatGolden;\nstatic const ConcatRun concat_runs[]={\n'+',\n'.join('{%d,%d,%d}'%tuple(r) for r in runs)+'\n};\nstatic const ConcatGolden concat_golden[]={\n'+',\n'.join(records)+'\n};\n#endif\n')

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--golden-header',type=Path);parser.add_argument('--output',type=Path,default=ROOT/'build/runtime_concat_search/trace.json');a=parser.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');rows=fixtures(raw);visited={pc for r in rows for pc in r['visited']};selected={pc for lo,hi in RANGES[:2] for pc in range(lo,hi,4)}
    summary=dict(count=len(rows),instructions=sum(r['instructions'] for r in rows),max_instructions=max(r['instructions'] for r in rows),selected_instructions=len(selected),executed_selected_instructions=len(visited&selected),unexecuted_selected=sorted(selected-visited),support_instructions=len(visited-selected),input_sha256=hashlib.sha256(json.dumps([r['parameters'] for r in rows],sort_keys=True,separators=(',',':')).encode()).hexdigest(),limitations='Finite initialized padded disjoint char objects and representable low32 addresses; real whole strcpy original executes. No faults/MMIO/overlap/capacity/timing/EE emulator or original C/compiler identity claim.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(dict(fixtures=rows,summary=summary),indent=2)+'\n',newline='\n')
    if a.golden_header:a.golden_header.write_text(header(rows),newline='\n')
    print(json.dumps(summary,indent=2))
if __name__=='__main__':main()
