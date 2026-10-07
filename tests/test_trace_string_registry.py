"""Strict registry decoder guards and authentic parsing/publication regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_string_registry as m
from analyze import validated_elf
from trace_geometry import RETURN
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class RegistryGuards(unittest.TestCase):
    def trace(self,words=()):
        raw=bytearray(m.RANGES[-1][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,m.SELECTED[0][0]-0xFF000+4*i,w)
        return m.RegistryTrace(bytes(raw),dict(failure=0,mutation=0))
    def test_initialized_owned_memory(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.KEY,1)
        t.save(m.KEY,0x7F,1);self.assertEqual(t.load(m.KEY,1),0x7F)
        for a,n in ((m.END,4),(m.BASE-1,1),(m.KEY,4),(m.COUNT+12,8),(m.TEXT_GLOBALS[2]+128,1)):
            with self.assertRaises(ValueError):t.load(a,n)
    def test_entry_fetch_terminal_and_depth(self):
        t=self.trace((0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'terminal'):t.run(m.SELECTED[0][0])
        with self.assertRaises(ValueError):t.run(m.SELECTED[0][0]+4)
        with self.assertRaises(ValueError):t.run(m.SELECTED[0][0],depth=6)
        for pc in (m.SELECTED[0][0]-4,m.SELECTED[0][0]+1,m.SELECTED[0][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
    def test_reserved_mmi_signed_lanes_and_zero(self):
        t=self.trace();pc=m.SELECTED[0][0]
        for w in (0xFFFFFFFF,(15<<26)|(1<<21),(28<<26)|8|(2<<6),(17<<26),0x12|(1<<21)):
            with self.assertRaises(ValueError):t.execute(w,pc)
        t.r[4]=0;t.r[5]=(1<<128)-1
        t.execute((28<<26)|(4<<21)|(5<<16)|(2<<11)|(1<<6)|8,pc)
        self.assertEqual(t.r[2],sum(1<<(32*i) for i in range(4)))
        t.execute((28<<26)|(4<<21)|(5<<16)|(2<<11)|(9<<6)|8,pc)
        self.assertEqual(t.r[2],sum(1<<(8*i) for i in range(16)))
        t.r[0]=5;t.execute(0,pc);self.assertEqual(t.r[0],0)
    def test_delay_rejection_and_annul(self):
        for equal in (False,True):
            t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,0x03E00008));t.r[4:6]=[1,1 if equal else 2]
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.SELECTED[0][0])
        t=self.trace();t.r[4:6]=[1,2]
        self.assertEqual(t.execute((20<<26)|(4<<21)|(5<<16)|1,m.SELECTED[0][0]),(None,True))
    def test_bounds_and_indeterminate_domains(self):
        t=self.trace();t.instruction_count=30000
        with self.assertRaises(ValueError):t.execute(0,m.SELECTED[0][0])
        for kw in ({'routine':True},{'routine':4},{'routine':1,'input':''},{'routine':1,'input':':tag'},
                   {'key':'\x80'},{'capacity':-1},{'failure':16},{'mutation':4}):
            with self.assertRaises(ValueError):m.fixture(b'',**kw)
        t.r[4:7]=[m.OUTPUT,m.KEY,65]
        with self.assertRaisesRegex(ValueError,'copy observer'):t.run(0x3934F8)
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_selected_coverage_and_real_helpers(self):
        _,raw=validated_elf(ORIGINAL);cases=m.fixtures(raw);seen={p for c in cases for p in c['visited']}
        for a,b in m.SELECTED:self.assertFalse(set(range(a,b,4))-seen)
        for index in range(4,len(m.RANGES)):self.assertTrue(any(c['invocations'][index] for c in cases))
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_both_allocations_before_failure_and_captured_ends(self):
        _,raw=validated_elf(ORIGINAL);c=m.fixture(raw,failure=1)
        self.assertEqual([e[0] for e in c['events']],[1,3,1,2])
        c=m.fixture(raw,key='',path='')
        self.assertEqual(c['result'],1)
        # Captured original preceding ':'/' bytes suppress appended delimiters.
        self.assertEqual(dict(c['changes'])[(m.NEW_KEY-m.BASE)//4]&255,0)
        self.assertEqual(dict(c['changes'])[(m.NEW_PATH-m.BASE)//4]&255,0)
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_fresh_capacity_after_free_and_lazy_failure(self):
        _,raw=validated_elf(ORIGINAL);c=m.fixture(raw,count=1,capacity=1,mutation=2)
        self.assertEqual(c['globals_expected'][1],6)
        c=m.fixture(raw,3,table=0,failure=1,count=3)
        self.assertEqual(c['globals_expected'][:3],[0,10,0])
        self.assertEqual(c['texts_expected'],c['texts_initial'])
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_leading_colon_output_alias_and_clear_before_self_copy(self):
        _,raw=validated_elf(ORIGINAL)
        c=m.fixture(raw,1,input=':TAG:Tail',output_alias=1)
        self.assertEqual(c['result'],0);self.assertTrue(c['changes'])
        c=m.fixture(raw,2,setter_alias=1)
        self.assertEqual(c['texts_expected'][0][0],0)
        self.assertEqual(c['texts_expected'][0][1:],c['texts_initial'][0][1:])

if __name__=='__main__':unittest.main()
