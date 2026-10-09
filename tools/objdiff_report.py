"""Convert the tracked progress report into an objdiff report for decomp.dev.

Only the committed reports/ files are read, so CI needs no game executable.
Reviewed functions are grouped by source file. Code no function covers is
grouped into fixed address windows, so the treemap shows where unreviewed code
lives. Match rules come from verify.py: only identical C/C++ functions count,
and reused assembly stays out of the matched totals.
"""
import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORT_VERSION = 2
CODE_SECTIONS = (".text", ".rentext", ".vutext")
WINDOW = 0x10000


def matched(function):
    return function["identical"] and function["language"] in ("C", "C++")


def fuzzy_percent(function):
    """Percent of original bytes reproduced; reused assembly earns nothing."""
    if function["language"] not in ("C", "C++"):
        return 0.0
    if function["identical"]:
        return 100.0
    return max(0.0, 100.0 * (1 - function["different_bytes"] / function["expected_size"]))


def measures(items, function_count=None):
    """objdiff measures for (size, fuzzy, matched) items. u64 fields are strings, as objdiff writes them."""
    total = sum(size for size, _, _ in items)
    exact = sum(size for size, _, is_matched in items if is_matched)
    functions = len(items) if function_count is None else function_count
    exact_functions = sum(is_matched for _, _, is_matched in items)
    return {"fuzzy_match_percent": sum(size * fuzzy for size, fuzzy, _ in items) / total if total else 0.0,
            "total_code": str(total), "matched_code": str(exact),
            "matched_code_percent": 100 * exact / total if total else 0.0,
            "total_data": "0", "matched_data": "0", "matched_data_percent": 0.0,
            "total_functions": functions, "matched_functions": exact_functions,
            "matched_functions_percent": 100 * exact_functions / functions if functions else 0.0,
            "complete_code": "0", "complete_code_percent": 0.0, "complete_data": "0", "complete_data_percent": 0.0,
            "total_units": 1, "complete_units": 0}


def unit_name(source):
    path = Path(source)
    if path.parts[0] == "src":
        path = Path(*path.parts[1:])
    return path.with_suffix("").as_posix()


def source_units(functions, sections):
    text = sections[".text"]["address"]
    by_source = {}
    for function in functions:
        by_source.setdefault(function["source"], []).append(function)
    names = [unit_name(source) for source in by_source]
    if len(set(names)) != len(names):
        raise ValueError("Two sources share a unit name")
    units = []
    for source, members in sorted(by_source.items()):
        members.sort(key=lambda f: int(f["address"], 16))
        items = [(f["expected_size"], fuzzy_percent(f), matched(f)) for f in members]
        units.append({"name": unit_name(source), "measures": measures(items),
                      "sections": [{"name": ".text", "size": str(sum(size for size, _, _ in items)),
                                    "fuzzy_match_percent": measures(items)["fuzzy_match_percent"]}],
                      "functions": [{"name": f["name"], "size": str(f["expected_size"]),
                                     "fuzzy_match_percent": fuzzy_percent(f),
                                     "address": str(int(f["address"], 16) - text),
                                     "metadata": {"virtual_address": str(int(f["address"], 16))}}
                                    for f in members],
                      "metadata": {"source_path": source, "auto_generated": False}})
    return units


def unreviewed_spans(functions, sections):
    """Yield (section, start, end) code ranges not covered by any reviewed function."""
    covered = sorted((int(f["address"], 16), f["expected_size"]) for f in functions)
    for name in CODE_SECTIONS:
        start, end = sections[name]["address"], sections[name]["address"] + sections[name]["size"]
        cursor = start
        for address, size in covered:
            if address + size <= cursor or address >= end:
                continue
            if address < cursor:
                raise ValueError(f"Overlapping functions at {address:#010x}")
            if address > cursor:
                yield name, cursor, address
            cursor = address + size
        if cursor < end:
            yield name, cursor, end


def unreviewed_units(functions, sections, candidates):
    """Group uncovered bytes by section and address window.

    analyze.py's candidate count is not committed per region, so the unreviewed
    function count is spread over .text windows by size. It is an estimate that
    keeps the function denominator near the README's candidate total.
    """
    windows = {}
    for name, start, end in unreviewed_spans(functions, sections):
        while start < end:
            key = (name, start - start % WINDOW if name == ".text" else sections[name]["address"])
            stop = min(end, key[1] + WINDOW) if name == ".text" else end
            windows[key] = windows.get(key, 0) + stop - start
            start = stop
    text_windows = [key for key in windows if key[0] == ".text"]
    text_bytes = sum(windows[key] for key in text_windows)
    remaining = max(0, candidates - len(functions))
    shares = {key: remaining * windows[key] // text_bytes for key in text_windows}
    for key in sorted(text_windows, key=lambda k: -(remaining * windows[k] % text_bytes))[:remaining - sum(shares.values())]:
        shares[key] += 1
    units = []
    for (name, address), size in sorted(windows.items(), key=lambda item: item[0][1]):
        label = f"unreviewed/{name.lstrip('.')}_{address:08X}" if name == ".text" else f"unreviewed/{name.lstrip('.')}"
        unit_measures = measures([(size, 0.0, False)], shares.get((name, address), 0))
        units.append({"name": label, "measures": unit_measures,
                      "sections": [{"name": name, "size": str(size), "fuzzy_match_percent": 0.0}],
                      "functions": [], "metadata": {"auto_generated": True}})
    return units


def build_report(progress, executable):
    sections = {s["name"]: s for s in executable["sections"] if s["name"] in CODE_SECTIONS}
    functions = progress["functions"]
    units = source_units(functions, sections) + unreviewed_units(
        functions, sections, executable.get("heuristic_function_candidates", 0))
    items = [(f["expected_size"], fuzzy_percent(f), matched(f)) for f in functions]
    uncovered = sum(int(u["measures"]["total_code"]) for u in units if u["name"].startswith("unreviewed/"))
    function_total = sum(u["measures"]["total_functions"] for u in units)
    total = measures(items + [(uncovered, 0.0, False)], function_total)
    total["total_units"] = len(units)
    if int(total["total_code"]) != progress["total_code_bytes"]:
        raise ValueError(f"Code total {total['total_code']} differs from {progress['total_code_bytes']}")
    if int(total["matched_code"]) != progress["matched_code_bytes"]:
        raise ValueError(f"Matched code {total['matched_code']} differs from {progress['matched_code_bytes']}")
    if total["matched_functions"] != progress["matched_functions"]:
        raise ValueError(f"Matched functions {total['matched_functions']} differ from {progress['matched_functions']}")
    return {"measures": total, "units": units, "version": REPORT_VERSION, "categories": []}


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("-o", "--output", type=Path, default=ROOT / "build/report.json")
    args = parser.parse_args()
    progress = json.loads((ROOT / "reports/progress.json").read_text(encoding="utf-8"))
    executable = json.loads((ROOT / "reports/executable.json").read_text(encoding="utf-8"))
    report = build_report(progress, executable)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=1) + "\n", encoding="utf-8")
    m = report["measures"]
    print(f'{m["matched_code"]}/{m["total_code"]} code bytes ({m["matched_code_percent"]:.6f}%), '
          f'{m["matched_functions"]}/{m["total_functions"]} functions, {m["total_units"]} units -> {args.output}')


if __name__ == "__main__":
    main()
