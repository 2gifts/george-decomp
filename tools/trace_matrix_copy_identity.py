"""Exact raw-copy and VF00 constant-selection observations on initialized arenas.

No general VU arithmetic, address exceptions, atomicity, flags or timing model.
Identity source reuse compares final memory, not SDK versus retail store order.
"""
from pathlib import Path
import argparse,hashlib,json,random,struct
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_matrix_kernels import word128
R=Path(__file__).resolve().parents[1]
RANGES=((0x2A1C08,0x2A1C30),(0x2A1C30,0x2A1C60))
BUFFER,END=0x30000,0x30100
LIMIT=12
MASKS={0x2A1C30:(0x4B0000D3,3,8,True),0x2A1C34:(0x4A800093,2,4,True),0x2A1C38:(0x4A400053,1,2,True),0x2A1C40:(0x4AE000D4,3,7,False),0x2A1C44:(0x4B600094,2,11,False),0x2A1C48:(0x4BA00054,1,13,False)}
H=lambda b:hashlib.sha256(b).hexdigest()

class CopyIdentityTrace(Trace):
    def __init__(self,original):
        super().__init__(original)
        self.vf=[[None]*4 for _ in range(32)]
        self.vf[0]=[0,0,0,0x3F800000]
        self.defined=[[False]*4 for _ in range(32)];self.defined[0]=[True]*4
        self.gpr_defined={0,4,5,31};self.writes=[];self.visited=set()
    def memory_check(self,address,size):
        if(type(address)is not int or size!=16 or address%16 or
           not BUFFER<=address<address+size<=END or
           any(address+i not in self.memory for i in range(size))):
            raise ValueError('matrix copy/identity outside initialized aligned16 arena')
    def load(self,address,size):
        self.memory_check(address,size);return Trace.load(self,address,size)
    def save(self,address,value,size):
        self.memory_check(address,size);Trace.save(self,address,value,size)
        self.writes.append((address,size,value))
    def fetch(self,pc):
        if type(pc)is not int or pc&3 or not any(a<=pc<b for a,b in RANGES):
            raise ValueError('unreviewed matrix copy/identity PC')
        offset=pc-0xFF000
        if not 0<=offset<=len(self.original)-4:raise ValueError('matrix PC outside original')
        return struct.unpack_from('<I',self.original,offset)[0]
    def execute(self,instruction,pc):
        # Every rejecting check precedes register/memory/count/visited changes.
        if self.instruction_count>=LIMIT:raise ValueError('matrix copy/identity budget')
        if type(instruction)is not int or not 0<=instruction<=0xFFFFFFFF or self.fetch(pc)!=instruction:
            raise ValueError('matrix copy/identity word differs from owned PC')
        op=instruction>>26;rs=instruction>>21&31;rt=instruction>>16&31
        staged=None;store=None;register=None
        if pc in MASKS:
            word,fd,mask,maximum=MASKS[pc]
            if instruction!=word:raise ValueError('unapproved constant-mask encoding')
            if self.vf[0]!=[0,0,0,0x3F800000]or not all(self.defined[0]):
                raise ValueError('VF00 constant source changed or unknown')
            # Primary greater/smaller selection has only literal 0 and1 operands.
            source=self.vf[0][3 if maximum else 0]
            staged=(fd,[(i,source if maximum else 0)for i in range(4)if mask&(8>>i)])
        elif pc in(0x2A1C3C,0x2A1C4C,0x2A1C50,0x2A1C54):
            expected={0x2A1C3C:(0,48),0x2A1C4C:(3,0),0x2A1C50:(2,16),0x2A1C54:(1,32)}[pc]
            if op!=0x3E or rs!=4 or rt!=expected[0]or instruction&65535!=expected[1]:
                raise ValueError('unapproved matrix quad-store pair')
            if not all(self.defined[rt])or any(v is None for v in self.vf[rt]):
                raise ValueError('unknown matrix VF lane in quad-store')
            if rs not in self.gpr_defined:raise ValueError('unknown matrix address register')
            address=(word128(self.r[rs])+expected[1])&0xFFFFFFFF;self.memory_check(address,16)
            value=sum(v<<(32*i)for i,v in enumerate(self.vf[rt]));word128(value);store=(address,value)
        elif 0x2A1C08<=pc<0x2A1C28:
            offset=(pc-0x2A1C08)//4
            load_specs=((7,48),(2,0),(3,16),(6,32))
            store_specs=((2,0),(3,16),(6,32),(7,48))
            wanted=load_specs[offset]if offset<4 else store_specs[offset-4]
            base=5 if offset<4 else 4;wanted_op=0x1E if offset<4 else 0x1F
            if op!=wanted_op or rs!=base or rt!=wanted[0]or instruction&65535!=wanted[1]:
                raise ValueError('unapproved raw128 copy pair')
            if rs not in self.gpr_defined:raise ValueError('unknown matrix address register')
            address=(word128(self.r[rs])+wanted[1])&0xFFFFFFFF;self.memory_check(address,16)
            if offset<4:register=(rt,self.load(address,16))
            else:
                if rt not in self.gpr_defined:raise ValueError('unknown raw128 source register')
                word128(self.r[rt])
        elif pc in(0x2A1C28,0x2A1C58):
            if instruction!=0x03E00008 or 31 not in self.gpr_defined:raise ValueError('unapproved matrix return')
            word128(self.r[31])
        elif pc in(0x2A1C2C,0x2A1C5C):
            if instruction!=0:raise ValueError('unapproved matrix delay')
        else:raise ValueError('unsupported matrix copy/identity instruction')
        if staged is not None:
            fd,values=staged
            for i,value in values:self.vf[fd][i]=value;self.defined[fd][i]=True
        if store is not None:self.save(store[0],store[1],16)
        if pc in MASKS or store is not None:
            self.instruction_count+=1;result=(None,False)
        else:
            result=Trace.execute(self,instruction,pc)
            if register is not None:self.gpr_defined.add(register[0])
        self.visited.add(pc);self.r[0]=0
        return result
    def run(self,entry):
        body=next((p for p in RANGES if p[0]==entry),None)
        if body is None:raise ValueError('matrix requires whole approved entry')
        pc=entry
        while body[0]<=pc<body[1]:
            w=self.fetch(pc);target,annul=self.execute(w,pc)
            if is_control_transfer(w):
                delay=self.fetch(pc+4)
                if annul or is_control_transfer(delay):raise ValueError('matrix control transfer in delay')
                if self.execute(delay,pc+4)!=(None,False):raise ValueError('matrix nested delay')
                if w==0x03E00008 and target==RETURN:return
                raise ValueError('matrix requires actual JR31 return')
            pc+=4
        raise ValueError('matrix body fell through')

