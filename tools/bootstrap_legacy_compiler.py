#!/usr/bin/env python3
"""Optional pinned GPL GCC 2.9 EE compiler setup for Windows.

Install a reviewed binary ZIP, or rebuild its corresponding public GNU source.
The default GCC 3.2.3 toolchain is installed by bootstrap_toolchain.py separately.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import urllib.request
import zipfile

from bootstrap_toolchain import ROOT, VENDOR, digest, safe_member

PROFILE_PATH = ROOT / "config/compiler_profiles.json"
REVISION = "b595ded606227e93b8c4a447446c1d2ac093827d"
REPOSITORY = "https://github.com/SSXModding/ps2-ee-toolchain"
SOURCE = VENDOR / "ps2-ee-toolchain"
BUILD = ROOT / "build/reuse/gcc29"
INSTALL = VENDOR / "gcc29-local"
MINGW = VENDOR / "ps2dev-20181019/MinGW"
PATCH = ROOT / "tools/patches/gcc29-windows-host.patch"
PREFIX = "/r/tools/vendor/gcc29-local"


def run(command, **kwargs):
    return subprocess.run(command, check=True, **kwargs)


def failures(profile):
    return [entry["path"] for entry in profile["binaries"]
            if not (ROOT / entry["path"]).is_file()
            or digest(ROOT / entry["path"]) != entry["sha256"]]


def extract_zip(archive):
    with zipfile.ZipFile(archive) as package:
        for info in package.infolist():
            safe_member(info.filename)
            if (info.external_attr >> 16) & 0o170000 == 0o120000:
                raise RuntimeError("Symbolic links are not accepted in the compiler ZIP")
            if not info.filename.startswith("gcc29-local/"):
                raise RuntimeError(f"Unexpected compiler ZIP member: {info.filename}")
        package.extractall(VENDOR)


def install_binary(profile, archive=None):
    metadata = profile["archive"]
    if not metadata.get("sha256"):
        raise RuntimeError("No reviewed compiler archive hash is configured")
    if archive is None:
        if not metadata.get("url"):
            raise RuntimeError("The optional compiler release is not published yet; use --build or --binary-archive")
        archive = VENDOR / "gcc29-windows.zip"
        if not archive.is_file() or digest(archive) != metadata["sha256"]:
            request = urllib.request.Request(metadata["url"], headers={"User-Agent": "GeorgeDecomp-legacy-bootstrap/1"})
            with urllib.request.urlopen(request, timeout=60) as response, archive.open("wb") as output:
                shutil.copyfileobj(response, output)
    if digest(archive) != metadata["sha256"]:
        raise RuntimeError("Legacy compiler archive SHA-256 mismatch; refusing extraction")
    extract_zip(archive)


def prepare_host():
    archive = VENDOR / "ps2toolchain-20181019.7z"
    manifest = json.loads((ROOT / "tools/toolchain_manifest.json").read_text())
    if not archive.is_file() or digest(archive) != manifest["candidate_compiler"]["archive_sha256"]:
        raise RuntimeError("Run tools/bootstrap_toolchain.py first to obtain the pinned official PS2DEV archive")
    tar = shutil.which("tar")
    if not tar:
        raise RuntimeError("Windows tar.exe is required")
    listing = run([tar, "-tf", str(archive)], capture_output=True, text=True)
    for member in listing.stdout.splitlines():
        safe_member(member)
    # GNU host compiler/runtime only; the existing EE assembler is reused.
    run([tar, "-xf", str(archive), "-C", str(MINGW.parent),
         "MinGW/lib", "MinGW/include", "MinGW/libexec",
         "MinGW/msys/1.0/bin", "MinGW/msys/1.0/etc"])


def prepare_source(profile):
    if not SOURCE.exists():
        run(["git", "-c", "core.autocrlf=false", "clone", "--no-checkout", REPOSITORY, str(SOURCE)])
        run(["git", "-C", str(SOURCE), "-c", "core.autocrlf=false", "checkout", REVISION])
    revision = run(["git", "-C", str(SOURCE), "rev-parse", "HEAD"], capture_output=True, text=True).stdout.strip()
    if revision != REVISION:
        raise RuntimeError("Existing GNU source checkout has a different revision; refusing to reset it")
    if digest(PATCH) != profile["source_provenance"]["host_patch_sha256"]:
        raise RuntimeError("Host patch hash does not match the pinned compiler profile")
    check = subprocess.run(["git", "-C", str(SOURCE), "apply", "--check", str(PATCH)], capture_output=True)
    if check.returncode == 0:
        run(["git", "-C", str(SOURCE), "apply", str(PATCH)])
    else:
        reverse = subprocess.run(["git", "-C", str(SOURCE), "apply", "--reverse", "--check", str(PATCH)], capture_output=True)
        if reverse.returncode:
            raise RuntimeError("GNU host patch is neither applicable nor already applied; inspect local source changes")


def build_source(profile, jobs):
    prepare_host()
    prepare_source(profile)
    BUILD.mkdir(parents=True, exist_ok=True)
    # This old make cannot use a source/build path containing spaces. A temporary
    # drive alias preserves the tested /r paths without copying the checkout.
    alias = Path("R:/")
    created_alias = False
    if alias.exists():
        if not os.path.samefile(alias, ROOT):
            raise RuntimeError("Drive R: is already in use; this recipe needs R: free")
    else:
        run(["subst", "R:", str(ROOT)])
        created_alias = True
    fstab = MINGW / "msys/1.0/etc/fstab"
    old_fstab = fstab.read_bytes() if fstab.exists() else None
    fstab.write_text("R:/tools/vendor/ps2dev-20181019/MinGW /mingw\n")
    script = BUILD / "build-compiler.sh"
    # All interpolated paths are fixed recipe paths; jobs is a validated integer.
    script.write_text(f"""#!/bin/sh
