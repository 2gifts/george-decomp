"""New bounded allocation/copy/release fixtures, with unchanged shared decoding.

All three complete original bodies execute at their true PCs. Allocation and
release are controlled nonnull caller observations, not engine heap/OS models.
Only initialized capacities, defined shifts, disjoint or exact self-copy are
admitted. No source instruction arrays or general Bigint class claim are used.
"""
import argparse
import json
from pathlib import Path
from analyze import validated_elf
from trace_geometry import RETURN
from trace_string_registry import sx32
from trace_libio_bigint import (LibioBigintTrace, initial_word, BASE, WORDS,
                               END, STACK, A, B, NEW, MASK)

ROOT=Path(__file__).resolve().parents[1]
SELECTED=((0x35e910,96),(0x35e970,36),(0x35e998,68))


class LibioAllocatorTrace(LibioBigintTrace):
    def fetch(self,pc):
        if not any(a<=pc<a+n for a,n in SELECTED):
            raise ValueError('unreviewed allocator instruction or padding')
        return super().fetch(pc)

    def execute(self,word,pc):
        if self.instruction_count>=1200:
            raise ValueError('allocator budget before mutation')
        return super().execute(word,pc)

    def external(self,target):
        if target==0x2af140:
            size=self.r[4]&MASK
            if size not in (52,84,148) or self.allocations:
                raise ValueError('allocator nonnull event outside capacity domain')
            self.allocations=1
            self.events.append([1,size,NEW-BASE])
            if self.p['mutation']:
                self.save(NEW+4,0x76543210,4)
                self.save(NEW+8,0x12345678,4)
                self.save(NEW+12,0xabcd1234,4)
                self.save(NEW+16,7,4)
            self.r[2]=NEW
        elif target==0x2af1e8:
            pointer=self.r[4]&MASK
            if pointer not in (A,B) or self.events:
                raise ValueError('allocator release event outside object domain')
            self.events.append([2,pointer-BASE,self.load(pointer+12,2)])
            if self.p['mutation']:
                self.save(pointer+14,self.load(pointer+14,2)^0x55aa,2)
                self.save(pointer+16,3,4)
        else:
            raise ValueError('unreviewed allocator external target')

    def initialize(self):
        p=self.p
        if p['k'] not in (3,4,5) or not 0<=p['n']<=1<<p['k'] or p['selfcopy'] not in (0,1) or p['flip'] not in (0,1):
            raise ValueError('invalid allocator initialized capacity/copy domain')
        if p['method']==0 and p['argument'] not in (-5,-1,0,1,2,3,4,5):
            raise ValueError('allocator argument outside defined-shift domain')
        for i in range(WORDS): self.save(BASE+4*i,initial_word(i,p['seed']),4)
        for address in range(*STACK): self.memory[address]=(address*13+p['seed']*7)&255
        for address,limbs,sign in ((A,p['a'],p['sign']),(B,p['b'],p['sign']^0x55aa)):
            self.save(address+4,p['k'],4);self.save(address+8,1<<p['k'],4)
            self.save(address+12,p['stack'],2);self.save(address+14,sign,2)
            self.save(address+16,p['n'],4)
            for i,v in enumerate(limbs):self.save(address+20+4*i,v,4)
        self.r[29]=0x80800;self.r[31]=RETURN

    def observe(self):
        self.initialize();p=self.p
        if p['method']==0:self.r[4]=sx32(p['argument'])
        elif p['method']==1:self.r[4]=0 if p['null'] else A
        elif p['method']==2:
            self.r[4],self.r[5]=(B,A) if p['flip'] else (A,B)
            if p['selfcopy']:self.r[5]=self.r[4]
        else:raise ValueError('unreviewed allocator selector')
        self.run(SELECTED[p['method']][0])
        return dict(parameters=p,return_value=(self.r[2]&MASK)-BASE if p['method']==0 else 0,
            void_return_ignored=p['method']!=0,memory=[self.load(BASE+4*i,4) for i in range(WORDS)],
            events=self.events,instruction_count=self.instruction_count,visited=sorted(self.visited),
            branches=self.branches,invocations=self.invocations)


