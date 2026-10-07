"""Build real selected close/copy and unchanged published flush/init/read C.

The native-only fread memcpy bridge has the authentic pointer-return prototype
and executes the selected void copy; its NULL result is discarded by fread.
Seek, refill and allocator paths are outside these fixtures and fail if called.
"""
import json,os,subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/stdio_close'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing',
        '-ffunction-sections','-fdata-sections','-I',str(ROOT/'src/runtime/stdlib/compat'),
        '-I',str(EE),'-I',str(ROOT/'include'),'-I',str(ROOT/'src/runtime/stdio')]
    rename=['-Dfclose=func_003941D8','-Dfflush=stdio_real_fflush','-D__sinit=stdio_real_sinit',
        '-Dfread=stdio_real_fread','-D_impure_ptr=D_00405694','-D_cleanup_r=stdio_cleanup_r',
        '-D_cleanup=stdio_cleanup','-D_fwalk=stdio_real_fwalk','-D__sfp=stdio_real_sfp',
        '-D__sfmoreglue=stdio_real_sfmoreglue']
    records=[];objects=[]
    for name,source in [('test','tests/native/stdio_close.c'),('close','src/runtime/stdio/fclose.c'),
        ('copy','src/runtime/stdio/copy_bytes.c'),('flush','src/runtime/stdio/fflush.c'),
        ('init','src/runtime/stdio/findfp.c'),('walk','src/runtime/stdio/fwalk.c'),('read','src/runtime/stdio/fread.c')]:
        obj=OUT/(name+'.o');extra=[]
        if name=='init':extra=['-Wno-missing-parameter-type'] # unchanged K&R flags/file rely on default int.
        if name=='read':extra=['-Dmemcpy=stdio_observe_fread_copy','-Wno-sign-compare'] # unchanged resid/r comparison.
        command=[*flags,*rename,*extra,'-c',str(ROOT/source),'-o',str(obj)]
        result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        records.append(dict(command=command,stdout=result.stdout,stderr=result.stderr,exit_code=result.returncode))
        if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
        objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    records.append(dict(command=command,stdout=result.stdout,stderr=result.stderr,exit_code=result.returncode))
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    records.append(dict(command=[str(OUT/'checks.exe')],stdout=result.stdout,stderr=result.stderr,exit_code=result.returncode))
    (OUT/'commands.json').write_text(json.dumps(records,indent=2)+'\n')
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')

if __name__=='__main__':main()
