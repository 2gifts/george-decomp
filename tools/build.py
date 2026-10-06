"""Build a byte-exact partial reconstruction of the retail ELF.

The main .text is rebuilt from generated assembly, with verified C/upstream
assembly functions substituted from compiled objects. Original ELF metadata,
data, .rentext, and VU programs are retained from the user-supplied executable.
This is a hybrid reconstruction, not a complete source build or a PC port.
"""
import hashlib
import json
import subprocess
import sys

from elftools.elf.elffile import ELFFile
from analyze import ROOT, validated_elf, write_json
from verify import object_functions, validate_target_function


def main():
    subprocess.run([sys.executable, str(ROOT / "tools/bootstrap_toolchain.py"), "--verify-only"], cwd=ROOT, check=True)
    # Recompile and recheck on every build so stale artifacts cannot earn matches.
    for script in ("baseline.py", "verify.py"):
        subprocess.run([sys.executable, str(ROOT / "tools" / script)], cwd=ROOT, check=True)
    spec, original = validated_elf(ROOT / "orig/SLUS_216.68")
    with (ROOT / "orig/SLUS_216.68").open("rb") as stream:
        elf = ELFFile(stream)
        sections = [{"name": name, "offset": elf.get_section_by_name(name)["sh_offset"],
                     "address": elf.get_section_by_name(name)["sh_addr"],
                     "size": elf.get_section_by_name(name)["sh_size"]}
                    for name in (".text", ".rentext", ".vutext")]
    text = sections[0]
    with (ROOT / "build/text.elf").open("rb") as stream:
        compiled_text = ELFFile(stream).get_section_by_name(".text").data()
    if len(compiled_text) != text["size"]:
        raise ValueError("Reassembled .text size changed")
    rebuilt = bytearray(original)
    rebuilt[text["offset"]:text["offset"] + text["size"]] = compiled_text
    report = json.loads((ROOT / "build/progress.json").read_text(encoding="utf-8"))
    patched = []
    objects = {}
    for function in report["functions"]:
        if not function["identical"]:
            continue
        if function.get("linked_elf"):
            with (ROOT / function["linked_elf"]).open("rb") as stream:
                linked = ELFFile(stream)
                symbol = next(s for s in linked.get_section_by_name(".symtab").iter_symbols()
                              if s.name == function["compiled_symbol"])
                section = linked.get_section(symbol["st_shndx"])
                start = symbol["st_value"] - section["sh_addr"]
                code = section.data()[start:start + symbol["st_size"]]
                relocations = []
        else:
            object_path = ROOT / function["object"]
            if object_path not in objects:
                objects[object_path] = object_functions(object_path)
            code, relocations = objects[object_path][function["compiled_symbol"]]
        if relocations or len(code) != function["expected_size"]:
            raise ValueError(f"Unresolved code in {function['name']}")
        address = int(function["address"], 0)
        section = next(s for s in sections if s["address"] <= address < s["address"] + s["size"])
        offset = section["offset"] + address - section["address"]
        validate_target_function({"name": function["name"], "address": address,
                                  "file_offset": offset, "size": len(code)}, sections)
        if code != original[offset:offset + len(code)]:
            raise ValueError(f"Compiled source differs from retail bytes: {function['name']}")
        rebuilt[offset:offset + len(code)] = code
        patched.append(function["name"])
    digest = hashlib.sha256(rebuilt).hexdigest()
    if digest != spec["executable_sha256"]:
        raise ValueError("Hybrid executable differs from the retail ELF")
    output = ROOT / "build/SLUS_216.68"
    output.write_bytes(rebuilt)
    summary = {"schema_version": 1, "kind": "hybrid_reconstruction", "output": "build/SLUS_216.68",
               "sha256": digest, "size": len(rebuilt), "byte_identical_to_original": True,
               "compiled_source_functions_substituted": patched,
               "original_material_retained": ["ELF metadata", "data sections", ".rentext", ".vutext", "VU overlays"],
               "full_source_build": False, "emulator_tested": False}
    write_json(ROOT / "build/build_report.json", summary)
    print(f"Built {output}: SHA-256 {digest} (hybrid, byte-identical to retail).")


if __name__ == "__main__":
    main()
