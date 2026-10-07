"""Complete original matrix-basis effects under a nominal binary32 model.

Only fifty exact reviewed PC/word pairs extend unchanged RecordTrace. The
initialized integer lattice is a nominal representability oracle, never a
manufacturer-guaranteed EE/VU arithmetic, flag, exception or timing proof.
"""
from pathlib import Path
import argparse,hashlib,json,random,struct
from analyze import validated_elf
from trace_record_collision import RecordTrace,MASK128
from trace_matrix_kernels import MatrixTrace,word128
from trace_geometry import RETURN,word,scalar,rounded,is_control_transfer
from trace_segment_distance import normal_single

R=Path(__file__).resolve().parents[1]
RANGES=((0x2A1E78,0x2A1F14),(0x2A1F18,0x2A1FD4))
ENTRIES=tuple(a for a,b in RANGES)
BUFFER,END=0x30000,0x30200
WORDS=(END-BUFFER)//4
LIMIT=47
H=lambda b:hashlib.sha256(b).hexdigest()
# These are exact approved words and fields, not an opcode-family whitelist.
SPECS=(
 (0x4BC00283,'add',14,10,0,0,3),(0x4BE10968,'add',15,5,1,1,None),
 (0x4A200318,'mul',1,12,0,0,0),(0x4A200358,'mul',1,13,0,0,0),
 (0x4A200398,'mul',1,14,0,0,0),(0x4BC50A5B,'mul',14,9,1,5,3),
 (0x4BC50998,'mul',14,6,1,5,0),(0x4BC509D9,'mul',14,7,1,5,1),
 (0x4BC50A1A,'mul',14,8,1,5,2),(0x4A893306,'sub',4,12,6,9,2),
 (0x4B093B42,'add',8,13,7,9,2),(0x4B094385,'sub',8,14,8,9,1),
 (0x4B075305,'sub',8,12,10,7,1),(0x4A865344,'sub',4,13,10,6,0),
 (0x4A465384,'sub',2,14,10,6,0),(0x4A493301,'add',2,12,6,9,1),
 (0x4A493B44,'sub',2,13,7,9,0),(0x4A894380,'add',4,14,8,9,0),
 (0x4B086306,'sub',8,12,12,8,2),(0x4A886B46,'sub',4,13,13,8,2),
 (0x4A477385,'sub',2,14,14,7,1),
 (0xF8800030,'store',15,0,0,0,None),(0xF88C0000,'store',15,12,0,0,None),
 (0xF88D0010,'store',15,13,0,0,None),(0xF88E0020,'store',15,14,0,0,None))
NEW={entry+48+4*i:s for entry in ENTRIES for i,s in enumerate(SPECS)}
assert len(NEW)==50

def nominal(value):
 value=normal_single(value)
 numerator=value*128
 if numerator!=int(numerator)or abs(numerator)>8192:
  raise ValueError('basis value outside bounded nominal integer lattice')
 return value

def coefficient_integers(lanes):
 out=[]
 for v in lanes:
  v=nominal(v);n=v*16
  if n!=int(n)or abs(n)>64:raise ValueError('basis coefficient outside captured domain')
  out.append(int(n))
 nonnegative=all(0<=v<=64 for v in out)and all(v!=0 or word(lanes[i])==0 for i,v in enumerate(out))
 signed=all(-64<=v<=64 and v!=0 for v in out)
 if not(nonnegative or signed):raise ValueError('basis coefficient signs outside approved subsets')
 return out

