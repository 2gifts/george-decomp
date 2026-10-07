"""Bounded complete original matrix kernels and exact dyadic fixtures.

Only the six reviewed PEXTUW/SQC2 pairs extend unchanged published decoders.
Numerical comparisons cover initialized positive k/16 values, 0<=k<=64 only.
No general EE/VU ACC/FCR/flags, class/prototype or hardware equivalence claim.
"""
from pathlib import Path
import argparse,hashlib,json,random,struct,sys
from analyze import validated_elf
from trace_record_collision import RecordTrace,MASK128
from trace_resource_base import ResourceBaseTrace
from trace_geometry import RETURN,word,is_control_transfer
from trace_segment_distance import normal_single
R=Path(__file__).resolve().parents[1]
H=lambda b:hashlib.sha256(b).hexdigest()
RANGES=((0x2A1DF0,0x2A1E78),(0x2A2200,0x2A2278))
ENTRIES=tuple(a for a,b in RANGES)
BUFFER,END=0x30000,0x30300
COUNT_LIMIT=64
NEW={0x2A1E44:0x70421CA8,0x2A1E48:0x70631CA8,
     0x2A2260:0xF88F0000,0x2A2264:0xF88E0010,
     0x2A2268:0xF88D0020,0x2A226C:0xF88C0030}

def word128(value):
    if type(value)is not int or not 0<=value<=MASK128:
        raise ValueError('matrix128-bit register outside bounded representation')
    return value

def upper_words(first,second):
    first,second=word128(first),word128(second)
    lanes=(second>>64&0xFFFFFFFF,first>>64&0xFFFFFFFF,
           second>>96&0xFFFFFFFF,first>>96&0xFFFFFFFF)
    return sum(v<<(32*i)for i,v in enumerate(lanes))

class MatrixTrace(RecordTrace):
    def __init__(self,original):
        super().__init__(original)
        self.vf=[[None]*4 for _ in range(32)]
        self.vf[0]=[0.0,0.0,0.0,1.0]
        self.vf_defined={0}
        self.visited=set()
        self.writes=[]

    def memory_check(self,address,size):
        if (size not in(4,16)or address%size or
            not BUFFER<=address<address+size<=END or
            any(address+i not in self.memory for i in range(size))):
            raise ValueError('matrix memory outside initialized aligned window')

    def save(self,address,value,size):
        ResourceBaseTrace.save(self,address,value,size)
        self.writes.append((address,size))

    def fetch(self,pc):
        if pc&3 or not any(a<=pc<b for a,b in RANGES):
            raise ValueError('unreviewed matrix instruction')
        offset=pc-0xFF000
        if not 0<=offset<=len(self.original)-4:
            raise ValueError('matrix instruction outside original')
        return int.from_bytes(self.original[offset:offset+4],'little')

    def _vf(self,index):
        if index not in self.vf_defined or len(self.vf[index])!=4 or any(v is None for v in self.vf[index]):
            raise ValueError('matrix uninitialized VF source')
        # Validate the complete source before publishing any result.
        return tuple(normal_single(v)for v in self.vf[index])

    def execute(self,instruction,pc):
        # All local checks precede new instruction mutation, including count
        # and the successful-execution visited set.
        if self.instruction_count>=COUNT_LIMIT:
            raise ValueError('matrix instruction bound exceeded')
        op=instruction>>26;rs=instruction>>21&31;rt=instruction>>16&31
        rd=instruction>>11&31;fn=instruction&63;shift=instruction>>6&31
        is_new_family=op==0x3E or op==0x1C and instruction&0x7FF==0x4A8
        if pc in NEW or is_new_family:
            if NEW.get(pc)!=instruction:
                raise ValueError('unapproved exact matrix PC/word pair')
            if op==0x1C:
                # Both sources are captured before the equal-source/target
                # original writes. No high lane comes from scalar sign fill.
                first,second=word128(self.r[rs]),word128(self.r[rt])
                value=upper_words(first,second)
                self.r[rd]=value
            else:
                lanes=self._vf(rt)
                bits=sum(word(v)<<(32*i)for i,v in enumerate(lanes))
                base=word128(self.r[rs])
                immediate=instruction&65535
                if immediate&32768:immediate-=65536
                address=(base+immediate)&0xFFFFFFFF
                self.memory_check(address,16)
                self.save(address,bits,16)
            self.instruction_count+=1
            self.visited.add(pc)
            self.r[0]=0
            self.vf[0]=[0.0,0.0,0.0,1.0]
            return None,False
        # Initial VF values are never silently assumed. Only the unchanged
        # decoder's successful producers establish defined local VF sources.
        written_vf=None
        if op==0x36:written_vf=rt
        elif op==18:
            if rs==1:self._vf(rd)
            elif rs==5:
                word128(self.r[rt]);written_vf=rd
            else:
                self._vf(rd);self._vf(rt)
                if fn!=0x3C and self.vu_acc is None:
                    raise ValueError('matrix accumulator read before producer')
                if fn in(0x0A,0x0B):written_vf=shift
        before=self.instruction_count
        result=RecordTrace.execute(self,instruction,pc)
        if self.instruction_count!=before+1:
            raise ValueError('matrix inherited instruction counted incorrectly')
        if written_vf is not None:self.vf_defined.add(written_vf)
        self.visited.add(pc)
        return result

    def library_call(self,target):
        raise ValueError('selected matrix kernels have no helper calls')

    def run(self,entry,stop=RETURN):
        body=next((r for r in RANGES if r[0]==entry),None)
        if body is None:raise ValueError('matrix requires complete approved entry')
        pc=entry
        while True:
            if not body[0]<=pc<body[1]:
                raise ValueError('matrix transfer outside complete body')
            instruction=self.fetch(pc)
            target,annul=self.execute(instruction,pc)
            if is_control_transfer(instruction):
                if not annul:
                    if pc+4>=body[1]:raise ValueError('matrix delay outside complete body')
                    delay=self.fetch(pc+4)
                    if is_control_transfer(delay)or self.execute(delay,pc+4)!=(None,False):
                        raise ValueError('matrix control transfer in delay')
                if instruction==0x03E00008 and target==stop:return
                raise ValueError('matrix requires actual JR31 return only')
            pc+=4

