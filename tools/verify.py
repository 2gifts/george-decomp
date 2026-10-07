"""Compile recovered C/C++ and count only exact function matches after linking.

The retail compiler has not been identified. This candidate compiler is useful
for individual byte matches; it does not establish a full matching game build.
"""
import argparse
import hashlib
import json
import os
import struct
import subprocess
from pathlib import Path

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection
from analyze import ROOT, validated_elf, write_json
from source_provenance import validate_sources

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
    paths = [ROOT / "config/recovered_functions.json", ROOT / "config/runtime_functions.json"]
    paths.extend(sorted((ROOT / "config/functions").glob("*.json")))
    for path in paths:
        if path.exists():
            functions.extend(json.loads(path.read_text(encoding="utf-8"))["functions"])
    if not functions:
        raise ValueError("No recovered functions recorded")
    ranges = sorted((number(f["address"]), number(f["address"]) + number(f["size"])) for f in functions)
    if any(a[1] > b[0] for a, b in zip(ranges, ranges[1:])):
        raise ValueError("Recovered function ranges overlap; progress would double-count bytes")
    return functions


def symbol_bindings(function):
    bindings = {}
    for path in sorted((ROOT / "config/symbols").glob("*.json")):
        data = json.loads(path.read_text(encoding="utf-8"))
        for name, value in data.get("symbols", {}).items():
            address = number(value["address"] if isinstance(value, dict) else value)
            if name in bindings and bindings[name] != address:
                raise ValueError(f"Conflicting symbol binding {name}")
            bindings[name] = address
    for name, value in function.get("link_symbols", {}).items():
        address = number(value["address"] if isinstance(value, dict) else value)
        if name in bindings and bindings[name] != address:
            raise ValueError(f"Conflicting per-function symbol binding {name}")
        bindings[name] = address
    if "reuse_compiled_symbol" in function:
        seed_name = function["reuse_compiled_symbol"]
        if not isinstance(seed_name, str) or not seed_name:
            raise ValueError("Compiled-symbol reuse requires a canonical seed name")
        seeds = [item for item in manifests() if item["name"] == seed_name]
        if len(seeds) != 1:
            raise ValueError("Compiled-symbol reuse requires one canonical seed")
        seed = seeds[0]
        reviewed = seed.get("reviewed") is True
        legacy_review = (seed.get("status") in ("matched", "matching")
                         and isinstance(seed.get("independent_review"), str)
                         and bool(seed["independent_review"].strip()))
        if not (reviewed or legacy_review) or "reuse_compiled_symbol" in seed:
            raise ValueError("Compiled-symbol reuse requires a reviewed primary seed")
        symbol = function.get("compiled_symbol", function["name"])
        if symbol != seed.get("compiled_symbol", seed["name"]):
            raise ValueError("Compiled-symbol reuse changed the selected source symbol")
        for key in ("source", "source_kind", "compiler_profile", "compile_flags",
                    "size", "original_sha256"):
            if key not in seed or function.get(key) != seed[key]:
                raise ValueError(f"Compiled-symbol reuse changed seed field {key}")
        if function.get("source_provenance_manifest") != seed.get("source_provenance_manifest"):
            raise ValueError("Compiled-symbol reuse changed seed provenance manifest")
        address = number(function["address"])
        seed_address = number(seed["address"])
        if (address == seed_address or function["name"] == seed_name
                or bindings.get(function["name"]) != address
                or bindings.get(symbol) != seed_address):
            raise ValueError("Compiled-symbol reuse requires distinct canonical entry bindings")
        if validate_sources([seed, function], ROOT) != [seed_name, function["name"]]:
            raise ValueError("Compiled-symbol reuse requires complete source fingerprints")
        # This is the selected definition, which GNU ld places at its own
        # original VMA. Every external/helper/data binding stays canonical;
        # the global seed binding remains unchanged outside this one call.
        del bindings[symbol]
    return bindings


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


