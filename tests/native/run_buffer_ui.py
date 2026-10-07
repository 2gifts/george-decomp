"""Compile UI with genuine published buffer/map and licensed runtime sources.

Renderer/parser/completion contracts are explicit native controls. Exact complete
Deimos function/macro spans are extracted to avoid unrelated PE unresolveds.
Generic licensed strings observe defined results without claiming original EE
overread/stack layout identity. Original text bindings remain local-only inputs.
"""
import hashlib,json,os,re,subprocess,sys,zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/buffer_ui'
SELECTED=ROOT/'src/game/buffer_ui.c'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
def digest(data):return hashlib.sha256(data).hexdigest()
def function(source,name):
    found=re.search(rb'(?m)^[A-Za-z_][^;\n]*\b'+name.encode()+rb'\(',source)
    assert found is not None,name
    begin=found.start();at=source.index(name.encode()+b'(',begin)
    opening=source.index(b'{',at);depth=1;end=opening+1
    while depth:
        b=source[end];depth+=(b==123)-(b==125);end+=1
    return begin,end,source[begin:end]
def extract(path,names,prefix,extra=b''):
    raw=(ROOT/path).read_bytes();parts=[];records=[]
    for name in names:
        begin,end,part=function(raw,name);parts.append(part)
        records.append(dict(name=name,source=path,whole_source_raw_sha256=digest(raw),raw_offset=begin,raw_size=end-begin,complete_span_raw_sha256=digest(part)))
    generated=prefix+b'\n'+b'\n\n'.join(parts)+b'\n'+extra
    return generated,records
