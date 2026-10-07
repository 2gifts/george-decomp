"""Compile and execute real plane-intersection C as a separate native TU."""
import os
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/plane_intersection.exe'


def main():
    OUT.parent.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    command=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing',
             '-msse2','-mfpmath=sse','-I',str(ROOT/'include'),
             str(ROOT/'tests/native/plane_intersection.c'),str(ROOT/'src/game/plane_intersection.c'),
             '-o',str(OUT),'-lm']
    result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')


if __name__=='__main__':main()
