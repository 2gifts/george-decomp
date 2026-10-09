"""decomp.dev report conversion using synthetic progress rows."""
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import objdiff_report


def function(name, address, size, source, *, identical=True, different=0, language="C"):
    return {"name": name, "address": f"{address:#010x}", "expected_size": size, "source": source,
            "identical": identical, "different_bytes": different, "language": language}


def executable(candidates=10):
    return {"heuristic_function_candidates": candidates,
            "sections": [{"name": ".text", "address": 0x100000, "size": 0x20000},
                         {"name": ".rentext", "address": 0x120000, "size": 0x100},
                         {"name": ".vutext", "address": 0x120100, "size": 0x200},
                         {"name": ".data", "address": 0x120300, "size": 0x400}]}


def progress(functions, total=0x20300):
    exact = [f for f in functions if f["identical"] and f["language"] != "assembly"]
    return {"functions": functions, "total_code_bytes": total,
            "matched_code_bytes": sum(f["expected_size"] for f in exact), "matched_functions": len(exact)}


ROWS = [function("a", 0x100000, 0x10, "src/game/one.c"),
        function("b", 0x100010, 0x20, "src/game/one.c", identical=False, different=8),
        function("c", 0x110000, 0x40, "src/runtime/two.C"),
        function("d", 0x110040, 0x10, "src/runtime/three.s", language="assembly")]


class ObjdiffReportTests(unittest.TestCase):
    def test_totals_follow_verify_rules(self):
        report = objdiff_report.build_report(progress(ROWS), executable())
        m = report["measures"]
        self.assertEqual(report["version"], 2)
        self.assertEqual((m["total_code"], m["matched_code"]), ("131840", "80"))
        self.assertEqual((m["matched_functions"], m["total_functions"]), (2, 10))
        self.assertEqual(m["total_units"], len(report["units"]))

    def test_units_cover_sources_windows_and_other_code_sections(self):
        units = {u["name"]: u for u in objdiff_report.build_report(progress(ROWS), executable())["units"]}
        self.assertEqual(set(units), {"game/one", "runtime/two", "runtime/three", "unreviewed/text_00100000",
                                      "unreviewed/text_00110000", "unreviewed/rentext", "unreviewed/vutext"})
        self.assertEqual(units["game/one"]["measures"]["fuzzy_match_percent"], (0x10 * 100 + 0x20 * 75) / 0x30)
        self.assertEqual(units["runtime/three"]["measures"]["matched_code"], "0")
        self.assertEqual(units["unreviewed/text_00100000"]["measures"]["total_code"], str(0x10000 - 0x30))
        self.assertEqual(sum(u["measures"]["total_functions"] for u in units.values()), 10)

    def test_inconsistent_totals_and_overlaps_fail(self):
        stale = progress(ROWS)
        stale["matched_code_bytes"] += 4
        with self.assertRaises(ValueError):
            objdiff_report.build_report(stale, executable())
        overlap = ROWS + [function("e", 0x100018, 0x10, "src/game/four.c")]
        with self.assertRaises(ValueError):
            objdiff_report.build_report(progress(overlap), executable())


if __name__ == "__main__":
    unittest.main()
