"""Run route C and genuine road/goal/provider/registry/bounds/runtime as separate TUs."""
import os
import re
import hashlib
import json
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/actor_route.exe'
BUILD=ROOT/'build/actor_route/native'

def source_item(text,name):
    # Byte extraction preserves the published line endings and every byte of
    # each complete function. Only its known column-zero closing brace ends it.
    pattern=(rb'(?m)^(?:static __inline__ [^\n]*|(?:s32|const GeorgeMathVec3 \*)[^\n]*)'
        +name.encode()+rb'\([^;]*?\r?\n\{\r?\n.*?^\}\r?\n')
    matches=list(re.finditer(pattern,text,re.S))
    if len(matches)!=1:raise ValueError('ambiguous/missing exact published body '+name)
    return matches[0].group()

def helpers():
    # PE ld resolves unused COFF references before GC. Complete unchanged
    # published function bytes plus exact lexical dependencies form ignored
    # separate native TUs; whole source, byte spans and generated TUs are pinned.
    sources=[ROOT/'src/game/goal_methods.c',ROOT/'src/game/goal_methods5.c']
    text=[p.read_bytes() for p in sources]
    cafe=source_item(text[0],'func_001CAFE0');plane=source_item(text[1],'func_001CFBF8')
    lookup=source_item(text[1],'lookup_road');record=source_item(text[1],'record34')
    def macro(t,name):
        lines=t.splitlines(keepends=True)
        starts=[i for i,line in enumerate(lines) if line.startswith(('#define '+name+'(').encode())]
        if len(starts)!=1:raise ValueError('ambiguous/missing macro '+name)
        i=starts[0];out=lines[i]
        while lines[i].rstrip().endswith(bytes([92])):
            i+=1
            if i>=len(lines):raise ValueError('unterminated published macro')
            out+=lines[i]
        return out
    adjust=macro(text[0],'ADJUST_THIS');address=macro(text[1],'ADDRESS');field=macro(text[1],'FIELD')
    items=(('func_001CAFE0',0,cafe),('func_001CFBF8',1,plane),('lookup_road',1,lookup),
           ('record34',1,record),('ADJUST_THIS',0,adjust),('ADDRESS',1,address),('FIELD',1,field))
    outputs=[BUILD/'published_cafe.c',BUILD/'published_plane.c']
    outputs[0].write_bytes(b'#include "george/goals.h"\nextern const GeorgeMathVec3 *func_00120B68(void *);\n'+adjust+cafe)
    outputs[1].write_bytes(b'#include "george/goal_methods5.h"\nextern GeorgeGoalRoad *func_001CCA28(u32);\n'+address+field+lookup+record+plane)
    proof={'limitation':'Exact raw existing function bytes extracted into separate native TUs; whole goal TUs are not compiled.',
           'source_raw_sha256':{p.relative_to(ROOT).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
           'extractions':{},'generated_tus':{p.relative_to(ROOT).as_posix():{'size':p.stat().st_size,
               'raw_sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in outputs}}
    for name,i,data in items:
        if text[i].count(data)!=1:raise ValueError('ambiguous byte extraction '+name)
        proof['extractions'][name]={'source':sources[i].relative_to(ROOT).as_posix(),
            'byte_offset':text[i].find(data),'size':len(data),'raw_sha256':hashlib.sha256(data).hexdigest()}
    (BUILD/'published_source_extraction.json').write_text(json.dumps(proof,indent=2)+'\n')
    return outputs

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
    sources=['tests/native/actor_route.c','src/game/actor_route.c','src/game/road_queries.c','src/game/spatial_queries.c',
             'src/game/geometry_bounds.c','src/game/resource_registry.c','src/game/accessors.c',
             'src/game/vector_transform.c','src/game/vector_math.c']
    extracted=helpers()
    dependencies={}
    for source in extracted:
        dep=subprocess.run([str(BIN/'gcc.exe'),*flags,'-I',str(ROOT/'include'),'-M',str(source)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        if dep.returncode or dep.stderr:raise RuntimeError(dep.stdout+dep.stderr)
        dependencies[source.relative_to(ROOT).as_posix()]=dep.stdout
    (BUILD/'published_source_dependencies.json').write_text(json.dumps(dependencies,indent=2)+'\n')
    command=[str(BIN/'gcc.exe'),*flags,'-Wl,--gc-sections','-I',str(ROOT/'include'),
             *[str(ROOT/p) for p in sources],*[str(p) for p in extracted],str(BUILD/'ceilf.o'),str(BUILD/'float.o'),'-o',str(OUT)]
    result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')

if __name__=='__main__':main()
