import sys
from pathlib import Path
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_property_pack import PropertyPackTrace,BUFFER,END,NODES,RETURN


class PackGuards(unittest.TestCase):
    def trace(self):
        t=PropertyPackTrace(b'',0,0,0)
        for p in range(BUFFER,END):t.memory[p]=0x5A
        return t

    def test_byte_halfword_truncation_and_signed_offset(self):
        t=self.trace();t.r[16]=BUFFER+8;t.r[2]=0x12345678
        t.execute((0x28<<26)|(16<<21)|(2<<16)|0xFFFD,0x2B9FF0)
        self.assertEqual(t.load(BUFFER+5,1),0x78)
        t.execute((0x29<<26)|(16<<21)|(2<<16)|6,0x2BA010)
        self.assertEqual(t.load(BUFFER+14,2),0x5678)
        self.assertEqual(t.load(BUFFER+16,1),0x5A)

    def test_memory_and_other_table_permissions_are_rejected(self):
        t=self.trace()
        for address,size in ((BUFFER-1,1),(END-1,2),(0x4455D0,4),(0x4455A0,4)):
            with self.assertRaises(ValueError):t.load(address,size)
            with self.assertRaises(ValueError):t.save(address,0,size)
        del t.memory[BUFFER]
        with self.assertRaises(KeyError):t.load(BUFFER,1)
        with self.assertRaises(ValueError):t.save(BUFFER,0,1)

    def test_only_complete_owned_code_scopes(self):
        t=self.trace()
        for address in (0x2B9F34,0x2BA10C,0x29A8B4,0x29A508,0x2BA071):
            with self.assertRaises(ValueError):t.fetch(address)
        with self.assertRaises(ValueError):t.run(0x29A508)
        with self.assertRaises(ValueError):t.fetch(0x2B9F38)
        t.r[16]=RETURN
        t.fetch=lambda pc:(16<<21)|8 if pc==0x2B9F38 else 0
        with self.assertRaises(ValueError):t.run(0x2B9F38)
        t.r[31]=0x2BA070
        t.fetch=lambda pc:0x03E00008 if pc==0x2B9F38 else 0
        with self.assertRaises(ValueError):t.run(0x2B9F38)

    def test_unknown_calls_and_invalid_copy_extents(self):
        t=self.trace()
        with self.assertRaises(ValueError):t.library_call(0x2AEC2C)
        t.r[4],t.r[5],t.r[6]=BUFFER,BUFFER+32,129
        with self.assertRaises(ValueError):t.library_call(0x3934F8)
        t.r[6]=40
        with self.assertRaises(ValueError):t.library_call(0x3934F8)
        t.r[4]=BUFFER
        with self.assertRaises(ValueError):t.library_call(0x29C648)

    def test_nested_next_executes_body_and_link_delay(self):
        t=self.trace();t.r[4],t.r[31]=BUFFER+16,0x2BA02C
        t.fetch=lambda pc:{0x29A890:0x03E00008,0x29A894:0x24020007}[pc]
        t.library_call(0x29A890)
        self.assertEqual(t.r[2],7)
        self.assertEqual(t.instruction_count,2)
        self.assertEqual(t.calls[4],1)
        self.assertEqual(t.events,[4,16])

    def test_inherited_reserved_forms_stay_strict(self):
        t=self.trace()
        for instruction in ((16<<21)|(4<<11)|(1<<6)|0x2D,
                            (17<<26)|(4<<21)|(1<<16)|1):
            with self.assertRaises(ValueError):t.execute(instruction,0x2B9F50)


if __name__=='__main__':unittest.main()
