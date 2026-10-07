import sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
from trace_hashtable_clear import ClearTrace,ENTRY,STOP,TABLE,HEADS,NODES,RETURN,fixture,parameters

class Guards(unittest.TestCase):
    def test_memory_geometry_initialization(self):
        t=ClearTrace(b'')
        for a,n in ((TABLE+1,4),(TABLE,8),(TABLE+20,4),(0x12340,4)):
            with self.assertRaises(ValueError):t.load(a,n)
            with self.assertRaises(ValueError):t.save(a,0,n)
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(TABLE,4)

    def test_opcode_and_reserved_forms(self):
        t=ClearTrace(b'')
        for w in (0x0C000000,0x44810000,0x0000000C,0x00800008,0x00220800,0x00621863):
            with self.assertRaisesRegex(ValueError,'unsupported'):t.execute(w,ENTRY)

    def test_fetch_and_fixture_domains(self):
        t=ClearTrace(b'')
        for pc in (ENTRY-4,ENTRY+1,STOP):
            with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
        with self.assertRaisesRegex(ValueError,'image bounds'):t.fetch(ENTRY)
        with self.assertRaisesRegex(ValueError,'fixture domain'):
            fixture(b'',dict(buckets=0,nodes=1,distribution=0,pool=0))

    def test_taken_untaken_delay_and_outside(self):
        class Controlled(ClearTrace):
            def fetch(self,pc):return self.code.get(pc,0)
        for branch in (0x10000000,0x14000000):
            t=Controlled(b'');t.code={ENTRY:branch,ENTRY+4:0x03E00008}
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run()
        t=Controlled(b'');t.code={ENTRY:0x10000040}
        with self.assertRaisesRegex(ValueError,'outside complete'):t.run()

    def test_terminal_identity_and_loop_bound(self):
        class Controlled(ClearTrace):
            def fetch(self,pc):return self.code.get(pc,0)
        t=Controlled(b'');t.code={ENTRY:0x03E00008};t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'actual terminal'):t.run()
        t=Controlled(b'');t.code={ENTRY:0x1000FFFF}
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.run()
        # Failed budget must leave every observable decoder state untouched.
        t=ClearTrace(b'');t.save(TABLE,123,4);t.initializing=False
        t.instruction_count=1200;t.r[4]=TABLE;t.r[2]=456
        snapshot=(list(t.r),dict(t.memory),set(t.visited),list(t.accesses),t.instruction_count)
        with self.assertRaisesRegex(ValueError,'instruction bound'):
            t.execute(0xAC820000,ENTRY)
        self.assertEqual((t.r,t.memory,t.visited,t.accesses,t.instruction_count),snapshot)

    def test_branch_likely_annuls_delay(self):
        t=ClearTrace(b'');t.r[5]=0
        self.assertEqual(t.execute(0x54A0FFFB,ENTRY),(None,True))
        t.r[5]=1
        self.assertEqual(t.execute(0x54A0FFFB,ENTRY),((ENTRY+4-20)&0xFFFFFFFF,False))

class Original(unittest.TestCase):
    def setUp(self):
        p=ROOT/'orig/SLUS_216.68'
        if not p.is_file():self.skipTest('private original absent')
        _,self.original=validated_elf(p)

    def test_empty_table_keeps_all_pool_storage(self):
        p=dict(buckets=0,nodes=0,distribution=0,pool=4)
        r=fixture(self.original,p)
        self.assertEqual([x for x in r['accesses'] if x[0]=='write'],[['write',TABLE+16,0]])
        self.assertFalse(any(x[1]>=HEADS for x in r['accesses']))

    def test_free_head_next_overlay_publication_order(self):
        r=fixture(self.original,dict(buckets=1,nodes=2,distribution=0,pool=1))
        seq=r['accesses'];i=next(i for i,x in enumerate(seq) if x[:2]==['read',HEADS+4])
        self.assertEqual([x[:2] for x in seq[i:i+4]],
            [['read',HEADS+4],['read',NODES],['write',NODES],['write',HEADS+4]])
        self.assertEqual(seq[i+1][2],NODES+12)

    def test_entire39_and_both_branch_outcomes(self):
        rows=[fixture(self.original,p) for p in parameters()]
        self.assertEqual({pc for r in rows for pc in r['visited']},set(range(ENTRY,STOP,4)))
        for pc in (0x2BF42C,0x2BF454,0x2BF478,0x2BF4A4):
            self.assertEqual({taken for r in rows for p,taken in r['branches'] if p==pc},{False,True})

if __name__=='__main__':unittest.main()
