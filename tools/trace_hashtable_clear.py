"""Strict initialized integer observations of the complete39-word clear body.

Original instructions are read only from the validated local ELF. Synthetic
typed-prefix observations carry no C++ lifetime, concurrent allocator, upper
register, timing, exception, or general hardware claim.
"""
import argparse, hashlib, json, struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace, RETURN, is_control_transfer
from trace_string_registry import RegistryTrace

ROOT=Path(__file__).resolve().parents[1]
ENTRY,STOP=0x2BF418,0x2BF4B4
TABLE,BUCKETS,NODES,FREE,HEADS=0x20100,0x20200,0x20400,0x20600,0x3F21B8
REGIONS=((TABLE,20),(BUCKETS,64),(NODES,144),(FREE,48),(HEADS,64))
MASK=0xFFFFFFFF
sx=lambda n:(n&MASK)-0x100000000 if n&0x80000000 else n&MASK


class ClearTrace(Trace):
    def __init__(self,original):
        super().__init__(original)
        self.visited=set();self.branches=[];self.accesses=[];self.initializing=True

    def check(self,a,n):
        if n!=4 or a&3 or not any(lo<=a<a+n<=lo+size for lo,size in REGIONS):
            raise ValueError('unowned/unaligned clear memory')

    def load(self,a,n):
        self.check(a,n)
        if not all(a+i in self.memory for i in range(n)):
            raise ValueError('uninitialized clear memory')
        v=super().load(a,n)
        if not self.initializing:self.accesses.append(['read',a,v])
        return v

    def save(self,a,v,n):
        self.check(a,n)
        super().save(a,v,n)
        if not self.initializing:self.accesses.append(['write',a,v&MASK])

    def fetch(self,pc):
        if pc&3 or not ENTRY<=pc<STOP:raise ValueError('unreviewed clear instruction')
        off=pc-0xFF000
        if off<0 or off+4>len(self.original):raise ValueError('clear image bounds')
        return struct.unpack_from('<I',self.original,off)[0]

    def execute(self,w,pc):
        # Check the local budget before the shared decoder mutates counters,
        # registers, memory or coverage. Keep its full64 DADDU semantics.
        if self.instruction_count>=1200:raise ValueError('clear instruction bound')
        op=w>>26;rs=w>>21&31;sh=w>>6&31;fn=w&63
        allowed=(w==0 or
            op==0 and (fn in (0,3) and rs==0 or
                       fn in (0x21,0x23,0x2D,0x2B) and sh==0 or
                       w==0x03E00008) or
            op in (9,0x23,0x2B,4,5,21) or op==15 and rs==0)
        if not allowed:raise ValueError('unsupported clear instruction family/operands')
        return RegistryTrace.execute(self,w,pc)

    def run(self):
        pc=ENTRY
        while True:
            if not ENTRY<=pc<STOP:raise ValueError('clear transfer outside complete body')
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
                    if pc!=STOP-8 or target!=RETURN:raise ValueError('clear actual terminal JR31/stop')
                    return
                pc=pc+8 if target is None else target
            else:
                if target is not None or annul:raise ValueError('clear noncontrol transfer')
                pc+=4


def parameters():
    return [dict(buckets=b,nodes=n,distribution=d,pool=p)
            for b in (0,1,2,3,5,8,16)
            for n in ((0,) if b==0 else (0,1,2,3,6,12))
            for d in ((0,) if b==0 else range(4))
            for p in (0,1,4)]


def bucket_for(i,p):
    b=p['buckets'];d=p['distribution']
    return (0,b-1,i%b,(i*7+1)%b)[d]


def initialize(t,p):
    if set(p)!=set(('buckets','nodes','distribution','pool')) or p not in parameters():
        raise ValueError('clear fixture domain')
    for lo,size in REGIONS:
        for a in range(lo,lo+size,4):t.save(a,0,4)
    t.save(TABLE,0xA5310201,4)
    t.save(TABLE+4,BUCKETS,4);t.save(TABLE+8,BUCKETS+4*p['buckets'],4)
    t.save(TABLE+12,BUCKETS+64,4);t.save(TABLE+16,p['nodes'],4)
    for i in range(12):
        t.save(NODES+12*i+4,0x80000000+i*0x010101,4)
        t.save(NODES+12*i+8,BUCKETS+4*(i%16),4)
    for i in reversed(range(p['nodes'])):
        b=BUCKETS+4*bucket_for(i,p)
        t.save(NODES+12*i,t.load(b,4),4);t.save(b,NODES+12*i,4)
    for i in range(4):
        t.save(FREE+12*i,FREE+12*(i+1) if i+1<p['pool'] else 0,4)
        t.save(FREE+12*i+4,0xE5010000+i,4);t.save(FREE+12*i+8,BUCKETS+4*i,4)
    t.save(HEADS+4,FREE if p['pool'] else 0,4)
    t.r[4]=TABLE;t.r[31]=RETURN
    t.initializing=False


def fixture(original,p):
    t=ClearTrace(original);initialize(t,p);t.run()
    output=[Trace.load(t,a,4) for lo,n in REGIONS for a in range(lo,lo+n,4)]
    return dict(parameters=p,expected=output,instructions=t.instruction_count,
                visited=sorted(t.visited),branches=t.branches,accesses=t.accesses)


def golden_header(rows):
    count=sum(n//4 for _,n in REGIONS)
    lines=['/* Authored initialized fixtures; no original code or data arrays. */',
        'struct ClearGolden { u32 buckets,nodes,distribution,pool; u32 expected[%d]; };'%count,
        'static const struct ClearGolden clear_golden[] = {']
    for row in rows:
        p=row['parameters'];lines.append('{%du,%du,%du,%du,{%s}},'%(p['buckets'],p['nodes'],p['distribution'],p['pool'],','.join('0x%08Xu'%v for v in row['expected'])))
    return '\n'.join(lines+['};',''])


def main():
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--elf',type=Path,default=ROOT/'orig/SLUS_216.68')
    a.add_argument('--output',type=Path,default=ROOT/'build/hashtable_clear/trace.json')
    a.add_argument('--golden-header',type=Path)
    opts=a.parse_args();_,original=validated_elf(opts.elf)
    inputs=parameters();rows=[fixture(original,p) for p in inputs]
    visited=sorted({pc for row in rows for pc in row['visited']})
    assert visited==list(range(ENTRY,STOP,4))
    report=dict(schema_version=1,fixtures=len(rows),input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),
        total_instructions=sum(r['instructions'] for r in rows),maximum_instructions=max(r['instructions'] for r in rows),
        visited=visited,observations=rows,
        qualification='Initialized disjoint typed-prefix storage, valid finite bucket/node/free-list graphs. Lower32 pointer/store observations and reviewed integer/control instructions only; no C++ lifetime, upper128/FCR/exception/timing/concurrency or allocator/data award.')
    opts.output.parent.mkdir(parents=True,exist_ok=True);opts.output.write_text(json.dumps(report,indent=2)+'\n',newline='\n')
    if opts.golden_header:
        opts.golden_header.parent.mkdir(parents=True,exist_ok=True);opts.golden_header.write_text(golden_header(rows),newline='\n')
    print('Clear fixtures',len(rows),'original instructions',report['total_instructions'],'max',report['maximum_instructions'])


if __name__=='__main__':main()
