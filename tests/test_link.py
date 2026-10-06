"""Relocation-aware linking tests contain only synthetic instructions and ELF."""

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
                     extra_symbols=(), extra_sections=(), relocations=(), flags=0x20923001):
    """Extra symbols are (name, value, size, info, shndx), starting at index 2."""
    code = struct.pack("<2I", 0x03E00008, 0)
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


class PlanningTests(unittest.TestCase):
    def inspect(self, image=None, bindings=None, address=0x110000, symbol="fixture"):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            path.write_bytes(image if image is not None else synthetic_object())
            return link_match.inspect_function(path, symbol, address, {} if bindings is None else bindings)

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

    def link(self, body, bindings, extra=""):
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
            result = link_match.link_function(object_path, "fixture", 0x110000, bindings, root / "linked")
            self.assertEqual(object_path.read_bytes(), original)
            self.assertEqual(result["remaining_relocations"], [])
            self.assertTrue(result["linked_path"].is_file())
            return result["code"]

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


if __name__ == "__main__":
    unittest.main()
