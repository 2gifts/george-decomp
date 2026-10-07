"""Whole genuine SGI templates and exact published helper source spans.

Native historical frontend warnings remain visible. Engine heap core and
unreached stream/exit branch are explicit controls; no helper award.
"""
from pathlib import Path
import hashlib,json,os,subprocess,sys
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'build/native/packet_construction'
B=ROOT/'tools/vendor/ps2dev-20181019/MinGW';F=ROOT/'tools/vendor/vector-insert-native323'
EE=B/'msys/1.0/local/ps2dev/ee/ee/include';REV='b595ded606227e93b8c4a447446c1d2ac093827d';SHA=lambda b:hashlib.sha256(b).hexdigest()
SELECTED_SOURCE=ROOT/'src/game/packet_construction.c'
ALIASES=['-DD_003F21B8=_ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE','-Dfunc_002521F8=_ZN24__default_alloc_templateILb0ELi0EE9_S_refillEj','-Dfunc_00252168=_ZN23__malloc_alloc_templateILi0EE13_S_oom_mallocEj','-Dfunc_002BD1F0=_Z6fill_nIPhjhET_S1_T0_RKT1_']
def main():
 OUT.mkdir(parents=True,exist_ok=True);commands=[];reuse=[]
 raw=(ROOT/'src/game/heap.c').read_bytes();start=raw.index(b'#define DEFINE_ALLOCATION(');end=raw.index(b'\n\nDEFINE_ALLOCATION(',start);macro=raw[start:end]
 lines=[next(x for x in raw.splitlines(keepends=True)if x.startswith(b'DEFINE_ALLOCATION('+name+b','))for name in (b'func_002AEE60',b'func_002AF140')]
 generated=b'#include "george/heap.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+macro+b'\n'+b''.join(lines)+b'#undef DEFINE_ALLOCATION\n'
 (OUT/'heap_wrappers.c').write_bytes(generated)
 for selected in [macro,*lines]:reuse.append(dict(source='src/game/heap.c',source_raw_sha256=SHA(raw),offset=raw.index(selected),bytes=len(selected),span_raw_sha256=SHA(selected),unchanged=True))
 raw=(ROOT/'src/game/string_algorithms.c').read_bytes();start=raw.index(b'u32 func_00295050(');opening=raw.index(b'{',start);end=opening+1;depth=1
 while depth:depth+=(raw[end]==123)-(raw[end]==125);end+=1
 span=raw[start:end];(OUT/'length.c').write_bytes(b'#include "george/string_algorithms.h"\n'+span+b'\n')
 reuse.append(dict(source='src/game/string_algorithms.c',source_raw_sha256=SHA(raw),offset=start,bytes=len(span),span_raw_sha256=SHA(span),unchanged=True,complete_function=True))
 raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REV+':ee/newlib/libc/string/memcpy.c'],capture_output=True,check=True).stdout
 (OUT/'memcpy.c').write_bytes(raw);reuse.append(dict(source='ee/newlib/libc/string/memcpy.c',revision=REV,raw_sha256=SHA(raw),unchanged=True,native_support_only=True,license='Newlib1.8.1 default Cygnus1994/1997 clause9'))
 env=os.environ.copy();env['PATH']=str(B/'bin')+os.pathsep+str(F/'bin')+os.pathsep+env['PATH']
 cflags=[str(B/'bin/gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
 cppflags=[str(F/'bin/gcc.exe'),'-B'+str(B/'bin')+'/', '-O2','-fno-builtin','-fno-exceptions','-fno-weak','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Isrc/runtime/sgi/include','-Isrc/runtime/sgi/libio','-Isrc/runtime/sgi/gcc','-Isrc/runtime/sgi/config','-Iinclude','-I'+str(B/'include')]
 sources=[('test',ROOT/'tests/native/packet_construction.c',ALIASES,False),('selected',SELECTED_SOURCE,ALIASES,False),('fill',ROOT/'src/runtime/sgi/packet_fill.cpp',[],True),('sgi',ROOT/'tests/native/packet_construction_support.cpp',[],True),('opaque',ROOT/'tests/native/vector_insert_unreachable.c',[],False),('heap',OUT/'heap_wrappers.c',[],False),('length',OUT/'length.c',[],False),('memcpy',OUT/'memcpy.c',['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dmemcpy=func_003934F8'],False)]
 def run(cmd):
  q=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60);commands.append(dict(command=cmd,exit_code=q.returncode,stdout=q.stdout,stderr=q.stderr));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n');assert not q.returncode,(q.stdout,q.stderr);return q
 objects=[];inventories=[]
 for name,path,extra,cpp in sources:
  obj=OUT/(name+'.o');q=run([*(cppflags if cpp else cflags),*extra,'-c',str(path),'-o',str(obj)]);objects.append(str(obj))
  if q.stderr:print(q.stderr,end='')
  if not cpp:assert not q.stderr,q.stderr
  nm=run([str(B/'bin/nm.exe'),str(obj)])
  dump=run([str(B/'bin/objdump.exe'),'-h',str(obj)])
  inventories.append(dict(name=name,object=str(obj),raw_sha256=SHA(obj.read_bytes()),symbols=nm.stdout,sections=dump.stdout))
 (OUT/'source_reuse.json').write_text(json.dumps(dict(spans=reuse,generated=[dict(path=p.name,raw_sha256=SHA(p.read_bytes()))for p in (OUT/'heap_wrappers.c',OUT/'length.c')],source_bodies_not_modified=True,whole_primary_sgi_headers=True,native_only_macro_symbol_aliases=ALIASES),indent=2)+'\n')
 (OUT/'native_symbol_inventory.json').write_text(json.dumps(inventories,indent=2)+'\n')
 run([cflags[0],'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]);q=run([str(OUT/'checks.exe')]);assert not q.stderr;print(q.stdout,end='')
if __name__=='__main__':main()
