"""Initialized same-array SGI upper-bound observations at all 59 real entries.

One published integer decoder is inherited unchanged. Predicate effects are
explicit controlled ordinary callbacks; no original type/class identity,
invalid pointer subtraction or whole-program callback behavior is asserted.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from analyze import validated_elf
from trace_geometry import Trace,RETURN,is_control_transfer
from trace_string_registry import RegistryTrace,signed
ROOT=Path(__file__).resolve().parents[1]
ENTRIES=(1051696,1067632,1081400,1099848,1187432,1258400,1295880,1321416,
 1375248,1397928,1407872,1435192,1451784,1666024,1677776,1767728,1781336,
 1796904,1818136,1870528,1928264,1934896,2007872,2021824,2034320,2039936,
 2054368,2098480,2100464,2135504,2137488,2193160,2295416,2301728,2315152,
 2335688,2344248,2368080,2375024,2379360,2386800,2390144,2392272,2433936,
 2438296,2484264,2550648,2568264,2585496,2611728,2616480,2624080,2627232,
 2629752,2876512,2878672,2952088,2958920,2980904)
BASE,WORDS,STACK,CALLBACK=0x20000,96,(0x7F000,0x81000),0xF00100
U32=0xFFFFFFFF
initial=lambda i:(0xA5070201^(i*0x10203))&U32
class UpperTrace(RegistryTrace):
 def __init__(self,original,entry,parameters):
  if entry not in ENTRIES:raise ValueError('unreviewed upper-bound entry')
  Trace.__init__(self,original);self.entry=entry;self.p=parameters
  self.visited=set();self.events=[];self.branches=[];self.lo=self.hi=0
 def memory_check(self,a,n):
  if n not in (4,8,16) or a%n or not(BASE<=a<a+n<=BASE+WORDS*4 or STACK[0]<=a<a+n<=STACK[1]):
   raise ValueError('unowned or unaligned upper-bound memory')
 def load(self,a,n):
  self.memory_check(a,n)
  if not all(a+i in self.memory for i in range(n)):raise ValueError('uninitialized upper-bound memory')
  return Trace.load(self,a,n)
 def save(self,a,v,n):self.memory_check(a,n);Trace.save(self,a,v,n)
 def fetch(self,pc):
  if pc&3 or not self.entry<=pc<self.entry+156:raise ValueError('unreviewed upper-bound instruction')
  off=pc-0xFF000
  if off<0 or off+4>len(self.original):raise ValueError('upper-bound image bound')
  return struct.unpack_from('<I',self.original,off)[0]
 def execute(self,w,pc):
  op,rs,rt,rd,sh,fn=w>>26,w>>21&31,w>>16&31,w>>11&31,w>>6&31,w&63
  ok=w==0 or op in (9,30,31,35,55,63) or op==6 and rt==0 or op==4
  if op==0:
   ok=ok or fn in (0x21,0x23,0x2D) and sh==0 or fn in (0,3) and rs==0
   ok=ok or fn==8 and rs==31 and rt==rd==sh==0 or fn==9 and rs==20 and rt==sh==0 and rd==31
  if not ok:raise ValueError('unsupported upper-bound instruction operands')
  return RegistryTrace.execute(self,w,pc)
 def canonical(self,a):
  if BASE<=a<=BASE+WORDS*4 and not a&3:return (a-BASE)//4
  raise ValueError('upper-bound pointer outside initialized typed ledger')
 def predicate(self,target):
  if target!=CALLBACK:raise ValueError('unknown upper-bound callback')
  a,b=self.r[4]&U32,self.r[5]&U32
  if not(BASE+128<=a<BASE+196 and BASE+128<=b<BASE+196) or a&3 or b&3:
   raise ValueError('callback payload outside typed ledger')
  av,bv=signed(self.load(a,4)),signed(self.load(b,4));call=len(self.events)
  result=int(av<bv)
  if self.p['mode']==1:result=(-1 if result else 0)
  elif self.p['mode']==2:result=(-2147483648 if result else 0)
  elif self.p['mode']==3:result=1 if call%2 else 0
  if self.p['mutation']&1 and call==0:self.save(self.p['slot'],BASE+192,4)
  if self.p['mutation']&2 and call==0:self.save(BASE+192,17,4)
  if self.p['mutation']&4 and call==0:
   # Original candidate is captured by its JALR delay before this write.
   middle=self.r[17]&U32
   if not BASE<=middle<BASE+64 or middle&3:raise ValueError('callback middle ledger')
   self.save(middle,BASE+192,4)
  if self.p['mutation']&8:self.save(b,(bv+7)&U32,4)
  self.events.append([self.canonical(a),self.canonical(b),av&U32,bv&U32,result&U32,
    self.canonical(self.load(self.p['slot'],4))])
  # Caller-saved integer lanes are intentionally clobbered; saved lanes remain.
  for i in tuple(range(2,16))+(24,25):self.r[i]=0xBAD00000+i
  self.r[2]=result&U32
 def run(self):
  pc=self.entry
  while True:
   w=self.fetch(pc);target,annul=self.execute(w,pc)
   if is_control_transfer(w):
    if annul:raise ValueError('unreviewed upper-bound likely branch')
    delay=self.fetch(pc+4)
    if is_control_transfer(delay) or self.execute(delay,pc+4)!=(None,False):raise ValueError('control in upper-bound delay')
    self.branches.append([pc-self.entry,target is not None])
    if w==0x03E00008:
     if pc!=self.entry+148 or target!=RETURN:raise ValueError('actual terminal JR31/stop')
     return
    if w&63==9 and w>>26==0:
     continuation=self.r[31]&U32;self.predicate(target);pc=continuation
    else:pc=target if target is not None else pc+8
   else:
    if target is not None or annul:raise ValueError('unexpected upper-bound transfer')
    pc+=4
def fixture(original,routine,n,start,key,pattern,mode,mutation,alias):
 if any(type(x) is not int for x in (routine,n,start,key,pattern,mode,mutation,alias)):
  raise ValueError('upper-bound fixture integer domain')
 if not(0<=routine<59 and 0<=n<=16 and 0<=start<=16-n and -(1<<31)<=key<(1<<31) and pattern in range(4) and mode in range(4) and mutation in range(16) and alias in range(3)):
  raise ValueError('upper-bound fixture same-array domain')
 slot=BASE+(63 if not alias else 0 if alias==1 else 7)*4
 p=dict(n=n,start=start,key=key,pattern=pattern,mode=mode,mutation=mutation,alias=alias,slot=slot)
 t=UpperTrace(original,ENTRIES[routine],p)
 for i in range(WORDS):t.save(BASE+4*i,initial(i),4)
 for i in range(16):
  t.save(BASE+4*i,BASE+128+4*i,4)
  v=(2*i-12 if pattern==0 else 12-2*i if pattern==1 else 3 if pattern==2 else (-2147483648 if i%2 else 2147483647))
  t.save(BASE+128+4*i,v,4)
 t.save(BASE+192,key,4)
 if not alias:t.save(slot,BASE+192,4)
 before=[t.load(BASE+4*i,4) for i in range(WORDS)]
 for i in range(32):t.r[i]=((0x112233445566778899AABBCCDD000000+i)&((1<<128)-1))
 t.r[0]=0;t.r[4]=BASE+4*start;t.r[5]=BASE+4*(start+n);t.r[6]=slot;t.r[7]=CALLBACK;t.r[8]=0
 t.r[29]=0x80000;t.r[31]=RETURN
 saved={i:t.r[i] for i in range(16,24)};saved.update({29:t.r[29],31:t.r[31]})
 t.run()
 assert all(t.r[i]==v for i,v in saved.items()),'callee saved lanes'
 result=t.canonical(t.r[2]&U32)
 memory=[t.load(BASE+4*i,4) for i in range(WORDS)]
 pointers=set(range(16))|({63} if not alias else set())
 for i in pointers:memory[i]=t.canonical(memory[i]);before[i]=t.canonical(before[i])
 return dict(parameters=[routine,n,start,key,pattern,mode,mutation,alias],initial=before,memory=memory,
  result=result,events=t.events,instructions=t.instruction_count,visited=sorted(pc-t.entry for pc in t.visited),branches=t.branches)
def cases():
 # Every original entry executes every distribution; these are new observations.
 for r in range(59):
  for n in range(17):
   for k in (-16,-1,0,3,8,16,30):yield(r,n,(16-n)//2,k,0,0,0,0)
  for p in range(4):
   for mode in range(4):
    for mut in range(16):yield(r,13,1,6,p,mode,mut,mut%3)
  for alias in range(3):
   for mut in range(16):yield(r,16,0,-1,0,0,mut,alias)
def golden(rows):
 lines=['/* Authored initialized synthetic fixtures; no original instruction bytes. */',
  'struct UpperGolden { unsigned parameters[8],memory[96],result,event_count,events[32][6]; };',
  'static const struct UpperGolden upper_golden[] = {']
 # One 423-case expectation suite is deduplicated only after comparing every
 # complete result/memory/event record from all 59 actual original PCs.
 suite=[q for q in rows if q['parameters'][0]==0]
 for r in range(59):
  other=[q for q in rows if q['parameters'][0]==r]
  assert len(other)==len(suite)
  for a,b in zip(suite,other):
   assert a['parameters'][1:]==b['parameters'][1:]
   for field in ('initial','memory','result','events','instructions','visited','branches'):assert a[field]==b[field],(r,field)
 for q in suite:
  par=[x&U32 for x in q['parameters']];ev=q['events']+[[0]*6]*(32-len(q['events']))
  if len(q['events'])>32:raise ValueError('upper-bound event bound')
  vals=lambda v:'{'+','.join('0x%08Xu'%x for x in v)+'}'
  lines.append('{%s,%s,%du,%du,{%s}},'%(vals(par),vals(q['memory']),q['result'],len(q['events']),','.join(vals(x) for x in ev)))
 lines.append('};');return ('\n'.join(lines)+'\n').encode()
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path);ap.add_argument('--golden-header',type=Path);args=ap.parse_args()
 _,original=validated_elf(ROOT/'orig/SLUS_216.68')
 rows=[fixture(original,*p) for p in cases()]
 selected_coverage=[sorted(set(x for q in rows if q['parameters'][0]==r for x in q['visited'])) for r in range(59)]
 assert all(x==[i for i in range(0,156,4) if i!=100] for x in selected_coverage)
 package=dict(fixtures=rows,count=len(rows),instructions=sum(q['instructions'] for q in rows),max_instructions=max(q['instructions'] for q in rows),
  coverage=selected_coverage,input_sha256=hashlib.sha256(json.dumps([q['parameters'] for q in rows],separators=(',',':')).encode()).hexdigest(),
  qualification='59 actual entries, 38/39 words each; offset100 NOP is unreachable behind unconditional delay-completed branch. No old C/native counts reused.')
 if args.output:args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_bytes((json.dumps(package,indent=2)+'\n').encode())
 if args.golden_header:args.golden_header.parent.mkdir(parents=True,exist_ok=True);args.golden_header.write_bytes(golden(rows))
 print('SGI original observations PASS',len(rows),package['instructions'],package['max_instructions'])
if __name__=='__main__':main()
