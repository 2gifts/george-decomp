"""Build focused native 32-bit utility harnesses, without original game files."""
import argparse
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HARNESS_SOURCES = {
    "arena_ownership": ["src/game/render_records.c", "src/game/arena_ownership.c", "src/game/arena_buffers.c"],
    "arena_extensions": ["src/game/arena_extensions.c"],
    "resource_base": ["src/game/resource_base.c"],
    "arena_buffers": ["src/game/arena_buffers.c"],
    "resource_groups": ["src/game/resource_groups.c"],
    "property_management": ["src/game/property_management.c", "src/game/property_lifecycle.c"],
    "property_updates": ["src/game/property_updates.c", "src/game/property_lifecycle.c"],
    "resource_lifecycle": ["src/game/resource_lifecycle.c"],
    "property_lifecycle": ["src/game/property_lifecycle.c"],
    "property_hierarchy": ["src/game/property_hierarchy.c"],
    "geometry_classify6": ["src/game/geometry_classify6.c"],
    "property_pack": ["src/game/property_pack.c"],
    "matrix_rigid": ["src/game/matrix_rigid.c"],
    "property_records": ["src/game/property_records.c"],
    "camera_basis": ["src/game/camera_basis.c"],
    "geometry_classify": ["src/game/geometry_classify.c"],
    "camera_transform": ["src/game/camera_transform.c"],
    "camera_motion": ["src/game/camera_motion.c"],
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
    "geometry_classify6": ["-ffloat-store"],
    "matrix_rigid": ["-ffloat-store"],
    "property_records": ["-ffloat-store"],
    "camera_basis": ["-ffloat-store"],
    "geometry_classify": ["-ffloat-store"],
    "camera_transform": ["-ffloat-store"],
    "camera_motion": ["-ffloat-store"],
    "engine_angles": ["-ffloat-store"],
    "tree_updates": ["-DGEORGE_TREE_NATIVE_COUNTER"],
}
# Windows may classify an unmanifested executable containing "update" as an
# installer and demand elevation. The algorithm harness requires no elevation.
HARNESS_EXECUTABLES = {"tree_updates": "tree_walk.exe", "property_updates": "property_tick.exe"}


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
