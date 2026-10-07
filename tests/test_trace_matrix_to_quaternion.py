import copy
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from analyze import validated_elf
from trace_geometry import Trace, RETURN, word
from trace_matrix_to_quaternion import (QuaternionTrace, BUFFER, BYTES, MATRIX,
                                       TABLE, ENTRY, END, COUNT_LIMIT, make_fixture)


def controlled_image(*pairs):
    image = bytearray(END - 0xFF000)
    for pc, instruction in pairs:
        struct.pack_into('<I', image, pc - 0xFF000, instruction)
    return bytes(image)


class QuaternionGuards(unittest.TestCase):
    def reject(self, trace, action, message):
        before = copy.deepcopy(trace.__dict__)
        with self.assertRaisesRegex(ValueError, message):
            action()
        self.assertEqual(trace.__dict__, before)

    def test_PC_word_budget_repeat_and_register_zero(self):
        pc, instruction = 0x2A0B00, 0x00A0402D
        t = QuaternionTrace(controlled_image((pc, instruction)))
        self.reject(t, lambda: t.execute(instruction, pc + 1), 'PC/word')
        self.reject(t, lambda: t.execute(instruction ^ 1, pc), 'PC/word')
        t.instruction_count = COUNT_LIMIT
        self.reject(t, lambda: t.execute(instruction, pc), 'budget')
        t.instruction_count = 0
        t.visited.add(pc)
        self.reject(t, lambda: t.execute(instruction, pc), 'repeated')
        t.visited.clear()
        t.r[0] = 1
        self.reject(t, lambda: t.execute(instruction, pc), 'register zero')

    def test_initialized_owned_float_load_and_store_before_mutation(self):
        pc, instruction = 0x2A0B08, 0xC5030000
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.r[8] = MATRIX
        self.reject(t, lambda: t.execute(instruction, pc), 'uninitialized')
        Trace.save(t, MATRIX, word(1.0), 4)
        t.r[8] = MATRIX + 1
        self.reject(t, lambda: t.execute(instruction, pc), 'unaligned')
        t.r[8] = BUFFER
        Trace.save(t, BUFFER, word(1.0), 4)
        self.reject(t, lambda: t.execute(instruction, pc), 'matrix load')
        pc, instruction = 0x2A0B58, 0xE541000C
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.r[10] = BUFFER
        Trace.save(t, BUFFER + 12, word(1.0), 4)
        self.reject(t, lambda: t.execute(instruction, pc), 'float source')
        t.f_defined.add(1)
        t.f[1] = float('inf')
        self.reject(t, lambda: t.execute(instruction, pc), 'finite')

    def test_table_index_values_and_INTEGER_form(self):
        pc, instruction = 0x2A0C00, 0x8C690000
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.r[3] = TABLE
        Trace.save(t, TABLE, 3, 4)
        self.reject(t, lambda: t.execute(instruction, pc), 'table index')
        Trace.save(t, TABLE, 2, 4)
        t.execute(instruction, pc)
        self.assertEqual((t.r[9], t.instruction_count, t.routes['integer']), (2, 1, 1))
        pc, instruction = 0x2A0C08, 0x012B1858  # Reserved MULT shift.
        t = QuaternionTrace(controlled_image((pc, instruction)))
        self.reject(t, lambda: t.execute(instruction, pc), 'SPECIAL form')

    def test_SQRT_ft_and_scalar_failures_precede_count(self):
        pc, instruction = 0x2A0C34, 0x460200C4
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.f[0], t.f[2] = 81.0, 4.0
        t.f_defined.update((0, 2))
        t.execute(instruction, pc)
        self.assertEqual((t.f[3], t.instruction_count, t.routes['scalar']), (2.0, 1, 1))
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.f_defined.add(2)
        t.f[2] = -1.0
        self.reject(t, lambda: t.execute(instruction, pc), 'negative root')
        pc, instruction = 0x2A0C54, 0x460320C3
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.f_defined.update((3, 4))
        t.f[4], t.f[3] = 0.5, 0.0
        self.reject(t, lambda: t.execute(instruction, pc), 'zero denominator')
        pc, instruction = 0x2A0B08, 0xC5030000
        t = QuaternionTrace(controlled_image((pc, instruction)))
        t.r[8] = MATRIX
        for bits in (1, 0x7F800000, 0x7FC00000):
            Trace.save(t, MATRIX, bits, 4)
            self.reject(t, lambda: t.execute(instruction, pc), 'domain')

    def test_ordinary_likely_delays_and_preflight_rejections(self):
        pc = 0x2A0B24
        t = QuaternionTrace(controlled_image((pc, 0x4500001D), (pc + 4, 0x0080502D)))
        t.condition, t.r[4] = True, BUFFER
        self.assertEqual(t.advance(pc), pc + 8)
        self.assertEqual((t.r[10], t.instruction_count, t.visited), (BUFFER, 2, {pc, pc + 4}))
        pc = 0x2A0BC8
        t = QuaternionTrace(controlled_image((pc, 0x45030001), (pc + 4, 0x240D0002)))
        t.condition, t.r[13] = False, 1
        self.assertEqual(t.advance(pc), pc + 8)
        self.assertEqual((t.r[13], t.instruction_count, t.visited), (1, 1, {pc}))
        for delay in (0x10000000, 0x04010000, 0x45000000, 0x03E00008):
            pc = 0x2A0B24
            t = QuaternionTrace(controlled_image((pc, 0x4500001D), (pc + 4, delay)))
            self.reject(t, lambda: t.advance(pc), 'encoded control')
        t = QuaternionTrace(controlled_image((pc, 0x4500001D), (pc + 4, 0x0080502D)))
        t.instruction_count = COUNT_LIMIT - 1
        self.reject(t, lambda: t.advance(pc), 'delay instruction budget')

    def test_JR_delay_store_and_invalid_target(self):
        pc, instruction = 0x2A0B94, 0x03E00008
        t = QuaternionTrace(controlled_image((pc, instruction), (pc + 4, 0xE5400008)))
        t.r[10], t.r[31] = BUFFER, RETURN - 4
        Trace.save(t, BUFFER + 8, word(7.0), 4)
        t.f_defined.add(0)
        t.f[0] = 0.5
        self.reject(t, lambda: t.advance(pc), 'return')
        t.r[31] = RETURN
        self.assertEqual(t.advance(pc), RETURN)
        self.assertEqual(t.load(BUFFER + 8, 4), word(0.5))
        self.assertEqual(t.writes, [dict(pc=pc + 4, address=BUFFER + 8, bits=word(0.5))])
        self.assertEqual(t.instruction_count, 2)


