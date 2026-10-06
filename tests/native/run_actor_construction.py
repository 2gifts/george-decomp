"""Compile and run the asset-free 32-bit actor initialization/physics/lifecycle harness."""
from pathlib import Path
import argparse
import os
import subprocess
import ctypes

ROOT = Path(__file__).resolve().parents[2]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=ROOT /
                        "tools/vendor/ps2dev-20181019/MinGW/bin/gcc.exe")
    compiler = parser.parse_args().compiler.resolve()
    if not compiler.is_file():
        parser.error("32-bit native GCC is missing; bootstrap toolchain or pass --compiler")
    output = ROOT / "build/native/actor_construction"
    output.mkdir(parents=True, exist_ok=True)
    executable = output / "semantic_harness.exe"
    env = os.environ.copy()
    env["PATH"] = str(compiler.parent) + os.pathsep + env.get("PATH", "")
    if os.name == "nt":
        ctypes.windll.kernel32.SetErrorMode(0x0003)
    command = [str(compiler), "-m32", "-O2", "-Wall", "-Wextra",
               "-Werror=implicit-function-declaration", "-fno-strict-aliasing", "-I", str(ROOT / "include"),
               str(ROOT / "tests/native/actor_construction.c"), "-lm", "-o", str(executable)]
    subprocess.run(command, env=env, cwd=ROOT, check=True)
    subprocess.run([str(executable)], env=env, cwd=ROOT, check=True, timeout=30)

if __name__ == "__main__":
    main()
