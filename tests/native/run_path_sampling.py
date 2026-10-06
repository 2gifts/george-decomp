"""Compile and run the isolated five-body path sampling fixture harness."""
import os, subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/path_sampling.exe'

def main():
    OUT.parent.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    command=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing','-I',str(ROOT/'include'),
             str(ROOT/'tests/native/path_sampling.c'),str(ROOT/'src/game/path_sampling.c'),
             str(ROOT/'src/game/matrix_rigid.c'),'-o',str(OUT)]
    result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT)],cwd=ROOT,env=env,capture_output=True,text=True)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')

if __name__=='__main__':main()
