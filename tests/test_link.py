"""Relocation-aware linking tests contain only synthetic instructions and ELF."""

import hashlib
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS))
import link_match  # noqa: E402


def synthetic_object(*, target_section=".text.fixture", target_value=0, target_size=8,
                     extra_symbols=(), extra_sections=(), relocations=(), flags=0x20923001,
                     code_words=(0x03E00008, 0)):
    """Extra symbols are (name, value, size, info, shndx), starting at index 2."""
    code = struct.pack("<" + "I" * len(code_words), *code_words)
    section_names = ["", target_section, ".strtab", ".symtab", ".rel.text", ".shstrtab"]
    section_names.extend(name for name, _, _ in extra_sections)
    name_data = b"\x00"
    name_offsets = [0]
    for name in section_names[1:]:
        name_offsets.append(len(name_data))
        name_data += name.encode("ascii") + b"\x00"
    string_data = b"\x00"
    symbol_data = bytearray(16)
    for name, value, size, info, index in [("fixture", target_value, target_size, 0x12, 1), *extra_symbols]:
        name_offset = len(string_data)
        string_data += name.encode("ascii") + b"\x00"
        symbol_data.extend(struct.pack("<IIIBBH", name_offset, value, size, info, 0, index))
    relocation_data = b"".join(struct.pack("<II", offset, (index << 8) | kind)
                               for offset, kind, index in relocations)
    image = bytearray(52)
    sections = [(0,) * 10]
    specifications = [(1, 6, code, 0, 0, 8, 0), (3, 0, string_data, 0, 0, 1, 0),
                      (2, 0, symbol_data, 2, 1, 4, 16), (9, 0, relocation_data, 3, 1, 4, 8),
                      (3, 0, name_data, 0, 0, 1, 0)]
    specifications.extend((1, section_flags, data, 0, 0, 4, 0) for _, data, section_flags in extra_sections)
    for name_offset, specification in zip(name_offsets[1:], specifications):
        kind, section_flags, data, link, info, alignment, entry_size = specification
        image.extend(b"\x00" * (-len(image) % alignment))
        offset = len(image)
        image.extend(data)
        sections.append((name_offset, kind, section_flags, 0, offset, len(data), link, info, alignment, entry_size))
    image.extend(b"\x00" * (-len(image) % 4))
    section_offset = len(image)
    image.extend(b"".join(struct.pack("<10I", *section) for section in sections))
    ident = b"\x7fELF\x01\x01\x01" + b"\x00" * 9
    image[:52] = struct.pack("<16sHHIIIIIHHHHHH", ident, 1, 8, 1, 0, 0, section_offset, flags,
                             52, 0, 0, 40, len(sections), 5)
    return bytes(image)


def data_mapping(data=b"\x00\x00\x80\x3F", address=0x348008):
    return {"address": address, "expected_bytes": data,
            "expected_sha256": hashlib.sha256(data).hexdigest()}


def section_field(image, index, field, value):
    result = bytearray(image)
    table = struct.unpack_from("<I", image, 32)[0]
    struct.pack_into("<I", result, table + 40 * index + 4 * field, value)
    return bytes(result)


