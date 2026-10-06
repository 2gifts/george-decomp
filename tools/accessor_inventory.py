"""Audit short accessor leads using exact instructions and entry references.

Generated function geometry alone is insufficient. Accepted leads have an
encoded direct JAL/J edge, or a contiguous adjusted-function array whose nearby
header address is materialized and stored by original code. This inventory
does not generate source or claim compiler matches.
"""

import argparse
import bisect
import csv
import hashlib
import json
from pathlib import Path
import re
import struct

from elftools.elf.elffile import ELFFile

from analyze import validated_elf

ROOT = Path(__file__).resolve().parents[1]
LOADS = {32: "s8", 33: "s16", 35: "u32", 36: "u8", 37: "u16", 49: "float"}


def written_register(word):
    opcode = word >> 26
    if opcode in (8, 9, 10, 11, 12, 13, 14, 15, 24, 25,
                  26, 27, 30, 32, 33, 34, 35, 36, 37, 38, 39, 55, 56, 60):
        return (word >> 16) & 31
    if opcode == 0 and (word & 63) not in (8, 12, 13, 26, 27):
        return (word >> 11) & 31
    if opcode == 28:  # MMI integer result register.
        return (word >> 11) & 31
    if opcode in (16, 17, 18) and ((word >> 21) & 31) in (0, 1, 2):
        return (word >> 16) & 31
    return None


def has_delay_slot(word):
    opcode = word >> 26
    return opcode in (1, 2, 3, 4, 5, 6, 7, 20, 21, 22, 23) or (
        opcode == 0 and (word & 63) in (8, 9)) or (
        opcode in (16, 17, 18) and ((word >> 21) & 31) == 8)


def materializations(text, address):
    """Conservative straight-line LUI/ADDIU/SW proofs, including call delay SW.

    Stop on calls/branches after their single delay slot. A register write
    invalidates the proof. No inference is made from symbol names alone.
    """
    words = list(struct.unpack(f"<{len(text) // 4}I", text))
    result = {}
    for index, first in enumerate(words):
        if first >> 26 != 15:
            continue
        if index and has_delay_slot(words[index - 1]):
            continue
        register = (first >> 16) & 31
        if register == 0:
            continue
        high = (first & 0xFFFF) << 16
        combined = None
        branch_delay = False
        for next_index in range(index + 1, min(index + 13, len(words))):
            word = words[next_index]
            opcode = word >> 26
            rt, rs = (word >> 16) & 31, (word >> 21) & 31
            if combined is None and opcode == 9 and rt == register and rs == register:
                low = word & 0xFFFF
                if low & 0x8000:
                    low -= 0x10000
                combined = (high + low) & 0xFFFFFFFF
                combine_pc = address + next_index * 4
            elif combined is not None and opcode == 43 and rt == register:
                displacement = word & 0xFFFF
                if displacement & 0x8000:
                    displacement -= 0x10000
                proof = {"lui_address": f"0x{address + index * 4:08X}",
                         "addiu_address": f"0x{combine_pc:08X}",
                         "store_address": f"0x{address + next_index * 4:08X}",
                         "store_base_register": rs, "store_offset": displacement}
                result.setdefault(combined, []).append(proof)
                break
            elif written_register(word) == register:
                break
            if branch_delay:
                break
            if has_delay_slot(word):
                branch_delay = True
            if opcode == 0 and (word & 63) in (12, 13):
                break
    return result


