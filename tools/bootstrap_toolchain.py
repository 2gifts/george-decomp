#!/usr/bin/env python3
"""Install the pinned, openly published Windows PS2 tools inside tools/vendor.

No Sony SDK or proprietary compiler is downloaded. Sources and provenance are
documented in docs/reuse.md and tools/toolchain_manifest.json.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import sys
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]
VENDOR = ROOT / "tools" / "vendor"
MANIFEST = ROOT / "tools" / "toolchain_manifest.json"


def digest(path):
    hasher = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            hasher.update(chunk)
    return hasher.hexdigest()


def verify_component(component):
    failures = []
    for binary in component["binaries"]:
        path = ROOT / binary["path"]
        if not path.is_file() or digest(path) != binary["sha256"]:
            failures.append(binary["path"])
    return failures


def safe_member(name):
    # Reject both POSIX and Windows traversal/absolute path conventions.
    normalized = name.replace("\\", "/")
    path = PurePosixPath(normalized)
    if path.is_absolute() or ".." in path.parts or ":" in normalized:
        raise RuntimeError(f"Unsafe archive member: {name!r}")


def fetch(component, archive_path):
    if archive_path.is_file() and digest(archive_path) == component["archive_sha256"]:
        return
    request = urllib.request.Request(
        component["archive_url"], headers={"User-Agent": "GeorgeDecomp-tool-bootstrap/1"}
    )
    temporary = archive_path.with_name(archive_path.name + ".partial")
    print(f"Downloading {component['archive_url']}", flush=True)
    with urllib.request.urlopen(request, timeout=60) as response, temporary.open("wb") as stream:
        shutil.copyfileobj(response, stream)
    if digest(temporary) != component["archive_sha256"]:
        raise RuntimeError(f"SHA-256 mismatch for {archive_path.name}; refusing extraction")
    os.replace(temporary, archive_path)


def install_binutils(component):
    archive = VENDOR / "binutils-mips-ps2-decompals-windows-x86-64-v0.10.zip"
    fetch(component, archive)
    destination = VENDOR / "binutils-v0.10"
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(archive) as package:
        for info in package.infolist():
            safe_member(info.filename)
            if (info.external_attr >> 16) & 0o170000 == 0o120000:
                raise RuntimeError("Symbolic links are not accepted in the tools ZIP")
        package.extractall(destination)


def install_compiler(component):
    archive = VENDOR / "ps2toolchain-20181019.7z"
    fetch(component, archive)
    destination = VENDOR / "ps2dev-20181019"
    destination.mkdir(parents=True, exist_ok=True)
    # Windows ships libarchive's tar, which reads this 7-Zip archive directly.
    tar = shutil.which("tar")
    if not tar:
        raise RuntimeError("Windows tar.exe is required to extract the official 7-Zip archive")
    listing = subprocess.run([tar, "-tf", str(archive)], check=True, capture_output=True, text=True)
    for member in listing.stdout.splitlines():
        safe_member(member)
    subprocess.run(
        [tar, "-xf", str(archive), "-C", str(destination), *component["extracted_roots"]],
        check=True,
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--verify-only", action="store_true", help="Check installed hashes without network access")
    args = parser.parse_args()
    if sys.platform != "win32":
        raise RuntimeError("This manifest and installer target Windows x86-64; other hosts need a separate tool lock")
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    VENDOR.mkdir(parents=True, exist_ok=True)
    for name, installer in [("binutils", install_binutils), ("candidate_compiler", install_compiler)]:
        component = manifest[name]
        failures = verify_component(component)
        if failures and not args.verify_only:
            installer(component)
            failures = verify_component(component)
        if failures:
            raise RuntimeError(f"{name} failed hash verification: {', '.join(failures)}")
        print(f"Verified {name}: {len(component['binaries'])} binary hashes")
    print("Tools ready. GCC 3.2.3 is a candidate compiler; the original game compiler is still unidentified.")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        print(f"Toolchain setup failed: {error}", file=sys.stderr)
        sys.exit(1)
