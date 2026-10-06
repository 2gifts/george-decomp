"""Guards for the bounded camera decoder extension; no original game required."""
import math
from pathlib import Path
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_camera_motion import CameraTrace
from trace_geometry import word


class CameraDecoderTests(unittest.TestCase):
    def test_and_masks_low_word_and_resets_zero(self):
        trace=CameraTrace(b'')
        trace.r[4],trace.r[5]=0x1234567800000000,0xFFFFFFFFFFFFFFFF
        trace.execute((4<<21)|(5<<16)|(6<<11)|0x24,0x100)
        self.assertEqual(trace.r[6],0)
        trace.r[4]=0x12345678
        trace.execute((4<<21)|(5<<16)|0x24,0x104)
        self.assertEqual(trace.r[0],0)
        self.assertEqual(trace.instruction_count,2)
        with self.assertRaisesRegex(ValueError,'AND encoding'):
            trace.execute((4<<21)|(5<<16)|(1<<6)|0x24,0)

    def test_bne_branches_and_sign_extends_offset(self):
        trace=CameraTrace(b'')
        instruction=(5<<26)|(4<<21)|(5<<16)|0xFFFF
        trace.r[4],trace.r[5]=1,0
        self.assertEqual(trace.execute(instruction,0x100),(0x100,False))
        trace.r[5]=1
        self.assertEqual(trace.execute(instruction,0x100),(None,False))

    def test_max_observes_ee_signed_encoding_order(self):
        trace=CameraTrace(b'')
        instruction=(0x11<<26)|(16<<21)|(2<<16)|(1<<11)|(3<<6)|0x28
        for first,second,expected in ((-2.0,-1.0,-1.0),(-1.0,2.0,2.0),
                                      (2.0,1.0,2.0),(-0.0,0.0,0.0),(0.0,-0.0,0.0)):
            trace.f[1],trace.f[2]=first,second
            trace.execute(instruction,0)
            self.assertEqual(word(trace.f[3]),word(expected))
        trace.f[1]=math.inf
        with self.assertRaisesRegex(ValueError,'finite model'):
            trace.execute(instruction,0)

    def test_less_comparisons_and_invalid_condition_forms(self):
        trace=CameraTrace(b'')
        base=(0x11<<26)|(16<<21)|(2<<16)|(1<<11)
        for first,second,strict,inclusive in ((-1,0,True,True),(1,1,False,True),(2,1,False,False)):
            trace.f[1],trace.f[2]=first,second
            trace.execute(base|0x34,0)
            self.assertEqual(trace.condition,strict)
            trace.execute(base|0x36,0)
            self.assertEqual(trace.condition,inclusive)
        with self.assertRaisesRegex(ValueError,'comparison condition code'):
            trace.execute(base|(1<<6)|0x34,0)
        with self.assertRaisesRegex(ValueError,'branch condition code'):
            trace.execute((0x11<<26)|(8<<21)|(4<<16),0)

    def test_unknown_memory_calls_and_code_rejected(self):
        trace=CameraTrace(b'')
        with self.assertRaises(KeyError):trace.load(0x20000,4)
        with self.assertRaisesRegex(ValueError,'uninitialized memory'):trace.save(0x20000,0,4)
        with self.assertRaisesRegex(ValueError,'address windows'):trace.save(0x11000,0,4)
        with self.assertRaisesRegex(ValueError,'unreviewed controlled camera call'):trace.library_call(0x123456)
        with self.assertRaisesRegex(ValueError,'unreviewed camera instruction'):trace.fetch(0x2B5EF0)
        with self.assertRaisesRegex(ValueError,'outside original image'):trace.fetch(0x2B59F8)

    def test_budget_counts_custom_instructions(self):
        trace=CameraTrace(b'')
        trace.instruction_count=3000
        with self.assertRaisesRegex(ValueError,'exceeded its bound'):
            trace.execute((5<<26),0)

    def test_reused_special_and_cop1_operand_forms_rejected(self):
        trace=CameraTrace(b'')
        instructions=((4<<21)|(5<<16)|(6<<11)|(1<<6)|0x21,
                      (31<<21)|(1<<16)|8,
                      (31<<21)|(1<<11)|8,
                      (0x11<<26)|(4<<21)|1,
                      (0x11<<26)|1,
                      (0x11<<26)|(16<<21)|(1<<16)|6,
                      (0x11<<26)|(16<<21)|(1<<16)|7,
                      (0x11<<26)|(16<<21)|(1<<6)|0x32)
        for instruction in instructions:
            with self.subTest(instruction=instruction):
                with self.assertRaises(ValueError):trace.execute(instruction,0)

    def test_ee_sqrt_reads_ft_with_aliased_output(self):
        trace=CameraTrace(b'')
        trace.f[1]=9.0
        instruction=(0x11<<26)|(16<<21)|(1<<16)|(1<<6)|4
        trace.execute(instruction,0)
        self.assertEqual(trace.f[1],3.0)
        with self.assertRaisesRegex(ValueError,'SQRT source form'):
            trace.execute(instruction|(2<<11),0)


if __name__=='__main__':unittest.main()
