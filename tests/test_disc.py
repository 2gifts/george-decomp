"""Synthetic ISO tests: no original game data is required."""

import hashlib
import io
import json
import os
import struct
import tempfile
import unittest
from pathlib import Path

from tools.probe_disc import DiscError, ISO9660, SECTOR, boot_path, import_disc


def both(data, offset, value, width=4):
    struct.pack_into("<I" if width == 4 else "<H", data, offset, value)
    struct.pack_into(">I" if width == 4 else ">H", data, offset + width, value)


def directory_record(name, extent, size, directory=False):
    raw = name if isinstance(name, bytes) else name.encode("ascii")
    data = bytearray(33 + len(raw) + (len(raw) % 2 == 0))
    data[0] = len(data)
    both(data, 2, extent)
    both(data, 10, size)
    data[25] = 2 if directory else 0
    both(data, 28, 1, 2)
    data[32] = len(raw)
    data[33:33 + len(raw)] = raw
    return data


def synthetic_iso(extra_names=(), boot=b"ELF fixture bytes", cnf=None):
    cnf = cnf or b"BOOT2 = cdrom0:\\SLUS_123.45;1\r\nVER = 1.00\r\n"
    image = bytearray(32 * SECTOR)
    pvd = bytearray(SECTOR)
    pvd[:7] = b"\x01CD001\x01"
    pvd[40:72] = b"TEST DISC".ljust(32, b" ")
    both(pvd, 80, 32)
    both(pvd, 120, 1, 2)
    both(pvd, 124, 1, 2)
    both(pvd, 128, SECTOR, 2)
    root = directory_record(b"\x00", 20, SECTOR, True)
    pvd[156:156 + len(root)] = root
    image[16 * SECTOR:17 * SECTOR] = pvd
    image[17 * SECTOR:17 * SECTOR + 7] = b"\xffCD001\x01"
    records = [root, directory_record(b"\x01", 20, SECTOR, True),
               directory_record("SYSTEM.CNF;1", 21, len(cnf)),
               directory_record("SLUS_123.45;1", 22, len(boot)),
               directory_record("DATA.BIN;1", 23, 5)]
    records += [directory_record(name, 24, 4) for name in extra_names]
    directory = b"".join(records)
    image[20 * SECTOR:20 * SECTOR + len(directory)] = directory
    image[21 * SECTOR:21 * SECTOR + len(cnf)] = cnf
    image[22 * SECTOR:22 * SECTOR + len(boot)] = boot
    image[23 * SECTOR:23 * SECTOR + 5] = b"asset"
    return image


