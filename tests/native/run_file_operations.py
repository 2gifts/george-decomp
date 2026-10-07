"""Real selected C and exact recovered helper source closures as separate TUs.

SDK/kernel I/O and core heap effects are explicitly controlled. Generic pinned
newlib byte paths support the native contract; they earn no retail source award.
Local original literals stay in ignored output, never in this public runner.
"""
import hashlib,json,os,subprocess,sys,zlib,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/file_operations'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    reuse=[]
    def span(source,raw,begin,end):
        data=raw[begin:end]
        reuse.append(dict(source=source,source_raw_sha256=SHA(raw),raw_offset=begin,
          raw_size=len(data),span_raw_sha256=SHA(data),unmodified=True))
        return data
    def line(source,raw,prefix):
        value=next(x for x in raw.splitlines(keepends=True) if x.startswith(prefix))
        begin=raw.index(value)
        return span(source,raw,begin,begin+len(value))
    def generated(name,prefix,body):
        path=OUT/name;path.write_bytes(prefix+body+b'\n')
        reuse.append(dict(generated_file=name,entire_tu_raw_sha256=SHA(path.read_bytes())))
        return path
    source='src/game/heap.c';raw=(ROOT/source).read_bytes()
    begin=raw.index(b'#define DEFINE_ALLOCATION(');end=raw.index(b'\n\nDEFINE_ALLOCATION(',begin)
    body=span(source,raw,begin,end)+b'\n'
    for name in (b'func_002AEB60',b'func_002AEC28'):
        body+=line(source,raw,b'DEFINE_ALLOCATION('+name+b',')
    body+=b'#undef DEFINE_ALLOCATION\n'+line(source,raw,b'GEORGE_DEFINE_FREE_FORWARD(func_002AEE40,')
    heap=generated('recovered_heap.c',b'#include "george/heap.h"\n#include "george/function_templates.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n',body)
    source='include/george/algorithm_templates.h';raw=(ROOT/source).read_bytes()
    begin=raw.index(b'#define GEORGE_DEFINE_MAP_LOOKUP(');end=raw.index(b'\n\n#define ',begin)
    body=span(source,raw,begin,end)+b'\n'
    source='src/game/deimos_tables.c';raw=(ROOT/source).read_bytes()
    body+=line(source,raw,b'GEORGE_DEFINE_MAP_LOOKUP(func_002A7C08)')
    mapping=generated('recovered_map.c',b'#include "george/deimos_tables.h"\n',body)
    # Exact accessor macro and two complete original entry invocations, rather
    # than incompatible proxy accessors under a real-source claim.
    source='src/game/accessors.c';raw=(ROOT/source).read_bytes()
    begin=raw.index(b'#define ACCESSOR(');end=raw.index(b'ACCESSOR(func_',begin)
    body=span(source,raw,begin,end)+b'\n'
    for name in (b'func_002B1AF0',b'func_002B1AF8'):body+=line(source,raw,b'ACCESSOR('+name+b',')
    accessors=generated('recovered_accessors.c',b'#include "george/accessors.h"\n',body)
    (OUT/'source_reuse.json').write_text(json.dumps(reuse,indent=2)+'\n')
    from analyze import validated_elf
    _,original=validated_elf(ROOT/'orig/SLUS_216.68')
    records=[];declarations=[]
    for address,n in ((0x446FE8,2),(0x446FF0,2),(0x446FF8,7),(0x4473C0,8)):
        data=original[address-0xFF000:address-0xFF000+n]
        declarations.append('const char D_%08X[]={%s};'%(address,','.join(str(b) for b in data)))
        records.append(dict(address=hex(address),bytes=n,raw_sha256=SHA(data),local_original_only_no_asset_or_data_award=True))
    table=[]
    for i in range(256):
        value=i
        for _ in range(8):value=(value>>1)^(0xEDB88320 if value&1 else 0)
        table.append(value)
    data=struct.pack('<256I',*table)
    assert data==original[0x445650-0xFF000:0x445A50-0xFF000]
    declarations.append('const u32 D_00445650[256]={%s};'%','.join(hex(x)+'u' for x in table))
    records.append(dict(address='0x445650',bytes=1024,raw_sha256=SHA(data),generated_reflected_polynomial='0xEDB88320',entire_original_table_equal=True,data_award=False))
    bindings=OUT/'local_bindings.c';bindings.write_text('#include "george/types.h"\n'+'\n'.join(declarations)+'\n',newline='\n')
    (OUT/'local_bindings.json').write_text(json.dumps(records,indent=2)+'\n')
    sources=[('test',ROOT/'tests/native/file_operations.c',[]),('selected',ROOT/'src/game/file_operations.c',[]),
      ('registry',ROOT/'src/game/string_registry.c',[]),('heap',heap,[]),('map',mapping,[]),('accessors',accessors,[]),
      ('algorithms',ROOT/'src/game/string_algorithms.c',[]),('bindings',bindings,[]),
      ('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]),
      ('strlwr',ROOT/'src/runtime/strlwr.c',['-I',str(EE),'-Dstrlwr=file_real_strlwr']),
      ('tolower',ROOT/'src/runtime/tolower.c',['-I',str(EE)]),
      ('strstr',ROOT/'src/runtime/strstr.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dstrstr=file_real_strstr'])]
    licensed=[]
    for name in ('memcpy','strcpy','strcat','strcmp'):
        path='ee/newlib/libc/string/'+name+'.c'
        data=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':'+path],capture_output=True,check=True).stdout
        file=OUT/('generic_'+name+'.c');file.write_bytes(data)
        sources.append((name,file,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Wno-parentheses','-D'+name+'=file_real_'+name]))
        licensed.append(dict(revision=REVISION,path=path,raw_sha256=SHA(data),unchanged=True,target_source_identity_or_award=False))
    (OUT/'licensed_support.json').write_text(json.dumps(licensed,indent=2)+'\n')
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    commands=[];objects=[]
    for name,path,extra in sources:
        obj=OUT/(name+'.o');command=[*flags,*extra,'-c',str(path),'-o',str(obj)]
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
        (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
        assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
        objects.append(str(obj))
    command=[flags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]
    r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=r.stdout,stderr=r.stderr,exit_code=r.returncode))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    print(r.stdout,end='')

if __name__=='__main__':main()
