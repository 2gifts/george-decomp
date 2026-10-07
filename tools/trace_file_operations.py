"""Bounded original file wrappers with real registry/string/heap/map helpers.

The reviewed RegistryTrace integer/MMI decoder is reused unchanged. SDK I/O,
cache, DMA and core heap/free effects are explicit terminating controls, not
kernel execution, device ownership or a PS2 hardware/emulator equivalence.
"""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,U64,sx32,signed

ROOT=Path(__file__).resolve().parents[1]
SELECTED=((0x2B0F60,0x2B10D4),(0x2B1138,0x2B1194),(0x2B1198,0x2B11D4),
 (0x2B11F0,0x2B125C),(0x2B1408,0x2B1468),(0x2B1468,0x2B1508),
 (0x2B0710,0x2B084C),(0x2B10D8,0x2B1134),(0x2B1260,0x2B12C0),(0x2B1510,0x2B15A4))
SUPPORT=((0x2ABAE8,0x2ABDE8),(0x2B1BA8,0x2B1C04),(0x2B1AF0,0x2B1AF8),(0x2B1AF8,0x2B1B00),
 (0x2AEB60,0x2AEC28),(0x2AEC28,0x2AECD0),(0x2AEE40,0x2AEE5C),
 (0x295050,0x295080),(0x29C648,0x29C6C0),(0x2A7C08,0x2A7C58),
 (0x3934F8,0x3935A4),(0x393758,0x393888),(0x393A28,0x393B74),
 (0x393B74,0x393C8C),(0x3984D8,0x398554),(0x398628,0x39869C),(0x39CA58,0x39CA78))
STUBS=((0x363AE0,100),(0x363C00,118),(0x363C20,119))
RANGES=SELECTED+SUPPORT+tuple((a,a+16) for a,n in STUBS)
BASE,WORDS=0x20000,2048;END=BASE+4*WORDS;STACK=(0x7D000,0x81000)
PATH=0x20401;DATA=0x20800;HEAP=0x20A00;ALT_HEAP=0x20A40;TABLE=0x20B00
KEY=0x20B81;VALUE=0x20C01;CONTEXT=0x20D00;ALT_CONTEXT=0x20D80
MAP=0x20E00;BUCKETS=0x20E40;NODE=0x20E80;RECORD=0x20F00;ALLOCATION=0x21200
SLOTS=0x469BD0;COUNT,MODE,CONTEXT_GLOBAL=0x3FD23C,0x3FD240,0x3FD244
REG_COUNT,REG_CAP,REG_TABLE,HEAP_GLOBAL=0x3FD1D8,0x3FD1DC,0x3FD1E0,0x3FD204
GLOBALS=(COUNT,MODE,CONTEXT_GLOBAL,REG_COUNT,REG_CAP,REG_TABLE,HEAP_GLOBAL)
TEXT_GLOBALS=(0x469A00,0x469A80,0x469B00)
READONLY=((0x445650,0x445A50),(0x446FE0,0x447010),(0x4473C0,0x4473C8),(0x456118,0x456219))
initial_word=lambda i:(0xA50B0301^(i*0x10203))&MASK

def text_hash(text):
    value=2166136261
    for b in text:value=((value^b)*16777619)&MASK
    return value

