"""Actual selected C plus exact raw published helper-function reuse as native TUs.

The PE linker requires the lexical closure, rather than whole unrelated goal
translation units. Complete function/macro bytes are preserved and recorded;
whole original source hashes and real compiler -M outputs identify extraction.
"""
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
BUILD=ROOT/'build/actor_route_init/native'
OUT=ROOT/'build/native/actor_route_init.exe'

def source_item(data,name):
    pattern=rb'(?m)^[^\n;{}]*\b'+name.encode()+rb'\([^;]*?\r?\n\{\r?\n.*?^\}\r?\n'
    found=list(re.finditer(pattern,data,re.S))
    if len(found)!=1:raise ValueError('missing/ambiguous complete published source '+name)
    return found[0].group()

def macro(data,name):
    lines=data.splitlines(keepends=True)
    starts=[i for i,line in enumerate(lines) if line.startswith(('#define '+name+'(').encode())]
    if len(starts)!=1:raise ValueError('missing/ambiguous published macro '+name)
    i=starts[0];out=lines[i]
    while lines[i].rstrip().endswith(bytes([92])):
        i+=1
        if i>=len(lines):raise ValueError('unterminated macro')
        out+=lines[i]
    return out

def helpers():
    plans=[('provider','src/game/road_queries.c',b'#include "george/road_queries.h"\n',
            [('macro','AT'),('macro','FIELD'),('function','func_001CC710')]),
           ('bounds','src/game/geometry_bounds.c',b'#include "george/geometry_bounds.h"\n',
            [('function','func_002A00A0'),('function','func_002A0390')]),
           ('position','src/game/goal_methods.c',b'#include "george/goals.h"\nextern const GeorgeMathVec3 *func_00120B68(void *);\n',
            [('macro','ADJUST_THIS'),('function','func_001CAFE0')]),
           ('movement','src/game/actor_movement.c',b'#include "george/actor_movement.h"\nextern void func_0018D8A0(GeorgeGoalEntity *,const GeorgeMathVec3 *);\nextern void func_0018FD30(GeorgeGoalEntity *,GeorgeActorBits64);\nextern void func_0018FD40(GeorgeGoalEntity *,GeorgeActorBits64);\n',
            [('macro','ADDRESS'),('macro','FIELD'),('macro','VECTOR'),('macro','ADJUST'),
             ('function','actor_project'),('function','func_00177E48')]),
           ('queue','src/game/actor_route.c',b'#include "george/actor_route.h"\n',
            [('macro','AT'),('macro','FIELD'),('function','func_001CC628')]),
           ('scale','src/game/vector_math.c',b'#include "george/vector_math.h"\n#include "george/ee_math.h"\n',
            [('macro','LENGTH3'),('function','func_002A35C0')])]
    proof={'limit':'Entire published helper-function and exact macro bytes in ignored separate TUs; whole original goal/road/movement TUs are not compiled. No source normalization or proxy helper implementations.',
           'source_raw_sha256':{},'extractions':{},'generated_tus':{}}
    sources=[]
    for label,path,prefix,items in plans:
        data=(ROOT/path).read_bytes();output=prefix
        proof['source_raw_sha256'][path]=hashlib.sha256(data).hexdigest()
        for kind,name in items:
            part=(macro if kind=='macro' else source_item)(data,name)
            if data.count(part)!=1:raise ValueError('ambiguous raw source span')
            output+=part
            proof['extractions'][label+':'+name]={'source':path,'kind':kind,'byte_offset':data.find(part),
                                                 'size':len(part),'raw_sha256':hashlib.sha256(part).hexdigest()}
        dest=BUILD/('published_'+label+'.c');dest.write_bytes(output);sources.append(dest)
        proof['generated_tus'][dest.relative_to(ROOT).as_posix()]={'size':len(output),'raw_sha256':hashlib.sha256(output).hexdigest()}
    (BUILD/'published_source_extraction.json').write_text(json.dumps(proof,indent=2)+'\n')
    return sources

def main():
    BUILD.mkdir(parents=True,exist_ok=True);OUT.parent.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=['-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing',
           '-msse2','-mfpmath=sse','-ffunction-sections','-fdata-sections']
    sources=[ROOT/p for p in ('tests/native/actor_route_init.c','src/game/actor_route_init.c',
                             'src/game/route_setup.c')]+helpers()
    dependencies={}
    objects=[]
    for index,source in enumerate(sources):
        aliases=[]
        if source.name=='route_setup.c':aliases=['-Dfunc_0020BEA8=route_init_published_setup']
        if source.name=='published_movement.c':aliases=['-Dfunc_00177E48=route_init_published_movement']
        command=[str(BIN/'gcc.exe'),*flags,*aliases,'-I',str(ROOT/'include'),'-M',str(source)]
        result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
        dependencies[source.relative_to(ROOT).as_posix()]={'command':command,'actual_M_output':result.stdout}
        obj=BUILD/(str(index)+'_'+source.stem+'.o');objects.append(obj)
        result=subprocess.run([str(BIN/'gcc.exe'),*flags,*aliases,'-I',str(ROOT/'include'),'-c',str(source),'-o',str(obj)],
                              cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    (BUILD/'dependencies.json').write_text(json.dumps(dependencies,indent=2)+'\n')
    command=[str(BIN/'gcc.exe'),*flags,'-Wl,--gc-sections','-I',str(ROOT/'include'),
             *[str(p) for p in objects],'-lm','-o',str(OUT)]
    result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    result=subprocess.run([str(OUT)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    if result.returncode or result.stderr:raise RuntimeError(result.stdout+result.stderr)
    print(result.stdout,end='')

if __name__=='__main__':main()
