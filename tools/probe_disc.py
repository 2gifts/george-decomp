"""Inventory a cooked ISO9660 disc and import only its PS2 boot files.

Original game files belong in the ignored orig/ directory, never in Git. This
reader rejects unsupported multi-extent/interleaved files instead of extracting
incorrect bytes. It needs only Python's standard library and never mounts or
executes the image.
"""

import argparse
import hashlib
import json
import os
import re
import struct
import tempfile
from pathlib import Path

SECTOR = 2048
CHUNK = 1024 * 1024
MAX_DIRECTORY = 16 * 1024 * 1024
MAX_ENTRIES = 100000
MAX_DEPTH = 64
MAX_CNF = 64 * 1024
METADATA_FILES = {"disc_inventory.json", "import_manifest.json"}


class DiscError(ValueError):
    """Invalid, unsupported, or unexpected disc image."""


def _both_endian(data, offset, width):
    little_format, big_format = ("<H", ">H") if width == 2 else ("<I", ">I")
    little = struct.unpack_from(little_format, data, offset)[0]
    big = struct.unpack_from(big_format, data, offset + width)[0]
    if little != big:
        raise DiscError("ISO9660 little/big endian fields disagree")
    return little


def _safe_component(name):
    """Accept one portable, unambiguous output-path component."""
    if (not name or name in (".", "..") or len(name) > 255
            or any(ord(c) < 32 or ord(c) == 127 for c in name)
            or any(c in name for c in '/\\:<>"|?*')
            or name.endswith((".", " "))):
        raise DiscError(f"Unsafe disc path component: {name!r}")
    device = name.split(".")[0].upper()
    if device in {"CON", "PRN", "AUX", "NUL"} or re.fullmatch(r"(?:COM|LPT)[1-9]", device):
        raise DiscError(f"Reserved disc path component: {name!r}")
    return name


def _iso_name(raw):
    try:
        name = raw.decode("ascii")
    except UnicodeDecodeError as exc:
        raise DiscError("Non-ASCII primary ISO9660 filename") from exc
    if name in ("\x00", "\x01"):
        return name
    if ";" in name:
        name, version = name.rsplit(";", 1)
        if ";" in name or not re.fullmatch(r"[1-9][0-9]*", version):
            raise DiscError("Invalid ISO9660 file version")
    return _safe_component(name)


def record(data, offset=0):
    """Parse and validate one ISO9660 directory record."""
    if offset < 0 or offset >= len(data):
        raise DiscError("Directory record lies outside its directory")
    length = data[offset]
    if not length:
        return None
    if length < 34 or offset + length > len(data):
        raise DiscError("Truncated ISO9660 directory record")
    raw = data[offset:offset + length]
    name_length = raw[32]
    if not name_length or 33 + name_length + (name_length % 2 == 0) > length:
        raise DiscError("Truncated ISO9660 filename")
    if raw[1] or raw[26] or raw[27] or raw[25] & 0x80:
        raise DiscError("Extended attributes, interleaving, and multi-extent files are unsupported")
    if _both_endian(raw, 28, 2) != 1:
        raise DiscError("Multi-volume ISO9660 images are unsupported")
    return {
        "name": _iso_name(raw[33:33 + name_length]),
        "extent": _both_endian(raw, 2, 4),
        "size": _both_endian(raw, 10, 4),
        "directory": bool(raw[25] & 2),
    }


