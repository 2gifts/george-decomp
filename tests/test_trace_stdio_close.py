"""Initialized-memory, strict-delay and original stdio mutation regressions."""
from pathlib import Path
import struct,sys,unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_stdio_close as m
from analyze import validated_elf
from trace_geometry import RETURN

ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class StdioGuards(unittest.TestCase):
    def trace(self,words=()):
        raw=bytearray(m.RANGES[-1][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,m.RANGES[0][0]-0xFF000+i*4,w)
        return m.StdioTrace(bytes(raw))

    def test_owned_aligned_initialized_memory(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.FILE,4)
        t.save(m.FILE,0xFFFFFFFF,4);self.assertEqual(t.load(m.FILE,4),0xFFFFFFFF)
        for address,size in ((m.FILE+1,4),(m.END,4),(m.BUFFER-4,4),(m.IMPURE,8),(m.FILE,3)):
            with self.assertRaises(ValueError):t.load(address,size)

    def test_full_entry_fetch_padding_return_and_instruction_bound(self):
        t=self.trace()
        for pc in (m.RANGES[0][0]-4,m.RANGES[0][0]+1,m.RANGES[0][1],m.RANGES[-1][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(m.RANGES[0][0]+4)
        with self.assertRaises(ValueError):m.StdioTrace(b'').fetch(m.RANGES[0][0])
        t=self.trace((0x03E00008,0));t.r[31]=RETURN
        with self.assertRaisesRegex(ValueError,'nonterminal'):t.run(m.RANGES[0][0])
        t.instruction_count=12000
        with self.assertRaisesRegex(ValueError,'bound'):t.execute(0,m.RANGES[0][0])

    def test_unsupported_reserved_and_signed_halfword(self):
        t=self.trace();t.r[4]=m.FILE;t.save(m.FILE,0x8000,2)
        t.execute((33<<26)|(4<<21)|(2<<16),m.RANGES[0][0]);self.assertEqual(t.r[2],0xFFFF8000)
        for w in ((6<<26)|(1<<16), (15<<26)|(1<<21),0xFFFFFFFF,
                  (4<<21)|(5<<16)|(2<<11)|0x09,0x0000000D,0x0000001B):
            with self.assertRaises(ValueError):t.execute(w,m.RANGES[0][0])
        t.r[0]=100;t.execute(0,m.RANGES[0][0]);self.assertEqual(t.r[0],0)

    def test_actual_untaken_delay_encoding_and_call_targets(self):
        for delay in ((4<<26)|(4<<21)|(5<<16)|1,(1<<26)|(4<<21)|(1<<16)|1,
                      (17<<26)|(8<<21)|1,(3<<26),0x03E00008):
            t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay))
            t.r[4:6]=[1,2]
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.RANGES[0][0])
        t=self.trace(((3<<26)|(0x123456>>2),0))
        with self.assertRaisesRegex(ValueError,'callback'):t.run(m.RANGES[0][0])

    def test_fixture_types_domains_and_observer_bounds(self):
        for kw in ({'routine':3},{'mode':7},{'count':65},{'flags':65536},{'mutation':64},
                   {'routine':1,'source':m.END,'count':1},{'routine':True}):
            with self.assertRaises(ValueError):m.fixture(b'',**kw)
        t=self.trace();t.selected_file=0
        with self.assertRaisesRegex(ValueError,'selected FILE'):t.callback(m.CLOSE)
        with self.assertRaises(ValueError):m.StdioTrace(b'',mutation=-1)
        # Packed flags/descriptor remain scalars even when numerically pointer-like.
        self.assertIn(m.CTX0+0x1E4+2*88+12,m.PACKED_SHORT_CELLS)
        self.assertNotIn(m.CTX0+0x1E4+2*88+12,m.POINTER_CELLS)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_original_whole_selected_coverage_and_independent_copy_invariants(self):
        _,raw=validated_elf(ORIGINAL);cases=m.fixtures(raw)
        visited=set(p for c in cases for p in c['visited'])
        for a,b in m.RANGES[:2]:self.assertFalse(set(range(a,b,4))-visited)
        for c in cases:
            if c['routine']!=1:continue
            words=dict(c['initial']);before=bytearray(m.WORDS*4)
            for i,v in words.items():struct.pack_into('<I',before,i*4,v)
            expected=bytearray(before)
            # Sequential byte loop, including forward-overlap propagation.
            for i in range(c['count']):expected[c['destination']-m.BUFFER+i]=expected[c['source']-m.BUFFER+i]
            got=bytearray(m.WORDS*4)
            for i,v in c['expected']:struct.pack_into('<I',got,i*4,v)
            self.assertEqual(got,expected)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_original_buffer_retention_null_inactive_and_negative_close(self):
        _,raw=validated_elf(ORIGINAL)
        for result in (0,-1,-2147483648):
            c=m.fixture(raw,flags=0x88,closeret=result);before=dict(c['initial']);after=dict(c['expected'])
            self.assertEqual(c['result'],0 if result==0 else 0xFFFFFFFF)
            self.assertEqual(after.get((m.FILE+12-m.BUFFER)//4,0)&65535,0)
            for off in (16,48,68):self.assertEqual(after[(m.FILE+off-m.BUFFER)//4],before[(m.FILE+off-m.BUFFER)//4])
        for mode in (0,1):
            c=m.fixture(raw,mode=mode,flags=0)
            self.assertEqual(c['result'],0);self.assertEqual(c['initial'],c['expected']);self.assertFalse(c['events'])

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_original_close_callback_and_context_reloaded_after_flush(self):
        _,raw=validated_elf(ORIGINAL)
        c=m.fixture(raw,mutation=1|4|8|16|32,writeret=2,closeret=-1)
        self.assertEqual(c['events'][7*3],3) # Three writes then newly selected alternate close.
        self.assertEqual(c['events'][7*3+1],m.BUFFER+0x60)
        self.assertEqual(c['events'][7*3+6],m.CTX1)
        self.assertEqual(c['impure'],m.CTX1)
        after=dict(c['expected'])
        for off in (16,48,68):self.assertEqual(after[(m.FILE+off-m.BUFFER)//4],m.ALT)
        self.assertEqual(after.get((m.FILE+12-m.BUFFER)//4,0)&65535,0)
        self.assertTrue(all(c['invocations'][i]>0 for i in (0,2)))
        cancelled=m.fixture(raw,mutation=2)
        self.assertTrue(cancelled['events']);self.assertTrue(all(cancelled['events'][i]==0 for i in range(0,len(cancelled['events']),7)))

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_original_real_initialization_and_fread_copy_closure(self):
        _,raw=validated_elf(ORIGINAL)
        c=m.fixture(raw,mode=6)
        self.assertGreater(c['invocations'][3],0);self.assertEqual(c['invocations'][4],3)
        self.assertEqual(c['events'][0],2)
        self.assertEqual(c['events'][4],10) # stdout/err standard initialization was executed.
        read=m.fixture(raw,routine=2,count=32,flags=4,destination=m.PAYLOAD+100)
        self.assertEqual(read['result'],32);self.assertEqual(read['invocations'][1],1)
        self.assertEqual(read['invocations'][5],1);self.assertFalse(read['events'])

if __name__=='__main__':unittest.main()
