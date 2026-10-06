"""Resolve one dedicated function section at a proven retail address with GNU ld.

This links code only. Explicit bindings refer to existing retail functions or
globals; compiler-local constant pools are rejected until their bytes and
addresses have a separate mapping. No instruction or relocation bytes are
masked, and unresolved references are errors.
"""

import hashlib
import json
import re
import struct
import subprocess
import tempfile
from pathlib import Path

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_BINUTILS = ROOT / "tools/vendor/binutils-v0.10"
GP = 0x0045FB70
SYMBOL = re.compile(r"[A-Za-z_.$][A-Za-z0-9_.$]*\Z")
RELOCATIONS = {0, 2, 4, 5, 6, 7, 10, 12}  # NONE, 32, 26, HI16, LO16, GPREL16, PC16, GPREL32.
GP_SYMBOLS = {"_gp", "__gnu_local_gp"}
ENTRY_SYMBOL = "__george_link_entry"


class LinkError(ValueError):
    """An unsupported object, unproven reference, or failed exact link."""


def _address(value, label):
    if type(value) is not int or not 0 <= value <= 0xFFFFFFFF:
        raise LinkError(f"{label} must be a 32-bit unsigned integer")
    return value


def _symbol(value):
    if not isinstance(value, str) or not SYMBOL.fullmatch(value):
        raise LinkError(f"Unsupported symbol name: {value!r}")
    if value == ENTRY_SYMBOL:
        raise LinkError(f"Reserved linker symbol: {value}")
    return value


def _bindings(values):
    if not isinstance(values, dict):
        raise LinkError("Bindings must be a dictionary of unique symbol names to addresses")
    result = {}
    for name, address in values.items():
        _symbol(name)
        result[name] = _address(address, f"Binding {name}")
        if name in GP_SYMBOLS and address != GP:
            raise LinkError(f"Binding {name} conflicts with the verified GP 0x{GP:08X}")
    return result


def _format(elf, expected_type):
    if (elf.elfclass != 32 or not elf.little_endian or elf["e_machine"] != "EM_MIPS"
            or elf["e_type"] != expected_type):
        raise LinkError(f"Expected ELF32 little-endian MIPS {expected_type}")
    flags = elf["e_flags"]
    # Both supported assemblers use R5900/MIPS III with EABI32 or EABI64.
    # PIC, CPIC, compressed ISA, NaN2008, and unknown flags are not supported.
    if ((flags & 0xF0000000) != 0x20000000 or (flags & 0x00FF0000) != 0x00920000
            or (flags & 0xF000) not in (0x3000, 0x4000)
            or flags & ~0x20FF7001):
        raise LinkError(f"Unsupported R5900/EABI ELF flags: 0x{flags:08X}")


