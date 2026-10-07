"""Run the real frustum and published normalizer as separate native TUs."""
import os
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/geometry_frustum'


def main():
    OUT.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing',
           '-msse2','-mfpmath=sse','-I',str(ROOT/'include')]
    objects=[]
    for name,source,extra in (
        ('test','tests/native/geometry_frustum.c',[]),
        ('constructor','src/game/geometry_frustum.c',[]),
        ('normalize','src/game/vector_math.c',['-Dfunc_002A3538=frustum_native_normalize']),
    ):
        obj=OUT/(name+'.o');objects.append(str(obj))
        result=subprocess.run([*flags,*extra,'-c',str(ROOT/source),'-o',str(obj)],cwd=ROOT,env=env,
                              capture_output=True,text=True,timeout=60)
        if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([flags[0],'-m32',*objects,'-o',str(OUT/'checks.exe'),'-lm'],cwd=ROOT,env=env,
                          capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')


if __name__=='__main__':main()
