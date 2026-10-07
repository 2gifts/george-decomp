import struct,sys,unittest
from pathlib import Path
R=Path(__file__).resolve().parents[1];sys.path.insert(0,str(R/'tools'))
from analyze import validated_elf
from trace_vector_insert import InsertTrace,RANGES,BASE,END,GLOBALS,STACK,H1,H2,fixture
from trace_geometry import RETURN

class Guards(unittest.TestCase):
    def trace(self,image=b''):
        return InsertTrace(image,dict(routine=0,count=1,capacity=1,position=0,value_index=-1,cache=1,failures=0,mutation=0))
    def test_whole_scope_and_image(self):
        t=self.trace()
        for address in (RANGES[0][0]+1,RANGES[0][1],0x100C30):
            with self.assertRaises(ValueError):t.fetch(address)
        with self.assertRaises(ValueError):t.fetch(RANGES[0][0])
    def test_initialized_owned_memory(self):
        t=self.trace()
        for address,n in ((BASE,4),(BASE+1,4),(END-4,8),(GLOBALS[0],8)):
            with self.assertRaises(ValueError):t.load(address,n)
        t.save(BASE,0x80000000,4);self.assertEqual(t.load(BASE,4),0x80000000)
        t.save(STACK[0],0xCAFE123456781234CAFE123456781234,16)
        self.assertEqual(t.load(STACK[0],16),0xCAFE123456781234CAFE123456781234)
    def test_exact_budget_and_traps(self):
        t=self.trace();t.instruction_count=29999;t.execute(0,RANGES[0][0])
        with self.assertRaises(ValueError):t.execute(0,RANGES[0][0])
        t=self.trace()
        for w in (0x0000000D,0x0000003D,0x0020000F,0x0000001B):
            with self.assertRaises(ValueError):t.execute(w,RANGES[0][0])
    def test_delay_control_both_outcomes(self):
        a=RANGES[0][0];image=bytearray(a-0xFF000+708)
        struct.pack_into('<II',image,a-0xFF000,0x10850002,0x03E00008)
        for equal in (False,True):
            t=self.trace(bytes(image));t.r[4]=1;t.r[5]=1 if equal else 2;t.r[31]=RETURN
            with self.assertRaisesRegex(ValueError,'delay'):t.run(a)
    def test_actual_terminal_and_supported_external(self):
        a=RANGES[0][0];image=bytearray(a-0xFF000+708)
        struct.pack_into('<II',image,a-0xFF000,0x03E00008,0)
        t=self.trace(bytes(image));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'actual'):t.run(a)
        with self.assertRaises(ValueError):t.external(0x3581E0)
        with self.assertRaises(ValueError):t.external(0x3712E8)

@unittest.skipUnless((R/'orig/SLUS_216.68').is_file(),'original ELF not available')
class OriginalContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.original=validated_elf(R/'orig/SLUS_216.68')[1]
    def test_changing_handlers_and_failed_allocations(self):
        c=fixture(self.original,count=17,capacity=17,position=0,cache=5,failures=3)
        self.assertEqual([x[1] for x in c['events'] if x[0]==3],[1,2,2])
        self.assertEqual(c['globals_expected'][0],H2)
        self.assertEqual(len([x for x in c['events'] if x[0]==1]),4)
    def test_partial_refill_salvage_and_count_reload(self):
        c=fixture(self.original,count=2,capacity=2,position=0,cache=3)
        self.assertFalse(any(x[0]==1 for x in c['events']))
        c=fixture(self.original,count=2,capacity=2,position=0,cache=4,failures=1)
        self.assertEqual(len([x for x in c['events'] if x[0]==1]),1)
        self.assertGreater(c['globals_expected'][2],BASE)
    def test_input_capture_after_prefix_copy(self):
        c=fixture(self.original,count=17,capacity=17,position=0,cache=5,mutation=2)
        self.assertIn([1024,0x7F001234],c['changes'])
        self.assertEqual(c['changes'][1],[1,0x21044])
    def test_explicit_bad_domain_and_unsigned_compare(self):
        for p in (dict(count=0,capacity=1),dict(count=1,capacity=2,position=1),dict(count=64,capacity=67)):
            with self.assertRaises(ValueError):fixture(self.original,**p)
        self.assertEqual(fixture(self.original,routine=1,count=2,capacity=2,position=0,cache=1,mutation=1)['result'],0)
        self.assertEqual(fixture(self.original,routine=1,count=1,capacity=1,position=0,cache=1,mutation=2)['result'],1)
if __name__=='__main__':unittest.main()
