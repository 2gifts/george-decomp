"""Owned encoding/delay/initialized-object guards; exact-original cases skip."""
from pathlib import Path
import struct,sys,unittest
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_type_info_before import BeforeTrace,ENTRY,END_ENTRY,CMP,END_CMP,fixture
from trace_string_registry import BASE,STACK,RETURN

class Guards(unittest.TestCase):
 def observer(self,words=()):
  raw=bytearray(END_CMP-0xFF000)
  for i,w in enumerate(words):struct.pack_into('<I',raw,ENTRY-0xFF000+i*4,w)
  struct.pack_into('<II',raw,END_ENTRY-8-0xFF000,0x03E00008,0)
  t=BeforeTrace(bytes(raw));t.r[31]=RETURN;return t
 def test_owned_fetch_image(self):
  t=self.observer()
  for pc in (ENTRY-4,END_ENTRY,ENTRY+1,END_CMP):
   with self.assertRaisesRegex(ValueError,'unowned'):t.fetch(pc)
  t.original=b''
  with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(ENTRY)
 def test_initialized_alignment_and_wrap(self):
  t=self.observer()
  with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASE,4)
  t.save(BASE,0x89ABCDEF,4);self.assertEqual(t.load(BASE,4),0x89ABCDEF)
  for a in (BASE+1,0,0xFFFFFFFF):
   with self.assertRaisesRegex(ValueError,'unowned or unaligned'):t.load(a,4)
 def test_budget_and_zero_register(self):
  t=self.observer();t.instruction_count=29999;t.execute(0x24000001,ENTRY)
  self.assertEqual((t.instruction_count,t.r[0]),(30000,0));state=(t.r[:],t.visited.copy())
  with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,ENTRY)
  self.assertEqual((t.r,t.visited),state)
 def test_reserved_operands(self):
  for w in (0x002217C2,0x03E10008,0x44000000):
   with self.assertRaisesRegex(ValueError,'unsupported'):self.observer().execute(w,ENTRY)
 def test_delay_rejects_taken_and_untaken_controls(self):
  for outer in (0x10000001,0x14000001):
   for delay in (0x10000000,0x04010000,0x45000000,0x03E00008):
    with self.assertRaisesRegex(ValueError,'control in delay'):self.observer((outer,delay)).run_before()
 def test_actual_terminal_jr_and_reviewed_call(self):
  self.observer().run_before()
  with self.assertRaisesRegex(ValueError,'terminal JR31'):self.observer((0x03E00008,0)).run_before()
  t=self.observer();t.r[31]=RETURN+4
  with self.assertRaisesRegex(ValueError,'terminal JR31'):t.run_before()
  with self.assertRaisesRegex(ValueError,'unreviewed before callee'):self.observer((0x0C000000,0)).run_before()
 def test_fixture_domain(self):
  cases=((b'bad\0',b'',0,0,0),(b'A'*49,b'',0,0,0),(b'a',b'b',0,0,1),(b'',b'',16,0,0),(b'',b'',True,0,0))
  for args in cases:
   with self.assertRaisesRegex(ValueError,'domain|alias'):fixture(b'',*args)

@unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'exact local original required')
class OriginalContracts(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  from analyze import validated_elf
  _,cls.raw=validated_elf(ROOT/'orig/SLUS_216.68')
 def test_unsigned_names_and_all_selected_words(self):
  for a,b in ((b'\x80',b'\x7f'),(b'\x7f',b'\x80'),(b'',b'xyz'),(b'xyz',b'')):
   q,t=fixture(self.raw,a,b,1,3);self.assertEqual(q['result'],int(a<b));self.assertEqual(len([p for p in t.visited if ENTRY<=p<END_ENTRY]),9)
 def test_same_name_and_same_object(self):
  for alias in (1,2):
   q,t=fixture(self.raw,b'\xff-same',b'\xff-same',15,15,alias);self.assertEqual(q['result'],0);self.assertEqual(t.r[29],STACK[1]-0x100)
if __name__=='__main__':unittest.main()
