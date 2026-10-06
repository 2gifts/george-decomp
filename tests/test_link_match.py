"""Synthetic readonly self-pointer fixtures; no original game data or code."""
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


def pointer_mapping(data=None, address=0x348008):
    if data is None:
        data = struct.pack("<4I", address + 8, address + 8, 0x12345678, 0x76543210)
    return {**fixtures.data_mapping(data, address), "relocation_policy": "self_r_mips_32"}


def pointer_object(relocations=None, symbols=None, data=None, extra_sections=()):
    if data is None:
        data = struct.pack("<4I", 8, 0xFFFFFFFC, 0x12345678, 0x76543210)
    if relocations is None:
        relocations = [(0, 2, 2), (4, 2, 3)]
    table = b"".join(struct.pack("<II", offset, index << 8 | kind) for offset, kind, index in relocations)
    image = fixtures.synthetic_object(
        code_words=(0x3C020000, 0x24420000), relocations=[(0, 5, 2), (4, 6, 2)],
        extra_symbols=symbols if symbols is not None else [("", 0, 0, 3, 6), ("local_data", 12, 4, 1, 6)],
        extra_sections=[(".rodata", data, 2), (".rel.rodata", table, 0), *extra_sections])
    for field, value in ((1, 9), (6, 3), (7, 6), (9, 8)):
        image = fixtures.section_field(image, 7, field, value)
    return image


class SelfPointerPlanTests(unittest.TestCase):
    def inspect(self, image=None, mapping=None, bindings=None):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            path.write_bytes(pointer_object() if image is None else image)
            return link_match.inspect_function(path, "fixture", 0x110000, bindings or {},
                                               mapped_sections={".rodata": pointer_mapping() if mapping is None else mapping})

    def test_self_section_and_named_negative_addend_are_retained_unmodified(self):
        plan = self.inspect()
        mapped = plan["mapped_sections"][0]
        self.assertEqual(plan["symbol_patches"], {})
        self.assertNotEqual(mapped["data"], mapped["expected_bytes"])
        self.assertEqual([item["relative_offset"] for item in mapped["input_relocations"]], [8, 8])
        self.assertEqual([item["addend"] for item in mapped["input_relocations"]], [8, -4])
        self.assertEqual(plan["resolved_bindings"]["local_data"], 0x348014)
        self.assertEqual(mapped["input_sha256"], hashlib.sha256(mapped["data"]).hexdigest())

    def test_old_no_relocation_policy_stays_strict(self):
        source = struct.pack("<4I", 8, 0xFFFFFFFC, 0x12345678, 0x76543210)
        with self.assertRaisesRegex(link_match.LinkError, "Relocation-bearing"):
            self.inspect(mapping=fixtures.data_mapping(source))
        with self.assertRaisesRegex(link_match.LinkError, "bytes differ"):
            self.inspect(mapping=fixtures.data_mapping(pointer_mapping()["expected_bytes"]))

    def test_policy_must_be_explicit_supported_and_nonempty(self):
        for policy in (None, True, "none", "external_r_mips_32"):
            with self.subTest(policy=policy), self.assertRaisesRegex(link_match.LinkError, "policy"):
                self.inspect(mapping={**pointer_mapping(), "relocation_policy": policy})
        with self.assertRaisesRegex(link_match.LinkError, "nonempty"):
            self.inspect(pointer_object(relocations=[]))

    def test_only_aligned_bounded_unique_r_mips_32_entries_are_accepted(self):
        for relocations in ([(0, 0, 2)], [(0, 5, 2)], [(2, 2, 2)], [(16, 2, 2)],
                            [(0, 2, 999)], [(0, 2, 2), (0, 2, 2)]):
            with self.subTest(relocations=relocations), self.assertRaises(link_match.LinkError):
                self.inspect(pointer_object(relocations=relocations))

    def test_rel_table_association_and_geometry_are_checked(self):
        for field, value in ((1, 4), (6, 2), (9, 0), (9, 12), (5, 15)):
            with self.subTest(field=field, value=value), self.assertRaises(link_match.LinkError):
                self.inspect(fixtures.section_field(pointer_object(), 7, field, value))

    def test_multiple_nonempty_data_tables_and_unused_self_mapping_fail(self):
        second = struct.pack("<II", 8, 2 << 8 | 2)
        image = pointer_object(extra_sections=[(".rel.rodata.extra", second, 0)])
        for field, value in ((1, 9), (6, 3), (7, 6), (9, 8)):
            image = fixtures.section_field(image, 8, field, value)
        with self.assertRaisesRegex(link_match.LinkError, "one nonempty"):
            self.inspect(image)
        # Internal self-pointers do not make an otherwise unused mapping live.
        with self.assertRaisesRegex(link_match.LinkError, "Unused"):
            self.inspect(fixtures.section_field(pointer_object(), 4, 5, 0))

    def test_unknown_absolute_code_and_cross_section_targets_are_rejected(self):
        for symbol in (("external", 0, 0, 0x10, 0), ("absolute", 0x348008, 0, 0x10, 0xFFF1),
                       ("fixture_data", 0, 0, 0x11, 1), ("wrong_type", 12, 4, 2, 6),
                       ("cross", 0, 4, 1, 8)):
            symbols = [("", 0, 0, 3, 6), symbol]
            with self.subTest(symbol=symbol), self.assertRaisesRegex(link_match.LinkError, "same mapped section"):
                self.inspect(pointer_object(symbols=symbols, extra_sections=[(".rodata.other", bytes(4), 2)]),
                             bindings={symbol[0]: 0x348008})

    def test_addends_and_symbol_bounds_cannot_escape_section(self):
        for data in (struct.pack("<4I", 16, 0, 0, 0), struct.pack("<4I", 0xFFFFFFFF, 0, 0, 0),
                     struct.pack("<4I", 8, 8, 0, 0)):
            with self.subTest(data=data), self.assertRaisesRegex(link_match.LinkError, "addend"):
                self.inspect(pointer_object(data=data))
        for symbol in (("local_data", 16, 0, 1, 6), ("local_data", 12, 8, 1, 6), ("", 4, 0, 3, 6)):
            with self.subTest(symbol=symbol), self.assertRaises(link_match.LinkError):
                self.inspect(pointer_object(symbols=[("", 0, 0, 3, 6), symbol]))

    def test_data_symbol_binding_conflicts_and_ambiguity_are_rejected(self):
        with self.assertRaisesRegex(link_match.LinkError, "conflicts"):
            self.inspect(bindings={"local_data": 0x348018})
        symbols = [("", 0, 0, 3, 6), ("local_data", 12, 4, 1, 6), ("local_data", 12, 4, 1, 6)]
        with self.assertRaisesRegex(link_match.LinkError, "Ambiguous"):
            self.inspect(pointer_object(symbols=symbols))

    def test_writable_data_stays_rejected_with_optin(self):
        with self.assertRaisesRegex(link_match.LinkError, "read-only"):
            self.inspect(fixtures.section_field(pointer_object(), 6, 2, 3))


class SelfPointerLinkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fixtures.RealLinkTests.setUpClass.__func__(cls)

    link = fixtures.RealLinkTests.link
    BODY = "lui $v0,%hi(.rodata.ptr)\naddiu $v0,$v0,%lo(.rodata.ptr)\njr $ra\nnop"
    EXTRA = '.section .rodata.ptr,"a",@progbits\n.align 2\n.word .rodata.ptr+8,.rodata.ptr+8,0x12345678,0x76543210\n'

    def test_actual_gnu_link_resolves_all_self_pointers_and_returns_full_data(self):
        proof = pointer_mapping()
        result = self.link(self.BODY, {}, self.EXTRA, mapped_sections={".rodata.ptr": proof}, return_result=True)
        self.assertEqual(result["code"], struct.pack("<4I", 0x3C020035, 0x24428008, 0x03E00008, 0))
        mapping = result["mapped_sections"][0]
        self.assertEqual(mapping["data"], proof["expected_bytes"])
        self.assertEqual(mapping["sha256"], proof["expected_sha256"])
        self.assertEqual(len(mapping["input_relocations"]), 2)
        self.assertEqual(mapping["relocation_policy"], "self_r_mips_32")

    def test_entire_final_data_mismatch_fails_without_masking(self):
        expected = bytearray(pointer_mapping()["expected_bytes"])
        for position in (0, 8, 15):
            changed = bytearray(expected); changed[position] ^= 1
            with self.subTest(position=position), self.assertRaisesRegex(link_match.LinkError, "differs from fixed original proof"):
                self.link(self.BODY, {}, self.EXTRA, mapped_sections={".rodata.ptr": pointer_mapping(bytes(changed))})

    def test_unresolved_raw_input_cannot_be_accepted_as_expected_output(self):
        source = struct.pack("<4I", 8, 8, 0x12345678, 0x76543210)
        with self.assertRaisesRegex(link_match.LinkError, "differs from fixed original proof"):
            self.link(self.BODY, {}, self.EXTRA, mapped_sections={".rodata.ptr": pointer_mapping(source)})

    def test_even_explicit_external_binding_cannot_relax_self_policy(self):
        extra = '.section .rodata.ptr,"a",@progbits\n.align 2\n.word external\n'
        with self.assertRaisesRegex(link_match.LinkError, "same mapped section"):
            self.link(self.BODY, {"external": 0x348008}, extra,
                      mapped_sections={".rodata.ptr": pointer_mapping(struct.pack("<I", 0x348008))})


class VerifierSelfPointerProofTests(unittest.TestCase):
    def proof(self, **fields):
        return {"link_data": {".rodata": {"address": "0x4000", "file_offset": 8, "size": 4,
                "original_sha256": hashlib.sha256(b"data").hexdigest(), **fields}}}

    def test_policy_is_forwarded_only_after_original_geometry_and_hash_proof(self):
        sections = [{"offset": 8, "address": 0x4000, "size": 4}]
        result = verify.mapped_data(self.proof(relocation_policy="self_r_mips_32", evidence="synthetic"), b"prefix!!data", sections)
        self.assertEqual(result[".rodata"]["relocation_policy"], "self_r_mips_32")
        self.assertEqual(result[".rodata"]["expected_bytes"], b"data")
        for changed in ({"address": "0x4004"}, {"size": 5}, {"original_sha256": "0" * 64}):
            with self.subTest(changed=changed), self.assertRaises(ValueError):
                verify.mapped_data(self.proof(relocation_policy="self_r_mips_32", **changed), b"prefix!!data", sections)

    def test_unknown_schema_policy_and_noninteger_geometry_are_rejected(self):
        sections = [{"offset": 8, "address": 0x4000, "size": 4}]
        for fields in ({"unexpected": True}, {"relocation_policy": None}, {"relocation_policy": "external"},
                       {"address": True}, {"size": 4.0}, {"file_offset": 8.0}):
            with self.subTest(fields=fields), self.assertRaises(ValueError):
                verify.mapped_data(self.proof(**fields), b"prefix!!data", sections)


if __name__ == "__main__":
    unittest.main()