def mapped_data(function, original, sections):
    mappings = {}
    proofs = function.get("link_data", {})
    if not isinstance(proofs, dict):
        raise ValueError("Mapped data proofs must be a dictionary")
    for name, proof in proofs.items():
        required = {"file_offset", "size", "address", "original_sha256"}
        allowed = required | {"evidence", "relocation_policy"}
        if not isinstance(proof, dict) or not required <= set(proof) or set(proof) - allowed:
            raise ValueError(f"Mapped data requires complete original proof and supported metadata: {name}")
        if "relocation_policy" in proof and proof["relocation_policy"] != "self_r_mips_32":
            raise ValueError(f"Unsupported mapped-data relocation policy: {name}")
        offset, size, address = (number(proof[key]) for key in ("file_offset", "size", "address"))
        if (any(type(value) is not int for value in (offset, size, address))
                or offset < 0 or size <= 0 or offset + size > len(original)
                or not 0 <= address <= 0xFFFFFFFF or address + size > 0x100000000
                or not any(section["offset"] <= offset
                           and offset + size <= section["offset"] + section["size"]
                           and address == section["address"] + offset - section["offset"]
                           for section in sections)):
            raise ValueError(f"Mapped data is not inside original read-only data: {name}")
        data = original[offset:offset + size]
        if hashlib.sha256(data).hexdigest() != proof["original_sha256"]:
            raise ValueError(f"Original mapped-data fingerprint mismatch: {name}")
        mappings[name] = {"address": address, "expected_bytes": data,
                          "expected_sha256": proof["original_sha256"]}
        if "relocation_policy" in proof:
            mappings[name]["relocation_policy"] = proof["relocation_policy"]
    return mappings


def mapped_codegen_data(function, original, sections):
    """Prove original readonly geometry and real code-derived table pointers.

    This is distinct from exact-original mapped_data. Complete input literals
    and real compiler-local R_MIPS_32 labels are checked by the linker planner;
    actual whole output data still determines matching eligibility.
    """
    proofs = function.get("link_codegen_readonly", {})
    if not isinstance(proofs, dict):
        raise ValueError("Generated readonly proofs must be a dictionary")
    mappings = {}
    for name, proof in proofs.items():
        required = {"file_offset", "size", "address", "original_sha256", "code_bindings"}
        if not isinstance(proof, dict) or not required <= set(proof) or set(proof) - required - {"evidence"}:
            raise ValueError("Generated readonly requires whole original geometry/hash and code pointer bindings")
        base_proof = {key: proof[key] for key in ("file_offset", "size", "address", "original_sha256")}
        mapping = mapped_data({"link_data": {name: base_proof}}, original, sections)[name]
        address, size = mapping["address"], len(mapping["expected_bytes"])
        function_offset, function_size = number(function["file_offset"]), number(function["size"])
        function_address = number(function["address"])
        if (any(type(value) is not int for value in (function_offset, function_size, function_address))
                or function_offset < 0 or function_size <= 0 or function_size & 3
                or function_offset + function_size > len(original)
                or not 0 <= function_address <= 0xFFFFFFFF or function_address + function_size > 0x100000000):
            raise ValueError("Invalid original function extent for generated readonly proof")
        pointers = proof["code_bindings"]
        if not isinstance(pointers, list) or not pointers:
            raise ValueError("Generated readonly requires actual original code pointer bindings")
        seen = set()
        for pointer in pointers:
            if not isinstance(pointer, dict) or set(pointer) != {"hi_offset", "lo_offset", "relative_offset"}:
                raise ValueError("Unsupported generated readonly code binding proof")
            hi, lo, relative = (number(pointer[key]) for key in ("hi_offset", "lo_offset", "relative_offset"))
            if (any(type(value) is not int for value in (hi, lo, relative)) or hi & 3 or lo & 3
                    or not 0 <= hi < lo <= function_size - 4 or lo - hi not in (4, 8)
                    or not 0 <= relative < size or (hi, lo) in seen):
                raise ValueError("Generated readonly code binding escapes whole function/data")
            seen.add((hi, lo))
            upper, lower = (struct.unpack_from("<I", original, function_offset + offset)[0] for offset in (hi, lo))
            register = (upper >> 16) & 31
            if (upper >> 26 != 15 or (upper >> 21) & 31 or register == 0
                    or lower >> 26 != 9 or (lower >> 21) & 31 != register or (lower >> 16) & 31 == 0):
                raise ValueError("Generated readonly binding must materialize a nonzero LUI/ADDIU pointer")
            if lo - hi == 8:
                middle = struct.unpack_from("<I", original, function_offset + hi + 4)[0]
                if middle >> 26 != 0 or middle & 63 != 0 or (middle >> 21) & 31 or (middle >> 11) & 31 == register:
                    raise ValueError("Generated readonly pointer pair has an intervening unsupported/clobbering instruction")
            immediate = (lower & 0xFFFF) - (0x10000 if lower & 0x8000 else 0)
            if (((upper & 0xFFFF) << 16) + immediate) & 0xFFFFFFFF != address + relative:
                raise ValueError("Generated readonly original pointer binding disagrees with section offset")
        mappings[name] = {**mapping, "original_function_size": function_size}
    return mappings


