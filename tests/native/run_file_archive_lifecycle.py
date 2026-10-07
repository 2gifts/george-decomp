"""Exact recovered source closures and unchanged pinned native string support.

No native helper body is equated to unawarded retail/SDK code. The approved
160-byte heap creator is an explicitly disclosed source-order bridge.
"""
import hashlib,json,os,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/file_archive_lifecycle'
SELECTED=ROOT/'src/game/file_archive_lifecycle.c'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()
def main():
    OUT.mkdir(parents=True,exist_ok=True);commands=[];reuse=[]
    def record(path,raw,start,end,kind):
        data=raw[start:end];reuse.append(dict(source=path,source_raw_sha256=SHA(raw),offset=start,bytes=len(data),raw_sha256=SHA(data),kind=kind,body_modified=False));return data
    def span(path,signature):
        raw=(ROOT/path).read_bytes();start=raw.index(signature);opening=raw.index(b'{',start);finish=opening+1;depth=1
        while depth:
            byte=raw[finish];depth+=(byte==123)-(byte==125);finish+=1
        return record(path,raw,start,finish,'complete_function')
    hp='src/game/heap.c';heap=(ROOT/hp).read_bytes();start=heap.index(b'#define DEFINE_ALLOCATION(');end=heap.index(b'\n\nDEFINE_ALLOCATION(',start)
    macro=record(hp,heap,start,end,'complete_macro');lines=[]
    for name in (b'func_002AEB60',b'func_002AEC28'):
        line=next(x for x in heap.splitlines(keepends=True) if x.startswith(b'DEFINE_ALLOCATION('+name+b','));lines.append(record(hp,heap,heap.index(line),heap.index(line)+len(line),'complete_macro_invocation'))
    free_line=next(x for x in heap.splitlines(keepends=True) if x.startswith(b'GEORGE_DEFINE_FREE_FORWARD(func_002AEE40,'))
    free_line=record(hp,heap,heap.index(free_line),heap.index(free_line)+len(free_line),'complete_macro_invocation')
    blocks={
      'heap':b'#include "george/heap.h"\n#include "george/function_templates.h"\nextern u32 D_003FD200;\nextern GeorgeHeap *D_003FD204,*D_00469B80[];\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+span(hp,b'void func_002AEAF0(')+b'\n'+macro+b'\n'+b''.join(lines)+b'#undef DEFINE_ALLOCATION\n'+free_line,
      'map':b'#include "george/deimos_tables.h"\n#include "george/heap.h"\n'+span('src/game/deimos_tables.c',b'void func_002A7CD0(')+b'\n',
      'crc_length':b'#include "george/string_algorithms.h"\n'+span('src/game/string_algorithms.c',b'u32 func_00295050(')+b'\n'+span('src/game/string_algorithms.c',b'u32 func_0029C648(')+b'\n'}
    for name,data in blocks.items():(OUT/('reused_'+name+'.c')).write_bytes(data)
    (OUT/'complete_span_reuse.json').write_text(json.dumps(dict(spans=reuse,generated=[dict(path='reused_'+name+'.c',raw_sha256=SHA(data)) for name,data in blocks.items()],whole_tu_compilation_claimed=False,helper_source_only_no_new_award=True,heap_creator_native_only_bridge=True),indent=2)+'\n')
    from analyze import validated_elf
    _,original=validated_elf(ROOT/'orig/SLUS_216.68');records=[];declarations=[]
    for address,n in ((0x4471F8,1),(0x447238,37),(0x447398,1),(0x4473C8,1),(0x4473D0,2),(0x4473D8,2),(0x4473E0,6),(0x4473E8,9),(0x4473F8,19),(0x447410,20),(0x447428,20),(0x445650,1024)):
        raw=original[address-0xFF000:address-0xFF000+n]
        if n==1024:
            words=[int.from_bytes(raw[i:i+4],'little') for i in range(0,n,4)]
            for i,w in enumerate(words):
                c=i
                for unused in range(8):c=(c>>1)^(0xEDB88320 if c&1 else 0)
                assert c==w
            declarations.append('const u32 D_%08X[]={%s};'%(address,','.join('0x%08Xu'%w for w in words)))
        elif address==0x4473D0:declarations.append('const u16 D_%08X=0x%04Xu;'%(address,int.from_bytes(raw,'little')))
        else:declarations.append('const char D_%08X[]={%s};'%(address,','.join(str(b) for b in raw)))
        records.append(dict(address=hex(address),bytes=n,raw_sha256=SHA(raw),ignored_native_binding_only_no_data_award=True))
    (OUT/'local_bindings.c').write_text('#include "george/types.h"\n'+'\n'.join(declarations)+'\n',newline='\n')
    (OUT/'local_bindings.json').write_text(json.dumps(records,indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    sources=[('test',ROOT/'tests/native/file_archive_lifecycle.c',[]),('selected',SELECTED,[]),
      *[(name,OUT/('reused_'+name+'.c'),[]) for name in blocks],('bindings',OUT/'local_bindings.c',[]),
      ('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]),
      ('strlwr',ROOT/'src/runtime/strlwr.c',['-I',str(EE),'-Dstrlwr=lifecycle_real_strlwr']),
      ('tolower',ROOT/'src/runtime/tolower.c',['-I',str(EE)])]
    licensed=[]
    for name in ('memcpy','memset','strcpy','strcat','strncat','strncmp','strncpy'):
        path='ee/newlib/libc/string/'+name+'.c'
        raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':'+path],capture_output=True,check=True).stdout
        file=OUT/('generic_'+name+'.c');file.write_bytes(raw)
        sources.append((name,file,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Wno-parentheses','-D'+name+'=lifecycle_real_'+name]))
        licensed.append(dict(revision=REVISION,path=path,raw_sha256=SHA(raw),unchanged=True,native_support_only_no_helper_identity_or_award=True,license='LICENSES/newlib-1.8.1.txt clause9 Cygnus1994/1997 default'))
    (OUT/'licensed_support.json').write_text(json.dumps(licensed,indent=2)+'\n')
    objects=[]
    for name,path,extra in sources:
        obj=OUT/(name+'.o');command=[*flags,*extra,'-c',str(path),'-o',str(obj)]
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr);objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60);commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr);print(r.stdout,end='')
if __name__=='__main__':main()
