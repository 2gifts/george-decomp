"""Test match-accounting gates using fixtures, never retail game bytes."""

import hashlib
import json
import struct
import sys
import tempfile
import unittest
from types import SimpleNamespace
from pathlib import Path
from unittest.mock import patch

# The tools are also runnable as standalone scripts with sibling imports.
TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS))
import analyze  # noqa: E402
import verify  # noqa: E402


def elf_object(*, code=b"\x08\x00\xe0\x03\x00\x00\x00\x00", symbol_size=None,
               symbol_value=0, section_address=0, relocations=()):
    """Small ELF32 MIPS relocatable with one explicit function symbol."""
    symbol_size = len(code) if symbol_size is None else symbol_size
    names = ["", ".text", ".strtab", ".symtab", ".rel.text", ".shstrtab"]
    section_names = b"\x00"
    name_offsets = [0]
    for name in names[1:]:
        name_offsets.append(len(section_names))
        section_names += name.encode("ascii") + b"\x00"
    symbol_strings = b"\x00fixture\x00"
    symbols = b"\x00" * 16 + struct.pack("<IIIBBH", 1, symbol_value, symbol_size, 0x12, 0, 1)
    relocation_data = b"".join(struct.pack("<II", offset, (1 << 8) | 2) for offset in relocations)
    image = bytearray(52)
    sections = [(0,) * 10]
    specifications = [(1, 6, section_address, code, 0, 0, 4, 0),
                      (3, 0, 0, symbol_strings, 0, 0, 1, 0),
                      (2, 0, 0, symbols, 2, 1, 4, 16),
                      (9, 0, 0, relocation_data, 3, 1, 4, 8),
                      (3, 0, 0, section_names, 0, 0, 1, 0)]
    for name_offset, spec in zip(name_offsets[1:], specifications):
        kind, flags, address, data, link, info, alignment, entry_size = spec
        image.extend(b"\x00" * (-len(image) % alignment))
        offset = len(image)
        image.extend(data)
        sections.append((name_offset, kind, flags, address, offset, len(data), link, info, alignment, entry_size))
    image.extend(b"\x00" * (-len(image) % 4))
    section_offset = len(image)
    image.extend(b"".join(struct.pack("<10I", *section) for section in sections))
    ident = b"\x7fELF\x01\x01\x01" + b"\x00" * 9
    image[:52] = struct.pack("<16sHHIIIIIHHHHHH", ident, 1, 8, 1, 0, 0, section_offset, 0,
                             52, 0, 0, 40, len(sections), len(sections) - 1)
    return bytes(image)


class ComparisonTests(unittest.TestCase):
    def test_exact_bytes_size_and_no_relocations_are_required(self):
        code = b"\x08\x00\xe0\x03\x00\x00\x00\x00"
        result = verify.compare_function(code, code)
        self.assertTrue(result["identical"])
        self.assertEqual(result["different_bytes"], 0)
        self.assertEqual(result["expected_sha256"], hashlib.sha256(code).hexdigest())
        self.assertEqual(result["compiled_sha256"], result["expected_sha256"])
        for actual in (code[:-1], code + b"\x00", b"\x09" + code[1:]):
            with self.subTest(actual=actual):
                result = verify.compare_function(code, actual)
                self.assertFalse(result["identical"])
                self.assertEqual(result["different_bytes"], 1)
        relocated = verify.compare_function(code, code, [0])
        self.assertFalse(relocated["identical"])
        self.assertEqual(relocated["different_bytes"], 0)
        self.assertEqual(relocated["unresolved_relocations"], [0])

    def test_number_accepts_hex_and_existing_integer(self):
        self.assertEqual(verify.number("0x001100D0"), 0x1100D0)
        self.assertEqual(verify.number(0x1100D0), 0x1100D0)


class ObjectTests(unittest.TestCase):
    def functions(self, image):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.o"
            path.write_bytes(image)
            return verify.object_functions(path)

    def test_function_bytes_exclude_object_padding(self):
        code = b"\x08\x00\xe0\x03\x00\x00\x00\x00"
        result = self.functions(elf_object(code=code + b"PAD!", symbol_size=len(code)))
        self.assertEqual(result, {"fixture": (code, [])})

    def test_relocations_at_start_inside_and_outside_function(self):
        result = self.functions(elf_object(relocations=[0, 4, 8]))
        code, relocations = result["fixture"]
        self.assertEqual(relocations, [0, 4])
        self.assertFalse(verify.compare_function(code, code, relocations)["identical"])

    def test_symbol_bounds_are_rejected(self):
        for image in (elf_object(symbol_size=9), elf_object(section_address=4)):
            with self.subTest(image=image), self.assertRaisesRegex(ValueError, "bounds"):
                self.functions(image)

    def test_nonmatching_object_formats_rejected(self):
        variants = []
        executable = bytearray(elf_object())
        struct.pack_into("<H", executable, 16, 2)  # ET_EXEC uses absolute relocation addresses.
        variants.append(executable)
        non_mips = bytearray(elf_object())
        struct.pack_into("<H", non_mips, 18, 3)  # EM_386.
        variants.append(non_mips)
        big_endian = bytearray(elf_object())
        big_endian[5] = 2
        variants.append(big_endian)
        elf64 = bytearray(elf_object())
        elf64[4] = 2
        variants.append(elf64)
        for image in variants:
            with self.subTest(image=image), self.assertRaisesRegex(ValueError, "ELF32"):
                self.functions(image)

    def test_function_in_data_section_rejected(self):
        image = bytearray(elf_object())
        section_offset = struct.unpack_from("<I", image, 32)[0]
        struct.pack_into("<I", image, section_offset + 40 + 8, 2)  # SHF_ALLOC only.
        with self.assertRaisesRegex(ValueError, "outside executable"):
            self.functions(image)