def gate_generated_comparison(comparison, mapping_reports):
    """A whole code match alone cannot award a differing generated data table."""
    result = dict(comparison)
    if mapping_reports:
        if any(type(mapping.get("identical")) is not bool for mapping in mapping_reports):
            raise ValueError("Generated readonly comparison requires actual full-data equality")
        result["code_identical"] = result["identical"]
        result["generated_data_identical"] = all(mapping["identical"] for mapping in mapping_reports)
        result["identical"] = result["code_identical"] and result["generated_data_identical"]
    return result


def _nobits_preserved_instruction(instruction, register):
    """Recognize only reviewed straight-line instructions preserving the base."""
    if instruction == 0:
        return True
    opcode, source, destination = instruction >> 26, instruction >> 21 & 31, instruction >> 16 & 31
    if opcode in (43, 63):  # Ordinary SW/SD do not write any GPR.
        return True
    if opcode == 9:  # Only the observed stack adjustment, never the LUI base.
        return source == destination == 29 and register != 29
    if opcode == 35:  # LW writes its destination after reading the address.
        return destination != 0 and destination != register
    return False


def mapped_bss(function, original, sections):
    """Prove complete zero storage from original pointers or opt-in word accesses.

    Legacy LUI/ADDIU pairs retain their original four/eight-byte grammar.
    Explicit word accesses and longer pairs require narrowly decoded base
    preservation; they do not change the linker or create initialized bytes.
    """
    from link_match import MAX_NOBITS_SIZE
    proofs = function.get("link_bss", {})
    if not isinstance(proofs, dict):
        raise ValueError("NOBITS proofs must be a dictionary")
    result = {}
    function_offset, function_size = number(function["file_offset"]), number(function["size"])
    for name, proof in proofs.items():
        required = {"address", "size", "zero_sha256", "original_section", "code_bindings"}
        if not isinstance(proof, dict) or not required <= set(proof) or set(proof) - required - {"evidence"}:
            raise ValueError("NOBITS requires complete memory geometry and code binding proof; file offsets are forbidden")
        address, size = number(proof["address"]), number(proof["size"])
        digest = proof["zero_sha256"]
        if (type(address) is not int or type(size) is not int or not 0 <= address <= 0xFFFFFFFF
                or not 0 < size <= MAX_NOBITS_SIZE or address + size > 0x100000000
                or digest != hashlib.sha256(bytes(size)).hexdigest()):
            raise ValueError("Invalid NOBITS zero initialization proof")
        if not any(section["name"] == proof["original_section"]
                   and section["type"] == "SHT_NOBITS" and section["flags"] == 3
                   and section["address"] <= address and address + size <= section["address"] + section["size"]
                   for section in sections):
            raise ValueError("Mapped zero storage is not inside original allocated writable NOBITS")
        pointers = proof["code_bindings"]
        if not isinstance(pointers, list) or not pointers:
            raise ValueError("NOBITS mapping requires original code pointer bindings")
        seen = set()
        for pointer in pointers:
            required = {"hi_offset", "lo_offset", "relative_offset"}
            if (not isinstance(pointer, dict) or not required <= set(pointer)
                    or set(pointer) - required - {"access", "preserved_sequence"}):
                raise ValueError("Unsupported original NOBITS code binding proof")
            access = pointer.get("access")
            if "access" in pointer and access not in ("lw32", "sw32"):
                raise ValueError("Unsupported original NOBITS access proof")
            preserved = pointer.get("preserved_sequence", False)
            if "preserved_sequence" in pointer and preserved is not True:
                raise ValueError("NOBITS preserved_sequence requires literal true")
            hi, lo, relative = (number(pointer[key]) for key in ("hi_offset", "lo_offset", "relative_offset"))
            if (any(type(value) is not int for value in (hi, lo, relative)) or hi & 3 or lo & 3
                    or type(function_offset) is not int or type(function_size) is not int
                    or function_size <= 0 or function_size & 3
                    or not 0 <= hi < lo <= function_size - 4 or lo - hi not in (4, 8, 12)
                    or (lo - hi == 12 and not preserved)
                    or (access is not None and lo - hi != 4 and not preserved)
                    or not 0 <= relative < size
                    or function_offset < 0 or function_offset + function_size > len(original)
                    or (hi, lo) in seen):
                raise ValueError("Original NOBITS code binding lies outside complete function/storage")
            seen.add((hi, lo))
            upper, lower = (struct.unpack_from("<I", original, function_offset + offset)[0] for offset in (hi, lo))
            register = upper >> 16 & 31
            lower_opcode, destination = lower >> 26, lower >> 16 & 31
            if (upper >> 26 != 15 or (upper >> 21 & 31) or not register
                    or (lower >> 21 & 31) != register
                    or (access is None and (lower_opcode != 9 or not destination))
                    or (access == "lw32" and (lower_opcode != 35 or not destination or destination == register))
                    or (access == "sw32" and lower_opcode != 43)):
                raise ValueError("NOBITS original binding requires LUI and its declared same-base lower instruction")
            if preserved:
                for offset in range(hi + 4, lo, 4):
                    middle = struct.unpack_from("<I", original, function_offset + offset)[0]
                    if not _nobits_preserved_instruction(middle, register):
                        raise ValueError("Original NOBITS preserved sequence contains an unsupported/base-clobbering instruction")
            elif lo - hi == 8:
                middle = struct.unpack_from("<I", original, function_offset + hi + 4)[0]
                opcode = middle >> 26
                branch = (opcode in (2, 4, 5, 6, 7, 20, 21, 22, 23)
                          or (opcode == 1 and (middle >> 16 & 31) in (0, 1, 2, 3)))
                if middle and middle != 0x03E00008 and not branch:
                    raise ValueError("Original NOBITS HI/LO proof crosses an unproven register write")
            immediate = lower & 0xFFFF
            target = (((upper & 0xFFFF) << 16) + (immediate - 0x10000 if immediate & 0x8000 else immediate)) & 0xFFFFFFFF
            if target != address + relative:
                raise ValueError("Original code pointer differs from NOBITS mapping")
            if access is not None and (target & 3 or relative + 4 > size):
                raise ValueError("Original NOBITS word access is unaligned or exceeds whole storage")
        result[name] = {"address": address, "size": size, "zero_sha256": digest}
    return result