def inventory(elf_path):
    spec, original = validated_elf(elf_path)
    with (ROOT / "build/functions.csv").open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    functions = {int(row["address"], 16): row for row in rows}
    starts = sorted(functions)
    leads = []
    for start in starts:
        row = functions[start]
        size = int(row["length"], 16)
        if not 8 <= size <= 16:
            continue
        name = row["name"]
        assembly = (ROOT / "build/functions" / f"{name}.s").read_text(encoding="utf-8")
        body, separator, padding = assembly.partition(f"endlabel {name}")
        if not separator:
            continue
        instructions = re.findall(r"/\*\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]{8})\s+\*/", body)
        # This batch requires exactly JR RA + load in its delay slot.
        if len(instructions) != 2:
            continue
        offset = int(row["vrom"], 16)
        if [int(item[1], 16) for item in instructions] != [start, start + 4]:
            continue
        raw = original[offset:offset + 8]
        if raw != bytes.fromhex("".join(item[2] for item in instructions)):
            raise ValueError(f"Assembly bytes disagree for {name}")
        first, load = struct.unpack("<II", raw)
        opcode, base, target = load >> 26, (load >> 21) & 31, (load >> 16) & 31
        if first != 0x03E00008 or opcode not in LOADS or base != 4:
            continue
        if target != (0 if opcode == 49 else 2):
            continue
        padding_words = re.findall(r"/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+\*/", padding)
        if any(word != "00000000" for word in padding_words):
            continue
        # The next generated start must immediately follow the body/padding.
        next_index = bisect.bisect_right(starts, start)
        if next_index >= len(starts) or starts[next_index] != start + size:
            continue
        if size != 8 + 4 * len(padding_words):
            continue
        displacement = load & 0xFFFF
        if displacement & 0x8000:
            displacement -= 0x10000
        leads.append({"name": name, "address": f"0x{start:08X}",
                      "file_offset": f"0x{offset:08X}", "size": 8,
                      "kind": LOADS[opcode], "offset": displacement,
                      "original_sha256": hashlib.sha256(raw).hexdigest(),
                      "alignment_padding_size": size - 8, "entry_references": []})
    targets = {int(item["address"], 16): item for item in leads}
    with elf_path.open("rb") as stream:
        elf = ELFFile(stream)
        text = elf.get_section_by_name(".text")
        words = struct.unpack(f"<{text['sh_size'] // 4}I", text.data())
        stored_addresses = materializations(text.data(), text["sh_addr"])
        for index, word in enumerate(words):
            opcode = word >> 26
            if opcode not in (2, 3):
                continue
            pc = text["sh_addr"] + index * 4
            target = ((pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
            if target not in targets:
                continue
            caller_start = starts[bisect.bisect_right(starts, pc) - 1]
            caller = functions[caller_start]
            if pc >= caller_start + int(caller["length"], 16):
                continue
            targets[target]["entry_references"].append({"kind": "direct_jal" if opcode == 3 else "direct_jump",
                "instruction_address": f"0x{pc:08X}", "caller": caller["name"]})
        for section in elf.iter_sections():
            if section.name not in (".data", ".rodata", ".sdata"):
                continue
            data = section.data()
            def pair_at(offset):
                if offset < 0 or offset + 8 > len(data):
                    return None
                adjustment, reserved, pointer = struct.unpack_from("<hhI", data, offset)
                if adjustment != 0 or reserved != 0 or pointer not in functions:
                    return None
                return pointer
            for offset in range(4, len(data) - 3, 4):
                target = struct.unpack_from("<I", data, offset)[0]
                if target not in targets or pair_at(offset - 4) != target:
                    continue
                first = last = offset - 4
                while pair_at(first - 8) is not None:
                    first -= 8
                while pair_at(last + 8) is not None:
                    last += 8
                if last - first < 16:
                    continue
                array_start = section["sh_addr"] + first
                # A stored address may denote the immediately preceding header,
                # the first pair itself or a bounded secondary table prefix.
                bases = [address for address in stored_addresses
                         if array_start - 8 <= address <= section["sh_addr"] + offset - 4]
                if not bases:
                    continue
                base = max(bases)
                if base % 4:
                    continue
                region = data[first:last + 8]
                targets[target]["entry_references"].append({"kind": "stored_dispatch_pair",
                    "section": section.name, "pointer_address": f"0x{section['sh_addr'] + offset:08X}",
                    "adjustment": 0, "reserved_halfword": 0,
                    "paired_region_address": f"0x{array_start:08X}",
                    "paired_region_size": len(region),
                    "paired_region_sha256": hashlib.sha256(region).hexdigest(),
                    "stored_table_address": f"0x{base:08X}",
                    "materialization": stored_addresses[base][0]})
    accepted, deferred = [], []
    for item in leads:
        if item["entry_references"]:
            accepted.append(item)
        else:
            item["deferred_reason"] = "No encoded direct entry edge or stored dispatch-pair proof; function geometry alone remains insufficient."
            deferred.append(item)
    return {"schema_version": 1, "executable_sha256": spec["executable_sha256"],
            "method": "Exact complete 8-byte load/return geometry plus direct edges or stored adjusted-function-array evidence; no original class names inferred.",
            "accepted": accepted, "deferred": deferred}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=ROOT / "orig/SLUS_216.68")
    parser.add_argument("--output", type=Path, default=ROOT / "build/accessor_inventory.json")
    args = parser.parse_args()
    result = inventory(args.elf)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(f"Accepted {len(result['accepted'])}; deferred {len(result['deferred'])} complete accessor leads.")


if __name__ == "__main__":
    main()