def inspect_function(object_path, symbol, address, bindings):
    """Validate a target and plan symbol-only normalization, without invoking ld.

    Defined functions in other dedicated sections are changed to SHN_ABS in a
    private object copy only when their retail address is explicitly bound.
    This lets ld discard unrelated source functions without relocating them to
    the selected function's address. Code and relocation entries are untouched.
    """
    _symbol(symbol)
    if symbol in GP_SYMBOLS:
        raise LinkError("A GP linker symbol cannot be the target function")
    _address(address, "Target address")
    if address & 3:
        raise LinkError("Target address must be instruction-aligned")
    bindings = _bindings(bindings)
    if symbol in bindings and bindings[symbol] != address:
        raise LinkError(f"Binding {symbol} conflicts with target address")
    with Path(object_path).open("rb") as stream:
        object_hash = hashlib.sha256(stream.read()).hexdigest()
        stream.seek(0)
        elf = ELFFile(stream)
        _format(elf, "ET_REL")
        symtab = elf.get_section_by_name(".symtab")
        if symtab is None or symtab["sh_entsize"] != 16:
            raise LinkError("Missing or unsupported ELF32 symbol table")
        symbols = list(symtab.iter_symbols())
        matches = [(index, item) for index, item in enumerate(symbols)
                   if item.name == symbol and item["st_info"]["type"] == "STT_FUNC"
                   and isinstance(item["st_shndx"], int)]
        if len(matches) != 1:
            raise LinkError(f"Expected exactly one defined function symbol: {symbol}")
        target_index, target = matches[0]
        section_index = target["st_shndx"]
        if section_index <= 0 or section_index >= elf.num_sections():
            raise LinkError("Target function has invalid section index")
        section = elf.get_section(section_index)
        if (section.name != ".text." + symbol or section["sh_type"] != "SHT_PROGBITS"
                or not section["sh_flags"] & 4 or section["sh_flags"] & ~6):
            raise LinkError("Target requires a dedicated executable .text.<symbol> section; use -ffunction-sections")
        size = target["st_size"]
        if target["st_value"] != 0 or section["sh_addr"] != 0 or not 0 < size <= section["sh_size"]:
            raise LinkError("Invalid dedicated-function symbol bounds")
        if address + size > 0x100000000:
            raise LinkError("Target function exceeds 32-bit address space")
        alignment = section["sh_addralign"]
        if alignment < 4 or alignment & (alignment - 1) or address % alignment:
            raise LinkError("Target address conflicts with function-section alignment")
        if any(index != target_index and item["st_info"]["type"] == "STT_FUNC"
               and item["st_shndx"] == section_index for index, item in enumerate(symbols)):
            raise LinkError("Multiple function symbols share the selected section")
        duplicates = {}
        for item in symbols:
            if item.name:
                duplicates[item.name] = duplicates.get(item.name, 0) + 1
        if duplicates[symbol] != 1:
            raise LinkError(f"Ambiguous duplicate target symbol: {symbol}")
        patches = {}
        resolved = {}
        assignments = {}
        relocations = []

        def bind(index):
            item = symbols[index]
            name = item.name
            location = item["st_shndx"]
            if index == target_index or location == section_index:
                return
            if name and duplicates[name] != 1:
                raise LinkError(f"Ambiguous duplicate object symbol: {name}")
            if location == "SHN_ABS":
                if name in GP_SYMBOLS and item["st_value"] != GP:
                    raise LinkError(f"Absolute symbol {name} conflicts with the verified GP")
                if name in bindings and bindings[name] != item["st_value"]:
                    raise LinkError(f"Binding {name} conflicts with an absolute object symbol")
                if name:
                    resolved[name] = item["st_value"]
                return
            if location == "SHN_UNDEF":
                _symbol(name)
                if name in GP_SYMBOLS:
                    assignments[name] = GP
                elif name in bindings:
                    assignments[name] = bindings[name]
                else:
                    raise LinkError(f"Unknown undefined reference: {name}")
                resolved[name] = assignments[name]
                return
            if not isinstance(location, int) or not 0 < location < elf.num_sections():
                raise LinkError(f"Unsupported symbol definition: {name!r}")
            definition = elf.get_section(location)
            key = name
            if item["st_info"]["type"] == "STT_SECTION":
                if not definition.name.startswith(".text."):
                    raise LinkError(f"Unmapped local data/constant section: {definition.name}")
                key = definition.name[len(".text."):]
                functions = [entry for entry in symbols if entry["st_shndx"] == location
                             and entry["st_info"]["type"] == "STT_FUNC"]
                if len(functions) != 1 or functions[0].name != key or functions[0]["st_value"] != 0:
                    raise LinkError(f"Referenced code section lacks one unambiguous function: {definition.name}")
            elif not definition["sh_flags"] & 4:
                if item["st_info"]["bind"] == "STB_LOCAL" or name.startswith(".LC"):
                    raise LinkError(f"Unmapped compiler-local data/constant: {name}")
            if key not in bindings:
                raise LinkError(f"Missing retail binding for defined reference: {key}")
            if definition["sh_addr"] != 0:
                raise LinkError(f"Unexpected address in relocatable section: {definition.name}")
            _symbol(key)
            # A section symbol maps to its section's base; a named symbol maps
            # to the explicit address of that symbol, retaining relocation addends.
            patches[index] = bindings[key]
            resolved[key] = bindings[key]

        for relocation_section in elf.iter_sections():
            if not isinstance(relocation_section, RelocationSection) or relocation_section["sh_info"] != section_index:
                continue
            if relocation_section.is_RELA() or relocation_section["sh_link"] != elf.get_section_index(".symtab"):
                raise LinkError("Unsupported relocation table or symbol-table association")
            for relocation in relocation_section.iter_relocations():
                offset, kind, index = relocation["r_offset"], relocation["r_info_type"], relocation["r_info_sym"]
                if kind not in RELOCATIONS:
                    raise LinkError(f"Unsupported MIPS relocation type: {kind}")
                if offset & 3 or offset + 4 > size or index >= len(symbols):
                    raise LinkError("Relocation lies outside the selected function or has an invalid symbol")
                if kind:
                    bind(index)
                relocations.append({"offset": offset, "type": kind, "symbol": symbols[index].name})
        return {"symbol": symbol, "address": address, "size": size, "section": section.name,
                "object_sha256": object_hash,
                "symbol_table_offset": symtab["sh_offset"], "symbol_patches": patches,
                "assignments": assignments, "resolved_bindings": resolved, "input_relocations": relocations}


