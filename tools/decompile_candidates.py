"""Run pinned upstream m2c on local, revision-checked function candidates.

Outputs are research drafts in ignored build/ or .local/. They never update
function manifests, recovered-source counts, or byte-match status.
"""
import argparse
import csv
import hashlib
import io
import re
import subprocess
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile
from analyze import ROOT, validated_elf, write_json

UPSTREAM_URL = "https://github.com/matt-kempster/m2c"
UPSTREAM_REVISION = "708d2d2cb2698f091a92492b328f73b24209f72d"
UPSTREAM_LICENSE = "GPL-3.0-only"
SOURCE_DIGEST = "ea6f716da03b47479cfc41eea9a57a2f521b61950c73f2b04dd52af1a13271eb"
DEFAULT_VENDOR = ROOT / "tools/vendor/m2c-708d2d2"
TARGET = "mipsee-gcc-c"
SYMBOL = re.compile(r"func_[0-9A-F]{8}\Z")
WORD_LINE = re.compile(
    r"^(\s*/\*\s*([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s*\*/\s*)(.*)$"
)
ERROR_MARKERS = re.compile(r"M2C_ERROR|ERROR:|Error:|Traceback|Failed to decompile|second half of f64")


def source_digest(vendor):
    """Hash upstream executable sources plus license/metadata, independent of CRLF."""
    names = ["LICENSE", "pyproject.toml", "m2c.py"]
    for directory in ("m2c", "m2c_pycparser"):
        names.extend(path.relative_to(vendor).as_posix()
                     for path in (vendor / directory).rglob("*.py"))
    digest = hashlib.sha256()
    for name in sorted(names):
        data = (vendor / name).read_bytes().replace(b"\r\n", b"\n")
        digest.update(name.encode("utf-8") + b"\0" + data + b"\0")
    return digest.hexdigest()


def checked_output(path):
    resolved = path.resolve()
    if not resolved.is_relative_to(ROOT.resolve()) or not any(
            resolved.is_relative_to((ROOT / directory).resolve())
            for directory in ("build", ".local")):
        raise ValueError("Generated candidates must stay in ignored build/ or .local/")
    return resolved


def function_rows(inventory):
    rows = []
    with inventory.open(newline="", encoding="utf-8") as stream:
        for row in csv.DictReader(stream):
            if not SYMBOL.fullmatch(row["name"]):
                continue
            rows.append({"name": row["name"], "address": int(row["address"], 0),
                         "offset": int(row["vrom"], 0), "size": int(row["length"], 0)})
    if len({row["name"] for row in rows}) != len(rows):
        raise ValueError("Duplicate function names in local inventory")
    return sorted(rows, key=lambda row: row["address"])


