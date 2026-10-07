"""Strict synthetic decoder guards plus genuine navigation/alias regressions."""
from pathlib import Path
import struct,sys,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import trace_buffer_records as m
from analyze import validated_elf
from trace_geometry import RETURN
ORIGINAL=m.ROOT/'orig/SLUS_216.68'

class BufferGuards(unittest.TestCase):
    def trace(self,words=()):
        raw=bytearray(m.RANGES[-1][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,m.RANGES[0][0]-0xFF000+4*i,w)
        return m.BufferTrace(bytes(raw))

    def test_initialized_owned_memory(self):
        t=self.trace()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(m.OWNER,4)
        t.save(m.OWNER,0xFFFFFFFF,4);self.assertEqual(t.load(m.OWNER,4),0xFFFFFFFF)
        for address,size in ((m.OWNER+1,4),(m.END,4),(m.BUFFER-4,4),(m.OWNER,2)):
            with self.assertRaises(ValueError):t.load(address,size)

    def test_fetch_entries_padding_and_terminal(self):
        t=self.trace((0x03E00008,0));t.r[31]=RETURN
        for pc in (m.RANGES[0][0]-4,m.RANGES[0][0]+1,m.RANGES[0][1]):
            with self.assertRaises(ValueError):t.fetch(pc)
        with self.assertRaises(ValueError):t.run(m.RANGES[0][0]+4)
        with self.assertRaisesRegex(ValueError,'terminal'):t.run(m.RANGES[0][0])
        with self.assertRaisesRegex(ValueError,'outside original'):m.BufferTrace(b'').fetch(m.RANGES[0][0])

    def test_reserved_encodings_delay_and_zero(self):
        t=self.trace()
        for w in ((6<<26)|(1<<16),0xFFFFFFFF,(4<<21)|(5<<16)|(2<<11)|9,(17<<26)):
            with self.assertRaises(ValueError):t.execute(w,m.RANGES[0][0])
        for delay in ((4<<26)|1,(3<<26),0x03E00008):
            t=self.trace(((4<<26)|(4<<21)|(5<<16)|1,delay));t.r[4:6]=[1,2]
            with self.assertRaisesRegex(ValueError,'control in delay'):t.run(m.RANGES[0][0])
        t.r[0]=4;t.execute(0,m.RANGES[0][0]);self.assertEqual(t.r[0],0)

    def test_helper_domain_and_instruction_bounds(self):
        t=self.trace();t.r[4:7]=[m.DEST,m.TEXTS[0]-1,8]
        with self.assertRaisesRegex(ValueError,'bulk'):t.run(m.RANGES[7][0])
        t.instruction_count=1600
        with self.assertRaisesRegex(ValueError,'instruction bound'):t.execute(0,m.RANGES[0][0])
        with self.assertRaises(ValueError):t.run(0x123456)

    def test_fixture_domains(self):
        for kw in ({'routine':True},{'routine':6},{'index':-1},{'index':9},{'shift':4},{'bound':33},
                   {'routine':2,'count':33},{'routine':3,'index':10},{'routine':5,'alias':3,'bound':8}):
            with self.assertRaises(ValueError):m.fixture(b'',**kw)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_full_coverage_and_helpers(self):
        _,raw=validated_elf(ORIGINAL);cases=m.fixtures(raw);seen={p for c in cases for p in c['visited']}
        for a,b in m.RANGES[:7]:self.assertFalse(set(range(a,b,4))-seen)
        self.assertTrue(any(c['invocations'][7] and c['invocations'][6] for c in cases))

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_signed_wrap_and_next_endpoint(self):
        _,raw=validated_elf(ORIGINAL)
        for i,e,want in ((0,0,0),(0,1,1),(2,3,3),(4,3,4),(0xFFFFFFFF,0,0),(0x80000000,0x80000002,0x80000001)):
            c=m.fixture(raw,1,i,e);values={**dict(c['initial']),**dict(c['changes'])}
            self.assertEqual(values[(m.OWNER+32-m.BUFFER)//4],want)
        c=m.fixture(raw,0,0x40000000);self.assertEqual(dict(c['changes'])[(m.OWNER+32-m.BUFFER)//4],0x3FFFFFFF)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_capture_before_counter_store_and_signed_delete(self):
        _,raw=validated_elf(ORIGINAL)
        for count in (0,0xFFFFFFFF,0x80000000):
            c=m.fixture(raw,2,count=count);self.assertFalse(c['changes'])
        c=m.fixture(raw,2,count=2,alias=2);values={**dict(c['initial']),**dict(c['changes'])}
        self.assertEqual(values[(m.CHILD+8-m.BUFFER)//4],(m.CHILD+8)&0xFFFF00FF)
        self.assertEqual(values[(m.CHILD+4-m.BUFFER)//4],1)

    @unittest.skipUnless(ORIGINAL.is_file(),'requires locally supplied original ELF')
    def test_fresh_data_after_real_byte_store(self):
        _,raw=validated_elf(ORIGINAL);c=m.fixture(raw,5,bound=4,alias=3)
        self.assertEqual(c['events'][4:],[2,m.NEW_TEXT,0,0])
        values={**dict(c['initial']),**dict(c['changes'])}
        self.assertEqual(values[(m.CHILD+8-m.BUFFER)//4],m.NEW_TEXT)
        self.assertEqual(values[(m.CHILD+4-m.BUFFER)//4],3)

if __name__=='__main__':unittest.main()