class DiscTests(unittest.TestCase):
    def import_fixture(self, folder, image=None, **kwargs):
        path = Path(folder) / "fixture.iso"
        path.write_bytes(synthetic_iso() if image is None else image)
        return import_disc(path, Path(folder) / "orig", **kwargs)

    def test_import_only_boot_files_and_deterministic_manifest(self):
        with tempfile.TemporaryDirectory() as folder:
            manifest = self.import_fixture(folder)
            output = Path(folder) / "orig"
            self.assertEqual({item.name for item in output.iterdir()},
                             {"SYSTEM.CNF", "SLUS_123.45", "disc_inventory.json", "import_manifest.json"})
            self.assertEqual(manifest["boot_path"], "SLUS_123.45")
            self.assertEqual(manifest["image"]["sha256"], hashlib.sha256(synthetic_iso()).hexdigest())
            for entry in manifest["files"]:
                self.assertEqual(entry["sha256"], hashlib.sha256((output / entry["path"]).read_bytes()).hexdigest())
            self.assertEqual(json.loads((output / "import_manifest.json").read_text()), manifest)
            inventory = json.loads((output / "disc_inventory.json").read_text())
            self.assertEqual([entry["path"] for entry in inventory["files"]],
                             ["DATA.BIN", "SLUS_123.45", "SYSTEM.CNF"])
            first_manifest = (output / "import_manifest.json").read_bytes()
            self.assertEqual(self.import_fixture(folder), manifest)
            self.assertEqual((output / "import_manifest.json").read_bytes(), first_manifest)

    def test_expected_hashes_are_checked_before_writes(self):
        for option in ("expect_iso_sha256", "expect_boot_sha256"):
            with self.subTest(option=option), tempfile.TemporaryDirectory() as folder:
                with self.assertRaisesRegex(DiscError, "SHA256 mismatch"):
                    self.import_fixture(folder, **{option: "0" * 64})
                self.assertFalse((Path(folder) / "orig").exists())

    def test_valid_expected_hashes(self):
        with tempfile.TemporaryDirectory() as folder:
            self.import_fixture(folder, expect_iso_sha256=hashlib.sha256(synthetic_iso()).hexdigest().upper(),
                                expect_boot_sha256=hashlib.sha256(b"ELF fixture bytes").hexdigest())

    def test_out_of_bounds_file_rejected(self):
        image = synthetic_iso()
        offset = 20 * SECTOR + 34 + 34
        both(image, offset + 2, 32)
        with self.assertRaisesRegex(DiscError, "bounds"):
            ISO9660(io.BytesIO(image)).inventory()

    def test_out_of_bounds_volume_rejected(self):
        image = synthetic_iso()
        both(image, 16 * SECTOR + 80, 33)
        with self.assertRaisesRegex(DiscError, "bounds"):
            ISO9660(io.BytesIO(image))

    def test_endian_mismatch_rejected(self):
        image = synthetic_iso()
        image[16 * SECTOR + 84] = 1
        with self.assertRaisesRegex(DiscError, "endian"):
            ISO9660(io.BytesIO(image))

    def test_truncated_record_rejected(self):
        image = synthetic_iso()
        image[20 * SECTOR] = 20
        with self.assertRaisesRegex(DiscError, "Truncated"):
            ISO9660(io.BytesIO(image)).inventory()

    def test_unsafe_and_case_colliding_disc_paths_rejected(self):
        for name in ("../BAD;1", "..;1", "C:BAD;1", "CON;1", "NUL.BIN;1", "BAD\\NAME;1", "data.bin;1"):
            with self.subTest(name=name), self.assertRaises(DiscError):
                ISO9660(io.BytesIO(synthetic_iso(extra_names=[name]))).inventory()

    def test_boot_paths(self):
        self.assertEqual(boot_path(b"BOOT2 = cdrom0:\\BIN\\MAIN.ELF;1\n"), "BIN/MAIN.ELF")
        self.assertEqual(boot_path(b"boot = cdrom:/MAIN.EXE;1\n"), "MAIN.EXE")
        for cnf in (b"BOOT2 = cdrom0:\\..\\BAD;1\n", b"BOOT2 = cdrom0:\\\\BAD;1\n",
                    b"BOOT2 = host:BAD\n", b"VER = 1.00\n",
                    b"BOOT2 = cdrom0:\\MAIN;1\nBOOT = cdrom:\\OTHER;1\n"):
            with self.subTest(cnf=cnf), self.assertRaises(DiscError):
                boot_path(cnf)

    def test_missing_boot_file_rejected_before_writes(self):
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(DiscError, "missing"):
                self.import_fixture(folder, synthetic_iso(cnf=b"BOOT2 = cdrom0:\\OTHER.ELF;1\n"))
            self.assertFalse((Path(folder) / "orig").exists())

    def test_nested_boot_file_import(self):
        image = synthetic_iso(cnf=b"BOOT2 = cdrom0:\\BIN\\MAIN.ELF;1\n")
        root_records = [directory_record(b"\x00", 20, SECTOR, True),
                        directory_record(b"\x01", 20, SECTOR, True),
                        directory_record("SYSTEM.CNF;1", 21, len(b"BOOT2 = cdrom0:\\BIN\\MAIN.ELF;1\n")),
                        directory_record("BIN", 24, SECTOR, True),
                        directory_record("DATA.BIN;1", 23, 5)]
        image[20 * SECTOR:21 * SECTOR] = b"".join(root_records).ljust(SECTOR, b"\x00")
        nested = [directory_record(b"\x00", 24, SECTOR, True),
                  directory_record(b"\x01", 20, SECTOR, True),
                  directory_record("MAIN.ELF;1", 22, len(b"ELF fixture bytes"))]
        image[24 * SECTOR:25 * SECTOR] = b"".join(nested).ljust(SECTOR, b"\x00")
        with tempfile.TemporaryDirectory() as folder:
            manifest = self.import_fixture(folder, image)
            self.assertEqual(manifest["boot_path"], "BIN/MAIN.ELF")
            self.assertEqual((Path(folder) / "orig" / "BIN" / "MAIN.ELF").read_bytes(), b"ELF fixture bytes")
            self.assertFalse((Path(folder) / "orig" / "DATA.BIN").exists())

    def test_directory_cycle_rejected(self):
        image = synthetic_iso()
        directory = directory_record("LOOP", 20, SECTOR, True)
        offset = 20 * SECTOR + sum(len(directory_record(*args)) for args in (
            (b"\x00", 20, SECTOR, True), (b"\x01", 20, SECTOR, True),
            ("SYSTEM.CNF;1", 21, 43), ("SLUS_123.45;1", 22, 17), ("DATA.BIN;1", 23, 5)))
        image[offset:offset + len(directory)] = directory
        with self.assertRaisesRegex(DiscError, "cycle"):
            ISO9660(io.BytesIO(image)).inventory()

    def test_existing_output_symlink_rejected(self):
        with tempfile.TemporaryDirectory() as folder:
            output = Path(folder) / "orig"
            output.mkdir()
            elsewhere = Path(folder) / "elsewhere"
            elsewhere.write_bytes(b"untouched")
            try:
                os.symlink(elsewhere, output / "SLUS_123.45")
            except (OSError, NotImplementedError):
                self.skipTest("Creating symlinks is unavailable on this host")
            with self.assertRaisesRegex(DiscError, "symlink"):
                self.import_fixture(folder)
            self.assertEqual(elsewhere.read_bytes(), b"untouched")


if __name__ == "__main__":
    unittest.main()
