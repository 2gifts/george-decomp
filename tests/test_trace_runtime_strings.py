"""Strict initialized/encoding/budget guards; original-dependent tests skip."""
import struct,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_runtime_strings import RuntimeStringsTrace,RANGES,BASES,SIZE,fixture,initial_buffers,defined_subtraction_subset
from trace_geometry import RETURN

class Guards(unittest.TestCase):
    def observer(self,words=()):
        entry=RANGES[0][0];raw=bytearray(RANGES[0][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,entry-0xFF000+i*4,w)
        t=RuntimeStringsTrace(bytes(raw));t.r[31]=RETURN;return t
    def test_initialized_owned_alignment(self):
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASES[0],16)
        t.save(BASES[0],0xAABBCCDD,4);self.assertEqual(t.load(BASES[0],4),0xAABBCCDD)
        for a,n in ((BASES[0]+1,4),(BASES[0]+SIZE,1),(0,1),(0xFFFFFFFF,4)):
            with self.assertRaisesRegex(ValueError,'unowned or unaligned'):t.load(a,n)
    def test_fetch_scope_and_image(self):
        t=self.observer()
        for pc in (RANGES[0][0]+1,RANGES[-1][1],0):
            with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
        t.original=b''
        with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(RANGES[0][0])
    def test_delay_control_both_outcomes(self):
        for outer in (0x10000001,0x14000001):
            for delay in (0x10000000,0x04010000,0x45000000,0x03E00008):
                with self.assertRaisesRegex(ValueError,'control in delay'):self.observer((outer,delay)).run(RANGES[0][0])
    def test_actual_jr_return_and_no_call(self):
        self.observer((0x03E00008,0)).run(RANGES[0][0])
        t=self.observer((0x03E00008,0));t.r[31]=RETURN+4
        with self.assertRaisesRegex(ValueError,'JR31/stop'):t.run(RANGES[0][0])
        t=self.observer((0x03C00008,0));t.r[30]=RETURN
        with self.assertRaisesRegex(ValueError,'outside full body'):t.run(RANGES[0][0])
        with self.assertRaisesRegex(ValueError,'unreviewed leaf call'):self.observer((0x0C000000,0)).run(RANGES[0][0])
    def test_pxor_full_lanes_aliases_zero_and_unsupported(self):
        a=0xFFFF00001111222200000000AAAABBBB;b=0x123456789ABCDEF08765432100001111
        for rd in (2,9,0):
            t=self.observer();t.r[2]=a;t.r[9]=b;w=0x704904C9|(rd<<11);t.execute(w,0x3933E4)
            self.assertEqual(t.r[rd],0 if rd==0 else a^b);self.assertEqual(t.instruction_count,1)
        for invalid in (0x704914C9^3,0x704914C9^(3<<6),0x704914C9^(1<<26)):
            with self.assertRaisesRegex(ValueError,'unsupported'):self.observer().execute(invalid,0x3933E4)
    def test_budget_all_paths_preserve_counter(self):
        for word in (0,0x704914C9,0x70081EE9):
            t=self.observer();t.instruction_count=249999;t.execute(word,0x3933E4)
            self.assertEqual(t.instruction_count,250000);before=t.r[:];visited=t.visited.copy()
            with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(word,0x3933E4)
            self.assertEqual((t.instruction_count,t.r,t.visited),(250000,before,visited))
    def test_fixture_and_signed_arithmetic_domain(self):
        with self.assertRaisesRegex(ValueError,'count'):fixture(b'',routine=0,count=257)
        p=dict(routine=4,count=16,character=0,offset1=0,offset2=0,offsetd=0,length1=-1,length2=-1,dest_length=0,difference=-1,difference_value=0,pattern=2)
        b=initial_buffers(p);self.assertFalse(defined_subtraction_subset(p,b,4));self.assertFalse(defined_subtraction_subset(p,b,8))
        p['pattern']=0;b=initial_buffers(p);self.assertTrue(defined_subtraction_subset(p,b,4));self.assertTrue(defined_subtraction_subset(p,b,8))

@unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'exact local original required')
class OriginalContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from analyze import validated_elf
        _,cls.raw=validated_elf(ROOT/'orig/SLUS_216.68')
    def test_zero_counts_and_whole_write_extent(self):
        for routine in range(5):
            row=fixture(self.raw,routine=routine,count=0)
            self.assertEqual(row['changes'],[])
            if routine!=2:self.assertEqual(row['reads'],[])
        row=fixture(self.raw,routine=1,count=33,offsetd=1,character=-1)
        self.assertEqual(sum(n for a,n in row['writes']),33)
    def test_padding_lookahead_and_initial_conservative_detector(self):
        row=fixture(self.raw,routine=3,count=17)
        self.assertIn((BASES[0]+16,16),row['reads'])
        ascii_row=fixture(self.raw,routine=4,count=32,pattern=0)
        high_row=fixture(self.raw,routine=4,count=32,pattern=1)
        self.assertFalse(any(n==16 for a,n in ascii_row['writes']))
        self.assertTrue(any(n==16 for a,n in high_row['writes']))
        self.assertEqual(sum(n for a,n in high_row['writes']),32)
if __name__=='__main__':unittest.main()
