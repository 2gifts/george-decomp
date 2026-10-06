"""Pure synthetic strict decoder/scope/recursive-helper guard regressions."""
import math
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import trace_plane_geometry as model
from trace_geometry import RETURN, word


class PlaneGuards(unittest.TestCase):
    def trace(self, instructions=(), entry=None):
        entry = model.ENTRIES[0] if entry is None else entry
        image = bytearray(model.HELPER[1] - 0xFF000)
        for i, instruction in enumerate(instructions):
            struct.pack_into('<I', image, entry - 0xFF000 + i * 4, instruction)
        return model.PlaneTrace(bytes(image))

    def test_entries_memory_and_widths(self):
        t = self.trace()
        for a, b in (*model.RANGES, model.HELPER):
            self.assertIsInstance(t.fetch(a), int)
            with self.assertRaises(ValueError): t.fetch(a + 1)
        with self.assertRaises(ValueError): t.fetch(model.RANGES[0][1])
        with self.assertRaises(ValueError): t.run(model.ENTRIES[0] + 4)
        with self.assertRaises(ValueError): model.PlaneTrace(b'').fetch(model.ENTRIES[0])
        with self.assertRaises(ValueError): t.load(model.BUFFER, 4)
        for p in range(model.BUFFER, model.BUFFER + 32): t.memory[p] = 0
        for n in (4, 8, 16):
            t.save(model.BUFFER, 0x12345678, n)
            self.assertEqual(t.load(model.BUFFER, n), 0x12345678)
        for a, n in ((model.BUFFER + 1, 4), (model.END, 4), (model.BUFFER, 1), (model.BUFFER, 3)):
            with self.assertRaises(ValueError): t.load(a, n)
            with self.assertRaises(ValueError): t.save(a, 0, n)

    def test_unsupported_and_reserved_encodings(self):
        t = self.trace()
        single = (17 << 26) | (16 << 21)
        for instruction in ((2 << 26), (1 << 26), (35 << 26), (18 << 26),
                            (15 << 26) | (1 << 21), (31 << 21) | 9,
                            (1 << 6) | 0x2D, (31 << 21) | (1 << 16) | 8,
                            (17 << 26) | (4 << 21) | 1,
                            single | (1 << 16) | 6, single | (1 << 11) | 4,
                            single | (1 << 6) | 0x34,
                            (17 << 26) | (8 << 21) | (4 << 16)):
            with self.assertRaises(ValueError): t.execute(instruction, model.ENTRIES[0])
        with self.assertRaises(ValueError): t.library_call(model.HELPER[0])

    def test_finite_arithmetic_and_ee_ft_sqrt(self):
        t = self.trace()
        single = (17 << 26) | (16 << 21)
        t.f[1], t.f[2] = 16, 9
        t.execute(single | (2 << 16) | (3 << 6) | 4, 0)
        self.assertEqual(word(t.f[3]), word(3))
        t.f[2] = -1
        with self.assertRaisesRegex(ValueError, 'negative square root'):
            t.execute(single | (2 << 16) | 4, 0)
        t.f[2] = 0
        with self.assertRaisesRegex(ValueError, 'zero denominator'):
            t.execute(single | (2 << 16) | (1 << 11) | 3, 0)
        t.f[1] = math.inf
        with self.assertRaisesRegex(ValueError, 'finite'):
            t.execute(single | (1 << 11) | 6, 0)
        for p in range(model.BUFFER, model.BUFFER + 4): t.memory[p] = 0
        t.save(model.BUFFER, 0x7F800000, 4); t.r[4] = model.BUFFER
        with self.assertRaisesRegex(ValueError, 'finite'):
            t.execute((49 << 26) | (4 << 21), 0)

    def test_ordinary_likely_and_encoding_control_delays(self):
        branch = (17 << 26) | (8 << 21) | 2
        for condition in (False, True):
            for delay in ((4 << 26) | (1 << 21) | 2, (1 << 26) | (1 << 16) | 2,
                          (17 << 26) | (8 << 21) | 2, (3 << 26), (2 << 26), (31 << 21) | 9):
                t = self.trace((branch, delay, 0x03E00008, 0))
                t.r[31], t.condition = RETURN, condition
                with self.assertRaisesRegex(ValueError, 'transfer in delay'): t.run(model.ENTRIES[0])
        t = self.trace((branch, 0x70000000, 0x03E00008, 0))
        t.r[31], t.condition = RETURN, True
        with self.assertRaises(ValueError): t.run(model.ENTRIES[0])
        t = self.trace((branch | (2 << 16), 0x70000000, 0x03E00008, 0))
        t.r[31], t.condition = RETURN, True
        t.run(model.ENTRIES[0]); self.assertEqual(t.instruction_count, 3)

    def test_actual_return_and_bounded_control(self):
        t = self.trace(((2 << 21) | 8, 0)); t.r[2] = RETURN
        with self.assertRaisesRegex(ValueError, 'actual JR31'): t.run(model.ENTRIES[0])
        t = self.trace((0x03E00008, 0)); t.r[31] = model.ENTRIES[1]
        with self.assertRaisesRegex(ValueError, 'selected stop'): t.run(model.ENTRIES[0])
        t = self.trace(((4 << 26) | 0xFFFF, 0))
        with self.assertRaisesRegex(ValueError, 'bound'): t.run(model.ENTRIES[0])
        a, b = model.RANGES[0]
        t = self.trace(((4 << 26) | ((b - a - 4) // 4), 0))
        with self.assertRaisesRegex(ValueError, 'outside owned'): t.run(a)
        image = bytearray(t.original)
        struct.pack_into('<I', image, a - 0xFF000, (4 << 26) | ((b - a - 8) // 4))
        struct.pack_into('<I', image, b - 4 - 0xFF000, 0x03E00008)
        t = model.PlaneTrace(bytes(image)); t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'delay outside'): t.run(a)

    def test_real_scoped_helper_call_contract(self):
        call = (3 << 26) | (model.HELPER[0] >> 2)
        t = self.trace((call, 0, 0x03E00008, 0))
        image = bytearray(t.original)
        struct.pack_into('<II', image, model.HELPER[0] - 0xFF000, 0x03E00008, 0)
        t = model.PlaneTrace(bytes(image)); t.r[31] = RETURN
        # Synthetic caller did not save/restore RA, so returning to the call's
        # continuation instead of the selected stop must be rejected.
        with self.assertRaisesRegex(ValueError, 'selected stop'): t.run(model.ENTRIES[0])
        t = self.trace(((3 << 26) | (0x29C860 >> 2), 0))
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'unreviewed helper'): t.run(model.ENTRIES[0])


if __name__ == '__main__':
    unittest.main()