class BasisTrace(RecordTrace):
 def __init__(self,original):
  super().__init__(original)
  self.vf=[[None]*4 for _ in range(32)];self.vf[0]=[0.0,0.0,0.0,1.0]
  self.defined=[[False]*4 for _ in range(32)];self.defined[0]=[True]*4
  self.vf_defined={0};self.visited=set();self.writes=[]
 def memory_check(self,address,size):
  if(type(address)is not int or size not in(4,16)or address%size or
     not BUFFER<=address<address+size<=END or any(address+i not in self.memory for i in range(size))):
   raise ValueError('basis memory outside initialized aligned arena')
 def save(self,address,value,size):
  super().save(address,value,size);self.writes.append((address,size,value&((1<<(8*size))-1)))
 def fetch(self,pc):
  if type(pc)is not int or pc&3 or not any(a<=pc<b for a,b in RANGES):
   raise ValueError('unreviewed basis PC')
  off=pc-0xFF000
  if not 0<=off<=len(self.original)-4:raise ValueError('basis PC outside original')
  return struct.unpack_from('<I',self.original,off)[0]
 def lane(self,index,lane):
  if not self.defined[index][lane] or self.vf[index][lane]is None:
   raise ValueError('basis demanded lane is unknown')
  return nominal(self.vf[index][lane])
 def _vf(self,index):
  # Entire complete-register validation is the published MatrixTrace method.
  if not all(self.defined[index]):raise ValueError('basis whole quadstore includes unknown lane')
  return tuple(nominal(v)for v in MatrixTrace._vf(self,index))
 def execute(self,instruction,pc):
  if self.instruction_count>=LIMIT:raise ValueError('basis invocation bound exceeded')
  if type(instruction)is not int or not 0<=instruction<=0xFFFFFFFF or self.fetch(pc)!=instruction:
   raise ValueError('basis word differs from owned original')
  op=instruction>>26;rs=instruction>>21&31;rt=instruction>>16&31;rd=instruction>>11&31
  if pc in NEW:
   spec=NEW[pc]
   if spec[0]!=instruction:raise ValueError('unapproved basis PC/word pair')
   _,family,mask,fd,fs,ft,bc=spec
   if family=='store':
    lanes=self._vf(fd);bits=sum(word(v)<<(32*i)for i,v in enumerate(lanes))
    base=word128(self.r[rs]);imm=instruction&65535;imm-=65536 if imm&32768 else 0
    address=(base+imm)&0xFFFFFFFF;self.memory_check(address,16)
    self.save(address,bits,16)
   else:
    staged=[]
    for i in range(4):
     if not mask&(8>>i):continue
     a=self.lane(fs,i);b=self.lane(ft,i if bc is None else bc)
     result=rounded(a+b if family=='add'else a-b if family=='sub'else a*b)
     staged.append((i,nominal(result)))
    for i,value in staged:self.vf[fd][i]=value;self.defined[fd][i]=True
    if all(self.defined[fd]):self.vf_defined.add(fd)
   self.instruction_count+=1;self.visited.add(pc);self.r[0]=0
   self.vf[0]=[0.0,0.0,0.0,1.0];self.defined[0]=[True]*4
   return None,False
  # Only authentic scalar/pack/transfer operations elsewhere in both bodies.
  # All possible rejection checks precede the inherited mutation/counter.
  if op in(0x31,0x39):
   base=word128(self.r[rs]);imm=instruction&65535;imm-=65536 if imm&32768 else 0
   address=(base+imm)&0xFFFFFFFF;self.memory_check(address,4)
   if op==0x31:nominal(scalar(self.load(address,4)))
   else:nominal(self.f[rt])
  elif op==17 and rs==0:
   if instruction&0x7FF:raise ValueError('reserved basis MFC1')
   nominal(self.f[rd])
  elif op==17 and rs==4:
   if instruction&0x7FF:raise ValueError('reserved basis MTC1')
   word128(self.r[rt]);nominal(scalar(self.r[rt]&0xFFFFFFFF))
  elif op==0x1C and instruction&0x7FF==0x488:
   word128(self.r[rs]);word128(self.r[rt])
  elif op==18 and rs==5 and instruction&0x7FF==0 and rd==1:
   packed=word128(self.r[rt]);lanes=[nominal(scalar(packed>>(32*i)&0xFFFFFFFF))for i in range(4)]
   coefficient_integers(lanes)
  elif op==15 and rs==0:pass
  elif instruction==0:pass
  elif instruction==0x03E00008:word128(self.r[31])
  else:raise ValueError('unsupported basis inherited instruction')
  before=self.instruction_count;result=RecordTrace.execute(self,instruction,pc)
  assert self.instruction_count==before+1
  if op==18:self.defined[rd]=[True]*4;self.vf_defined.add(rd)
  self.visited.add(pc)
  return result
 def library_call(self,target):raise ValueError('basis selected routines have no calls')
 def run(self,entry,stop=RETURN):
  body=next((q for q in RANGES if q[0]==entry),None)
  if body is None:raise ValueError('basis invocation requires complete entry')
  pc=entry
  while True:
   if not body[0]<=pc<body[1]:raise ValueError('basis transfer outside selected body')
   w=self.fetch(pc);target,annul=self.execute(w,pc)
   if is_control_transfer(w):
    if annul or pc+4>=body[1]:raise ValueError('basis unsupported delay')
    delay=self.fetch(pc+4)
    if is_control_transfer(delay)or self.execute(delay,pc+4)!=(None,False):
     raise ValueError('basis control in delay')
    if w!=0x03E00008 or target!=stop:raise ValueError('basis requires actual JR31 to stop')
    return
   pc+=4

def integer_oracle(initial,out,coeff,position,routine):
 a,b,c,d=coefficient_integers([scalar(v)for v in initial[coeff:coeff+4]])
 rows=[128-b*b-c*c,a*b-c*d,a*c+b*d,0,
       a*b+c*d,128-a*a-c*c,b*c-a*d,0,
       a*c-b*d,b*c+a*d,128-a*a-b*b,0]
 expected=initial[:];events=[]
 # Direct integer polynomial, independently applied whole-memory effects.
 for offset,values in((12,[0,0,0,word(1.0)]),(0,[word(v/128)for v in rows[:4]]),
                      (4,[word(v/128)for v in rows[4:8]]),(8,[word(v/128)for v in rows[8:]])):
  expected[out+offset:out+offset+4]=values
  events.append((BUFFER+4*(out+offset),16,sum(v<<(32*i)for i,v in enumerate(values))))
 if routine:
  for dest,source in((out+12,position),(out+13,position+1)):
   expected[dest]=expected[source];events.append((BUFFER+4*dest,4,expected[dest]))
  z=expected[position+2]
  expected[out+15]=word(1.0);events.append((BUFFER+4*(out+15),4,word(1.0)))
  expected[out+14]=z;events.append((BUFFER+4*(out+14),4,z))
 return expected,rows,events