def _linker(binutils):
    directory = Path(binutils)
    paths = [directory / "mips-ps2-decompals-ld.exe", directory / "mips-ps2-decompals-ld"]
    linker = next((path for path in paths if path.is_file()), None)
    if linker is None:
        raise LinkError(f"Pinned PS2 GNU linker is missing from {directory}")
    manifest = json.loads((ROOT / "tools/toolchain_manifest.json").read_text(encoding="utf-8"))
    hashes = {Path(item["path"]).name: item["sha256"] for item in manifest["binutils"]["binaries"]}
    expected = hashes.get(linker.name)
    if expected is None or hashlib.sha256(linker.read_bytes()).hexdigest() != expected:
        raise LinkError(f"PS2 linker does not match the pinned toolchain fingerprint: {linker.name}")
    return linker.resolve()


def link_function(object_path, symbol, address, bindings, output_dir, binutils=DEFAULT_BINUTILS):
    """Return exact linked bytes and audit artifacts, or raise LinkError.

    The return dict contains code, remaining_relocations, linked_path,
    script_path, object_path (normalized private copy), address, size,
    resolved_bindings and input_relocations. No expected/original bytes are
    needed by this linker; the caller compares the complete returned code.
    """
    object_path = Path(object_path).resolve()
    plan = inspect_function(object_path, symbol, address, bindings)
    linker = _linker(binutils)
    output_dir = Path(output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix="link-", dir=output_dir))
    normalized = work / "function.o"
    data = bytearray(object_path.read_bytes())
    if hashlib.sha256(data).hexdigest() != plan["object_sha256"]:
        raise LinkError("Input object changed during linking")
    for index, value in plan["symbol_patches"].items():
        entry = plan["symbol_table_offset"] + index * 16
        struct.pack_into("<I", data, entry + 4, value)
        struct.pack_into("<H", data, entry + 14, 0xFFF1)  # SHN_ABS.
        if data[entry + 12] & 0x0F == 3:  # STT_SECTION is no longer a section after binding.
            data[entry + 12] &= 0xF0  # STT_NOTYPE; preserve local/global binding.
    normalized.write_bytes(data)
    script = work / "function.ld"
    lines = [f"ENTRY({ENTRY_SYMBOL})", f"{ENTRY_SYMBOL} = 0x{address:08X};", f"_gp = 0x{GP:08X};"]
    lines.extend(f"{name} = 0x{value:08X};" for name, value in sorted(plan["assignments"].items()) if name != "_gp")
    lines.extend(["SECTIONS {", f'  .text 0x{address:08X} : {{ KEEP(*("{plan["section"]}")) }}',
                  "  /DISCARD/ : { *(*) }", "}"])
    script.write_text("\n".join(lines) + "\n", encoding="ascii")
    linked = work / "function.elf"
    command = [str(linker), "-EL", "-G0", "--fatal-warnings", "--no-undefined", "--check-sections",
               "--orphan-handling=error", "-T", str(script), "-o", str(linked), str(normalized)]
    process = subprocess.run(command, capture_output=True, text=True)
    if process.returncode:
        raise LinkError("GNU PS2 link failed:\n" + (process.stderr + process.stdout)[-6000:])
    remaining = []
    with linked.open("rb") as stream:
        elf = ELFFile(stream)
        _format(elf, "ET_EXEC")
        text = elf.get_section_by_name(".text")
        symbols = elf.get_section_by_name(".symtab")
        matches = [entry for entry in symbols.iter_symbols() if entry.name == symbol
                   and entry["st_info"]["type"] == "STT_FUNC"] if symbols else []
        if len(matches) != 1 or matches[0]["st_value"] != address or matches[0]["st_size"] != plan["size"]:
            raise LinkError("Linked function symbol address or size changed")
        if (text is None or text["sh_addr"] != address or matches[0]["st_shndx"] != elf.get_section_index(".text")
                or text["sh_size"] < plan["size"]):
            raise LinkError("Linked code does not occupy the requested original address")
        if any(section["sh_flags"] & 4 and section.name != ".text" for section in elf.iter_sections()):
            raise LinkError("Unexpected additional executable output section")
        for section in elf.iter_sections():
            if isinstance(section, RelocationSection):
                remaining.extend({"section": section.name, "offset": entry["r_offset"], "type": entry["r_info_type"]}
                                 for entry in section.iter_relocations())
        if remaining:
            raise LinkError("Linked output retains unresolved relocation entries")
        code = text.data()[:plan["size"]]
    return {"code": code, "remaining_relocations": remaining, "linked_path": linked, "script_path": script,
            "object_path": normalized, "address": address, "size": plan["size"],
            "resolved_bindings": plan["resolved_bindings"], "input_relocations": plan["input_relocations"]}
