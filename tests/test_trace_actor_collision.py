"""Scope, real recursion/callbacks, aliases and unwritten-output boundaries."""
import math
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import trace_actor_collision as model
from analyze import validated_elf
from trace_geometry import RETURN, word

LOCAL_ORIGINAL = (model.ROOT / 'orig/SLUS_216.68').is_file()


class CollisionGuards(unittest.TestCase):
    def trace(self, instructions=()):
        image = bytearray(max(b for _, b in model.INTERVALS) - 0xFF000)
        for i, instruction in enumerate(instructions):
            struct.pack_into('<I', image, model.ENTRIES[1] - 0xFF000 + i * 4, instruction)
        return model.CollisionTrace(bytes(image))

    def test_whole_entry_memory_and_execution_boundaries(self):
        t = self.trace()
        for pc in (model.ENTRIES[0] - 4, model.ENTRIES[0] + 1, model.RANGES[0][1], model.HELPERS[-1][1]):
            with self.assertRaises(ValueError): t.fetch(pc)
        with self.assertRaises(ValueError): model.CollisionTrace(b'').fetch(model.ENTRIES[0])
        with self.assertRaises(ValueError): t.run(model.ENTRIES[0] + 4)
        for a, n in ((model.BUFFER + 1, 4), (model.END, 4), (model.POOL + 28, 4), (model.BUFFER, 3)):
            with self.assertRaises(ValueError): t.load(a, n)
        t.instruction_count = 100000
        with self.assertRaisesRegex(ValueError, 'bound'): t.execute(0, model.ENTRIES[1])
        t.frames = [0] * 40
        with self.assertRaises(ValueError): t.run(model.ENTRIES[1])

    def test_unknown_stack_mode_requires_real_store(self):
        t = self.trace()
        pointer = 0x7FE94
        with self.assertRaisesRegex(ValueError, 'uninitialized'): t.load(pointer, 4)
        t.save(pointer, 0x12340000, 4)
        self.assertEqual(t.load(pointer, 4), 0x12340000)
        with self.assertRaisesRegex(ValueError, 'uninitialized'): t.load(pointer + 4, 4)

    def test_delay_encoding_outcome_and_real_return(self):
        for delay in ((4 << 26) | (4 << 21) | (5 << 16) | 1,
                      (1 << 26) | (4 << 21) | (1 << 16) | 1,
                      (17 << 26) | (8 << 21) | 1, 3 << 26, 0x03E00008):
            t = self.trace((0x03E00008, delay))
            t.r[4], t.r[5], t.r[31] = 1, 2, RETURN
            with self.assertRaisesRegex(ValueError, 'control transfer in delay'): t.run(model.ENTRIES[1])
        t = self.trace(((2 << 21) | 8, 0)); t.r[2] = RETURN
        with self.assertRaisesRegex(ValueError, 'actual JR31'): t.run(model.ENTRIES[1])
        # Even the untaken outer ordinary branch executes and validates delay.
        t = self.trace(((4 << 26) | (4 << 21) | (5 << 16) | 1, 0x03E00008))
        t.r[4], t.r[5] = 1, 2
        with self.assertRaisesRegex(ValueError, 'control transfer in delay'): t.run(model.ENTRIES[1])

    def test_signed_conversion_preserves_raw_integer_transfer(self):
        t = self.trace(); pc = model.ENTRIES[0]
        t.r[4] = 0xFFFFFFFE
        t.execute((17 << 26) | (4 << 21) | (4 << 16) | (2 << 11), pc)
        self.assertTrue(math.isnan(t.f[2]))  # Raw bits, never scalar arithmetic.
        t.execute((17 << 26) | (20 << 21) | (2 << 11) | (3 << 6) | 32, pc)
        self.assertEqual(word(t.f[3]), word(-2.0))
        with self.assertRaises(ValueError): t.execute((17 << 26) | (20 << 21) | (1 << 16) | 32, pc)
        with self.assertRaises(ValueError): t.execute((17 << 26) | (4 << 21) | 1, pc)
        # Full64 DADDU must retain all soft operands, not narrow them to u32.
        t.r[4], t.r[5] = 0x3FF0000000000000, 0
        t.execute((4 << 21) | (5 << 16) | (2 << 11) | 0x2D, pc)
        self.assertEqual(t.r[2], 0x3FF0000000000000)

    def test_signed_division_finite_and_unknown_callback_guards(self):
        t = self.trace(); pc = model.ENTRIES[0]
        divide = (4 << 21) | (5 << 16) | 0x1A
        t.r[4], t.r[5] = 0xFFFFFFF9, 3
        t.execute(divide, pc)
        self.assertEqual((model.signed(t.lo), model.signed(t.hi)), (-2, -1))
        for a, b in ((1, 0), (0x80000000, 0xFFFFFFFF)):
            t.r[4], t.r[5] = a, b
            with self.assertRaises(ValueError): t.execute(divide, pc)
        for instruction in (divide | 64, divide | (1 << 11), 0x0000000D, 0x70000000):
            with self.assertRaises(ValueError): t.execute(instruction, pc)
        with self.assertRaises(ValueError): t.library_call(0x1C1544)
        with self.assertRaises(ValueError): t.library_call(model.POSITION_CALL)
        t.f[0], t.f[1] = 1.0, 0.0
        with self.assertRaisesRegex(ValueError, 'zero denominator'): t.execute(0x46010003, pc)

    @unittest.skipUnless(LOCAL_ORIGINAL, 'requires locally supplied original ELF')
    def test_original_full_control_and_byte_narrowing_are_distinct(self):
        _, original = validated_elf(model.ROOT / 'orig/SLUS_216.68')
        for control in (256, -256, 1, -1):
            c = model.fixture(original, control=control)
            self.assertEqual(c['invocations'][0], 1)
            self.assertEqual(c['invocations'][2], 1)
            self.assertEqual(c['events'], [])
        c = model.fixture(original, slot=256, scalar0=99)
        self.assertEqual(c['invocations'][:2], [2, 1])
        self.assertEqual(c['expected'][0x42 // 4] >> 16 & 255, 1)
        # Retail does not protect the subsequent stores when the genuine
        # allocator returns null. This is an observer failure, not a new guard.
        t = model.fixture(original, control=1, run=False)
        t.save(model.POOL + 0x1A, 1, 2)
        with self.assertRaisesRegex(ValueError, 'outside aligned observer'): t.run(model.ENTRIES[0])

    @unittest.skipUnless(LOCAL_ORIGINAL, 'requires locally supplied original ELF')
    def test_original_point_z_capture_precedes_overlapping_scalar_store(self):
        _, original = validated_elf(model.ROOT / 'orig/SLUS_216.68')
        c = model.fixture(original, control=1, point_pointer=model.POOL_DATA + 8,
                          point=(7, 8, 9), scalar1=-3)
        self.assertEqual(c['expected'][0x900 // 4], word(7))
        self.assertEqual(c['expected'][0x904 // 4], word(8))
        self.assertEqual(c['expected'][0x908 // 4], word(9))
        self.assertEqual(c['expected'][0x910 // 4], word(-3))

    @unittest.skipUnless(LOCAL_ORIGINAL, 'requires locally supplied original ELF')
    def test_original_unwritten_query_mode_is_not_fabricated(self):
        _, original = validated_elf(model.ROOT / 'orig/SLUS_216.68')
        t = model.fixture(original, objects=1, polygon=True, run=False)
        with self.assertRaisesRegex(ValueError, 'uninitialized collision read'): t.run(model.ENTRIES[0])
        self.assertEqual(t.query_count, 1)
        self.assertEqual(t.invocations[8:12], [1, 1, 1, 4])
        self.assertIn(0x1E4468, t.visited)  # Actual LW of original SP+104.

    @unittest.skipUnless(LOCAL_ORIGINAL, 'requires locally supplied original ELF')
    def test_original_two_children_and_callback_mutation_reloads(self):
        _, original = validated_elf(model.ROOT / 'orig/SLUS_216.68')
        for mutation in (0, 2, 3, 5, 7):
            c = model.fixture(original, objects=1, query=1, mutation=mutation, scalar0=99)
            self.assertEqual(c['invocations'][:2], [3, 2])
            self.assertNotEqual(c['expected'][0x918 // 4], 0)
            self.assertNotEqual(c['expected'][0x91C // 4], 0)
            self.assertEqual(c['invocations'][-1], 4)  # Both distance calls on both sides.
        c = model.fixture(original, objects=1, query=1, mutation=4, scalar0=99)
        self.assertEqual(c['invocations'][:2], [2, 0])
        self.assertEqual(c['expected'][0x918 // 4], 0)

        # Full route IDs use LW independently from LHU record/edge indexing.
        t = model.fixture(original, run=False)
        for pc in (0x1E4398, 0x1E4404, 0x1E4824, 0x1E4894):
            self.assertEqual(t.fetch(pc) >> 26, 35)
            self.assertEqual(t.fetch(pc) & 65535, 0)
        for initial_key in (0, 0x80000000):
            for mutation in (0, 5, 8):
                c = model.fixture(original, objects=1, query=1, mutation=mutation,
                                  scalar0=99, query_upper=0x8000,
                                  initial_key=initial_key, orientation=1)
                self.assertEqual(c['invocations'][:2], [3, 2])
                self.assertEqual(c['expected'][0x14 // 4],
                                 0x80000000 + (mutation in (5, 8)))
                self.assertTrue({0x1E4398, 0x1E4824} <= set(c['visited']))
                if initial_key == 0:
                    self.assertIn(0x1E4404, c['visited'])
                if mutation == 8:
                    self.assertIn(0x1E4894, c['visited'])
                # A full-key match preserves the existing orientation; a
                # mismatch executes the reset even with the same low16 bits.
                self.assertEqual(c['expected'][0x18 // 4] & 255,
                                 int(initial_key == 0x80000000 and mutation == 0))


if __name__ == '__main__': unittest.main()
