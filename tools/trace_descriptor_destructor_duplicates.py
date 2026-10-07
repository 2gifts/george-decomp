"""Actual24-PC observations using unchanged complete resolver decoder/factory.

Only lexical complete-entry scopes and fixture dispatch differ. No instruction
implementation, old source, memory contract or helper/global is altered. The
copied Python global dictionaries do not mutate the imported producer modules.
"""
from pathlib import Path
import argparse,hashlib,json,types
from analyze import validated_elf
import trace_completion_resolver as inherited

ROOT=Path(__file__).resolve().parents[1]
ADDRESSES=(0x1080D8,0x14FCB0,0x15E6D8,0x199A70,0x1AF9D0,0x1BBEB8,
           0x1C8B60,0x1D6CE8,0x1D86D0,0x1F5980,0x2177A8,0x230718,
           0x235430,0x23A468,0x23C5D8,0x244F80,0x2730E8,0x277438,
           0x27DAB0,0x27ED40,0x2BED70,0x2D0C38,0x2D26E8,0x2D7CC8)
SELECTED=tuple((a,a+128) for a in ADDRESSES)
RANGES=SELECTED+inherited.RANGES

def scoped_function(function,bindings):
    """The complete existing function code stays identical; bounds are local."""
    environment=dict(function.__globals__)
    environment.update(bindings)
    result=types.FunctionType(function.__code__,environment,function.__name__,function.__defaults__)
    result.__kwdefaults__=function.__kwdefaults__
    assert result.__code__ is function.__code__
    return result

class DescriptorTrace(inherited.ResolverTrace):
    def __init__(self,original,parameters):
        super().__init__(original,parameters)
        self.invocations=[0]*len(RANGES)

    fetch=scoped_function(inherited.ResolverTrace.fetch,dict(RANGES=RANGES))
    run=scoped_function(inherited.ResolverTrace.run,dict(RANGES=RANGES))

def fixture(original,entry=0,nodes=3,flags=3,mutation=0):
    if type(entry) is not int or not 0<=entry<len(SELECTED):
        raise ValueError('descriptor actual entry domain')
    selected=list(inherited.SELECTED)
    selected[7]=SELECTED[entry]
    factory=scoped_function(inherited.fixture,dict(ResolverTrace=DescriptorTrace,SELECTED=tuple(selected)))
    observation=factory(original,routine=7,nodes=nodes,flags=flags,mutation=mutation)
    observation['parameters']['entry']=entry
    assert observation['invocations'][entry]==1
    assert sum(observation['invocations'][:24])==1
    return observation

def parameters():
    return [dict(entry=entry,nodes=nodes,flags=flags,mutation=mutation)
            for entry in range(24) for nodes in range(4)
            for flags in (0,1,2,3,0x80000000,0x80000001,0xFFFFFFFF)
            for mutation in (0,2,4,6)]

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'build/descriptor_destructor_duplicates/trace.json')
    args=parser.parse_args();_,original=validated_elf(ROOT/'orig/SLUS_216.68')
    inputs=parameters();observations=[fixture(original,**p) for p in inputs]
    assert len(inputs)==len({tuple(sorted(p.items())) for p in inputs})==2688
    # Independent effect invariants, in addition to whole output observations.
    for p,row in zip(inputs,observations):
        globals_=dict(row['globals']);changes=dict(row['differences'])
        assert globals_[inherited.REG]==0 and globals_[inherited.SHUTDOWN]==1
        assert changes[inherited.DESCRIPTOR+4-inherited.BASE]==0x4200F8
        descriptor_events=[e for e in row['events'] if e[:2]==[2,inherited.DESCRIPTOR]]
        assert len(descriptor_events)==(p['flags']&1)
        if descriptor_events:assert row['events'][-1]==descriptor_events[0]
        assert all(not lo<=pc<hi or lo==SELECTED[p['entry']][0]
                   for pc in row['visited'] for lo,hi in SELECTED)
    visited=sorted({pc for row in observations for pc in row['visited']})
    assert all(pc in visited for a,b in SELECTED for pc in range(a,b,4))
    report=dict(schema_version=1,fixtures=len(observations),distinct_inputs=len(inputs),
        input_sha256=hashlib.sha256(json.dumps(inputs,sort_keys=True).encode()).hexdigest(),
        total_instructions=sum(x['instructions'] for x in observations),
        maximum_instructions=max(x['instructions'] for x in observations),
        visited=visited,observations=observations,
        qualification='Fresh actual24-PC initialized descriptor observations, genuine unchanged resolver opcode decoder/factory/control functions with local entry scopes only. Inherited real teardown/helper paths and controlledrelease mutations; no class/prototype/invocation/hardware/helper/data award. Old493 fixtures/native8094485 are not these counts.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print(report['fixtures'],report['total_instructions'],report['maximum_instructions'])

if __name__=='__main__':main()
