import sys
from pathlib import Path
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_camera_basis import CameraBasisTrace,TABLE,TABLE_END,double_value


def special(rs,rt,rd,shamt,fn):
    return (rs<<21)|(rt<<16)|(rd<<11)|(shamt<<6)|fn


class CameraBasisGuards(unittest.TestCase):
    def trace(self):return CameraBasisTrace(b'')

    def test_full_width_moves_and_threshold_shifts(self):
        t=self.trace();t.r[16]=0xBFEFAE1480000000
        t.execute(special(16,0,4,0,0x2D),0x299E10)
        self.assertEqual(t.r[4],0xBFEFAE1480000000)
        t.r[5]=0xFFBE
        t.execute(special(0,5,5,16,0x38),0x299E30)
        t.execute((0xD<<26)|(5<<21)|(5<<16)|0xB852,0x299E34)
        t.execute(special(0,5,5,30,0x38),0x299E38)
        self.assertEqual(t.r[5],0x3FEFAE1480000000)
        t.r[0]=99
        t.execute(special(5,0,0,0,0x2D),0x299E40)
        self.assertEqual(t.r[0],0)

    def test_signed_full_width_branches(self):
        t=self.trace();t.r[2]=0xFFFFFFFFFFFFFFFF
        bgez=(1<<26)|(2<<21)|(1<<16)|4
        blez=(6<<26)|(2<<21)|4
        self.assertEqual(t.execute(bgez,0x299E18),(None,False))
        self.assertEqual(t.execute(blez,0x299E44),(0x299E58,False))
        t.r[2]=0
        self.assertEqual(t.execute(bgez,0x299E18),(0x299E2C,False))
        t.r[2]=0x100000000
        self.assertEqual(t.execute(blez,0x299E44),(None,False))

    def test_unsigned_gate_and_word_shift(self):
        t=self.trace();t.r[3]=0xFFFFFFFFFFFFFFFF
        sltiu=(0xB<<26)|(3<<21)|(2<<16)|5
        t.execute(sltiu,0x299C00);self.assertEqual(t.r[2],0)
        t.r[3]=4;t.execute(sltiu,0x299C00);self.assertEqual(t.r[2],1)
        t.r[3]=0x80000000
        t.execute(special(0,3,3,0,0),0x299C0C)
        self.assertEqual(t.r[3],0xFFFFFFFF80000000)

    def test_reject_reserved_new_forms(self):
        t=self.trace()
        for instruction in (special(1,3,3,2,0),special(1,5,5,16,0x38),
                            special(16,0,4,1,0x2D),(1<<26)|(2<<21)|(3<<16),
                            (6<<26)|(2<<21)|(1<<16)):
            with self.subTest(instruction=instruction):
                with self.assertRaises(ValueError):t.execute(instruction,0x299E18)

    def test_table_extent_identity_and_writes(self):
        t=self.trace()
        for address,size in ((TABLE+1,4),(TABLE,8),(TABLE_END-2,4)):
            with self.assertRaises(ValueError):t.load(address,size)
        with self.assertRaises(ValueError):t.load(TABLE,4)
        with self.assertRaises(KeyError):t.load(TABLE_END,4)
        with self.assertRaises(ValueError):t.save(TABLE,0,4)

    def test_reject_unreviewed_code_memory_and_calls(self):
        t=self.trace()
        for address in (0x299BD8,0x29A100,0x29A109,0x2A3538):
            with self.assertRaises(ValueError):t.fetch(address)
        with self.assertRaises(ValueError):t.library_call(0x29A308)
        with self.assertRaises(KeyError):t.library_call(0x2A3538)
        with self.assertRaises(ValueError):double_value(0x7FF0000000000000)


if __name__=='__main__':unittest.main()
