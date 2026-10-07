"""Exact clone scopes and inherited ownership/callback/annul regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_descriptor_destructor_duplicates as m
import trace_completion_resolver as inherited
from trace_geometry import RETURN
from analyze import validated_elf
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class DescriptorGuards(unittest.TestCase):
 def trace(self,words=()):
  raw=bytearray(0x448058-0xFF000)
  for i,w in enumerate(words):struct.pack_into('<I',raw,m.ADDRESSES[0]-0xFF000+i*4,w)
  return m.DescriptorTrace(bytes(raw),{'routine':7,'mutation':0,'fail_first':0})
 def test_exact_code_identity_and_imported_scopes_preserved(self):
  self.assertIs(m.DescriptorTrace.fetch.__code__,inherited.ResolverTrace.fetch.__code__)
  self.assertIs(m.DescriptorTrace.run.__code__,inherited.ResolverTrace.run.__code__)
  self.assertIs(m.DescriptorTrace.execute,inherited.ResolverTrace.execute)
  self.assertEqual(len(inherited.RANGES),19);self.assertEqual(len(m.RANGES),43)
  self.assertEqual(inherited.SELECTED[7],(0x104B10,0x104B90))
 def test_actual_entries_and_invalid_fixture_types(self):
  for e in (True,-1,24,0.0):
   with self.assertRaisesRegex(ValueError,'entry domain'):m.fixture(b'',entry=e)
  for p in ({'nodes':4},{'flags':-1},{'flags':0x100000000},{'mutation':16}):
   with self.assertRaises(ValueError):m.fixture(b'',**p)
  t=self.trace()
  for pc in (m.ADDRESSES[0]-4,m.ADDRESSES[0]+1,m.ADDRESSES[0]+128):
   with self.assertRaises(ValueError):t.fetch(pc)
  with self.assertRaises(ValueError):t.run(m.ADDRESSES[0]+4)
 def test_owned_initialized_storage_and_readonly_bounds(self):
  t=self.trace()
  with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(inherited.DESCRIPTOR+4,4)
  t.save(inherited.DESCRIPTOR+4,0xA5870301,4);self.assertEqual(t.load(inherited.DESCRIPTOR+4,4),0xA5870301)
  for a,n in ((inherited.BASE-4,4),(inherited.END,4),(inherited.DESCRIPTOR+1,4),(inherited.RANGE+12,4)):
   with self.assertRaises(ValueError):t.load(a,n)
  with self.assertRaises(ValueError):t.load(0x447F63,1)
 def test_delay_control_including_untaken_and_likely_annul(self):
  for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,0x03E00008):
   t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay));t.r[4:6]=[1,2];t.r[31]=RETURN
   with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.ADDRESSES[0])
  t=self.trace();t.r[4:6]=[1,1]
  self.assertEqual(t.execute((21<<26)|(4<<21)|(5<<16)|1,m.ADDRESSES[0]),(None,True))
 def test_pre_mutation_budget_and_r0(self):
  t=self.trace();t.instruction_count=30000;t.r[29]=0x80000
  before=(dict(t.memory),set(t.visited),list(t.events),list(t.r))
  with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute((43<<26)|(29<<21)|(4<<16),m.ADDRESSES[0])
  self.assertEqual((t.memory,t.visited,t.events,t.r),before)
  t=self.trace();t.r[0]=0;t.execute((9<<26)|123,m.ADDRESSES[0]);self.assertEqual(t.r[0],0)
 def test_external_release_and_reserved_words_rejected(self):
  t=self.trace()
  with self.assertRaises(ValueError):t.external(0x123456)
  t.r[4]=inherited.END
  with self.assertRaisesRegex(ValueError,'release contract'):t.external(0x2AE158)
  for w in (0xFFFFFFFF,(6<<26)|(1<<16),(1<<26)|(4<<16)):
   with self.assertRaises(ValueError):t.execute(w,m.ADDRESSES[0])
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_all24_complete_bytes_actual_pc_and_lowbit(self):
  _,raw=validated_elf(ORIGINAL);seed=raw[0x104B10-0xFF000:0x104B90-0xFF000]
  for i,a in enumerate(m.ADDRESSES):
   self.assertEqual(raw[a-0xFF000:a+128-0xFF000],seed)
   row=m.fixture(raw,entry=i,nodes=0,flags=0x80000000)
   covered=set(row['visited'])
   self.assertEqual(sum(row['invocations'][:24]),1);self.assertEqual(row['events'],[])
   row=m.fixture(raw,entry=i,nodes=0,flags=0x80000001)
   self.assertEqual(row['events'],[[2,inherited.DESCRIPTOR,0,0]])
   covered.update(row['visited']);covered.update(m.fixture(raw,entry=i,nodes=3,flags=1)['visited'])
   self.assertTrue(set(range(a,a+128,4)).issubset(covered))
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_captured_descriptor_global_mutation_and_payload_order(self):
  _,raw=validated_elf(ORIGINAL);row=m.fixture(raw,entry=23,nodes=3,flags=0xFFFFFFFF,mutation=6)
  releases=[e[1] for e in row['events'] if e[0]==2]
  self.assertEqual(releases,[inherited.PAYLOADS[0],inherited.PAYLOADS[2],inherited.RESOLVER,inherited.DESCRIPTOR])
  self.assertEqual(dict(row['globals'])[inherited.REG],0)
  self.assertEqual(dict(row['globals'])[inherited.SHUTDOWN],1)
  self.assertEqual(dict(row['differences'])[inherited.DESCRIPTOR+4-inherited.BASE],0x4200F8)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
 def test_no_descriptor_delete_even_flag_preserves_teardown(self):
  _,raw=validated_elf(ORIGINAL)
  for flags in (0,2,0x80000000):
   row=m.fixture(raw,entry=0,nodes=2,flags=flags,mutation=4)
   self.assertEqual(row['events'][-1],[2,inherited.RESOLVER,0,0])
   self.assertNotIn([2,inherited.DESCRIPTOR,0,0],row['events'])
   self.assertEqual(dict(row['globals'])[inherited.SHUTDOWN],1)

if __name__=='__main__':unittest.main()
