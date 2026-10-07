"""Strict finite scope, 64-bit soft-ABI and control-delay regression guards."""
from pathlib import Path
import math
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import trace_segment_distance as model
from trace_geometry import RETURN, word


class SegmentGuards(unittest.TestCase):
    def trace(self, instructions=()):
        image = bytearray(model.END - 0xFF000)
        for i, instruction in enumerate(instructions):
            struct.pack_into('<I', image, model.ENTRY - 0xFF000 + i * 4, instruction)
        return model.SegmentTrace(bytes(image))

    def test_owned_entry_and_whole_initialized_aligned_memory(self):
        t = self.trace()
        self.assertEqual(t.fetch(model.ENTRY), 0)
        for address in (model.ENTRY - 4, model.ENTRY + 1, model.END):
            with self.assertRaises(ValueError):
                t.fetch(address)
        with self.assertRaises(ValueError):
            t.run(model.ENTRY + 4)
        with self.assertRaises(ValueError):
            model.SegmentTrace(b'').fetch(model.ENTRY)
        for address in range(model.BUFFER, model.BUFFER + 16):
            t.memory[address] = 0
        for size in (4, 8, 16):
            t.save(model.BUFFER, 123, size)
            self.assertEqual(t.load(model.BUFFER, size), 123)
        for address, size in ((model.BUFFER + 1, 4), (model.BUFFER_END, 4),
                              (model.BUFFER, 3), (model.BUFFER + 4, 8),
                              (model.BUFFER + 8, 16), (model.BUFFER + 16, 4)):
            with self.assertRaises(ValueError):
                t.load(address, size)
            with self.assertRaises(ValueError):
                t.save(address, 0, size)

    def test_unknown_opcodes_callees_and_operand_forms(self):
        t = self.trace()
        cop = (17 << 26) | (16 << 21)
        words = [(2 << 26), (18 << 26), (35 << 26), (1 << 26),
                 (15 << 26) | (1 << 21), (1 << 6) | 0x2D,
                 (31 << 21) | (1 << 16) | 8, (17 << 26) | (4 << 21) | 1,
                 cop | (1 << 16) | 6, cop | (1 << 16) | 7,
                 cop | (1 << 6) | 0x36, cop | 4,
                 (17 << 26) | (8 << 21) | (4 << 16)]
        for instruction in words:
            with self.subTest(instruction=hex(instruction)), self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRY)
        with self.assertRaises(ValueError):
            t.library_call(0x374850)

    def test_daddu_preserves_high_soft_bits_and_signed_bgez(self):
        t = self.trace()
        t.r[2] = 0xBFF0000000000000
        t.execute((2 << 21) | (16 << 11) | 0x2D, model.ENTRY)
        self.assertEqual(t.r[16], 0xBFF0000000000000)
        branch = (1 << 26) | (2 << 21) | (1 << 16) | 1
        for value, expected in ((model.MASK64, None), (0, model.ENTRY + 8), (1, model.ENTRY + 8)):
            t.r[2] = value
            self.assertEqual(t.execute(branch, model.ENTRY), (expected, False))
        t.r[4], t.r[5] = model.MASK64, 1
        t.execute((4 << 21) | (5 << 16) | 0x2D, model.ENTRY)
        self.assertEqual(t.r[0], 0)

    def test_finite_normal_zero_soft_models_and_signed_zero(self):
        t = self.trace()
        t.f[12] = -0.0
        t.library_call(0x374848)
        self.assertEqual(t.r[2], 0x8000000000000000)
        t.r[4], t.r[5] = t.r[2], 0
        t.library_call(0x373250)
        self.assertEqual(t.r[2], 0)  # Original absolute leaves -0 unchanged.
        t.r[4] = 0x8000000000000000
        t.library_call(0x3734F8)
        self.assertEqual(word(t.f[0]), 0x80000000)
        t.r[4], t.r[5] = 0, model.double_bits(-1.5)
        t.library_call(0x372CC0)
        self.assertEqual(t.r[2], model.double_bits(1.5))
        for value in (math.inf, math.nan, struct.unpack('<f', struct.pack('<I', 1))[0]):
            with self.assertRaises(ValueError):
                model.normal_single(value)
        for value in (0x7FF0000000000000, 1):
            with self.assertRaises(ValueError):
                model.double_value(value)

    def test_zero_division_nonfinite_and_subnormal_scalar_rejection(self):
        t = self.trace()
        cop = (17 << 26) | (16 << 21)
        t.f[1], t.f[2] = 1, 0
        with self.assertRaisesRegex(ValueError, 'zero denominator'):
            t.execute(cop | (2 << 16) | (1 << 11) | 3, 0)
        for value in (math.inf, math.nan):
            t.f[1] = value
            with self.assertRaisesRegex(ValueError, 'finite'):
                t.execute(cop | (1 << 11) | 6, 0)
        for i in range(4):
            t.memory[model.BUFFER + i] = 0
        t.r[4] = model.BUFFER
        for bits in (0x7F800000, 1):
            t.save(model.BUFFER, bits, 4)
            with self.assertRaisesRegex(ValueError, 'finite'):
                t.execute((49 << 26) | (4 << 21), 0)

    def test_ordinary_delays_and_all_encoded_control_delays(self):
        outer = (4 << 26) | (4 << 21) | (5 << 16) | 1
        delays = ((4 << 26) | (4 << 21) | (5 << 16) | 1,
                  (1 << 26) | (1 << 16) | (2 << 21) | 1,
                  (17 << 26) | (8 << 21) | 1, (18 << 26) | (8 << 21) | 1)
        for equal in (False, True):
            for delay in delays:
                t = self.trace((outer, delay, 0x03E00008, 0))
                t.r[4], t.r[5], t.r[2], t.r[31], t.condition = 1, 1 if equal else 2, model.MASK64, RETURN, True
                with self.assertRaisesRegex(ValueError, 'control transfer in delay'):
                    t.run()
                self.assertEqual(t.instruction_count, 1)
        t = self.trace((outer, 0x70000000, 0x03E00008, 0))
        t.r[4], t.r[5], t.r[31] = 1, 2, RETURN
        with self.assertRaises(ValueError):
            t.run()

    def test_likely_null_store_annul_and_same_output_store(self):
        branch = (21 << 26) | (8 << 21) | 1
        store = (57 << 26) | (8 << 21) | (7 << 16)
        t = self.trace((branch, store, 0x03E00008, 0))
        t.r[31] = RETURN
        t.run()
        self.assertEqual(t.instruction_count, 3)
        t = self.trace((branch, store, 0x03E00008, 0))
        for i in range(4):
            t.memory[model.BUFFER + i] = 0
        t.r[8], t.r[31], t.f[7] = model.BUFFER, RETURN, 0.5
        t.run()
        self.assertEqual(t.load(model.BUFFER, 4), word(0.5))
        self.assertEqual(t.instruction_count, 4)
        t = self.trace((branch, (4 << 26) | 1, 0x03E00008, 0))
        t.r[31] = RETURN
        t.run()  # Encoded branch in an annulled delay is not fetched.
        self.assertEqual(t.instruction_count, 3)

    def test_actual_jr31_and_only_selected_external_return(self):
        t = self.trace(((2 << 21) | 8, 0))
        t.r[2] = RETURN
        with self.assertRaisesRegex(ValueError, 'actual JR31'):
            t.run()
        for target in (model.ENTRY + 8, model.END):
            t = self.trace((0x03E00008, 0))
            t.r[31] = target
            with self.assertRaisesRegex(ValueError, 'selected stop'):
                t.run()

    def test_instruction_bound_branch_extent_and_delay_extent(self):
        t = self.trace(((4 << 26) | 0xFFFF, 0))
        with self.assertRaisesRegex(ValueError, 'bound'):
            t.run()
        t = self.trace(((4 << 26) | ((model.END - model.ENTRY - 4) // 4), 0))
        with self.assertRaisesRegex(ValueError, 'outside owned'):
            t.run()
        t = self.trace(((4 << 26) | ((model.END - model.ENTRY - 8) // 4), 0))
        image = bytearray(t.original)
        struct.pack_into('<I', image, model.END - 4 - 0xFF000, 0x03E00008)
        t = model.SegmentTrace(bytes(image))
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'delay outside'):
            t.run()


if __name__ == '__main__':
    unittest.main()
