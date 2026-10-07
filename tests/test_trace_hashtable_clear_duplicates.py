import sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
from trace_geometry import RETURN
from trace_hashtable_clear_duplicates import DuplicateClearTrace,ENTRIES,TABLE,NODES,HEADS,parameters,fixture

class Guards(unittest.TestCase):
    def test_memory_geometry_and_unknown(self):
        t=DuplicateClearTrace(b'',0)
        for address,n in ((TABLE+1,4),(TABLE,8),(TABLE+32,4),(NODES+192,4),(0x12340,4)):
            with self.assertRaises(ValueError):t.load(address,n)
            with self.assertRaises(ValueError):t.save(address,0,n)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(TABLE,4)

    def test_actual_range_and_no_pc_spoof(self):
        for routine,entry in enumerate(ENTRIES):
            t=DuplicateClearTrace(b'',routine)
            for pc in (entry-4,entry+1,entry+156,0x2BF418):
                with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
                with self.assertRaisesRegex(ValueError,'unreviewed'):t.execute(0,pc)
            with self.assertRaisesRegex(ValueError,'image bounds'):t.fetch(entry)
        for routine in (-1,5,True):
            with self.assertRaisesRegex(ValueError,'routine domain'):DuplicateClearTrace(b'',routine)

    def test_reserved_and_unknown_forms(self):
        t=DuplicateClearTrace(b'',0)
        for word in (0x0C000000,0x44810000,0x0000000C,0x00800008,0x00220800,0x00621863):
            with self.assertRaisesRegex(ValueError,'unsupported'):t.execute(word,ENTRIES[0])

    def test_delay_taken_untaken_and_boundary(self):
        class Controlled(DuplicateClearTrace):
            def fetch(self,pc):return self.code.get(pc,0)
        for branch in (0x10000000,0x14000000):
            t=Controlled(b'',0);t.code={t.entry:branch,t.entry+4:0x03E00008}
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run()
        t=Controlled(b'',0);t.code={t.entry:0x10000040}
        with self.assertRaisesRegex(ValueError,'outside complete'):t.run()

    def test_return_identity_and_preserving_budget(self):
        class Controlled(DuplicateClearTrace):
            def fetch(self,pc):return self.code.get(pc,0)
        t=Controlled(b'',0);t.code={t.entry:0x03E00008};t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'actual terminal'):t.run()
        t=Controlled(b'',0);t.code={t.entry:0x1000FFFF}
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.run()
        t=DuplicateClearTrace(b'',0);t.save(TABLE,123,4);t.initializing=False
        t.instruction_count=1200;t.r[4]=TABLE;t.r[2]=456
        snapshot=(list(t.r),dict(t.memory),set(t.visited),list(t.accesses),t.instruction_count)
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0xAC820000,t.entry)
        self.assertEqual((t.r,t.memory,t.visited,t.accesses,t.instruction_count),snapshot)

    def test_likely_delay_annul(self):
        for routine in range(5):
            t=DuplicateClearTrace(b'',routine);t.r[5]=0
            self.assertEqual(t.execute(0x54A0FFFB,t.entry+96),(None,True))
            t.r[5]=1
            self.assertEqual(t.execute(0x54A0FFFB,t.entry+96),(t.entry+80,False))

    def test_fixture_domains(self):
        with self.assertRaisesRegex(ValueError,'fixture domain'):
            fixture(b'',dict(routine=0,alias=0,buckets=0,nodes=1,distribution=0,pool=0))
        self.assertEqual(len(parameters()),2910)

class Original(unittest.TestCase):
    def setUp(self):
        p=ROOT/'orig/SLUS_216.68'
        if not p.is_file():self.skipTest('private original absent')
        _,self.original=validated_elf(p)

    def test_zero_and_overlay_order_at_five_real_entries(self):
        for routine in range(5):
            empty=fixture(self.original,dict(routine=routine,alias=2,buckets=0,nodes=0,distribution=0,pool=4))
            self.assertEqual([x for x in empty['accesses'] if x[0]=='write'],[['write',TABLE+16,0]])
            self.assertFalse(any(x[1]>=HEADS for x in empty['accesses']))
            row=fixture(self.original,dict(routine=routine,alias=0,buckets=1,nodes=2,distribution=0,pool=1))
            seq=row['accesses'];i=next(i for i,x in enumerate(seq) if x[:2]==['read',HEADS+4])
            self.assertEqual([x[:2] for x in seq[i:i+4]],[['read',HEADS+4],['read',NODES],['write',NODES],['write',HEADS+4]])
            self.assertEqual(seq[i+1][2],NODES+16)

    def test_full_195_and_all_branch_outcomes(self):
        rows=[fixture(self.original,p) for p in parameters()]
        self.assertEqual({pc for x in rows for pc in x['visited']},{pc for a in ENTRIES for pc in range(a,a+156,4)})
        for a in ENTRIES:
            for d in (20,60,96,140):self.assertEqual({taken for x in rows for pc,taken in x['branches'] if pc==a+d},{False,True})

if __name__=='__main__':unittest.main()
