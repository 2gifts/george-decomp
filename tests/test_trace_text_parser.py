"""Focused guards for newly required parser integer decoding; no game files."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from trace_text_parser import ParserTrace


class ParserDecoderTests(unittest.TestCase):
    def test_bgtz_observes_signed_low_word(self):
        trace = ParserTrace(b'')
        instruction = (7 << 26) | (4 << 21) | 2
        for value, target in ((0, None), (0xFFFFFFFF, None), (0x80000000, None), (1, 0x10C)):
            trace.r[4] = value
            self.assertEqual(trace.execute(instruction, 0x100), (target, False))
        self.assertEqual(trace.total_instructions, 4)

    def test_invalid_bgtz_encoding_rejected(self):
        with self.assertRaisesRegex(ValueError, 'BGTZ'):
            ParserTrace(b'').execute((7 << 26) | (1 << 16), 0)

    def test_subu_wraps_without_signed_overflow(self):
        trace = ParserTrace(b'')
        trace.r[4], trace.r[5] = 0, 1
        trace.execute((4 << 21) | (5 << 16) | (6 << 11) | 0x23, 0)
        self.assertEqual(trace.r[6], 0xFFFFFFFF)

    def test_andi_zero_extends_immediate(self):
        trace = ParserTrace(b'')
        trace.r[4] = 0xFFFFFFFF
        trace.execute((0xC << 26) | (4 << 21) | (5 << 16) | 0x8000, 0)
        self.assertEqual(trace.r[5], 0x8000)

    def test_total_budget_survives_inherited_counter_reset(self):
        trace = ParserTrace(b'')
        trace.total_instructions = 120000
        with self.assertRaisesRegex(ValueError, 'finite instruction bound'):
            trace.execute(0, 0)

    def test_unreviewed_code_memory_and_calls_rejected(self):
        trace = ParserTrace(b'')
        with self.assertRaisesRegex(ValueError, 'unreviewed parser instruction'):
            trace.fetch(0x2B3EE0)
        with self.assertRaises(KeyError):
            trace.load(0x10000, 1)
        with self.assertRaisesRegex(ValueError, 'outside initialized'):
            trace.save(0x10000, 0, 1)
        with self.assertRaisesRegex(ValueError, 'unreviewed controlled call'):
            trace.library_call(0x123456)


if __name__ == '__main__':
    unittest.main()
