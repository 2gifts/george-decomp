"""Compile genuine road/provider/registry/bounds and GNU float runtime sources."""
import os
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/road_queries.exe'
BUILD=ROOT/'build/road_queries/native'

def main():
    BUILD.mkdir(parents=True,exist_ok=True);OUT.parent.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=['-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing',
           '-msse2','-mfpmath=sse','-ffunction-sections','-fdata-sections']
    runtime=[('ceilf','src/runtime/ceilf.c',[
        '-I',str(ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/sys-include'),
        '-Dceilf=func_0037AF00']),('float','src/runtime/fp_bit.c',[
        '-DFINE_GRAINED_LIBRARIES','-DFLOAT','-DUS_SOFTWARE_GOFAST',
        '-DFLOAT_BIT_ORDER_MISMATCH','-DNO_DENORMALS','-DL_unpack_sf','-DL_sf_to_usi',
        '-Dfptoui=func_00374748'])]
    for name,source,extra in runtime:
        command=[str(BIN/'gcc.exe'),*flags,*extra,'-c',str(ROOT/source),'-o',str(BUILD/(name+'.o'))]
        result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    sources=['tests/native/road_queries.c','src/game/road_queries.c','src/game/spatial_queries.c',
             'src/game/geometry_bounds.c','src/game/resource_registry.c','src/game/accessors.c',
             'src/game/vector_transform.c','src/game/vector_math.c']
    command=[str(BIN/'gcc.exe'),*flags,'-Wl,--gc-sections','-I',str(ROOT/'include'),
             *[str(ROOT/p) for p in sources],str(BUILD/'ceilf.o'),str(BUILD/'float.o'),'-o',str(OUT)]
    result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')

if __name__=='__main__':main()
