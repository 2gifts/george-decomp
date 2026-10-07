"""Execute real timer C with published/native SGI, heap, search and atexit.
Count/core allocation/diagnostic/pop callbacks are controlled observation seams.
Historical genuine C++ headers retain their warnings; no retail compiler claim.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,subprocess,sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,default=ROOT/'build/native/timer_registry')
OUT=parser.parse_args().output.resolve();OUT.relative_to(ROOT)
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
FRONT=ROOT/'tools/vendor/vector-insert-native323/bin/gcc.exe'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
SHA=lambda b:hashlib.sha256(b).hexdigest()

def main():
    OUT.mkdir(parents=True,exist_ok=True);_,original=validated_elf(ROOT/'orig/SLUS_216.68')
    env=dict(os.environ);env['PATH']=str(BIN)+os.pathsep+str(FRONT.parent)+os.pathsep+env['PATH']
    spans=[];commands=[];licensed=[]
    raw=(ROOT/'src/game/heap.c').read_bytes();lo=raw.index(b'#define DEFINE_ALLOCATION(');hi=raw.index(b'\nDEFINE_ALLOCATION(',lo)
    macro=raw[lo:hi];pieces=[macro]
    spans.append(dict(source='src/game/heap.c',whole_source_raw_sha256=SHA(raw),raw_offset=lo,raw_size=hi-lo,complete_raw_sha256=SHA(macro),whole_macro=True))
    for line in (b'DEFINE_ALLOCATION(func_002AEE60, (u32 size), D_003FD204, 0x28, 4)',b'DEFINE_ALLOCATION(func_002AF140, (u32 size), D_003FD204, 0x18, 3)',b'GEORGE_DEFINE_FREE_FORWARD(func_002AF1E8, func_002AE158)'):
        assert raw.count(line)==1;pieces.append(line);spans.append(dict(source='src/game/heap.c',whole_source_raw_sha256=SHA(raw),raw_offset=raw.index(line),raw_size=len(line),complete_raw_sha256=SHA(line),whole_invocation=True))
    heap=OUT/'heap.c';heap.write_bytes(b'#include "george/heap.h"\n#include "george/function_templates.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+b'\n\n'.join(pieces)+b'\n')
    raw=(ROOT/'src/game/startup_services.c').read_bytes();line=b'GEORGE_DEFINE_UPPER_BOUND(func_00100C30)';assert raw.count(line)==1
    upper=OUT/'upper.c';upper.write_bytes(b'#include "george/startup.h"\n#include "george/algorithm_templates.h"\n'+line+b'\n')
    spans.append(dict(source='src/game/startup_services.c',whole_source_raw_sha256=SHA(raw),raw_offset=raw.index(line),raw_size=len(line),complete_raw_sha256=SHA(line),whole_invocation=True))
    bridge=OUT/'bridge.cpp'
    bridge.write_text('''/* Native-only authentic constructed vector plus C prefix bridge.
 * C views use measured GNU alias/storage semantics, not a universal class ABI. */
#include "'''+(ROOT/'src/runtime/sgi/vector_insert.cpp').as_posix()+'''"
template char *GeorgeVectorInsertAllocator::_S_start_free;
template char *GeorgeVectorInsertAllocator::_S_end_free;
template size_t GeorgeVectorInsertAllocator::_S_heap_size;
template GeorgeVectorInsertAllocator::_Obj *GeorgeVectorInsertAllocator::_S_free_list[16];
template void (*__malloc_alloc_template<0>::__malloc_alloc_oom_handler)();
struct TimerActualVector : GeorgeVectorInsertTemplate {
 void bind(void **b,void **e,void **c){_M_start=b;_M_finish=e;_M_end_of_storage=c;}
 void insert_aux(void **p,void *const &v){_M_insert_aux(p,v);}
 void offsets(unsigned *p){p[0]=(char *)&_M_start-(char *)this;p[1]=(char *)&_M_finish-(char *)this;p[2]=(char *)&_M_end_of_storage-(char *)this;}
};
extern "C" {
 void timer_unexecuted(void);
 TimerActualVector D_0046A0F0;
 void timer_native_bind(void **b,void **e,void **c){D_0046A0F0.bind(b,e,c);}
 void timer_native_construct_slots(void **p,unsigned n){for(unsigned i=0;i<n;++i)construct(p+i,(void *)0);}
 void timer_native_layout(unsigned *p){p[0]=sizeof(void *);p[1]=sizeof(TimerActualVector);p[2]=sizeof(size_t);p[3]=sizeof(ptrdiff_t);p[4]=16*sizeof(void *);p[5]=sizeof(char *);p[6]=sizeof(size_t);D_0046A0F0.offsets(p+7);}
 void func_001007E0(void *r,void **p,void *const *v){if(r!=&D_0046A0F0)timer_unexecuted();D_0046A0F0.insert_aux(p,*v);}
}
ostream &ostream::operator<<(const char *){timer_unexecuted();return *this;}
ostream &endl(ostream &s){timer_unexecuted();return s;}
''',newline='\n')
    binding=OUT/'bindings.c';parts=['#include "george/types.h"', '/* Inactive, fail-fast historical ostream support only; no retail object/layout claim. */', 'unsigned char cerr[256];']
    for name,address,size in [('D_00447AA0',0x447AA0,16),('D_00447238',0x447238,44)]:
        b=original[address-0xFF000:address-0xFF000+size];parts.append('const %s %s[]={%s};'%('u8' if name=='D_00447AA0' else 'char',name,','.join(str(v) for v in b)))
    binding.write_text('\n'.join(parts)+'\n',newline='\n')
    exit_abi=OUT/'atexit_abi.c';exit_abi.write_text('''#include <stddef.h>
#include <reent.h>
const unsigned timer_native_atexit_abi[]={sizeof(struct _atexit),offsetof(struct _atexit,_next),offsetof(struct _atexit,_ind),offsetof(struct _atexit,_fns),offsetof(struct _reent,_atexit),offsetof(struct _reent,_atexit0),_ATEXIT_SIZE};
''',newline='\n')
    path='ee/newlib/libc/string/memmove.c';revision='b595ded606227e93b8c4a447446c1d2ac093827d'
    b=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',revision+':'+path],capture_output=True,check=True).stdout
    memmove=OUT/'memmove.c';memmove.write_bytes(b)
    licensed.append(dict(source=path,revision=revision,whole_primary_raw_sha256=SHA(b),unchanged=True,flags=['PREFER_SIZE_OVER_SPEED'],no_helper_award=True))
    cflags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
    sources=[('test',ROOT/'tests/native/timer_registry.c',['-DGEORGE_TREE_NATIVE_COUNTER']),('selected',ROOT/'src/game/timer_registry.c',['-DGEORGE_TREE_NATIVE_COUNTER']),('heap',heap,[]),('upper',upper,[]),('compare',ROOT/'src/game/vector_insert_compare.c',[]),('bindings',binding,[]),('memmove',memmove,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dmemmove=timer_real_memmove']),('atexit',ROOT/'src/runtime/stdlib/atexit.c',['-I',str(ROOT/'src/runtime/stdlib/compat'),'-I',str(EE),'-Datexit=func_00396260','-Dmalloc=timer_exit_malloc','-D_impure_ptr=D_00405694']),('atexit_abi',exit_abi,['-I',str(ROOT/'src/runtime/stdlib/compat'),'-I',str(EE)])]
    cppflags=[str(FRONT),'-B'+str(BIN)+'/', '-O2','-fno-builtin','-fno-exceptions','-fno-weak','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Isrc/runtime/sgi/include','-Isrc/runtime/sgi/libio','-Isrc/runtime/sgi/gcc','-Isrc/runtime/sgi/config','-Iinclude','-I'+str(BIN.parent/'include')]
    objects=[]
    def run(cmd):
        if '-o' in cmd:
            Path(cmd[cmd.index('-o')+1]).resolve().relative_to(OUT)
        p=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=120)
        commands.append(dict(command=cmd,returncode=p.returncode,stdout=p.stdout,stderr=p.stderr));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
        assert not p.returncode,(p.stdout,p.stderr)
        return p
    for name,source,extra in sources:
        obj=OUT/(name+'.o');p=run([*cflags,*extra,'-c',str(source),'-o',str(obj)]);assert not p.stderr,(name,p.stderr);objects.append(str(obj))
    obj=OUT/'bridge.o';run([*cppflags,'-c',str(bridge),'-o',str(obj)]);objects.append(str(obj))
    exe=OUT/'checks.exe';run([str(FRONT),'-B'+str(BIN)+'/', '-B'+str(BIN.parent/'lib')+'/',*objects,'-L'+str(BIN.parent/'lib'),'-Wl,--gc-sections','-o',str(exe)])
    p=run([str(exe)]);assert not p.stderr,p.stderr
    (OUT/'reuse.json').write_text(json.dumps(dict(complete_raw_spans=spans,whole_genuine_Cpp_source=dict(path='src/runtime/sgi/vector_insert.cpp',raw_sha256=SHA((ROOT/'src/runtime/sgi/vector_insert.cpp').read_bytes())),licensed_support=licensed,
      generated_TUs=[dict(path=source.relative_to(ROOT).as_posix(),bytes=len(source.read_bytes()),raw_sha256=SHA(source.read_bytes())) for source in (heap,upper,bridge,binding,memmove,exit_abi)],
      limits='Constructed genuine vector and pointer-element lives; C prefix views and static member aliases use measured GNU/native domains. Historical C++ warnings retained. Full published upper-bound/comparator/heap macros and licensed memmove/atexit execute. Count, core allocation, reporting/abort/pop/SDK are controlled support; no additional recovery, class/data/compiler/hardware award.'),indent=2)+'\n')
    print(p.stdout,end='')
if __name__=='__main__':main()
