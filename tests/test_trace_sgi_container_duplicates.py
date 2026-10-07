"""Concrete clone PCs, unchanged decoder and valid consumed-view regressions."""
from pathlib import Path
import struct, sys, unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))
import trace_sgi_container_duplicates as m
from trace_geometry import RETURN
from analyze import validated_elf
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class CloneGuards(unittest.TestCase):
    def trace(self, words=(), entry=9):
        raw=bytearray(0x448058-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,m.SELECTED[entry][0]-0xFF000+i*4,w)
        return m.CloneTrace(bytes(raw), dict(routine=6, mutation=0, fail_first=0))

    def test_owned_initialized_bounds(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.TABLE+1,1)
        t.save(m.TABLE+1,0xA5,1);self.assertEqual(t.load(m.TABLE+1,1),0xA5)
        for a,n in ((m.BASE-4,4),(m.END,4),(m.TABLE+1,4)):
            with self.assertRaises(ValueError):t.load(a,n)

    def test_actual_clone_scopes_exclude_seed_padding_and_bad_entry(self):
        t=self.trace()
        for pc in (0x2BF0F0,0x2BF4B8,m.SELECTED[9][0]+1,m.SELECTED[9][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(m.SELECTED[9][0]+4)
        with self.assertRaisesRegex(ValueError,'image bounds'):m.CloneTrace(b'',{}).fetch(m.SELECTED[0][0])

    def test_taken_untaken_delay_and_wrong_return(self):
        for equal in (True,False):
            t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,0x03E00008))
            t.r[4:6]=[1,1 if equal else 2];t.r[31]=RETURN
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.SELECTED[9][0])
        t=self.trace((0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'terminal JR31'):t.run(m.SELECTED[9][0])

    def test_budget_before_decoder_mutation(self):
        t=self.trace();t.instruction_count=30000;t.r[4]=m.TABLE;t.save(m.TABLE,123,4)
        before=(list(t.r),dict(t.memory),set(t.visited),t.instruction_count)
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute((43<<26)|(4<<21)|(5<<16),m.SELECTED[9][0])
        self.assertEqual(before,(t.r,t.memory,t.visited,t.instruction_count))

    def test_division_trap_reserved_operands_and_unknown_engine(self):
        t=self.trace();t.r[4]=3;t.r[5]=0
        with self.assertRaisesRegex(ValueError,'division by zero'):t.execute((4<<21)|(5<<16)|27,m.SELECTED[9][0])
        for w in (0x000001CD,0xFFFFFFFF,(6<<26)|(1<<16),(1<<26)|(4<<16)):
            with self.assertRaises(ValueError):t.execute(w,m.SELECTED[9][0])
        with self.assertRaisesRegex(ValueError,'external'):t.external(0x123456)

    def test_integral_initialized_domains(self):
        for p in ({'entry':True},{'entry':14},{'size':4,'capacity':3},{'count':-1},{'position':5},{'key':0x100000000},{'bucket_count':0},{'iterator_path':3}):
            with self.assertRaises(ValueError):m.fixture(b'',**p)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_every_actual_entry_and_original_return(self):
        _,raw=validated_elf(ORIGINAL)
        for entry,(lo,hi) in enumerate(m.SELECTED):
            x=m.fixture(raw,entry=entry)
            self.assertEqual(x['invocations'][entry],1)
            self.assertIn(hi-8,x['visited']);self.assertIn(hi-4,x['visited'])
            self.assertFalse(any(0x2BF0F0<=pc<0x2BF550 for pc in x['visited']))
            self.assertEqual(x['return_value'],None if entry<9 else m.ITERATOR)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_unsigned_key_and_bucket_paths(self):
        _,raw=validated_elf(ORIGINAL)
        for entry in range(9,14):
            for path,expected in ((0,m.NODES[1]),(1,m.NODES[2]),(2,0)):
                x=m.fixture(raw,entry=entry,key=0x80000000,bucket_count=5,iterator_path=path)
                changed=dict(x['differences']);self.assertEqual(changed[m.ITERATOR-m.BASE],expected)
                self.assertEqual(x['return_value'],m.ITERATOR)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_genuine_alloc_refill_oom_and_value_alias(self):
        _,raw=validated_elf(ORIGINAL)
        x=m.fixture(raw,entry=0,size=3,capacity=4,position=1,count=33,alias_value=1,fail_first=1)
        self.assertEqual([e[0] for e in x['events']],[1,3,4,1])
        self.assertTrue(x['invocations'][14]);self.assertTrue(x['invocations'][19])
        y=m.fixture(raw,entry=8,size=3,capacity=4,position=1,count=12)
        self.assertTrue(y['invocations'][15]);self.assertTrue(y['invocations'][16])

if __name__=='__main__':unittest.main()
