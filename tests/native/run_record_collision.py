"""Run record queries with real inverse, normalization and segment-distance TUs."""
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT = ROOT / 'build/native/record_collision.exe'


def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env['PATH'] = str(BIN) + os.pathsep + env['PATH']
    command = [str(BIN / 'gcc.exe'), '-m32', '-O2', '-Wall', '-Wextra',
               '-fno-strict-aliasing', '-msse2', '-mfpmath=sse',
               '-I', str(ROOT / 'include'), str(ROOT / 'tests/native/record_collision.c'),
               str(ROOT / 'src/game/record_collision.c'), str(ROOT / 'src/game/matrix_rigid.c'),
               str(ROOT / 'src/game/vector_math.c'), str(ROOT / 'src/game/segment_distance.c'),
               '-o', str(OUT)]
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr:
        raise RuntimeError(result.stdout + result.stderr)
    result = subprocess.run([str(OUT)], cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr:
        raise RuntimeError(result.stdout + result.stderr)
    print(result.stdout, end='')


if __name__ == '__main__':
    main()
