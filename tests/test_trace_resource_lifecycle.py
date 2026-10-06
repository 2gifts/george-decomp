import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from trace_resource_lifecycle import ResourceLifecycleTrace, BUFFER, END, RANGES, CALLS, RETURN


class ResourceLifecycleGuards(unittest.TestCase):
    def trace(self):
        t = ResourceLifecycleTrace(b'')
        for a in range(BUFFER, BUFFER + 16):
            t.memory[a] = 0x55
        return t

    def test_signed_halfword_and_alignment(self):
        t = self.trace()
        t.r[4] = BUFFER
        t.save(BUFFER + 2, 0xFFFC, 2)
        self.assertEqual(t.execute((0x21 << 26) | (4 << 21) | (5 << 16) | 2, 0), (None, False))
        self.assertEqual(t.r[5], 0xFFFFFFFC)
        t.save(BUFFER + 2, 12, 2)
        t.execute((0x21 << 26) | (4 << 21) | (5 << 16) | 2, 0)
        self.assertEqual(t.r[5], 12)
        with self.assertRaises(ValueError):
            t.execute((0x21 << 26) | (4 << 21) | (5 << 16) | 1, 0)

    def test_narrow_memory_and_no_original_tables(self):
        t = self.trace()
        for a in (BUFFER - 1, END, 0x43A408, 0x4455D0, 0x4455A0):
            with self.assertRaises(ValueError): t.load(a, 4)
            with self.assertRaises(ValueError): t.save(a, 1, 4)
        with self.assertRaises(ValueError): t.save(END - 1, 1, 2)
        with self.assertRaises((KeyError, ValueError)): t.load(BUFFER + 32, 4)

    def test_complete_code_scope(self):
        t = self.trace()
        for pc in (RANGES[0][0], RANGES[1][1], RANGES[0][0] + 1, 0x2AD9A8):
            with self.assertRaises(ValueError): t.fetch(pc)
        with self.assertRaises(ValueError): t.run(RANGES[0][0] + 4)

    def test_controlled_calls(self):
        t = self.trace()
        for call in (*CALLS, 0x226FB0):
            with self.assertRaises(ValueError): t.library_call(call)

    def test_real_return_and_owned_delay(self):
        entry, end = RANGES[0]
        original = bytearray(end - 0xFF000)
        struct.pack_into('<II', original, entry - 0xFF000, 0x00800008, 0)
        t = ResourceLifecycleTrace(original)
        t.r[4] = t.r[31] = RETURN
        with self.assertRaises(ValueError): t.run(entry)
        struct.pack_into('<II', original, entry - 0xFF000, 0x0FFFFFFF, 0)
        t = ResourceLifecycleTrace(original)
        with self.assertRaises(ValueError): t.run(entry, 0x0FFFFFFC)
        struct.pack_into('<I', original, entry - 0xFF000, (4 << 26) | ((end - entry - 8) // 4))
        struct.pack_into('<I', original, end - 4 - 0xFF000, 0x03E00008)
        t = ResourceLifecycleTrace(original); t.r[31] = RETURN
        with self.assertRaises(ValueError): t.run(entry)

    def test_untaken_branch_delay_and_internal_fake_return(self):
        entry, end = RANGES[0]
        original = bytearray(end - 0xFF000)
        struct.pack_into('<II', original, entry - 0xFF000, 0x10020001, 0x03E00008)
        t = ResourceLifecycleTrace(original); t.r[2] = 1; t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'delay'): t.run(entry)
        struct.pack_into('<II', original, entry - 0xFF000, 0x00800008, 0)
        t = ResourceLifecycleTrace(original); t.r[4] = entry + 8
        with self.assertRaisesRegex(ValueError, 'return'): t.run(entry)

    def test_inherited_store_widths_and_reserved_operands(self):
        t = self.trace(); t.r[4] = BUFFER; t.r[5] = 0xABCD
        t.execute((0x28 << 26) | (4 << 21) | (5 << 16) | 7, 0)
        self.assertEqual(t.load(BUFFER + 6, 2), 0xCD55)
        t.execute((0x29 << 26) | (4 << 21) | (5 << 16) | 2, 0)
        self.assertEqual(t.load(BUFFER + 2, 2), 0xABCD)
        for w in (0x00810809, 0x00A1F809, 0x0081086D, 0x46010006, 0x44850801):
            with self.assertRaises(ValueError): t.execute(w, 0)


if __name__ == '__main__':
    unittest.main()
