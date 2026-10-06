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

    def word_proof(self, access="lw32", distance=4, relative=0, preserved=None):
        pointer = {"hi_offset": 0, "lo_offset": distance, "relative_offset": relative}
        if access is not None:
            pointer["access"] = access
        if preserved is not None:
            pointer["preserved_sequence"] = preserved
        proof = self.proof(code_bindings=[pointer])
        proof["size"] = distance + 4
        return proof

    def test_declared_word_accesses_use_exact_signed_low_effective_address(self):
        for access, lower in (("lw32", 0x8C438008), ("sw32", 0xAC448008),
                              ("sw32", 0xAC408008)):
            with self.subTest(access=access, lower=lower):
                self.assertEqual(verify.mapped_bss(self.word_proof(access),
                    struct.pack("<II", 0x3C020035, lower), self.sections()),
                    {".bss": zero_mapping()})
        # The final complete four-byte word is valid, but an interior pointer
        # to the last byte is not sufficient evidence for a word load/store.
        self.assertEqual(verify.mapped_bss(self.word_proof(relative=12),
            struct.pack("<II", 0x3C020035, 0x8C438014), self.sections()),
            {".bss": zero_mapping()})

    def test_word_access_requires_opt_in_opcode_register_and_full_extent(self):
        for lower in (0x24438008, 0x84438008, 0xDC438008, 0xAC438008,
                      0x8C638008, 0x8C408008, 0x8C428008, 0x8C438010):
            with self.subTest(lower=hex(lower)), self.assertRaises(ValueError):
                verify.mapped_bss(self.word_proof(), struct.pack("<II", 0x3C020035, lower), self.sections())
        for relative in (1, 13, 14, 15):
            with self.subTest(relative=relative), self.assertRaisesRegex(ValueError, "unaligned|whole storage"):
                verify.mapped_bss(self.word_proof(relative=relative),
                    struct.pack("<II", 0x3C020035, 0x8C438008 + relative), self.sections())
        for size in (1, 2, 3):
            proof=self.word_proof();proof["link_bss"][".bss"].update(zero_mapping(size=size))
            with self.subTest(size=size), self.assertRaisesRegex(ValueError, "whole storage"):
                verify.mapped_bss(proof, struct.pack("<II", 0x3C020035, 0x8C438008), self.sections())
        with self.assertRaisesRegex(ValueError, "declared same-base"):
            verify.mapped_bss(self.proof(), struct.pack("<II", 0x3C020035, 0x8C438008), self.sections())

    def test_preserved_sequence_is_explicit_and_literal_true_only(self):
        original=struct.pack("<III", 0x3C020035, 0, 0x8C438008)
        with self.assertRaises(ValueError):
            verify.mapped_bss(self.word_proof(distance=8), original, self.sections())
        self.assertEqual(verify.mapped_bss(self.word_proof(distance=8, preserved=True),
            original, self.sections()), {".bss": zero_mapping()})
        for value in (False, 0, 1, "true", None, [], {}):
            proof=self.word_proof(distance=8, preserved=True)
            proof["link_bss"][".bss"]["code_bindings"][0]["preserved_sequence"]=value
            with self.subTest(value=value), self.assertRaisesRegex(ValueError, "literal true"):
                verify.mapped_bss(proof, original, self.sections())
        for value in ("pointer", "lw", "sd64", False, 1, None):
            proof=self.word_proof();proof["link_bss"][".bss"]["code_bindings"][0]["access"]=value
            with self.subTest(value=value), self.assertRaisesRegex(ValueError, "access proof"):
                verify.mapped_bss(proof, struct.pack("<II", 0x3C020035, 0x8C438008), self.sections())
        proof=self.word_proof();proof["link_bss"][".bss"]["code_bindings"][0]["unknown"]=True
        with self.assertRaisesRegex(ValueError, "Unsupported original"):
            verify.mapped_bss(proof, struct.pack("<II", 0x3C020035, 0x8C438008), self.sections())

    def test_preserved_whitelist_matches_real_frame_global_sequences(self):
        # Synthetic addresses/words, rather than game code or data fixtures.
        for middle in (0, 0xFFA40010, 0xACA40008, 0x27BDFFE0, 0x8C438008):
            for access, lower in (("lw32", 0x8C508008), ("sw32", 0xAC448008),
                                  (None, 0x24458008)):
                with self.subTest(middle=hex(middle), access=access):
                    proof=self.word_proof(access, distance=12, preserved=True)
                    self.assertEqual(verify.mapped_bss(proof,
                        struct.pack("<IIII", 0x3C020035, middle, middle, lower), self.sections()),
                        {".bss": zero_mapping()})
        # LUI, stack adjustment, load into a different register, ADDIU pointer.
        proof=self.word_proof(None, distance=12, preserved=True)
        self.assertEqual(verify.mapped_bss(proof,
            struct.pack("<IIII", 0x3C020035, 0x27BDFFE0, 0x8C438008, 0x24458008),
            self.sections()), {".bss": zero_mapping()})

    def test_preserved_whitelist_rejects_clobbers_calls_control_and_unknowns(self):
        for middle in (0x24420001, 0x24430001, 0x8C428008, 0x8C408008,
                       0xDC438008, 0x00401021, 0x0C100000, 0x08100000,
                       0x10000004, 0x03E00008, 0x0080F809, 0x40026000,
                       0x70000000, 0x00021000):
            with self.subTest(middle=hex(middle)), self.assertRaisesRegex(ValueError, "unsupported/base-clobbering"):
                verify.mapped_bss(self.word_proof(distance=8, preserved=True),
                    struct.pack("<III", 0x3C020035, middle, 0x8C438008), self.sections())
        # An adjustment of SP clobbers the address when SP is the LUI base.
        with self.assertRaisesRegex(ValueError, "unsupported/base-clobbering"):
            verify.mapped_bss(self.word_proof(None, distance=8, preserved=True),
                struct.pack("<III", 0x3C1D0035, 0x27BDFFE0, 0x27A58008), self.sections())
        # Legacy gap8 JR/branch proofs remain unchanged, but the opt-in
        # straight-line preservation grammar deliberately rejects them.
        for middle in (0x03E00008, 0x10000004):
            verify.mapped_bss(self.word_proof(None, distance=8),
                struct.pack("<III", 0x3C020035, middle, 0x24458008), self.sections())
            with self.assertRaises(ValueError):
                verify.mapped_bss(self.word_proof(None, distance=8, preserved=True),
                    struct.pack("<III", 0x3C020035, middle, 0x24458008), self.sections())

    def test_extended_geometry_duplicates_bools_and_longer_gap_fail(self):
        words=struct.pack("<IIII", 0x3C020035, 0, 0, 0x24458008)
        with self.assertRaises(ValueError):
            verify.mapped_bss(self.word_proof(None, distance=12), words, self.sections())
        for key in ("file_offset", "size"):
            proof=self.word_proof(None, distance=12, preserved=True);proof[key]=True
            with self.subTest(key=key), self.assertRaises(ValueError):
                verify.mapped_bss(proof, words, self.sections())
        for key in ("hi_offset", "lo_offset", "relative_offset"):
            proof=self.word_proof(None, distance=12, preserved=True)
            proof["link_bss"][".bss"]["code_bindings"][0][key]=False
            with self.subTest(key=key), self.assertRaises(ValueError):
                verify.mapped_bss(proof, words, self.sections())
        proof=self.word_proof(None, distance=12, preserved=True)
        proof["link_bss"][".bss"]["code_bindings"]*=2
        with self.assertRaises(ValueError):verify.mapped_bss(proof, words, self.sections())
        proof=self.word_proof(None, distance=16, preserved=True)
        with self.assertRaises(ValueError):
            verify.mapped_bss(proof, struct.pack("<IIIII", 0x3C020035, 0, 0, 0, 0x24458008), self.sections())

    def test_scheduled_pair_and_effective_address_bounds_and_carry_fail(self):
        original=struct.pack("<IIII", 0x3C020035, 0, 0, 0x8C438008)
        proof=self.word_proof(distance=12, preserved=True)
        for change in ({"hi_offset": 1}, {"lo_offset": 16}, {"relative_offset": -1}):
            altered=self.word_proof(distance=12, preserved=True)
            altered["link_bss"][".bss"]["code_bindings"][0].update(change)
            with self.subTest(change=change), self.assertRaises(ValueError):
                verify.mapped_bss(altered, original, self.sections())
        with self.assertRaises(ValueError):verify.mapped_bss(proof, original[:-1], self.sections())
        with self.assertRaisesRegex(ValueError, "differs"):
            verify.mapped_bss(proof, struct.pack("<IIII", 0x3C020034, 0, 0, 0x8C438008), self.sections())


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

    @unittest.skipUnless((link_match.DEFAULT_BINUTILS / "mips-ps2-decompals-ld.exe").is_file(), "pinned linker unavailable")
    def test_public_word_access_proof_drives_actual_unpatched_gnu_link(self):
        sections=[{"name": ".bss", "address": 0x340000, "size": 0x10000,
                   "type": "SHT_NOBITS", "flags": 3}]
        for access, input_word, expected_word in (("lw32", 0x8C430000, 0x8C438008),
                                                 ("sw32", 0xAC440000, 0xAC448008)):
            with self.subTest(access=access), tempfile.TemporaryDirectory() as folder:
                path=Path(folder)/"fixture.o"
                original_object=zero_object(words=(0x3C020000,input_word))
                path.write_bytes(original_object)
                expected=struct.pack("<II",0x3C020035,expected_word)
                proof={"file_offset": 0, "size": 8, "link_bss": {".bss": {
                    **zero_mapping(), "original_section": ".bss", "code_bindings": [
                        {"hi_offset": 0, "lo_offset": 4, "relative_offset": 0, "access": access}]}}}
                mapping=verify.mapped_bss(proof,expected,sections)
                linked=link_match.link_function(path,"fixture",0x110000,{},Path(folder)/"linked",
                    mapped_nobits=mapping)
                self.assertEqual(linked["code"],expected)
                self.assertEqual(linked["remaining_relocations"],[])
                self.assertEqual(linked["mapped_nobits"][0]["size"],16)
                self.assertEqual(path.read_bytes(),original_object)
                self.assertEqual(linked["object_path"].read_bytes(),original_object)


if __name__ == "__main__":
    unittest.main()
