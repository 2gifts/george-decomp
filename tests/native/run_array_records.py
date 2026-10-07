"""Separate TUs: selected C, whole published accessors/qsort, exact heap
allocation/free macro spans and unchanged pinned generic memcpy. Controlled
heap/free hooks are in the test TU; helper reuse earns no new recovery credit.
"""
import hashlib,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/array_records'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    support=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':ee/newlib/libc/string/memcpy.c'],capture_output=True,check=True).stdout
    assert support==(ROOT/'tools/vendor/ps2-ee-toolchain/ee/newlib/libc/string/memcpy.c').read_bytes().replace(b'\r\n',b'\n')
    (OUT/'support_memcpy.c').write_bytes(support)
    heap=(ROOT/'src/game/heap.c').read_bytes()
    begin=heap.index(b'#define DEFINE_ALLOCATION(');end=heap.index(b'\n#undef DEFINE_ALLOCATION',begin)
    # Preserve the exact complete macro and the two actual invocations, without
    # unrelated heap entry points or invented globals to satisfy their linker.
    macro_end=heap.index(b'\n\nDEFINE_ALLOCATION(',begin)
    macro=heap[begin:macro_end]
    invocations=[]
    for name in (b'func_002AEB60',b'func_002AEC28'):
        line=next(line for line in heap[macro_end:end].splitlines(keepends=True) if line.startswith(b'DEFINE_ALLOCATION('+name+b','))
        invocations.append(line)
    free_line=next(line for line in heap.splitlines(keepends=True) if line.startswith(b'GEORGE_DEFINE_FREE_FORWARD(func_002AEE40,'))
    generated=b'#include "george/heap.h"\n#include "george/function_templates.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+macro+b'\n'+b''.join(invocations)+b'#undef DEFINE_ALLOCATION\n'+free_line
    (OUT/'recovered_heap_wrappers.c').write_bytes(generated)
    (OUT/'source_reuse.json').write_text(json.dumps(dict(heap_published_source='src/game/heap.c',heap_raw_sha256=SHA(heap),
      macro_offset=begin,macro_size=len(macro),macro_raw_sha256=SHA(macro),
      complete_invocations=[dict(raw_sha256=SHA(line),raw_size=len(line),raw_offset=heap.index(line)) for line in invocations+[free_line]],
      generated_tu_sha256=SHA(generated),body_modified=False,core_heap_and_free_controlled=True,
      memcpy_revision=REVISION,memcpy_raw_sha256=SHA(support),memcpy_native_compatibility_only=True,
      accessors_whole_source_sha256=SHA((ROOT/'src/game/accessors.c').read_bytes()),
      qsort_whole_source_sha256=SHA((ROOT/'src/runtime/qsort.c').read_bytes())),indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    sources=[('test',ROOT/'tests/native/array_records.c',[]),('selected',ROOT/'src/game/array_records.c',[]),
      ('heap_wrappers',OUT/'recovered_heap_wrappers.c',[]),('accessors',ROOT/'src/game/accessors.c',[]),
      ('memcpy',OUT/'support_memcpy.c',['-DPREFER_SIZE_OVER_SPEED','-Dmemcpy=array_support_memcpy','-I',str(EE)]),
      ('qsort',ROOT/'src/runtime/qsort.c',['-Wno-sign-compare','-Dqsort=array_real_qsort','-I',str(EE)])]
    commands=[];objects=[]
    for name,path,extra in sources:
        obj=OUT/(name+'.o');command=[*flags,*extra,'-c',str(path),'-o',str(obj)]
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
        assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
        objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    print(r.stdout,end='')

if __name__=='__main__':main()
