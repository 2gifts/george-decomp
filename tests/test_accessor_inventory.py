"""Synthetic scanner checks; no original executable is required."""

from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from accessor_inventory import materializations, written_register


def immediate(opcode, rs, rt, value):
    return opcode << 26 | rs << 21 | rt << 16 | value & 0xFFFF


def words(*values):
    return struct.pack(f"<{len(values)}I", *values)


class MaterializationTests(unittest.TestCase):
    def setUp(self):
        self.high = immediate(15, 0, 6, 0x0040)
        self.low = immediate(9, 6, 6, 0x0020)
        self.store = immediate(43, 4, 6, 4)

    def test_straight_line_proof(self):
        proofs = materializations(words(self.high, 0, self.low, self.store), 0x1000)
        self.assertEqual(proofs[0x400020][0]["store_address"], "0x0000100C")
        self.assertEqual(proofs[0x400020][0]["store_base_register"], 4)

    def test_signed_low_half(self):
        low = immediate(9, 6, 6, 0xFFE0)
        self.assertIn(0x3FFFE0, materializations(words(self.high, low, self.store), 0x1000))

    def test_call_delay_store(self):
        call = 3 << 26 | 0x100
        self.assertIn(0x400020, materializations(words(self.high, self.low, call, self.store), 0x1000))

    def test_store_after_delay_rejected(self):
        call = 3 << 26 | 0x100
        self.assertEqual(materializations(words(self.high, self.low, call, 0, self.store), 0x1000), {})

    def test_integer_register_overwrite_rejected(self):
        overwrite = immediate(35, 5, 6, 0)
        self.assertEqual(materializations(words(self.high, self.low, overwrite, self.store), 0x1000), {})

    def test_store_conditional_overwrites_rejected(self):
        for opcode in (56, 60):
            with self.subTest(opcode=opcode):
                overwrite = immediate(opcode, 4, 6, 0)
                self.assertEqual(written_register(overwrite), 6)
                self.assertEqual(materializations(words(self.high, self.low, overwrite, self.store), 0x1000), {})

    def test_initial_lui_in_delay_slot_rejected(self):
        transfers = (3 << 26 | 0x100, 2 << 26 | 0x100, 0x03E00008,
                     immediate(4, 4, 5, 4), 17 << 26 | 8 << 21)
        for transfer in transfers:
            with self.subTest(word=transfer):
                self.assertEqual(materializations(words(transfer, self.high, self.low, self.store), 0x1000), {})

    def test_coprocessor_read_overwrite_rejected(self):
        for opcode in (16, 17, 18):
            with self.subTest(opcode=opcode):
                overwrite = opcode << 26 | 6 << 16
                self.assertEqual(written_register(overwrite), 6)
                self.assertEqual(materializations(words(self.high, self.low, overwrite, self.store), 0x1000), {})

    def test_mmi_and_lq_overwrites_rejected(self):
        for overwrite in (28 << 26 | 6 << 11, immediate(30, 4, 6, 0)):
            with self.subTest(word=overwrite):
                self.assertEqual(written_register(overwrite), 6)
                self.assertEqual(materializations(words(self.high, self.low, overwrite, self.store), 0x1000), {})

    def test_zero_register_and_traps_rejected(self):
        self.assertEqual(materializations(words(immediate(15, 0, 0, 0x40),
            immediate(9, 0, 0, 0x20), immediate(43, 4, 0, 4)), 0x1000), {})
        self.assertEqual(materializations(words(self.high, self.low, 12, self.store), 0x1000), {})


if __name__ == "__main__":
    unittest.main()
