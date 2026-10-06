"""Resolve one dedicated function section at a proven retail address with GNU ld.

Explicit bindings refer to existing retail functions or globals. Optional local
read-only mappings require exact expected bytes, their SHA-256, and fixed retail
addresses; the caller separately proves those against the original executable.
Separate local NOBITS mappings require complete zero-initialized section
geometry; they never permit initialized writable data or relax read-only guards.
No instruction or relocation bytes are masked, and unresolved references fail.
"""

import hashlib
import json
import re
import struct
import subprocess
import tempfile
from pathlib import Path

from elftools.common.exceptions import ELFError
from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_BINUTILS = ROOT / "tools/vendor/binutils-v0.10"
GP = 0x0045FB70
SYMBOL = re.compile(r"[A-Za-z_.$][A-Za-z0-9_.$]*\Z")
RELOCATIONS = {0, 2, 4, 5, 6, 7, 10, 12}  # NONE, 32, 26, HI16, LO16, GPREL16, PC16, GPREL32.
GP_SYMBOLS = {"_gp", "__gnu_local_gp"}
ENTRY_SYMBOL = "__george_link_entry"
SECTION = re.compile(r"\.[A-Za-z0-9_.$-]+\Z")
SHA256 = re.compile(r"[0-9a-f]{64}\Z")
READONLY_FLAGS = 2 | 0x10 | 0x20 | 0x10000000  # ALLOC, MERGE, STRINGS, MIPS_GPREL.
SELF_POINTER_POLICY = "self_r_mips_32"
MAX_NOBITS_SIZE = 65536


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


def _self_pointer_relocations(elf, section_index, section, symbols, symtab_index):
    """Validate REL self-pointers without changing any input or expected bytes."""
    tables = [item for item in elf.iter_sections()
              if isinstance(item, RelocationSection) and item["sh_info"] == section_index
              and item["sh_size"]]
    if len(tables) != 1:
        raise LinkError("Self-pointer policy requires one nonempty relocation table")
    table = tables[0]
    if (table.is_RELA() or table["sh_link"] != symtab_index or table["sh_entsize"] != 8
            or table["sh_size"] % 8):
        raise LinkError("Unsupported self-pointer REL table or symbol-table association")
    data, size = section.data(), section["sh_size"]
    result, positions = [], set()
    for relocation in table.iter_relocations():
        offset, kind, index = relocation["r_offset"], relocation["r_info_type"], relocation["r_info_sym"]
        if kind != 2:
            raise LinkError("Self-pointer data permits only R_MIPS_32 relocations")
        if offset & 3 or offset + 4 > size or index >= len(symbols):
            raise LinkError("Self-pointer relocation lies outside complete data or has an invalid symbol")
        if offset in positions:
            raise LinkError("Duplicate self-pointer relocation offset")
        positions.add(offset)
        item = symbols[index]
        value, item_size, kind = item["st_value"], item["st_size"], item["st_info"]["type"]
        if (item["st_shndx"] != section_index or kind not in ("STT_SECTION", "STT_OBJECT", "STT_NOTYPE")
                or value >= size or value + item_size > size or (kind == "STT_SECTION" and value != 0)):
            raise LinkError("Self-pointer symbol must be bounded data in the same mapped section")
        word = struct.unpack_from("<I", data, offset)[0]
        addend = word - (0x100000000 if word & 0x80000000 else 0)
        relative = value + addend
        if not 0 <= relative < size:
            raise LinkError("Self-pointer addend points outside its complete mapped section")
        result.append({"offset": offset, "type": 2, "symbol": item.name, "symbol_index": index,
                       "addend": addend, "relative_offset": relative})
    return result


