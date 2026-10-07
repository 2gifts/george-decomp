"""Run collision nodes and their genuine published helper translation units."""
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT = ROOT / 'build/native/actor_collision.exe'


def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env['PATH'] = str(BIN) + os.pathsep + env['PATH']
    sources = ['tests/native/actor_collision.c', 'tests/native/actor_collision_unused.c', 'src/game/actor_collision.c',
               'src/game/pool_slots.c', 'src/game/vector_math.c', 'src/game/engine_angles.c',
               'src/game/segment_distance.c', 'src/game/spatial_queries.c',
               'src/game/geometry_bounds.c', 'src/game/vector_transform.c', 'src/game/goal_methods.c']
    command = [str(BIN / 'gcc.exe'), '-m32', '-O2', '-Wall', '-Wextra',
               '-fno-strict-aliasing', '-msse2', '-mfpmath=sse',
               '-ffunction-sections', '-fdata-sections', '-Wl,--gc-sections',
               '-I', str(ROOT / 'include'), *[str(ROOT / p) for p in sources], '-o', str(OUT)]
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr: raise RuntimeError(result.stdout + result.stderr)
    result = subprocess.run([str(OUT)], cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr: raise RuntimeError(result.stdout + result.stderr)
    print(result.stdout, end='')


if __name__ == '__main__': main()
