"""Strict whole-body, finite VU/MMI, soft-ABI and alias guard regressions."""
from pathlib import Path
import math
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import trace_record_collision as model
from trace_geometry import RETURN, word


class RecordGuards(unittest.TestCase):
    def trace(self, instructions=()):
        image = bytearray(0x2A3640 - 0xFF000)
        for i, instruction in enumerate(instructions):
            struct.pack_into('<I', image, model.ENTRIES[0] - 0xFF000 + i * 4, instruction)
        return model.RecordTrace(bytes(image))

    def test_complete_entries_memory_and_instruction_scope(self):
        t = self.trace()
        for address in (model.ENTRIES[0] - 4, model.ENTRIES[0] + 1, 0x270D98, 0x2A1CDC):
            with self.assertRaises(ValueError):
                t.fetch(address)
        with self.assertRaises(ValueError):
            t.run(model.ENTRIES[0] + 4)
        with self.assertRaises(ValueError):
            model.RecordTrace(b'').fetch(model.ENTRIES[0])
        for i in range(16):
            t.memory[model.BUFFER + i] = 0
        for size in (2, 4, 8, 16):
            t.save(model.BUFFER, 123, size)
            self.assertEqual(t.load(model.BUFFER, size), 123)
        for address, size in ((model.BUFFER + 1, 2), (model.END, 4),
                              (model.BUFFER, 1), (model.BUFFER + 4, 8),
                              (model.BUFFER + 8, 16), (model.BUFFER + 16, 4)):
            with self.assertRaises(ValueError):
                t.load(address, size)

    def test_real_quad_lane_pack_unpack_and_upper_copy(self):
        t = self.trace()
        t.r[7], t.r[11], t.r[10], t.r[5] = word(2), word(1), word(3), word(4)
        for instruction in (0x71674488, 0x71454C88, 0x71091488, 0x48A22800):
            t.execute(instruction, model.ENTRIES[0])
        self.assertEqual(t.vf[5], [4, 2, 3, 1])
        t.execute(0x48222800, model.ENTRIES[0])
        t.execute(0x70421BA9, model.ENTRIES[0])
        self.assertEqual(t.r[3] & 0xFFFFFFFF, word(3))
        t.execute(0x0002183E, model.ENTRIES[0])
        self.assertEqual(t.r[3] & 0xFFFFFFFF, word(2))
        for instruction in (0x70000000, 0x48A22801, 0x48222801, 0x0022183E):
            with self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRIES[0])

    def test_vu_accumulator_order_preservation_and_reserved_forms(self):
        t = self.trace()
        t.vf[4] = [1, 2, 3, 4]
        t.vf[3] = [5, 6, 7, 8]
        t.vf[2] = [9, 10, 11, 12]
        t.vf[1] = [13, 14, 15, 16]
        t.vf[5] = [2, 3, 4, 1]
        with self.assertRaisesRegex(ValueError, 'before producer'):
            t.execute(0x4BE518BD, model.ENTRIES[0])
        t.execute(0x4BE521BC, model.ENTRIES[0])
        self.assertEqual(t.vu_acc, [2, 4, 6, 8])
        t.execute(0x4BE518BD, model.ENTRIES[0])
        self.assertEqual(t.vu_acc, [17, 22, 27, 32])
        t.execute(0x4BE510BE, model.ENTRIES[0])
        self.assertEqual(t.vu_acc, [53, 62, 71, 80])
        before = list(t.vu_acc)
        t.execute(0x4BE0094B, model.ENTRIES[0])
        self.assertEqual(t.vf[5], [66, 76, 86, 96])
        self.assertEqual(t.vu_acc, before)
        for instruction in (0x4BC521BC, 0x4BE521FC, 0x4BE518FD, 0x4BE518BF):
            with self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRIES[0])

    def test_product_rounding_precedes_vu_accumulator_add(self):
        t = self.trace()
        t.vf[4] = [-(1 + 2**-22)] * 4
        t.vf[3] = [1 + 2**-23] * 4
        t.vf[5] = [1, 1 + 2**-23, 0, 0]
        t.execute(0x4BE521BC, model.ENTRIES[0])
        t.execute(0x4BE518BD, model.ENTRIES[0])
        self.assertEqual(t.vu_acc, [0.0] * 4)
        self.assertNotEqual(-(1 + 2**-22) + (1 + 2**-23)**2, 0.0)

    def test_finite_normal_zero_and_arithmetic_domain_rejection(self):
        t = self.trace()
        for value in (math.inf, math.nan, struct.unpack('<f', struct.pack('<I', 1))[0]):
            t.vf[4] = [value] * 4
            t.vf[5] = [1] * 4
            with self.assertRaises(ValueError):
                t.execute(0x4BE521BC, model.ENTRIES[0])
        cop = (17 << 26) | (16 << 21)
        t.f[1], t.f[2] = 1.0, 0.0
        with self.assertRaisesRegex(ValueError, 'zero denominator'):
            t.execute(cop | (2 << 16) | (1 << 11) | 3, model.ENTRIES[0])
        t.f[2] = -1.0
        with self.assertRaisesRegex(ValueError, 'negative square root'):
            t.execute(cop | (2 << 16) | 4, model.ENTRIES[0])
        with self.assertRaises(ValueError):
            t.library_call(0x374849)

    def test_signed_halfword_and_low32_negative_map_branch(self):
        t = self.trace()
        for i in range(4):
            t.memory[model.BUFFER + i] = 0
        t.save(model.BUFFER, 0x8001, 2)
        t.r[4] = model.BUFFER
        t.execute((0x21 << 26) | (4 << 21) | (5 << 16), model.ENTRIES[0])
        self.assertEqual(t.r[5] & 0xFFFFFFFF, 0xFFFF8001)
        branch = (1 << 26) | (5 << 21) | 1
        self.assertEqual(t.execute(branch, model.ENTRIES[0]), (model.ENTRIES[0] + 8, False))
        t.r[5] = 0x7FFF
        self.assertEqual(t.execute(branch, model.ENTRIES[0]), (None, False))

    def test_delay_encoding_rejection_for_taken_and_untaken_outer_branch(self):
        outer = (4 << 26) | (4 << 21) | (5 << 16) | 1
        delays = (outer, (1 << 26) | (2 << 21) | 1,
                  (17 << 26) | (8 << 21) | 1, 0x03E00008, (3 << 26))
        for equal in (False, True):
            for delay in delays:
                t = self.trace((outer, delay, 0x03E00008, 0))
                t.r[4], t.r[5], t.r[31] = 1, 1 if equal else 2, RETURN
                with self.assertRaisesRegex(ValueError, 'control transfer in delay'):
                    t.run(model.ENTRIES[0])
                self.assertEqual(t.instruction_count, 1)
        t = self.trace(((21 << 26) | (4 << 21) | 1, outer, 0x03E00008, 0))
        t.r[31] = RETURN
        t.run(model.ENTRIES[0])  # Annulled likely delay is never fetched.
        self.assertEqual(t.instruction_count, 3)

    def test_actual_jr31_unknown_call_and_local_branch_bounds(self):
        t = self.trace(((2 << 21) | 8, 0))
        t.r[2] = RETURN
        with self.assertRaisesRegex(ValueError, 'actual JR31'):
            t.run(model.ENTRIES[0])
        t = self.trace(((3 << 26) | (0x270C90 >> 2), 0))
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'unknown record'):
            t.run(model.ENTRIES[0])
        t = self.trace(((4 << 26) | ((0x270C90 - 0x270A00 - 4) // 4), 0))
        with self.assertRaisesRegex(ValueError, 'outside complete body'):
            t.run(model.ENTRIES[0])
        t = self.trace(((4 << 26) | 0xFFFF, 0))
        with self.assertRaisesRegex(ValueError, 'bound'):
            t.run(model.ENTRIES[0])


if __name__ == '__main__':
    unittest.main()