class ISO9660:
    """Bounded, read-only reader for a single-volume 2048-byte-sector ISO."""

    def __init__(self, stream):
        self.stream = stream
        stream.seek(0, os.SEEK_END)
        self.image_size = stream.tell()
        if self.image_size % SECTOR or self.image_size < 18 * SECTOR:
            raise DiscError("Expected a cooked ISO9660 image with 2048-byte sectors")
        primary = None
        terminated = False
        for sector in range(16, min(self.image_size // SECTOR, 16 + 64)):
            stream.seek(sector * SECTOR)
            descriptor = stream.read(SECTOR)
            if descriptor[1:7] != b"CD001\x01":
                raise DiscError("Invalid ISO9660 volume descriptor")
            if descriptor[0] == 1:
                if primary is not None:
                    raise DiscError("Multiple primary volume descriptors")
                primary = descriptor
            if descriptor[0] == 255:
                terminated = True
                break
        if primary is None or not terminated:
            raise DiscError("Missing primary volume descriptor or descriptor terminator")
        if _both_endian(primary, 128, 2) != SECTOR:
            raise DiscError("Only 2048-byte ISO9660 logical blocks are supported")
        if _both_endian(primary, 120, 2) != 1 or _both_endian(primary, 124, 2) != 1:
            raise DiscError("Multi-volume ISO9660 images are unsupported")
        self.volume_size = _both_endian(primary, 80, 4) * SECTOR
        if not self.volume_size or self.volume_size > self.image_size:
            raise DiscError("Declared ISO9660 volume exceeds image bounds")
        try:
            self.volume = primary[40:72].decode("ascii").rstrip(" \x00")
        except UnicodeDecodeError as exc:
            raise DiscError("Invalid ISO9660 volume identifier") from exc
        self.root = record(primary, 156)
        if self.root is None or not self.root["directory"] or self.root["name"] != "\x00":
            raise DiscError("Invalid ISO9660 root directory")
        self._bounds(self.root)

    def _bounds(self, entry):
        start = entry["extent"] * SECTOR
        if start > self.volume_size or entry["size"] > self.volume_size - start:
            raise DiscError(f"Disc extent exceeds volume bounds: {entry.get('path', entry['name'])!r}")
        if entry["directory"] and not 0 < entry["size"] <= MAX_DIRECTORY:
            raise DiscError("Invalid or excessive ISO9660 directory size")

    def chunks(self, entry):
        self._bounds(entry)
        self.stream.seek(entry["extent"] * SECTOR)
        remaining = entry["size"]
        while remaining:
            chunk = self.stream.read(min(remaining, CHUNK))
            if not chunk:
                raise DiscError("Disc image ended inside a file")
            remaining -= len(chunk)
            yield chunk

    def inventory(self):
        entries = []
        seen_extents = set()
        seen_paths = set()

        def visit(directory, parent, depth):
            if depth > MAX_DEPTH:
                raise DiscError("ISO9660 directory nesting exceeds safety limit")
            if directory["extent"] in seen_extents:
                raise DiscError("Repeated directory extent or directory cycle")
            seen_extents.add(directory["extent"])
            data = b"".join(self.chunks(directory))
            offset = 0
            while offset < len(data):
                length = data[offset]
                if not length:
                    offset = (offset // SECTOR + 1) * SECTOR
                    continue
                if offset % SECTOR + length > SECTOR:
                    raise DiscError("Directory record crosses a sector boundary")
                item = record(data, offset)
                offset += length
                self._bounds(item)
                if item["name"] in ("\x00", "\x01"):
                    if not item["directory"]:
                        raise DiscError("Special directory entry is not a directory")
                    continue
                item["path"] = parent + item["name"]
                key = item["path"].casefold()
                if key in seen_paths:
                    raise DiscError(f"Duplicate or case-colliding disc path: {item['path']!r}")
                seen_paths.add(key)
                if len(seen_paths) > MAX_ENTRIES:
                    raise DiscError("Disc inventory exceeds safety limit")
                if item["directory"]:
                    visit(item, item["path"] + "/", depth + 1)
                else:
                    entries.append(item)

        visit(self.root, "", 0)
        return sorted(entries, key=lambda item: item["path"])


def boot_path(system_cnf):
    """Resolve a PS2 BOOT2 (or PS1 BOOT) line without trusting its path."""
    try:
        text = system_cnf.decode("ascii")
    except UnicodeDecodeError as exc:
        raise DiscError("SYSTEM.CNF must be ASCII") from exc
    candidates = []
    for line in text.splitlines():
        match = re.fullmatch(r"\s*BOOT2?\s*=\s*(.*?)\s*", line, re.IGNORECASE)
        if match:
            candidates.append(match.group(1))
    if len(candidates) != 1:
        raise DiscError("SYSTEM.CNF must contain exactly one BOOT or BOOT2 entry")
    match = re.fullmatch(r"cdrom[0-9]*:[\\/](.+)", candidates[0], re.IGNORECASE)
    if not match:
        raise DiscError("Unsupported SYSTEM.CNF boot device or path")
    components = match.group(1).replace("\\", "/").split("/")
    return "/".join(_iso_name(component.encode("ascii")) for component in components)


def _sha256(chunks):
    digest = hashlib.sha256()
    for chunk in chunks:
        digest.update(chunk)
    return digest.hexdigest()


def _expected_hash(value, actual, label):
    if value is None:
        return
    if not re.fullmatch(r"[0-9a-fA-F]{64}", value):
        raise DiscError(f"Expected {label} SHA256 must be 64 hexadecimal characters")
    if actual != value.lower():
        raise DiscError(f"{label} SHA256 mismatch: expected {value.lower()}, found {actual}")


def _output_path(root, relative):
    components = relative.split("/")
    for component in components:
        _safe_component(component)
    destination = root.joinpath(*components)
    # Refuse links even if they happen to point inside the output directory.
    current = root
    for component in components:
        current = current / component
        if current.is_symlink():
            raise DiscError(f"Refusing output symlink: {relative}")
    try:
        destination.resolve().relative_to(root)
    except ValueError as exc:
        raise DiscError(f"Output path escapes destination: {relative}") from exc
    if destination.exists() and not destination.is_file():
        raise DiscError(f"Output destination is not a regular file: {relative}")
    return destination


def _atomic_write(destination, chunks, expected_sha256=None):
    destination.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=destination.parent, prefix=".import-", delete=False) as stream:
            temporary = Path(stream.name)
            digest = hashlib.sha256()
            for chunk in chunks:
                digest.update(chunk)
                stream.write(chunk)
        if expected_sha256 is not None and digest.hexdigest() != expected_sha256:
            raise DiscError("Disc content changed during extraction")
        os.replace(temporary, destination)
    finally:
        if temporary is not None and temporary.exists():
            temporary.unlink()


def import_disc(image_path, output_dir=Path("orig"), *, expect_iso_sha256=None, expect_boot_sha256=None):
    """Validate first, then stream SYSTEM.CNF and its boot executable to output.

    Return the deterministic dictionary written to import_manifest.json.
    disc_inventory.json lists every file's path, size, and extent; the other
    disc assets are not extracted.
    """
    image_path = Path(image_path)
    root = Path(output_dir).resolve()
    with image_path.open("rb") as stream:
        original_stat = os.fstat(stream.fileno())
        disc = ISO9660(stream)
        entries = disc.inventory()
        lookup = {entry["path"].casefold(): entry for entry in entries}
        system = lookup.get("system.cnf")
        if system is None or system["size"] > MAX_CNF:
            raise DiscError("Missing or excessive SYSTEM.CNF")
        executable_path = boot_path(b"".join(disc.chunks(system)))
        executable = lookup.get(executable_path.casefold())
        if executable is None or not executable["size"]:
            raise DiscError(f"Boot executable is missing or empty: {executable_path}")
        imported = [system, executable]
        if system is executable or any(entry["path"].split("/")[0].casefold() in METADATA_FILES
                                       for entry in imported):
            raise DiscError("Boot file collides with import metadata or SYSTEM.CNF")
        # Complete all checks before creating or replacing output files.
        hashes = {entry["path"]: _sha256(disc.chunks(entry)) for entry in imported}
        stream.seek(0)
        image_hash = _sha256(iter(lambda: stream.read(CHUNK), b""))
        _expected_hash(expect_iso_sha256, image_hash, "ISO")
        _expected_hash(expect_boot_sha256, hashes[executable["path"]], "Boot executable")
        current_stat = os.fstat(stream.fileno())
        if (current_stat.st_size, current_stat.st_mtime_ns) != (original_stat.st_size, original_stat.st_mtime_ns):
            raise DiscError("Disc image changed during validation")
        manifest = {
            "schema_version": 1,
            "image": {"name": image_path.name, "size": disc.image_size, "sha256": image_hash},
            "volume": disc.volume,
            "boot_path": executable["path"],
            "files": [{"path": entry["path"], "size": entry["size"], "sha256": hashes[entry["path"]]}
                      for entry in imported],
        }
        inventory = {"schema_version": 1, "volume": disc.volume, "files": entries}
        destinations = {name: _output_path(root, name)
                        for name in [entry["path"] for entry in imported] + sorted(METADATA_FILES)}
        if any(destination.resolve() == image_path.resolve() for destination in destinations.values()):
            raise DiscError("Output destination would overwrite the source image")
        for entry in imported:
            _atomic_write(destinations[entry["path"]], disc.chunks(entry), hashes[entry["path"]])
        for name, data in (("disc_inventory.json", inventory), ("import_manifest.json", manifest)):
            _atomic_write(destinations[name], [(json.dumps(data, indent=2, sort_keys=True) + "\n").encode("utf-8")])
    return manifest


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("iso", type=Path)
    parser.add_argument("--output", "--extract", dest="output", type=Path, default=Path("orig"),
                        help="Ignored local import directory (default: orig)")
    parser.add_argument("--expect-iso-sha256", help="Reject a different disc revision before writing")
    parser.add_argument("--expect-boot-sha256", help="Reject a different boot executable before writing")
    args = parser.parse_args(argv)
    try:
        manifest = import_disc(args.iso, args.output, expect_iso_sha256=args.expect_iso_sha256,
                               expect_boot_sha256=args.expect_boot_sha256)
    except (DiscError, OSError) as exc:
        parser.exit(1, f"Import failed: {exc}\n")
    print(f"Imported {manifest['boot_path']} and SYSTEM.CNF to {args.output}")
    print(f"ISO SHA256: {manifest['image']['sha256']}")
    print("Wrote disc_inventory.json and import_manifest.json")


if __name__ == "__main__":
    main()