def _mapped_sections(values, elf, code_address, code_size, symbols, symtab_index):
    if values is None:
        values = {}
    if not isinstance(values, dict):
        raise LinkError("Mapped sections must be a dictionary keyed by exact input section name")
    result = {}
    ranges = [(code_address, code_address + code_size, "selected code section")]
    for name, mapping in values.items():
        if not isinstance(name, str) or not SECTION.fullmatch(name):
            raise LinkError(f"Unsupported mapped section name: {name!r}")
        required = {"address", "expected_bytes", "expected_sha256"}
        if (not isinstance(mapping, dict) or not required <= set(mapping)
                or set(mapping) - required - {"relocation_policy"}):
            raise LinkError(f"Mapped section {name} requires byte/hash/address proof and optional relocation_policy only")
        policy = mapping.get("relocation_policy")
        if "relocation_policy" in mapping and policy != SELF_POINTER_POLICY:
            raise LinkError(f"Unsupported mapped-section relocation policy: {policy!r}")
        address = _address(mapping["address"], f"Mapped section {name} address")
        expected, digest = mapping["expected_bytes"], mapping["expected_sha256"]
        if (type(expected) is not bytes or not expected or not isinstance(digest, str)
                or not SHA256.fullmatch(digest) or hashlib.sha256(expected).hexdigest() != digest):
            raise LinkError(f"Invalid complete byte/hash proof for mapped section: {name}")
        matches = [(index, section) for index, section in enumerate(elf.iter_sections()) if section.name == name]
        if len(matches) != 1:
            raise LinkError(f"Missing or ambiguous mapped input section: {name}")
        index, section = matches[0]
        flags, alignment = section["sh_flags"], section["sh_addralign"]
        if (section["sh_type"] != "SHT_PROGBITS" or not flags & 2 or flags & ~READONLY_FLAGS
                or section["sh_addr"] != 0):
            raise LinkError(f"Mapped section must be supported allocated read-only data: {name}")
        if alignment < 1 or alignment & (alignment - 1) or address % alignment:
            raise LinkError(f"Mapped section address conflicts with alignment: {name}")
        if flags & 0x20 and not flags & 0x10:
            raise LinkError(f"String section lacks merge flag: {name}")
        if flags & 0x10 and (not section["sh_entsize"] or section["sh_size"] % section["sh_entsize"]):
            raise LinkError(f"Unsupported merge entry geometry: {name}")
        data = section.data()
        if section["sh_size"] != len(expected) or (policy is None and data != expected):
            raise LinkError(f"Mapped section bytes differ from complete original proof: {name}")
        if address + len(data) > 0x100000000:
            raise LinkError(f"Mapped section exceeds 32-bit address space: {name}")
        data_relocations = []
        if policy == SELF_POINTER_POLICY:
            data_relocations = _self_pointer_relocations(elf, index, section, symbols, symtab_index)
        else:
            for relocation_section in elf.iter_sections():
                if isinstance(relocation_section, RelocationSection) and relocation_section["sh_info"] == index:
                    if relocation_section.num_relocations():
                        raise LinkError(f"Relocation-bearing mapped data is unsupported: {name}")
        ranges.append((address, address + len(data), name))
        result[index] = {"name": name, "address": address, "size": len(data), "data": data,
                         "expected_bytes": expected, "input_sha256": hashlib.sha256(data).hexdigest(),
                         "sha256": digest, "relocation_policy": policy, "input_relocations": data_relocations,
                         "output_section": f".george_rodata_{len(result)}"}
    for previous, current in zip(sorted(ranges), sorted(ranges)[1:]):
        if previous[1] > current[0]:
            raise LinkError(f"Mapped section address overlap: {previous[2]} and {current[2]}")
    return result