def fixtures():
    out=[]
    def add(method,**kw):
        p=dict(method=method,seed=len(out),k=3,n=1,sign=0x8001,stack=0,argument=3,
               mutation=0,null=0,selfcopy=0,flip=0)
        p.update(kw)
        p['a']=[(0x80000001^i*0x1020301^p['seed']*13)&MASK for i in range(p['n'])]
        p['b']=[(0xfedcba98^i*0x3040501^p['seed']*17)&MASK for i in range(p['n'])]
        out.append(p)
    for k in (-5,-1,0,1,2,3,4,5):
        for mutation in (0,1):add(0,argument=k,mutation=mutation)
    for null in (0,1):
        for flag in (0,1,2,0x7fff,0x8000,0x8001,0xffff):
            for mutation in (0,1):add(1,null=null,stack=flag,mutation=mutation)
    for k in (3,4,5):
        for n in (0,1,2,7,8,9,15,16,31,32):
            if n>1<<k:continue
            for sign in (0,1,0x8000,0xffff):
                for selfcopy in (0,1):
                    for flip in (0,1):add(2,k=k,n=n,sign=sign,selfcopy=selfcopy,flip=flip)
    return out


def generate(original):
    observations=[LibioAllocatorTrace(original,p).observe() for p in fixtures()]
    pcs={pc for a,n in SELECTED for pc in range(a,a+n,4)}
    visited=set().union(*(set(x['visited']) for x in observations))
    branches={}
    for x in observations:
        for pc,taken in x['branches']:branches.setdefault(pc,set()).add(taken)
    return dict(summary=dict(fixture_count=len(observations),original_instructions=sum(x['instruction_count'] for x in observations),
        maximum_instructions=max(x['instruction_count'] for x in observations),selected_total_words=len(pcs),selected_visited_words=len(pcs&visited),
        unvisited_selected_pcs=['0x%08X'%p for p in sorted(pcs-visited)],
        branch_outcomes=[dict(pc='0x%08X'%pc,outcomes=sorted(v)) for pc,v in sorted(branches.items())],
        source_or_helper_data_award=False,controlled_nonnull_allocation_and_free=True),observations=observations)


def golden(packet,path):
    arr=lambda v:'{'+','.join('0x%08Xu'%(x&MASK) for x in v)+'}'
    lines=['/* Authored inputs and exact original-observed sparse deltas; every arena word is checked. */',
        '#define LIBIO_ALLOCATOR_WORDS %d'%WORDS,
        'typedef struct { unsigned method,seed,k,n,sign,stack,argument,mutation,is_null,selfcopy,flip; unsigned a[32],b[32]; unsigned result,event_n,events[1][3]; unsigned delta_n,deltas[128][2]; } LibioAllocatorFixture;',
        'static const LibioAllocatorFixture libio_allocator_fixtures[] = {']
    for x in packet['observations']:
        p=x['parameters'];fields=[p[k]&MASK for k in ('method','seed','k','n','sign','stack','argument','mutation','null','selfcopy','flip')]
        ds=[[i,v] for i,v in enumerate(x['memory']) if v!=initial_word(i,p['seed'])]
        assert len(ds)<=128
        parts=[','.join('0x%08Xu'%v for v in fields),arr(p['a']),arr(p['b']),str(x['return_value'])+'u',str(len(x['events']))+'u',
            '{'+','.join(arr(e) for e in x['events'])+'}',str(len(ds))+'u','{'+','.join(arr(d) for d in ds)+'}']
        lines.append('{'+','.join(parts)+'},')
    lines.append('};');path.write_text('\n'.join(lines)+'\n',newline='\n')


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/mprec_second_allocator/trace.json')
    p.add_argument('--golden-header',type=Path)
    a=p.parse_args();_,original=validated_elf(a.original);packet=generate(original)
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(packet,separators=(',',':'))+'\n',newline='\n')
    if a.golden_header:a.golden_header.parent.mkdir(parents=True,exist_ok=True);golden(packet,a.golden_header)
    print(json.dumps(packet['summary'],indent=2))
