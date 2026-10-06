"""Index reviewed-disassembler output to prioritize original game research.

References are static assembly symbol references, not proof of executed behavior
or original function names. Printable game strings and generated indexes remain
in ignored build/. This reuses spimdisasm's function and symbol analysis.
"""
import argparse
import csv
import json
import re
from pathlib import Path

from analyze import ROOT, write_json

ADDRESS_NAME = re.compile(r"\b(?:D_(?:FLT_|DBL_)?|func_|T_|STR_|FLT_|DBL_|RO_|B_)([0-9A-Fa-f]{8})\b")
INDIRECT_CALL = re.compile(r"/\*\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+[0-9A-Fa-f]+\s+\*/\s+jalr\s+(.*)")


def build_index():
    with (ROOT / "build/functions.csv").open(newline="", encoding="utf-8") as stream:
        candidates = list(csv.DictReader(stream))
    strings = {s["address"]: s["text"] for s in json.loads((ROOT / "build/strings.json").read_text(encoding="utf-8"))}
    functions = {}
    for row in candidates:
        name = row["name"]
        assembly_path = ROOT / "build/functions" / f"{name}.s"
        if not assembly_path.exists():
            continue
        source = assembly_path.read_text(encoding="utf-8")
        references = sorted({int(m.group(1), 16) for m in ADDRESS_NAME.finditer(source)})
        calls = [s for s in row["functions called by this function"].strip("[]").split(";") if s]
        functions[name] = {"name": name, "address": int(row["address"], 16),
                           "file_offset": int(row["vrom"], 16), "candidate_size": int(row["length"], 16),
                           "calls": calls, "callers": [], "assembly": assembly_path.relative_to(ROOT).as_posix(),
                           "indirect_calls": [{"address": int(m.group(1), 16), "operand": m.group(2).strip()}
                                              for m in INDIRECT_CALL.finditer(source)],
                           "referenced_addresses": references,
                           "strings": [{"address": a, "text": strings[a]} for a in references if a in strings]}
    for name, function in functions.items():
        for callee in set(function["calls"]):
            if callee in functions:
                functions[callee]["callers"].append(name)
    result = {"schema_version": 1, "kind": "heuristic_research_index", "functions": functions}
    write_json(ROOT / "build/function_index.json", result)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rebuild", action="store_true")
    parser.add_argument("--query", help="Case-insensitive substring of referenced strings")
    parser.add_argument("--address", help="Function address in hex")
    parser.add_argument("--limit", type=int, default=20)
    args = parser.parse_args()
    path = ROOT / "build/function_index.json"
    index = build_index() if args.rebuild or not path.exists() else json.loads(path.read_text(encoding="utf-8"))
    functions = list(index["functions"].values())
    if args.address:
        address = int(args.address, 16)
        functions = [f for f in functions if f["address"] == address]
    if args.query:
        functions = [f for f in functions if any(args.query.lower() in s["text"].lower() for s in f["strings"])]
    if not args.query and not args.address:
        print(f"Indexed {len(functions):,} candidate functions. Use --query or --address to inspect research leads.")
        return
    for function in functions[:max(args.limit, 0)]:
        print(json.dumps(function, indent=2))
    print(f"{len(functions)} candidate functions selected; names and boundaries remain provisional.")


if __name__ == "__main__":
    main()
