"""C/C++ reporting guards using synthetic byte comparisons and public gates."""
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import verify


def row(name, source, expected=b"abcd", actual=None, *, category="game", relocations=(), data=()):
    comparison = verify.compare_function(expected, expected if actual is None else actual, relocations)
    comparison = verify.gate_generated_comparison(comparison, data)
    return {"name": name, "source": source, "category": category,
            "language": verify.source_language({"source": source}), **comparison}


class ReportingTests(unittest.TestCase):
    def test_extensions_uppercase_c_and_legacy_fallback(self):
        cases = {"one.c": "C", "one.C": "C++", "one.cc": "C++", "one.cpp": "C++",
                 "one.cxx": "C++", "one.S": "assembly", "one.s": "assembly",
                 "one.asm": "assembly", "one": "C", "one.generated": "C"}
        for source, language in cases.items():
            with self.subTest(source=source):
                self.assertEqual(verify.source_language({"source": source}), language)

    def test_explicit_kind_and_unknown_suffix(self):
        for source, kind, language in (("one.c", "c", "C"), ("one.C", "cxx", "C++"),
                                        ("one.cc", "cxx", "C++"), ("one.S", "assembly", "assembly"),
                                        ("one.generated", "cxx", "C++")):
            with self.subTest(source=source, kind=kind):
                self.assertEqual(verify.source_language({"source": source, "source_kind": kind}), language)

    def test_contradictory_or_malformed_kinds_fail(self):
        for source, kind in (("one.c", "cxx"), ("one.C", "c"), ("one.cpp", "assembly"),
                             ("one.s", "c"), ("one.asm", "cxx"), ("one.c", "C"),
                             ("one.c", "unknown"), ("one.c", None), ("one.c", True),
                             ("one.c", []), ("one.c", {})):
            with self.subTest(source=source, kind=kind):
                with self.assertRaises(ValueError):
                    verify.source_language({"source": source, "source_kind": kind})

    def mixed(self):
        return [row("c_game", "src/game/a.c"),
                row("c_runtime", "src/runtime/b.c", b"12345678", category="runtime"),
                row("c_nonmatch", "src/game/c.c", b"abcdef", b"abcxef"),
                row("cpp_runtime", "src/runtime/d.cc", b"123456789012", category="runtime"),
                row("cpp_game", "src/game/e.C", b"1234567890123456"),
                row("cpp_nonmatch", "src/runtime/f.cpp", b"1234567890", b"123456789", category="runtime"),
                row("asm_exact", "src/runtime/g.S", b"a" * 20, category="runtime"),
                row("asm_nonmatch", "src/runtime/h.s", b"a" * 24, b"a" * 23, category="runtime")]

    def test_mixed_source_totals_and_assembly_exclusion(self):
        p = verify.source_progress(self.mixed(), 1000)
        expected = {"reconstructed_c_functions": 3, "reconstructed_c_code_bytes": 18,
                    "matched_c_functions": 2, "matched_c_code_bytes": 12,
                    "reconstructed_cpp_functions": 3, "reconstructed_cpp_code_bytes": 38,
                    "matched_cpp_functions": 2, "matched_cpp_code_bytes": 28,
                    "reconstructed_source_functions": 6, "reconstructed_source_code_bytes": 56,
                    "matched_functions": 4, "matched_code_bytes": 40,
                    "matched_game_functions": 2, "matched_runtime_functions": 2,
                    "reused_assembly_functions": 1, "reused_assembly_code_bytes": 20,
                    "matched_code_percent": 4.0}
        self.assertEqual(p, expected)
        self.assertEqual(p["matched_c_code_bytes"] + p["matched_cpp_code_bytes"], p["matched_code_bytes"])

    def test_full_bytes_size_relocations_and_generated_data_are_gates(self):
        exact = row("exact", "x.cpp")
        bad = [row("byte", "x.cpp", actual=b"abce"), row("size", "x.cpp", actual=b"abc"),
               row("reloc", "x.cpp", relocations=(0,)),
               row("data", "x.cpp", data=[{"identical": False}])]
        p = verify.source_progress([exact] + bad, 1000)
        self.assertEqual(p["reconstructed_cpp_functions"], 5)
        self.assertEqual(p["matched_cpp_functions"], 1)
        self.assertEqual(p["matched_cpp_code_bytes"], 4)
        self.assertEqual(p["reconstructed_c_functions"], 0)
        self.assertTrue(bad[-1]["code_identical"])
        self.assertFalse(bad[-1]["generated_data_identical"])

    def test_empty_and_assembly_only_reports_have_no_source_credit(self):
        for rows in ([], [row("asm", "x.s", b"a" * 100)]):
            p = verify.source_progress(rows, 1000)
            for key in ("matched_functions", "matched_code_bytes", "reconstructed_c_functions",
                        "reconstructed_cpp_functions", "reconstructed_source_functions"):
                self.assertEqual(p[key], 0)
        with self.assertRaises(ValueError):
            verify.source_progress([{**row("unknown", "x.c"), "language": "Rust"}], 1000)

    def test_existing_c_only_fields_keep_values(self):
        rows = [row("game", "a.c"), row("runtime", "b.c", category="runtime"),
                row("nonmatch", "c.c", actual=b"abce"), row("asm", "d.S")]
        p = verify.source_progress(rows, 1000)
        old = {"reconstructed_c_functions": 3, "reconstructed_c_code_bytes": 12,
               "matched_functions": 2, "matched_game_functions": 1, "matched_runtime_functions": 1,
               "reused_assembly_functions": 1, "reused_assembly_code_bytes": 4,
               "matched_code_bytes": 8, "matched_code_percent": 0.8}
        self.assertEqual({key: p[key] for key in old}, old)
        self.assertEqual(p["reconstructed_source_functions"], p["reconstructed_c_functions"])

    def test_existing_manifest_overlap_gate_prevents_double_count(self):
        with tempfile.TemporaryDirectory() as directory, patch.object(verify, "ROOT", Path(directory)):
            config = Path(directory) / "config"
            config.mkdir()
            first = {"name": "same", "address": "0x1000", "size": 16, "source": "a.c"}
            second = {**first, "source": "a.cpp"}
            (config / "recovered_functions.json").write_text(json.dumps({"functions": [first]}))
            (config / "runtime_functions.json").write_text(json.dumps({"functions": [second]}))
            with self.assertRaisesRegex(ValueError, "overlap"):
                verify.manifests()
            second["address"] = "0x1010"
            (config / "runtime_functions.json").write_text(json.dumps({"functions": [second]}))
            self.assertEqual(len(verify.manifests()), 2)

    def test_readme_and_cli_labels_include_both_languages(self):
        rows = self.mixed()
        progress = {**verify.source_progress(rows, 1000), "total_code_bytes": 1000, "functions": rows}
        with tempfile.TemporaryDirectory() as directory, patch.object(verify, "ROOT", Path(directory)):
            readme = Path(directory) / "README.md"
            readme.write_text("before\n<!-- progress:start -->old<!-- progress:end -->\nafter\n")
            verify.publish_readme_progress(progress)
            text = readme.read_text()
            self.assertTrue(text.startswith("before\n"))
            self.assertTrue(text.endswith("\nafter\n"))
            for expected in ("Recovered game C/C++", "Reused upstream C/C++",
                             "| C source | 3 functions reviewed; 2 match (12 bytes)",
                             "| C++ source | 3 functions reviewed; 2 match (28 bytes)",
                             "40 / 1,000 code bytes", "`cpp_runtime`", "do **not** count as C/C++ progress"):
                self.assertIn(expected, text)
            self.assertNotIn("`asm_exact`", text)
            self.assertNotIn("`cpp_nonmatch`", text)
            lines = verify.progress_summary(progress)
            self.assertIn("4/6 C/C++ functions match", lines[0])
            self.assertIn("Game: 2 matches. Reused runtime: 2 matches.", lines[1])
            self.assertIn("excluded from C/C++ progress", lines[2])

    def test_readme_missing_or_ambiguous_markers_still_fail(self):
        progress = {**verify.source_progress([], 1000), "functions": [], "total_code_bytes": 1000}
        with tempfile.TemporaryDirectory() as directory, patch.object(verify, "ROOT", Path(directory)):
            readme = Path(directory) / "README.md"
            for text in ("", "<!-- progress:start --><!-- progress:start --><!-- progress:end -->"):
                readme.write_text(text)
                with self.assertRaisesRegex(ValueError, "markers"):
                    verify.publish_readme_progress(progress)



if __name__ == "__main__":
    unittest.main()
