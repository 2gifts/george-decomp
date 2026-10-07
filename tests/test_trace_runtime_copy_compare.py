"""Fail-closed string observer guards; proprietary-image tests skip explicitly."""
import struct,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_runtime_copy_compare import CopyCompareTrace,RANGES,BASES,SIZE,fixture,parameters
from trace_geometry import RETURN

class Guards(unittest.TestCase):
    def observer(self,words=()):
        raw=bytearray(RANGES[-1][1]-0xFF000);entry=RANGES[0][0]
        for i,w in enumerate(words):struct.pack_into('<I',raw,entry-0xFF000+i*4,w)
        t=CopyCompareTrace(bytes(raw));t.r[31]=RETURN;return t
    def test_owned_initialized_alignment(self):
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASES[0],16)
        t.save(BASES[0],0x112233445566778899AABBCCDDEEFF00,16)
        self.assertEqual(t.load(BASES[0],16),0x112233445566778899AABBCCDDEEFF00)
        for a,n in ((BASES[0]+1,4),(BASES[0]+SIZE,1),(0xFFFFFFFF,4),(0x80000,4)):
            with self.assertRaisesRegex(ValueError,'unowned or unaligned'):t.load(a,n)
    def test_complete_fetch_and_entry(self):
        t=self.observer()
        for pc in (RANGES[0][0]+1,RANGES[-1][1],0):
            with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
        with self.assertRaisesRegex(ValueError,'entry'):t.run(RANGES[0][0]+4)
        t.original=b''
        with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(RANGES[0][0])
    def test_taken_and_untaken_delays(self):
        for branch in (0x10000001,0x14000001):
            for delay in (0x10000000,0x04010000,0x45000000,0x03E00008):
                with self.assertRaisesRegex(ValueError,'control in delay'):self.observer((branch,delay)).run(RANGES[0][0])
    def test_actual_jr_and_leaf_calls(self):
        self.observer((0x03E00008,0)).run(RANGES[0][0])
        t=self.observer((0x03E00008,0));t.r[31]=RETURN+4
        with self.assertRaisesRegex(ValueError,'JR31/stop'):t.run(RANGES[0][0])
        with self.assertRaisesRegex(ValueError,'leaf call'):self.observer((0x0C0E4EDD,0)).run(RANGES[0][0])
    def test_reused_packed_lanes_and_budget(self):
        t=self.observer();t.r[2]=0;t.r[3]=1
        # Actual PSUBW format: rs2,rt3,rd7,sh1,fn8. Higher word lanes
        # remain zero, unlike a scalar128 subtraction with borrowing.
        t.execute(0x70433848,RANGES[0][0]);self.assertEqual(t.r[7],0xFFFFFFFF)
        t.r[2]=0;t.r[3]=0x0101
        t.execute(0x70433A48,RANGES[0][0]);self.assertEqual(t.r[7],0xFFFF)
        with self.assertRaisesRegex(ValueError,'unsupported'):t.execute(0x00000005,RANGES[0][0])
        t.instruction_count=250000;before=(t.r[:],t.visited.copy(),t.memory.copy())
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,RANGES[0][0])
        self.assertEqual((t.r,t.visited,t.memory),before)
    def test_parameter_contract(self):
        for kw in (dict(length_left=257),dict(offset_left=16),dict(value=256),dict(routine=2),dict(routine=1,difference=0)):
            with self.assertRaises(ValueError):parameters(**kw)

@unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'exact local original required')
class OriginalContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from analyze import validated_elf
        _,cls.raw=validated_elf(ROOT/'orig/SLUS_216.68')
    def test_exact_unsigned_difference(self):
        self.assertEqual(fixture(self.raw,length_left=33,length_right=33,difference=16,value=255)['result'],(-190)&0xFFFFFFFF)
        self.assertEqual(fixture(self.raw,length_left=33,length_right=33,difference=16,value=1)['result'],64)
    def test_copy_quad_and_doubleword(self):
        for offset,width in ((0,16),(8,8)):
            row=fixture(self.raw,routine=1,offset_left=offset,offset_destination=offset,length_left=33,pattern=4)
            self.assertIn((BASES[0]+offset,width),row['reads']);self.assertIn((BASES[2]+offset,width),row['writes'])
            self.assertEqual(row['result'],BASES[2]+offset);self.assertEqual(row['calls'],[])
    def test_subtraction_subset_distinguished(self):
        row=fixture(self.raw,routine=1,length_left=33,pattern=2)
        self.assertFalse(row['signed_subtraction_subset']['long32'])
        self.assertFalse(row['signed_subtraction_subset']['long64'])
        row=fixture(self.raw,routine=1,length_left=33,pattern=0)
        self.assertTrue(row['signed_subtraction_subset']['long32']);self.assertTrue(row['signed_subtraction_subset']['long64'])
if __name__=='__main__':unittest.main()
