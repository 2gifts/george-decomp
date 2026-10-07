"""Execute the complete unchanged libio allocation primary in initialized native observers."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def run(output, primary=None, only_fixture=None):
    output = output.resolve()
    assert output.is_relative_to(ROOT) and output != ROOT
    output.mkdir(parents=True, exist_ok=True)
    binary = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'
    gcc = binary / 'gcc.exe'
    env = dict(os.environ)
    env['PATH'] = str(binary) + os.pathsep + env['PATH']
    flags = ['-m32', '-O2', '-Wall', '-Wextra', '-fno-builtin', '-fno-strict-aliasing',
             '-ffunction-sections', '-fdata-sections',
             '-I', str(primary.parent if primary is not None else ROOT / 'src/runtime/libio'),
             '-I', str(ROOT / 'src/runtime/libio'),
             '-I', str(ROOT / 'src/runtime/sgi/libio'),
             '-I', str(ROOT / 'src/runtime/sgi/config'),
             '-I', str(ROOT / 'tests/native')]
    if only_fixture is not None:
        flags += ['-DLIBIO_ALLOCATOR_ONLY_FIXTURE=%d' % only_fixture]
    obj = output / 'native.o'
    exe = output / 'checks.exe'
    commands = []
    for command in ([str(gcc), *flags, '-c', str(ROOT / 'tests/native/libio_allocator.c'), '-o', str(obj)],
                    [str(gcc), '-m32', '-Wl,--gc-sections', str(obj), '-o', str(exe)], [str(exe)]):
        if '-o' in command:
            assert Path(command[command.index('-o') + 1]).resolve().parent == output
        r = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, timeout=120)
        commands.append(dict(command=command, returncode=r.returncode,
            stdout=r.stdout.decode(errors='replace'), stderr=r.stderr.decode(errors='replace')))
        (output / 'commands.json').write_text(json.dumps(commands, indent=2) + '\n', newline='\n')
        if r.returncode:
            return r
    return r


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'build/native/libio_allocator')
    args = parser.parse_args()
    result = run(args.output)
    print(result.stdout.decode(errors='replace'), end='')
    if result.returncode:
        print(result.stderr.decode(errors='replace'), end='')
    assert result.returncode == 0
