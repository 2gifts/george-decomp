import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from analyze import validated_elf
from trace_route_setup import SetupTrace, RANGES, BUFFER, OWNER, WORDS, initial_word, make_fixture, fixtures


class SetupGuards(unittest.TestCase):
    def test_initialized_aligned_memory(self):
        t = SetupTrace(b'')
        for address, size in ((BUFFER + 1, 4), (BUFFER - 4, 4), (BUFFER, 8)):
            with self.assertRaises(ValueError):
                t.load(address, size)
        with self.assertRaisesRegex(ValueError, 'uninitialized'):
            t.load(BUFFER, 4)

    def test_reserved_and_unreviewed_encodings(self):
        t = SetupTrace(b'')
        for instruction in (0x0C000000, 0x44810001, 0x00800008, 0x0000006D):
            with self.assertRaisesRegex(ValueError, 'encoding'):
                t.execute(instruction, RANGES[0][0])

    def test_fetch_excludes_neighboring_code(self):
        t = SetupTrace(b'')
        for address in (RANGES[0][0] - 4, RANGES[0][1], RANGES[1][1], RANGES[0][0] + 1):
            with self.assertRaisesRegex(ValueError, 'unreviewed'):
                t.fetch(address)

    def test_control_delay_and_transfer_bounds(self):
        class ControlledTrace(SetupTrace):
            def fetch(self, pc):
                return self.instructions.get(pc, 0)
        entry = RANGES[0][0]
        t = ControlledTrace(b'')
        t.instructions = {entry: 0x10000000, entry + 4: 0x03E00008}
        with self.assertRaisesRegex(ValueError, 'in delay'):
            t.run(entry)
        t = ControlledTrace(b'')
        t.instructions = {entry: 0x10000043}
        with self.assertRaisesRegex(ValueError, 'outside whole body'):
            t.run(entry)
        t = ControlledTrace(b'')
        t.instructions = {entry: 0x1000FFFF}
        with self.assertRaisesRegex(ValueError, 'instruction bound'):
            t.run(entry)

    def test_fixture_layout_is_bounded(self):
        for args in ((True, 1, 2, 3, 4, 5), (2, 1, 2, 3, 4, 5),
                     (0, WORDS, 2, 3, 4, 5), (0, 1, -1, 3, 4, 5),
                     (0, 1, 2, 0x100000000, 4, 5), (0, 1.5, 2, 3, 4, 5)):
            with self.assertRaisesRegex(ValueError, 'fixture layout'):
                make_fixture(b'', *args)


class SetupOriginal(unittest.TestCase):
    def setUp(self):
        path = ROOT / 'orig/SLUS_216.68'
        if not path.is_file():
            self.skipTest('private original absent')
        _, self.original = validated_elf(path)

    def test_sentinel_category_and_equal_override(self):
        for entry, base, category, kind in ((0, 0xECC, 0xED2, 0xED1), (1, 0xCB0, 0xCB6, 0xCB5)):
            for first, second, wanted in ((1, 2, 0), (3, 3, 8), (0xFFFFFFFF, 4, 6),
                                          (5, 0xFFFFFFFF, 6), (0xFFFFFFFF, 0xFFFFFFFF, 8)):
                c = make_fixture(self.original, entry, 1200, 1208, first, second, 0x123456AB)
                actual = c['expected'][(category - base) // 4] >> ((category - base) % 4 * 8) & 255
                narrowed = c['expected'][(kind - base) // 4] >> ((kind - base) % 4 * 8) & 255
                self.assertEqual(actual, wanted)
                self.assertEqual(narrowed, 0xAB)
                self.assertEqual(c['expected_status'], initial_word(OWNER, 1) & 0xFFFFFF00)

    def test_shifted_sequential_copy_aliases(self):
        for entry, base, point_offset, output_offset in ((0, 0xECC, 0xF18, 0xF1C), (1, 0xCB0, 0xCF0, 0xCF4)):
            point = OWNER + point_offset // 4
            c = make_fixture(self.original, entry, point, 1208, 1, 2, 12, 9)
            start = (output_offset - base) // 4
            wanted = 0x49742400 if entry == 0 else initial_word(point, 9)
            self.assertEqual(c['expected'][start:start + 3], [wanted] * 3)

    def test_raw_words_and_opposite_signed_zero(self):
        point = 1201
        seed = next(s for s in range(1, 18) if (point * 7 + s) % 17 == 8)
        c = make_fixture(self.original, 0, point, 1208, 1, 2, 12, seed)
        self.assertEqual(c['expected'][(0xF1C - 0xECC) // 4], 0x80000000)
        point = OWNER + 0xECC // 4
        c = make_fixture(self.original, 0, point, 1208, 1, 2, 12, 3)
        self.assertEqual(c['expected'][(0xF1C - 0xECC) // 4], c['opaque_state'])

    def test_complete_instruction_coverage(self):
        cases = fixtures(self.original)
        self.assertEqual(len(cases), 1224)
        for start, end in RANGES:
            covered = {pc for c in cases for pc in c['visited'] if start <= pc < end}
            self.assertEqual(covered, set(range(start, end, 4)))


if __name__ == '__main__':
    unittest.main()
