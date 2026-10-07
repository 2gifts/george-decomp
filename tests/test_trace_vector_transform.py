"""Strict complete-entry, VU/MMI operand, alias and delay regressions."""
import math
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import trace_vector_transform as model
from analyze import validated_elf
from trace_geometry import RETURN, word


class TransformGuards(unittest.TestCase):
    def trace(self, instructions=()):
        image = bytearray(model.RANGES[-1][1] - 0xFF000)
        for i, instruction in enumerate(instructions):
            struct.pack_into('<I', image, model.ENTRIES[0] - 0xFF000 + i * 4, instruction)
        return model.VectorTrace(bytes(image))

    def test_complete_entry_initialized_memory_and_instruction_bounds(self):
        t = self.trace()
        for address in (model.ENTRIES[0] - 4, model.ENTRIES[0] + 1, model.RANGES[0][1], model.RANGES[1][1]):
            with self.assertRaises(ValueError):
                t.fetch(address)
        with self.assertRaises(ValueError):
            model.VectorTrace(b'').fetch(model.ENTRIES[0])
        with self.assertRaises(ValueError):
            t.run(model.ENTRIES[0] + 4)
        for i in range(16):
            t.memory[model.BUFFER + i] = 0
        t.save(model.BUFFER, 123, 16)
        self.assertEqual(t.load(model.BUFFER, 16), 123)
        for address, size in ((model.BUFFER + 1, 4), (model.BUFFER + 4, 16),
                              (model.BUFFER + 16, 4), (model.END, 4), (model.BUFFER, 8)):
            with self.assertRaises(ValueError):
                t.load(address, size)
        t.instruction_count = 64
        with self.assertRaisesRegex(ValueError, 'bound'):
            t.run(model.ENTRIES[0])

    def test_original_quad_pack_and_result_extract_lanes(self):
        t = self.trace()
        t.r[7], t.r[11], t.r[10], t.r[5] = word(2), word(1), word(3), word(4)
        for instruction in (0x71674488, 0x71454C88, 0x71091488, 0x48A22800):
            t.execute(instruction, model.ENTRIES[0])
        self.assertEqual(t.vf[5], [4, 2, 3, 1])
        t.execute(0x48222800, model.ENTRIES[0])
        t.execute(0x70421BA9, model.ENTRIES[0])
        self.assertEqual(t.r[3] & 0xFFFFFFFF, word(3))
        t.execute(0x0002183E, model.ENTRIES[0])
        self.assertEqual(t.r[3] & 0xFFFFFFFF, word(2))
        for instruction in (0x70000000, 0x48A22801, 0x48222801, 0x0022183E):
            with self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRIES[0])

    def test_accumulator_producer_result_preservation_and_encoding_guards(self):
        t = self.trace()
        t.vf[4], t.vf[3], t.vf[2], t.vf[1] = ([1,2,3,4], [5,6,7,8], [9,10,11,12], [13,14,15,16])
        t.vf[5] = [2,3,4,1]
        with self.assertRaisesRegex(ValueError, 'before producer'):
            t.execute(0x4BE518BD, model.ENTRIES[0])
        t.execute(0x4BE521BC, model.ENTRIES[0])
        t.execute(0x4BE518BD, model.ENTRIES[0])
        self.assertEqual(t.vu_acc, [17,22,27,32])
        before = list(t.vu_acc)
        t.execute(0x4BE5114A, model.ENTRIES[0])
        self.assertEqual(t.vf[5], [53,62,71,80])
        self.assertEqual(t.vu_acc, before)
        t.vf[5] = [2,3,4,1]
        t.execute(0x4BE510BE, model.ENTRIES[0])
        t.execute(0x4BE0094B, model.ENTRIES[0])
        self.assertEqual(t.vf[5], [66,76,86,96])
        for instruction in (0x4BC521BC, 0x4BE521FC, 0x4BE518FD, 0x4BE518BF):
            with self.assertRaises(ValueError):
                t.execute(instruction, model.ENTRIES[0])

    def test_separate_binary32_products_finite_domain_and_unused_lanes(self):
        t = self.trace()
        t.vf[4] = [-(1 + 2**-22)] * 4
        t.vf[3] = [1 + 2**-23] * 4
        t.vf[5] = [1,1 + 2**-23,0,1]
        t.execute(0x4BE521BC, model.ENTRIES[0])
        t.execute(0x4BE518BD, model.ENTRIES[0])
        self.assertEqual(t.vu_acc, [0.0] * 4)
        self.assertNotEqual(-(1 + 2**-22) + (1 + 2**-23)**2, 0.0)
        for value in (math.inf, math.nan, struct.unpack('<f', struct.pack('<I',1))[0]):
            t.vf[4] = [1,2,3,value]  # Original executes the fourth lane too.
            t.vf[5] = [1,0,0,1]
            with self.assertRaises(ValueError):
                t.execute(0x4BE521BC, model.ENTRIES[0])

    def test_actual_return_delay_encoding_and_no_callee_transfers(self):
        for delay in ((4 << 26) | (4 << 21) | (5 << 16) | 1,
                      (1 << 26) | (4 << 21) | (1 << 16) | 1,
                      (17 << 26) | (8 << 21) | 1, (3 << 26), 0x03E00008):
            t = self.trace((0x03E00008, delay))
            t.r[4], t.r[5], t.r[31] = 1,2,RETURN
            with self.assertRaisesRegex(ValueError, 'control transfer in delay'):
                t.run(model.ENTRIES[0])
        t = self.trace(((2 << 21) | 8,0))
        t.r[2] = RETURN
        with self.assertRaisesRegex(ValueError, 'actual JR31'):
            t.run(model.ENTRIES[0])
        with self.assertRaises(ValueError):
            t.library_call(model.ENTRIES[1])

    @unittest.skipUnless((model.ROOT / 'orig/SLUS_216.68').is_file(),
                         'requires locally supplied original ELF')
    def test_actual_matrix_and_input_aliases_capture_before_three_stores(self):
        _, original = validated_elf(model.ROOT / 'orig/SLUS_216.68')
        for routine in (0,1):
            reference = model.fixture(original, routine)
            for offset in (0,4,16,48,52,0x80,0x84):
                case = model.fixture(original, routine, output_offset=offset)
                target = offset // 4
                expected = list(case['initial'])
                expected[target:target+3] = reference['expected'][0xC0//4:0xC0//4+3]
                self.assertEqual(case['expected'], expected)
                self.assertEqual(case['instruction_count'], 31 if routine == 0 else 29)


if __name__ == '__main__':
    unittest.main()