def checked_assembly(path, row, original, executable_sections):
    """Check inventory geometry/raw disassembler words and canonicalize one known alias."""
    source = path.read_text(encoding="utf-8")
    boundary = re.search(rf"^nonmatching\s+{row['name']},\s*(0x[0-9A-Fa-f]+)\s*$",
                         source, re.MULTILINE)
    if boundary is None:
        raise ValueError(f"Missing disassembler body boundary: {row['name']}")
    address, offset, size = row["address"], row["offset"], int(boundary[1], 16)
    if not size <= row["size"] <= size + 12:
        raise ValueError(f"Unexpected body/inventory size disagreement: {row['name']}")
    if size <= 0 or size % 4 or address % 4 or offset < 0 or offset + size > len(original):
        raise ValueError(f"Invalid candidate geometry: {row['name']}")
    if not any(section["address"] <= address
               and address + size <= section["address"] + section["size"]
               and offset == section["offset"] + address - section["address"]
               for section in executable_sections):
        raise ValueError(f"Candidate lies outside executable section: {row['name']}")
    if re.search(r"^\s*\.(?:include|incbin)\b", source, re.MULTILINE):
        raise ValueError("Local m2c inputs must not load external assembly/data")
    if not re.search(rf"^glabel\s+{row['name']}(?:\s|$)", source, re.MULTILINE):
        raise ValueError(f"Missing expected function label: {row['name']}")
    words, transformations, lines, ended = [], [], [], False
    for line in source.splitlines():
        if re.match(rf"^endlabel\s+{row['name']}(?:\s|$)", line):
            lines.append(line)
            ended = True
            break  # trailing alignment nops are outside the candidate body
        match = WORD_LINE.match(line)
        if match:
            index = len(words)
            file_offset, pc = int(match[2], 16), int(match[3], 16)
            word = bytes.fromhex(match[4])
            if file_offset != offset + 4 * index or pc != address + 4 * index:
                raise ValueError(f"Noncontiguous disassembly: {row['name']}")
            words.append(word)
            # spimdisasm 1.42.4's R5900 decoder writes this scalar sqrt as c1.
            # No instruction is dropped; 0x46000004 is exactly sqrt.s $f0,$f0.
            if word == b"\x04\x00\x00\x46" and re.fullmatch(r"c1\s+0x4\s*", match[5]):
                line = match[1] + "sqrt.s      $f0, $f0"
                transformations.append({"address": f"0x{pc:08X}",
                                        "from": "c1 0x4", "to": "sqrt.s $f0, $f0"})
            elif word == b"\x64\x00\x00\x46" and re.fullmatch(
                    r"\.word\s+0x46000064\s*(?:#.*)?", match[5]):
                # R5900 uses this encoding for trunc.w.s; standard-MIPS cvt.w.s
                # annotations from the disassembler are misleading here.
                line = match[1] + "trunc.w.s   $f1, $f0"
                transformations.append({"address": f"0x{pc:08X}",
                                        "from": ".word 0x46000064", "to": "trunc.w.s $f1, $f0"})
        lines.append(line)
    if not ended or b"".join(words) != original[offset:offset + size]:
        raise ValueError(f"Disassembly words differ from validated executable: {row['name']}")
    padding = original[offset + size:offset + row["size"]]
    if padding != bytes(len(padding)):
        raise ValueError(f"Nonzero bytes excluded as alignment padding: {row['name']}")
    return "\n".join(lines) + "\n", transformations, hashlib.sha256(source.encode()).hexdigest(), size


