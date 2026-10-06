"""Scope/callee guards for transform tracing, without original game files."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from trace_camera_transform import CameraTransformTrace,TRANSFORM


class TransformScopeTests(unittest.TestCase):
    def test_existing_decoder_is_inherited_without_replacement(self):
        trace=CameraTransformTrace(b'')
        trace.r[4],trace.r[5]=0xFFFFFFFF,0x12345678
        trace.execute((4<<21)|(5<<16)|(6<<11)|0x24,0)
        self.assertEqual(trace.r[6],0x12345678)

    def test_scope_excludes_padding_other_helpers_and_unknown_image(self):
        trace=CameraTransformTrace(b'')
        for pc in (0x29A168,0x29A260,0x29A504,0x299BE0,0x2B59F8,0x29A101):
            with self.subTest(pc=pc):
                with self.assertRaisesRegex(ValueError,'unreviewed camera transform instruction'):trace.fetch(pc)
        with self.assertRaisesRegex(ValueError,'outside original image'):trace.fetch(0x29A100)

    def test_unknown_calls_and_bad_allocation_size_fail(self):
        trace=CameraTransformTrace(b'')
        with self.assertRaisesRegex(ValueError,'unreviewed controlled transform call'):trace.library_call(0x123456)
        trace.r[4]=12
        with self.assertRaisesRegex(ValueError,'allocation size'):trace.library_call(0x2AEC28)

    def test_bad_cache_rotation_or_uncaptured_angle_fail(self):
        trace=CameraTransformTrace(b'')
        trace.r[4]=TRANSFORM+4
        with self.assertRaisesRegex(ValueError,'uncaptured transform cache'):trace.library_call(0x299BE0)
        trace.r[5]=TRANSFORM+0x10
        with self.assertRaisesRegex(ValueError,'rotation argument'):trace.library_call(0x2A1E78)
        with self.assertRaisesRegex(ValueError,'uncaptured projection angle'):trace.library_call(0x29C090)

    def test_unknown_output_memory_remains_rejected(self):
        trace=CameraTransformTrace(b'')
        trace.r[4],trace.r[5]=0x20000,TRANSFORM+0x1C
        with self.assertRaisesRegex(ValueError,'uninitialized memory'):trace.library_call(0x2A1E78)
        trace.r[4]=0x21000
        with self.assertRaisesRegex(ValueError,'address windows'):trace.library_call(0x2A1E78)


if __name__=='__main__':unittest.main()
