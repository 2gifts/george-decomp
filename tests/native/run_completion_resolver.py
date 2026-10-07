"""Real initialized resolver/container source with explicit native ABI bridges.
The genuine historical C++ frontend retains its inactive upstream warnings.
Unknown core/release/vfprintf callbacks remain authored finite contracts.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,subprocess,sys
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
options=argparse.ArgumentParser(description=__doc__)
options.add_argument('--output',type=Path,default=ROOT/'build/native/completion_resolver')
OUT=options.parse_args().output.resolve()
OUT.relative_to(ROOT)
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
FRONT=ROOT/'tools/vendor/vector-insert-native323/bin/gcc.exe'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
PIN='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()

def function(raw,name):
 match=re.search(rb'(?m)^[A-Za-z_][^;\n]*\b'+name.encode()+rb'\(',raw);assert match,name
 start=match.start();opening=raw.index(b'{',raw.index(name.encode()+b'(',start));depth=1;end=opening+1
 while depth:
  b=raw[end];depth+=(b==123)-(b==125);end+=1
 return start,end,raw[start:end]

def main():
 OUT.mkdir(parents=True,exist_ok=True);_,original=validated_elf(ROOT/'orig/SLUS_216.68')
 env=dict(os.environ);env['PATH']=str(BIN)+os.pathsep+str(FRONT.parent)+os.pathsep+env['PATH']
 spans=[];licensed=[];commands=[];objects=[]
 raw=(ROOT/'src/game/heap.c').read_bytes();start=raw.index(b'#define DEFINE_ALLOCATION(');end=raw.index(b'\nDEFINE_ALLOCATION(',start)
 allocation=raw[start:end];line=b'DEFINE_ALLOCATION(func_002AF140, (u32 size), D_003FD204, 0x18, 3)';assert raw.count(line)==1
 pieces=[allocation,line]
 for off,body,label in ((start,allocation,'entire allocation macro'),(raw.index(line),line,'entire selected macro invocation')):
  spans.append(dict(source='src/game/heap.c',whole_source_raw_sha256=SHA(raw),raw_offset=off,raw_size=len(body),complete_raw_sha256=SHA(body),label=label))
 for n in ('func_002AF100','func_002AF120'):
  a,b,body=function(raw,n);pieces.append(body);spans.append(dict(source='src/game/heap.c',name=n,whole_source_raw_sha256=SHA(raw),raw_offset=a,raw_size=b-a,complete_raw_sha256=SHA(body)))
 line=b'GEORGE_DEFINE_FREE_FORWARD(func_002AF1E8, func_002AE158)';assert raw.count(line)==1;pieces.append(line)
 spans.append(dict(source='src/game/heap.c',whole_source_raw_sha256=SHA(raw),raw_offset=raw.index(line),raw_size=len(line),complete_raw_sha256=SHA(line),complete_macro_invocation=True))
 heap=OUT/'heap.c';heap.write_bytes(b'#include "george/heap.h"\n#include "george/function_templates.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+b'\n\n'.join(pieces)+b'\n')
 cpp=OUT/'bridge.cpp';cpp.write_text(r'''/* Native bridge only: unchanged full genuine methods and genuine static
 instantiations. Measured GNU/native prefix views are explicit qualifications. */
#include "../../../src/runtime/sgi_resolver/completion_containers.cpp"
template char *__default_alloc_template<false,0>::_S_start_free;
template char *__default_alloc_template<false,0>::_S_end_free;
template size_t __default_alloc_template<false,0>::_S_heap_size;
template __default_alloc_template<false,0>::_Obj * volatile __default_alloc_template<false,0>::_S_free_list[16];
template void (*__malloc_alloc_template<0>::__malloc_alloc_oom_handler)();
template void *__malloc_alloc_template<0>::_S_oom_malloc(size_t);
template void *__default_alloc_template<false,0>::_S_refill(size_t);
extern "C" {
void resolver_unexecuted(void);
void resolver_construct(void *a,void *b,void *v){
 GeorgeCompletionHashtable *x=new(a) GeorgeCompletionHashtable(0,hash<unsigned>(),equal_to<unsigned>());
 GeorgeCompletionHashtable *y=new(b) GeorgeCompletionHashtable(0,hash<unsigned>(),equal_to<unsigned>());
 /* The boot allocations belong only to establishing authentic object lives.
  * Observed field setup immediately replaces their detached prefix ranges. */
 GeorgeCompletionAllocator::deallocate(((void **)((char *)x+4))[0],53*4);
 GeorgeCompletionAllocator::deallocate(((void **)((char *)y+4))[0],53*4);
 new(v) GeorgeCompletionBucketVector;
}
void resolver_destroy_empty(void *a,void *b,void *v){
 ((GeorgeCompletionHashtable *)a)->~hashtable();((GeorgeCompletionHashtable *)b)->~hashtable();((GeorgeCompletionBucketVector *)v)->~vector();
}
void resolver_node(void *p,unsigned key,void *value,void *next){
 typedef _Hashtable_node<GeorgeCompletionPair> Node;Node *n=new(p)Node;
 new(&n->_M_val)GeorgeCompletionPair(key,value);n->_M_next=(Node *)next;
}
void resolver_layout(unsigned *v){
 typedef _Hashtable_node<GeorgeCompletionPair> Node;
 v[0]=sizeof(void *);v[1]=sizeof(GeorgeCompletionBucketVector);v[2]=sizeof(GeorgeCompletionHashtable);v[3]=sizeof(GeorgeCompletionTemplateIterator);v[4]=sizeof(Node);
 v[5]=offsetof(Node,_M_val);v[6]=offsetof(Node,_M_val.second);v[7]=offsetof(GeorgeCompletionTemplateIterator,_M_cur);v[8]=offsetof(GeorgeCompletionTemplateIterator,_M_ht);v[9]=sizeof(unsigned);v[10]=sizeof(unsigned long long);v[11]=16*sizeof(void *);
}
void func_002BF0F0(void *p,void **at,unsigned n,void *const *value){((GeorgeCompletionBucketVector *)p)->insert(at,n,*value);}
void func_002BF418(void *p){((GeorgeCompletionHashtable *)p)->clear();}
void *func_002BF4B8(void *p){
 void **prefix=(void **)p;
 GeorgeCompletionTemplateIterator i((_Hashtable_node<GeorgeCompletionPair> *)prefix[0],(GeorgeCompletionHashtable *)prefix[1]);
 ++i;prefix[0]=i._M_cur;prefix[1]=i._M_ht;return p;
}
const unsigned long long *func_00252110(const unsigned long long *a,const unsigned long long *b,const unsigned long long *key){return lower_bound(a,b,*key);}
}
/* These complete declarations satisfy inactive historical diagnostic paths;
 * every attempted execution fails. No actual stream/OS implementation claim. */
ostream &ostream::operator<<(const char *){resolver_unexecuted();return *this;}
ostream &endl(ostream &s){resolver_unexecuted();return s;}
'''.replace('#include "../../../src/runtime/sgi_resolver/completion_containers.cpp"','#include "'+(ROOT/'src/runtime/sgi_resolver/completion_containers.cpp').as_posix()+'"'),newline='\n')
 bindings=OUT/'bindings.c';parts=['#include "george/completion_resolver.h"','struct NativeTable { u32 words[2]; GeorgeCompletionVirtual method; };','const struct NativeTable D_004208A0={{0,0},{0,0,func_00104B10}};','static void unused(void *p,s32 n){(void)p;(void)n;extern void resolver_unexecuted(void);resolver_unexecuted();}','const struct NativeTable D_004200F8={{0,0},{0,0,unused}};','unsigned char cerr[256];']
 for name,a,n in [('D_00447F48',0x447F48,27),('D_00447238',0x447238,44)]:
  data=original[a-0xFF000:a-0xFF000+n];parts.append('const char %s[]={%s};'%(name,','.join(str(x) for x in data)))
 primes=original[0x447F78-0xFF000:0x448058-0xFF000];parts.append('const GeorgeCompletionPrime D_00447F78[]={'+','.join('0x%XULL'%int.from_bytes(primes[i:i+8],'little') for i in range(0,224,8))+'};')
 bindings.write_text('\n'.join(parts)+'\n',newline='\n')
 observer=OUT/'format.c';observer.write_text(r'''#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
extern const char D_00447F48[];
extern void resolver_formatter_event(void *,unsigned);
int resolver_observe_vfprintf(FILE *f,const char *fmt,va_list ap){
 unsigned key=va_arg(ap,unsigned),i=0,j;char digits[8];const char *p=fmt;
 if(fmt!=D_00447F48||f->_flags!=0x208||f->_w!=0x7FFFFFFF||f->_bf._size!=0x7FFFFFFF)abort();
 resolver_formatter_event(f->_p,key);for(j=0;j<8;++j)digits[7-j]="0123456789abcdef"[(key>>(4*j))&15];
 while(*p){if(p[0]=='%'&&p[1]=='0'&&p[2]=='8'&&p[3]=='x'){for(j=0;j<8;++j)*f->_p++=digits[j];p+=4;i+=8;}else{*f->_p++=(unsigned char)*p++;++i;}}return (int)i;
}
''',newline='\n')
 aliases=OUT/'allocator_aliases.c';aliases.write_text('extern void *_ZN23__malloc_alloc_templateILi0EE13_S_oom_mallocEj(unsigned);\nextern void *_ZN24__default_alloc_templateILb0ELi0EE9_S_refillEj(unsigned);\nvoid *func_00252168(unsigned n){return _ZN23__malloc_alloc_templateILi0EE13_S_oom_mallocEj(n);}\nvoid *func_002521F8(unsigned n){return _ZN24__default_alloc_templateILb0ELi0EE9_S_refillEj(n);}\n',newline='\n')
 generic=OUT/'memmove.c';raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',PIN+':ee/newlib/libc/string/memmove.c'],capture_output=True,check=True).stdout;generic.write_bytes(raw)
 licensed.append(dict(pin=PIN,path='ee/newlib/libc/string/memmove.c',entire_raw_sha256=SHA(raw),unchanged=True,no_original_helper_award=True))
 legacy=['-Isrc/runtime/stdlib/compat','-I'+str(EE)]
 cflags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Iinclude']
 sources=[('aliases',aliases,[]),('test',ROOT/'tests/native/completion_resolver.c',[]),('selected',ROOT/'src/game/completion_resolver.c',['-DD_003F21B8=_ZN24__default_alloc_templateILb0ELi0EE12_S_free_listE']),('heap',heap,[]),('bindings',bindings,[]),('memmove',generic,['-I'+str(EE),'-DPREFER_SIZE_OVER_SPEED','-Dmemmove=resolver_generic_memmove']),('sprintf',ROOT/'src/runtime/stdio/sprintf.c',legacy+['-Isrc/runtime/stdio','-Dsprintf=func_003952C8','-Dvfprintf=resolver_observe_vfprintf','-D_impure_ptr=D_00405694']),('format',observer,legacy)]
 cppflags=[str(FRONT),'-B'+str(BIN)+'/', '-O2','-fno-exceptions','-fno-weak','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Dmemmove=resolver_generic_memmove','-Isrc/runtime/sgi_resolver/include','-Isrc/runtime/sgi/include','-Isrc/runtime/sgi/libio','-Isrc/runtime/sgi/gcc','-Isrc/runtime/sgi/config','-I'+str(ROOT/'tools/vendor/ps2dev-20181019/MinGW/include')]
 for name,source,extra in sources+[('bridge',cpp,[])]:
  obj=OUT/(name+'.o');cmd=[*(cppflags if name=='bridge' else cflags),*extra,'-c',str(source),'-o',str(obj)]
  assert cmd.count('-o')==1 and Path(cmd[cmd.index('-o')+1]).resolve().parent==OUT
  run=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
  commands.append(dict(command=cmd,returncode=run.returncode,stdout=run.stdout,stderr=run.stderr));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n');assert run.returncode==0,(name,run.stdout,run.stderr)
  if name!='bridge':assert not run.stderr,(name,run.stderr)
  objects.append(str(obj))
 cmd=[str(BIN/'gcc.exe'),'-m32','-Wl,--gc-sections',*objects,'-o',str(OUT/'checks.exe')]
 assert cmd.count('-o')==1 and Path(cmd[cmd.index('-o')+1]).resolve().parent==OUT
 run=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
 commands.append(dict(command=cmd,returncode=run.returncode,stdout=run.stdout,stderr=run.stderr));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n');assert run.returncode==0,(run.stdout,run.stderr)
 run=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60);commands.append(dict(command=[str(OUT/'checks.exe')],returncode=run.returncode,stdout=run.stdout,stderr=run.stderr));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
 (OUT/'reuse.json').write_text(json.dumps(dict(spans=spans,licensed_support=licensed,generated_inputs=[dict(path=str(p),raw_sha256=SHA(p.read_bytes())) for p in (heap,cpp,bindings,observer,generic,aliases)],limits='Initialized actual placement-constructed container/node lifetimes with explicitly measured GNU/native C prefix alias views. Real unchanged SGI methods/static storage and published full heap wrappers/Newlib sprintf execute. Generic memmove models output only, not EE bulk reads. Core/release/vfprintf are controlled; clear is native support only, target unproved deallocate remains unawarded. Inactive historical libio/typename warnings retained.'),indent=2)+'\n')
 assert run.returncode==0,(run.stdout,run.stderr);print(run.stdout,end='')

if __name__=='__main__':main()