set -eu
export PATH=/mingw/bin:/bin:$PATH
export CC=gcc
export LDFLAGS=-static
mkdir -p /r/build/reuse/gcc29/libiberty /r/build/reuse/gcc29/bison /r/build/reuse/gcc29/gcc
cd /r/build/reuse/gcc29/libiberty
if test ! -f Makefile; then
  /r/tools/vendor/ps2-ee-toolchain/ee/libiberty/configure --build=i686-pc-mingw32 --host=i686-pc-mingw32 --target=ee --prefix={PREFIX} --srcdir=/r/tools/vendor/ps2-ee-toolchain/ee/libiberty --with-gcc-version-trigger=/r/tools/vendor/ps2-ee-toolchain/ee/gcc/version.c --with-stabs --with-gnu-as --with-gnu-ld --with-newlib --without-sim --enable-c-cpplib --enable-languages=c,c++ --disable-sim --disable-nls > configure.log 2>&1
fi
make -j{jobs} > build.log 2>&1
cd /r/build/reuse/gcc29/bison
if test ! -f Makefile; then
  /r/tools/vendor/ps2-ee-toolchain/ee/bison/configure --host=i686-pc-mingw32 --build=i686-pc-mingw32 --disable-nls --prefix={PREFIX} > configure.log 2>&1
fi
make -j{jobs} > build.log 2>&1
cd /r/build/reuse/gcc29/gcc
if test ! -f Makefile; then
  /r/tools/vendor/ps2-ee-toolchain/ee/gcc/configure --target=mips64r5900-sky-elf --enable-c-cpplib --with-newlib --disable-multilib --host=i686-pc-mingw32 --build=i686-pc-mingw32 --disable-nls --enable-languages=c,c++ --program-prefix=ee- --prefix={PREFIX} > configure.log 2>&1
fi
make -j{jobs} cc1.exe cc1plus.exe xgcc.exe cpp.exe BISON="/r/build/reuse/gcc29/bison/bison.exe -L /r/tools/vendor/ps2-ee-toolchain/ee/bison/" > build-compiler.log 2>&1
""", newline="\n")
    try:
        run([str(MINGW / "msys/1.0/bin/bash.exe"), "--login", "/r/build/reuse/gcc29/build-compiler.sh"], cwd=BUILD)
    finally:
        if old_fstab is None:
            fstab.unlink(missing_ok=True)
        else:
            fstab.write_bytes(old_fstab)
        if created_alias:
            run(["subst", "R:", "/D"])
    backend = INSTALL / "lib/gcc-lib/mips64r5900-sky-elf/2.9-ee-991111"
    backend.mkdir(parents=True, exist_ok=True)
    (INSTALL / "bin").mkdir(parents=True, exist_ok=True)
    for name in ("cc1.exe", "cc1plus.exe", "cpp.exe"):
        shutil.copy2(BUILD / "gcc" / name, backend / name)
    shutil.copy2(BUILD / "gcc/xgcc.exe", INSTALL / "bin/ee-gcc.exe")
    shutil.copytree(SOURCE / "ee/gcc/ginclude", INSTALL / "include", dirs_exist_ok=True)
    # Keep local source builds distinct from the reviewed binary release: PE
    # build timestamps and host debug paths can change executable hashes.
    receipt = {"source_revision": REVISION, "host_patch_sha256": digest(PATCH),
               "binaries": [{"path": entry["path"], "sha256": digest(ROOT / entry["path"])} for entry in profile["binaries"]]}
    (BUILD / "local-build-receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--verify-only", action="store_true")
    parser.add_argument("--build", action="store_true", help="Build public GNU source; executable fingerprints may differ from the release")
    parser.add_argument("--binary-archive", type=Path, help="Install a local copy of the reviewed binary ZIP")
    parser.add_argument("--jobs", type=int, default=4)
    args = parser.parse_args()
    if sys.platform != "win32":
        raise RuntimeError("This optional recipe targets Windows")
    if not 1 <= args.jobs <= 64:
        raise RuntimeError("--jobs must be between 1 and 64")
    if sum((args.verify_only, args.build, args.binary_archive is not None)) > 1:
        raise RuntimeError("Choose one setup mode")
    profile = json.loads(PROFILE_PATH.read_text())["profiles"]["gcc29"]
    if args.build:
        build_source(profile, args.jobs)
        different = failures(profile)
        if different:
            print("Local source build complete. Its host executable hashes differ from the reviewed release.")
            print("Review build/reuse/gcc29/local-build-receipt.json before selecting this compiler for public progress.")
            return
    elif not args.verify_only and (failures(profile) or args.binary_archive):
        install_binary(profile, args.binary_archive)
    different = failures(profile)
    if different:
        raise RuntimeError("Pinned legacy compiler hash verification failed: " + ", ".join(different))
    print(f"Verified optional GCC {profile['version']}: {len(profile['binaries'])} executable hashes")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        print(f"Legacy compiler setup failed: {error}", file=sys.stderr)
        sys.exit(1)
