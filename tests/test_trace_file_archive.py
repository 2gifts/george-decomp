"""Meaningful decoder/domain guards; original-dependent checks skip cleanly."""
import struct,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from trace_file_archive import ArchiveTrace,BASE,STACK,SELECTED,COUNT,MODE,SLOT,MAP,ALT_MAP,RESULT,ALT_RESULT,fixture
from trace_geometry import RETURN

class DecoderGuards(unittest.TestCase):
    def observer(self,words=()):
        entry=SELECTED[0][0];raw=bytearray(SELECTED[0][1]-0xFF000)
        for i,w in enumerate(words):struct.pack_into('<I',raw,entry-0xFF000+i*4,w)
        t=ArchiveTrace(bytes(raw),dict(mutation=0,failure=0));t.r[29]=0x80000;t.r[31]=RETURN
        return t
    def test_initialized_owned_memory(self):
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'uninitialized'):t.load(BASE,4)
        t.save(BASE,0x81234567,4);self.assertEqual(t.load(BASE,4),0x81234567)
        for address,size in ((BASE+1,4),(0,4),(0xFFFFFFFF,4),(STACK[1],4)):
            with self.assertRaisesRegex(ValueError,'unowned or unaligned'):t.load(address,size)
    def test_fetch_geometry(self):
        t=self.observer()
        for pc in (SELECTED[0][0]+1,SELECTED[0][1],0):
            with self.assertRaisesRegex(ValueError,'unreviewed'):t.fetch(pc)
        t.original=b''
        with self.assertRaisesRegex(ValueError,'image bound'):t.fetch(SELECTED[0][0])
    def test_control_delay_taken_and_untaken(self):
        for branch in (0x10000001,0x14000001):
            with self.assertRaisesRegex(ValueError,'control in delay'):self.observer((branch,0x03E00008)).run(SELECTED[0][0])
    def test_actual_terminal_return(self):
        with self.assertRaisesRegex(ValueError,'terminal'):self.observer((0x03E00008,0)).run(SELECTED[0][0])
        t=self.observer((0x03C00008,0));t.r[30]=RETURN
        with self.assertRaisesRegex(ValueError,'outside full body'):t.run(SELECTED[0][0])
    def test_external_allowlist_and_readonly_write(self):
        t=self.observer()
        with self.assertRaisesRegex(ValueError,'unreviewed archive external'):t.external(0x123456)
        with self.assertRaisesRegex(ValueError,'unowned'):t.save(0x4473C0,0,1)
        with self.assertRaisesRegex(ValueError,'unsupported registry SPECIAL'):t.execute(0x0000003D,SELECTED[0][0])

@unittest.skipUnless((ROOT/'orig/SLUS_216.68').is_file(),'local original required')
class OriginalContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from analyze import validated_elf
        _,cls.raw=validated_elf(ROOT/'orig/SLUS_216.68')
    def test_close_fresh_count_and_untouched_field(self):
        c=fixture(self.raw,mutation=3,encoded=0x80000000,count=2)
        # Cache runs a second time after close, so its second controlled write
        # supersedes the SDK-close write before the selected fresh decrement.
        self.assertEqual(c['globals_expected'][:2],[0x12345677,0x80000000])
        changes=dict(c['changes']);self.assertEqual(changes[4],0xBEEFF00D)
        self.assertEqual([r[0] for r in c['events']],[1,2,1])
        self.assertEqual(c['events'][1][1],0x7FFFFFFF)
    def test_mode_bypass_and_zero_encoding(self):
        self.assertFalse(fixture(self.raw,mode=1,encoded=0xFFFFFFFF)['events'])
        self.assertFalse(fixture(self.raw,mode=0,encoded=0)['events'])
        self.assertEqual(fixture(self.raw,count=0)['globals_expected'][0],0xFFFFFFFF)
    def test_lookup_collision_and_fresh_context(self):
        self.assertEqual(fixture(self.raw,1,collision=1)['result'],RESULT)
        self.assertEqual(fixture(self.raw,1,table=0,mutation=4)['result'],ALT_RESULT)
        self.assertEqual(fixture(self.raw,1,match=0,collision=1)['result'],0)
    def test_unbounded_invalid_domains_are_not_fabricated(self):
        for kwargs in (dict(input=''),dict(input=':only'),dict(input='x'*31),dict(input='nonascii\u00e9'),dict(count=-1)):
            with self.assertRaises(ValueError):fixture(self.raw,1,**kwargs)

if __name__=='__main__':unittest.main()
