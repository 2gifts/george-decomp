"""Initialized file-slot/archive observations using the published registry decoder.

Whole selected bodies and actual resolver/string/CRC/map/close helpers execute.
Heap allocation and SDK cache/close are controlled call contracts, not an OS or
EE emulator. String lanes (including optimized overreads) are initialized.
"""
import argparse,hashlib,json,struct,zlib
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,MASK,U64,U128,sx32
ROOT=Path(__file__).resolve().parents[1]
BASE,WORDS=0x20000,2048;END=BASE+WORDS*4
SLOT,INPUT,CONTEXT,HEAP,TABLE,KEY,PATH,NEW_TABLE=0x20000,0x20101,0x20200,0x20300,0x20400,0x20501,0x20601,0x20700
MAP,ALT_MAP,BUCKETS,ALT_BUCKETS,NODE,ALT_NODE,RESULT,ALT_RESULT=0x20800,0x20820,0x20900,0x20940,0x20A00,0x20A40,0x20B00,0x20B40
COUNT,MODE,REG_COUNT,REG_CAP,REG_TABLE,HEAP_GLOBAL=0x3FD23C,0x3FD240,0x3FD1D8,0x3FD1DC,0x3FD1E0,0x3FD204
GLOBALS=(COUNT,MODE,REG_COUNT,REG_CAP,REG_TABLE,HEAP_GLOBAL)
TEXT_GLOBALS=(0x469A00,0x469A80,0x469B00);STACK=(0x7D000,0x81000)
SELECTED=((0x2B0998,0x2B09F4),(0x2B1BA8,0x2B1C04))
SUPPORT=((0x2B1198,0x2B11D4),(0x2ABAE8,0x2ABDE8),(0x2AEC28,0x2AECD0),
 (0x295050,0x295080),(0x29C648,0x29C6C0),(0x2A7C08,0x2A7C58),
 (0x393758,0x393888),(0x393A28,0x393B74),(0x393B74,0x393C8C),
 (0x3984D8,0x398554),(0x398628,0x39869C),(0x39CA58,0x39CA78))
RANGES=SELECTED+SUPPORT
READONLY=((0x445650,0x445A50),(0x446FE0,0x447010),(0x4473C0,0x4473C8),(0x456118,0x456219))
initial_word=lambda i:(0xAD4B0301^(i*0x10203))&MASK