def main():
    OUT.mkdir(parents=True,exist_ok=True);_,original=validated_elf(ROOT/'orig/SLUS_216.68')
    scope=json.loads((ROOT/'build/buffer_ui_scope/scope.json').read_text())
    readonly=[];declarations=[]
    for record in scope['readonly_text_bindings']+scope['readonly_copied_prefixes']:
        a=int(record['address'],16);n=record.get('complete_terminated_extent',record.get('observed_copied_extent'));raw=original[a-0xFF000:a-0xFF000+n]
        assert digest(raw)==record['sha256']
        declarations.append('const signed char %s[]={%s};'%(record['binding'],','.join(str(b) for b in raw)))
        readonly.append(dict(name=record['binding'],size=n,raw_sha256=digest(raw),local_original_input_only=True))
    crc_table=[]
    for i in range(256):
        v=i
        for j in range(8):v=(v>>1)^(0xEDB88320 if v&1 else 0)
        crc_table.append(v)
    import struct
    assert struct.pack('<256I',*crc_table)==original[0x445650-0xFF000:0x445A50-0xFF000]
    declarations.append('const u32 D_00445650[]={%s};'%','.join('0x%08Xu'%v for v in crc_table))
    binding=OUT/'local_original_bindings.c';binding.write_text('#include "george/types.h"\n'+'\n'.join(declarations)+'\n',newline='\n')
    values,span1=extract('src/game/deimos_values.c',['func_002CD990','func_002CDA20'],b'#include "george/deimos.h"')
    (OUT/'deimos_values.c').write_bytes(values)
    tables,span2=extract('src/game/deimos_tables.c',['func_002CDD60','func_002CDD88','func_002CDDB0'],
      b'#include "george/deimos_tables.h"\n#include "george/algorithm_templates.h"\nextern GeorgeGenericMap *D_00481760;',
      b'GEORGE_DEFINE_MAP_LOOKUP(func_002A7C08)\nGEORGE_DEFINE_MAP_VISITOR(func_002A8130)\n')
    # Macro invocations retain exact published bytes; definitions are the entire
    # unchanged included header, with its full raw fingerprint below.
    actual=(ROOT/'src/game/deimos_tables.c').read_bytes()
    for line in (b'GEORGE_DEFINE_MAP_LOOKUP(func_002A7C08)',b'GEORGE_DEFINE_MAP_VISITOR(func_002A8130)'):
        assert actual.count(line)==1
    (OUT/'deimos_tables.c').write_bytes(tables)
    getter,span3=extract('src/game/array_records.c',['func_002AAF50'],b'#include "george/array_records.h"')
    (OUT/'array_getter.c').write_bytes(getter)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-msse2','-mfpmath=sse','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    sources=[('test',ROOT/'tests/native/buffer_ui.c',[]),('selected',SELECTED,[]),
      ('manager',ROOT/'src/game/buffer_manager.c',[]),('records',ROOT/'src/game/buffer_records.c',[]),
      ('accessors',ROOT/'src/game/accessors.c',[]),('strings',ROOT/'src/game/string_algorithms.c',[]),
      ('tables',OUT/'deimos_tables.c',[]),('values',OUT/'deimos_values.c',[]),('bindings',binding,[]),
      ('array_getter',OUT/'array_getter.c',['-Dfunc_002AAF50=ui_real_array_get']),
      ('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]),
      ('strrchr',ROOT/'src/runtime/strrchr.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dstrrchr=ui_generic_strrchr']),
      ('strpbrk',ROOT/'src/runtime/strpbrk.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dstrpbrk=ui_generic_strpbrk']),
      ('strstr',ROOT/'src/runtime/strstr.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dstrstr=ui_generic_strstr']),
      ('float',ROOT/'src/runtime/fp_bit.c',['-DFINE_GRAINED_LIBRARIES','-DFLOAT','-DUS_SOFTWARE_GOFAST','-DFLOAT_BIT_ORDER_MISMATCH','-DNO_DENORMALS','-DL_unpack_sf','-DL_sf_to_df','-Dfptodp=ui_real_fptodp']),
      ('double',ROOT/'src/runtime/fp_bit.c',['-DFINE_GRAINED_LIBRARIES','-DUS_SOFTWARE_GOFAST','-DFLOAT_BIT_ORDER_MISMATCH','-DL_make_df','-DL_pack_df'])]
    licensed=[]
    for name in ('strcpy','strcat','strncpy','memset'):
        path='ee/newlib/libc/string/'+name+'.c'
        raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':'+path],capture_output=True,check=True).stdout
        output=OUT/('generic_'+name+'.c');output.write_bytes(raw)
        sources.append((name,output,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Wno-parentheses','-D'+name+'=ui_generic_'+name]))
        licensed.append(dict(revision=REVISION,path=path,raw_sha256=digest(raw),unchanged=True,target_source_identity_or_award=False))
    commands=[];objects=[]
    for name,source,extra in sources:
        obj=OUT/(name+'.o');cmd=[*flags,*extra,'-c',str(source),'-o',str(obj)]
        run=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=cmd,stdout=run.stdout,stderr=run.stderr,exit_code=run.returncode));assert not run.returncode and not run.stderr,(name,run.stdout,run.stderr)
        objects.append(str(obj))
    cmd=[str(BIN/'gcc.exe'),'-m32','-Wl,--gc-sections',*objects,'-o',str(OUT/'checks.exe')]
    run=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=cmd,stdout=run.stdout,stderr=run.stderr,exit_code=run.returncode));assert not run.returncode and not run.stderr,(run.stdout,run.stderr)
    run=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=120)
    commands.append(dict(command=[str(OUT/'checks.exe')],stdout=run.stdout,stderr=run.stderr,exit_code=run.returncode))
    reuse=dict(complete_raw_function_spans=span1+span2+span3,generated_TUs=[dict(path=str(OUT/p),sha256=digest((OUT/p).read_bytes())) for p in ('deimos_tables.c','deimos_values.c','array_getter.c')],
      full_macro_header_raw_sha256=digest((ROOT/'include/george/algorithm_templates.h').read_bytes()),licensed_generic_strings=licensed,
      local_original_named_bindings=readonly,stack_or_EE_bulk_read_identity=False,full_helper_graph_no_new_award=True)
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n');(OUT/'reuse.json').write_text(json.dumps(reuse,indent=2)+'\n')
    assert not run.returncode and not run.stderr,(run.stdout,run.stderr);print(run.stdout,end='')
if __name__=='__main__':main()
