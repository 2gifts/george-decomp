"""Initialized padded observations of actual strcmp/strcpy.

Reuse the reviewed runtime-strings execution decoder without new instructions.
Both entire leaves execute; no external engine hooks or new instruction decoder.
This is no full EE emulator, fault/MMIO or arbitrary-overlap claim.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_runtime_strings import RuntimeStringsTrace,BASES,SIZE,payload,MASK,sx32
ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x393A28,0x393B74),(0x393B74,0x393C8C))
STACK=0x80000
KEYS=('routine','offset_left','offset_right','offset_destination','length_left','length_right','pattern','difference','value')

class CopyCompareTrace(RuntimeStringsTrace):
    def __init__(self,original):
        super().__init__(original);self.calls=[]
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (any(base<=a<a+n<=base+SIZE for base in BASES)):
            raise ValueError('copy/compare unowned or unaligned memory %#x/%d'%(a,n))
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('copy/compare unreviewed instruction')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('copy/compare code image bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def run(self,entry,stop=RETURN):
        interval=next((r for r in RANGES if r[0]==entry),None)
        if interval is None:raise ValueError('copy/compare unreviewed entry')
        lo,hi=interval;pc=lo
        while True:
            if not lo<=pc<hi:raise ValueError('copy/compare transfer outside full body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('copy/compare taken annul')
                    pc+=8;continue
                if pc+4>=hi:raise ValueError('copy/compare delay outside body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('copy/compare control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('copy/compare actual JR31/stop')
                    return
                if w>>26 in (2,3):raise ValueError('copy/compare unreviewed leaf call')
                pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('copy/compare unexpected transfer')
                pc+=4

def parameters(**kw):
    p=dict(zip(KEYS,(0,0,0,0,0,0,0,-1,1)));p.update(kw)
    if set(p)!=set(KEYS) or any(type(x) is not int for x in p.values()):raise ValueError('copy/compare parameter types')
    if p['routine'] not in (0,1) or any(not 0<=p[k]<16 for k in ('offset_left','offset_right','offset_destination')):raise ValueError('copy/compare routine/alignment')
    if any(not 0<=p[k]<=256 for k in ('length_left','length_right')) or p['pattern'] not in range(5):raise ValueError('copy/compare length/pattern')
    if not -1<=p['difference']<=256 or not 0<=p['value']<=255:raise ValueError('copy/compare difference/value')
    if p['routine']==1 and p['difference']!=-1:raise ValueError('copy/compare copy mutation')
    return p

def initial_buffers(p):
    b=[bytearray(((i*37+j*59)^0xA5)&255 for i in range(SIZE)) for j in range(3)]
    for j,key in enumerate(('offset_left','offset_right')):
        offset=p[key]
        for i in range(288):b[j][offset+i]=payload(p['pattern'],i)
        b[j][offset+p['length_left' if j==0 else 'length_right']]=0
    if p['difference']>=0:b[1][p['offset_right']+p['difference']]=p['value']
    return b

def signed_subtraction_subset(p,b,width):
    # Arithmetic subset only, distinct from the measured GNU aligned-cast/
    # alias contract and from the native -fwrapv implementation observation.
    if not p['routine'] or (p['offset_left']|p['offset_destination'])&(width-1):return True
    offset=p['offset_left'];ones=int.from_bytes(b'\1'*width,'little');limit=1<<(width*8-1)
    while True:
        block=b[0][offset:offset+width];value=int.from_bytes(block,'little',signed=True)
        if not -limit<=value-ones<limit:return False
        if 0 in block:return True
        offset+=width

def logical_reference(p,b):
    out=[bytearray(x) for x in b];a=p['offset_left'];z=p['offset_right']
    if not p['routine']:
        while out[0][a] and out[0][a]==out[1][z]:a+=1;z+=1
        result=(out[0][a]-out[1][z])&MASK
    else:
        dest=p['offset_destination'];result=BASES[2]+dest
        while True:
            v=out[0][a];out[2][dest]=v;a+=1;dest+=1
            if not v:break
    return out,result

def fixture(raw,**kw):
    p=parameters(**kw);b=initial_buffers(p);t=CopyCompareTrace(raw)
    for base,buf in zip(BASES,b):
        for i,v in enumerate(buf):t.save(base+i,v,1)
    t.reads.clear();t.writes.clear();t.r[29]=STACK;t.r[31]=RETURN
    preserved=[0xAABBCCDD000000001122334400000000+i for i in range(16,24)]
    for reg,v in zip(range(16,24),preserved):t.r[reg]=v
    args=(BASES[0]+p['offset_left'],BASES[1]+p['offset_right']) if not p['routine'] else (BASES[2]+p['offset_destination'],BASES[0]+p['offset_left'])
    for reg,a in enumerate(args,4):t.r[reg]=sx32(a)
    t.run(RANGES[p['routine']][0]);assert t.r[29]==STACK and t.r[31]==RETURN and [t.r[reg] for reg in range(16,24)]==preserved
    out=[bytearray(t.memory[base+i] for i in range(SIZE)) for base in BASES];expected,result=logical_reference(p,b)
    assert out==expected and t.r[2]&MASK==result,(p,result,t.r[2]&MASK)
    changes=[]
    for j,(old,new) in enumerate(zip(b,out)):
        i=0
        while i<SIZE:
            if old[i]==new[i]:i+=1;continue
            start=i;v=new[i];i+=1
            while i<SIZE and old[i]!=new[i] and new[i]==v:i+=1
            changes.append([j*SIZE+start,i-start,v])
    return dict(parameters=p,result=result,changes=changes,instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,reads=t.reads,writes=t.writes,calls=t.calls,signed_subtraction_subset={'long32':signed_subtraction_subset(p,b,4),'long64':signed_subtraction_subset(p,b,8)},initial_sha256=hashlib.sha256(b''.join(b)).hexdigest(),expected_sha256=hashlib.sha256(b''.join(out)).hexdigest())

def fixtures(raw):
    rows=[];seen=set()
    def add(**kw):
        p=parameters(**kw);key=tuple(p[k] for k in KEYS)
        if key not in seen:seen.add(key);rows.append(fixture(raw,**p))
    lengths=(0,1,7,8,9,15,16,17,31,32,33,63,64,65,127,128,129,255,256)
    # All ordinary offsets, both alignment decisions, empty/long strings and
    # high-byte arithmetic inputs. Optimized sources remain active.
    for o in range(16):
        for n in lengths:
            for pat in range(5):
                add(routine=1,offset_left=o,offset_destination=(o*7)&15,length_left=n,pattern=pat)
                for right in (o,(o*7)&15):
                    add(offset_left=o,offset_right=right,length_left=n,length_right=n,pattern=pat)
                    for other in (max(0,n-1),min(256,n+1)):
                        add(offset_left=o,offset_right=right,length_left=n,length_right=other,pattern=pat)
    for left in (0,8,1):
        for right in (0,8,1):
            for n in range(33):
                for pos in range(n+1):
                    for value in (0,1,65,127,128,255):add(offset_left=left,offset_right=right,length_left=n,length_right=n,pattern=3,difference=pos,value=value)
        for dest in (0,8,1):
            for n in range(33):
                for pat in range(5):add(routine=1,offset_left=left,offset_destination=dest,length_left=n,pattern=pat)
    # Every byte value at the first difference, including exact negative and
    # positive differences outside a sign-only comparator's possible values.
    for value in range(256):
        for left,right in ((0,0),(8,8),(1,3)):
            add(offset_left=left,offset_right=right,length_left=33,length_right=33,pattern=0,difference=16,value=value)
    return rows

def header(rows):
    runs=[];records=[]
    for row in rows:
        start=len(runs);runs.extend(row['changes']);p=[row['parameters'][k] for k in KEYS]
        records.append('{{%s},0x%08Xu,%du,%du,%du,%du}'%(','.join(str(x) for x in p),row['result'],start,len(row['changes']),row['signed_subtraction_subset']['long32'],row['signed_subtraction_subset']['long64']))
    return ('/* Synthetic initialized padded inputs; full original instruction outputs. */\n#ifndef GEORGE_COPY_COMPARE_GOLDEN_H\n#define GEORGE_COPY_COMPARE_GOLDEN_H\ntypedef struct { unsigned short offset,length; unsigned char value; } CopyCompareRun;\ntypedef struct { int p[9]; unsigned int result,start,count; unsigned char subset32,subset64; } CopyCompareGolden;\nstatic const CopyCompareRun copy_compare_runs[]={\n'+',\n'.join('{%d,%d,%d}'%tuple(r) for r in runs)+'\n};\nstatic const CopyCompareGolden copy_compare_golden[]={\n'+',\n'.join(records)+'\n};\n#endif\n')

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--golden-header',type=Path);parser.add_argument('--output',type=Path,default=ROOT/'build/runtime_copy_compare/trace.json');a=parser.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');rows=fixtures(raw);visited={pc for r in rows for pc in r['visited']};selected={pc for lo,hi in RANGES for pc in range(lo,hi,4)}
    summary=dict(count=len(rows),instructions=sum(r['instructions'] for r in rows),max_instructions=max(r['instructions'] for r in rows),selected_instructions=len(selected),executed_selected_instructions=len(visited&selected),unexecuted_selected=sorted(selected-visited),signed_subtraction_subsets={name:sum(r['signed_subtraction_subset'][name] for r in rows) for name in ('long32','long64')},input_sha256=hashlib.sha256(json.dumps([r['parameters'] for r in rows],sort_keys=True,separators=(',',':')).encode()).hexdigest(),limitations='Initialized padded disjoint char storage, representable low32 pointers, GNU aligned long-cast contract. Optimized generic long32/native vs long64/retailLD-LQ schedules differ. strcpy signed arithmetic subset is separately recorded; native -fwrapv extension is not universal ISO C. No source/compiler/hardware/fault/MMIO/overlap/capacity claim.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(dict(fixtures=rows,summary=summary),indent=2)+'\n',newline='\n')
    if a.golden_header:a.golden_header.write_text(header(rows),newline='\n')
    print(json.dumps(summary,indent=2))
if __name__=='__main__':main()
