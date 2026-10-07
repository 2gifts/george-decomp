import sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
from trace_geometry import RETURN
from trace_pooled_destructor_duplicates import PooledTrace,ENTRIES,OBJECT,GLOBAL,MANAGER1,KEYS,parameters,fixture,initialize
class Guards(unittest.TestCase):
 def test_memory_ownership_alignment_unknown(self):
  t=PooledTrace(b'',0)
  for a,n in((OBJECT+1,2),(OBJECT+64,1),(GLOBAL,8),(0x12340,4),(OBJECT,3)):
   with self.assertRaisesRegex(ValueError,'unowned'):t.load(a,n)
   with self.assertRaisesRegex(ValueError,'unowned'):t.save(a,0,n)
  with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(OBJECT,4)
 def test_actual_pc_scope_and_image_bounds(self):
  for r,e in enumerate(ENTRIES):
   t=PooledTrace(b'',r)
   for pc in(e-4,e+1,e+84,0x16CED0):
    with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
    with self.assertRaisesRegex(ValueError,'unreviewed'):t.execute(0,pc)
   with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(e)
  for r in(True,-1,5):
   with self.assertRaisesRegex(ValueError,'routine domain'):PooledTrace(b'',r)
 def test_reserved_forms_and_zero(self):
  t=PooledTrace(b'',0)
  for w in(0x00201000,0x44810000,0x00210008,0x3C410001,0x04300000,0x0000000C):
   with self.assertRaises(ValueError):t.execute(w,t.entry)
  t.r[0]=0;t.r[1]=123;t.execute(0x00200025,t.entry);self.assertEqual(t.r[0],0)
 def test_budget_rejects_before_mutation(self):
  t=PooledTrace(b'',0);t.instruction_count=100;t.r[4]=OBJECT;t.r[2]=123;t.save(OBJECT,456,4);t.initializing=False
  before=(list(t.r),dict(t.memory),set(t.visited),list(t.accesses),t.instruction_count)
  with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0xAC820000,t.entry)
  self.assertEqual((t.r,t.memory,t.visited,t.accesses,t.instruction_count),before)
 def test_untaken_taken_delays_external_and_return(self):
  class Synthetic(PooledTrace):
   def fetch(self,pc):return self.code.get(pc,0)
  for branch in(0x10000000,0x14000000,0x04010000):
   t=Synthetic(b'',0);t.code={t.entry:branch,t.entry+4:0x10000000}
   with self.assertRaisesRegex(ValueError,'control in delay'):t.run()
  t=Synthetic(b'',0);t.code={t.entry:0x10000040}
  with self.assertRaisesRegex(ValueError,'outside body'):t.run()
  t=Synthetic(b'',0);t.code={t.entry:0x03E00008};t.r[31]=RETURN
  with self.assertRaisesRegex(ValueError,'terminal JR31'):t.run()
  t=PooledTrace(b'',0)
  with self.assertRaisesRegex(ValueError,'unreviewed pooled external'):t.external(0x123456)
 def test_fixture_domain_is_explicit(self):
  p=parameters()[0]
  self.assertEqual(len(parameters()),10080)
  for k,v in(('routine',True),('mode',-1),('size',65536),('replacement',-1),('mutation',8),('manager',3),('offset',1)):
   bad={**p,k:v}
   with self.assertRaisesRegex(ValueError,'fixture domain'):initialize(PooledTrace(b'',0),bad)
class Original(unittest.TestCase):
 def setUp(self):
  path=ROOT/'orig/SLUS_216.68'
  if not path.is_file():self.skipTest('private original absent')
  _,self.original=validated_elf(path)
 def test_full105_and_both_mode_paths(self):
  rows=[fixture(self.original,p)for p in parameters()]
  self.assertEqual({pc for x in rows for pc in x['visited']},{pc for e in ENTRIES for pc in range(e,e+84,4)})
  for e in ENTRIES:self.assertEqual({v for x in rows for pc,v in x['branches']if pc==e+36},{False,True})
 def test_fresh_manager_lhu_and_object_mode_capture(self):
  for r in range(5):
   p=dict(zip(KEYS,(r,0x80000001,0xFFFF,7,0x8001,1,16)));x=fixture(self.original,p)
   self.assertEqual(x['events'],[[0x3064F0,OBJECT+16,0,0,0],[0x2E2CD8,MANAGER1,OBJECT+16,0x8001,0x27]])
   access=x['accesses'];i=next(i for i,a in enumerate(access)if a[:3]==['read',GLOBAL,4])
   self.assertEqual(access[i+1][:3],['read',OBJECT+20,2]);self.assertTrue(x['restored128_s16_s17'])
 def test_zero_mode_still_base_but_no_release(self):
  for r in range(5):
   p=dict(zip(KEYS,(r,2,1,3,0x8000,1,0)));x=fixture(self.original,p)
   self.assertEqual(len(x['events']),1);self.assertFalse(any(a[:3]==['read',GLOBAL,4]for a in x['accesses']))
if __name__=='__main__':unittest.main()