def run_candidates(rows, *, elf_path, assembly_dir, vendor, output_dir, context=(),
                   union_fields=(), timeout=30):
    output_dir = checked_output(output_dir)
    if any(not SYMBOL.fullmatch(row["name"]) for row in rows):
        raise ValueError("Invalid candidate symbol name")
    if len({row["name"] for row in rows}) != len(rows):
        raise ValueError("Duplicate requested candidate")
    if source_digest(vendor) != SOURCE_DIGEST:
        raise ValueError("m2c source/license tree differs from the pinned upstream revision")
    revision, original = validated_elf(elf_path)
    elf = ELFFile(io.BytesIO(original))
    if elf.elfclass != 32 or not elf.little_endian or elf["e_machine"] != "EM_MIPS":
        raise ValueError("Expected ELF32 little-endian MIPS executable")
    sections = [{"address": section["sh_addr"], "offset": section["sh_offset"],
                 "size": section["sh_size"]} for section in elf.iter_sections()
                if section["sh_type"] == "SHT_PROGBITS" and section["sh_flags"] & 4]
    contexts = [{"path": str(path.resolve()),
                 "sha256": hashlib.sha256(path.read_bytes()).hexdigest()} for path in context]
    output_dir.mkdir(parents=True, exist_ok=True)
    results = []
    for row in rows:
        source_path = assembly_dir / (row["name"] + ".s")
        source, transformations, input_digest, body_size = checked_assembly(source_path, row, original, sections)
        row = {**row, "inventory_size": row["size"], "size": body_size}
        candidate_dir = checked_output(output_dir / row["name"])
        candidate_dir.mkdir(parents=True, exist_ok=True)
        normalized_path = checked_output(candidate_dir / "input.s")
        normalized_path.write_text(source, encoding="utf-8")
        command = [sys.executable, str(vendor / "m2c.py"), "--target", TARGET,
                   "--no-cache", "--stop-on-error", "--function", row["name"],
                   "--deterministic-vars", "--valid-syntax"]
        for item in contexts:
            command.extend(["--context", item["path"]])
        for union_field in union_fields:
            command.extend(["--union-field", union_field])
        command.append(str(normalized_path))
        try:
            process = subprocess.run(command, cwd=ROOT, capture_output=True,
                                     text=True, encoding="utf-8", errors="replace", timeout=timeout)
            output, errors, return_code = process.stdout, process.stderr, process.returncode
        except subprocess.TimeoutExpired:
            output, errors, return_code = "", f"m2c exceeded {timeout} seconds\n", None
        diagnostics = [line.strip() for line in (output + "\n" + errors).splitlines()
                       if ERROR_MARKERS.search(line)]
        if errors.strip() and not diagnostics:
            diagnostics.append(errors.strip())
        status = "candidate" if return_code == 0 and not diagnostics else "diagnostic"
        checked_output(candidate_dir / "candidate.c").write_text(output, encoding="utf-8")
        checked_output(candidate_dir / "stderr.txt").write_text(errors, encoding="utf-8")
        results.append({**row, "status": status, "reviewed": False, "matched": False,
                        "boundary_reviewed": False,
                        "original_code_sha256": hashlib.sha256(original[row["offset"]:row["offset"] + row["size"]]).hexdigest(),
                        "assembly_sha256": input_digest, "normalizations": transformations,
                        "output_sha256": hashlib.sha256(output.encode()).hexdigest(),
                        "return_code": return_code, "diagnostics": diagnostics,
                        "candidate_path": str(candidate_dir / "candidate.c")})
    report = {"schema_version": 1, "tool": "m2c", "upstream": UPSTREAM_URL,
              "revision": UPSTREAM_REVISION, "source_digest": SOURCE_DIGEST,
              "license": UPSTREAM_LICENSE, "target": TARGET, "contexts": contexts,
              "union_fields": list(union_fields),
              "executable_sha256": revision["executable_sha256"],
              "recovered_functions_added": 0, "matched_functions_added": 0,
              "warning": "Generated drafts require independent review and compile/link comparison.",
              "functions": results}
    write_json(checked_output(output_dir / "report.json"), report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=ROOT / "orig/SLUS_216.68")
    parser.add_argument("--inventory", type=Path, default=ROOT / "build/functions.csv")
    parser.add_argument("--assembly-dir", type=Path, default=ROOT / "build/functions")
    parser.add_argument("--vendor", type=Path, default=DEFAULT_VENDOR)
    parser.add_argument("--output", type=Path, default=ROOT / "build/m2c")
    parser.add_argument("--context", type=Path, action="append", default=[],
                        help="Preprocessed C context; may be repeated")
    parser.add_argument("--union-field", action="append", default=[],
                        help="Upstream union member selection, UNION_NAME:FIELD_NAME")
    parser.add_argument("--function", action="append", default=[])
    parser.add_argument("--start", type=lambda value: int(value, 0))
    parser.add_argument("--end", type=lambda value: int(value, 0), help="Exclusive start-address limit")
    parser.add_argument("--minimum-size", type=int, default=64)
    parser.add_argument("--maximum-size", type=int, default=1024)
    parser.add_argument("--limit", type=int, default=25)
    parser.add_argument("--timeout", type=int, default=30)
    args = parser.parse_args()
    if args.limit <= 0 or not 1 <= args.timeout <= 300:
        parser.error("limit must be positive; timeout must be 1..300 seconds")
    if bool(args.function) == (args.start is not None or args.end is not None):
        parser.error("Choose explicit --function names or both --start and --end")
    rows = function_rows(args.inventory)
    if args.function:
        if len(set(args.function)) != len(args.function):
            parser.error("Duplicate requested function")
        unknown = set(args.function) - {row["name"] for row in rows}
        if unknown:
            parser.error("Unknown functions: " + ", ".join(sorted(unknown)))
        rows = [row for row in rows if row["name"] in args.function]
    else:
        if args.start is None or args.end is None or args.start >= args.end:
            parser.error("A valid address range requires both --start and --end")
        if args.minimum_size <= 0 or args.maximum_size < args.minimum_size:
            parser.error("Invalid size range")
        rows = [row for row in rows if args.start <= row["address"] < args.end
                and args.minimum_size <= row["size"] <= args.maximum_size]
    rows = rows[:args.limit]
    if not rows:
        parser.error("No candidates selected")
    try:
        report = run_candidates(rows, elf_path=args.elf, assembly_dir=args.assembly_dir,
                                vendor=args.vendor.resolve(), output_dir=args.output,
                                context=args.context, union_fields=args.union_field,
                                timeout=args.timeout)
    except (ValueError, OSError) as error:
        parser.exit(2, str(error) + "\n")
    diagnostic_count = sum(row["status"] == "diagnostic" for row in report["functions"])
    print(f"m2c wrote {len(rows)} drafts ({diagnostic_count} with diagnostics) to {args.output}.")
    print("No recovered or matching functions were added.")


if __name__ == "__main__":
    main()