def plans():
 coeffs=[(0,0,0,0),(1,2,3,4),(16,0,0,0),(0,16,0,0),(0,0,16,0),(0,0,0,16),
         (64,64,64,64),(1,63,2,64),(64,1,63,2),(-1,2,3,-4),(-64,64,-64,64)]
 for i in range(4):
  for value in(1,15,16,32,63,64):
   q=[0]*4;q[i]=value;coeffs.append(tuple(q))
 positions=(0,16,20,24,25,26,27,28,96)
 inputs=(0,14,16,17,20,24,28,32,36,64)
 for index,q in enumerate(coeffs):
  for coeff in inputs:
   yield(0,16,coeff,96,q,index)
   for pos in positions:yield(1,16,coeff,pos,q,index)
 rng=random.Random(0xB4515)
 for i in range(1000):
  signed=i&1
  q=tuple(rng.choice([v for v in range(-64,65)if v])if signed else rng.randrange(65)for _ in range(4))
  yield(i&1,16,rng.choice(inputs),rng.choice(positions),q,10000+i)

def observe(original):
 cases=[];seen=set();visited=set();total=0
 for routine,out,coeff,position,q,seed in plans():
  rng=random.Random(seed);initial=[word(rng.randrange(-8192,8193)/128)for _ in range(WORDS)]
  initial[position:position+3]=[0x80000000,word(17/128),word(-23/128)]
  initial[coeff:coeff+4]=[word(v/16)for v in q]
  key=(routine,out,coeff,position,tuple(initial))
  if key in seen:continue
  seen.add(key);expected,numerators,events=integer_oracle(initial,out,coeff,position,routine)
  t=BasisTrace(original);arena=struct.pack('<%dI'%WORDS,*initial)
  t.memory.update({BUFFER+i:b for i,b in enumerate(arena)});t.r[4:7]=[BUFFER+4*out,BUFFER+4*coeff,BUFFER+4*position];t.r[31]=RETURN
  t.run(ENTRIES[routine]);actual=[t.load(BUFFER+4*i,4)for i in range(WORDS)]
  assert actual==expected and t.writes==events,(len(cases),routine,coeff,position)
  assert t.visited==set(range(*RANGES[routine],4))and t.instruction_count==(39 if routine==0 else 47)
  visited.update(t.visited);total+=t.instruction_count
  cases.append(dict(routine=routine,out=out,coeff=coeff,position=position,initial=initial,expected=actual,
                    independent_integer_numerators=numerators,denominator=128,writes=t.writes,instructions=t.instruction_count))
 return cases,visited,total

def golden(cases):
 lines=['/* Complete original nominal observer; independent integer whole-arena oracle agrees. */',
 '#ifndef MATRIX_BASIS_GOLDEN_H','#define MATRIX_BASIS_GOLDEN_H',
 'struct BasisGolden { unsigned routine,out,coeff,position; unsigned initial[%d],expected[%d]; };'%(WORDS,WORDS),
 'static const struct BasisGolden basis_golden[] = {']
 for q in cases:
  arr=lambda v:'{'+','.join('0x%08Xu'%w for w in v)+'}'
  lines.append('{%d,%d,%d,%d,%s,%s},'%(q['routine'],q['out'],q['coeff'],q['position'],arr(q['initial']),arr(q['expected'])))
 return ('\n'.join(lines+['};','#endif',''])).encode()

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=R/'build/matrix_basis/trace.json')
 p.add_argument('--golden-header',type=Path);a=p.parse_args();_,raw=validated_elf(R/'orig/SLUS_216.68')
 cases,visited,total=observe(raw);header=golden(cases)
 report=dict(passed=True,fixtures=len(cases),unique_parameters=len(cases),original_instructions=total,max_instructions=max(q['instructions']for q in cases),
 visited_original_PCs=['0x%08X'%v for v in sorted(visited)],all86_selected_PCs_executed=len(visited)==86,
 nominal_binary32_only=True,no_hardware_arithmetic_equivalence=True,independent_integer_denominator=128,
 cases=cases,golden_LF_bytes=len(header),golden_LF_sha256=H(header),
 limitations='Initialized captured positive0..64/16 or signed nonzero-64..64/16 coefficients, finite +/-zero or bounded p/128 translation; nominal model only, no EE/VU flags/rounding/error/timing/concurrency/unknown class identity.')
 a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_bytes((json.dumps(report,indent=2)+'\n').encode())
 if a.golden_header:a.golden_header.parent.mkdir(parents=True,exist_ok=True);a.golden_header.write_bytes(header)
 print(json.dumps({k:v for k,v in report.items()if k not in('cases','visited_original_PCs')}))
if __name__=='__main__':main()
