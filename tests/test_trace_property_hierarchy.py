import sys
from pathlib import Path
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_property_hierarchy import HierarchyTrace,BUFFER,END,ENTRIES,RETURN


class HierarchyGuards(unittest.TestCase):
    def trace(self):
        t=HierarchyTrace(b'')
        for p in range(BUFFER,END):t.memory[p]=0x5A
        return t

    def test_subtract_low_word_and_reserved_shift(self):
        t=self.trace();t.r[20],t.r[18]=5,0xFFFFFFFF
        code=(20<<21)|(18<<16)|(6<<11)|0x23
        t.execute(code,0x2B9AA8);self.assertEqual(t.r[6],6)
        t.r[20],t.r[18]=0,1;t.execute(code,0x2B9AA8);self.assertEqual(t.r[6],0xFFFFFFFF)
        with self.assertRaises(ValueError):t.execute(code|(1<<6),0x2B9AA8)

    def test_whole_code_scopes_and_unavailable_original(self):
        t=self.trace()
        for address in (0x2B9924,0x2B99AC,0x2B9AF4,0x2B9C04,0x2B9E34,0x2B9F34,0x2B9929,0x2B9440):
            with self.assertRaises(ValueError):t.fetch(address)
        with self.assertRaises(ValueError):t.fetch(ENTRIES[0])
        with self.assertRaises(ValueError):t.run(0x2B9440)

    def test_unknown_memory_calls_and_string_extent(self):
        t=self.trace()
        for address,size in ((BUFFER-1,1),(END-1,2),(0x4455D0,4)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,0,size)
        del t.memory[BUFFER]
        with self.assertRaises(KeyError):t.load(BUFFER,1)
        with self.assertRaises(ValueError):t.save(BUFFER,0,1)
        with self.assertRaises(ValueError):t.library_call(0x393A2C)
        with self.assertRaises(ValueError):t.bytestring(BUFFER+4)

    def test_real_return_and_delay_guards(self):
        t=self.trace();t.r[16]=RETURN
        t.fetch=lambda pc:(16<<21)|8 if pc==ENTRIES[0] else 0
        with self.assertRaises(ValueError):t.run(ENTRIES[0])
        t.r[31]=ENTRIES[1];t.fetch=lambda pc:0x03E00008 if pc==ENTRIES[0] else 0
        with self.assertRaises(ValueError):t.run(ENTRIES[0])
        t.r[31]=RETURN;t.fetch=lambda pc:0x03E00008
        with self.assertRaises(ValueError):t.run(ENTRIES[0])

    def test_nested_recursion_uses_actual_delay_and_saved_link(self):
        t=self.trace();entry,child=ENTRIES[:2]
        # Authored JAL and return words only, not any original body bytes.
        words={entry:(31<<21)|(16<<11)|0x2D,
               entry+4:(3<<26)|(child>>2),entry+8:0,
               entry+12:(16<<21)|(31<<11)|0x2D,entry+16:0x03E00008,entry+20:0,
               child:0x03E00008,child+4:0x24020007}
        t.r[31]=RETURN;t.fetch=lambda pc:words[pc]
        t.run(entry)
        self.assertEqual(t.r[2],7);self.assertEqual(t.instruction_count,8)

    def test_inherited_jalr_and_special_forms_stay_strict(self):
        t=self.trace()
        for code in ((19<<21)|(1<<16)|(31<<11)|9,
                     (19<<21)|(31<<11)|(1<<6)|9,
                     (16<<21)|(4<<11)|(1<<6)|0x2D):
            with self.assertRaises(ValueError):t.execute(code,0x2B9B9C)


if __name__=='__main__':unittest.main()