class RevisionTests(unittest.TestCase):
    def test_supported_revision_requires_both_hash_and_size(self):
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "fixture.elf"
            configuration = Path(folder) / "revision.json"
            original = elf_object()
            path.write_bytes(original)
            spec = {"executable_size": len(original), "executable_sha256": hashlib.sha256(original).hexdigest()}
            configuration.write_text(json.dumps(spec), encoding="utf-8")
            with patch.object(analyze, "CONFIG", configuration):
                self.assertEqual(analyze.validated_elf(path), (spec, original))
                path.write_bytes(original[:-1] + bytes([original[-1] ^ 1]))
                with self.assertRaisesRegex(ValueError, "does not match"):
                    analyze.validated_elf(path)
                path.write_bytes(original + b"\x00")
                with self.assertRaisesRegex(ValueError, "does not match"):
                    analyze.validated_elf(path)


class ManifestTests(unittest.TestCase):
    def read_manifests(self, game, runtime=(), batch=()):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            config = root / "config"
            config.mkdir()
            for name, functions in (("recovered_functions.json", game), ("runtime_functions.json", runtime)):
                (config / name).write_text(json.dumps({"functions": list(functions)}), encoding="utf-8")
            (config / "functions").mkdir()
            (config / "functions/batch.json").write_text(json.dumps({"functions": list(batch)}), encoding="utf-8")
            with patch.object(verify, "ROOT", root):
                return verify.manifests()

    def test_adjacent_ranges_are_counted_once(self):
        game = [{"address": "0x1000", "size": "0x4"}]
        runtime = [{"address": 0x1004, "size": 4}]
        self.assertEqual(self.read_manifests(game, runtime), game + runtime)

    def test_overlap_across_manifests_rejected(self):
        game = [{"address": "0x1000", "size": "0x8"}]
        for runtime in ([{"address": "0x1004", "size": 8}], game):
            with self.subTest(runtime=runtime), self.assertRaisesRegex(ValueError, "overlap"):
                self.read_manifests(game, runtime)

    def test_empty_manifest_rejected(self):
        with self.assertRaisesRegex(ValueError, "No recovered"):
            self.read_manifests([])

    def test_batch_manifests_are_included_and_cannot_overlap(self):
        game = [{"address": "0x1000", "size": 4}]
        batch = [{"address": "0x1004", "size": 4}]
        self.assertEqual(self.read_manifests(game, batch=batch), game + batch)
        with self.assertRaisesRegex(ValueError, "overlap"):
            self.read_manifests(game, batch=game)


class BindingTests(unittest.TestCase):
    def bindings(self, central, function):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "config/symbols").mkdir(parents=True)
            for index, values in enumerate(central):
                (root / f"config/symbols/{index}.json").write_text(json.dumps({"symbols": values}), encoding="utf-8")
            with patch.object(verify, "ROOT", root):
                return verify.symbol_bindings(function)

    def test_central_and_per_function_addresses_are_merged(self):
        result = self.bindings([{"global": "0x4000", "callee": {"address": "0x1000"}}],
                               {"link_symbols": {"callee": 0x1000, "other": "0x2000"}})
        self.assertEqual(result, {"global": 0x4000, "callee": 0x1000, "other": 0x2000})

    def test_conflicting_central_addresses_are_rejected(self):
        with self.assertRaisesRegex(ValueError, "Conflicting symbol"):
            self.bindings([{"callee": "0x1000"}, {"callee": "0x1004"}], {})

    def test_function_cannot_override_central_address(self):
        with self.assertRaisesRegex(ValueError, "Conflicting per-function"):
            self.bindings([{"callee": "0x1000"}], {"link_symbols": {"callee": "0x1004"}})