class PlanningTests(unittest.TestCase):
    def inspect(self, image=None, bindings=None, address=0x110000, symbol="fixture", mapped_sections=None):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            path.write_bytes(image if image is not None else synthetic_object())
            return link_match.inspect_function(path, symbol, address, {} if bindings is None else bindings,
                                               mapped_sections=mapped_sections)

    def test_dedicated_target_plan(self):
        plan = self.inspect()
        self.assertEqual((plan["address"], plan["size"], plan["section"]), (0x110000, 8, ".text.fixture"))
        self.assertEqual(plan["symbol_patches"], {})

    def test_shared_section_and_invalid_symbol_bounds_rejected(self):
        for image in (synthetic_object(target_section=".text"), synthetic_object(target_size=9),
                      synthetic_object(target_value=4)):
            with self.subTest(image=image), self.assertRaises(link_match.LinkError):
                self.inspect(image)

    def test_alignment_flags_and_invalid_binding_rejected(self):
        for image, bindings, address in ((synthetic_object(flags=0x20923003), {}, 0x110000),
                                          (synthetic_object(), {}, 0x110004),
                                          (synthetic_object(), {"fixture": 0x110008}, 0x110000),
                                          (synthetic_object(), {"_gp": 0}, 0x110000),
                                          (synthetic_object(), {"injected;name": 0}, 0x110000),
                                          (synthetic_object(), {"global": True}, 0x110000)):
            with self.subTest(bindings=bindings, address=address), self.assertRaises(link_match.LinkError):
                self.inspect(image, bindings, address)

    def test_unknown_undefined_and_duplicate_symbols_rejected(self):
        undefined = ("callee", 0, 0, 0x10, 0)
        for extras, bindings in (([undefined], {}), ([undefined, undefined], {"callee": 0x120000})):
            image = synthetic_object(extra_symbols=extras, relocations=[(0, 4, 2)])
            with self.subTest(extras=extras), self.assertRaises(link_match.LinkError):
                self.inspect(image, bindings)

    def test_defined_internal_function_maps_to_explicit_address(self):
        image = synthetic_object(extra_symbols=[("callee", 0, 8, 0x12, 6)],
                                 extra_sections=[(".text.callee", struct.pack("<2I", 0x03E00008, 0), 6)],
                                 relocations=[(0, 4, 2)])
        plan = self.inspect(image, {"callee": 0x120000})
        self.assertEqual(plan["symbol_patches"], {2: 0x120000})
        self.assertEqual(plan["resolved_bindings"], {"callee": 0x120000})

    def test_compiler_local_constants_rejected_even_with_name_binding(self):
        image = synthetic_object(extra_symbols=[(".LC0", 0, 4, 0x01, 6)],
                                 extra_sections=[(".rodata", struct.pack("<f", 1.0), 2)],
                                 relocations=[(0, 5, 2)])
        with self.assertRaisesRegex(link_match.LinkError, "compiler-local"):
            self.inspect(image, {".LC0": 0x200000})

    def test_unsupported_and_outside_relocations_rejected(self):
        for offset, kind in ((0, 9), (8, 4), (2, 4)):
            image = synthetic_object(extra_symbols=[("callee", 0, 0, 0x10, 0)],
                                     relocations=[(offset, kind, 2)])
            with self.subTest(offset=offset, kind=kind), self.assertRaises(link_match.LinkError):
                self.inspect(image, {"callee": 0x120000})

    def test_conflicting_absolute_binding_rejected(self):
        image = synthetic_object(extra_symbols=[("global", 0x200000, 0, 0x10, 0xFFF1)],
                                 relocations=[(0, 5, 2)])
        with self.assertRaisesRegex(link_match.LinkError, "conflicts"):
            self.inspect(image, {"global": 0x210000})

    def mapped_object(self, *, flags=2, sections=None, symbols=None, relocations=None):
        return synthetic_object(
            code_words=(0x3C020000, 0x24420000),
            extra_symbols=symbols if symbols is not None else [(".LC0", 0, 4, 0x01, 6)],
            extra_sections=sections if sections is not None else [(".rodata", b"\x00\x00\x80\x3F", flags)],
            relocations=relocations if relocations is not None else [(0, 5, 2), (4, 6, 2)])

    def test_proven_readonly_local_and_section_symbols_are_retained(self):
        for symbol in ((".LC0", 0, 4, 0x01, 6), ("", 0, 0, 0x03, 6)):
            plan = self.inspect(self.mapped_object(symbols=[symbol]), mapped_sections={".rodata": data_mapping()})
            self.assertEqual(plan["symbol_patches"], {})
            self.assertEqual(plan["mapped_sections"][0]["data"], b"\x00\x00\x80\x3F")
            self.assertEqual(plan["mapped_sections"][0]["address"], 0x348008)

    def test_complete_readonly_byte_and_hash_proofs_are_required(self):
        valid = data_mapping()
        for mapping in ({"address": valid["address"], "expected_sha256": valid["expected_sha256"]},
                        {**valid, "expected_sha256": "0" * 64}, data_mapping(b"\x01\x00\x80\x3F"),
                        {**valid, "expected_bytes": bytearray(valid["expected_bytes"])},
                        {**valid, "unexpected": True}):
            with self.subTest(mapping=mapping), self.assertRaises(link_match.LinkError):
                self.inspect(self.mapped_object(), mapped_sections={".rodata": mapping})

    def test_readonly_mapping_flags_alignment_and_address_space_are_strict(self):
        for flags in (3, 6, 0, 0x402):
            with self.subTest(flags=flags), self.assertRaisesRegex(link_match.LinkError, "read-only"):
                self.inspect(self.mapped_object(flags=flags), mapped_sections={".rodata": data_mapping()})
        for address in (0x348009, 0xFFFFFFFF, True):
            with self.subTest(address=address), self.assertRaises(link_match.LinkError):
                self.inspect(self.mapped_object(), mapped_sections={".rodata": data_mapping(address=address)})

    def test_missing_ambiguous_and_unused_readonly_mappings_fail(self):
        with self.assertRaisesRegex(link_match.LinkError, "Missing or ambiguous"):
            self.inspect(self.mapped_object(), mapped_sections={".rodata.missing": data_mapping()})
        sections = [(".rodata", b"\x00\x00\x80\x3F", 2)] * 2
        with self.assertRaisesRegex(link_match.LinkError, "Missing or ambiguous"):
            self.inspect(self.mapped_object(sections=sections), mapped_sections={".rodata": data_mapping()})
        with self.assertRaisesRegex(link_match.LinkError, "Unused"):
            self.inspect(self.mapped_object(relocations=[]), mapped_sections={".rodata": data_mapping()})

    def test_code_data_and_data_data_overlaps_fail(self):
        with self.assertRaisesRegex(link_match.LinkError, "overlap"):
            self.inspect(self.mapped_object(), mapped_sections={".rodata": data_mapping(address=0x110000)})
        image = self.mapped_object(sections=[(".rodata", b"\x00\x00\x80\x3F", 2),
                                            (".rodata.other", b"\x00\x00\x80\x3F", 2)])
        with self.assertRaisesRegex(link_match.LinkError, "overlap"):
            self.inspect(image, mapped_sections={".rodata": data_mapping(), ".rodata.other": data_mapping()})

    def test_readonly_symbol_bounds_and_conflicting_named_bindings_fail(self):
        for symbol in ((".LC0", 4, 0, 0x01, 6), (".LC0", 0, 8, 0x01, 6),
                       (".LC0", 0, 4, 0x02, 6), ("", 1, 0, 0x03, 6)):
            with self.subTest(symbol=symbol), self.assertRaisesRegex(link_match.LinkError, "bounds/type"):
                self.inspect(self.mapped_object(symbols=[symbol]), mapped_sections={".rodata": data_mapping()})
        with self.assertRaisesRegex(link_match.LinkError, "conflicts"):
            self.inspect(self.mapped_object(), bindings={".LC0": 0x34800C},
                         mapped_sections={".rodata": data_mapping()})

    def test_relocation_bearing_readonly_data_is_rejected_before_link(self):
        image = section_field(self.mapped_object(), 4, 7, 6)  # .rel.text now relocates .rodata.
        with self.assertRaisesRegex(link_match.LinkError, "Relocation-bearing"):
            self.inspect(image, mapped_sections={".rodata": data_mapping()})

    def test_merge_section_geometry_is_checked(self):
        for image in (self.mapped_object(flags=0x22), self.mapped_object(flags=0x12),
                      section_field(self.mapped_object(flags=0x12), 6, 9, 3)):
            with self.subTest(image=image), self.assertRaises(link_match.LinkError):
                self.inspect(image, mapped_sections={".rodata": data_mapping()})

    def test_section_base_addends_cannot_escape_proven_readonly_data(self):
        image = synthetic_object(code_words=(0x3C020000, 0x24420004),
                                 extra_symbols=[("", 0, 0, 0x03, 6)],
                                 extra_sections=[(".rodata", b"\x00\x00\x80\x3F", 2)],
                                 relocations=[(0, 5, 2), (4, 6, 2)])
        with self.assertRaisesRegex(link_match.LinkError, "outside proven"):
            self.inspect(image, mapped_sections={".rodata": data_mapping()})
        with self.assertRaisesRegex(link_match.LinkError, "Unpaired HI16"):
            self.inspect(self.mapped_object(relocations=[(0, 5, 2)]),
                         mapped_sections={".rodata": data_mapping()})


class RealLinkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.assembler = link_match.DEFAULT_BINUTILS / "mips-ps2-decompals-as.exe"
        if not cls.assembler.is_file():
            raise unittest.SkipTest("Pinned PS2 GNU assembler is unavailable")
        try:
            link_match._linker(link_match.DEFAULT_BINUTILS)
        except link_match.LinkError as error:
            raise unittest.SkipTest(str(error))

    def link(self, body, bindings, extra="", mapped_sections=None, return_result=False):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            source = root / "fixture.s"
            object_path = root / "fixture.o"
            source.write_text('.set noreorder\n.section .text.fixture,"ax",@progbits\n.align 3\n'
                              '.globl fixture\n.ent fixture\nfixture:\n' + body +
                              '\n.end fixture\n.size fixture, .-fixture\n' + extra, encoding="ascii")
            command = [str(self.assembler), "-EL", "-march=r5900", "-mabi=eabi", "-G0", "-no-pad-sections",
                       "-o", str(object_path), str(source)]
            process = subprocess.run(command, capture_output=True, text=True)
            self.assertEqual(process.returncode, 0, process.stderr)
            original = object_path.read_bytes()
            result = link_match.link_function(object_path, "fixture", 0x110000, bindings, root / "linked",
                                              mapped_sections=mapped_sections)
            self.assertEqual(object_path.read_bytes(), original)
            self.assertEqual(result["remaining_relocations"], [])
            self.assertTrue(result["linked_path"].is_file())
            return result if return_result else result["code"]

    def test_absolute_call_and_discarded_internal_callee(self):
        body = "jal callee\nnop\njr $ra\nnop"
        extra = '.section .text.callee,"ax",@progbits\n.align 3\n.globl callee\n.ent callee\ncallee:\njr $ra\nnop\n.end callee\n'
        code = self.link(body, {"callee": 0x120000}, extra)
        self.assertEqual(code, struct.pack("<4I", 0x0C000000 | (0x120000 >> 2), 0, 0x03E00008, 0))

    def test_hi16_lo16_signed_carry(self):
        code = self.link("lui $v0,%hi(global)\naddiu $v0,$v0,%lo(global)\njr $ra\nnop", {"global": 0x00348008})
        self.assertEqual(code, struct.pack("<4I", 0x3C020035, 0x24428008, 0x03E00008, 0))

    def test_local_internal_callee(self):
        extra = '.section .text.callee,"ax",@progbits\n.align 3\n.ent callee\ncallee:\njr $ra\nnop\n.end callee\n'
        code = self.link("jal callee\nnop\njr $ra\nnop", {"callee": 0x120000}, extra)
        self.assertEqual(code, struct.pack("<4I", 0x0C000000 | (0x120000 >> 2), 0, 0x03E00008, 0))

    def test_internal_section_symbol(self):
        extra = '.section .text.callee,"ax",@progbits\n.align 3\n.ent callee\ncallee:\njr $ra\nnop\n.end callee\n'
        code = self.link("jal .text.callee\nnop\njr $ra\nnop", {"callee": 0x120000}, extra)
        self.assertEqual(code, struct.pack("<4I", 0x0C000000 | (0x120000 >> 2), 0, 0x03E00008, 0))

    def test_gp_relative_resolution(self):
        code = self.link("lw $v0,%gp_rel(global)($gp)\njr $ra\nnop", {"global": link_match.GP + 0x7FF0})
        self.assertEqual(code, struct.pack("<3I", 0x8F827FF0, 0x03E00008, 0))

    def test_gp_relative_overflow_is_an_error(self):
        with self.assertRaisesRegex(link_match.LinkError, "link failed"):
            self.link("lw $v0,%gp_rel(global)($gp)\njr $ra\nnop", {"global": link_match.GP + 0x8000})

    def test_unbound_call_fails(self):
        with self.assertRaisesRegex(link_match.LinkError, "Unknown undefined"):
            self.link("jal unknown\nnop\njr $ra\nnop", {})

    def test_fixed_readonly_constant_relocation_and_independent_output(self):
        extra = '.section .rodata.literal,"a",@progbits\n.align 2\n.LC0:\n.word 0x3f800000\n'
        result = self.link("lui $v0,%hi(.LC0)\nlw $v0,%lo(.LC0)($v0)\njr $ra\nnop", {}, extra,
                           mapped_sections={".rodata.literal": data_mapping()}, return_result=True)
        self.assertEqual(result["code"], struct.pack("<4I", 0x3C020035, 0x8C428008, 0x03E00008, 0))
        mapping = result["mapped_sections"][0]
        self.assertEqual((mapping["address"], mapping["size"], mapping["data"]),
                         (0x348008, 4, b"\x00\x00\x80\x3F"))
        self.assertEqual(mapping["sha256"], data_mapping()["expected_sha256"])

    def test_readonly_section_symbol_keeps_nonzero_addend(self):
        data = struct.pack("<2I", 0x3F800000, 0x40000000)
        extra = '.section .rodata.literal,"a",@progbits\n.align 2\n.word 0x3f800000,0x40000000\n'
        code = self.link("lui $v0,%hi(.rodata.literal+4)\nlw $v0,%lo(.rodata.literal+4)($v0)\njr $ra\nnop", {}, extra,
                         mapped_sections={".rodata.literal": data_mapping(data)})
        self.assertEqual(code, struct.pack("<4I", 0x3C020035, 0x8C42800C, 0x03E00008, 0))

    def test_readonly_merge_string_bytes_must_survive_unchanged(self):
        data = b"synthetic\0"
        extra = '.section .rodata.str1.1,"aMS",@progbits,1\n.align 0\n.LC0:\n.asciz "synthetic"\n'
        result = self.link("lui $v0,%hi(.LC0)\naddiu $v0,$v0,%lo(.LC0)\njr $ra\nnop", {}, extra,
                           mapped_sections={".rodata.str1.1": data_mapping(data)}, return_result=True)
        self.assertEqual(result["mapped_sections"][0]["data"], data)

    def test_mapped_readonly_relocations_cannot_be_hidden(self):
        extra = '.section .rodata.literal,"a",@progbits\n.align 2\n.LC0:\n.word unknown\n'
        with self.assertRaisesRegex(link_match.LinkError, "Relocation-bearing"):
            self.link("lui $v0,%hi(.LC0)\naddiu $v0,$v0,%lo(.LC0)\njr $ra\nnop", {}, extra,
                      mapped_sections={".rodata.literal": data_mapping(bytes(4))})

    def test_readonly_section_addend_cannot_escape_fixed_proof(self):
        extra = '.section .rodata.literal,"a",@progbits\n.align 2\n.word 0x3f800000\n'
        with self.assertRaisesRegex(link_match.LinkError, "outside proven"):
            self.link("lui $v0,%hi(.rodata.literal+4)\nlw $v0,%lo(.rodata.literal+4)($v0)\njr $ra\nnop", {}, extra,
                      mapped_sections={".rodata.literal": data_mapping()})


if __name__ == "__main__":
    unittest.main()
