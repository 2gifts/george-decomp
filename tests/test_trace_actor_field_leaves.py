import copy
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from analyze import validated_elf
from trace_actor_field_leaves import (ActorFieldTrace, BASE, BYTES, RANGES,
                                     RETURN, fixtures, make_fixture)


def controlled_image(routine, pc, word):
    """A single controlled guard word, not a replacement original body."""
    image = bytearray(RANGES[routine][1] - 0xFF000)
    struct.pack_into('<I', image, pc - 0xFF000, word)
    return bytes(image)


class ActorFieldGuards(unittest.TestCase):
    def unchanged_rejection(self, trace, callable_, message):
        before = copy.deepcopy(trace.__dict__)
        with self.assertRaisesRegex(ValueError, message):
            callable_()
        self.assertEqual(trace.__dict__, before)

    def test_memory_ownership_alignment_and_initialization(self):
        trace = ActorFieldTrace(b'')
        for address, size in ((BASE - 4, 4), (BASE + BYTES, 4), (BASE + 1, 4),
                              (BASE + 4, 8), (BASE, 16)):
            self.unchanged_rejection(trace, lambda: trace.load(address, size), 'memory')
            self.unchanged_rejection(trace, lambda: trace.save(address, 3, size), 'memory')
        self.unchanged_rejection(trace, lambda: trace.load(BASE, 4), 'uninitialized')

    def test_wrong_PC_word_precedes_mutation(self):
        pc = RANGES[0][0]
        trace = ActorFieldTrace(controlled_image(0, pc, 0x8C820198))
        trace.r[4] = BASE
        trace.save(BASE + 0x198, 0x80000000, 4)
        for word, address in ((0x8C820199, pc), (0x8C820198, pc + 1),
                              (0x8C820198, RANGES[0][1])):
            self.unchanged_rejection(trace, lambda: trace.execute(word, address), 'PC/word')

    def test_reserved_operand_validation_precedes_mutation(self):
        pc = RANGES[0][0]
        word = 0x8C620198  # Wrong base register in controlled input.
        trace = ActorFieldTrace(controlled_image(0, pc, word))
        self.unchanged_rejection(trace, lambda: trace.execute(word, pc), 'reserved')
        word = 0x00451064  # Reserved shift field on AND.
        trace = ActorFieldTrace(controlled_image(0, pc, word))
        self.unchanged_rejection(trace, lambda: trace.execute(word, pc), 'reserved')

    def test_budget_and_access_preflight_snapshot(self):
        pc, word = RANGES[0][0], 0x8C820198
        trace = ActorFieldTrace(controlled_image(0, pc, word))
        trace.r[4] = BASE
        trace.instruction_count = 8
        self.unchanged_rejection(trace, lambda: trace.execute(word, pc), 'budget')
        trace.instruction_count = 0
        self.unchanged_rejection(trace, lambda: trace.execute(word, pc), 'uninitialized')
        trace.r[4] = BASE + 1
        self.unchanged_rejection(trace, lambda: trace.execute(word, pc), 'unaligned')

    def test_JR_target_precedes_delay_store(self):
        pc, word = RANGES[1][1] - 8, 0x03E00008
        trace = ActorFieldTrace(controlled_image(1, pc, word), 1)
        trace.r[31] = RETURN - 4
        self.unchanged_rejection(trace, lambda: trace.execute(word, pc), 'JR31')

    def test_fetch_entry_and_fixture_domain(self):
        trace = ActorFieldTrace(b'')
        self.unchanged_rejection(trace, lambda: trace.fetch(RANGES[0][0]), 'PC')
        self.unchanged_rejection(trace, lambda: trace.run(RANGES[1][0]), 'entry/return')
        for routine in (-1, 11, True):
            with self.assertRaisesRegex(ValueError, 'entry'):
                ActorFieldTrace(b'', routine)
        with self.assertRaisesRegex(ValueError, 'fixture input'):
            make_fixture(b'', 0, 1 << 32, 0, 0, 0)


class ActorFieldOriginal(unittest.TestCase):
    def setUp(self):
        path = ROOT / 'orig/SLUS_216.68'
        if not path.is_file():
            self.skipTest('private original absent')
        _, self.original = validated_elf(path)

    def test_sign_extension_and_wrapping_boundaries(self):
        for word, signed in ((0, 0), (0x7FFFFFFF, 0x7FFFFFFF),
                             (0x80000000, 0xFFFFFFFF80000000),
                             (0xFFFFFFFF, 0xFFFFFFFFFFFFFFFF)):
            case = make_fixture(self.original, 10, word, 0, (1 << 64) - 1, 7)
            self.assertEqual(case['expected_result'], signed)
        self.assertEqual(make_fixture(self.original, 4, 0x7FFFFFFF, 0, 0, 7)['expected_result'],
                         0xFFFFFFFF80000000)
        self.assertEqual(make_fixture(self.original, 7, 0, 0, 0, 7)['expected_result'],
                         0xFFFFFFFFFFFFFFFF)

    def test_raw_capture_and_Boolean_are_distinct(self):
        self.assertEqual(make_fixture(self.original, 0, 0x80000000, 0, 1 << 63, 7)['expected_result'], 1)
        self.assertEqual(make_fixture(self.original, 3, 0x80000000, 0, 1 << 63, 7)['expected_result'], 0)
        self.assertEqual(make_fixture(self.original, 10, 0x10, 0, 0x10, 7)['expected_result'], 0x10)
        case = make_fixture(self.original, 2, 0x80000001, 0, 1 << 63, 7)
        self.assertEqual(case['expected_result'], 0x7FFFFFFF80000001)
        self.assertEqual(case['expected'][0x198 // 4], 0x80000001)

    def test_every_real_PC_and_delay_covered(self):
        cases = fixtures(self.original)
        self.assertEqual(len(cases), 1250)
        self.assertEqual(sum(case['instructions'] for case in cases), 5110)
        self.assertEqual({pc for case in cases for pc in case['visited']},
                         {pc for start, end in RANGES for pc in range(start, end, 4)})


if __name__ == '__main__':
    unittest.main()
