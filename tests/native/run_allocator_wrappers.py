"""Compile the two adapted production files as separate 32-bit native TUs.

The four authored compatibility headers expose only the wrapper interfaces; no
native layout or allocation implementation is substituted for the target ABI
proof. Symbol renaming avoids replacing the host C runtime's realloc function.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BIN = ROOT / 'tools/vendor/ps2dev-20181019/MinGW/bin'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, default=ROOT / 'build/native/allocator_wrappers')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--source-dir', type=Path, default=ROOT / 'src/runtime/stdlib',
                        help='explicit authored negative-control source directory')
    args = parser.parse_args()
    out = args.output_dir.resolve()
    headers = out / 'include'
    headers.mkdir(parents=True, exist_ok=True)
    compatibility = {
        '_ansi.h': '#ifndef AW_ANSI_H\n#define AW_ANSI_H\n#define _PTR void *\n#define _AND ,\n#define _PARAMS(args) args\n#define _DEFUN(name,args,decls) name(decls)\n#endif\n',
        'reent.h': '#ifndef AW_REENT_H\n#define AW_REENT_H\n#include <_ansi.h>\nstruct _reent;\nextern struct _reent *_impure_ptr;\n#define _REENT _impure_ptr\n#endif\n',
        'stdlib.h': '#ifndef AW_STDLIB_H\n#define AW_STDLIB_H\n#include <stddef.h>\nvoid *realloc(void *, size_t);\n#endif\n',
        'malloc.h': '#ifndef AW_MALLOC_H\n#define AW_MALLOC_H\n#include <stddef.h>\n#include <reent.h>\nvoid *memalign(size_t, size_t);\nvoid *_memalign_r(struct _reent *, size_t, size_t);\nvoid *_realloc_r(struct _reent *, void *, size_t);\n#endif\n',
    }
    for name, text in compatibility.items():
        (headers / name).write_text(text, newline='\n')
    executable = out / 'allocator_wrappers.exe'
    env = os.environ.copy()
    env['PATH'] = str(BIN) + os.pathsep + env['PATH']
    command = [str(BIN / 'gcc.exe'), '-m32', '-O2', '-Wall', '-Wextra', '-Werror',
               '-fno-builtin', '-Dmemalign=george_memalign', '-Drealloc=george_realloc',
               '-I', str(headers), str(ROOT / 'tests/native/allocator_wrappers.c'),
               str(args.source_dir.resolve() / 'malign.c'), str(args.source_dir.resolve() / 'realloc.c'),
               '-o', str(executable)]
    cp = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    if cp.returncode or cp.stdout or cp.stderr:
        raise RuntimeError(cp.stdout + cp.stderr)
    result = subprocess.run([str(executable)], cwd=ROOT, env=env, capture_output=True, text=True, timeout=60)
    if result.returncode or result.stderr:
        raise RuntimeError(result.stdout + result.stderr)
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(dict(command=command, stdout=result.stdout, stderr=result.stderr,
            returncode=result.returncode, compatibility_headers=compatibility,
            limit='Separate production TUs, authored allocation/lock observers; no heap, OS, thread or full target execution claim.'), indent=2) + '\n')
    print(result.stdout, end='')


if __name__ == '__main__':
    main()