def _mapped_addends(relocations, symbols, mapped, code):
    """Require every local data relocation's full REL addend inside its proof."""
    pending = {}

    def check(index, addend):
        item = symbols[index]
        mapping = mapped[item["st_shndx"]]
        relative = item["st_value"] + addend
        if not 0 <= relative < mapping["size"]:
            raise LinkError(f"Local relocation points outside proven mapped data: {mapping['name']}")

    for relocation in relocations:
        index, kind = relocation["symbol_index"], relocation["type"]
        if not kind or symbols[index]["st_shndx"] not in mapped:
            continue
        word = struct.unpack_from("<I", code, relocation["offset"])[0]
        low = (word & 0xFFFF) - (0x10000 if word & 0x8000 else 0)
        if kind == 5:  # HI16 entries may share one matching LO16.
            pending.setdefault(index, []).append((word & 0xFFFF) << 16)
        elif kind == 6:
            for upper in pending.pop(index, [0]):
                value = (upper + low) & 0xFFFFFFFF
                check(index, value - (0x100000000 if value & 0x80000000 else 0))
        elif kind == 7:  # GPREL16; supported object-local GP must be zero.
            check(index, low)
        elif kind in (2, 12):  # 32 and GPREL32.
            check(index, word - (0x100000000 if word & 0x80000000 else 0))
        else:
            raise LinkError("Read-only data referenced by unsupported control-flow relocation")
    if pending:
        raise LinkError("Unpaired HI16 relocation to mapped read-only data")


def _nobits_sections(values, elf, occupied_ranges):
    """Retain only complete explicitly proven compiler-local zero storage."""
    if values is None:
        values = {}
    if not isinstance(values, dict):
        raise LinkError("NOBITS mappings must be a dictionary keyed by exact input section name")
    result = {}
    ranges = list(occupied_ranges)
    for name, proof in values.items():
        if not isinstance(name, str) or not SECTION.fullmatch(name) or not (name == ".bss" or name.startswith(".bss.")):
            raise LinkError(f"Unsupported NOBITS section name: {name!r}")
        if not isinstance(proof, dict) or set(proof) != {"address", "size", "zero_sha256"}:
            raise LinkError("NOBITS mapping requires complete address/size/zero_sha256 proof only")
        address = _address(proof["address"], f"NOBITS {name} address")
        size, digest = proof["size"], proof["zero_sha256"]
        if (type(size) is not int or not 0 < size <= MAX_NOBITS_SIZE or address + size > 0x100000000
                or not isinstance(digest, str) or not SHA256.fullmatch(digest)
                or hashlib.sha256(bytes(size)).hexdigest() != digest):
            raise LinkError("Invalid complete NOBITS size/zero initialization digest")
        matches = [(index, section) for index, section in enumerate(elf.iter_sections()) if section.name == name]
        if len(matches) != 1:
            raise LinkError(f"Missing or ambiguous NOBITS input section: {name}")
        index, section = matches[0]
        if (section["sh_type"] != "SHT_NOBITS" or section["sh_flags"] != 3
                or section["sh_addr"] != 0 or section["sh_size"] != size
                or section["sh_entsize"] != 0):
            raise LinkError("NOBITS mapping requires a complete allocated writable NOBITS section")
        alignment = section["sh_addralign"]
        if alignment < 1 or alignment & (alignment - 1) or address % alignment:
            raise LinkError("NOBITS section address conflicts with alignment")
        for table in elf.iter_sections():
            if isinstance(table, RelocationSection) and table["sh_info"] == index and table["sh_size"]:
                raise LinkError("NOBITS storage cannot have data relocations")
        ranges.append((address, address + size, name))
        result[index] = {"name": name, "address": address, "size": size, "zero_sha256": digest,
                         "alignment": alignment, "input_relocations": [],
                         "output_section": f".george_nobits_{len(result)}"}
    for previous, current in zip(sorted(ranges), sorted(ranges)[1:]):
        if previous[1] > current[0]:
            raise LinkError(f"NOBITS mapped address overlap: {previous[2]} and {current[2]}")
    return result


