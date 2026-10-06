"""Strict synthetic local NOBITS linking and original-memory proof tests."""
import hashlib
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import test_link as fixtures
import link_match
import verify


def zero_mapping(address=0x348008, size=16):
    return {"address": address, "size": size, "zero_sha256": hashlib.sha256(bytes(size)).hexdigest()}


def zero_object(symbol=None, words=(0x3C020000, 0x24420000), extra_sections=()):
    image = fixtures.synthetic_object(code_words=words,
        extra_symbols=[symbol or ("local_zero", 0, 16, 0x01, 6)],
        extra_sections=[(".bss", bytes(16), 3), *extra_sections],
        relocations=[(0, 5, 2), (4, 6, 2)])
    return fixtures.section_field(image, 6, 1, 8)


class NobitsPlanningTests(unittest.TestCase):
    def inspect(self, image=None, mapping=None, readonly=None, bindings=None):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            path.write_bytes(zero_object() if image is None else image)
            return link_match.inspect_function(path, "fixture", 0x110000, bindings or {},
                mapped_sections=readonly, mapped_nobits={".bss": zero_mapping() if mapping is None else mapping})

    def test_whole_local_object_and_section_symbol_are_retained(self):
        for symbol in (("local_zero", 0, 16, 0x01, 6), ("", 0, 0, 0x03, 6), ("local_zero", 0, 0, 0x01, 6)):
            with self.subTest(symbol=symbol):
                plan = self.inspect(zero_object(symbol=symbol))
                self.assertEqual(plan["symbol_patches"], {})
                self.assertEqual(plan["mapped_sections"], [])
                self.assertEqual(plan["mapped_nobits"][0]["size"], 16)
                self.assertEqual(plan["mapped_nobits"][0]["address"], 0x348008)

    def test_nobits_is_opt_in_and_readonly_rejection_stays_strict(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            path.write_bytes(zero_object())
            with self.assertRaises(link_match.LinkError):
                link_match.inspect_function(path, "fixture", 0x110000, {})
            with self.assertRaisesRegex(link_match.LinkError, "read-only"):
                link_match.inspect_function(path, "fixture", 0x110000, {},
                    mapped_sections={".bss": fixtures.data_mapping(bytes(16))})

    def test_full_size_digest_and_strict_fields(self):
        for changed in ({"size": 8}, {"size": 0}, {"size": True}, {"size": 65537},
                        {"zero_sha256": "0" * 64}, {"zero_sha256": None},
                        {"expected_bytes": bytes(16)}, {"address": True}, {"address": 0xFFFFFFF8}):
            with self.subTest(changed=changed), self.assertRaises(link_match.LinkError):
                self.inspect(mapping={**zero_mapping(), **changed})

    def test_initialized_other_flags_alignment_and_nonzero_vma_fail(self):
        for field, value in ((1, 1), (2, 2), (2, 7), (2, 3 | 0x10000000),
                             (3, 0x348008), (8, 0), (8, 3), (8, 16), (9, 4)):
            with self.subTest(field=field, value=value), self.assertRaises(link_match.LinkError):
                self.inspect(fixtures.section_field(zero_object(), 6, field, value))

    def test_data_relocations_and_duplicate_input_names_fail(self):
        image = zero_object(extra_sections=[(".rel.bss", struct.pack("<II", 0, 2 << 8 | 2), 0)])
        for field, value in ((1, 9), (6, 3), (7, 6), (9, 8)):
            image = fixtures.section_field(image, 7, field, value)
        with self.assertRaisesRegex(link_match.LinkError, "data relocations"):
            self.inspect(image)
        with self.assertRaisesRegex(link_match.LinkError, "ambiguous"):
            self.inspect(zero_object(extra_sections=[(".bss", bytes(16), 3)]))

    def test_unsupported_symbol_type_binding_and_bounds_fail(self):
        for symbol in (("global_zero", 0, 16, 0x11, 6), ("local_zero", 16, 0, 0x01, 6),
                       ("local_zero", 8, 12, 0x01, 6), ("wrong_type", 0, 8, 0x02, 6),
                       ("", 4, 0, 0x03, 6)):
            with self.subTest(symbol=symbol), self.assertRaises(link_match.LinkError):
                self.inspect(zero_object(symbol=symbol))
        with self.assertRaisesRegex(link_match.LinkError, "conflicts"):
            self.inspect(bindings={"local_zero": 0x348010})

    def test_code_addends_cannot_leave_storage_or_be_control_flow(self):
        for words in ((0x3C020000, 0x24420010), (0x3C020000, 0x2442FFFF)):
            with self.subTest(words=words), self.assertRaises(link_match.LinkError):
                self.inspect(zero_object(words=words))
        image = zero_object()
        table = struct.unpack_from("<I", image, 32)[0]
        rel_offset = struct.unpack_from("<I", image, table + 4 * 40 + 16)[0]
        image = bytearray(image)
        struct.pack_into("<I", image, rel_offset + 4, 2 << 8 | 4)
        with self.assertRaises(link_match.LinkError):
            self.inspect(bytes(image))

    def test_unused_and_overlapping_zero_storage_fail(self):
        with self.assertRaisesRegex(link_match.LinkError, "Unused"):
            self.inspect(fixtures.section_field(zero_object(), 4, 5, 0))
        with self.assertRaisesRegex(link_match.LinkError, "overlap"):
            self.inspect(mapping=zero_mapping(0x110000))
        image = zero_object(extra_sections=[(".rodata", bytes(16), 2)])
        with self.assertRaisesRegex(link_match.LinkError, "overlap"):
            self.inspect(image, readonly={".rodata": fixtures.data_mapping(bytes(16), 0x348008)})


class OriginalNobitsProofTests(unittest.TestCase):
    def proof(self, **changed):
        return {"file_offset": 0, "size": 8, "link_bss": {".bss": {**zero_mapping(),
            "original_section": ".bss", "code_bindings": [{"hi_offset": 0, "lo_offset": 4, "relative_offset": 0}],
            **changed}}}

    def sections(self, **changed):
        return [{"name": ".bss", "address": 0x340000, "size": 0x10000,
                 "type": "SHT_NOBITS", "flags": 3, **changed}]

    def test_original_memory_bounds_and_signed_low_pointer_are_proven(self):
        original = struct.pack("<II", 0x3C020035, 0x24428008)
        self.assertEqual(verify.mapped_bss(self.proof(), original, self.sections()), {".bss": zero_mapping()})

    def test_wrong_original_storage_geometry_is_rejected(self):
        original = struct.pack("<II", 0x3C020035, 0x24428008)
        for changed in ({"name": ".other"}, {"type": "SHT_PROGBITS"}, {"flags": 2},
                        {"flags": 7}, {"address": 0x348010}, {"size": 0x8008}):
            with self.subTest(changed=changed), self.assertRaises(ValueError):
                verify.mapped_bss(self.proof(), original, self.sections(**changed))

    def test_wrong_fake_file_fields_and_initialization_proof_fail(self):
        original = struct.pack("<II", 0x3C020035, 0x24428008)
        for changed in ({"file_offset": 0}, {"zero_sha256": "0" * 64}, {"size": 0},
                        {"size": True}, {"code_bindings": []}, {"code_bindings": None}):
            with self.subTest(changed=changed), self.assertRaises(ValueError):
                verify.mapped_bss(self.proof(**changed), original, self.sections())

    def test_wrong_register_opcode_pointer_and_addend_fail(self):
        for words in ((0x3C020035, 0x24428010), (0x3C020035, 0x24628008),
                      (0x3C020035, 0x34428008), (0x24020035, 0x24428008),
                      (0x3C020035, 0x24408008)):
            with self.subTest(words=words), self.assertRaises(ValueError):
                verify.mapped_bss(self.proof(), struct.pack("<II", *words), self.sections())
        original = struct.pack("<II", 0x3C020035, 0x24428008)
        for pointer in ({"hi_offset": 0, "lo_offset": 8, "relative_offset": 0},
                        {"hi_offset": 0, "lo_offset": 4, "relative_offset": 16},
                        {"hi_offset": 0, "lo_offset": 4, "relative_offset": True}):
            with self.subTest(pointer=pointer), self.assertRaises(ValueError):
                verify.mapped_bss(self.proof(code_bindings=[pointer]), original, self.sections())

    def test_only_nonclobbering_nop_branch_or_return_can_separate_hi_low(self):
        pointer = {"hi_offset": 0, "lo_offset": 8, "relative_offset": 0}
        proof = self.proof(code_bindings=[pointer])
        proof["size"] = 12
        for middle in (0, 0x03E00008, 0x10000004):
            with self.subTest(middle=middle):
                verify.mapped_bss(proof, struct.pack("<III", 0x3C020035, middle, 0x24428008), self.sections())
        for middle in (0x24020000, 0x00401021, 0x0C100000):
            with self.subTest(middle=middle), self.assertRaisesRegex(ValueError, "register write"):
                verify.mapped_bss(proof, struct.pack("<III", 0x3C020035, middle, 0x24428008), self.sections())


class ActualNobitsLinkTests(unittest.TestCase):
    @unittest.skipUnless((link_match.DEFAULT_BINUTILS / "mips-ps2-decompals-ld.exe").is_file(), "pinned linker unavailable")
    def test_actual_gnu_link_retains_noload_without_input_instruction_changes(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            original = zero_object()
            path.write_bytes(original)
            linked = link_match.link_function(path, "fixture", 0x110000, {}, Path(folder) / "linked",
                mapped_nobits={".bss": zero_mapping()})
            self.assertEqual(linked["code"], struct.pack("<II", 0x3C020035, 0x24428008))
            self.assertEqual(linked["remaining_relocations"], [])
            self.assertEqual(linked["mapped_nobits"][0]["zero_sha256"], zero_mapping()["zero_sha256"])
            self.assertIn("(NOLOAD)", linked["script_path"].read_text())
            self.assertEqual(path.read_bytes(), original)
            self.assertEqual(linked["object_path"].read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