WORDS=(END-BUFFER)//4

def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def scalar(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def rational(words):
    values=[scalar(w)*16 for w in words]
    assert all(v==int(v)and 0<=v<=64 for v in values)
    return [int(v)for v in values]

def integer_oracle(routine,initial,a,b,c):
    # All input words are captured from the original arena before any result
    # publication. Numerators are integers <=16384, denominator256.
    k=rational(initial);expected=initial[:]
    if routine==0:
        numerator=[sum(k[a+4*j+i]*k[b+j]for j in range(4))for i in range(4)]
        for i,v in enumerate(numerator):expected[c+i]=bits(v/256)
    else:
        numerator=[sum(k[a+4*row+j]*k[b+4*j+col]for j in range(4))for row in range(4)for col in range(4)]
        for i,v in enumerate(numerator):expected[c+i]=bits(v/256)
    assert all(0<=v<=16384 for v in numerator)
    return expected,numerator

def initial_values(pattern,seed):
    rng=random.Random(seed)
    if pattern==0:return [0]*WORDS
    if pattern==1:return [64]*WORDS
    if pattern==2:return [(i%17)*4 for i in range(WORDS)]
    if pattern==3:return [16 if i%5==0 else 0 for i in range(WORDS)]
    if pattern==4:return [1 if i%3==0 else 63 for i in range(WORDS)]
    return [rng.randrange(65)for _ in range(WORDS)]

def plans():
    out=[]
    # Vector matrix/point/output disjoint, complete and shifted overlaps;
    # inputs may themselves address matrix lanes. Original matrix LQC2 is
    # aligned16, but point/output are individual4-byte lanes.
    for pattern in range(7):
        for inp in(16,17,20,28,32,64,96):
            destinations=sorted(set([128,*range(12,36),*range(inp-3,inp+4)]))
            for dst in destinations:out.append((0,16,inp,dst,pattern,100+pattern))
    # Product uses actual aligned16 quad inputs/output. Both source arrays
    # may overlap, and the64B destination may be shifted across either.
    for pattern in range(7):
        for right in(16,20,24,28,32,40,64,96):
            for dst in(4,8,12,16,20,24,28,32,36,40,44,48,64,80,96,100,104,108,112,128):
                out.append((1,16,right,dst,pattern,200+pattern))
    # Independent lane-isolation and full scalar endpoints expose arbitrary
    # homogeneous W and every matrix entry/orientation without cancellation.
    for routine in(0,1):
        for lane in range(16 if routine else 4):
            for value in(0,1,15,16,32,63,64):
                out.append((routine,16,64,128,8,1000+lane*100+value))
    return out

def observe(original):
    cases=[];seen=set();visited=set();total=0
    for routine,a,b,c,pattern,seed in plans():
        k=initial_values(pattern,seed)
        if pattern==8:
            lane=(seed-1000)//100;value=(seed-1000)%100
            k=[0]*WORDS
            if routine==0:
                for row in range(4):
                    for col in range(4):k[a+4*row+col]=(row+1)*(col+1)
                k[b+lane]=value
            else:
                k[a+lane]=value
                for row in range(4):
                    for col in range(4):k[b+4*row+col]=(row+1)*(col+1)
        initial=[bits(v/16)for v in k]
        key=(routine,a,b,c,tuple(initial))
        if key in seen:continue
        seen.add(key)
        expected,numerator=integer_oracle(routine,initial,a,b,c)
        t=MatrixTrace(original)
        arena=struct.pack('<%dI'%WORDS,*initial)
        for i,byte in enumerate(arena):t.memory[BUFFER+i]=byte
        t.writes=[];t.r[31]=RETURN
        if routine==0:t.r[4:7]=[BUFFER+4*a,BUFFER+4*b,BUFFER+4*c]
        else:t.r[4:7]=[BUFFER+4*c,BUFFER+4*a,BUFFER+4*b]
        t.run(ENTRIES[routine])
        actual=[t.load(BUFFER+4*i,4)for i in range(WORDS)]
        assert actual==expected,(len(cases),routine,a,b,c)
        write_order=([(BUFFER+4*(c+i),4)for i in(3,0,1,2)]if routine==0 else[(BUFFER+4*c+16*i,16)for i in range(4)])
        assert t.writes==write_order
        assert t.visited==set(range(*RANGES[routine],4))
        assert t.instruction_count==(34 if routine==0 else 30)
        visited.update(t.visited);total+=t.instruction_count
        cases.append(dict(routine=routine,source_a_word=a,source_b_word=b,output_word=c,
            initial=initial,expected=actual,oracle_numerators=numerator,oracle_denominator=256,
            instructions=t.instruction_count,writes=t.writes,
            incidental_GPR2_128='0x%032X'%t.r[2],
            incidental_COP1_0_3=[bits(f)for f in t.f[:4]],
            incidental_ACC=None if t.vu_acc is None else[bits(f)for f in t.vu_acc]))
    return cases,visited,total

def golden(cases):
    lines=['/* Generated ONLY from the complete reviewed original; integer oracle independently agrees. */',
        '#ifndef MATRIX_KERNELS_GOLDEN_H','#define MATRIX_KERNELS_GOLDEN_H',
        'struct MatrixGolden { unsigned routine, a, b, out; unsigned initial[%d], expected[%d]; };'%(WORDS,WORDS),
        'static const struct MatrixGolden matrix_golden[] = {']
    for q in cases:
        arr=lambda a:'{'+','.join('0x%08Xu'%v for v in a)+'}'
        lines.append('{%d,%d,%d,%d,%s,%s},'%(q['routine'],q['source_a_word'],q['source_b_word'],q['output_word'],arr(q['initial']),arr(q['expected'])))
    lines+=['};','#endif',''];return '\n'.join(lines).encode()

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=R/'build/matrix_kernels_public/trace.json')
    parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args()
    _,original=validated_elf(R/'orig/SLUS_216.68')
    cases,visited,total=observe(original);header=golden(cases)
    result=dict(passed=True,fixtures=len(cases),unique_parameters_and_initial_arena=len(cases),
        original_instructions=total,max_instructions=max(q['instructions']for q in cases),
        selected_instruction_coverage=len(visited),selected_instructions=64,
        fixture_SHA=H(json.dumps(cases,separators=(',',':')).encode()),golden_RAW_SHA=H(header),
        source_scope_freeze_RAW_SHA='4e38b01648c3225b467997d371aec0960201bb5f8d5252512f5759db6d7b3160',
        original_RAW_SHA=H(original),cases=cases,
        limits='Only initialized typed float storage, actual required4/16 alignment and exact positive k/16 with0<=k<=64 inputs. Products and four-term sums are integer/256 and exactly binary32. Integer rational oracle checks every original arena word and complete original store order. Incidental register observations are recorded, never native return equivalence or original prototype claims. No general EE/VU ACC/FCR/flags/exceptional values/timing/hardware equivalence.')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    if args.golden_header:
        args.golden_header.parent.mkdir(parents=True,exist_ok=True)
        args.golden_header.write_bytes(header)
    print('PASS %d unique fixtures / %d original instructions / all64 original PCs'%(len(cases),total))
if __name__=='__main__':main()
