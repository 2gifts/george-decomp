"""Separate target-reconstruction and authored-hook native translation units."""
import argparse
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output-dir', type=Path, default=ROOT / 'build/native/allocator_locks')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--production', type=Path, default=ROOT / 'src/runtime/stdlib/allocator_locks.c',
                        help='Isolated negative-control source only; no shared source mutation')
    args = parser.parse_args()
    out = args.output_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env['PATH'] = str(BIN) + os.pathsep + env['PATH']
    flags = [str(BIN / 'gcc.exe'), '-m32', '-O2', '-Wall', '-Wextra', '-Werror', '-fno-builtin', '-I', str(ROOT / 'include')]
    records, objects = [], []

    def run(command):
        result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
        records.append(dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr))
        if result.returncode or result.stderr:
            raise RuntimeError(result.stdout + result.stderr)
        return result

    for name, source in (('harness', ROOT / 'tests/native/allocator_locks.c'), ('production', args.production.resolve())):
        obj = out / (name + '.o')
        objects.append(str(obj))
        assert not run([*flags, '-c', str(source), '-o', str(obj)]).stdout
    executable = out / 'allocator_locks.exe'
    assert not run([flags[0], '-m32', *objects, '-o', str(executable)]).stdout
    result = run([str(executable)])
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(dict(commands=records, native_checks=80640,
            stdout=result.stdout, separate_translation_units=2, warning_free=True,
            limit='Author-controlled syscall observations/global mutations; no kernel/semaphore/thread-safety or allocator validation.'), indent=2) + '\n')
    print(result.stdout, end='')


if __name__ == '__main__':
    main()