class ArchiveTrace(RegistryTrace):
    def __init__(self,original,parameters):
        super().__init__(original,parameters)
        self.invocations=[0]*len(RANGES);self.stack_base=None
    def memory_check(self,a,n):
        if n not in (1,2,4,8,16) or a%n or not (
          BASE<=a<a+n<=END or STACK[0]<=a<a+n<=STACK[1] or
          any(g<=a<a+n<=g+4 for g in GLOBALS) or
          any(g<=a<a+n<=g+128 for g in TEXT_GLOBALS)):
            raise ValueError('unowned or unaligned archive memory %#x/%d'%(a,n))
    def load(self,a,n):
        if any(lo<=a<a+n<=hi for lo,hi in READONLY):
            if a%n:raise ValueError('unaligned archive readonly')
            self.readonly_reads.add((a,n));off=a-0xFF000
            if off<0 or off+n>len(self.original):raise ValueError('archive readonly image bound')
            return int.from_bytes(self.original[off:off+n],'little')
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized archive read')
        return Trace.load(self,a,n)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed archive instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('archive code image bound')
        return struct.unpack_from('<I',self.original,off)[0]
    def event(self,kind,a,b=0,c=0):
        if len(self.events)>=16:raise ValueError('archive event bound')
        self.events.append([kind,a&MASK,b&MASK,c&MASK,self.load(COUNT,4),self.load(MODE,4),self.load(SLOT+4,4),self.load(SLOT+16,4)])
    def external(self,target):
        if target==0x363AE0:
            if self.r[4]&MASK:raise ValueError('archive cache call argument')
            self.event(1,0)
            if self.p['mutation']&1:
                self.save(COUNT,0x12345678,4);self.save(MODE,0x80000000,4)
                self.save(SLOT+16,0xBEEFF00D,4)
        elif target==0x368C40:
            self.event(2,self.r[4])
            if self.p['mutation']&2:
                self.save(COUNT,0xFFFFFFFF,4);self.save(SLOT+4,0xAAAAAAAA,4)
                self.save(SLOT+16,0x87654321,4)
        elif target==0x2ADF60:
            if tuple(self.r[i]&MASK for i in (4,5,6))!=(HEAP,80,3):raise ValueError('archive allocator contract')
            self.event(3,HEAP,80,3);self.allocations+=1
            if self.p['mutation']&4:self.save(CONTEXT+0x50,ALT_MAP,4)
            result=0 if self.p['failure'] else NEW_TABLE
            for r in range(3,16):self.r[r]=sx32(0xCAFEBABE)
            self.r[2]=result;return
        elif target==0x394F68:
            if self.r[4]&MASK!=0x447238:raise ValueError('archive allocation diagnostic')
            self.event(4,self.r[5],self.r[6],self.r[7])
        else:raise ValueError('unreviewed archive external %#x'%target)
        for r in range(3,16):self.r[r]=sx32(0xCAFEBABE)
        self.r[2]=sx32(0x80000001)
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>12:raise ValueError('unreviewed archive entry/depth')
        lo,hi=RANGES[index];self.invocations[index]+=1;pc=lo
        while True:
            if not lo<=pc<hi:raise ValueError('archive transfer outside full body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('archive annul taken')
                    pc+=8;continue
                if pc+4>=hi:raise ValueError('archive delay outside body')
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('archive control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('archive actual JR31/stop')
                    if index<2 and pc!=hi-8:raise ValueError('archive terminal JR31')
                    return
                if w>>26==3:
                    continuation=self.r[31]&MASK
                    if target in [a for a,b in SUPPORT]:self.run(target,continuation,depth+1)
                    else:self.external(target)
                    pc=continuation
                elif w>>26==2 and target in [a for a,b in SUPPORT]:self.run(target,stop,depth+1);return
                else:pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('archive unexpected transfer')
                pc+=4

KEYS=('routine','mode','encoded','count','mutation','input','base','prefix','suffix','table','failure','match','collision')
def fixture(original,routine=0,mode=0,encoded=2,count=1,mutation=0,input='TAG:File/One',base='',prefix='',suffix='',table=1,failure=0,match=1,collision=0):
    p=dict(zip(KEYS,(routine,mode,encoded,count,mutation,input,base,prefix,suffix,table,failure,match,collision)))
    if routine not in (0,1) or table not in (0,1) or failure not in (0,1) or match not in (0,1) or collision not in (0,1) or mutation not in range(8):raise ValueError('archive fixture selector domain')
    if any(type(x) is not int or not 0<=x<=MASK for x in (mode,encoded,count)):raise ValueError('archive fixture word')
    for text in (input,base,prefix,suffix):
        if not isinstance(text,str) or len(text)>30 or any(ord(c)==0 or ord(c)>127 for c in text):raise ValueError('archive initialized string domain')
    if not input or input.startswith(':') and ':' not in input[1:]:raise ValueError('archive prior-stack/colon domain')
    t=ArchiveTrace(original,p)
    for i in range(WORDS):t.save(BASE+4*i,initial_word(i),4)
    for a in range(STACK[0],STACK[1],4):t.save(a,0xA5030201,4)
    for g,s in zip(TEXT_GLOBALS,(base,prefix,suffix)):
        for i in range(128):t.save(g+i,0xA5,1)
        t.put(g,s.encode())
    for a in (INPUT-1,KEY-1,PATH-1):
        for i in range(128):t.save(a+i,0xA5,1)
    t.put(INPUT,input.encode());t.put(KEY,b'tag:');t.put(PATH,b'cdrom0:Folder/')
    for i in range(7):t.save(SLOT+4*i,(0x13570000+i*0x10101)&MASK,4)
    t.save(SLOT+4,encoded,4);t.save(COUNT,count,4);t.save(MODE,mode,4)
    t.save(REG_COUNT,1,4);t.save(REG_CAP,10,4);t.save(REG_TABLE,TABLE if table else 0,4);t.save(HEAP_GLOBAL,HEAP,4)
    t.save(TABLE,KEY,4);t.save(TABLE+4,PATH,4)
    t.save(HEAP+0x18,0,4);t.save(HEAP+0x24,MASK,4);t.save(HEAP+0x28,0,4)
    t.save(CONTEXT+0x50,MAP,4)
    # Resolve key using the same documented valid short string domains. Actual
    # original helpers still compute the observed key; this only seeds buckets.
    s=input.lower()
    if not table:resolved=s if s.startswith('cdrom0:') or len(s)>1 and s[1]==':' else s
    elif s.startswith('tag:'):resolved=base+prefix+suffix+'cdrom0:Folder/'+s[4:]
    elif s.startswith('cdrom0:'):resolved=s
    elif len(s)>1 and s[1]==':':resolved=base+s
    else:resolved=base+prefix+suffix+s
    if not table:resolved=s if s.startswith('cdrom0:') else s
    resolved=resolved.replace('/','\\')
    if len(resolved)>=64:raise ValueError('archive output before saved-RA domain')
    at=resolved.find('cdrom0:');key=zlib.crc32((resolved[at+7:] if at>=0 else resolved).encode())&MASK
    for m,b,n,v in ((MAP,BUCKETS,NODE,RESULT),(ALT_MAP,ALT_BUCKETS,ALT_NODE,ALT_RESULT)):
        t.save(m+4,8,4);t.save(m+12,b,4)
        for i in range(8):t.save(b+i*4,0,4)
        t.save(b+(key%8)*4,n,4);t.save(n,0,4);t.save(n+4,key if match else key^0x80000000,4);t.save(n+8,v,4)
        if collision:
            t.save(n,n+16,4);t.save(n+4,key^0x80000000,4);t.save(n+16,0,4);t.save(n+20,key if match else key^0x40000000,4);t.save(n+24,v,4)
    arena_initial=[t.load(BASE+4*i,4) for i in range(WORDS)]
    globals_initial=[t.load(g,4) for g in GLOBALS]
    texts_initial=[[t.load(g+i,1) for i in range(128)] for g in TEXT_GLOBALS]
    t.r[4]=SLOT if routine==0 else CONTEXT;t.r[5]=INPUT;t.r[29]=0x80000;t.r[31]=RETURN
    preserved=[0xCAFE0000000000000000000000000000+i for i in range(16,24)]+[0x123456789ABCDEF0123456789ABCDEF0]
    for r,v in zip(list(range(16,24))+[30],preserved):t.r[r]=v
    t.run(SELECTED[routine][0]);assert t.r[29]==0x80000
    assert [t.r[r] for r in list(range(16,24))+[30]]==preserved
    expected=[t.load(BASE+4*i,4) for i in range(WORDS)]
    return dict(p,result=t.r[2]&MASK if routine else 0,initial=[[i,v] for i,v in enumerate(arena_initial) if v!=initial_word(i)],
      changes=[[i,v] for i,v in enumerate(expected) if v!=arena_initial[i]],globals_initial=globals_initial,
      globals_expected=[t.load(g,4) for g in GLOBALS],texts_initial=texts_initial,
      texts_expected=[[t.load(g+i,1) for i in range(128)] for g in TEXT_GLOBALS],events=t.events,
      instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches,invocations=t.invocations,readonly_reads=sorted(t.readonly_reads))

def fixtures(raw):
    cases=[]
    for mode in (0,1,0x80000000,MASK):
        for encoded in (0,1,0x7FFFFFFF,0x80000000,MASK):
            for count in (0,1,0x80000000,MASK):
                for mutation in range(4):cases.append(fixture(raw,mode=mode,encoded=encoded,count=count,mutation=mutation))
    for inp in ('TAG:File/One','cdrom0:File','x/cdrom0:File','C:Dir/File','path/noalias','host0:File/Dir','a','cdrom0:','x/cdrom0:cdrom0:File',':TAG:Tail'):
        for table in (0,1):
            for match in (0,1):
                for collision in (0,1):
                    for mutation in (0,4):cases.append(fixture(raw,1,input=inp,table=table,match=match,collision=collision,mutation=mutation))
    for base,prefix,suffix in (('B/','P/','S/'),('','cdrom0:',''),('x/cdrom0:','',''),('','X/','Y/')):
        for inp in ('TAG:File','missing:file','File/One'):
            cases.append(fixture(raw,1,input=inp,base=base,prefix=prefix,suffix=suffix))
    for failure in (0,1):
        for mutation in (0,4):
            for match in (0,1):cases.append(fixture(raw,1,table=0,failure=failure,mutation=mutation,match=match))
    return cases

def golden(cases):
    mi=max(len(c['initial']) for c in cases);mc=max(len(c['changes']) for c in cases);me=max(len(c['events']) for c in cases)
    lines=['/* Authored initialized inputs/outputs only; no original code arrays. */',
      '#define FILE_ARCHIVE_WORDS %du'%WORDS,'#define FILE_ARCHIVE_INITIAL %du'%mi,'#define FILE_ARCHIVE_CHANGES %du'%mc,'#define FILE_ARCHIVE_EVENTS %du'%max(me,1),
      'struct ArchivePair{u32 index,value;};',
      'struct ArchiveGolden{u32 routine,mutation,failure,result,initial_count,change_count,event_count;struct ArchivePair initial[FILE_ARCHIVE_INITIAL],changes[FILE_ARCHIVE_CHANGES];u32 globals_initial[6],globals_expected[6];u8 texts_initial[3][128],texts_expected[3][128];u32 events[FILE_ARCHIVE_EVENTS][8];};',
      'static const struct ArchiveGolden archive_golden[]={']
    for c in cases:
        pair=lambda values:'{'+','.join('{%du,0x%08Xu}'%(i,v) for i,v in values)+'}'
        rows=lambda values:'{'+','.join('{'+','.join('0x%Xu'%v for v in row)+'}' for row in values)+'}'
        scalar=[c[k] for k in ('routine','mutation','failure','result')]+[len(c['initial']),len(c['changes']),len(c['events'])]
        lines.append('{'+','.join('%du'%v for v in scalar)+','+pair(c['initial'])+','+pair(c['changes'])+',{'+','.join('0x%08Xu'%v for v in c['globals_initial'])+'},{'+','.join('0x%08Xu'%v for v in c['globals_expected'])+'},'+rows(c['texts_initial'])+','+rows(c['texts_expected'])+','+rows(c['events'])+'},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/file_archive/trace.json');p.add_argument('--header',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases),input_sha256=hashlib.sha256(json.dumps([{k:c[k] for k in KEYS} for c in cases],sort_keys=True).encode()).hexdigest(),coverage=[len({pc for c in cases for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],cases=cases)
    if a.header:a.header.write_text(golden(cases),newline='\n')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n');print(report['fixtures'],report['instructions'],report['coverage'])
if __name__=='__main__':main()
