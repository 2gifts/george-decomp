"""Owned-memory, exact helper boundaries and real lifecycle regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_completion_resolver as m
from trace_geometry import RETURN
from analyze import validated_elf
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class ResolverGuards(unittest.TestCase):
 def trace(self,words=()):
  raw=bytearray(0x448058-0xFF000)
  for i,w in enumerate(words):struct.pack_into('<I',raw,m.SELECTED[0][0]-0xFF000+i*4,w)
  return m.ResolverTrace(bytes(raw),{'routine':3,'mutation':0,'fail_first':0})
 def test_owned_initialized_prior_byte_and_bounds(self):
  t=self.trace()
  with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.TABLE+1,1)
  t.save(m.TABLE+1,0xA5,1);self.assertEqual(t.load(m.TABLE+1,1),0xA5)
  for a,n in ((m.BASE-4,4),(m.END,4),(m.TABLE+1,4),(m.RANGE+12,4)):
   with self.assertRaises(ValueError):t.load(a,n)
  self.assertEqual(t.load(0x447F62,1),0)
  with self.assertRaises(ValueError):t.load(0x447F63,1)
 def test_full_fetch_formatter_eof_exclusion_and_entry(self):
  t=self.trace()
  for pc in (m.SELECTED[0][0]-4,m.SELECTED[0][0]+1,0x2BF964,0x395348):
   with self.assertRaises(ValueError):t.fetch(pc)
  with self.assertRaises(ValueError):t.run(m.SELECTED[0][0]+4)
  with self.assertRaisesRegex(ValueError,'image bounds'):m.ResolverTrace(b'',{}).fetch(m.SELECTED[0][0])
 def test_delay_control_reserved_and_annul(self):
  for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,0x03E00008):
   t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay));t.r[4:6]=[1,2];t.r[31]=RETURN
   with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.SELECTED[0][0])
  t=self.trace();t.r[4:6]=[1,1]
  target,annul=t.execute((21<<26)|(4<<21)|(5<<16)|1,m.SELECTED[0][0]);self.assertIsNone(target);self.assertTrue(annul)
  for w in (0xFFFFFFFF,(6<<26)|(1<<16),(1<<26)|(4<<16)):
   with self.assertRaises(ValueError):t.execute(w,m.SELECTED[0][0])
 def test_division_traps_instruction_and_contract_limits(self):
  t=self.trace();t.r[4]=3;t.r[5]=0
  with self.assertRaisesRegex(ValueError,'division by zero'):t.execute((4<<21)|(5<<16)|27,m.SELECTED[0][0])
  with self.assertRaises(ValueError):t.execute(0x000001CD,m.SELECTED[0][0])
  with self.assertRaisesRegex(ValueError,'external'):t.external(0x123456)
  t.r[4:7]=[m.HEAP,0x1401,3]
  with self.assertRaisesRegex(ValueError,'allocation contract'):t.external(0x2ADF60)
  t.instruction_count=30000
  with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,m.SELECTED[0][0])
 def test_typed_links_are_pc_based_and_finite_swc1(self):
  t=self.trace();t.pc=0x252260;t.save(m.NEW,0,4);self.assertIn(m.NEW,t.pointer_cells)
  t.pc=0x1006C4;t.save(m.NEW+8,m.NEW+16,4);self.assertIn(m.NEW+8,t.pointer_cells)
  t.pc=0x2BF550;t.save(m.NEW+24,m.NEW,4);self.assertNotIn(m.NEW+24,t.pointer_cells)
  t.f[12]=float('nan');t.r[29]=0x80000
  with self.assertRaisesRegex(ValueError,'nonfinite'):t.execute((57<<26)|(29<<21)|(12<<16),0x395310)
 def test_initialized_fixture_domain(self):
  for p in ({'routine':True},{'routine':9},{'size':4,'capacity':3},{'count':-1},{'position':5},{'nodes':4},{'key':0x100000000},{'slot':16},{'mutation':16}):
   with self.assertRaises(ValueError):m.fixture(b'',**p)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_full_format_fresh_index_and_first_failed_allocation(self):
  _,raw=validated_elf(ORIGINAL);r=m.fixture(raw,routine=1,slot=7,key=0x89ABCDEF,mutation=8)
  final=dict(r['differences']);self.assertEqual(final[m.RESOLVER+24-m.BASE],0)
  self.assertEqual(r['events'],[[5,m.RESOLVER+28+7*64,0x89ABCDEF,0]])
  r=m.fixture(raw,routine=0,fail_first=1);self.assertEqual([e[0] for e in r['events']],[1,3,4,1]);self.assertTrue(r['invocations'][10])
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_fill_alias_and_real_allocator_refill_links(self):
  _,raw=validated_elf(ORIGINAL);r=m.fixture(raw,routine=3,size=3,capacity=4,position=1,count=5,alias_value=1)
  self.assertTrue(r['invocations'][11]);self.assertTrue(r['invocations'][12])
  self.assertTrue(any(m.NEW<=a<m.END for a in r['pointer_cells']))
  self.assertEqual(r['return_value'],None)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_payload_before_node_release_and_exit_null_publication(self):
  _,raw=validated_elf(ORIGINAL);r=m.fixture(raw,routine=4,nodes=3,mutation=2)
  payloads=[e[1] for e in r['events'] if e[0]==2];self.assertEqual(payloads,[m.PAYLOADS[0],m.PAYLOADS[2],m.RESOLVER])
  self.assertTrue(r['invocations'][5]);self.assertTrue(r['invocations'][6])
  r=m.fixture(raw,routine=8,exit_null=1);self.assertEqual(dict(r['globals'])[m.RANGE+4],m.EXIT_SLOTS);self.assertEqual(r['events'],[])

if __name__=='__main__':unittest.main()