class QuaternionOriginal(unittest.TestCase):
    def setUp(self):
        if not (ROOT / 'orig/SLUS_216.68').is_file():
            self.skipTest('private original absent')
        _, self.original = validated_elf(ROOT / 'orig/SLUS_216.68')

    def test_synthetic_mutable_table_zero_root_skips_division(self):
        m = [0.0] * 16
        m[0], m[5], m[10], m[15] = 1.0, -2.0, -2.0, 1.0
        c = make_fixture(self.original, m, [0, 1, 2], BUFFER, 'zero_root')
        self.assertNotIn(0x2A0C54, c['visited'])
        self.assertEqual([w['address'] for w in c['stores']], [BUFFER, BUFFER + 12, BUFFER, BUFFER])
        self.assertEqual(c['expected'][1:3], c['initial'][1:3])
        self.assertEqual(c['table_final'], [0, 1, 2])

    def test_positive_publication_and_live_alias_reload(self):
        m = [0.0] * 16
        m[0] = m[5] = m[10] = m[15] = 1.0
        plain = make_fixture(self.original, m, [1, 2, 0], BUFFER, 'plain')
        alias = make_fixture(self.original, m, [1, 2, 0], MATRIX + 24, 'alias')
        self.assertEqual([w['pc'] for w in plain['stores']], [0x2A0B58, 0x2A0B6C, 0x2A0B80, 0x2A0B98])
        self.assertLess(plain['visited'].index(0x2A0B54), plain['visited'].index(0x2A0B58))
        self.assertNotEqual(plain['expected'][:4], alias['expected'][22:26])


if __name__ == '__main__':
    unittest.main()
