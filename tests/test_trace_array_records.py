"""Decoder rejection guards and authentic array mutation/wrapping regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_array_records as m
from analyze import validated_elf
from trace_geometry import RETURN
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class ArrayGuards(unittest.TestCase):
    def trace(self,words=()):
        raw=bytearray(m.RANGES[-1][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,m.RANGES[0][0]-0xFF000+4*i,w)
        return m.ArrayTrace(bytes(raw),dict(routine=0,mutation=0,success=1))

    def test_initialized_and_owned_memory(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.ARRAY,4)
        t.save(m.ARRAY,0xFFFFFFFF,4);self.assertEqual(t.load(m.ARRAY,4),0xFFFFFFFF)
        for a,n in ((m.ARRAY+1,4),(m.END,4),(m.BASE-4,4),(m.ARRAY,2),(m.GLOBAL+4,4)):
            with self.assertRaises(ValueError):t.load(a,n)

    def test_whole_entry_and_real_return(self):
        t=self.trace((0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'JR31'):t.run(m.RANGES[0][0])
        for a in (m.RANGES[0][0]+4,0x1234):
            with self.assertRaises(ValueError):t.run(a)
        for a in (m.RANGES[0][0]-4,m.RANGES[0][0]+1,m.RANGES[0][1]):
            with self.assertRaises(ValueError):t.fetch(a)
        with self.assertRaises(ValueError):t.run(m.RANGES[0][0],depth=4)

    def test_reserved_encodings_zero_and_multiply(self):
        t=self.trace();pc=m.RANGES[0][0]
        for w in ((15<<26)|(1<<21),(1<<26)|(4<<16),0x12|(1<<21),0x18|(1<<6),0x23|(1<<6),4|(1<<6),0xFFFFFFFF):
            with self.assertRaises(ValueError):t.execute(w,pc)
        t.r[4]=0xFFFFFFFF;t.r[5]=0x80000000
        t.execute((4<<21)|(5<<16)|0x18,pc);t.execute((2<<11)|0x12,pc)
        self.assertEqual(t.r[2],0x80000000)
        t.r[0]=7;t.execute(0,pc);self.assertEqual(t.r[0],0)

    def test_taken_and_untaken_delay_rejection_and_annul(self):
        for equal in (False,True):
            for delay in (0x03E00008,(3<<26),(4<<26)|1):
                t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay));t.r[4:6]=[1,1 if equal else 2]
                with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.RANGES[0][0])
        t=self.trace();t.r[4:6]=[1,2]
        self.assertEqual(t.execute((20<<26)|(4<<21)|(5<<16)|1,m.RANGES[0][0]),(None,True))

    def test_domains_and_bounds(self):
        t=self.trace();t.r[4:7]=[m.OLD,m.OLD+4,8]
        with self.assertRaisesRegex(ValueError,'nonoverlap'):t.run(m.RANGES[9][0])
        t.r[4:7]=[m.NEW,m.OLD,65]
        with self.assertRaisesRegex(ValueError,'bounded'):t.run(m.RANGES[9][0])
        t.instruction_count=1600
        with self.assertRaises(ValueError):t.execute(0,m.RANGES[0][0])
        for kw in ({'routine':True},{'index':-1},{'routine':9},{'mutation':32},{'alias':1},{'success':2}):
            with self.assertRaises(ValueError):m.fixture(b'',**kw)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_all_selected_instructions_and_real_bulk_copy(self):
        _,raw=validated_elf(ORIGINAL);cases=m.fixtures(raw);seen={p for c in cases for p in c['visited']}
        for a,b in m.RANGES[:9]:self.assertFalse(set(range(a,b,4))-seen)
        self.assertTrue(any(c['invocations'][9] and c['invocations'][10] for c in cases))
        self.assertTrue(any(64 in c['events'][3::4] for c in cases))

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_signed_indices_and_wrapped_products(self):
        _,raw=validated_elf(ORIGINAL)
        self.assertEqual(m.fixture(raw,0,0xFFFFFFFF)['result'],0)
        self.assertEqual(m.fixture(raw,0,0,used=0x80000000)['result'],0)
        self.assertEqual(m.fixture(raw,0,4,stride=0x40000000,used=5)['result'],m.OLD)
        self.assertEqual(m.fixture(raw,7,0xFFFFFFFF,stride=0,used=0)['result'],0xFFFFFFFF)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_fresh_after_copy_free_and_publication(self):
        _,raw=validated_elf(ORIGINAL)
        for routine in (5,6):
            c=m.fixture(raw,routine,1,capacity=2,used=2,mutation=2)
            self.assertEqual(c['events'][8:12],[2,m.ALT,1,2])
        c=m.fixture(raw,6,capacity=2,used=2,mutation=8)
        self.assertEqual(dict(c['changes'])[(m.ARRAY+8-m.BASE)//4],6)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_header_alias_and_release_store_order(self):
        _,raw=validated_elf(ORIGINAL)
        for alias in (1,2):
            c=m.fixture(raw,6,used=0,alias=alias)
            self.assertEqual(c['events'][:4],[3,m.ARRAY+(4 if alias==1 else 8),m.ELEMENT,4])
        c=m.fixture(raw,4,mutation=4)
        self.assertEqual(c['events'][-4:],[2,m.ARRAY,0,0])

if __name__=='__main__':unittest.main()
