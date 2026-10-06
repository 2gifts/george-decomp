import sys
from pathlib import Path
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_property_records import PropertyTrace,TABLE,TABLE_END,scalar,word


def special(rs,rt,rd,shift,fn):return (rs<<21)|(rt<<16)|(rd<<11)|(shift<<6)|fn


class PropertyGuards(unittest.TestCase):
    def trace(self):return PropertyTrace(b'')

    def test_byte_conversion_and_reserved_forms(self):
        t=self.trace();convert=(17<<26)|(20<<21)|(1<<11)|(2<<6)|0x20
        for value in (0,1,128,255):
            t.f[1]=scalar(value);t.execute(convert,0x29A744)
            self.assertEqual(t.f[2],float(value))
        t.f[1]=scalar(256)
        with self.assertRaises(ValueError):t.execute(convert,0x29A744)
        for instruction in (convert|(1<<16),convert^1):
            with self.assertRaises(ValueError):t.execute(instruction,0x29A744)

    def test_three_operand_multiply_and_unsigned_order(self):
        t=self.trace();t.r[4]=0xFFFFFFFF;t.r[2]=24
        t.execute(special(4,2,3,0,0x18),0x29A584)
        self.assertEqual(t.r[3],0xFFFFFFE8)
        t.execute(special(4,2,3,0,0x2B),0x29A558)
        self.assertEqual(t.r[3],0)
        with self.assertRaises(ValueError):t.execute(special(4,2,3,1,0x18),0x29A584)

    def test_shift_sign_and_masks(self):
        t=self.trace();t.r[3]=0xABCD1234
        t.execute(special(0,3,2,8,3),0x29A748)
        self.assertEqual(t.r[2],0xFFABCD12)
        t.execute((0xC<<26)|(2<<21)|(2<<16)|0xFF,0x29A74C)
        self.assertEqual(t.r[2],0x12)
        t.execute(special(0,3,4,24,2),0x29A7F4)
        self.assertEqual(t.r[4],0xAB)
        with self.assertRaises(ValueError):t.execute(special(1,3,2,8,3),0x29A748)

    def test_likely_branch_annul_and_taken(self):
        t=self.trace();t.r[3]=4;t.r[5]=5
        bnel=(0x15<<26)|(3<<21)|(5<<16)|0xFFF9
        self.assertEqual(t.execute(bnel,0x29A570),(0x29A558,False))
        t.r[3]=5;self.assertEqual(t.execute(bnel,0x29A570),(None,True))

    def test_callback_link_and_reserved_forms(self):
        t=self.trace();t.r[2]=0xF0000080
        self.assertEqual(t.execute(special(2,0,31,0,9),0x29A658),(0xF0000080,False))
        self.assertEqual(t.r[31],0x29A660)
        for insn in (special(2,1,31,0,9),special(2,0,0,0,9),special(2,0,31,1,9)):
            with self.assertRaises(ValueError):t.execute(insn,0x29A658)
        with self.assertRaises(ValueError):t.library_call(0xF0000084)

    def test_table_identity_scope_and_unknown_memory(self):
        t=self.trace()
        for address,size in ((TABLE+1,4),(TABLE,8),(TABLE_END-2,4)):
            with self.assertRaises(ValueError):t.load(address,size)
        with self.assertRaises(ValueError):t.load(TABLE,4)
        with self.assertRaises(ValueError):t.load(0x4455A0,4)
        with self.assertRaises(KeyError):t.load(TABLE_END,4)
        with self.assertRaises(ValueError):t.save(TABLE,0,4)
        with self.assertRaises(KeyError):t.library_call(0x393B74)

    def test_body_scope_and_reused_guard(self):
        t=self.trace()
        for address in (0x29A504,0x29A888,0x29A8B4,0x29A8E4,0x299BE0):
            with self.assertRaises(ValueError):t.fetch(address)
        with self.assertRaises(ValueError):t.execute(special(16,0,4,1,0x2D),0x29A514)
        # The inherited MTC1 form requires its reserved low bits to be zero.
        with self.assertRaises(ValueError):t.execute((17<<26)|(4<<21)|(1<<16)|1,0x29A730)


if __name__=='__main__':unittest.main()
