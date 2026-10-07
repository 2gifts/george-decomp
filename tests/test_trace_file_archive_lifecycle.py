"""Strict lifecycle encoding/ownership controls, clean original-absent skips."""
import struct,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_file_archive_lifecycle import LifecycleTrace,BASE,STACK,SELECTED,CTX,MAP,ALTMAP,ALTBUCKETS,NODE,KEYOUT,HEAP,GLOBALS,fixture
from trace_geometry import RETURN

class DecoderGuards(unittest.TestCase):
    def observer(self,words=()):
        entry=SELECTED[0][0];raw=bytearray(SELECTED[0][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,entry-0xFF000+i*4,w)
        t=LifecycleTrace(bytes(raw),dict(scenario=0,prefix='',mutation=0,failure=0,retry=0))
        t.r[29]=0x80000;t.r[31]=RETURN;return t
    def test_initialized_owned_memory(self):
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASE,4)
        t.save(BASE,0x81234567,4);self.assertEqual(t.load(BASE,4),0x81234567)
        for address,size in ((BASE+1,4),(0,4),(0xFFFFFFFF,4),(STACK[1],4)):
            with self.assertRaisesRegex(ValueError,'unowned or unaligned'):t.load(address,size)
        with self.assertRaisesRegex(ValueError,'unowned'):t.save(0x4473D0,0,2)
    def test_fetch_geometry_and_image(self):
        t=self.observer()
        for pc in (SELECTED[0][0]+1,SELECTED[7][1],0):
            with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
        t.original=b''
        with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(SELECTED[0][0])
        with self.assertRaisesRegex(ValueError,'image bound'):t.load(0x4473D0,2)
    def test_delay_control_both_outcomes(self):
        for branch in (0x10000001,0x14000001):
            for delay in (0x10000000,0x04010000,0x45000000,0x03E00008):
                with self.assertRaisesRegex(ValueError,'control in delay'):self.observer((branch,delay)).run(SELECTED[0][0])
    def test_actual_terminal_return_and_external(self):
        with self.assertRaisesRegex(ValueError,'terminal'):self.observer((0x03E00008,0)).run(SELECTED[0][0])
        t=self.observer((0x03C00008,0));t.r[30]=RETURN
        with self.assertRaisesRegex(ValueError,'outside full body'):t.run(SELECTED[0][0])
        t=self.observer((0x03E00008,0));t.r[31]=RETURN+4
        with self.assertRaisesRegex(ValueError,'JR31/stop'):t.run(SELECTED[0][0])
        with self.assertRaisesRegex(ValueError,'unreviewed lifecycle external'):t.external(0x123456)
    def test_pcpyh_distinct_lanes_alias_zero_and_reserved(self):
        t=self.observer();value=0xAAAABBBBCCCC1234DDDDEEEEFFFF5678
        t.r[8]=value;word=0x70081EE9;t.execute(word,0x3936CC)
        self.assertEqual(t.r[3],0x12341234123412345678567856785678)
        t.r[8]=value;t.execute((word&~(31<<11))|(8<<11),0x3936CC)
        self.assertEqual(t.r[8],0x12341234123412345678567856785678)
        t.execute(word&~(31<<11),0x3936CC);self.assertEqual(t.r[0],0)
        for invalid in (word|(1<<21),word^(1<<6),word^1,0x0000003D):
            with self.assertRaisesRegex(ValueError,'reserved|unsupported'):t.execute(invalid,0x3936CC)
    def test_exact_scoped_budget_and_traps(self):
        for word in (0,0x70081EE9):
            t=self.observer();t.instruction_count=249999;t.execute(word,0x3936CC)
            self.assertEqual(t.instruction_count,250000)
            with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(word,0x3936CC)
            self.assertEqual(t.instruction_count,250000)
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'division by zero'):t.execute(0x0085001B,0x100000)
        with self.assertRaisesRegex(ValueError,'BREAK'):t.execute(0x000001CD,0x100000)

@unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'local original required')
class OriginalContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from analyze import validated_elf
        _,cls.raw=validated_elf(ROOT/'orig/SLUS_216.68')
    def test_pop_captures_value_then_fresh_aliases(self):
        c=fixture(self.raw,5,key_alias=2);self.assertEqual(c['result'],0x20700)
        self.assertEqual(dict(c['changes'])[(NODE+8-BASE)//4],0x12345678)
        c=fixture(self.raw,5,key_alias=3)
        self.assertEqual(dict(c['changes'])[(MAP+12-BASE)//4],ALTBUCKETS)
        self.assertEqual(dict(c['changes'])[(ALTBUCKETS+4-BASE)//4],0)
        c=fixture(self.raw,5,mutation=2);self.assertEqual(dict(c['changes'])[(MAP+8-BASE)//4],6)
    def test_opposite_retries_and_persistent_recursion(self):
        for routine in (0,1):
            c=fixture(self.raw,routine,scenario=3,retry=1,prefix='')
            reads=[e[1] for e in c['events'] if e[0]==6]
            self.assertGreaterEqual(reads.count(10),2);self.assertIn(20,reads);self.assertIn(21,reads)
            self.assertEqual(c['globals_expected'][3],0)
        c=fixture(self.raw,4,scenario=3,retry=1,prefix='')
        self.assertGreater(c['instructions'],30000);self.assertLess(c['instructions'],250000)
        self.assertEqual(c['slots_expected'][4],c['slots_initial'][4])
    def test_domain_limits_without_production_guards(self):
        for kwargs in (dict(path='x'*25),dict(prefix='nonascii\u00e9'),dict(bucket_count=9),dict(depth=-1)):
            with self.assertRaises(ValueError):fixture(self.raw,**kwargs)
        c=fixture(self.raw,6,bucket_count=0x40000000)
        self.assertEqual(c['result'],0x23000)
        self.assertEqual(dict(c['changes'])[(0x23004-BASE)//4],0x40000000)

if __name__=='__main__':unittest.main()
