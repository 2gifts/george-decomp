"""Strict synthetic decoding/ownership and actual alias/callback regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_buffer_completion as m
from trace_geometry import RETURN
from analyze import validated_elf
ORIGINAL=m.ROOT/'orig/SLUS_216.68'
class CompletionGuards(unittest.TestCase):
 def trace(self,words=()):
  raw=bytearray(0x448000-0xFF000)
  for i,w in enumerate(words):struct.pack_into('<I',raw,m.SELECTED[0][0]-0xFF000+i*4,w)
  return m.CompletionTrace(bytes(raw),{})
 def test_owned_initialized_and_one_past(self):
  t=self.trace()
  with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.UI,4)
  t.save(m.UI,0x89ABCDEF,4);self.assertEqual(t.load(m.UI,4),0x89ABCDEF)
  for a,n in ((m.END,4),(m.BASE-4,4),(m.UI+1,4),(m.RANGE+12,4)):
   with self.assertRaises(ValueError):t.load(a,n)
  self.assertEqual(t.load(0x447F62,1),0)
  with self.assertRaises(ValueError):t.load(0x447F63,1)
 def test_full_fetch_and_actual_terminal(self):
  t=self.trace((0x03E00008,0));t.r[31]=RETURN
  with self.assertRaisesRegex(ValueError,'terminal'):t.run(m.SELECTED[0][0])
  for pc in (m.SELECTED[0][0]-4,m.SELECTED[0][0]+1,m.SELECTED[0][1],0x395348):
   with self.assertRaises(ValueError):t.fetch(pc)
  with self.assertRaises(ValueError):t.run(m.SELECTED[0][0]+4)
  with self.assertRaisesRegex(ValueError,'image bound'):m.CompletionTrace(b'',{}).fetch(m.SELECTED[0][0])
 def test_reserved_delay_and_annul(self):
  t=self.trace()
  for w in ((6<<26)|(1<<16),0xFFFFFFFF,(17<<26),(1<<26)|(4<<16)):
   with self.assertRaises(ValueError):t.execute(w,m.SELECTED[0][0])
  for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,0x03E00008):
   t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay));t.r[4:6]=[1,2]
   with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.SELECTED[0][0])
  t=self.trace();t.r[4:6]=[1,1];t.r[16]=m.UI
  target,annul=t.execute((21<<26)|(4<<21)|(5<<16)|1,m.SELECTED[0][0]);self.assertTrue(annul);self.assertIsNone(target);self.assertNotIn(m.UI,t.memory)
  t.r[0]=1;t.execute(0,m.SELECTED[0][0]);self.assertEqual(t.r[0],0)
 def test_unknown_contract_and_bounds(self):
  t=self.trace()
  with self.assertRaisesRegex(ValueError,'external'):t.external(0x123456)
  t.instruction_count=30000
  with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,m.SELECTED[0][0])
  t=self.trace();t.f[12]=float('nan');t.r[29]=0x80000
  with self.assertRaisesRegex(ValueError,'nonfinite'):t.execute((57<<26)|(29<<21)|(12<<16),0x395310)
 def test_fixture_domain(self):
  for kw in ({'routine':True},{'routine':4},{'mutation':16},{'count':-1},{'common':0x100000000},{'input':'\x80'},{'input':'a'*81},{'alias':5},{'line_limit':0}):
   with self.assertRaises(ValueError):m.fixture(b'',**kw)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_complete_selected_and_whole_helpers(self):
  _,raw=validated_elf(ORIGINAL);rows=[m.fixture(raw,**p) for p in m.cases()];seen={pc for r in rows for pc in r['visited']}
  for lo,hi in m.SELECTED:self.assertFalse(set(range(lo,hi,4))-seen)
  self.assertTrue(any(r['invocations'][12] for r in rows));self.assertTrue(any(r['invocations'][16] for r in rows))
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_full_key_format_and_current_global_ui(self):
  _,raw=validated_elf(ORIGINAL);r=m.fixture(raw,routine=0,key=0x89ABCDEF,mutation=6)
  text=bytes(sum(([v>>(8*i)&255 for i in range(4)] for v in r['final'][(m.RESOLVER+28-m.BASE)//4:]),[])).split(b'\0')[0]
  fmt=raw[0x447F48-0xFF000:0x447F63-0xFF000-1]
  self.assertEqual(text,fmt.replace(b'%08x',b'89abcdef'));self.assertEqual(r['globals_final'][1],m.ALT)
  self.assertEqual(r['final'][(m.RESOLVER+24-m.BASE)//4],0)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_alias_publication_fresh_end_and_manager(self):
  _,raw=validated_elf(ORIGINAL);r=m.fixture(raw,routine=0,range_mode=3)
  self.assertEqual(r['globals_final'][3],m.RECORD+4)
  r=m.fixture(raw,count=1,alias=3);appends=[e for e in r['events'] if e[0]==5]
  self.assertEqual([e[1] for e in appends],[m.MANAGERS[0],m.MANAGERS[1],m.MANAGERS[1]])
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_walkers_distinct_next_timing_and_wrap(self):
  _,raw=validated_elf(ORIGINAL);direct=m.fixture(raw,routine=2,mutation=8);generic=m.fixture(raw,routine=3,mutation=8)
  self.assertEqual(direct['invocations'][0],1);self.assertEqual(generic['invocations'][0],2)
  r=m.fixture(raw,count=0x7FFFFFFF);self.assertEqual(r['final'][(m.UI+12-m.BASE)//4],0x80000000)
if __name__=='__main__':unittest.main()