class TargetRangeTests(unittest.TestCase):
    SECTIONS = [{"name": ".text", "offset": 0x1000, "address": 0x100000, "size": 0x10}]

    def test_valid_section_start_and_exact_end(self):
        for offset, size in ((0x1000, 4), (0x1008, 8)):
            function = {"name": "fixture", "file_offset": hex(offset), "size": hex(size),
                        "address": hex(0x100000 + offset - 0x1000)}
            self.assertIsNone(verify.validate_target_function(function, self.SECTIONS))

    def test_faked_address_rejected(self):
        function = {"name": "fixture", "file_offset": 0x1004, "size": 4, "address": 0x100000}
        with self.assertRaisesRegex(ValueError, "address/offset"):
            verify.validate_target_function(function, self.SECTIONS)

    def test_outside_code_and_crossing_end_rejected(self):
        for offset, size in ((0x0FFC, 4), (0x100C, 8), (0x1010, 4), (0x2000, 4)):
            function = {"name": "fixture", "file_offset": offset, "size": size,
                        "address": 0x100000 + offset - 0x1000}
            with self.subTest(offset=offset, size=size), self.assertRaisesRegex(ValueError, "code section"):
                verify.validate_target_function(function, self.SECTIONS)

    def test_empty_and_negative_ranges_rejected(self):
        for offset, size in ((0x1000, 0), (0x1000, -1), (-1, 4)):
            function = {"name": "fixture", "file_offset": offset, "size": size, "address": 0x100000}
            with self.subTest(offset=offset, size=size), self.assertRaisesRegex(ValueError, "Invalid function range"):
                verify.validate_target_function(function, self.SECTIONS)


class CompilerProfileTests(unittest.TestCase):
    def test_default_override_preserves_other_profiles(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "config").mkdir()
            data = {"default_profile": "default", "profiles": {
                "default": {"compiler": "old.exe", "compiler_sha256": "old", "path_entries": ["old"]},
                "other": {"compiler": "other.exe", "compiler_sha256": "other"}}}
            (root / "config/compiler_profiles.json").write_text(json.dumps(data), encoding="utf-8")
            args = SimpleNamespace(compiler=root / "custom.exe", dll_path=root / "dll")
            with patch.object(verify, "ROOT", root):
                result = verify.compiler_configuration(args)
            self.assertEqual(result["profiles"]["other"], data["profiles"]["other"])
            self.assertEqual(result["profiles"]["default"]["compiler"], str(args.compiler.resolve()))
            self.assertNotIn("compiler_sha256", result["profiles"]["default"])
            self.assertEqual(result["profiles"]["default"]["path_entries"], [str(args.dll_path.resolve())])

    def test_dependencies_verified_before_running_compiler(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "compiler.exe").write_bytes(b"driver")
            profile = {"compiler": "compiler.exe", "binaries": [
                {"path": "frontend.exe", "sha256": hashlib.sha256(b"frontend").hexdigest()}]}
            with patch.object(verify, "ROOT", root), patch.object(verify.subprocess, "run") as run:
                with self.assertRaisesRegex(ValueError, "dependency fingerprint"):
                    verify.prepare_compiler(profile)
                (root / "frontend.exe").write_bytes(b"different")
                with self.assertRaisesRegex(ValueError, "dependency fingerprint"):
                    verify.prepare_compiler(profile)
                run.assert_not_called()

    def test_assembler_verified_before_running_compiler(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "compiler.exe").write_bytes(b"driver")
            profile = {"compiler": "compiler.exe", "assembler_provenance": {
                "path": "assembler.exe", "sha256": hashlib.sha256(b"assembler").hexdigest()}}
            with patch.object(verify, "ROOT", root), patch.object(verify.subprocess, "run") as run:
                with self.assertRaisesRegex(ValueError, "dependency fingerprint"):
                    verify.prepare_compiler(profile)
                run.assert_not_called()


class MappedDataTests(unittest.TestCase):
    ORIGINAL = b"prefix!!constanttail"
    SECTIONS = [{"offset": 8, "address": 0x4000, "size": 8}]

    def proof(self, **overrides):
        return {"link_data": {".rodata": {"file_offset": 8, "size": 8,
                "address": 0x4000, "original_sha256": hashlib.sha256(b"constant").hexdigest(), **overrides}}}

    def test_mapping_requires_address_geometry_and_entire_fingerprint(self):
        result = verify.mapped_data(self.proof(), self.ORIGINAL, self.SECTIONS)
        self.assertEqual(result[".rodata"]["expected_bytes"], b"constant")
        self.assertEqual(result[".rodata"]["address"], 0x4000)
        for overrides in ({"address": 0x4004}, {"file_offset": 7}, {"size": 9}, {"size": 0}, {"file_offset": -1}):
            with self.subTest(overrides=overrides), self.assertRaisesRegex(ValueError, "read-only data"):
                verify.mapped_data(self.proof(**overrides), self.ORIGINAL, self.SECTIONS)
        with self.assertRaisesRegex(ValueError, "fingerprint"):
            verify.mapped_data(self.proof(original_sha256="0" * 64), self.ORIGINAL, self.SECTIONS)

    def test_data_without_proven_original_section_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "read-only data"):
            verify.mapped_data(self.proof(), self.ORIGINAL, [])


if __name__ == "__main__":
    unittest.main()