class FileTrace(RegistryTrace):
    def __init__(self,original,p):
        super().__init__(original,p);self.events=[];self.invocations=[0]*len(RANGES)
        self.io_counts=[0]*10;self.dma_calls=self.polls=self.cache_calls=self.core_calls=0
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
            BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or
            SLOTS<=a<a+n<=SLOTS+560 or any(g<=a<a+n<=g+4 for g in GLOBALS) or
            any(g<=a<a+n<=g+128 for g in TEXT_GLOBALS)):
            raise ValueError('unowned/unaligned file observer memory %#x/%d'%(a,n))
    def load(self,a,n):
        if any(lo<=a<a+n<=hi for lo,hi in READONLY):
            if a%n:raise ValueError('unaligned file readonly')
            off=a-0xFF000
            if off<0 or off+n>len(self.original):raise ValueError('file readonly image bound')
            self.readonly_reads.add((a,n));return int.from_bytes(self.original[off:off+n],'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized file observer read')
        return Trace.load(self,a,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed file instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('file code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def event(self,kind,args):
        slot=SLOTS+28*self.p['slot']
        words=[kind,*args,self.load(COUNT,4),self.load(MODE,4),self.load(CONTEXT_GLOBAL,4),
               *[self.load(slot+4*i,4) for i in range(7)]]
        assert len(words)==16
        if len(self.events)>=160:raise ValueError('file event bound')
        self.events.append([v&MASK for v in words]);self.io_counts[kind]+=1
    def clobber(self,result):
        for r in range(3,16):self.r[r]=0xCAFEBABE
        self.r[2]=sx32(result)
    def syscall(self,pc):
        expected={a+4:n for a,n in STUBS}
        if pc not in expected or self.r[3]&MASK!=expected[pc]:raise ValueError('unreviewed/wrong file syscall')
        number=expected[pc]
        if number==100:
            if self.r[4]&MASK:raise ValueError('file cache mode contract')
            self.event(2,[0,0,0,0,0]);self.cache_calls+=1
            if self.cache_calls==1:
                if self.p['mutation']==1:
                    self.save(COUNT,MASK,4);self.save(MODE,self.load(MODE,4)^1,4);self.save(CONTEXT_GLOBAL,ALT_CONTEXT,4)
                    self.save(PATH,ord('Z'),1)
                elif self.p['mutation']==6:self.save(SLOTS+28*self.p['slot'],0,4)
                elif self.p['mutation']==8:self.put(TEXT_GLOBALS[0],b'changed/')
            self.clobber(0x13579BDF)
        elif number==119:
            packet=self.r[4]&MASK
            if self.r[5]&MASK!=1 or not STACK[0]<=packet<packet+16<=STACK[1]:raise ValueError('file DMA packet contract')
            fields=[self.load(packet+4*i,4) for i in range(4)]
            self.event(8,[*fields,1]);self.dma_calls+=1
            if self.p['mutation']==5 and self.dma_calls==1:
                self.save(packet+4,0x12345678,4);self.save(packet+8,0x76543210,4);self.save(packet+12,0x55,4)
            if self.dma_calls>24:raise ValueError('nonterminating file DMA observer')
            self.clobber(0 if self.dma_calls%3==1 else 0x80000005)
        else:
            token=self.r[4]&MASK
            if token!=0x80000005:raise ValueError('file DMA status token contract')
            self.event(9,[token,0,0,0,0]);self.polls+=1
            if self.polls>48:raise ValueError('nonterminating file DMA status observer')
            self.clobber(0 if self.polls%2 else MASK)
    def execute(self,w,pc):
        if w==12:
            if self.instruction_count>=30000:raise ValueError('file instruction bound')
            self.instruction_count+=1;self.visited.add(pc);self.syscall(pc);self.r[0]=0
            return None,False
        return super().execute(w,pc)
    def external(self,target):
        a,b,c=(self.r[r]&MASK for r in (4,5,6));slot=SLOTS+28*self.p['slot']
        if target==0x2ADF60:
            if a not in (HEAP,ALT_HEAP) or c not in (3,6):raise ValueError('file core allocator contract')
            self.event(0,[a,b,c,0,0]);self.core_calls+=1
            if self.p['mutation']==7 and self.core_calls==1:self.save(HEAP+0x18,ALT_HEAP,4)
            if c==3:result=TABLE if not self.p['registry_fail'] else 0
            else:result=0 if self.core_calls<=self.p['failures'] else ALLOCATION
            self.clobber(result)
        elif target==0x2AE158:self.event(1,[a,0,0,0,0]);self.clobber(0)
        elif target==0x394F68:
            if a!=0x447238:raise ValueError('file allocation diagnostic pointer')
            args=[self.r[r]&MASK for r in (5,6,7)]
            self.event(1,[args[0],args[1],args[2],1,0]);self.clobber(0)
        elif target==0x3689B0:
            if b not in (1,0x602):raise ValueError('file open flags contract')
            self.event(3,[text_hash(self.text(a)),b,0,0,0])
            if self.p['mutation']==2:self.save(slot+4,0x11223344,4)
            self.clobber(self.p['sdk_result'])
        elif target==0x368C40:self.event(4,[a,0,0,0,0]);self.clobber(self.p['sdk_result'])
        elif target==0x368DB8:
            self.event(5,[a,b,c,0,0]);n=self.io_counts[5]
            if self.p['mutation']==3 and n==1:self.save(slot+4,0,4)
            result=self.p['position'] if n==1 else self.p['end'] if n==2 else self.p['sdk_result']
            self.clobber(result)
        elif target in (0x368FF8,0x369268):
            kind=6 if target==0x368FF8 else 7
            self.event(kind,[a,b,c,0,0]);n=self.io_counts[kind]
            if self.p['routine']==0:
                result=self.p['read_results'][n-1] if n<=len(self.p['read_results']) else 0
            else:result=self.p['sdk_result']
            if kind==6 and signed(result)>0:
                for i in range(min(signed(result),16)):self.save((b+i)&MASK,(0x51+i+n)&255,1)
            self.clobber(result)
        else:raise ValueError('unreviewed file external %#x'%target)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>10:raise ValueError('unreviewed file entry/depth')
        a,b=RANGES[index];pc=a;self.invocations[index]+=1
        if entry==0x3934F8:
            dst,src,n=(self.r[r]&MASK for r in (4,5,6))
            if n>64 or n and not(dst+n<=src or src+n<=dst):raise ValueError('file memcpy observer domain')
        while True:
            if not a<=pc<b:raise ValueError('file control outside owned body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                if not annul:
                    if pc+4>=b:raise ValueError('file delay outside whole body')
                    delay=self.fetch(pc+4)
                    if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('file encoded control in delay')
                if w==0x03E00008:
                    if target!=stop or index<len(SELECTED) and pc!=b-8:raise ValueError('file actual terminal JR31/stop required')
                    return
                if w>>26==3:
                    continuation=self.r[31]&MASK
                    if target in tuple(x for x,y in RANGES):self.run(target,continuation,depth+1)
                    else:self.external(target)
                    if self.r[31]&MASK!=continuation:raise ValueError('file external changed RA')
                    pc=continuation
                elif w>>26==2 and target in tuple(x for x,y in SUPPORT):
                    self.run(target,stop,depth+1);return
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('file noncontrol transfer')
                pc+=4

KEYS=('routine','path','descriptor','count','sdk_result','position','end','slot','mode','open_count','failures','mutation','archive_alias','archive_hit','registry_table','registry_fail','read_results','buffer_alias')
def fixture(original,routine=0,path='plain',descriptor=3,count=32,sdk_result=5,position=7,end=99,slot=0,mode=0,
            open_count=0,failures=0,mutation=0,archive_alias=0,archive_hit=1,registry_table=1,registry_fail=0,read_results=(16,16),buffer_alias=0):
    p=dict(zip(KEYS,(routine,path,descriptor,count,sdk_result,position,end,slot,mode,open_count,failures,mutation,archive_alias,archive_hit,registry_table,registry_fail,list(read_results),buffer_alias)))
    if type(routine) is not int or routine not in range(10) or type(slot) is not int or not 0<=slot<20 or mutation not in range(9) or failures not in range(5):raise ValueError('file fixture selector')
    if archive_alias not in (0,1) or archive_hit not in (0,1) or registry_table not in (0,1) or registry_fail not in (0,1) or buffer_alias not in (0,1):raise ValueError('file fixture boolean')
    if not isinstance(path,str) or len(path)>64 or any(ord(c)==0 or ord(c)>127 for c in path) or path.startswith(':') and ':' not in path[1:]:raise ValueError('file string observer domain')
    if routine==6 and mode and not path:raise ValueError('archive empty prior-stack domain')
    if any(type(v) is not int or not 0<=v<=MASK for v in (descriptor,sdk_result,position,end,open_count,mode)) or not -0x80000000<=count<=0x7FFFFFFF:raise ValueError('file lowword fixture domain')
    if any(type(v) is not int or not -0x80000000<=v<=0x7FFFFFFF for v in read_results):raise ValueError('file read queue domain')
    t=FileTrace(original,p);pointers=set()
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xA5030201,4)
    for g in TEXT_GLOBALS:
        for i in range(128):t.save(g+i,0xA5,1)
        t.put(g,b'')
    def ptr(a,v):pointers.add((a-BASE)//4);t.save(a,v,4)
    for i in range(20):
        for j in range(7):t.save(SLOTS+i*28+j*4,0x13570000+i*32+j,4)
        t.save(SLOTS+i*28,int(i<slot),4)
    t.put(PATH,path.encode());t.put(KEY,b'tag:');t.put(VALUE,b'folder/')
    ptr(TABLE,KEY);ptr(TABLE+4,VALUE)
    for h in (HEAP,ALT_HEAP):ptr(h+0x18,0);t.save(h+0x24,MASK,4);t.save(h+0x28,0,4)
    for ctx in (CONTEXT,ALT_CONTEXT):ptr(ctx+0x50,MAP)
    t.save(MAP,0,4);t.save(MAP+4,8,4);t.save(MAP+8,1,4);ptr(MAP+12,BUCKETS)
    for i in range(8):ptr(BUCKETS+4*i,0)
    resolved=b'folder\\file\\one' if path=='TAG:File/One' else path.lower().replace('/','\\').encode()
    at=resolved.find(b'cdrom0:');hashed=resolved[at+7:] if at>=0 else resolved
    crc=zlib.crc32(hashed)&MASK
    ptr(BUCKETS+4*(crc%8),NODE);ptr(NODE,0);t.save(NODE+4,crc if archive_hit else crc^1,4)
    ptr(NODE+8,SLOTS+28*slot+12 if archive_alias else RECORD)
    t.save(RECORD,0x12345678,4);t.save(RECORD+4,0x87654321,4)
    for g,v in zip(GLOBALS,(open_count,mode,CONTEXT,1 if registry_table else 0,10,TABLE if registry_table else 0,HEAP)):t.save(g,v,4)
    initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    initial_slots=[t.load(SLOTS+4*i,4) for i in range(140)]
    initial_globals=[t.load(g,4) for g in GLOBALS]
    initial_texts=[[t.load(g+i,1) for i in range(128)] for g in TEXT_GLOBALS]
    target_buffer=SLOTS+28*slot+4 if buffer_alias else DATA
    args=[descriptor,0x11000000,count&MASK] if routine==0 else [PATH] if routine in (1,6,7,9) else [descriptor] if routine in (2,5) else [descriptor,target_buffer,count&MASK] if routine in (3,8) else [descriptor,position,2]
    for r,v in enumerate(args,4):t.r[r]=sx32(v)
    t.r[29]=0x80000;t.r[31]=RETURN
    saved=[(0xCAFE0000000000000000000000000000+i) for i in range(16,24)]+[0x123456789ABCDEF0123456789ABCDEF0]
    t.r[16:24]=saved[:8];t.r[30]=saved[8]
    t.run(SELECTED[routine][0])
    assert t.r[29]==0x80000 and t.r[31]&MASK==RETURN and t.r[16:24]==saved[:8] and t.r[30]==saved[8]
    after=[t.load(BASE+4*i,4) for i in range(WORDS)]
    return dict(parameters=p,initial=[[i,v] for i,v in enumerate(initial) if v!=initial_word(i)],pointers=sorted(pointers),
        initial_slots=initial_slots,initial_globals=initial_globals,initial_texts=initial_texts,
        changes=[[i,v] for i,v in enumerate(after) if v!=initial[i]],expected_slots=[t.load(SLOTS+4*i,4) for i in range(140)],
        expected_globals=[t.load(g,4) for g in GLOBALS],expected_texts=[[t.load(g+i,1) for i in range(128)] for g in TEXT_GLOBALS],
        result=t.r[2]&MASK if routine!=2 else 0,events=t.events,invocations=t.invocations,
        instructions=t.instruction_count,visited=sorted(t.visited),readonly_reads=sorted(t.readonly_reads))

def fixtures(original):
    cases=[]
    for r in (1,7,9):
        for path in ('','plain','TAG:File/One','cdrom0:/FILE'):
            for result in (0,1,0x7FFFFFFF,0x80000000,MASK):cases.append(fixture(original,r,path=path,sdk_result=result))
    for r in (2,3,4,5,8):
        for descriptor in (0,1,0x80000001,MASK):
            for n in (0,1,-1,17):cases.append(fixture(original,r,descriptor=descriptor,count=n,sdk_result=MASK if n<0 else 5))
    for slot in (0,1,5,19):
        for mode in (0,1):
            for path in ('plain','TAG:File/One','cdrom0:/FILE'):
                for hit in (0,1):cases.append(fixture(original,6,path=path,slot=slot,mode=mode,archive_hit=hit))
    for n in (-1,0,1,15,16,17,32,33,64):
        for failure in (0,1,2,4):
            for reads in ((16,16),(0,),(-1,),(1,),(17,),(-16,0)):
                cases.append(fixture(original,count=n,failures=failure,read_results=reads))
    for mutation in range(9):
        for r in range(10):cases.append(fixture(original,r,mutation=mutation,slot=1,open_count=MASK,mode=0,count=32))
    for path in ('plain','TAG:File/One','cdrom0:/FILE'):
        for alias in (0,1):
            for count in (0,MASK,0x80000000):cases.append(fixture(original,6,path=path,mode=1,archive_alias=alias,open_count=count))
    for r in (1,6,7,9):
        for fail in (0,1):cases.append(fixture(original,r,registry_table=0,registry_fail=fail))
    for descriptor in (1,MASK):cases.append(fixture(original,3,descriptor=descriptor,count=4,sdk_result=4,buffer_alias=1))
    return cases

def golden_header(cases):
    lines=['/* Synthetic file/SDK controls and original observations; no original instruction/data arrays. */',
      'struct FilePair { unsigned int index,value; };',
      'struct FileGolden { unsigned int routine,descriptor,count,sdk_result,position,end,slot,mode,open_count,failures,mutation,archive_alias,archive_hit,registry_table,registry_fail,buffer_alias; const char *path; unsigned int read_count,reads[8],result; const struct FilePair *initial,*changes,*slot_changes,*text_changes; unsigned int initial_count,change_count,slot_change_count,text_change_count; const unsigned int *pointers; unsigned int pointer_count; unsigned int expected_globals[7]; const unsigned int *events; unsigned int event_count; };']
    summaries=[]
    for i,c in enumerate(cases):
        p=c['parameters']
        pairs=dict(initial=c['initial'],changes=c['changes'],
          slot_changes=[[k,v] for k,v in enumerate(c['expected_slots']) if v!=c['initial_slots'][k]],
          text_changes=[[g*128+k,v] for g in range(3) for k,v in enumerate(c['expected_texts'][g]) if v!=c['initial_texts'][g][k]])
        for kind,records in pairs.items():
            lines.append('static const struct FilePair file_%s_%d[]={%s};'%(kind,i,','.join('{%du,0x%08Xu}'%(k,v) for k,v in records) or '{0,0}'))
        lines.append('static const unsigned int file_pointers_%d[]={%s};'%(i,','.join('%du'%v for v in c['pointers']) or '0'))
        flat=[v for event in c['events'] for v in event]
        lines.append('static const unsigned int file_events_%d[]={%s};'%(i,','.join('0x%08Xu'%v for v in flat) or '0'))
        fields=[p[k]&MASK for k in ('routine','descriptor','count','sdk_result','position','end','slot','mode','open_count','failures','mutation','archive_alias','archive_hit','registry_table','registry_fail','buffer_alias')]
        reads=p['read_results']+[0]*(8-len(p['read_results']))
        summaries.append(' {%s,%s,%du,{%s},0x%08Xu,%s,%s,file_pointers_%d,%du,{%s},file_events_%d,%du},'%(
          ','.join('0x%08Xu'%v for v in fields),json.dumps(p['path']),len(p['read_results']),','.join('0x%08Xu'%(v&MASK) for v in reads),c['result'],
          ','.join('file_%s_%d'%(k,i) for k in pairs),','.join('%du'%len(v) for v in pairs.values()),i,len(c['pointers']),
          ','.join('0x%08Xu'%v for v in c['expected_globals']),i,len(c['events'])))
    lines+=['static const struct FileGolden file_golden[]={',*summaries,'};','']
    return '\n'.join(lines)

def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,default=ROOT/'build/file_operations/trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(original)
    report=dict(fixtures=cases,fixture_count=len(cases),original_instructions=sum(c['instructions'] for c in cases),max_instructions=max(c['instructions'] for c in cases),
        visited=sorted(set().union(*(set(c['visited']) for c in cases))),limit='Initialized finite low-word controls with real integer/MMI helper instructions. No SDK/kernel/hardware timing, full concurrency, overflow-safe paths or upper-register ABI claim.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n',newline='\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(cases),newline='\n')
    print('File original fixtures',len(cases),'instructions',report['original_instructions'],'maximum',report['max_instructions'])
if __name__=='__main__':main()
