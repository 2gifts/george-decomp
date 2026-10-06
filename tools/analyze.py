"""Validate this revision and reproduce ELF analysis and PS2 disassembly.

Uses established pyelftools, spimdisasm and Rabbitizer; generated assembly and
strings stay in ignored build/. Heuristic function detection is not a source
decompilation or proof of original function boundaries.
"""
import argparse
import csv
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "config/SLUS_216.68.json"


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def validated_elf(path):
    spec = json.loads(CONFIG.read_text(encoding="utf-8"))
    data = path.read_bytes()
    if len(data) != spec["executable_size"] or hashlib.sha256(data).hexdigest() != spec["executable_sha256"]:
        raise ValueError("Executable does not match the supported SLUS_216.68 revision")
    return spec, data


def analyze(path):
    spec, data = validated_elf(path)
    with path.open("rb") as stream:
        elf = ELFFile(stream)
        if elf.elfclass != 32 or not elf.little_endian or elf["e_machine"] != "EM_MIPS":
            raise ValueError("Expected an ELF32 little-endian MIPS executable")
        sections = [{"name": s.name, "address": s["sh_addr"], "offset": s["sh_offset"],
                     "size": s["sh_size"], "type": s["sh_type"], "flags": s["sh_flags"],
                     "alignment": s["sh_addralign"]} for s in elf.iter_sections()]
        report = {"revision": spec["serial"], "sha256": spec["executable_sha256"],
                  "entry_point": elf["e_entry"], "elf_flags": elf["e_flags"],
                  "has_symbols": elf.get_section_by_name(".symtab") is not None,
                  "has_mdebug": elf.get_section_by_name(".mdebug") is not None,
                  "sections": sections, "segments": [dict(s.header) for s in elf.iter_segments()]}
        # String contents are local research material, not publishable progress.
        strings = []
        for section in elf.iter_sections():
            if section["sh_type"] != "SHT_PROGBITS" or section["sh_flags"] & 4:
                continue
            for match in re.finditer(rb"[\x20-\x7e]{6,}", section.data()):
                strings.append({"address": section["sh_addr"] + match.start(),
                                "section": section.name, "text": match.group().decode("ascii")})
    write_json(ROOT / "build/analysis.json", report)
    write_json(ROOT / "build/strings.json", strings)
    return report


def disassemble(path, report):
    text = next(s for s in report["sections"] if s["name"] == ".text")
    spec = json.loads(CONFIG.read_text(encoding="utf-8"))
    cmd = [sys.executable, "-m", "spimdisasm.singleFileDisasm", str(path), "build/disasm",
           "--start", f'{text["offset"]:X}', "--end", f'{text["offset"] + text["size"]:X}',
           "--vram", f'{text["address"]:X}', "--endian", "little", "--instr-category", "r5900",
           "--compiler", "EEGCC", "--abi", "EABI32", "--gp", spec["gp"],
           "--no-libultra-syms", "--no-ique-syms", "--no-hardware-regs", "--no-filter-low-addresses",
           "--no-use-fpccsr", "--no-cop0-named-registers", "--quiet",
           "--split-functions", "build/functions", "--function-info", "build/functions.csv",
           "--save-context", "build/context"]
    with (ROOT / "build/disassembler.log").open("w", encoding="utf-8") as log:
        subprocess.run(cmd, cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    with (ROOT / "build/functions.csv").open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    report["heuristic_function_candidates"] = len(rows)
    report["disassembler"] = "spimdisasm 1.42.4 / Rabbitizer 1.16.2 (R5900)"
    write_json(ROOT / "build/analysis.json", report)
    print(f"Detected {len(rows):,} candidate function regions; boundaries require review.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=ROOT / "orig/SLUS_216.68")
    parser.add_argument("--disassemble", action="store_true")
    args = parser.parse_args()
    report = analyze(args.elf)
    print(f'Validated {report["revision"]}: {len(report["sections"])} sections, entry 0x{report["entry_point"]:08X}.')
    if args.disassemble:
        disassemble(args.elf, report)


if __name__ == "__main__":
    main()