def initial(pattern,seed):
    rng=random.Random(seed)
    special=[0,0x80000000,0x7F800000,0xFF800000,0x7FC12345,0x7F800001,0xFFFFFFFF,1,0x00800000,0x3F800000]
    if pattern==0:return bytes(range(256))
    if pattern==1:return b'\xA5'*256
    if pattern==2:return b''.join(struct.pack('<I',special[i%len(special)])for i in range(64))
    return bytes(rng.randrange(256)for _ in range(256))

def observe(original):
    cases=[];visited=set();total=0
    for routine in(0,1):
        for pattern in range(7):
            for source in([64,96,128]if routine==0 else[0]):
                for out in range(0,193,16):
                    before=initial(pattern,100+pattern);expected=bytearray(before)
                    if routine==0:expected[out:out+64]=before[source:source+64]
                    else:expected[out:out+64]=struct.pack('<16I',*[0x3F800000 if i%5==0 else 0 for i in range(16)])
                    t=CopyIdentityTrace(original);t.memory={BUFFER+i:b for i,b in enumerate(before)}
                    t.r[4],t.r[5],t.r[31]=BUFFER+out,BUFFER+source,RETURN;t.run(RANGES[routine][0])
                    actual=bytes(t.memory[BUFFER+i]for i in range(256));assert actual==bytes(expected)
                    assert [p[0]-BUFFER for p in t.writes]==[out+i for i in([0,16,32,48]if routine==0 else[48,0,16,32])]
                    assert t.visited==set(range(*RANGES[routine],4))
                    visited.update(t.visited);total+=t.instruction_count
                    cases.append(dict(routine=routine,source=source,output=out,pattern=pattern,seed=100+pattern,
                        initial=list(before),expected=list(actual),original_store_ledger=t.writes,instructions=t.instruction_count))
    return cases,visited,total

def golden(cases):
    lines=['/* Synthetic full arena outputs: actual original observer plus independent snapshot/constants. */',
        '#ifndef MATRIX_COPY_IDENTITY_GOLDEN_H','#define MATRIX_COPY_IDENTITY_GOLDEN_H',
        'struct MatrixCopyIdentityGolden { unsigned routine, source, output; unsigned char initial[256], expected[256]; };',
        'static const struct MatrixCopyIdentityGolden matrix_copy_identity_golden[] = {']
    for q in cases:
        arr=lambda values:'{'+','.join(str(v)for v in values)+'}'
        lines.append('{%d,%d,%d,%s,%s},'%(q['routine'],q['source'],q['output'],arr(q['initial']),arr(q['expected'])))
    return ('\n'.join(lines+['};','#endif',''])).encode()

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,default=R/'build/matrix_copy_identity/trace.json');parser.add_argument('--golden-header',type=Path)
    args=parser.parse_args();_,original=validated_elf(R/'orig/SLUS_216.68');cases,visited,total=observe(original);header=golden(cases)
    packet=dict(passed=True,fixtures=len(cases),original_instructions=total,max_instructions=12,selected_words=len(visited),cases=cases,original_sha256=H(original),golden_raw_sha256=H(header),fixture_sha256=H(json.dumps(cases,separators=(',',':')).encode()),limits='Raw128 aligned initialized complete64byte copy snapshots; identity literal VF0 constant selection only. SDK source equivalence final ordinary nonvolatile binary32 MATRIX contents; original ledger is not SDK publication order. No general arithmetic/unaligned floor/exceptions/atomicity/flags/timing/microconcurrency/incidental returns/class identity.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_bytes((json.dumps(packet,indent=2)+'\n').encode())
    if args.golden_header:args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_bytes(header)
    print('PASS',len(cases),'fixtures /',total,'original instructions /',len(visited),'selected words')
if __name__=='__main__':main()
