"""Bounded initialized resource pointer effects at four actual original entries.

Whole RegistryTrace scalar execution is unchanged. Local checks precede every
mutation; the separate integer oracle models owned bytes, not engine classes.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer
from trace_string_registry import RegistryTrace

ROOT=Path(__file__).resolve().parents[1]
MASK,U64=0xFFFFFFFF,(1<<64)-1
BASE,WORDS,LARGE_WORDS=0x20000,768,18432
RANGES=((0x2B6228,0x2B6258),(0x2B62E0,0x2B6308),
        (0x2B6308,0x2B6398),(0x2B64E0,0x2B6574))
KEYS=('routine','flavor','owner_word','flags','byte2','byte3','index','count',
      'value','pattern','payload_delta','extra','mode','salt')
MODES=(0,1,2,0x100000000,0x8000000000000000,U64)

def initial_word(i,salt):return (0x692F1301^(i*0x1020305)^salt)&MASK
def sign_word(word):return word if word<0x80000000 else word+(U64-MASK)

class ResourcePointerTrace(Trace):
    def __init__(self,original,routine=0,words=WORDS):
        super().__init__(original)
        if type(routine) is not int or not 0<=routine<4 or words not in (WORDS,LARGE_WORDS):
            raise ValueError('unreviewed resource entry/storage')
        self.routine=routine;self.words=words;self.lo=self.hi=0
        self.visited=set();self.events=[];self.branches=[]
        self.defined={0,4,31}|({5,6} if routine==3 else set())
        a,b=RANGES[routine];self.expected_words={}
        for pc in range(a,b,4):
            off=pc-0xFF000
            if 0<=off and off+4<=len(original):
                self.expected_words[pc]=struct.unpack_from('<I',original,off)[0]
    def memory_check(self,address,width):
        if width not in (1,2,4) or address%width or not BASE<=address<address+width<=BASE+self.words*4:
            raise ValueError('unowned or unaligned resource memory')
    def initialized_check(self,address,width):
        self.memory_check(address,width)
        if not all(address+i in self.memory for i in range(width)):
            raise ValueError('uninitialized resource memory')
    def load(self,address,width):
        self.initialized_check(address,width);value=Trace.load(self,address,width)
        self.events.append(['load',address,width,value]);return value
    def save(self,address,value,width):
        self.initialized_check(address,width)
        self.events.append(['store',address,width,value&((1<<(8*width))-1)])
        Trace.save(self,address,value,width)
    def fetch(self,pc):
        if pc not in self.expected_words:raise ValueError('unreviewed resource PC')
        word=struct.unpack_from('<I',self.original,pc-0xFF000)[0]
        if word!=self.expected_words[pc]:raise ValueError('changed resource instruction')
        return word
    def execute(self,word,pc):
        if pc not in self.expected_words or word!=self.expected_words[pc]:
            raise ValueError('unreviewed resource PC/word')
        if self.instruction_count>=1200:raise ValueError('resource instruction budget')
        op,rs,rt,rd,sh,fn,im=word>>26,word>>21&31,word>>16&31,word>>11&31,word>>6&31,word&63,word&65535
        required=[];dest=None;conditional=False
        if word==0:pass
        elif op==0:
            if fn in (0x21,0x23,0x2D,0x24,0x25,0x27,0x2B) and sh==0:
                required=[rs,rt];dest=rd
            elif fn==0 and rs==0:
                required=[rt];dest=rd
            elif fn==4 and sh==0:
                required=[rs,rt];dest=rd
            elif fn==0x0B and sh==0:
                required=[rs,rt];dest=rd;conditional=True
            elif fn==0x18 and sh==0:
                required=[rs,rt];dest=rd if rd else None
            elif fn==8 and (rs,rt,rd,sh)==(31,0,0,0):
                required=[31]
                if self.r[31]&MASK!=RETURN:raise ValueError('unexpected resource JR31')
            else:raise ValueError('reserved resource SPECIAL/family operands')
        elif op in (9,12):required=[rs];dest=rt
        elif op==15 and rs==0:dest=rt
        elif op in (35,36,37):required=[rs];dest=rt
        elif op in (40,43):required=[rs,rt]
        elif op in (4,5,21):required=[rs,rt]
        else:raise ValueError('reserved resource family/operands')
        if any(r not in self.defined for r in required):raise ValueError('undefined resource register')
        if any(type(self.r[r]) is not int or not 0<=self.r[r]<=U64 for r in required):
            raise ValueError('undefined resource lower64 lane')
        si=im-65536 if im&32768 else im
        if op in (35,36,37,40,43):
            width={35:4,36:1,37:2,40:1,43:4}[op]
            self.initialized_check(((self.r[rs]&MASK)+si)&MASK,width)
        if op in (4,5,21):
            target=(pc+4+4*si)&MASK
            if target not in self.expected_words:raise ValueError('unreviewed resource branch target')
        if is_control_transfer(word):
            if pc+4 not in self.expected_words or is_control_transfer(self.fetch(pc+4)):
                raise ValueError('control transfer in resource delay')
        define=dest is not None and (not conditional or self.r[rt]!=0)
        result=RegistryTrace.execute(self,word,pc)
        if define:self.defined.add(dest)
        if op in (4,5,21):self.branches.append([pc,result[0] is not None,result[1]])
        return result
    def run(self,entry=None):
        a,b=RANGES[self.routine]
        if entry is None:entry=a
        if entry!=a or self.r[31]&MASK!=RETURN:raise ValueError('unreviewed resource entry/return')
        for pc,word in self.expected_words.items():
            if is_control_transfer(word) and (pc+4 not in self.expected_words or is_control_transfer(self.fetch(pc+4))):
                raise ValueError('control transfer in resource delay')
        return Trace.run(self,entry)

class IntegerArena:
    """Independent owned-byte algorithm oracle; never delegates instruction work."""
    def __init__(self,cells,base=BASE):
        self.base=base;self.data=bytearray().join(struct.pack('<I',v) for v in cells);self.events=[]
    def offset(self,address,width):
        off=address-self.base
        if width not in (1,2,4) or address%width or not 0<=off<=len(self.data)-width:
            raise ValueError('oracle storage domain')
        return off
    def read(self,address,width):
        off=self.offset(address,width);value=int.from_bytes(self.data[off:off+width],'little')
        self.events.append(['load',address,width,value]);return value
    def write(self,address,value,width):
        off=self.offset(address,width);value&=(1<<(8*width))-1
        self.events.append(['store',address,width,value]);self.data[off:off+width]=value.to_bytes(width,'little')
    def cells(self):return list(struct.unpack('<%dI'%(len(self.data)//4),self.data))

def integer_effects(cells,p,base=BASE):
    a=IntegerArena(cells,base);owner=base+p['owner_word']*4;r=p['routine'];result=None
    if r==0:
        payload=a.read(owner+8,4);mask=a.read(payload,4)&0x30000
        result=int(mask in (0x20000,0x30000))
    elif r==1:
        byte=a.read(owner+2,1);table=(owner+16)&MASK
        a.write(owner+4,table,4);slot=(table+4*byte)&MASK
        a.write(owner+12,slot,4);relative=a.read(slot,4);a.write(slot,owner+relative,4)
    elif r==2:
        flags=a.read(owner,2);groups=1
        if flags&1:groups=1<<(a.read(owner+2,1)&31)
        payload=a.read(owner+8,4);entry=0
        for group in range(groups):
            count=a.read(payload+28,2);index=0
            if count:
                while True:
                    slot=a.read(owner+12,4);table=a.read(slot,4);at=(table+4*entry)&MASK
                    relative=a.read(at,4)
                    if relative:a.write(at,owner+relative,4)
                    payload=a.read(owner+8,4);index+=1;count=a.read(payload+28,2);entry+=1
                    if index>=count:break
            if group+1<groups:payload=a.read(owner+8,4)
    else:
        old=a.read(owner+3,1);bit=1<<(p['index']&31)
        selected=(old|bit)&255 if p['mode'] else old&~bit
        current=a.read(owner+3,1)
        if selected==current:result=current
        else:
            payload=a.read(owner+8,4);old=a.read(owner+3,1);count=a.read(payload+28,2)
            slot=a.read(owner+12,4);word=a.read(slot,4)
            a.write(slot,word-4*count*old,4);a.write(owner+3,selected,1)
            payload=a.read(owner+8,4);slot=a.read(owner+12,4);count=a.read(payload+28,2);word=a.read(slot,4)
            word=(word+4*count*selected)&MASK;a.write(slot,word,4);result=sign_word(word)
    return a.cells(),result,a.events

def parameters():
    cases=[]
    def add(**kw):
        p=dict(zip(KEYS,(0,0,32,0,0,0,0,0,0,0,0x400,0,0,0)));p.update(kw)
        p['salt']=(0x17AB9301*(len(cases)+1))&MASK;cases.append(p)
    for mask in (0,0x10000,0x20000,0x30000):
        for noise in (0,0x80000000,0xFFFF,0xFFFC0000):
            for owner in (32,64):
                for delta in (0x400,4):add(value=mask|noise,owner_word=owner,payload_delta=delta)
    for byte in (0,1,2,3,4,7,31,255):
        for value in (0,0x40,0x7FFFFFFC,0xFFFFFFFC):
            for owner in (32,64):add(routine=1,byte2=byte,value=value,owner_word=owner)
    groups=((0,31),(1,0),(1,1),(1,35))
    for flags,byte in groups:
        for count in (0,1,2,4):
            for pattern in range(4):add(routine=2,flags=flags,byte2=byte,count=count,pattern=pattern)
    for flags,byte in groups:
        for delta in (0x400,0x600):
            for high in (0,0x10000,0x40000000,0x7FFF0000):
                add(routine=2,flavor=1,flags=flags,byte2=byte,count=4,value=high|4,payload_delta=delta,owner_word=16383)
    ordinary=((0,0,0,0),(1,3,1,0x7FFFFFFF),(7,128,65535,0x80000000),
              (8,3,2,MASK),(31,3,4,0),(32,0,1,MASK),(255,255,65535,0x7FFFFFFF),(2,5,17,0x80000000))
    for mode in MODES:
        for index,byte,count,value in ordinary:
            add(routine=3,mode=mode,index=index,byte3=byte,count=count,value=value)
    for flavor in (1,2):
        for mode in MODES:
            for index in (1,0,7,32):add(routine=3,flavor=flavor,mode=mode,index=index,byte3=1,count=1,value=1)
    assert len(cases)==320 and len({tuple(p[k] for k in KEYS if k!='salt') for p in cases})==320
    return cases

def initialized_cells(p,base=BASE):
    words=LARGE_WORDS if p['routine']==2 and p['flavor']==1 else WORDS
    cells=[initial_word(i,p['salt']) for i in range(words)]
    owner=base+p['owner_word']*4;payload=owner+p['payload_delta'];slot=owner+0x800;table=owner+0x900
    def put(address,value,width=4):
        off=address-base
        if address%width or not 0<=off<=words*4-width:raise ValueError('fixture initialized bounds')
        i,shift=off//4,(off%4)*8;mask=((1<<(width*8))-1)<<shift
        cells[i]=((cells[i]&~mask)|((value<<shift)&mask))&MASK
    put(owner,p['flags'],2);put(owner+2,p['byte2'],1);put(owner+3,p['byte3'],1)
    put(owner+4,owner+16);put(owner+8,payload);put(owner+12,slot)
    put(payload,0);put(payload+28,p['count'],2);put(slot,table)
    if p['routine']==0:put(payload,p['value'])
    elif p['routine']==1:put(owner+16+4*p['byte2'],p['value'])
    elif p['routine']==2:
        if p['flavor']==1:
            table=payload+28;put(slot,table)
            for i,value in enumerate((p['value'],0,0x40,0x80000000)):put(table+4*i,value)
        else:
            groups=1 if not p['flags']&1 else 1<<(p['byte2']&31)
            for i in range(groups*p['count']):
                values=(0,0 if i%2==0 else 0x40+4*i,0x20+4*i,0x80000000+4*i)
                put(table+4*i,values[p['pattern']])
    elif p['flavor']==1:put(owner+12,payload+28);put(payload+28,1)
    elif p['flavor']==2:put(owner+12,owner+12)
    else:put(slot,p['value'])
    return cells

def fixture(original,p):
    if set(p)!=set(KEYS) or any(type(p[k]) is not int for k in KEYS):raise ValueError('resource fixture schema')
    if p not in parameters():raise ValueError('unreviewed resource fixture domain')
    cells=initialized_cells(p);expected,result,events=integer_effects(cells,p)
    t=ResourcePointerTrace(original,p['routine'],len(cells))
    for i,value in enumerate(cells):Trace.save(t,BASE+4*i,value,4)
    t.r[4]=BASE+4*p['owner_word'];t.r[5]=p['index'];t.r[6]=p['mode'];t.r[31]=RETURN
    t.run();actual=[Trace.load(t,BASE+4*i,4) for i in range(len(cells))]
    assert actual==expected and t.events==events
    if result is not None:assert t.r[2]&U64==result
    return dict(parameters=p,words=len(cells),initial=cells,expected=actual,
                observed_lower64=t.r[2]&U64,source_result=result,events=events,
                changes=[[i,v] for i,v in enumerate(actual) if v!=cells[i]],
                instructions=t.instruction_count,visited=sorted(t.visited),branches=t.branches)

def golden_header(cases):
    fields='u32 '+','.join(k for k in KEYS if k!='mode')+'; GeorgeResourcePointerBits mode;'
    lines=['/* Synthetic parameters/full changed-word ledger, no original code arrays. */',
           '#define RESOURCE_POINTER_CHANGES 32',
           'struct ResourcePointerParams { '+fields+' };',
           'struct ResourcePointerPair { u32 cell,value; };',
           'struct ResourcePointerGolden { struct ResourcePointerParams p; u32 words,changes; GeorgeResourcePointerBits original_result; struct ResourcePointerPair change[RESOURCE_POINTER_CHANGES]; };',
           'static const struct ResourcePointerGolden resource_pointer_golden[] = {']
    for c in cases:
        assert len(c['changes'])<=32
        p=c['parameters'];values=','.join('0x%08Xu'%p[k] for k in KEYS if k!='mode')+',0x%016XULL'%p['mode']
        pairs=','.join('{%du,0x%08Xu}'%(i,v) for i,v in c['changes']) or '{0,0}'
        result=c['source_result'] if c['source_result'] is not None else 0
        lines.append(' {{%s},%du,%du,0x%016XULL,{%s}},'%(values,c['words'],len(c['changes']),result,pairs))
    return '\n'.join(lines+['};',''])

def main():
    a=argparse.ArgumentParser(description=__doc__);a.add_argument('--output',type=Path,default=ROOT/'build/resource_pointer/trace.json');a.add_argument('--golden-header',type=Path)
    args=a.parse_args();_,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=[fixture(raw,p) for p in parameters()]
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),max_instructions=max(c['instructions'] for c in cases),
        coverage=[len({pc for c in cases for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],
        input_sha256=hashlib.sha256(json.dumps([c['parameters'] for c in cases],sort_keys=True).encode()).hexdigest(),cases=cases,
        limitation='Initialized GNU32 raw scalar storage/lower64 effects; original class/prototypes/capacities/large shifts/concurrency/hardware/upper128 unproved. Native actual addresses use a separate integer oracle; no whole raw synthetic/native equality claim.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_text(golden_header(cases),newline='\n')
    print('resource pointer: %d fixtures, %d instructions, coverage%s'%(report['fixtures'],report['instructions'],report['coverage']))
if __name__=='__main__':main()
