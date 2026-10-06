"""Encoding-based delay rejection across bounded decoders; synthetic words only."""
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from trace_geometry import Trace, RETURN, is_control_transfer
from trace_camera_motion import CameraTrace
from trace_resource_base import ResourceBaseTrace
import trace_resource_base as resource
import trace_path_sampling as sampling
import trace_path_curves as curves


class DelayEncodingTests(unittest.TestCase):
    def test_branch_jump_encodings_are_outcome_independent(self):
        words = [op << 26 for op in (2, 3, 4, 5, 6, 7, 20, 21, 22, 23)]
        words += [(1 << 26) | (rt << 16) for rt in (0, 1, 2, 3, 16, 17, 18, 19)]
        words += [(31 << 21) | fn for fn in (8, 9)]
        words += [(op << 26) | (8 << 21) | (rt << 16)
                  for op in (16, 17, 18) for rt in range(4)]
        for word in words:
            with self.subTest(word=hex(word)):
                self.assertTrue(is_control_transfer(word))

    def test_arithmetic_loads_and_regimm_traps_are_not_branches(self):
        words = [0, 0x27BDFFF0, 0x8C820000, 0xAC820000, 0x00851021,
                 (17 << 26) | (16 << 21), (17 << 26) | (4 << 21)]
        words += [(1 << 26) | (rt << 16) for rt in (8, 9, 10, 11, 12, 14)]
        for word in words:
            with self.subTest(word=hex(word)):
                self.assertFalse(is_control_transfer(word))

    def runners(self):
        return ((Trace, 0x100), (CameraTrace, 0x100),
                (ResourceBaseTrace, resource.ENTRIES[0]),
                (sampling.PathTrace, sampling.ENTRIES[0]),
                (curves.CurveTrace, curves.ENTRIES[0]))

    def test_untaken_encoded_branch_in_delay_rejected_before_execution(self):
        delay = (4 << 26) | (4 << 21) | (5 << 16) | 2
        for cls, entry in self.runners():
            for outer in (0x03E00008, delay):
                with self.subTest(decoder=cls.__name__, outer=hex(outer)):
                    trace = cls(b"")
                    trace.r[4], trace.r[5], trace.r[31] = 1, 2, RETURN
                    words = {entry: outer, entry + 4: delay,
                             entry + 8: 0x03E00008, entry + 12: 0}
                    trace.fetch = words.__getitem__
                    with self.assertRaisesRegex(ValueError, "transfer in .*delay"):
                        trace.run(entry)
                    self.assertEqual(trace.instruction_count, 1)

    def test_annulled_likely_delay_is_never_fetched_or_executed(self):
        for cls, entry in self.runners():
            with self.subTest(decoder=cls.__name__):
                trace = cls(b"")
                trace.r[4], trace.r[5], trace.r[31] = 1, 2, RETURN
                if cls in (Trace, CameraTrace):
                    branch = (17 << 26) | (8 << 21) | (3 << 16) | 2
                    trace.condition = False
                else:
                    branch = (20 << 26) | (4 << 21) | (5 << 16) | 2
                # Deliberately absent entry+4: an annulled delay is not fetched.
                words = {entry: branch, entry + 8: 0x03E00008, entry + 12: 0}
                trace.fetch = words.__getitem__
                trace.run(entry)
                self.assertEqual(trace.instruction_count, 3)


if __name__ == "__main__":
    unittest.main()
