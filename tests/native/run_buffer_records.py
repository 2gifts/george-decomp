"""Compile selected C, its test, a complete recovered length function and
the unchanged pinned supporting strncpy; no supporting helper gets an award.

The generic public source is read from the pinned local GNU checkout by Git blob
identity, written only into ignored build output, and compiled unchanged with
its genuine PREFER_SIZE_OVER_SPEED byte implementation. The target production
recipe does not use that macro or replace the original helper binding.
"""
import hashlib,json,os,subprocess
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
OUT=ROOT/'build/native/buffer_records'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    support=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':ee/newlib/libc/string/strncpy.c'],capture_output=True,check=True).stdout
    assert hashlib.sha256(support).hexdigest()=='02668aeb2461c074f110b2cdda739cb0346a239196c23bf38f78db0917084da2'
    support_path=OUT/'support_strncpy.c';support_path.write_bytes(support)
    # The PE linker resolves CRC references in unrelated functions even with
    # gc-sections. Preserve the complete length function's exact raw lexical
    # span, rather than providing invented CRC globals or modifying its body.
    published_path=ROOT/'src/game/string_algorithms.c';published=published_path.read_bytes()
    begin=published.index(b'u32 func_00295050(');opening=published.index(b'{',begin)
    depth=1;end=opening+1
    while depth:
        byte=published[end];depth+=(byte==123)-(byte==125);end+=1
    lexical=published[begin:end]
    assert lexical.count(b'func_00295050')==1 and lexical.endswith(b'}')
    length_path=OUT/'recovered_strlen.c'
    length_path.write_bytes(b'#include "george/string_algorithms.h"\n'+lexical+b'\n')
    (OUT/'length_reuse.json').write_text(json.dumps(dict(published_source='src/game/string_algorithms.c',
        published_raw_sha256=hashlib.sha256(published).hexdigest(),raw_offset=begin,raw_size=end-begin,
        entire_function_raw_sha256=hashlib.sha256(lexical).hexdigest(),output_raw_sha256=hashlib.sha256(length_path.read_bytes()).hexdigest(),
        entire_original_C_function_retained=True,body_modification=False,unrelated_crc_functions_not_executed=True),indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    sources=[('test',ROOT/'tests/native/buffer_records.c',[]),
        ('selected',ROOT/'src/game/buffer_records.c',[]),
        ('length',length_path,['-Dfunc_00295050=buffer_real_strlen']),
        ('support',support_path,['-DPREFER_SIZE_OVER_SPEED','-Dstrncpy=buffer_support_strncpy','-I',str(EE)])]
    objects=[];commands=[]
    for name,source,extra in sources:
        obj=OUT/(name+'.o');command=[*flags,*extra,'-c',str(source),'-o',str(obj)]
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
        assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
        objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    print(r.stdout,end='')

if __name__=='__main__':main()
