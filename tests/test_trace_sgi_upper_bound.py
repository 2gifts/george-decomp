"""Strict bounded integer/callback observations, portable without private ELF."""
import struct,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_sgi_upper_bound import UpperTrace,ENTRIES,BASE,WORDS,RETURN,CALLBACK,fixture
from analyze import validated_elf
class UpperGuards(unittest.TestCase):
 def trace(self,words=()):
  entry=ENTRIES[0];b=bytearray(entry-0xFF000+156)
  for i,w in enumerate(words):struct.pack_into('<I',b,entry-0xFF000+4*i,w)
  return UpperTrace(bytes(b),entry,dict(mode=0,mutation=0,slot=BASE+252))
 def test_counter_boundary_preserves_state(self):
  t=self.trace();t.instruction_count=29999;t.execute(0,t.entry)
  self.assertEqual(t.instruction_count,30000);before=(t.r[:],dict(t.memory),set(t.visited))
  with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,t.entry+4)
  self.assertEqual(before,(t.r, t.memory,t.visited))
 def test_reserved_operands_fail(self):
  for w in ((1<<6)|0x2D,(1<<21)|3,0x0280F809|(1<<16),0x03E00008|(1<<11),0x18010000,0x70000000):
   with self.subTest(word=w),self.assertRaises(ValueError):self.trace().execute(w,ENTRIES[0])
 def test_initialized_alignment_and_memory_ownership(self):
  t=self.trace()
  for a,n in ((BASE,4),(BASE+1,4),(BASE+WORDS*4,4),(BASE,2),(0x80000,8)):
   with self.assertRaises(ValueError):t.load(a,n)
  t.save(BASE,0x89ABCDEF,4);self.assertEqual(t.load(BASE,4),0x89ABCDEF)
 def test_code_scope_does_not_accept_padding(self):
  t=self.trace()
  for pc in (t.entry-4,t.entry+156,t.entry+1):
   with self.assertRaises(ValueError):t.fetch(pc)
  with self.assertRaises(ValueError):UpperTrace(b'',ENTRIES[0]+4,{})
 def test_callback_contract_rejects_unknown_target_and_payload(self):
  t=self.trace()
  with self.assertRaisesRegex(ValueError,'unknown'):t.predicate(CALLBACK+4)
  t.r[4]=BASE;t.r[5]=BASE
  with self.assertRaisesRegex(ValueError,'payload'):t.predicate(CALLBACK)
 def test_untaken_branch_control_delay_is_rejected(self):
  # Authored synthetic BEQ $1,$2,+0, with encoded ordinary/REGIMM/COP delay.
  for delay in (0x10220000,0x04210000,0x45000000):
   t=self.trace((0x10220000,delay));t.r[1]=1;t.r[2]=2
   with self.assertRaisesRegex(ValueError,'control in'):t.run()
 def test_only_actual_terminal_jr31_returns(self):
  t=self.trace((0x03E00008,0));t.r[31]=RETURN
  with self.assertRaisesRegex(ValueError,'terminal'):t.run()
 def test_zero_lane_and_invalid_fixture_geometry(self):
  t=self.trace();t.r[0]=12;t.execute(0,t.entry);self.assertEqual(t.r[0],0)
  for args in ((59,0,0,0,0,0,0,0),(0,17,0,0,0,0,0,0),(0,16,1,0,0,0,0,0),(0,1,0,1<<31,0,0,0,0),(False,1,0,0,0,0,0,0)):
   with self.assertRaises(ValueError):fixture(b'',*args)
 @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'private exact original is not supplied')
 def test_actual_shifted_value_slot_and_full_saved_lanes(self):
  _,original=validated_elf(ROOT/'orig/SLUS_216.68')
  baseline=fixture(original,0,13,1,6,0,0,5,2)
  other=fixture(original,58,13,1,6,0,0,5,2)
  for k in ('memory','events','result','visited','instructions'):self.assertEqual(baseline[k],other[k])
  self.assertGreater(len(baseline['events']),1)
  self.assertNotEqual(baseline['events'][0][0],baseline['events'][1][0])
if __name__=='__main__':unittest.main()
