"""Reject unsupported memory, controls and callback shapes independently of ROM."""
from pathlib import Path
import struct
import sys
import subprocess
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from trace_allocator_wrappers import AllocatorWrapperTrace, CORES, ENTRIES, IMPURE, LOCK, RANGES, REENTS, STACK
from trace_geometry import RETURN


class AllocatorGuards(unittest.TestCase):
    def trace(self, original=b''):
        return AllocatorWrapperTrace(original)

    def test_owned_memory_width_alignment_and_uninitialized(self):
        t = self.trace()
        t.save(IMPURE, REENTS[0], 4)
        self.assertEqual(t.load(IMPURE, 4), REENTS[0])
        for a, n in ((IMPURE, 8), (IMPURE + 1, 4), (STACK[0] - 1, 1), (STACK[1], 4),
                     (STACK[1] - 4, 8), (0x4059F8, 4), (0x20000, 4)):
            with self.assertRaises(ValueError): t.load(a, n)
            with self.assertRaises(ValueError): t.save(a, 1, n)
        with self.assertRaises(KeyError): t.load(STACK[0], 4)

    def test_complete_instruction_scope_and_image_bounds(self):
        t = self.trace()
        for pc in (ENTRIES[0], ENTRIES[0] + 1, RANGES[0][1], ENTRIES[1], 0x3966E8):
            with self.assertRaises(ValueError): t.fetch(pc)
        with self.assertRaises(ValueError): t.run(ENTRIES[0] + 4)
        with self.assertRaises(ValueError): t.run(ENTRIES[0], ENTRIES[0] + 4)

    def test_callback_order_and_fresh_reent(self):
        t = self.trace()
        t.save(IMPURE, REENTS[0], 4)
        t.r[4] = REENTS[1]
        with self.assertRaises(ValueError): t.library_call(LOCK)
        t.r[4] = REENTS[0]
        with self.assertRaises(ValueError): t.library_call(CORES[0])
        with self.assertRaises(ValueError): t.library_call(0x363750)
        t.library_call(LOCK)
        t.r[4] = REENTS[0]
        with self.assertRaises(ValueError): t.library_call(CORES[1])

    def test_reserved_and_unreviewed_operands(self):
        t = self.trace()
        for w in (0x0081902D, 0x0080906D, 0x24840001, 0x3C110040, 0x8E055694,
                  0x7FB00020, 0xDFB10000, 0x00800008, 0x10000001, 0):
            with self.assertRaises(ValueError): t.execute(w, 0)

    def test_fake_return_and_terminal_delay_bounds(self):
        start, end = RANGES[0]
        original = bytearray(end - 0xFF000)
        struct.pack_into('<II', original, start - 0xFF000, 0x03E00008, 0x27BD0040)
        t = self.trace(original)
        t.r[29] = 0x80000
        t.r[31] = RETURN
        with self.assertRaisesRegex(ValueError, 'terminal'): t.run(start)
        struct.pack_into('<II', original, start - 0xFF000, 0x0C0E59BA, 0x03E00008)
        t = self.trace(original)
        with self.assertRaisesRegex(ValueError, 'delay'): t.run(start)

    def test_mutation_reloads_and_return_capture(self):
        t = AllocatorWrapperTrace(b'', mutation=7, result=0xFFFFFFFF)
        t.save(IMPURE, REENTS[0], 4)
        for target in (LOCK, CORES[0], 0x396748):
            t.r[4] = t.load(IMPURE, 4)
            t.library_call(target)
        self.assertEqual(t.load(IMPURE, 4), REENTS[3])
        self.assertEqual(t.stage, 3)
        self.assertEqual([t.events[i] for i in (1, 6, 11)], list(REENTS[:3]))
        self.assertEqual(t.r[2], 0xA5A5A5A5)
        with self.assertRaises(ValueError): t.library_call(LOCK)

    def test_missing_original_writes_no_fixture_or_header(self):
        root = Path(__file__).resolve().parents[1]
        with tempfile.TemporaryDirectory(dir=root / 'build') as directory:
            out = Path(directory)
            result = subprocess.run([sys.executable, str(root / 'tools/trace_allocator_wrappers.py'),
                '--elf', str(out / 'missing.elf'), '--output', str(out / 'trace.json'),
                '--golden-header', str(out / 'golden.h')], capture_output=True, text=True, timeout=10)
            self.assertNotEqual(result.returncode, 0)
            self.assertFalse((out / 'trace.json').exists())
            self.assertFalse((out / 'golden.h').exists())

    def test_wrong_original_hash_writes_no_fixture(self):
        root = Path(__file__).resolve().parents[1]
        with tempfile.TemporaryDirectory(dir=root / 'build') as directory:
            out = Path(directory)
            (out / 'wrong.elf').write_bytes(b'not the validated original')
            result = subprocess.run([sys.executable, str(root / 'tools/trace_allocator_wrappers.py'),
                '--elf', str(out / 'wrong.elf'), '--output', str(out / 'trace.json')],
                capture_output=True, text=True, timeout=10)
            self.assertNotEqual(result.returncode, 0)
            self.assertFalse((out / 'trace.json').exists())


if __name__ == '__main__':
    unittest.main()
