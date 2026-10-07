"""Two separate native TUs; --production accepts isolated actual wrong sources."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=ROOT / 'build/native/actor_field_leaves')
    parser.add_argument('--production', type=Path, default=ROOT / 'src/game/actor_field_leaves.c')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--expect-failure', action='store_true')
    args = parser.parse_args()
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env['PATH'] = str(BIN) + os.pathsep + env['PATH']
    flags = [str(BIN / 'gcc.exe'), '-m32', '-O2', '-Wall', '-Wextra', '-Werror',
             '-fno-builtin', '-fno-strict-aliasing', '-I', str(ROOT / 'include')]
    commands, objects = [], []
    for name, source in (('harness', ROOT / 'tests/native/actor_field_leaves.c'),
                         ('production', args.production.resolve())):
        obj = out / (name + '.o')
        command = [*flags, '-c', str(source), '-o', str(obj)]
        result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
        commands.append(dict(command=command, returncode=result.returncode,
                             stdout=result.stdout, stderr=result.stderr))
        assert result.returncode == 0 and not result.stdout and not result.stderr, commands[-1]
        objects.append(str(obj))
    executable = out / 'checks.exe'
    command = [flags[0], '-m32', *objects, '-o', str(executable)]
    result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    commands.append(dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr))
    assert result.returncode == 0 and not result.stdout and not result.stderr, commands[-1]
    result = subprocess.run([str(executable)], cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    commands.append(dict(command=[str(executable)], returncode=result.returncode,
                         stdout=result.stdout, stderr=result.stderr))
    if args.expect_failure:
        assert result.returncode == 1 and 'fixture ' in result.stderr and not result.stdout, commands[-1]
        checks = None
    else:
        assert result.returncode == 0 and not result.stderr, commands[-1]
        checks = int(re.fullmatch(r'actor field leaves: (\d+) checks\n', result.stdout)[1])
    record = dict(commands=commands, native_checks=checks, actual_failure=args.expect_failure,
                  warning_free=True, separate_translation_units=2,
                  source_sha256=hashlib.sha256(args.production.read_bytes()).hexdigest(),
                  limit='GNU32 typed initialized scalar fixture bytes and lower64 results; no original class, original C interface, upper128, hardware, concurrency or portable out-of-range conversion claim.')
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(record, indent=2) + '\n')
    print(result.stderr if args.expect_failure else result.stdout, end='')


if __name__ == '__main__':
    main()
