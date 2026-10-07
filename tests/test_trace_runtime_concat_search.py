"""Strict synthetic guards; local proprietary original tests explicitly skip."""
import struct,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_runtime_concat_search import ConcatSearchTrace,RANGES,BASES,SIZE,STACK,fixture,parameters,initial_buffers
from trace_geometry import RETURN

class Guards(unittest.TestCase):
    def observer(self,words=()):
        raw=bytearray(RANGES[-1][1]-0xFF000);entry=RANGES[0][0]
        for i,w in enumerate(words):struct.pack_into('<I',raw,entry-0xFF000+i*4,w)
        t=ConcatSearchTrace(bytes(raw));t.r[31]=RETURN;return t
    def test_initialized_owned_alignment_and_stack(self):
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASES[0],16)
        t.save(STACK-32,0x112233445566778899AABBCCDDEEFF00,16)
        self.assertEqual(t.load(STACK-32,16),0x112233445566778899AABBCCDDEEFF00)
        for a,n in ((BASES[0]+1,4),(BASES[0]+SIZE,1),(STACK,1),(STACK-33,1),(0xFFFFFFFF,4)):
            with self.assertRaisesRegex(ValueError,'unowned or unaligned'):t.load(a,n)
    def test_complete_fetch_and_entry_bounds(self):
        t=self.observer()
        for pc in (RANGES[0][0]+1,RANGES[-1][1],0):
            with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
        with self.assertRaisesRegex(ValueError,'entry'):t.run(RANGES[0][0]+4)
        t.original=b''
        with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(RANGES[0][0])
    def test_delay_control_taken_and_untaken(self):
        for outer in (0x10000001,0x14000001):
            for delay in (0x10000000,0x04010000,0x45000000,0x03E00008):
                with self.assertRaisesRegex(ValueError,'control in delay'):self.observer((outer,delay)).run(RANGES[0][0])
    def test_actual_jr_stop_and_reject_unreviewed_call(self):
        self.observer((0x03E00008,0)).run(RANGES[0][0])
        t=self.observer((0x03E00008,0));t.r[31]=RETURN+4
        with self.assertRaisesRegex(ValueError,'JR31/stop'):t.run(RANGES[0][0])
        with self.assertRaisesRegex(ValueError,'unreviewed call'):self.observer((0x0C0E4EDD,0)).run(RANGES[0][0])
    def test_decoder_budget_unsupported_and_register_zero(self):
        t=self.observer();t.execute(0x704914C9,RANGES[0][0]);self.assertEqual(t.r[0],0)
        with self.assertRaisesRegex(ValueError,'unsupported'):t.execute(0x00000005,RANGES[0][0])
        t.instruction_count=250000;before=(t.r[:],t.visited.copy())
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,RANGES[0][0])
        self.assertEqual((t.r,t.visited),before)
    def test_fixture_initialized_contract(self):
        for kw in (dict(length=257),dict(offset_source=16),dict(character=1<<31),dict(match=0),dict(routine=2)):
            with self.assertRaises(ValueError):parameters(**kw)
        p=parameters(routine=1,length=17,character=0,match=5)
        with self.assertRaisesRegex(ValueError,'shorten'):initial_buffers(p)

@unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'exact local original required')
class OriginalContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from analyze import validated_elf
        _,cls.raw=validated_elf(ROOT/'orig/SLUS_216.68')
    def test_copy_call_arguments_and_whole_saved_lanes(self):
        row=fixture(self.raw,length=33,dest_length=17,offset_source=1,offset_destination=8,pattern=4)
        self.assertEqual(row['calls'],[dict(pc=0x39386C,target=0x393B74,arguments=[BASES[2]+25,BASES[0]+1])])
        self.assertEqual([n for a,n in row['writes'] if STACK-32<=a<STACK],[16,16])
        self.assertEqual(row['result'],BASES[2]+8)
    def test_low_byte_search_first_match_and_nul(self):
        for c in (0,256,-256,-2147483648):
            self.assertEqual(fixture(self.raw,routine=1,length=17,character=c)['result'],BASES[0]+17)
        for c in (128,-128,384):
            self.assertEqual(fixture(self.raw,routine=1,length=33,character=c,match=8)['result'],BASES[0]+8)
        self.assertEqual(fixture(self.raw,routine=1,length=33,character=1)['result'],0)
    def test_wide_lookahead_and_empty_append(self):
        row=fixture(self.raw,length=0,dest_length=0)
        self.assertIn((BASES[2],16),row['reads']);self.assertEqual(row['changes'],[])
        row=fixture(self.raw,routine=1,length=17,character=1)
        self.assertIn((BASES[0]+16,16),row['reads']);self.assertEqual(row['writes'],[])
if __name__=='__main__':unittest.main()
