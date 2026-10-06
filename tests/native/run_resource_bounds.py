"""Run separate-TU resource bounds and real group helpers on authored fixtures."""
from pathlib import Path
import argparse
import ctypes
import os
import subprocess

ROOT=Path(__file__).resolve().parents[2]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--compiler',type=Path,default=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin/gcc.exe')
    compiler=p.parse_args().compiler.resolve()
    if not compiler.is_file():p.error('32-bit native compiler missing; bootstrap toolchain or pass --compiler')
    output=ROOT/'build/native/resource_bounds';output.mkdir(parents=True,exist_ok=True)
    executable=output/'semantic_harness.exe';env=os.environ.copy()
    env['PATH']=str(compiler.parent)+os.pathsep+env.get('PATH','')
    if os.name=='nt':ctypes.windll.kernel32.SetErrorMode(3)
    subprocess.run([str(compiler),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing','-I',str(ROOT/'include'),
                    str(ROOT/'tests/native/resource_bounds.c'),str(ROOT/'src/game/resource_bounds.c'),
                    str(ROOT/'src/game/resource_groups.c'),'-o',str(executable)],cwd=ROOT,env=env,check=True)
    subprocess.run([str(executable)],cwd=ROOT,env=env,check=True,timeout=30)

if __name__=='__main__':main()
