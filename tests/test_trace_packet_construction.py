"""Strict finite packet observer guards; private-image cases skip in clean CI."""
import sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_packet_construction import PacketTrace,fixture,BASE,END,SELECTED,GLOBALS,KEYS,VALUE
from trace_geometry import RETURN
ORIGINAL=ROOT/'orig/SLUS_216.68'
class PacketGuards(unittest.TestCase):
 def fresh(self):return PacketTrace(b'',dict(zip(KEYS,(0,1,64,0,0,0,0,4,7,165))))
 def test_unowned_unaligned_and_uninitialized(self):
  t=self.fresh()
  for address,n in ((BASE-4,4),(END,1),(BASE+1,4),(BASE,3)):
   with self.assertRaises(ValueError):t.load(address,n)
  with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASE,4)
  t.save(BASE,0x12345678,4);self.assertEqual(t.load(BASE,4),0x12345678)
 def test_fetch_bounds_and_image(self):
  t=self.fresh()
  for pc in (SELECTED[0][0]+1,SELECTED[0][1],0x12345000,SELECTED[0][0]):
   with self.assertRaises(ValueError):t.fetch(pc)
 def test_reserved_encoding_and_zero_register(self):
  t=self.fresh()
  for w in (0xfc000000|1<<16|1,0x00200840,0x7000003f):
   with self.assertRaises(ValueError):t.execute(w,SELECTED[0][0])
  t.execute(0x24001234,SELECTED[0][0]);self.assertEqual(t.r[0],0)
 def test_complete_instruction_budget(self):
  t=self.fresh();t.instruction_count=30000;before=(t.r.copy(),t.memory.copy(),t.visited.copy())
  with self.assertRaisesRegex(ValueError,'bound'):t.execute(0,SELECTED[0][0])
  self.assertEqual(t.instruction_count,30000);self.assertEqual(before,(t.r,t.memory,t.visited))
 def test_actual_return_required_and_delay_is_executed(self):
  entry=SELECTED[4][0];t=self.fresh();t.r[31]=RETURN
  t.fetch=lambda pc:{entry:0x03e00008,entry+4:0}.get(pc,0)
  with self.assertRaisesRegex(ValueError,'terminal'):t.run(entry)
  t=self.fresh();t.r[31]=RETURN+4;t.fetch=lambda pc:0x03e00008 if pc==entry else 0
  with self.assertRaisesRegex(ValueError,'JR31'):t.run(entry)
 def test_untaken_delay_controls_and_annul(self):
  entry=SELECTED[4][0]
  for delay in (0x10000000,0x04010000,0x45000000,0x03e00008):
   for take in (False,True):
    t=self.fresh();t.r[4]=1;t.r[5]=1 if take else 2;t.fetch=lambda pc:{entry:0x10850001,entry+4:delay}.get(pc,0)
    with self.assertRaisesRegex(ValueError,'control in delay'):t.run(entry)
  t=self.fresh();t.r[4]=1;t.r[5]=2;t.r[31]=RETURN
  t.fetch=lambda pc:{entry:0x50850007,entry+4:0x10000000,entry+8:0x03e00008}.get(pc,0)
  with self.assertRaisesRegex(ValueError,'terminal'):t.run(entry)
  self.assertNotIn(entry+4,t.visited)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires private original ELF')
 def test_real_fill_reference_reads_and_aliases(self):
  from analyze import validated_elf
  _,raw=validated_elf(ORIGINAL)
  for count in (0,1,2,7,17,64):
   for alias in range(5):
    c=fixture(raw,routine=4,length=count,alias=alias,value=0xa7)
    self.assertEqual(c['fill_reads'],count);self.assertEqual(c['result'],c['args'][0]+count)
 @unittest.skipUnless(ORIGINAL.is_file(),'requires private original ELF')
 def test_real_constructor_capture_and_nested_fresh_lengths(self):
  from analyze import validated_elf
  _,raw=validated_elf(ORIGINAL)
  original=fixture(raw,routine=1,first_length=4,second_length=7,mutation=0)
  shrink=fixture(raw,routine=1,first_length=4,second_length=7,mutation=1)
  grow=fixture(raw,routine=1,first_length=4,second_length=7,mutation=2)
  self.assertEqual(original['events'],shrink['events']);self.assertEqual(original['events'],grow['events'])
  self.assertEqual(original['result'],grow['result']);self.assertNotEqual(original['changes'],shrink['changes']);self.assertNotEqual(grow['changes'],shrink['changes'])
 @unittest.skipUnless(ORIGINAL.is_file(),'requires private original ELF')
 def test_real_capacity_failure_and_refill(self):
  from analyze import validated_elf
  _,raw=validated_elf(ORIGINAL)
  fail=fixture(raw,routine=2,length=17,capacity=24);self.assertEqual(fail['result'],0);self.assertEqual(fail['changes'],[])
  refill=fixture(raw,routine=0,length=17,allocator=0);self.assertEqual(refill['invocations'][-2:],[1,1])
if __name__=='__main__':unittest.main()
