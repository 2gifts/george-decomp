"""Fail-closed guards for the new three-body initialized allocator observer."""
import copy
from pathlib import Path
import sys
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import trace_libio_allocator as m

class Guards(unittest.TestCase):
    def dummy(self):return m.LibioAllocatorTrace(bytes(0x300000),m.fixtures()[0])
    def test_unowned(self):
        with self.assertRaisesRegex(ValueError,'unowned'):self.dummy().load(0x1000,4)
    def test_uninitialized(self):
        with self.assertRaisesRegex(ValueError,'uninitialized'):self.dummy().load(m.BASE,4)
    def test_unaligned(self):
        with self.assertRaisesRegex(ValueError,'unaligned'):self.dummy().save(m.BASE+1,2,4)
    def test_padding_or_other_body(self):
        with self.assertRaisesRegex(ValueError,'unreviewed'):self.dummy().fetch(0x35e994)
    def test_budget_before_mutation(self):
        t=self.dummy();t.instruction_count=1200
        before=copy.deepcopy((t.r,t.memory,t.visited,t.instruction_count,t.events))
        with self.assertRaisesRegex(ValueError,'budget before mutation'):t.execute(0,0x35e910)
        self.assertEqual(before,(t.r,t.memory,t.visited,t.instruction_count,t.events))
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'original absent')
    def test_changed_instruction(self):
        t=m.LibioAllocatorTrace((ROOT/'orig/SLUS_216.68').read_bytes(),m.fixtures()[0])
        with self.assertRaisesRegex(ValueError,'differs'):t.execute(0,0x35e910)
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'original absent')
    def test_wrong_return(self):
        t=m.LibioAllocatorTrace((ROOT/'orig/SLUS_216.68').read_bytes(),m.fixtures()[0]);t.initialize()
        t.r[4]=3;t.r[31]=m.RETURN+4
        with self.assertRaisesRegex(ValueError,'actual JR31'):t.run(0x35e910)
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'original absent')
    def test_invalid_capacity(self):
        p=m.fixtures()[0];p['k']=2
        t=m.LibioAllocatorTrace((ROOT/'orig/SLUS_216.68').read_bytes(),p)
        with self.assertRaisesRegex(ValueError,'capacity'):t.initialize()
    @unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'original absent')
    def test_signed_minimum_and_full_short_gate(self):
        raw=(ROOT/'orig/SLUS_216.68').read_bytes()
        a=m.LibioAllocatorTrace(raw,m.fixtures()[0]).observe()
        self.assertEqual(a['events'],[[1,52,m.NEW-m.BASE]])
        p=next(p for p in m.fixtures() if p['method']==1 and not p['null'] and p['stack']==0x8000)
        self.assertEqual(m.LibioAllocatorTrace(raw,p).observe()['events'],[])

if __name__=='__main__':unittest.main()
