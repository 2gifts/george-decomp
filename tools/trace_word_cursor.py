"""Initialized scalar cursor effects through actual original PC ranges.

Reuses the whole unchanged string RegistryTrace decoder and Trace run protocol.
This observes lower64 and owned scalar cells, not an engine class or capacity.
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
BASE,WORDS=0x20000,640
END=BASE+WORDS*4
RANGES=((0x2D4EB0,0x2D4ECC),(0x2D4ED0,0x2D4EEC),(0x2D4EF0,0x2D4F04))
CURSORS=(8,12,12)
LANES=(0x10,0x810,0x810)
VALUES=(0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFF,0x5A83C617,0xFEDCAB89)
OWNERS=(0,8,32)

def initial_word(i,salt):return (0x63CA8701^(i*0x1030521)^salt)&MASK
def sign_word(x):return x if x<0x80000000 else x+(U64-MASK)

class WordCursorTrace(Trace):
    def __init__(self,original,routine=0):
        super().__init__(original)
        if type(routine) is not int or not 0<=routine<3:raise ValueError('unreviewed cursor entry')
        self.routine=routine;self.visited=set();self.events=[];self.defined={0,4,31}
        self.expected_words={}
        a,b=RANGES[routine]
        for pc in range(a,b,4):
            off=pc-0xFF000
            if off>=0 and off+4<=len(original):self.expected_words[pc]=struct.unpack_from('<I',original,off)[0]
    @staticmethod
    def memory_check(a,n):
        if n!=4 or a%4 or not BASE<=a<a+4<=END:raise ValueError('unowned or unaligned cursor memory')
    def initialized_check(self,a,n):
        self.memory_check(a,n)
        if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized cursor memory')
    def load(self,a,n):
        self.initialized_check(a,n);v=Trace.load(self,a,n);self.events.append(['load',a,v]);return v
    def save(self,a,v,n):
        self.initialized_check(a,n);self.events.append(['store',a,v&MASK]);Trace.save(self,a,v,n)
    def fetch(self,pc):
        if pc not in self.expected_words:raise ValueError('unreviewed cursor PC')
        w=struct.unpack_from('<I',self.original,pc-0xFF000)[0]
        if w!=self.expected_words[pc]:raise ValueError('changed cursor instruction')
        return w
    def execute(self,w,pc):
        if pc not in self.expected_words or w!=self.expected_words[pc]:raise ValueError('unreviewed cursor PC/word')
        if self.instruction_count>=8:raise ValueError('cursor instruction budget')
        op,rs,rt,rd,sh,fn,im=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63,w&65535
        slot=(pc-RANGES[self.routine][0])//4
        peek=self.routine==2
        dest=None;required=[]
        if op==35:
            first=(slot==0 and rs==4 and rt==(2 if peek else 5) and im==CURSORS[self.routine])
            last=(slot==(4 if peek else 4) and rs==(4 if peek else 3) and rt==2 and im==LANES[self.routine])
            if not(first or last):raise ValueError('reserved cursor load operands')
            required=[rs];dest=rt
        elif op==43:
            if peek or slot!=6 or (rs,rt,im)!=(4,5,CURSORS[self.routine]):raise ValueError('reserved cursor store operands')
            required=[rs,rt]
        elif op==9:
            if peek or slot!=3 or (rs,rt,im)!=(5,5,1):raise ValueError('reserved cursor successor operands')
            required=[rs];dest=rt
        elif op==0 and fn==0:
            if slot!=1 or (rs,rt,rd,sh)!=(0,2 if peek else 5,2 if peek else 3,2):raise ValueError('reserved cursor shift operands')
            required=[rt];dest=rd
        elif op==0 and fn==0x21:
            if slot!=2 or (rs,rt,rd,sh)!=(4,2 if peek else 3,4 if peek else 3,0):raise ValueError('reserved cursor sum operands')
            required=[rs,rt];dest=rd
        elif op==0 and fn==8:
            if slot!=(3 if peek else 5) or (rs,rt,rd,sh)!=(31,0,0,0):raise ValueError('reserved cursor JR operands')
            required=[31]
            if self.r[31]&MASK!=RETURN:raise ValueError('unexpected cursor JR31 target')
        else:raise ValueError('unsupported cursor family')
        if any(i not in self.defined for i in required):raise ValueError('undefined cursor register')
        if op in (35,43):
            si=im-65536 if im&32768 else im
            self.initialized_check(((self.r[rs]&MASK)+si)&MASK,4)
        result=RegistryTrace.execute(self,w,pc)
        if dest is not None:self.defined.add(dest)
        return result
    def run(self,entry=None):
        a,b=RANGES[self.routine]
        if entry is None:entry=a
        if entry!=a or self.r[31]&MASK!=RETURN:raise ValueError('unreviewed cursor entry/return')
        jr=b-8
        if is_control_transfer(self.fetch(jr+4)):raise ValueError('control transfer in cursor delay')
        return Trace.run(self,entry)

def make_fixture(original,routine,cursor,payload,owner,salt):
    if (type(routine) is not int or not 0<=routine<3 or type(owner) is not int or owner not in OWNERS or
            any(type(v) is not int or not 0<=v<=MASK for v in (cursor,payload,salt))):raise ValueError('invalid cursor fixture')
    trace=WordCursorTrace(original,routine)
    cells=[initial_word(i,salt) for i in range(WORDS)]
    p=BASE+owner*4;ci=owner+CURSORS[routine]//4
    cells[ci]=cursor
    ea=(p+((cursor<<2)&MASK)+LANES[routine])&MASK
    WordCursorTrace.memory_check(ea,4);ei=(ea-BASE)//4
    if ei!=ci:cells[ei]=payload
    for i,v in enumerate(cells):Trace.save(trace,BASE+i*4,v,4)
    expected=cells[:];result=sign_word(cells[ei])
    ledger=[['load',p+CURSORS[routine],cursor],['load',ea,cells[ei]]]
    if routine!=2:
        expected[ci]=(cursor+1)&MASK;ledger.append(['store',p+CURSORS[routine],expected[ci]])
    trace.r[4],trace.r[31]=p,RETURN;trace.run()
    actual=[Trace.load(trace,BASE+i*4,4) for i in range(WORDS)]
    assert actual==expected and trace.r[2]&U64==result and trace.events==ledger
    return dict(routine=routine,cursor=cursor,payload=payload,owner=owner,salt=salt,
                element_cell=ei,initial=cells,expected=actual,result=trace.r[2]&U64,
                events=trace.events,instructions=trace.instruction_count,visited=sorted(trace.visited))

def fixtures(original):
    cases=[]
    for routine in range(3):
        indices=(0,1,2,7,15,63,0x40000000,0x80000001,0xC0000007,0x7FFFFFFF,0xFFFFFFFF)
        indices+=((0xFFFFFFFE,) if routine==0 else (0xFFFFFDFF,0xFFFFFDFE))
        for owner in OWNERS:
            for cursor in indices:
                for payload in VALUES:
                    cases.append(make_fixture(original,routine,cursor,payload,owner,(0x19A74301*(len(cases)+1))&MASK))
    return cases

def golden_header(cases):
    lines=['/* Synthetic parameters/results; no original instruction or asset arrays. */',
           'struct WordCursorGolden { u32 routine,cursor,payload,owner,salt,element_cell; GeorgeWordCursorResult result; };',
           'static const struct WordCursorGolden word_cursor_golden[] = {']
    for c in cases:
        lines.append(' {%s,0x%016XULL},'%(','.join('0x%08Xu'%c[k] for k in ('routine','cursor','payload','owner','salt','element_cell')),c['result']))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/word_cursor/trace.json');p.add_argument('--golden-header',type=Path)
    a=p.parse_args();_,original=validated_elf(ROOT/'orig/SLUS_216.68');cases=fixtures(original)
    inputs=[{k:c[k] for k in ('routine','cursor','payload','owner','salt')} for c in cases]
    report=dict(fixtures=len(cases),instructions=sum(c['instructions'] for c in cases),max_instructions=max(c['instructions'] for c in cases),
        coverage=[len({pc for c in cases for pc in c['visited'] if lo<=pc<hi}) for lo,hi in RANGES],
        input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),cases=cases,
        limitation='Initialized scalar storage/lower64 and wrapped32 addresses only; no original class/prototype/capacity/upper128/runtime invocation claim.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n')
    if a.golden_header:a.golden_header.parent.mkdir(parents=True,exist_ok=True);a.golden_header.write_text(golden_header(cases),newline='\n')
    print('word cursor: %d fixtures, %d instructions, coverage%s'%(report['fixtures'],report['instructions'],report['coverage']))
if __name__=='__main__':main()