def _inspect_function(object_path, symbol, address, bindings, mapped_sections=None, mapped_nobits=None):
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
        if address + section["sh_size"] > 0x100000000:
            raise LinkError("Selected code section exceeds 32-bit address space")
        mapped = _mapped_sections(mapped_sections, elf, address, section["sh_size"], symbols,
                                  elf.get_section_index(".symtab"))
        readonly = dict(mapped)
        occupied = [(address, address + section["sh_size"], "selected code section")]
        occupied.extend((item["address"], item["address"] + item["size"], item["name"]) for item in mapped.values())
        nobits = _nobits_sections(mapped_nobits, elf, occupied)
        mapped.update(nobits)
        used_mappings = set()
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
            if location in mapped:
                mapping = mapped[location]
                if name:
                    _symbol(name)
                value = item["st_value"]
                if (item["st_info"]["type"] not in ("STT_SECTION", "STT_OBJECT", "STT_NOTYPE")
                        or value < 0 or value >= mapping["size"]
                        or value + item["st_size"] > mapping["size"]
                        or (item["st_info"]["type"] == "STT_SECTION" and value != 0)):
                    raise LinkError(f"Invalid symbol bounds/type in mapped read-only section: {name!r}")
                if location in nobits and item["st_info"]["bind"] != "STB_LOCAL":
                    raise LinkError("NOBITS mapping accepts only bounded compiler-local data symbols")
                mapped_address = mapping["address"] + value
                if name in bindings and bindings[name] != mapped_address:
                    raise LinkError(f"Binding {name} conflicts with fixed mapped section address")
                used_mappings.add(location)
                if name:
                    resolved[name] = mapped_address
                # Retain this definition and section. GNU ld resolves relocation
                # addends against the fixed mapped section without byte patching.
                return
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
                relocations.append({"offset": offset, "type": kind, "symbol": symbols[index].name,
                                    "symbol_index": index})
        if any(item["type"] in (7, 12) and symbols[item["symbol_index"]]["st_shndx"] in mapped
               for item in relocations):
            if elf.get_section_by_name(".MIPS.options") is not None:
                raise LinkError("Mapped GP-relative data with .MIPS.options is unsupported")
            for info in elf.iter_sections():
                if info["sh_type"] == "SHT_MIPS_REGINFO":
                    raw = info.data()
                    if len(raw) != 24 or struct.unpack_from("<I", raw, 20)[0] != 0:
                        raise LinkError("Mapped GP-relative data requires a zero object-local GP")
        _mapped_addends(relocations, symbols, mapped, section.data())
        unused = set(mapped) - used_mappings
        if unused:
            raise LinkError("Unused mapped sections: " + ", ".join(mapped[index]["name"] for index in sorted(unused)))
        # Only code-referenced mappings are retained. Validate their data symbols
        # using the same ambiguity/binding guards, without patching definitions.
        for mapping in mapped.values():
            for relocation in mapping["input_relocations"]:
                bind(relocation["symbol_index"])
        return {"symbol": symbol, "address": address, "size": size, "section": section.name,
                "object_sha256": object_hash,
                "symbol_table_offset": symtab["sh_offset"], "symbol_patches": patches,
                "assignments": assignments, "resolved_bindings": resolved, "input_relocations": relocations,
                "mapped_sections": list(readonly.values()), "mapped_nobits": list(nobits.values())}


def inspect_function(object_path, symbol, address, bindings, mapped_sections=None, mapped_nobits=None):
    """Plan strict original-address linking; malformed ELF input is rejected."""
    try:
        return _inspect_function(object_path, symbol, address, bindings, mapped_sections, mapped_nobits)
    except (ELFError, struct.error) as error:
        raise LinkError(f"Malformed input ELF: {error}") from error


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


