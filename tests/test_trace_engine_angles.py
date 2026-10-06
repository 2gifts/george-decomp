"""Meaningful boundary and operand tests for the finite angle trace extension."""
import math
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))
from trace_engine_angles import AngleTrace, RANGES
from trace_geometry import word, scalar


def cop1(fs, ft, fd, function):
    return (0x11 << 26) | (16 << 21) | (ft << 16) | (fs << 11) | (fd << 6) | function


class AngleTraceTests(unittest.TestCase):
    def test_reciprocal_square_root_uses_both_operands(self):
        trace = AngleTrace(b'')
        trace.f[3], trace.f[5] = 3.0, 4.0
        self.assertEqual(trace.execute(cop1(3, 5, 7, 0x16), 0), (None, False))
        self.assertEqual(trace.f[7], 1.5)
        self.assertEqual(trace.instruction_count, 1)

    def test_reciprocal_square_root_captures_aliased_source(self):
        trace = AngleTrace(b'')
        trace.f[3], trace.f[5] = -3.0, 2.0
        trace.execute(cop1(3, 5, 5, 0x16), 0)
        root = scalar(word(math.sqrt(2.0)))
        self.assertEqual(word(trace.f[5]), word(-3.0 / root))

    def test_native_model_rejects_negative_or_zero_radicand(self):
        trace = AngleTrace(b'')
        trace.f[3], trace.f[5] = 1.0, -1.0
        with self.assertRaises(ValueError):
            trace.execute(cop1(3, 5, 7, 0x16), 0)
        trace.f[5] = 0.0
        with self.assertRaises(ZeroDivisionError):
            trace.execute(cop1(3, 5, 7, 0x16), 0)

    def test_strict_less_preserves_equality_and_unordered(self):
        trace = AngleTrace(b'')
        for first, second, expected in ((0.0, -0.0, False), (-1, 0, True), (float('nan'), 0, False)):
            trace.f[3], trace.f[5] = first, second
            trace.execute(cop1(3, 5, 0, 0x34), 0)
            self.assertEqual(trace.condition, expected)

    def test_fetch_rejects_padding_unaligned_and_other_code(self):
        start, end = RANGES[0]
        original = bytes(start - 0xFF000) + struct.pack('<I', 0x12345678)
        trace = AngleTrace(original)
        self.assertEqual(trace.fetch(start), 0x12345678)
        for address in (start-4, start+1, end, 0x100000):
            with self.assertRaisesRegex(ValueError, 'unreviewed angle'):
                trace.fetch(address)

    def test_unsupported_instruction_and_step_limit_fail(self):
        trace = AngleTrace(b'')
        with self.assertRaisesRegex(ValueError, 'unsupported COP1.S'):
            trace.execute(cop1(3, 5, 7, 0x3A), 0)
        trace.instruction_count = 3000
        with self.assertRaisesRegex(ValueError, 'exceeded its bound'):
            trace.execute(cop1(3, 5, 0, 0x34), 0)


if __name__ == '__main__':
    unittest.main()
