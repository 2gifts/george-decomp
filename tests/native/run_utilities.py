"""Build focused native 32-bit utility harnesses, without original game files."""
import argparse
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HARNESS_SOURCES = {
    "text_content": ["src/game/text_content.c"],
    "text_numbers": ["src/game/text_numbers.c", "src/game/text_tokens.c", "tests/native/text_lookup_ctype.c"],
    "text_parser": ["src/game/text_parser.c", "src/game/text_tokens.c", "src/game/text_lookup.c", "tests/native/text_lookup_ctype.c"],
    "engine_angles": ["src/game/engine_angles.c"],
    "text_values": ["src/game/text_values.c", "src/game/text_tokens.c"],
    "algorithm_duplicates": ["src/game/algorithm_duplicates.c"],
    "text_lookup": ["src/game/text_lookup.c", "src/game/text_tokens.c", "tests/native/text_lookup_ctype.c"],
    "text_tokens": ["src/game/text_tokens.c"],
    "geometry_bounds": ["src/game/geometry_bounds.c"],
    "cache_transfer": ["src/game/cache_transfer.c"],
    "matrix_scalar": ["src/game/matrix_scalar.c"],
    "byte_order": ["src/game/byte_order.c"],
    "rotation": ["src/game/rotation.c"],
    "pad_device": ["src/game/pad_device.c"],
    "pad_input": ["src/game/pad_input.c"],
    "input_state": ["src/game/input_state.c"],
    "heap": ["src/game/heap.c"],
    "list": ["src/game/list.c", "src/game/list_aros.c"],
    "pool_slots": ["src/game/pool_slots.c"],
    "interpolation": ["src/game/interpolation.c"],
    "deimos_interpreter": ["src/game/deimos_interpreter.c"],
    "tree": ["src/game/tree.c", "src/game/list.c", "src/game/list_aros.c"],
    "tree_updates": ["src/game/tree_updates.c", "src/game/tree.c", "src/game/list.c", "src/game/list_aros.c"],
    "vector_math": ["src/game/vector_math.c"],
    "geometry": ["src/game/geometry.c", "src/game/vector_math.c"],
}
HARNESS_FLAGS = {
    "engine_angles": ["-ffloat-store"],
    "tree_updates": ["-DGEORGE_TREE_NATIVE_COUNTER"],
}
# Windows may classify an unmanifested executable containing "update" as an
# installer and demand elevation. The algorithm harness requires no elevation.
HARNESS_EXECUTABLES = {"tree_updates": "tree_walk.exe"}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, default=ROOT /
                        "tools/vendor/ps2dev-20181019/MinGW/bin/gcc.exe")
    parser.add_argument("--harness", choices=sorted(HARNESS_SOURCES), action="append")
    args = parser.parse_args()
    compiler = args.compiler.resolve()
    if not compiler.is_file():
        parser.error("32-bit native GCC is missing; bootstrap the toolchain or pass --compiler")
    output = ROOT / "build/native/utilities"
    output.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env["PATH"] = str(compiler.parent) + os.pathsep + env.get("PATH", "")
    for harness in args.harness or sorted(HARNESS_SOURCES):
        executable = output / HARNESS_EXECUTABLES.get(harness, harness + ".exe")
        sources = [ROOT / "tests/native" / (harness + ".c")]
        sources += [ROOT / source for source in HARNESS_SOURCES[harness]]
        command = [str(compiler), "-m32", "-O2", "-Wall", "-Wextra",
                   "-fno-strict-aliasing", "-I", str(ROOT / "include"),
                   *HARNESS_FLAGS.get(harness, []),
                   *map(str, sources), "-lm", "-o", str(executable)]
        subprocess.run(command, env=env, cwd=ROOT, check=True)
        subprocess.run([str(executable)], env=env, cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
