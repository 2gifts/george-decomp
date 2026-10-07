"""Run real selected and normalization C as separately compiled native TUs."""
import os
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/segment_intersection.exe'


def main():
    OUT.parent.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    prefix=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing',
            '-msse2','-mfpmath=sse','-I',str(ROOT/'include')]
    normal=OUT.with_suffix('.normalize.o')
    commands=[[*prefix,'-Dfunc_002A3538=intersection_native_normalize','-c',str(ROOT/'src/game/vector_math.c'),'-o',str(normal)],
              [*prefix,str(ROOT/'tests/native/segment_intersection.c'),str(ROOT/'src/game/segment_intersection.c'),
               str(normal),'-o',str(OUT),'-lm']]
    for command in commands:
        result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')


if __name__=='__main__':main()
