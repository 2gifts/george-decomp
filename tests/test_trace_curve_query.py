"""Synthetic strict-scope/delay/operand checks for the query instruction model."""
import math
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import trace_curve_query as model
from trace_geometry import RETURN, word


class QueryGuards(unittest.TestCase):
    def trace(self, instructions=()):
        image = bytearray(model.RANGES[-1][1] - 0xFF000)
        for i, instruction in enumerate(instructions):
            struct.pack_into('<I', image, model.ENTRIES[0] - 0xFF000 + 4 * i, instruction)
        return model.QueryTrace(bytes(image))

    def test_scoped_entries_and_initialized_aligned_memory(self):
        t = self.trace()
        for start, end in model.RANGES:
            self.assertIsInstance(t.fetch(start), int)
            for pc in (start + 1, end):
                with self.assertRaises(ValueError):
                    t.fetch(pc)
        with self.assertRaises(ValueError):
            t.run(model.ENTRIES[0] + 4)
        with self.assertRaises(ValueError):
            model.QueryTrace(b'').fetch(model.ENTRIES[0])
        with self.assertRaises(ValueError):
            t.load(model.BUFFER, 4)
        for i in range(16):
            t.memory[model.BUFFER + i] = 0
        t.save(model.BUFFER, 123, 4)
        self.assertEqual(t.load(model.BUFFER, 4), 123)
        for a, n in ((model.BUFFER + 1, 4), (model.END, 4), (model.BUFFER - 4, 4), (model.BUFFER, 3)):
            with self.assertRaises(ValueError):
                t.load(a, n)
            with self.assertRaises(ValueError):
                t.save(a, 0, n)

    def test_unsupported_opcode_and_library_calls(self):
        t = self.trace()
        for instruction in ((3 << 26), (35 << 26), (18 << 26),
                            (15 << 26) | (1 << 21), (17 << 26)):
            with self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRIES[0])
        with self.assertRaises(ValueError):
            t.library_call(0x29CF28)

    def test_reserved_special_and_cop1_operands(self):
        t = self.trace()
        single = (17 << 26) | (16 << 21)
        for instruction in ((1 << 6) | 0x2D, (31 << 21) | (1 << 16) | 8,
                            (17 << 26) | (4 << 21) | 1,
                            single | (1 << 16) | 6, single | (1 << 11) | 4,
                            single | (1 << 6) | 0x34,
                            (17 << 26) | (8 << 21) | (4 << 16)):
            with self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRIES[0])

    def test_finite_arithmetic_domain_guards(self):
        t = self.trace()
        single = (17 << 26) | (16 << 21)
        t.f[1], t.f[2] = 1, 0
        with self.assertRaisesRegex(ValueError, 'zero denominator'):
            t.execute(single | (2 << 16) | (1 << 11) | 3, 0)
        t.f[2] = -1
        with self.assertRaisesRegex(ValueError, 'negative square root'):
            t.execute(single | (2 << 16) | 4, 0)
        t.f[1] = math.inf
        with self.assertRaisesRegex(ValueError, 'finite'):
            t.execute(single | (1 << 11) | 6, 0)
        for i in range(4):
            t.memory[model.BUFFER + i] = 0
        t.save(model.BUFFER, 0x7F800000, 4)
        t.r[4] = model.BUFFER
        with self.assertRaisesRegex(ValueError, 'finite'):
            t.execute((49 << 26) | (4 << 21), 0)

    def test_ordinary_and_likely_branch_delays(self):
        branch = (17 << 26) | (8 << 21) | 2  # BC1F, condition true => untaken.
        t = self.trace((branch, 0x70000000, 0x03E00008, 0))
        t.r[31], t.condition = RETURN, True
        with self.assertRaises(ValueError):
            t.run(model.ENTRIES[0])
        t = self.trace((branch | (2 << 16), 0x70000000, 0x03E00008, 0))
        t.r[31], t.condition = RETURN, True
        t.run(model.ENTRIES[0])
        self.assertEqual(t.instruction_count, 3)
        for condition in (True, False):
            t = self.trace((branch, (4 << 26) | 2, 0x03E00008, 0))
            t.r[31], t.condition = RETURN, condition
            with self.assertRaisesRegex(ValueError, 'transfer in delay'):
                t.run(model.ENTRIES[0])

    def test_actual_jr31_and_selected_stop(self):
        t = self.trace(((2 << 21) | 8, 0))
        t.r[2] = RETURN
        with self.assertRaisesRegex(ValueError, 'actual JR31'):
            t.run(model.ENTRIES[0])
        t = self.trace((0x03E00008, 0))
        t.r[31] = model.ENTRIES[1]
        with self.assertRaisesRegex(ValueError, 'selected stop'):
            t.run(model.ENTRIES[0])
        t = self.trace((0x03E00008, (4 << 26) | 2))
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'transfer in delay'):
            t.run(model.ENTRIES[0])

    def test_bound_and_cross_body_delays(self):
        t = self.trace(((4 << 26) | 0xFFFF, 0))
        with self.assertRaisesRegex(ValueError, 'bound'):
            t.run(model.ENTRIES[0])
        a, b = model.RANGES[0]
        t = self.trace(((4 << 26) | ((b - a - 4) // 4), 0))
        with self.assertRaisesRegex(ValueError, 'outside owned'):
            t.run(a)
        image = bytearray(t.original)
        struct.pack_into('<I', image, a - 0xFF000, (4 << 26) | ((b - a - 8) // 4))
        struct.pack_into('<I', image, b - 4 - 0xFF000, 0x03E00008)
        t = model.QueryTrace(bytes(image))
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'delay outside'):
            t.run(a)

    def test_bnel_optional_store_is_annulled_for_null(self):
        branch = (21 << 26) | (7 << 21) | 1
        store = (57 << 26) | (7 << 21)
        t = self.trace((branch, store, 0x03E00008, 0))
        t.r[31] = RETURN
        t.run(model.ENTRIES[0])
        self.assertEqual(t.instruction_count, 3)
        t = self.trace((branch, store, 0x03E00008, 0))
        for i in range(4):
            t.memory[model.BUFFER + i] = 0
        t.r[7], t.r[31], t.f[0] = model.BUFFER, RETURN, 0.25
        t.run(model.ENTRIES[0])
        self.assertEqual(t.load(model.BUFFER, 4), word(0.25))
        self.assertEqual(t.instruction_count, 4)

    def test_ee_sqrt_uses_ft_and_rounds_single(self):
        t = self.trace()
        t.f[0], t.f[1] = 16, 9
        instruction = (17 << 26) | (16 << 21) | (1 << 16) | (2 << 6) | 4
        t.execute(instruction, 0)
        self.assertEqual(word(t.f[2]), word(3))
        t.f[1] = 2
        t.execute(instruction, 0)
        self.assertEqual(word(t.f[2]), word(math.sqrt(2)))


if __name__ == '__main__':
    unittest.main()
