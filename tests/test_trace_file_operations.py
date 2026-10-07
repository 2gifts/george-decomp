"""Strict file observer bounds and original alias/kernel-contract regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_file_operations as m
from analyze import validated_elf
from trace_geometry import RETURN
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class FileGuards(unittest.TestCase):
    def trace(self,words=()):
        raw=bytearray(max(m.RANGES[-1][1],m.READONLY[-1][1])-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,m.SELECTED[0][0]-0xFF000+4*i,w)
        return m.FileTrace(bytes(raw),dict(slot=0,mutation=0,registry_fail=0,failures=0,routine=0,read_results=[],sdk_result=0,position=0,end=0))
    def test_owned_initialized_readonly_memory(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.PATH,1)
        t.save(m.PATH,0x7F,1);self.assertEqual(t.load(m.PATH,1),0x7F)
        for a,n in ((m.END,4),(m.BASE-1,1),(m.PATH,4),(m.COUNT+4,4),(m.SLOTS+560,4),(m.TEXT_GLOBALS[2]+128,1)):
            with self.assertRaises(ValueError):t.load(a,n)
        with self.assertRaises(ValueError):t.save(0x445650,1,4)
        self.assertEqual(t.load(0x445650,4),0)
    def test_whole_entry_terminal_stop_and_depth(self):
        t=self.trace((0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'terminal'):t.run(m.SELECTED[0][0])
        with self.assertRaises(ValueError):t.run(m.SELECTED[0][0]+4)
        with self.assertRaises(ValueError):t.run(m.SELECTED[0][0],depth=11)
        for pc in (m.SELECTED[0][0]-4,m.SELECTED[0][0]+1,m.SELECTED[0][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
        # A call encoded with RETURN as its destination cannot masquerade as JR.
        t=self.trace(((3<<26)|((RETURN>>2)&0x3FFFFFF),0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'external'):t.run(m.SELECTED[0][0])
    def test_reserved_signed_mmi_zero_and_budget(self):
        t=self.trace();pc=m.SELECTED[0][0]
        for w in (0xFFFFFFFF,(15<<26)|(1<<21),(28<<26)|8|(2<<6),(17<<26),0x12|(1<<21),12|(1<<6)):
            with self.assertRaises(ValueError):t.execute(w,pc)
        t.r[4]=0;t.r[5]=(1<<128)-1
        t.execute((28<<26)|(4<<21)|(5<<16)|(2<<11)|(1<<6)|8,pc)
        self.assertEqual(t.r[2],sum(1<<(32*i) for i in range(4)))
        t.r[0]=5;t.execute(0,pc);self.assertEqual(t.r[0],0)
        t.instruction_count=30000
        with self.assertRaisesRegex(ValueError,'bound'):t.execute(0,pc)
    def test_outcome_independent_delay_and_likely_annul(self):
        for equal in (False,True):
            for delay in ((4<<26)|1,(1<<26)|(1<<16)|1,(17<<26)|(8<<21)|1,0x03E00008):
                t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay));t.r[4:6]=[1,1 if equal else 2]
                with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.SELECTED[0][0])
        t=self.trace();t.r[4:6]=[1,2]
        self.assertEqual(t.execute((20<<26)|(4<<21)|(5<<16)|1,m.SELECTED[0][0]),(None,True))
    def test_actual_syscall_pc_selector_packet_and_token(self):
        t=self.trace();pc=m.STUBS[0][0]+4;t.r[3]=100;t.r[4]=0
        t.instruction_count=30000
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(12,pc)
        self.assertEqual(t.instruction_count,30000)
        t.instruction_count=0
        with self.assertRaisesRegex(ValueError,'syscall'):t.execute(12,m.SELECTED[0][0])
        t.r[3]=99
        with self.assertRaisesRegex(ValueError,'syscall'):t.execute(12,pc)
        t.r[3]=100;t.r[4]=1
        with self.assertRaisesRegex(ValueError,'cache mode'):t.execute(12,pc)
        t.r[3]=119;t.r[4]=m.BASE;t.r[5]=1
        with self.assertRaisesRegex(ValueError,'packet'):t.execute(12,m.STUBS[2][0]+4)
        t.r[3]=118;t.r[4]=0
        with self.assertRaisesRegex(ValueError,'token'):t.execute(12,m.STUBS[1][0]+4)
    def test_explicit_observer_domains_and_unknown_external(self):
        for kw in ({'routine':True},{'routine':10},{'slot':20},{'path':':tag'},{'path':'\x80'},
                   {'routine':6,'mode':1,'path':''},{'count':0x80000000},{'read_results':[0x80000000]}):
            with self.assertRaises(ValueError):m.fixture(b'',**kw)
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'external'):t.external(0x123456)
        t.r[4:7]=[m.BASE,m.BASE,4]
        with self.assertRaisesRegex(ValueError,'memcpy'):t.run(0x3934F8)
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_whole_selected_and_actual_kernel_helper_coverage(self):
        _,raw=validated_elf(ORIGINAL);cases=m.fixtures(raw);seen={pc for c in cases for pc in c['visited']}
        missing=set().union(*(set(range(a,b,4))-seen for a,b in m.SELECTED))
        self.assertEqual(missing,{0x2B0754,0x2B0FEC}) # Annulled/no-fallthrough NOPs.
        for index in list(range(10,20))+list(range(21,30)):
            self.assertTrue(any(c['invocations'][index] for c in cases),index)
        self.assertFalse(any(c['invocations'][20] for c in cases)) # memcpy compiled but not reached.
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_archive_alias_fresh_slot_failure_and_wrapping_count(self):
        _,raw=validated_elf(ORIGINAL)
        c=m.fixture(raw,6,path='TAG:File/One',mode=1,archive_alias=1,open_count=0xFFFFFFFF)
        self.assertEqual(c['result'],m.SLOTS);self.assertEqual(c['expected_globals'][0],0)
        # Record aliases slot+12; field10 publishes captured zero, then the
        # second getter reads it through the freshly captured record pointer.
        self.assertEqual(c['expected_slots'][2],0);self.assertEqual(c['expected_slots'][4],0)
        c=m.fixture(raw,6,mutation=3)
        self.assertEqual(c['result'],0);self.assertEqual(c['expected_globals'][0],0)
        self.assertEqual([e[0] for e in c['events']],[2,3,2,2,5,2,2,5,2,2,5,2])
    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_dma_retry_packet_mutation_negative_read_and_free(self):
        _,raw=validated_elf(ORIGINAL);c=m.fixture(raw,count=32,mutation=5)
        packets=[e for e in c['events'] if e[0]==8]
        self.assertEqual(packets[0][2:5],[0x11000000,16,0])
        self.assertEqual(packets[1][2:5],[0x12345678,0x76543210,0x55])
        self.assertEqual(packets[-1][4],0x55);self.assertEqual(c['result'],32)
        c=m.fixture(raw,count=32,read_results=(-1,))
        self.assertEqual(c['result'],0xFFFFFFFF);self.assertEqual(c['events'][-1][:2],[1,m.ALLOCATION])
        self.assertTrue(any(e[0]==8 and e[3]==0 for e in c['events']))

if __name__=='__main__':unittest.main()