def compiler_configuration(args):
    path = ROOT / "config/compiler_profiles.json"
    data = json.loads(path.read_text(encoding="utf-8")) if path.exists() else {
        "default_profile": "gcc323", "profiles": {"gcc323": {
            "compiler": "tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/bin/ee-gcc.exe",
            "path_entries": ["tools/vendor/ps2dev-20181019/MinGW/bin"],
            "command_prefix": [], "include_dirs": []}}}
    default = data["default_profile"]
    if args.compiler is not None:
        data["profiles"][default] = {**data["profiles"][default], "compiler": str(args.compiler.resolve())}
        data["profiles"][default].pop("compiler_sha256", None)
        data["profiles"][default].pop("binary_manifest", None)
        data["profiles"][default].pop("binaries", None)
        data["profiles"][default].pop("assembler_provenance", None)
    if args.dll_path is not None:
        data["profiles"][default]["path_entries"] = [str(args.dll_path.resolve())]
    return data


def prepare_compiler(profile):
    compiler = ROOT / profile["compiler"]
    digest = hashlib.sha256(compiler.read_bytes()).hexdigest()
    if profile.get("compiler_sha256") and digest != profile["compiler_sha256"]:
        raise ValueError(f"Compiler profile fingerprint mismatch: {profile['compiler']}")
    dependencies = list(profile.get("binaries", []))
    if profile.get("binary_manifest"):
        reference = profile["binary_manifest"]
        manifest = json.loads((ROOT / reference["path"]).read_text(encoding="utf-8"))
        dependencies.extend(manifest[reference["group"]]["binaries"])
    if profile.get("assembler_provenance"):
        dependencies.append(profile["assembler_provenance"])
    for dependency in dependencies:
        path = ROOT / dependency["path"]
        if not path.is_file() or hashlib.sha256(path.read_bytes()).hexdigest() != dependency["sha256"]:
            raise ValueError(f"Compiler dependency fingerprint mismatch: {dependency['path']}")
    env = dict(os.environ)
    paths = [str(ROOT / path) for path in profile.get("path_entries", [])]
    paths.append(str(compiler.parent))
    env["PATH"] = os.pathsep.join(paths + [env.get("PATH", "")])
    version = subprocess.run([str(compiler), "--version"], env=env, cwd=ROOT,
                             capture_output=True, text=True, check=True).stdout.splitlines()[0]
    return {"compiler": compiler, "sha256": digest, "version": version, "env": env, "profile": profile}