def link_function(object_path, symbol, address, bindings, output_dir, binutils=DEFAULT_BINUTILS,
                  mapped_sections=None, mapped_nobits=None):
    """Return exact linked bytes and audit artifacts, or raise LinkError.

    The return dict contains code, remaining_relocations, linked_path,
    script_path, object_path (normalized private copy), address, size,
    resolved_bindings and input_relocations. No expected/original bytes are
    needed for code; the caller compares the complete returned code. Optional
    mapped_sections requires {section_name: {address, expected_bytes,
    expected_sha256}}; exact input data is checked before linking. The optional
    relocation_policy='self_r_mips_32' permits only same-section REL pointers;
    the whole data proof is then checked against actual GNU ld output. Returned
    mapped_sections includes actual linked data, address, size and SHA-256 for
    the caller's independent comparison against the original read-only data.
    Separate mapped_nobits={section_name:{address,size,zero_sha256}} retains a
    complete compiler-local zero-initialized NOBITS section with GNU NOLOAD.
    The caller independently proves its original memory geometry and bindings.
    """
    object_path = Path(object_path).resolve()
    plan = inspect_function(object_path, symbol, address, bindings, mapped_sections, mapped_nobits)
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
    lines.extend(["SECTIONS {", f'  .text 0x{address:08X} : {{ KEEP(*("{plan["section"]}")) }}'])
    for mapping in plan["mapped_sections"]:
        lines.append(f'  {mapping["output_section"]} 0x{mapping["address"]:08X} : {{ KEEP(*("{mapping["name"]}")) }}')
    for mapping in plan["mapped_nobits"]:
        lines.append(f'  {mapping["output_section"]} 0x{mapping["address"]:08X} (NOLOAD) : {{ KEEP(*("{mapping["name"]}")) }}')
    lines.extend(["  /DISCARD/ : { *(*) }", "}"])
    script.write_text("\n".join(lines) + "\n", encoding="ascii")
    linked = work / "function.elf"
    command = [str(linker), "-EL", "-G0", "--fatal-warnings", "--no-undefined", "--check-sections",
               "--orphan-handling=error", "-T", str(script), "-o", str(linked), str(normalized)]
    process = subprocess.run(command, capture_output=True, text=True)
    if process.returncode:
        raise LinkError("GNU PS2 link failed:\n" + (process.stderr + process.stdout)[-6000:])
    remaining, linked_mappings, linked_nobits = [], [], []
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
        for mapping in plan["mapped_sections"]:
            section = elf.get_section_by_name(mapping["output_section"])
            if (section is None or section["sh_type"] != "SHT_PROGBITS" or section["sh_flags"] & ~READONLY_FLAGS
                    or not section["sh_flags"] & 2 or section["sh_addr"] != mapping["address"]
                    or section["sh_size"] != mapping["size"] or section.data() != mapping["expected_bytes"]):
                raise LinkError(f"Linked mapped section differs from fixed original proof: {mapping['name']}")
            actual = section.data()
            linked_mappings.append({**mapping, "data": actual, "sha256": hashlib.sha256(actual).hexdigest()})
        for mapping in plan["mapped_nobits"]:
            section = elf.get_section_by_name(mapping["output_section"])
            if (section is None or section["sh_type"] != "SHT_NOBITS" or section["sh_flags"] != 3
                    or section["sh_addr"] != mapping["address"] or section["sh_size"] != mapping["size"]
                    or section["sh_addralign"] != mapping["alignment"] or section["sh_entsize"] != 0):
                raise LinkError(f"Linked NOBITS storage differs from complete original proof: {mapping['name']}")
            linked_nobits.append(dict(mapping))
        expected_allocated = {".text", *(mapping["output_section"] for mapping in plan["mapped_sections"]),
                              *(mapping["output_section"] for mapping in plan["mapped_nobits"])}
        if any(section["sh_flags"] & 2 and section.name not in expected_allocated for section in elf.iter_sections()):
            raise LinkError("Unexpected allocated output section")
        for section in elf.iter_sections():
            if isinstance(section, RelocationSection):
                remaining.extend({"section": section.name, "offset": entry["r_offset"], "type": entry["r_info_type"]}
                                 for entry in section.iter_relocations())
        if remaining:
            raise LinkError("Linked output retains unresolved relocation entries")
        code = text.data()[:plan["size"]]
    return {"code": code, "remaining_relocations": remaining, "linked_path": linked, "script_path": script,
            "object_path": normalized, "address": address, "size": plan["size"],
            "resolved_bindings": plan["resolved_bindings"], "input_relocations": plan["input_relocations"],
            "mapped_sections": linked_mappings, "mapped_nobits": linked_nobits}
