"""Exact complete production function spans plus licensed native-only support.
Full optimized retail strings execute in the independent original observer;
generic bytewise native compatibility code earns no retail helper credit.
"""
import hashlib,json,os,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/file_archive'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()
def main():
    OUT.mkdir(parents=True,exist_ok=True);commands=[]
    heap=(ROOT/'src/game/heap.c').read_bytes();begin=heap.index(b'#define DEFINE_ALLOCATION(');end=heap.index(b'\n\nDEFINE_ALLOCATION(',begin)
    macro=heap[begin:end];line=next(x for x in heap[end:].splitlines(keepends=True) if x.startswith(b'DEFINE_ALLOCATION(func_002AEC28,'))
    generated=b'#include "george/heap.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+macro+b'\n'+line+b'#undef DEFINE_ALLOCATION\n'
    (OUT/'recovered_heap_wrapper.c').write_bytes(generated)
    (OUT/'source_reuse.json').write_text(json.dumps(dict(source='src/game/heap.c',source_raw_sha256=SHA(heap),macro_offset=begin,macro_bytes=len(macro),macro_raw_sha256=SHA(macro),invocation_offset=heap.index(line),invocation_bytes=len(line),invocation_raw_sha256=SHA(line),generated_sha256=SHA(generated),body_modified=False,allocation_core_controlled=True),indent=2)+'\n')
    from analyze import validated_elf
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');declarations=[];records=[]
    for address,n in ((0x446FE8,2),(0x446FF0,2),(0x446FF8,7),(0x4473C0,8),(0x445650,1024)):
        raw=original[address-0xFF000:address-0xFF000+n]
        if n==1024:
            words=[int.from_bytes(raw[i:i+4],'little') for i in range(0,n,4)]
            for i,w in enumerate(words):
                c=i
                for unused in range(8):c=(c>>1)^(0xEDB88320 if c&1 else 0)
                assert c==w
            declarations.append('const u32 D_%08X[]={%s};'%(address,','.join('0x%08Xu'%w for w in words)))
        else:declarations.append('const char D_%08X[]={%s};'%(address,','.join(str(b) for b in raw)))
        records.append(dict(address=hex(address),bytes=n,raw_sha256=SHA(raw),whole_original_data_no_asset_or_award=True))
    (OUT/'local_bindings.c').write_text('#include "george/types.h"\n'+'\n'.join(declarations)+'\n',newline='\n')
    (OUT/'local_bindings.json').write_text(json.dumps(records,indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    # Whole old-MinGW COFF TUs retain unresolved references from unrelated
    # functions despite --gc-sections. Preserve that genuine failed attempt,
    # then reuse complete lexical bodies/macros without edits or dummy globals.
    reuse=[]
    def span(path,signature):
        raw=(ROOT/path).read_bytes();start=raw.index(signature);opening=raw.index(b'{',start);finish=opening+1;depth=1
        while depth:
            byte=raw[finish];depth+=(byte==123)-(byte==125);finish+=1
        selected=raw[start:finish];reuse.append(dict(source=path,source_raw_sha256=SHA(raw),span_offset=start,span_bytes=len(selected),span_raw_sha256=SHA(selected),complete_function=True,body_modified=False))
        return selected
    close=span('src/game/file_operations.c',b'void func_002B1198(')
    resolver=span('src/game/string_registry.c',b'void func_002ABAE8(')
    raw=(ROOT/'src/game/string_registry.c').read_bytes();macro=next(x for x in raw.splitlines(keepends=True) if x.startswith(b'#define REGISTRY_PAIR('))
    reuse.append(dict(source='src/game/string_registry.c',source_raw_sha256=SHA(raw),span_offset=raw.index(macro),span_bytes=len(macro),span_raw_sha256=SHA(macro),complete_macro=True,body_modified=False))
    length=span('src/game/string_algorithms.c',b'u32 func_00295050(');crc=span('src/game/string_algorithms.c',b'u32 func_0029C648(')
    raw=(ROOT/'src/game/deimos_tables.c').read_bytes();line=next(x for x in raw.splitlines(keepends=True) if x.startswith(b'GEORGE_DEFINE_MAP_LOOKUP(func_002A7C08)'))
    reuse.append(dict(source='src/game/deimos_tables.c',source_raw_sha256=SHA(raw),span_offset=raw.index(line),span_bytes=len(line),span_raw_sha256=SHA(line),complete_macro_invocation=True,body_modified=False))
    blocks={
      'file_operations':b'#include "george/file_operations.h"\nextern s32 func_00363AE0(s32);\nextern s32 func_00368C40(s32);\n'+close+b'\n',
      'registry':b'#include "george/string_registry.h"\n#include "george/heap.h"\nextern char *func_00393B74(char *,const char *);\nextern char *func_00393758(char *,const char *);\nextern s32 func_00393A28(const char *,const char *);\nextern char *func_003984D8(char *);\nextern char *func_00398628(const char *,const char *);\n'+macro+resolver+b'\n#undef REGISTRY_PAIR\n',
      'crc_length':b'#include "george/string_algorithms.h"\n'+length+b'\n'+crc+b'\n',
      'map':b'#include "george/deimos_tables.h"\n#include "george/algorithm_templates.h"\n'+line}
    for name,data in blocks.items():(OUT/('reused_'+name+'.c')).write_bytes(data)
    (OUT/'complete_span_reuse.json').write_text(json.dumps(dict(spans=reuse,generated=[dict(path='reused_'+name+'.c',raw_sha256=SHA(data)) for name,data in blocks.items()],support_only_no_additional_awards=True,whole_tu_failure_preserved='build/file_archive/whole_tu_failure.json'),indent=2)+'\n')
    sources=[('test',ROOT/'tests/native/file_archive.c',[]),('selected',ROOT/'src/game/file_archive.c',[]),
      ('file_operations',OUT/'reused_file_operations.c',[]),('registry',OUT/'reused_registry.c',[]),
      ('crc_length',OUT/'reused_crc_length.c',[]),('map',OUT/'reused_map.c',[]),
      ('heap_wrapper',OUT/'recovered_heap_wrapper.c',[]),('bindings',OUT/'local_bindings.c',[]),
      ('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]),
      ('strlwr',ROOT/'src/runtime/strlwr.c',['-I',str(EE),'-Dstrlwr=archive_real_strlwr']),
      ('tolower',ROOT/'src/runtime/tolower.c',['-I',str(EE)]),
      ('strstr',ROOT/'src/runtime/strstr.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dstrstr=archive_real_strstr'])]
    licensed=[]
    for name in ('memcpy','strcpy','strcat','strcmp'):
        path='ee/newlib/libc/string/'+name+'.c'
        raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':'+path],capture_output=True,check=True).stdout
        file=OUT/('generic_'+name+'.c');file.write_bytes(raw)
        sources.append((name,file,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Wno-parentheses','-D'+name+'=archive_real_'+name]))
        licensed.append(dict(revision=REVISION,path=path,raw_sha256=SHA(raw),unchanged=True,native_support_only_no_helper_identity_or_award=True))
    (OUT/'licensed_support.json').write_text(json.dumps(licensed,indent=2)+'\n')
    objects=[]
    for name,path,extra in sources:
        obj=OUT/(name+'.o');command=[*flags,*extra,'-c',str(path),'-o',str(obj)]
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
        objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60);commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr);print(r.stdout,end='')
if __name__=='__main__':main()