def source_language(function):
    """Classify reporting language without changing compilation or match gates."""
    suffix = Path(function["source"]).suffix
    inferred = ("C++" if suffix == ".C" or suffix.lower() in (".cc", ".cpp", ".cxx")
                else "assembly" if suffix.lower() in (".s", ".asm")
                else "C" if suffix == ".c" else None)
    if "source_kind" in function:
        kind = function["source_kind"]
        kinds = {"c": "C", "cxx": "C++", "assembly": "assembly"}
        if not isinstance(kind, str) or kind not in kinds:
            raise ValueError(f"Unknown source kind: {kind!r}")
        language = kinds[kind]
        if inferred is not None and language != inferred:
            raise ValueError(f"Source kind contradicts suffix: {function['source']}")
        return language
    # Preserve the historical C default for sources with an unknown suffix.
    return inferred or "C"


def source_progress(results, total):
    """Count each validated function once; assembly is separate from source."""
    if any(r["language"] not in ("C", "C++", "assembly") for r in results):
        raise ValueError("Unknown report language")
    source = [r for r in results if r["language"] in ("C", "C++")]
    matched = [r for r in source if r["identical"]]
    assembly = [r for r in results if r["language"] == "assembly" and r["identical"]]
    matched_bytes = sum(r["expected_size"] for r in matched)
    progress = {"reconstructed_source_functions": len(source),
                "reconstructed_source_code_bytes": sum(r["expected_size"] for r in source),
                "matched_functions": len(matched),
                "matched_game_functions": sum(r["category"] == "game" for r in matched),
                "matched_runtime_functions": sum(r["category"] == "runtime" for r in matched),
                "reused_assembly_functions": len(assembly),
                "reused_assembly_code_bytes": sum(r["expected_size"] for r in assembly),
                "matched_code_bytes": matched_bytes,
                "matched_code_percent": round(100 * matched_bytes / total, 6)}
    for language, key in (("C", "c"), ("C++", "cpp")):
        recovered = [r for r in source if r["language"] == language]
        exact = [r for r in matched if r["language"] == language]
        progress[f"reconstructed_{key}_functions"] = len(recovered)
        progress[f"reconstructed_{key}_code_bytes"] = sum(r["expected_size"] for r in recovered)
        progress[f"matched_{key}_functions"] = len(exact)
        progress[f"matched_{key}_code_bytes"] = sum(r["expected_size"] for r in exact)
    return progress


