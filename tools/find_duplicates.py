"""Find complete original bodies identical to already matched game C.

This is a research inventory, not automatic recovery. Complete instruction bytes
and disassembler endlabel geometry must agree. Entries and signatures still need
review, and each proposed source body must compile/link at its own address.
"""
import csv
import hashlib
import io
import json
import re

from elftools.elf.elffile import ELFFile
from analyze import ROOT, validated_elf, write_json
from verify import manifests, number, validate_target_function

INSTRUCTION = re.compile(r"/\*\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+\*/")


def main():
    _, original = validated_elf(ROOT / "orig/SLUS_216.68")
    elf = ELFFile(io.BytesIO(original))
    sections = [{"offset": elf.get_section_by_name(name)["sh_offset"],
                 "address": elf.get_section_by_name(name)["sh_addr"],
                 "size": elf.get_section_by_name(name)["sh_size"]}
                for name in (".text", ".rentext", ".vutext")]
    functions = manifests()
    registered = {number(function["address"]) for function in functions}
    templates = {}
    for function in functions:
        if (function["status"] != "matched" or not function["source"].startswith("src/game/")
                or not function["source"].endswith(".c")):
            continue
        offset, size = number(function["file_offset"]), number(function["size"])
        validate_target_function(function, sections)
        body = original[offset:offset + size]
        if len(body) != size or hashlib.sha256(body).hexdigest() != function["original_sha256"]:
            raise ValueError(f"Template fingerprint changed: {function['name']}")
        templates.setdefault(size, {}).setdefault(body, []).append(function)
    index_path = ROOT / "build/function_index.json"
    index = json.loads(index_path.read_text(encoding="utf-8"))["functions"] if index_path.exists() else {}
    leads = []
    with (ROOT / "build/functions.csv").open(newline="", encoding="utf-8") as stream:
        for row in csv.DictReader(stream):
            address, offset, inventory_size = (int(row[key], 16) for key in ("address", "vrom", "length"))
            if not re.fullmatch(r"func_[0-9A-Fa-f]{8}", row["name"]):
                raise ValueError("Unsupported function inventory name")
            if address in registered or offset < 0 or offset + inventory_size > len(original):
                continue
            for size, patterns in templates.items():
                if not size <= inventory_size <= size + 7:
                    continue
                body = original[offset:offset + size]
                if body not in patterns:
                    continue
                validate_target_function({"name": row["name"], "address": address,
                                          "file_offset": offset, "size": size}, sections)
                path = ROOT / "build/functions" / (row["name"] + ".s")
                if not path.is_file():
                    continue
                before, separator, _ = path.read_text(encoding="utf-8").partition("endlabel " + row["name"])
                instructions = [int(match[1], 16) for match in INSTRUCTION.finditer(before)]
                if (not separator or instructions != list(range(address, address + size, 4))):
                    continue
                info = index.get(row["name"], {})
                leads.append({"name": row["name"], "address": row["address"], "file_offset": row["vrom"],
                              "size": size, "original_sha256": hashlib.sha256(body).hexdigest(),
                              "templates": [{"name": function["name"], "source": function["source"]}
                                            for function in patterns[body]],
                              "callers": info.get("callers", []), "calls": info.get("calls", []),
                              "reviewed": False, "matched": False,
                              "entry_evidence": "Heuristic entry; exact full body and endlabel, excluding trailing alignment. Independent entry/signature review required."})
    output = ROOT / "build/duplicate_candidates.json"
    write_json(output, {"schema_version": 1, "kind": "unreviewed_exact_body_duplicate_leads",
                        "original_sha256": hashlib.sha256(original).hexdigest(),
                        "progress_added": 0, "functions": leads})
    print(f"Found {len(leads)} unreviewed duplicate leads / {sum(row['size'] for row in leads):,} bytes: {output}")
    print("No recovered functions or matching progress added.")


if __name__ == "__main__":
    main()
