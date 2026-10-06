"""Check imported runtime data identities without awarding function matches.

Only complete symbol/section bytes are compared. Locale pointer relocation
proof uses actual R_MIPS_32 entries referencing the same readonly section;
no executable bytes are changed, masked, or credited as a C match.
"""
import hashlib
import json
import struct
import subprocess

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

from analyze import ROOT, validated_elf, write_json
from verify import number, prepare_compiler


def main():
    _, original = validated_elf(ROOT / "orig/SLUS_216.68")
    with (ROOT / "orig/SLUS_216.68").open("rb") as stream:
        original_sections = [dict(section.header) for section in ELFFile(stream).iter_sections()]
    manifest = json.loads((ROOT / "config/runtime_functions.json").read_text(encoding="utf-8"))
    profiles = json.loads((ROOT / "config/compiler_profiles.json").read_text(encoding="utf-8"))["profiles"]
    output = ROOT / "build/reuse/runtime_data"
    output.mkdir(parents=True, exist_ok=True)
    results = []
    for proof in manifest["data_identity_proofs"]:
        address, file_offset, size = number(proof["address"]), number(proof["file_offset"]), proof["size"]
        targets = [section for section in original_sections
                   if section["sh_type"] == "SHT_PROGBITS" and section["sh_flags"] & 2
                   and not section["sh_flags"] & 4
                   and section["sh_addr"] <= address
                   and address + size <= section["sh_addr"] + section["sh_size"]
                   and file_offset == section["sh_offset"] + address - section["sh_addr"]]
        if len(targets) != 1 or size <= 0:
            raise ValueError("Data proof must map exactly inside allocated non-executable retail data")
        source = ROOT / proof["source"]
        if hashlib.sha256(source.read_bytes()).hexdigest() != proof["upstream_source_sha256"]:
            raise ValueError(f"Imported upstream source changed: {proof['source']}")
        profile = profiles[proof["compiler_profile"]]
        setup = prepare_compiler(profile)
        object_path = output / (proof["name"] + ".o")
        command = [str(setup["compiler"]), *profile["command_prefix"], *proof["compile_flags"],
                   "-I" + str(source.parent),
                   *["-I" + str(ROOT / path) for path in profile["include_dirs"]],
                   "-c", str(source), "-o", str(object_path)]
        subprocess.run(command, cwd=ROOT, env=setup["env"], check=True)
        with object_path.open("rb") as stream:
            elf = ELFFile(stream)
            if (elf.elfclass != 32 or not elf.little_endian or elf["e_machine"] != "EM_MIPS"
                    or elf["e_type"] != "ET_REL"):
                raise ValueError("Expected a little-endian ELF32 MIPS relocatable object")
            if "symbol" in proof:
                symbols = elf.get_section_by_name(".symtab").get_symbol_by_name(proof["symbol"])
                if symbols is None or len(symbols) != 1:
                    raise ValueError("Missing or ambiguous data symbol")
                symbol = symbols[0]
                section_index = symbol["st_shndx"]
                if (symbol["st_info"]["type"] != "STT_OBJECT" or not isinstance(section_index, int)
                        or not 0 < section_index < elf.num_sections()):
                    raise ValueError("Expected a bounded defined data object")
                section = elf.get_section(section_index)
                offset = symbol["st_value"]
                if symbol["st_size"] != size or offset < 0 or offset + size > section["sh_size"]:
                    raise ValueError("Data symbol size changed")
                data = bytearray(section.data()[offset:offset + symbol["st_size"]])
            else:
                section_index = elf.get_section_index(proof["input_section"])
                section = elf.get_section(section_index)
                offset = 0
                data = bytearray(section.data())
            if (section["sh_type"] != "SHT_PROGBITS" or len(data) != size
                    or not section["sh_flags"] & 2 or section["sh_flags"] & 4
                    or bool(section["sh_flags"] & 1) != bool(targets[0]["sh_flags"] & 1)):
                raise ValueError("Unexpected complete runtime data section")
            relocations = [(table, rel) for table in elf.iter_sections()
                           if isinstance(table, RelocationSection) and table["sh_info"] == section_index
                           for rel in table.iter_relocations()
                           if offset <= rel["r_offset"] < offset + len(data)]
            if len(relocations) != proof.get("self_pointer_relocations", 0):
                raise ValueError("Runtime data relocation count changed")
            for table, relocation in relocations:
                if table.is_RELA():
                    raise ValueError("Only original-style REL data relocations are supported")
                relocation_symbols = elf.get_section(table["sh_link"])
                if relocation_symbols["sh_type"] != "SHT_SYMTAB":
                    raise ValueError("Data relocation must link to its actual symbol table")
                symbol = relocation_symbols.get_symbol(relocation["r_info_sym"])
                if (relocation["r_info_type"] != 2 or symbol["st_shndx"] != section_index
                        or symbol["st_value"] != 0 or section["sh_flags"] & 1):
                    raise ValueError("Only readonly self-section R_MIPS_32 pointers are supported")
                position = relocation["r_offset"] - offset
                addend = struct.unpack_from("<I", data, position)[0]
                if position & 3 or not 0 <= addend < len(data):
                    raise ValueError("Invalid readonly pointer addend")
                struct.pack_into("<I", data, position, address + addend)
        expected = original[file_offset:file_offset + size]
        digest = hashlib.sha256(expected).hexdigest()
        if digest != proof["original_sha256"] or bytes(data) != expected:
            raise ValueError(f"Complete runtime data identity differs: {proof['name']}")
        results.append({"name": proof["name"], "address": proof["address"], "size": len(data),
                        "sha256": digest, "resolved_pointer_relocations": len(relocations),
                        "function_progress_bytes": 0})
    write_json(output / "comparison.json", {"schema_version": 1, "proofs": results})
    print(f"Verified {len(results)} complete upstream runtime data identities; no function progress awarded.")


if __name__ == "__main__":
    main()
