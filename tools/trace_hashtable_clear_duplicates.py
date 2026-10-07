"""Actual five complete clear bodies on authored prefix graphs.

Reuses the unchanged published integer decoder and435 parameter distributions.
No PC substitution, class/complete-node-size identity or allocator execution.
Sixteen-byte node carriers and32-byte table carriers are synthetic storage only.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_hashtable_clear import ClearTrace,parameters as published_parameters,bucket_for

ROOT=Path(__file__).resolve().parents[1]
ENTRIES=(0x24B7D0,0x24BF58,0x24C320,0x24C6E8,0x24CE78)
TABLE,BUCKETS,NODES,FREE,HEADS=0x20100,0x20200,0x20400,0x20600,0x3F21B8
REGIONS=((TABLE,32),(BUCKETS,64),(NODES,192),(FREE,64),(HEADS,64))
MASK=0xFFFFFFFF

class DuplicateClearTrace(ClearTrace):
    def __init__(self,original,routine):
        if type(routine)is not int or routine not in range(5):raise ValueError('clear duplicate routine domain')
        super().__init__(original);self.entry=ENTRIES[routine];self.stop=self.entry+156

    def check(self,a,n):
        if n!=4 or a&3 or not any(lo<=a<a+n<=lo+size for lo,size in REGIONS):
            raise ValueError('unowned/unaligned duplicate clear memory')

    def fetch(self,pc):
        if pc&3 or not self.entry<=pc<self.stop:raise ValueError('unreviewed duplicate clear instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('duplicate clear image bounds')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        if pc&3 or not self.entry<=pc<self.stop:raise ValueError('unreviewed duplicate clear execution PC')
        return ClearTrace.execute(self,w,pc)

    def run(self):
        # Complete published run shape: only ENTRY/STOP become instance-owned.
        pc=self.entry
        while True:
            if not self.entry<=pc<self.stop:raise ValueError('clear transfer outside complete body')
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                self.branches.append([pc,target is not None])
                if annul:
                    if target is not None:raise ValueError('clear annul taken')
                    pc+=8;continue
                delay=self.fetch(pc+4)
                if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):
                    raise ValueError('clear control in delay')
                if w==0x03E00008:
                    if pc!=self.stop-8 or target!=RETURN:raise ValueError('clear actual terminal JR31/stop')
                    return
                pc=pc+8 if target is None else target
            else:
                if target is not None or annul:raise ValueError('clear noncontrol transfer')
                pc+=4

def parameters():
    base=published_parameters();assert len(base)==435
    inputs=[]
    for routine in range(5):
        for p in base:inputs.append(dict(routine=routine,alias=0,**p))
        # Valid typed void* bucket cells can reside in other global words while
        # class1 remains disjoint. This is an authored alias domain, no claim
        # that unrelated original allocator classes hold these live objects.
        for b in (1,3,8):
            for n in (0,1,6,12):
                for d in range(4):
                    for pool in (0,1,4):inputs.append(dict(routine=routine,alias=1,buckets=b,nodes=n,distribution=d,pool=pool))
        for pool in (0,1,4):inputs.append(dict(routine=routine,alias=2,buckets=0,nodes=0,distribution=0,pool=pool))
    return inputs

def initialize(t,p):
    if p not in parameters():raise ValueError('duplicate clear fixture domain')
    for lo,n in REGIONS:
        for a in range(lo,lo+n,4):t.save(a,0,4)
    bucket=BUCKETS if p['alias']==0 else HEADS+16 if p['alias']==1 else HEADS+4
    t.save(TABLE,0xA5310201,4);t.save(TABLE+4,bucket,4)
    t.save(TABLE+8,bucket+4*p['buckets'],4);t.save(TABLE+12,0xBD310201,4)
    t.save(TABLE+16,p['nodes'],4)
    for j in range(3):t.save(TABLE+20+4*j,0xBCF10000+j,4)
    for i in range(12):
        for j in range(3):t.save(NODES+16*i+4+4*j,(0x80000000+i*0x010101+j*0x112233)&MASK,4)
    for i in reversed(range(p['nodes'])):
        b=bucket+4*bucket_for(i,p)
        t.save(NODES+16*i,t.load(b,4),4);t.save(b,NODES+16*i,4)
    for i in range(4):
        t.save(FREE+16*i,FREE+16*(i+1) if i+1<p['pool'] else 0,4)
        for j in range(3):t.save(FREE+16*i+4+4*j,(0xE5010000+i+j*0x334455)&MASK,4)
    t.save(HEADS+4,FREE if p['pool'] else 0,4)
    t.r[4]=TABLE;t.r[31]=RETURN;t.initializing=False

def fixture(original,p):
    t=DuplicateClearTrace(original,p['routine']);initialize(t,p);t.run()
    output=[Trace.load(t,a,4) for lo,n in REGIONS for a in range(lo,lo+n,4)]
    return dict(parameters=p,expected=output,instructions=t.instruction_count,
                visited=sorted(t.visited),branches=t.branches,accesses=t.accesses)

def golden_header(rows):
    lines=['/* Authored initialized prefix carriers; no original bytes/classes. */',
      'struct DuplicateClearGolden { u32 routine,alias,buckets,nodes,distribution,pool; u32 expected[104]; };',
      'static const struct DuplicateClearGolden duplicate_clear_golden[] = {']
    for row in rows:
        p=row['parameters'];lanes=','.join('%du'%p[k] for k in ('routine','alias','buckets','nodes','distribution','pool'))
        lines.append('{%s,{%s}},'%(lanes,','.join('0x%08Xu'%v for v in row['expected'])))
    return '\n'.join(lines+['};',''])

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    p.add_argument('--output',type=Path,default=ROOT/'build/hashtable_clear_duplicates/trace.json')
    p.add_argument('--golden-header',type=Path);a=p.parse_args()
    _,original=validated_elf(a.elf);inputs=parameters();rows=[fixture(original,x) for x in inputs]
    visited=sorted({pc for x in rows for pc in x['visited']})
    assert visited==sorted(pc for entry in ENTRIES for pc in range(entry,entry+156,4))
    for entry in ENTRIES:
        for delta in (20,60,96,140):assert {taken for x in rows for pc,taken in x['branches'] if pc==entry+delta}=={False,True}
    report=dict(schema_version=1,fixtures=len(rows),base_distributions_reused=435,
      new_actual_function_runs=5,selected_coverage=195,visited=visited,
      input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),
      total_instructions=sum(x['instructions'] for x in rows),maximum_instructions=max(x['instructions'] for x in rows),observations=rows,
      qualification='Actual new5 PC ranges; unchanged published39-word decoder and435 parameter distributions only. Authored16-byte node/32-byte table carriers do not infer original complete objects. Initialized finite acyclic/disjoint prefix graphs, typed global bucket aliases and untouched tails; no allocator/helper/data/class/lifetime/FCR/timing/concurrency award. Historical50496 native count is not included.')
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2)+'\n',newline='\n')
    if a.golden_header:a.golden_header.parent.mkdir(parents=True,exist_ok=True);a.golden_header.write_text(golden_header(rows),newline='\n')
    print('Duplicate clear fixtures',len(rows),'actual original instructions',report['total_instructions'],'max',report['maximum_instructions'])

if __name__=='__main__':main()
