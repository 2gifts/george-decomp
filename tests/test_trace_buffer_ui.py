"""Synthetic decoder guards; local original-dependent contract regressions."""
import sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import trace_buffer_ui as ui
ORIGINAL=ROOT/'orig/SLUS_216.68'

class UiGuards(unittest.TestCase):
    def test_initialized_owned_memory_and_exact_widths(self):
        t=ui.UiTrace(b'')
        for a,n in ((ui.UI,4),(ui.STACK[0],8)):
            with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(a,n)
        for a,n in ((ui.UI+1,4),(ui.END,1),(ui.GLOBALS[0]-1,1)):
            with self.assertRaisesRegex(ValueError,'unowned'):t.save(a,0,n)
        t.save(ui.UI,0x81234567,4);t.r[4]=ui.UI
        t.execute(0x8C820000,0) # LW v0,0(a0), full signed64 result
        self.assertEqual(t.r[2],0xFFFFFFFF81234567)
        t.execute(0x94820000,0) # LHU v0,0(a0)
        self.assertEqual(t.r[2],0x4567)
    def test_declared_readonly_windows_are_not_general_image_memory(self):
        t=ui.UiTrace(b'')
        with self.assertRaisesRegex(ValueError,'image bound'):t.load(0x43A6E8,8)
        with self.assertRaisesRegex(ValueError,'unowned'):t.load(0x43A800,1)
        with self.assertRaisesRegex(ValueError,'unowned'):t.save(0x43A6E8,0,1)
    def test_reserved_encodings_and_division_traps_fail(self):
        t=ui.UiTrace(b'')
        for w in (0x0000000D,0x18810000,0x70A61408,0x46020806,0x48800000):
            with self.assertRaises(ValueError):t.execute(w,0)
        t.r[4]=123;t.r[5]=0
        with self.assertRaisesRegex(ValueError,'division by zero'):t.execute(0x0085001B,0)
    def test_encoded_control_in_delay_is_rejected_even_if_untaken(self):
        class Synthetic(ui.UiTrace):
            def fetch(self,pc):return self.words[pc]
        for delay in (0x10850000,0x04810000,0x45000000,0x03E00008,0x0C000000):
            t=Synthetic(b'');a=ui.SELECTED[0][0];t.words={a:0x10000001,a+4:delay}
            t.r[4]=1;t.r[5]=2;t.r[31]=ui.RETURN
            with self.assertRaisesRegex(ValueError,'encoded control'):t.run(a)
    def test_full64_and_vector_lane_operations_keep_values(self):
        t=ui.UiTrace(b'');t.r[3]=0x112233445566778899AABBCCDDEEFF00;t.r[4]=0xFFEEDDCCBBAA99887766554433221100
        t.execute((28<<26)|(3<<21)|(4<<16)|(2<<11)|(14<<6)|9,0)
        self.assertEqual(t.r[2],0x99AABBCCDDEEFF007766554433221100)
        t.execute((28<<26)|(3<<21)|(4<<16)|(2<<11)|(14<<6)|41,0)
        self.assertEqual(t.r[2],0xFFEEDDCCBBAA99881122334455667788)
        t.r[3]=0;t.r[4]=int.from_bytes(bytes(range(16)),'little')
        t.execute((28<<26)|(3<<21)|(4<<16)|(2<<11)|(9<<6)|8,0)
        self.assertEqual(t.r[2],int.from_bytes(bytes((-i)&255 for i in range(16)),'little'))
        for n in (0x4EFFFFFF,0xCEFFFFFF):self.assertEqual(ui.cvtw(n),int(ui.scalar(n))&ui.MASK)
        self.assertEqual(ui.cvtw(0x4F000000),0x7FFFFFFF)
        self.assertEqual(ui.cvtw(0xCF000000),0x80000000)
        self.assertEqual(ui.cvtw(ui.word(-3.75)),0xFFFFFFFD)
        self.assertEqual(ui.cvtw(ui.word(3.75)),3)
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_all_selected_paths_and_overlap_contract(self):
        _,raw=ui.validated_elf(ORIGINAL);cases=ui.fixtures(raw)
        visited=set(pc for c in cases for pc in c['visited'])
        for a,b in ui.SELECTED:
            missing=set(range(a,b,4))-visited
            self.assertEqual(missing,{0x21651C} if a==0x2163E8 else set())
        long=ui.fixture(raw,text=b'o'*75+b'.al',table=1,completion=1)
        self.assertGreater(long['instructions'],10000)
        self.assertTrue(any(a==0x2CDA20 for a,n in long['invocations']))
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_callback_fresh_secondary_and_differing_next_capture(self):
        _,raw=ui.validated_elf(ORIGINAL)
        direct=ui.fixture(raw,text=b'one.al',table=1,mutation=3)
        directkeys=[e[1] for e in direct['events'] if e[0]==0x2162D0]
        self.assertIn(0xABC00004,directkeys);self.assertNotIn(0xABC00002,directkeys)
        generic=ui.fixture(raw,text=b'al',mutation=4)
        generickeys=[e[1] for e in generic['events'] if e[0]==0x2162D0]
        # Generic original callback retains the already captured following node.
        import zlib
        self.assertEqual(generickeys,[zlib.crc32(b'one'),zlib.crc32(b'two')])
        secondary=ui.fixture(raw,text=b'one.al',table=1,mutation=2)
        keys=[e[1] for e in secondary['events'] if e[0]==0x2162D0]
        self.assertIn(0xABC00004,keys);self.assertNotIn(0xABC00003,keys)
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_formatter_full_varargs_and_update_gate_store_order(self):
        _,raw=ui.validated_elf(ORIGINAL)
        c=ui.fixture(raw,2);formats=[e for e in c['events'] if e[0]==0x23C908]
        self.assertEqual(formats[0][9:11],[0xFEDCBA98,0xFFFFFFFF])
        self.assertEqual(formats[0][11:13],[50,0])
        for disabled,flags,pressed in ((4,0x80000001,1),(0,1,1),(0,0x80000001,0)):
            c=ui.fixture(raw,1,key=65,disabled=disabled,flags=flags,pressed=pressed,delta=0.04)
            self.assertEqual(c['events'],[])
            self.assertIn([(ui.UI+4-ui.BUFFER)//4,ui.word(0.04)],c['changes'])

if __name__=='__main__':unittest.main()
