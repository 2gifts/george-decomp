"""Compile the unchanged production setup TU and synthetic whole-arena harness."""
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT = ROOT / 'build/native/route_setup'


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env['PATH'] = str(BIN) + os.pathsep + env['PATH']
    flags = [str(BIN / 'gcc.exe'), '-m32', '-O2', '-Wall', '-Wextra', '-fno-strict-aliasing',
             '-msse2', '-mfpmath=sse', '-I', str(ROOT / 'include')]
    objects = []
    for name, source in (('test', 'tests/native/route_setup.c'), ('setup', 'src/game/route_setup.c')):
        obj = OUT / (name + '.o')
        objects.append(str(obj))
        result = subprocess.run([*flags, '-c', str(ROOT / source), '-o', str(obj)], cwd=ROOT,
                                env=env, capture_output=True, text=True, timeout=60)
        if result.returncode or result.stderr:
            raise RuntimeError(result.stdout + result.stderr)
    result = subprocess.run([flags[0], '-m32', *objects, '-o', str(OUT / 'checks.exe')], cwd=ROOT,
                            env=env, capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr:
        raise RuntimeError(result.stdout + result.stderr)
    result = subprocess.run([str(OUT / 'checks.exe')], cwd=ROOT, env=env,
                            capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr:
        raise RuntimeError(result.stdout + result.stderr)
    print(result.stdout, end='')


if __name__ == '__main__':
    main()
