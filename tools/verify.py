"""Compile recovered C and count only exact, relocation-free function matches.

The retail compiler has not been identified. This candidate compiler is useful
for individual byte matches; it does not establish a full matching game build.
"""
import argparse
import hashlib
import json
import os
import subprocess
from pathlib import Path

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection
from analyze import ROOT, validated_elf, write_json

DEFAULT_FLAGS = ["-O2", "-G0", "-fno-builtin", "-fno-strict-aliasing"]


def number(value):
    return int(value, 0) if isinstance(value, str) else value


def compare_function(expected, actual, relocations=()):
    differences = [i for i, (a, b) in enumerate(zip(expected, actual)) if a != b]
    return {"identical": actual == expected and not relocations,
            "expected_size": len(expected), "compiled_size": len(actual),
            "different_bytes": len(differences) + abs(len(expected) - len(actual)),
            "first_different_offsets": differences[:8],
            "unresolved_relocations": list(relocations),
            "expected_sha256": hashlib.sha256(expected).hexdigest(),
            "compiled_sha256": hashlib.sha256(actual).hexdigest()}


def object_functions(path):
    result = {}
    with path.open("rb") as stream:
        elf = ELFFile(stream)
        if (elf["e_type"] != "ET_REL" or elf.elfclass != 32 or not elf.little_endian
                or elf["e_machine"] != "EM_MIPS"):
            raise ValueError(f"Expected ELF32 little-endian MIPS relocatable object: {path}")
        symbols = elf.get_section_by_name(".symtab")
        if symbols is None:
            raise ValueError(f"No symbol table in {path}")
        for symbol in symbols.iter_symbols():
            index = symbol["st_shndx"]
            if symbol["st_info"]["type"] != "STT_FUNC" or not isinstance(index, int):
                continue
            section = elf.get_section(index)
            if not section["sh_flags"] & 4:
                raise ValueError(f"Function symbol outside executable section: {symbol.name}")
            start = symbol["st_value"] - section["sh_addr"]
            end = start + symbol["st_size"]
            data = section.data()
            if start < 0 or end > len(data):
                raise ValueError(f"Invalid function symbol bounds: {symbol.name}")
            relocations = []
            for relocation_section in elf.iter_sections():
                if isinstance(relocation_section, RelocationSection) and relocation_section["sh_info"] == index:
                    for relocation in relocation_section.iter_relocations():
                        if start <= relocation["r_offset"] < end:
                            relocations.append(relocation["r_offset"] - start)
            result[symbol.name] = (data[start:end], relocations)
    return result


def manifests():
    functions = []
    for path in (ROOT / "config/recovered_functions.json", ROOT / "config/runtime_functions.json"):
        if path.exists():
            functions.extend(json.loads(path.read_text(encoding="utf-8"))["functions"])
    if not functions:
        raise ValueError("No recovered functions recorded")
    ranges = sorted((number(f["address"]), number(f["address"]) + number(f["size"])) for f in functions)
    if any(a[1] > b[0] for a, b in zip(ranges, ranges[1:])):
        raise ValueError("Recovered function ranges overlap; progress would double-count bytes")
    return functions


def validate_target_function(function, sections):
    offset = number(function["file_offset"])
    size = number(function["size"])
    address = number(function["address"])
    if size <= 0 or offset < 0:
        raise ValueError(f"Invalid function range: {function['name']}")
    for section in sections:
        if (section["offset"] <= offset and offset + size <= section["offset"] + section["size"]
                and address == section["address"] + offset - section["offset"]):
            return
    raise ValueError(f"Function address/offset is not inside an original code section: {function['name']}")


