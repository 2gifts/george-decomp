"""Initialized low-word observations of three buffer-manager originals.

Five complete helper intervals execute original instructions. Optimized strncpy
is restricted to its alignment gate and byte/padding path; unsupported bulk/MMI
encodings fail. Ctype reads use only the hash-proven 257 bytes (nonnegative
characters or EOF). Callback effects are authored controls, not SDK bodies.
No original instruction/data arrays are exported in synthetic golden fixtures.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer

ROOT=Path(__file__).resolve().parents[1]
RANGES=((0x2A4B70,0x2A4DB8),(0x2A4DB8,0x2A4EF0),(0x2A56C0,0x2A56D0),
        (0x295050,0x295080),(0x2AAF50,0x2AAF88),(0x2AAF88,0x2AAF90),
        (0x398628,0x39869C),(0x394010,0x3941D8))
BUFFER,WORDS=0x20000,2048
END=BUFFER+WORDS*4;MASK=0xFFFFFFFF;STACK=(0x7F000,0x81000)
MANAGER=0x20040;LINES=0x20100;ALT_LINES=0x20140
RECORDS=tuple(0x20200+i*32 for i in range(4));ALT_RECORD=0x20300
HISTORY=0x20400;ALT_HISTORY=0x20440;COMMANDS=0x20500;ALT_COMMANDS=0x20520
COMMAND_RECORDS=0x20600;ALT_COMMAND_RECORDS=0x20700
TEXTS=tuple(0x20801+i*0x80 for i in range(4));ALT_TEXT=0x20A01
HISTORY_TEXTS=tuple(0x20C01+i*0x80 for i in range(4))
INPUT,NEW_TEXT=0x21001,0x21101
CALLBACKS=(0xF0000010,0xF0000020)
CTYPE,CTYPE_SIZE=0x456118,257
CTYPE_SHA='8b55a0d9c781d7001042c99e21523fef362440f572faef950e8e600fae5de813'

def signed(value):
    value&=MASK
    return value-0x100000000 if value&0x80000000 else value

def initial_word(index):return (0xA5000001^(index*0x103))&MASK

class ManagerTrace(Trace):
    def __init__(self,original,mutation=0):
        super().__init__(original);self.mutation=mutation;self.visited=set()
        self.events=[];self.invocations=[0]*len(RANGES);self.callback_count=0
    def memory_check(self,address,size):
        if size not in (1,4,8,16) or address%size or not (
                BUFFER<=address<address+size<=END or STACK[0]<=address<address+size<=STACK[1]):
            raise ValueError('unowned/unaligned manager memory')
    def load(self,address,size):
        if CTYPE<=address<CTYPE+CTYPE_SIZE:
            if size!=1:raise ValueError('manager ctype byte domain')
            data=self.original[CTYPE-0xFF000:CTYPE-0xFF000+CTYPE_SIZE]
            if hashlib.sha256(data).hexdigest()!=CTYPE_SHA:raise ValueError('manager ctype identity')
            return data[address-CTYPE]
        self.memory_check(address,size)
        if not all(address+i in self.memory for i in range(size)):
            raise ValueError('uninitialized manager read')
        return super().load(address,size)
    def save(self,address,value,size):
        self.memory_check(address,size);super().save(address,value,size)
    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):raise ValueError('unreviewed manager code')
        offset=pc-0xFF000
        if offset<0 or offset+4>len(self.original):raise ValueError('manager image code bound')
        return struct.unpack_from('<I',self.original,offset)[0]
    def execute(self,w,pc):
        if self.instruction_count>=12000:raise ValueError('manager instruction bound')
        op,rs,rt,rd,sh,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
        im=w&65535;si=im-65536 if im&32768 else im;target=None;annul=False
        self.instruction_count+=1;self.visited.add(pc)
        if w==0:pass
        elif op==0:
            if fn in (0x21,0x2D) and sh==0:self.r[rd]=(self.r[rs]+self.r[rt])&MASK
            elif fn==0 and rs==0:self.r[rd]=(self.r[rt]<<sh)&MASK
            elif fn==3 and rs==0:self.r[rd]=(signed(self.r[rt])>>sh)&MASK
            elif fn==2 and rs==0:self.r[rd]=(self.r[rt]&MASK)>>sh
            elif fn in (0x24,0x25) and sh==0:
                self.r[rd]=(self.r[rs]&self.r[rt] if fn==0x24 else self.r[rs]|self.r[rt])&MASK
            elif fn in (0x2A,0x2B) and sh==0:
                self.r[rd]=int(signed(self.r[rs])<signed(self.r[rt])) if fn==0x2A else int((self.r[rs]&MASK)<(self.r[rt]&MASK))
            elif fn==0x0A and sh==0:
                if self.r[rt]&MASK==0:self.r[rd]=self.r[rs]
            elif fn==0x18 and sh==0 and pc==0x2AAF74:
                self.r[rd]=(signed(self.r[rs])*signed(self.r[rt]))&MASK
            elif w==0x03E00008:target=self.r[31]&MASK
            elif fn==9 and rt==0 and rd==31 and sh==0:
                target=self.r[rs]&MASK;self.r[31]=pc+8
            else:raise ValueError('unsupported manager SPECIAL/bulk operands')
        elif op==9:self.r[rt]=(self.r[rs]+si)&MASK
        elif op in (12,13,14):
            self.r[rt]=(self.r[rs]&im if op==12 else self.r[rs]|im if op==13 else self.r[rs]^im)&MASK
        elif op==15 and rs==0:self.r[rt]=im<<16
        elif op==1 and rt==3:
            if signed(self.r[rs])>=0:target=(pc+4+4*si)&MASK
            else:annul=True
        elif op==3:self.r[31]=pc+8;target=((pc+4)&0xF0000000)|((w&0x3FFFFFF)<<2)
        elif op in (4,5,6,20,21):
            if op==6 and rt:raise ValueError('reserved manager BLEZ')
            take=(self.r[rs]==self.r[rt] if op in (4,20) else self.r[rs]!=self.r[rt] if op in (5,21) else signed(self.r[rs])<=0)
            if take:target=(pc+4+4*si)&MASK
            elif op in (20,21):annul=True
        elif op in (30,32,35,36,55):
            size={30:16,32:1,35:4,36:1,55:8}[op];value=self.load((self.r[rs]+si)&MASK,size)
            if op==32 and value&128:value-=256
            self.r[rt]=value&MASK if size<=4 else value
        elif op in (31,40,43,63):self.save((self.r[rs]+si)&MASK,self.r[rt],{31:16,40:1,43:4,63:8}[op])
        else:raise ValueError('unsupported manager opcode/reserved operands')
        self.r[0]=0;return target,annul
    def callback(self,target):
        if target not in CALLBACKS:raise ValueError('unknown manager callback')
        args=[self.r[i]&MASK for i in (4,5,6)]
        if args[0]!=MANAGER or not BUFFER<=args[1]<END:raise ValueError('manager three-GPR callback contract')
        self.callback_count+=1;self.events.extend((8+CALLBACKS.index(target),*args))
        if self.callback_count==1:
            if self.mutation==1:self.save(MANAGER+0x14,ALT_COMMANDS,4)
            elif self.mutation==2:self.save(COMMANDS+8,1,4)
            elif self.mutation==3:self.save(RECORDS[3]+8,NEW_TEXT,4)
            elif self.mutation==4:self.save(MANAGER+0x10,ALT_RECORD,4)
            elif self.mutation==5:
                self.save(MANAGER+0x14,ALT_COMMANDS,4);self.save(ALT_COMMANDS+8,3,4)
            elif self.mutation==6:
                self.save(COMMAND_RECORDS+0x40+4,0xABCDEF01,4)
                self.save(COMMAND_RECORDS+0x40,CALLBACKS[1],4)
            elif self.mutation==7:
                self.save(MANAGER+0x24,0xFFFFFFFF,4);self.save(MANAGER+0x20,0x80000000,4)
        for r in range(2,16):self.r[r]=0xDEADBEEF
    def run(self,entry,stop=RETURN,depth=0):
        index=next((i for i,(a,b) in enumerate(RANGES) if a==entry),None)
        if index is None or depth>4:raise ValueError('manager whole entry/depth')
        a,b=RANGES[index];self.invocations[index]+=1;pc=a
        if index>=3:
            args=[self.r[i]&MASK for i in (4,5,6)]
            self.events.extend((index,*args))
        if index==7:
            dest,source,count=args
            if count>64 or count>=8 and (dest|source)&7==0:raise ValueError('manager strncpy bulk domain')
            if count and not(dest+count<=source or source+count<=dest):raise ValueError('manager strncpy overlapping domain')
        if len(self.events)>160:raise ValueError('manager event bound')
        while True:
            if not a<=pc<b:raise ValueError('manager control outside selected body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                if not annul:
                    if pc+4>=b:raise ValueError('manager delay outside body')
                    delay=self.fetch(pc+4)
                    if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('manager encoded control in delay')
                if w==0x03E00008:
                    if target!=stop:raise ValueError('manager actual JR31 stop required')
                    return
                if w>>26==3 or w>>26==0 and w&63==9:
                    continuation=self.r[31]
                    if target in tuple(p[0] for p in RANGES[3:]):self.run(target,continuation,depth+1)
                    else:self.callback(target)
                    if self.r[31]!=continuation:raise ValueError('manager callee changed RA')
                    pc=continuation
                else:
                    if target is not None and not a<=target<b:raise ValueError('manager branch crosses body')
                    pc=target if target is not None else pc+8
            else:
                if target is not None or annul:raise ValueError('manager noncontrol transfer')
                pc+=4

def fixture(original,routine=0,character=65,text=b'Abc',line_count=3,line_limit=8,
            line_used=0,history_limit=3,history_count=0,copy_limit=8,
            commands=0,mutation=0,alias=0,flags=0x80000001):
    if routine not in range(3) or mutation not in range(8) or alias not in range(7):raise ValueError('manager fixture selector')
    if not -1<=character<=127 or len(text)>64 or not 0<=copy_limit<=64:raise ValueError('manager fixture readable domain')
    t=ManagerTrace(original,mutation);pointers=set();functions=set()
    for i in range(WORDS):t.save(BUFFER+4*i,initial_word(i),4)
    def ptr(a,v):pointers.add(a);t.save(a,v,4)
    def string(a,v):
        for i in range(80):t.save(a+i,v[i] if i<len(v) else 0,1)
    t.save(MANAGER,flags,4);t.save(MANAGER+4,line_count&MASK,4);t.save(MANAGER+8,copy_limit,4)
    ptr(MANAGER+0xC,LINES);ptr(MANAGER+0x10,RECORDS[3]);ptr(MANAGER+0x14,COMMANDS)
    t.save(MANAGER+0x1C,history_limit&MASK,4);t.save(MANAGER+0x20,0x13579BDF,4);t.save(MANAGER+0x24,history_count&MASK,4);ptr(MANAGER+0x28,HISTORY)
    for i in range(-2,6):ptr(LINES+4*i,RECORDS[i%4]);ptr(ALT_LINES+4*i,RECORDS[(i+1)%4]);ptr(HISTORY+4*i,HISTORY_TEXTS[i%4]);ptr(ALT_HISTORY+4*i,HISTORY_TEXTS[(i+1)%4])
    for i,a in enumerate((*RECORDS,ALT_RECORD)):
        t.save(a,line_limit&MASK,4);t.save(a+4,line_used&MASK,4);ptr(a+8,TEXTS[i] if i<4 else ALT_TEXT)
    for i,a in enumerate(TEXTS):string(a,text if i==3 else bytes([97+i,98+i,99+i]))
    string(ALT_TEXT,b'alt');string(INPUT,text);string(NEW_TEXT,b'gone')
    for i,a in enumerate(HISTORY_TEXTS):string(a,bytes([104+i,105+i,106+i]))
    for a,records,n in ((COMMANDS,COMMAND_RECORDS,commands),(ALT_COMMANDS,ALT_COMMAND_RECORDS,1)):
        t.save(a,0x40,4);t.save(a+4,0x11223344,4);t.save(a+8,n&MASK,4);ptr(a+12,records)
        for i,pattern in enumerate((b'go',b'',b'one',b'x')):
            t.save(records+i*0x40,CALLBACKS[i%2],4);functions.add(records+i*0x40)
            t.save(records+i*0x40+4,0x12340000+i,4)
            for j in range(32):t.save(records+i*0x40+8+j,pattern[j] if j<len(pattern) else 0,1)
    if alias==1:
        # The second pointer store changes line_count from3 to2; final slot is
        # captured using the loop index, not the fresh count.
        ptr(MANAGER+0xC,MANAGER);ptr(MANAGER,RECORDS[1]);t.save(MANAGER+4,3,4);t.save(MANAGER+8,2,4);pointers.add(MANAGER+8)
    elif alias==2:
        # History pointer shift changes capacity while preserving a valid
        # final slot and a valid zero-length alternate command list.
        ptr(MANAGER+0x28,MANAGER+0x14);ptr(MANAGER+0x18,ALT_COMMANDS)
        t.save(MANAGER+0x1C,4,4);t.save(MANAGER+0x20,2,4);t.save(MANAGER+0x24,4,4);t.save(MANAGER+8,2,4);t.save(ALT_COMMANDS+8,0,4)
    elif alias==3:ptr(RECORDS[3]+8,RECORDS[3]+4)
    elif alias==4:
        # A low-byte pointer store is portable under the native observer's
        # explicitly aligned storage, not arbitrary host address placement.
        ptr(RECORDS[3]+8,RECORDS[3]+8)
    elif alias==5:
        # The input pointer remains fixed while output changes later bytes.
        ptr(RECORDS[3]+8,INPUT);t.save(RECORDS[3]+4,1,4)
    elif alias==6:
        ptr(RECORDS[0]+8,LINES+4)
    before=[t.load(BUFFER+i*4,4) for i in range(WORDS)]
    t.r[4]=MANAGER;t.r[5]=(character&MASK) if routine==0 else INPUT;t.r[29]=0x80000;t.r[31]=RETURN
    # Helper event arguments are meaningful only for the actual ABI lanes;
    # normalize unused incoming lanes so native records need not model garbage.
    t.r[6]=0
    t.run(RANGES[routine][0])
    after=[t.load(BUFFER+i*4,4) for i in range(WORDS)]
    event=[]
    for i in range(0,len(t.events),4):
        kind,a,b,c=t.events[i:i+4]
        if kind in (3,5):b=c=0
        elif kind==4:c=0
        elif kind==6:c=0
        event.extend((kind,a,b,c))
    return dict(routine=routine,character=character,mutation=mutation,alias=alias,
        text=list(text),line_count=line_count,line_limit=line_limit,line_used=line_used,
        history_limit=history_limit,history_count=history_count,copy_limit=copy_limit,
        commands=commands,flags=flags,initial=[[i,v] for i,v in enumerate(before) if v!=initial_word(i)],
        changes=[[i,v] for i,v in enumerate(after) if v!=before[i]],
        pointers=sorted((p-BUFFER)//4 for p in pointers),functions=sorted((p-BUFFER)//4 for p in functions),
        result=t.r[2]&MASK if routine==2 else 0,events=event,invocations=t.invocations,
        instructions=t.instruction_count,visited=sorted(t.visited))

def fixtures(original):
    cases=[]
    for character in (-1,0,1,9,32,33,48,64,65,70,90,91,96,97,122,123,127):
        for used,limit in ((-1,0),(0,0),(0,1),(1,2),(2,2),(3,2),(0,0x7FFFFFFF),(0,0x80000000)):
            cases.append(fixture(original,character=character,line_used=used,line_limit=limit))
    for character in (10,13):
        for text in (b'',b'g',b'go',b'gone',b'zgo',b'no'):
            for limit,count in ((1,0),(1,1),(2,1),(2,2),(3,0),(3,3),(0,0),(-1,-1)):
                for copy in (0,1,4,8):cases.append(fixture(original,character=character,text=text,history_limit=limit,history_count=count,copy_limit=copy,commands=3))
    for mutation in range(8):
        for count in (1,2,3):cases.append(fixture(original,character=10,text=b'gone',commands=count,mutation=mutation))
    for n in (-1,0,1,2,3,4):cases.append(fixture(original,character=10,text=b'',line_count=n))
    for text in (b'',b'a',b'abc',b'Ab\rZ',b'A\nB',b'12345678',b'abc\n\rxyz',bytes([128,255,1])):
        for n in (-1,0,1,2,3,4):
            for used,limit in ((-1,1),(0,0),(0,1),(0,4),(4,4),(5,4)):
                cases.append(fixture(original,1,text=text,line_count=n,line_used=used,line_limit=limit))
    cases.append(fixture(original,character=10,text=b'',alias=1))
    cases.append(fixture(original,1,text=b'\n',alias=1))
    cases.append(fixture(original,character=13,text=b'go',alias=2))
    for alias in (3,4):
        for character in (0,32,65,97,127):cases.append(fixture(original,character=character,alias=alias))
        for text in (b'a',b'c',b'A'):cases.append(fixture(original,1,text=text,alias=alias,line_limit=4))
    for text in (b'abc',b'A\nB',b'z'):
        cases.append(fixture(original,1,text=text,alias=5,line_limit=8))
    for routine in (0,1):cases.append(fixture(original,routine,character=10,text=b'\n' if routine else b'',alias=6))
    for flags in (0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFF):cases.append(fixture(original,2,flags=flags))
    return cases

def golden(cases):
    maximum=lambda key:max(len(c[key]) for c in cases)
    lines=['/* Authored instruction observations; no original game bytes. */',
        '#define MANAGER_WORDS %du'%WORDS,'struct ManagerPair {u32 index,value;};',
        'struct ManagerGolden {u32 routine,character,mutation,result,initial_count,change_count,pointer_count,function_count,event_count;struct ManagerPair initial[%d],changes[%d];u32 pointers[%d],functions[%d],events[%d],invocations[8];};'%(maximum('initial'),maximum('changes'),maximum('pointers'),maximum('functions'),maximum('events')),
        'static const struct ManagerGolden manager_golden[]={']
    pairs=lambda p:'{'+','.join('{%du,0x%08Xu}'%(i,v) for i,v in p)+'}'
    words=lambda p:'{'+','.join('0x%08Xu'%v for v in p)+'}'
    for c in cases:
        fields=[c[k]&MASK for k in ('routine','character','mutation','result')]+[len(c[k]) for k in ('initial','changes','pointers','functions','events')]
        lines.append('{'+','.join('%du'%v for v in fields)+','+pairs(c['initial'])+','+pairs(c['changes'])+','+','.join(words(c[k]) for k in ('pointers','functions','events','invocations'))+'},')
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/buffer_manager/trace.json');p.add_argument('--header',type=Path);a=p.parse_args()
    _,raw=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(raw)
    keys=('routine','character','mutation','alias','text','line_count','line_limit','line_used','history_limit','history_count','copy_limit','commands','flags')
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),maximum=max(c['instructions'] for c in cases),
        input_sha256=hashlib.sha256(json.dumps([{k:c[k] for k in keys} for c in cases],sort_keys=True).encode()).hexdigest(),
        coverage=[len({pc for c in cases for pc in c['visited'] if a<=pc<b}) for a,b in RANGES],cases=cases)
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n')
    if a.header:a.header.write_text(golden(cases),newline='\n')
    print(report['fixtures'],'manager fixtures;',report['instructions'],'instructions;',report['coverage'],'coverage')

if __name__=='__main__':main()
