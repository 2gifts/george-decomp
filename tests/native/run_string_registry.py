"""Separate TUs: selected C, exact published heap/length spans, unchanged
published ctype/string sources and pinned generic string compatibility sources.
Core heap/free hooks are controlled; native helper reuse earns no recovery credit.
"""
import hashlib,json,os,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/string_registry'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    heap=(ROOT/'src/game/heap.c').read_bytes()
    begin=heap.index(b'#define DEFINE_ALLOCATION(');end=heap.index(b'\n#undef DEFINE_ALLOCATION',begin)
    # Preserve the exact complete macro and the two actual invocations, without
    # unrelated heap entry points or invented globals to satisfy their linker.
    macro_end=heap.index(b'\n\nDEFINE_ALLOCATION(',begin)
    macro=heap[begin:macro_end]
    invocations=[]
    for name in (b'func_002AEC28',):
        line=next(line for line in heap[macro_end:end].splitlines(keepends=True) if line.startswith(b'DEFINE_ALLOCATION('+name+b','))
        invocations.append(line)
    free_line=next(line for line in heap.splitlines(keepends=True) if line.startswith(b'GEORGE_DEFINE_FREE_FORWARD(func_002AEE40,'))
    generated=b'#include "george/heap.h"\n#include "george/function_templates.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+macro+b'\n'+b''.join(invocations)+b'#undef DEFINE_ALLOCATION\n'+free_line
    (OUT/'recovered_heap_wrappers.c').write_bytes(generated)
    (OUT/'source_reuse.json').write_text(json.dumps(dict(heap_published_source='src/game/heap.c',heap_raw_sha256=SHA(heap),
      macro_offset=begin,macro_size=len(macro),macro_raw_sha256=SHA(macro),
      complete_invocations=[dict(raw_sha256=SHA(line),raw_size=len(line),raw_offset=heap.index(line)) for line in invocations+[free_line]],
      generated_tu_sha256=SHA(generated),body_modified=False,core_heap_and_free_controlled=True),indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    import re
    published=(ROOT/'src/game/string_algorithms.c').read_bytes();begin=published.index(b'u32 func_00295050(');opening=published.index(b'{',begin);depth=1;end=opening+1
    while depth:
        b=published[end];depth+=(b==123)-(b==125);end+=1
    span=published[begin:end];generated=b'#include "george/string_algorithms.h"\n'+span+b'\n'
    (OUT/'recovered_length.c').write_bytes(generated)
    (OUT/'length_reuse.json').write_text(json.dumps(dict(source='src/game/string_algorithms.c',source_raw_sha256=SHA(published),span_offset=begin,span_bytes=len(span),span_raw_sha256=SHA(span),generated_sha256=SHA(generated),complete_function=True,unrelated_CRC_helpers_excluded_no_award=True),indent=2)+'\n')
    from analyze import validated_elf
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');declarations=[];records=[]
    for address,n in ((0x446FE8,2),(0x446FF0,2),(0x446FF8,7)):
        raw=original[address-0xFF000:address-0xFF000+n];declarations.append('const char D_%08X[]={%s};'%(address,','.join(str(b) for b in raw)))
        records.append(dict(address=hex(address),bytes=n,raw_sha256=SHA(raw),local_original_only_no_asset_or_data_award=True))
    (OUT/'local_bindings.c').write_text('#include "george/types.h"\n'+'\n'.join(declarations)+'\n',newline='\n')
    (OUT/'local_bindings.json').write_text(json.dumps(records,indent=2)+'\n')
    sources=[('test',ROOT/'tests/native/string_registry.c',[]),('selected',ROOT/'src/game/string_registry.c',[]),
      ('heap_wrappers',OUT/'recovered_heap_wrappers.c',[]),('length',OUT/'recovered_length.c',[]),('bindings',OUT/'local_bindings.c',[]),
      ('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]),
      ('strlwr',ROOT/'src/runtime/strlwr.c',['-I',str(EE),'-Dstrlwr=registry_real_strlwr']),
      ('tolower',ROOT/'src/runtime/tolower.c',['-I',str(EE)]),
      ('strstr',ROOT/'src/runtime/strstr.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dstrstr=registry_real_strstr'])]
    licensed=[]
    for name in ('memcpy','strcpy','strcat','strcmp'):
        path='ee/newlib/libc/string/'+name+'.c'
        raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':'+path],capture_output=True,check=True).stdout
        file=OUT/('generic_'+name+'.c');file.write_bytes(raw)
        sources.append((name,file,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Wno-parentheses','-D'+name+'=registry_real_'+name]))
        licensed.append(dict(revision=REVISION,path=path,raw_sha256=SHA(raw),unchanged=True,target_source_identity_or_award=False))
    (OUT/'licensed_support.json').write_text(json.dumps(licensed,indent=2)+'\n')
    commands=[];objects=[]
    for name,path,extra in sources:
        obj=OUT/(name+'.o');command=[*flags,*extra,'-c',str(path),'-o',str(obj)]
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
        assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
        objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    print(r.stdout,end='')

if __name__=='__main__':main()
