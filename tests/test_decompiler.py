"""Candidate input guards use synthetic instruction bytes, never game data."""
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import decompile_candidates as candidate


class CandidateGuards(unittest.TestCase):
    ROW = {"name": "func_00100000", "address": 0x100000, "offset": 4, "size": 12}
    SECTIONS = [{"address": 0x100000, "offset": 4, "size": 16}]
    ORIGINAL = bytes(4) + bytes.fromhex("040000460800e003") + bytes(8)
    SOURCE = """.align 3
nonmatching func_00100000, 0x8
glabel func_00100000
    /* 000004 00100000 04000046 */ c1 0x4
    /* 000008 00100004 0800E003 */ jr $31
endlabel func_00100000
    /* 00000C 00100008 00000000 */ nop
"""

    def check(self, source=None, row=None, original=None):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.s"
            path.write_text(self.SOURCE if source is None else source, encoding="utf-8")
            return candidate.checked_assembly(path, self.ROW if row is None else row,
                                               self.ORIGINAL if original is None else original, self.SECTIONS)

    def test_proven_sqrt_alias_is_logged_and_padding_is_excluded(self):
        source, changes, digest, size = self.check()
        self.assertIn("sqrt.s      $f0, $f0", source)
        self.assertNotIn("00100008", source)
        self.assertEqual(size, 8)
        self.assertEqual(changes[0]["address"], "0x00100000")
        self.assertEqual(len(digest), 64)

    def test_proven_ee_truncation_alias_is_logged(self):
        source = self.SOURCE.replace("04000046", "64000046").replace("c1 0x4", ".word 0x46000064 # cvt.w.s")
        original = self.ORIGINAL[:4] + bytes.fromhex("64000046") + self.ORIGINAL[8:]
        normalized, changes, _, _ = self.check(source=source, original=original)
        self.assertIn("trunc.w.s   $f1, $f0", normalized)
        self.assertEqual(changes[0]["to"], "trunc.w.s $f1, $f0")

    def test_altered_word_or_address_is_rejected(self):
        for source in (self.SOURCE.replace("04000046", "00000000"),
                       self.SOURCE.replace("00100004", "00100008")):
            with self.subTest(source=source), self.assertRaises(ValueError):
                self.check(source)

    def test_geometry_and_nonzero_excluded_padding_are_rejected(self):
        row = {**self.ROW, "address": 0x100004}
        with self.assertRaisesRegex(ValueError, "outside executable"):
            self.check(row=row)
        with self.assertRaisesRegex(ValueError, "Nonzero bytes"):
            self.check(original=self.ORIGINAL[:12] + b"BAD!" + self.ORIGINAL[16:])

    def test_missing_boundary_or_external_assembly_is_rejected(self):
        for source in (self.SOURCE.replace("endlabel", "#endlabel"),
                       self.SOURCE + '\n.include "external.s"\n'):
            with self.subTest(source=source), self.assertRaises(ValueError):
                self.check(source)

    def test_generated_output_stays_inside_ignored_directories(self):
        with tempfile.TemporaryDirectory() as folder, patch.object(candidate, "ROOT", Path(folder)):
            root = Path(folder).resolve()
            self.assertEqual(candidate.checked_output(root / "build/draft"), root / "build/draft")
            self.assertEqual(candidate.checked_output(root / ".local/draft"), root / ".local/draft")
            for destination in (root / "src/draft", root.parent / "outside-draft"):
                with self.subTest(destination=destination), self.assertRaises(ValueError):
                    candidate.checked_output(destination)


if __name__ == "__main__":
    unittest.main()