def publish_readme_progress(progress):
    path = ROOT / "README.md"
    if not path.exists():
        return
    content = path.read_text(encoding="utf-8")
    start, end = "<!-- progress:start -->", "<!-- progress:end -->"
    if content.count(start) != 1 or content.count(end) != 1:
        raise ValueError("README progress markers are missing or ambiguous")
    game = [f for f in progress["functions"] if f["category"] == "game" and f["language"] == "C"]
    game_bytes = sum(f["expected_size"] for f in game if f["identical"])
    runtime = [f for f in progress["functions"] if f["category"] == "runtime" and f["language"] == "C" and f["identical"]]
    runtime_bytes = sum(f["expected_size"] for f in runtime)
    runtime_names = ", ".join(f"`{f['name']}`" for f in runtime)
    block = f"""{start}
| Milestone | Result |
| --- | --- |
| Disc identification and extraction | 565 files inventoried; boot files extracted |
| Main CPU assembly baseline | 2,949,800 / 2,949,800 bytes identical |
| Candidate function regions | 14,560 detected; boundaries need review |
| Recovered game C | {len(game)} functions reviewed; {progress['matched_game_functions']} match ({game_bytes:,} bytes) |
| Reused upstream C | {progress['matched_runtime_functions']} match ({runtime_bytes:,} bytes: {runtime_names}) |
| Reused upstream assembly | {progress['reused_assembly_functions']} functions match ({progress['reused_assembly_code_bytes']:,} bytes) |
| Full source build | Incomplete |

The C matching total is **{progress['matched_code_bytes']:,} / {progress['total_code_bytes']:,} code bytes ({progress['matched_code_percent']:.6f}%)**, including
game and runtime code. The denominator includes `.text`, `.rentext`, and
`.vutext`; middleware and VU code are still unresolved. Assembly reproduction
and original data retained in the hybrid build do **not** count as C progress.
{end}"""
    content = content.split(start)[0] + block + content.split(end)[1]
    path.write_text(content, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=ROOT / "tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/bin/ee-gcc.exe")
    parser.add_argument("--dll-path", type=Path, default=ROOT / "tools/vendor/ps2dev-20181019/MinGW/bin")
    parser.add_argument("--publish-report", action="store_true", help="Update the tracked reports/progress.json")
    args = parser.parse_args()
    spec, original = validated_elf(ROOT / "orig/SLUS_216.68")
    functions = manifests()
    with (ROOT / "orig/SLUS_216.68").open("rb") as stream:
        elf = ELFFile(stream)
        sections = [{"name": name, "offset": elf.get_section_by_name(name)["sh_offset"],
                     "address": elf.get_section_by_name(name)["sh_addr"],
                     "size": elf.get_section_by_name(name)["sh_size"]}
                    for name in (".text", ".rentext", ".vutext")]
    for function in functions:
        validate_target_function(function, sections)
    env = dict(os.environ)
    env["PATH"] = str(args.dll_path) + os.pathsep + str(args.compiler.parent) + os.pathsep + env.get("PATH", "")
    version = subprocess.run([str(args.compiler), "--version"], env=env, cwd=ROOT,
                             capture_output=True, text=True, check=True).stdout.splitlines()[0]
    compiled = {}
    compiled_paths = {}
    results = []
    output_dir = ROOT / "build/c"
    output_dir.mkdir(parents=True, exist_ok=True)
    for function in functions:
        source = function["source"]
        flags = tuple(function.get("compile_flags", DEFAULT_FLAGS))
        key = (source, flags)
        if key not in compiled:
            destination = output_dir / f"{len(compiled):03d}_{Path(source).stem}.o"
            command = [str(args.compiler), *flags, "-I", str(ROOT / "include"), "-c", str(ROOT / source), "-o", str(destination)]
            process = subprocess.run(command, env=env, cwd=ROOT, capture_output=True, text=True)
            if process.returncode:
                raise RuntimeError(f"Compilation failed for {source}:\n{process.stdout}{process.stderr}")
            compiled[key] = object_functions(destination)
            compiled_paths[key] = destination.relative_to(ROOT).as_posix()
        offset = number(function["file_offset"])
        size = number(function["size"])
        if offset < 0 or size <= 0 or offset + size > len(original):
            raise ValueError(f"Invalid original function bounds: {function['name']}")
        expected = original[offset:offset + size]
        if hashlib.sha256(expected).hexdigest() != function["original_sha256"]:
            raise ValueError(f"Original byte fingerprint mismatch: {function['name']}")
        symbol = function.get("compiled_symbol", function["name"])
        actual, relocations = compiled[key][symbol]
        comparison = compare_function(expected, actual, relocations)
        # A recorded match is a regression gate; reconstructed entries may differ.
        if function["status"] in ("matched", "reused_matching_assembly") and not comparison["identical"]:
            raise ValueError(f"Previously recorded match regressed: {function['name']}")
        results.append({"name": function["name"], "address": function["address"], "source": source,
                        "category": "runtime" if source.startswith("src/runtime/") else "game",
                        "language": "assembly" if Path(source).suffix.lower() in (".s", ".asm") else "C",
                        "object": compiled_paths[key], "compiled_symbol": symbol,
                        "compile_flags": list(flags), **comparison})
    with (ROOT / "orig/SLUS_216.68").open("rb") as stream:
        elf = ELFFile(stream)
        code_sizes = {name: elf.get_section_by_name(name)["sh_size"] for name in (".text", ".rentext", ".vutext")}
    total = sum(code_sizes.values())
    matched_c = [r for r in results if r["identical"] and r["language"] == "C"]
    reused_assembly = [r for r in results if r["identical"] and r["language"] == "assembly"]
    matched_bytes = sum(r["expected_size"] for r in matched_c)
    progress = {"schema_version": 1, "revision": spec["serial"], "original_sha256": spec["executable_sha256"],
                "compiler": version, "compiler_identity": "candidate; original compiler unidentified",
                "compiler_sha256": hashlib.sha256(args.compiler.read_bytes()).hexdigest(),
                "method": "Exact function code bytes and size, with no unresolved relocations. Padding excluded.",
                "full_game_build": False, "code_section_bytes": code_sizes, "total_code_bytes": total,
                "reconstructed_c_functions": sum(r["language"] == "C" for r in results),
                "matched_functions": len(matched_c),
                "matched_game_functions": sum(r["category"] == "game" for r in matched_c),
                "matched_runtime_functions": sum(r["category"] == "runtime" for r in matched_c),
                "reused_assembly_functions": len(reused_assembly),
                "reused_assembly_code_bytes": sum(r["expected_size"] for r in reused_assembly),
                "matched_code_bytes": matched_bytes, "matched_code_percent": round(100 * matched_bytes / total, 6),
                "functions": results}
    write_json(ROOT / "build/progress.json", progress)
    if args.publish_report:
        published = dict(progress)
        # Build paths are local implementation details, unnecessary for the public report.
        published["functions"] = [{k: v for k, v in r.items() if k != "object"} for r in results]
        write_json(ROOT / "reports/progress.json", published)
        publish_readme_progress(progress)
    print(f'{progress["matched_functions"]}/{progress["reconstructed_c_functions"]} C functions match: {matched_bytes:,}/{total:,} code bytes ({progress["matched_code_percent"]:.6f}%).')
    print(f'Game: {progress["matched_game_functions"]} matches. Reused runtime: {progress["matched_runtime_functions"]} matches.')
    print(f'Reused original assembly: {len(reused_assembly)} functions, {progress["reused_assembly_code_bytes"]} bytes (excluded from C progress).')


if __name__ == "__main__":
    main()