def progress_summary(progress):
    return [f'{progress["matched_functions"]}/{progress["reconstructed_source_functions"]} C/C++ functions match: {progress["matched_code_bytes"]:,}/{progress["total_code_bytes"]:,} code bytes ({progress["matched_code_percent"]:.6f}%).',
            f'Game: {progress["matched_game_functions"]} matches. Reused runtime: {progress["matched_runtime_functions"]} matches.',
            f'Reused original assembly: {progress["reused_assembly_functions"]} functions, {progress["reused_assembly_code_bytes"]} bytes (excluded from C/C++ progress).']


def publish_readme_progress(progress):
    path = ROOT / "README.md"
    if not path.exists():
        return
    content = path.read_text(encoding="utf-8")
    start, end = "<!-- progress:start -->", "<!-- progress:end -->"
    if content.count(start) != 1 or content.count(end) != 1:
        raise ValueError("README progress markers are missing or ambiguous")
    game = [f for f in progress["functions"] if f["category"] == "game" and f["language"] in ("C", "C++")]
    game_bytes = sum(f["expected_size"] for f in game if f["identical"])
    runtime = [f for f in progress["functions"] if f["category"] == "runtime" and f["language"] in ("C", "C++") and f["identical"]]
    runtime_bytes = sum(f["expected_size"] for f in runtime)
    runtime_names = ", ".join(f"`{f['name']}`" for f in runtime)
    block = f"""{start}
| Milestone | Result |
| --- | --- |
| Disc identification and extraction | 565 files inventoried; boot files extracted |
| Main CPU assembly baseline | 2,949,800 / 2,949,800 bytes identical |
| Candidate function regions | 14,560 detected; boundaries need review |
| Recovered game C/C++ | {len(game)} functions reviewed; {progress['matched_game_functions']} match ({game_bytes:,} bytes) |
| Reused upstream C/C++ | {progress['matched_runtime_functions']} match ({runtime_bytes:,} bytes: {runtime_names}) |
| C source | {progress['reconstructed_c_functions']} functions reviewed; {progress['matched_c_functions']} match ({progress['matched_c_code_bytes']:,} bytes) |
| C++ source | {progress['reconstructed_cpp_functions']} functions reviewed; {progress['matched_cpp_functions']} match ({progress['matched_cpp_code_bytes']:,} bytes) |
| Reused upstream assembly | {progress['reused_assembly_functions']} functions match ({progress['reused_assembly_code_bytes']:,} bytes) |
| Full source build | Incomplete |

The C/C++ matching total is **{progress['matched_code_bytes']:,} / {progress['total_code_bytes']:,} code bytes ({progress['matched_code_percent']:.6f}%)**, including
game and runtime code. The denominator includes `.text`, `.rentext`, and
`.vutext`; middleware and VU code are still unresolved. Assembly reproduction
and original data retained in the hybrid build do **not** count as C/C++ progress.
{end}"""
    content = content.split(start)[0] + block + content.split(end)[1]
    path.write_text(content, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, help="Override the default compiler profile only")
    parser.add_argument("--dll-path", type=Path, help="Override the default profile DLL directory only")
    parser.add_argument("--publish-report", action="store_true", help="Update the tracked reports/progress.json")
    args = parser.parse_args()
    spec, original = validated_elf(ROOT / "orig/SLUS_216.68")
    functions = manifests()
    validate_sources(functions)
    with (ROOT / "orig/SLUS_216.68").open("rb") as stream:
        elf = ELFFile(stream)
        sections = [{"name": name, "offset": elf.get_section_by_name(name)["sh_offset"],
                     "address": elf.get_section_by_name(name)["sh_addr"],
                     "size": elf.get_section_by_name(name)["sh_size"]}
                    for name in (".text", ".rentext", ".vutext")]
        readonly_sections = [{"offset": section["sh_offset"], "address": section["sh_addr"],
                              "size": section["sh_size"]} for section in elf.iter_sections()
                             if section["sh_type"] == "SHT_PROGBITS" and section["sh_flags"] & 2
                             and not section["sh_flags"] & 5]
        nobits_sections = [{"name": section.name, "address": section["sh_addr"], "size": section["sh_size"],
                            "type": section["sh_type"], "flags": section["sh_flags"]}
                           for section in elf.iter_sections() if section["sh_type"] == "SHT_NOBITS"]
    for function in functions:
        validate_target_function(function, sections)
        source_language(function)
    configuration = compiler_configuration(args)
    compiler_profiles = {}
    compiled = {}
    compiled_paths = {}
    results = []
    output_dir = ROOT / "build/c"
    output_dir.mkdir(parents=True, exist_ok=True)
    for function in functions:
        source = function["source"]
        profile_name = function.get("compiler_profile", configuration["default_profile"])
        if profile_name not in configuration["profiles"]:
            raise ValueError(f"Unknown compiler profile: {profile_name}")
        if profile_name not in compiler_profiles:
            compiler_profiles[profile_name] = prepare_compiler(configuration["profiles"][profile_name])
        setup = compiler_profiles[profile_name]
        profile = setup["profile"]
        use_linker = bool(function.get("link", False) or function.get("link_symbols")
                          or function.get("link_data") or function.get("link_bss") or function.get("link_codegen_readonly"))
        chosen_flags = list(function.get("compile_flags", DEFAULT_FLAGS))
        if use_linker and "-ffunction-sections" not in chosen_flags:
            chosen_flags.append("-ffunction-sections")
        flags = tuple(chosen_flags)
        key = (profile_name, source, flags)
        if key not in compiled:
            destination = output_dir / f"{len(compiled):03d}_{Path(source).stem}.o"
            command = [str(setup["compiler"]), *profile.get("command_prefix", []), *flags, "-I", str(ROOT / "include")]
            for path in profile.get("include_dirs", []):
                command.extend(["-I", str(ROOT / path)])
            if profile.get("source_parent_include", False):
                command.extend(["-I", str((ROOT / source).parent)])
            command.extend(["-c", str(ROOT / source), "-o", str(destination)])
            process = subprocess.run(command, env=setup["env"], cwd=ROOT, capture_output=True, text=True)
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
        linked_path = None
        mapping_report = []
        bss_report = []
        generated_report = []
        if use_linker:
            from link_match import link_function
            mappings = mapped_data(function, original, readonly_sections)
            bss_mappings = mapped_bss(function, original, nobits_sections)
            generated_mappings = mapped_codegen_data(function, original, readonly_sections)
            linked = link_function(ROOT / compiled_paths[key], symbol, number(function["address"]),
                                   symbol_bindings(function), ROOT / "build/linked" / function["name"],
                                   mapped_sections=mappings, mapped_nobits=bss_mappings,
                                   mapped_codegen_readonly=generated_mappings)
            for mapped in linked["mapped_sections"]:
                expected_mapping = mappings[mapped["name"]]
                if (mapped["data"] != expected_mapping["expected_bytes"]
                        or mapped["address"] != expected_mapping["address"]
                        or mapped["sha256"] != expected_mapping["expected_sha256"]):
                    raise ValueError(f"Linked mapped data changed: {mapped['name']}")
                mapping_report.append({k: mapped[k] for k in ("name", "address", "size", "sha256")})
            for mapped in linked["mapped_nobits"]:
                expected_mapping = bss_mappings[mapped["name"]]
                if any(mapped[key] != expected_mapping[key] for key in ("address", "size", "zero_sha256")):
                    raise ValueError(f"Linked NOBITS geometry changed: {mapped['name']}")
                bss_report.append({key: mapped[key] for key in ("name", "address", "size", "zero_sha256")})
            for mapped in linked["mapped_codegen_readonly"]:
                expected_mapping = generated_mappings[mapped["name"]]
                if (mapped["address"] != expected_mapping["address"]
                        or mapped["expected_sha256"] != expected_mapping["expected_sha256"]
                        or mapped["size"] != len(expected_mapping["expected_bytes"])
                        or mapped["sha256"] != hashlib.sha256(mapped["data"]).hexdigest()
                        or mapped["identical"] != (mapped["data"] == expected_mapping["expected_bytes"])):
                    raise ValueError("Generated readonly full output proof changed")
                generated_report.append({**{key: mapped[key] for key in ("name", "output_section", "address", "size", "sha256", "identical", "literal_byte_count")},
                                         "original_sha256": expected_mapping["expected_sha256"],
                                         "file_offset": number(function["link_codegen_readonly"][mapped["name"]]["file_offset"]),
                                         "relocation_count": len(mapped["input_relocations"])})
            actual, relocations = linked["code"], linked["remaining_relocations"]
            linked_path = linked["linked_path"].relative_to(ROOT).as_posix()
        comparison = gate_generated_comparison(compare_function(expected, actual, relocations), generated_report)
        # A recorded match is a regression gate; reconstructed entries may differ.
        if function["status"] in ("matched", "reused_matching_assembly") and not comparison["identical"]:
            raise ValueError(f"Previously recorded match regressed: {function['name']}")
        results.append({"name": function["name"], "address": function["address"], "source": source,
                        "category": "runtime" if source.startswith("src/runtime/") else "game",
                        "language": source_language(function),
                        "object": compiled_paths[key], "compiled_symbol": symbol,
                        "linked_elf": linked_path,
                        "compiler_profile": profile_name, "compiler": setup["version"],
                        "compiler_sha256": setup["sha256"], "mapped_data": mapping_report, "mapped_nobits": bss_report,
                        "compile_flags": list(flags), **({"mapped_codegen_readonly": generated_report} if generated_report else {}), **comparison})
    with (ROOT / "orig/SLUS_216.68").open("rb") as stream:
        elf = ELFFile(stream)
        code_sizes = {name: elf.get_section_by_name(name)["sh_size"] for name in (".text", ".rentext", ".vutext")}
    total = sum(code_sizes.values())
    counts = source_progress(results, total)
    default_setup = compiler_profiles.get(configuration["default_profile"], next(iter(compiler_profiles.values())))
    progress = {"schema_version": 1, "revision": spec["serial"], "original_sha256": spec["executable_sha256"],
                "compiler": default_setup["version"], "compiler_identity": "candidate profiles; original compiler(s) unidentified",
                "compiler_sha256": default_setup["sha256"],
                "compiler_profiles": {name: {"version": setup["version"], "sha256": setup["sha256"]}
                                      for name, setup in compiler_profiles.items()},
                "method": "Exact function code bytes and size plus complete equality of any generated readonly tables after actual linking; no unresolved relocations. Padding excluded.",
                "full_game_build": False, "code_section_bytes": code_sizes, "total_code_bytes": total,
                **counts,
                "functions": results}
    write_json(ROOT / "build/progress.json", progress)
    if args.publish_report:
        published = dict(progress)
        # Build paths are local implementation details, unnecessary for the public report.
        published["functions"] = [{k: v for k, v in r.items() if k not in ("object", "linked_elf")} for r in results]
        write_json(ROOT / "reports/progress.json", published)
        publish_readme_progress(progress)
    for line in progress_summary(progress):
        print(line)


if __name__ == "__main__":
    main()
