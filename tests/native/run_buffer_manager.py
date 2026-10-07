"""Compile selected functions with genuine supporting C; no helper award.

Length is a byte-exact complete published-function span; all count accessors,
strstr and ctype are unchanged whole published TUs. Generic pinned strncpy is
unchanged licensed source compiled with its genuine size-oriented macro. The
custom indexed-array observer and callbacks are explicit native controls.
"""
import hashlib,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/buffer_manager'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
def digest(data):return hashlib.sha256(data).hexdigest()
def main():
    OUT.mkdir(parents=True,exist_ok=True)
    support=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':ee/newlib/libc/string/strncpy.c'],capture_output=True,check=True).stdout
    assert digest(support)=='02668aeb2461c074f110b2cdda739cb0346a239196c23bf38f78db0917084da2'
    support_path=OUT/'support_strncpy.c';support_path.write_bytes(support)
    published_path=ROOT/'src/game/string_algorithms.c';published=published_path.read_bytes()
    begin=published.index(b'u32 func_00295050(');opening=published.index(b'{',begin);depth=1;end=opening+1
    while depth:
        byte=published[end];depth+=(byte==123)-(byte==125);end+=1
    lexical=published[begin:end];assert lexical.count(b'func_00295050')==1 and lexical.endswith(b'}')
    length_path=OUT/'recovered_strlen.c';length_path.write_bytes(b'#include "george/string_algorithms.h"\n'+lexical+b'\n')
    reuse=dict(published_source='src/game/string_algorithms.c',published_raw_sha256=digest(published),raw_offset=begin,raw_size=end-begin,
        entire_function_raw_sha256=digest(lexical),generated_TU_raw_sha256=digest(length_path.read_bytes()),
        entire_original_C_function_retained=True,body_modification=False,whole_published_TU_compiled=False,
        supporting_strncpy=dict(revision=REVISION,path='ee/newlib/libc/string/strncpy.c',raw_sha256=digest(support),
            unchanged_source=True,define='PREFER_SIZE_OVER_SPEED',target_source_identity_or_award=False),
        whole_published_TUs=[dict(path=p,raw_sha256=digest((ROOT/p).read_bytes())) for p in ('src/game/accessors.c','src/runtime/strstr.c','src/runtime/ctype_table.c')])
    (OUT/'reuse.json').write_text(json.dumps(reuse,indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-I',str(ROOT/'include')]
    sources=[('test',ROOT/'tests/native/buffer_manager.c',[]),('selected',ROOT/'src/game/buffer_manager.c',[]),
        ('length',length_path,['-Dfunc_00295050=manager_real_length']),
        ('count',ROOT/'src/game/accessors.c',['-Dfunc_002AAF88=manager_real_count']),
        ('search',ROOT/'src/runtime/strstr.c',['-Dstrstr=manager_real_strstr','-I',str(EE)]),
        ('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]),
        ('copy',support_path,['-DPREFER_SIZE_OVER_SPEED','-Dstrncpy=manager_real_strncpy','-I',str(EE)])]
    objects=[];commands=[]
    for name,source,extra in sources:
        obj=OUT/(name+'.o');cmd=[*flags,*extra,'-c',str(source),'-o',str(obj)]
        r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=cmd,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
        objects.append(str(obj))
    cmd=[flags[0],'-m32',*objects,'-o',str(OUT/'checks.exe')]
    r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=cmd,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr);print(r.stdout,end='')
if __name__=='__main__':main()
