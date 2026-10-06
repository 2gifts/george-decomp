"""Focused checks of the new bounded character-trace primitives, no game data."""
import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from trace_text_tokens import TextTrace, signed32


def immediate(op, rs, rt, value):
    return (op << 26) | (rs << 21) | (rt << 16) | (value & 0xFFFF)


def special(fn, rs=0, rt=0, rd=0, shift=0):
    return (rs << 21) | (rt << 16) | (rd << 11) | (shift << 6) | fn


class TextTraceTests(unittest.TestCase):
    def setUp(self):
        self.trace = TextTrace(b'')

    def test_byte_signedness_and_bounded_store(self):
        self.trace.initialize(0x1000, bytes((0x80, 0xFF, 1)))
        self.trace.r[4] = 0x1001
        self.trace.execute(immediate(0x20, 4, 5, -1), 0)
        self.assertEqual(self.trace.r[5], 0xFFFFFF80)
        self.trace.execute(special(0x2D, rs=5, rd=6), 0)
        self.assertEqual(self.trace.r[6], self.trace.r[5])
        self.assertEqual(self.trace.execute(immediate(4, 5, 6, 1), 0x100), (0x108, False))
        self.trace.execute(immediate(0x24, 4, 5, 0), 0)
        self.assertEqual(self.trace.r[5], 255)
        self.trace.r[5] = 0x1234
        self.trace.execute(immediate(0x28, 4, 5, 1), 0)
        self.assertEqual(self.trace.load(0x1002, 1), 0x34)
        with self.assertRaises(KeyError):
            self.trace.execute(immediate(0x20, 4, 5, 10), 0)
        with self.assertRaisesRegex(ValueError, 'outside initialized'):
            self.trace.execute(immediate(0x28, 4, 5, 10), 0)
        with self.assertRaisesRegex(ValueError, 'overlapping'):
            self.trace.initialize(0x1001, b'\x00')

    def test_likely_and_ordinary_branch_targets(self):
        for op, equal, taken, annul in ((5, True, False, False), (5, False, True, False),
                                       (0x14, True, True, False), (0x14, False, False, True),
                                       (0x15, False, True, False), (0x15, True, False, True)):
            self.trace.r[4], self.trace.r[5] = 9, 9 if equal else 8
            self.assertEqual(self.trace.execute(immediate(op, 4, 5, -2), 0x100),
                             (0xFC if taken else None, annul))

    def test_signed_shift_and_unsigned_comparisons(self):
        self.trace.r[4] = 0xFF
        self.trace.execute(special(0, rt=4, rd=5, shift=24), 0)
        self.assertEqual(self.trace.r[5], 0xFF000000)
        self.trace.execute(special(3, rt=5, rd=6, shift=24), 0)
        self.assertEqual(self.trace.r[6], 0xFFFFFFFF)
        self.assertEqual(signed32(0x80000000), -0x80000000)
        self.trace.r[4], self.trace.r[5] = -1, 1
        self.trace.execute(special(0x2B, rs=4, rt=5, rd=6), 0)
        self.assertEqual(self.trace.r[6], 0)
        self.trace.execute(special(0x2B, rs=5, rt=4, rd=6), 0)
        self.assertEqual(self.trace.r[6], 1)
        self.trace.r[4] = 0xFFFFFFFE
        self.trace.execute(immediate(0x0B, 4, 5, -1), 0)
        self.assertEqual(self.trace.r[5], 1)
        self.trace.r[4] = -1
        self.trace.execute(immediate(0x0B, 4, 5, 5), 0)
        self.assertEqual(self.trace.r[5], 0)

    def test_conditional_moves_and_xor(self):
        self.trace.r[4], self.trace.r[5], self.trace.r[6] = 22, 0, 9
        self.trace.execute(special(0x0A, rs=4, rt=5, rd=6), 0)
        self.assertEqual(self.trace.r[6], 22)
        self.trace.execute(immediate(0x0E, 4, 5, 22), 0)
        self.assertEqual(self.trace.r[5], 0)
        self.trace.r[6] = 9
        self.trace.execute(special(0x0B, rs=4, rt=5, rd=6), 0)
        self.assertEqual(self.trace.r[6], 9)
        self.trace.r[5] = -1
        self.trace.execute(special(0x0B, rs=4, rt=5, rd=6), 0)
        self.assertEqual(self.trace.r[6], 22)
        self.trace.execute(special(0, rt=4, rd=0, shift=24), 0)
        self.assertEqual(self.trace.r[0], 0)

    def test_controlled_compare_zero_count_and_unsigned_difference(self):
        self.trace.r[4:7] = (0x1000, 0x2000, 0)
        self.trace.library_call(0x393E48)
        self.assertEqual(self.trace.r[2], 0)  # No unknown-memory reads.
        self.trace.initialize(0x1000, b'\xff\x00X')
        self.trace.initialize(0x2000, b'\x80\x00Y')
        self.trace.r[6] = 3
        self.trace.library_call(0x393E48)
        self.assertEqual(self.trace.r[2], 127)
        self.trace.save(0x1000, 0x80, 1)
        self.trace.library_call(0x393E48)
        self.assertEqual(self.trace.r[2], 0)  # Equal NUL stops before X/Y.
        self.trace.r[6] = 129
        with self.assertRaisesRegex(ValueError, 'exceeds its bound'):
            self.trace.library_call(0x393E48)

    def test_strict_instruction_and_call_boundaries(self):
        for pc in (0x2B318C, 0x2B3191, 0x2B3460, 0x2B3EDC, 0x2B4188):
            with self.assertRaisesRegex(ValueError, 'unreviewed'):
                self.trace.fetch(pc)
        with self.assertRaisesRegex(ValueError, 'unreviewed controlled call'):
            self.trace.library_call(0x1234)
        with self.assertRaisesRegex(ValueError, 'unsupported opcode'):
            self.trace.execute(0x68000000, 0)
        self.trace.instruction_count = 3000
        with self.assertRaisesRegex(ValueError, 'exceeded its bound'):
            self.trace.execute(immediate(0x0E, 4, 5, 0), 0)


if __name__ == '__main__':
    unittest.main()
