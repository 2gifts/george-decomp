"""Run real completion C with exact published helper spans and genuine Newlib.
Unknown resolver/insertion/core/vfprintf contracts are authored native observers;
complete licensed sprintf/atexit wrappers and published append execute unchanged.
"""
import hashlib,json,os,re,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from analyze import validated_elf
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'
OUT=ROOT/'build/native/buffer_completion'
SELECTED=ROOT/'src/game/buffer_completion.c'
REVISION='b595ded606227e93b8c4a447446c1d2ac093827d'
SHA=lambda b:hashlib.sha256(b).hexdigest()
def function(raw,name):
 found=re.search(rb'(?m)^[A-Za-z_][^;\n]*\b'+name.encode()+rb'\(',raw);assert found,name
 start=found.start();opening=raw.index(b'{',raw.index(name.encode()+b'(',start));depth=1;end=opening+1
 while depth:
  b=raw[end];depth+=(b==123)-(b==125);end+=1
 return start,end,raw[start:end]
def main():
 OUT.mkdir(parents=True,exist_ok=True);_,original=validated_elf(ROOT/'orig/SLUS_216.68');spans=[];generated=[];licensed=[]
 def extract(path,names,prefix):
  raw=(ROOT/path).read_bytes();parts=[]
  for name in names:
   start,end,part=function(raw,name);parts.append(part);spans.append(dict(source=path,name=name,whole_source_raw_sha256=SHA(raw),raw_offset=start,raw_size=end-start,complete_raw_sha256=SHA(part)))
  return prefix+b'\n'+b'\n\n'.join(parts)+b'\n'
 length=OUT/'length.c';length.write_bytes(extract('src/game/string_algorithms.c',['func_00295050'],b'#include "george/string_algorithms.h"'))
 startup=OUT/'upper.c';raw=(ROOT/'src/game/startup_services.c').read_bytes();line=b'GEORGE_DEFINE_UPPER_BOUND(func_00100C30)';assert raw.count(line)==1
 startup.write_bytes(b'#include "george/startup.h"\n#include "george/algorithm_templates.h"\n'+line+b'\n')
 spans.append(dict(source='src/game/startup_services.c',whole_source_raw_sha256=SHA(raw),raw_offset=raw.index(line),raw_size=len(line),complete_raw_sha256=SHA(line),complete_macro_invocation=True))
 raw=(ROOT/'src/game/heap.c').read_bytes();start=raw.index(b'#define DEFINE_ALLOCATION(');end=raw.index(b'\nDEFINE_ALLOCATION(',start)
 macro=raw[start:end];line=b'DEFINE_ALLOCATION(func_002AEE60, (u32 size), D_003FD204, 0x28, 4)';assert raw.count(line)==1
 heap=OUT/'heap.c';heap.write_bytes(b'#include "george/heap.h"\nextern GeorgeHeap *D_003FD204;\nextern const char D_00447238[];\nextern s32 func_00394F68(const char *,...);\n'+macro+b'\n'+line+b'\n')
 for begin,data,label in ((start,macro,'entire allocation macro'),(raw.index(line),line,'entire selected macro invocation')):spans.append(dict(source='src/game/heap.c',whole_source_raw_sha256=SHA(raw),raw_offset=begin,raw_size=len(data),complete_raw_sha256=SHA(data),label=label))
 bindings=OUT/'bindings.c';parts=[]
 for name,address,n in (('D_004208A0',0x4208A0,16),('D_0043A690',0x43A690,2),('D_00447F48',0x447F48,27),('D_00447238',0x447238,44)):
  raw=original[address-0xFF000:address-0xFF000+n];parts.append('const %s %s[]={%s};'%('unsigned char' if name=='D_004208A0' else 'char',name,','.join(str(v) for v in raw)))
  generated.append(dict(name=name,size=n,sha256=SHA(raw),local_readonly_input=True,no_data_credit=True))
 bindings.write_text('\n'.join(parts)+'\n',newline='\n')
 adapter=OUT/'legacy_observer.c';adapter.write_text(r"""#include <stdio.h>
#include <reent.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include "george/types.h"
extern void completion_event(u32,u32,u32,u32);
extern void completion_formatter_event(void *,u32);
extern u32 completion_address(const void *);
extern const char D_00447F48[];
void completion_legacy_layout(void) {
 typedef char a[(offsetof(struct _reent,_atexit)==0x148)?1:-1];
 typedef char b[(offsetof(struct _reent,_atexit0)==0x14C)?1:-1];
 typedef char c[(sizeof(struct _atexit)==0x88)?1:-1];
 (void)sizeof(a);(void)sizeof(b);(void)sizeof(c);
}
void *completion_exit_malloc(unsigned size){(void)size;abort();return NULL;}
int completion_observe_vfprintf(FILE *f,const char *fmt,va_list ap){
 unsigned key=va_arg(ap,unsigned);char digits[8];unsigned i;const char *p=fmt;
 if(fmt!=D_00447F48||f->_flags!=0x208||f->_w!=0x7FFFFFFF||f->_bf._size!=0x7FFFFFFF)abort();
 for(i=0;i<8;++i)digits[7-i]="0123456789abcdef"[(key>>(i*4))&15];
 completion_formatter_event(f->_p,key);
 for(i=0;*p;++p){if(p[0]=='%'&&p[1]=='0'&&p[2]=='8'&&p[3]=='x'){unsigned j;for(j=0;j<8;++j)*f->_p++=digits[j];p+=3;i+=8;}else{*f->_p++=(unsigned char)*p;++i;}}
 return (int)i;
}
""",newline='\n')
 sources=[('test',ROOT/'tests/native/buffer_completion.c',[]),('selected',SELECTED,[]),('upper',startup,[]),('heap',heap,[]),('length',length,['-Dfunc_00295050=completion_real_length']),('manager',ROOT/'src/game/buffer_manager.c',['-Dfunc_002A4DB8=completion_real_append']),('bindings',bindings,[])]
 for name in ('strcpy','strncmp'):
  path='ee/newlib/libc/string/'+name+'.c';raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REVISION+':'+path],capture_output=True,check=True).stdout
  target=OUT/('generic_'+name+'.c');target.write_bytes(raw);sources.append((name,target,['-I',str(EE),'-DPREFER_SIZE_OVER_SPEED','-Wno-parentheses','-D'+name+'=completion_generic_'+name]))
  licensed.append(dict(revision=REVISION,path=path,raw_sha256=SHA(raw),unchanged=True,no_target_helper_award=True))
 legacy=['-I',str(ROOT/'src/runtime/stdlib/compat'),'-I',str(EE),'-D_impure_ptr=D_00405694']
 sources += [('atexit',ROOT/'src/runtime/stdlib/atexit.c',legacy+['-Datexit=completion_real_atexit','-Dmalloc=completion_exit_malloc']),('sprintf',ROOT/'src/runtime/stdio/sprintf.c',legacy+['-I',str(ROOT/'src/runtime/stdio'),'-Dsprintf=completion_real_sprintf','-Dvfprintf=completion_observe_vfprintf']),('observer',adapter,legacy)]
 walkers=OUT/'walkers.c';walkers.write_bytes(extract('src/game/deimos_values.c',['func_002CDA20'],b'#include \"george/deimos.h\"')+extract('src/game/deimos_tables.c',['func_002CDD88','func_002CDDB0'],b'#include \"george/deimos_tables.h\"\n#include \"george/algorithm_templates.h\"\nextern GeorgeGenericMap *D_00481760;')+b'GEORGE_DEFINE_MAP_VISITOR(func_002A8130)\n')
 published=(ROOT/'src/game/deimos_tables.c').read_bytes();invocation=b'GEORGE_DEFINE_MAP_VISITOR(func_002A8130)';assert published.count(invocation)==1
 spans.append(dict(source='src/game/deimos_tables.c',whole_source_raw_sha256=SHA(published),raw_offset=published.index(invocation),raw_size=len(invocation),complete_raw_sha256=SHA(invocation),complete_macro_invocation=True))
 sources.append(('walkers',walkers,[]))
 # PE resolves unused manager sections too: retain supporting declarations as
 # failing stubs, never substitutes for the actual selected append closure.
 stubs=OUT/'unexecuted.c';stubs.write_text('#include "george/types.h"\n#include <stdlib.h>\nu32 func_002AAF88(const void *p){(void)p;abort();return 0;}\nu32 *func_002AAF50(void *p,s32 i){(void)p;(void)i;abort();return 0;}\nchar *func_00394010(char *a,const char *b,u32 n){(void)a;(void)b;(void)n;abort();return 0;}\nchar *func_00398628(const char *a,const char *b){(void)a;(void)b;abort();return 0;}\n',newline='\n');sources.append(('unexecuted',stubs,[]));sources.append(('ctype',ROOT/'src/runtime/ctype_table.c',['-I',str(EE)]))
 env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH'];commands=[];objects=[]
 flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-I',str(ROOT/'include')]
 for name,source,extra in sources:
  obj=OUT/(name+'.o');cmd=[*flags,*extra,'-c',str(source),'-o',str(obj)];run=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
  commands.append(dict(command=cmd,stdout=run.stdout,stderr=run.stderr,exit_code=run.returncode));assert not run.returncode and not run.stderr,(name,run.stdout,run.stderr);objects.append(str(obj))
 cmd=[flags[0],'-m32','-Wl,--gc-sections',*objects,'-o',str(OUT/'checks.exe')];run=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
 commands.append(dict(command=cmd,stdout=run.stdout,stderr=run.stderr,exit_code=run.returncode));assert not run.returncode and not run.stderr,(run.stdout,run.stderr)
 run=subprocess.run([str(OUT/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=120)
 commands.append(dict(command=[str(OUT/'checks.exe')],stdout=run.stdout,stderr=run.stderr,exit_code=run.returncode));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
 (OUT/'reuse.json').write_text(json.dumps(dict(complete_raw_spans=spans,licensed_strings=licensed,readonly_inputs=generated,generated_TUs=[dict(path=str(source),raw_sha256=SHA(source.read_bytes())) for name,source,extra in sources if source.parent==OUT],limitations='Controlled supporting ctor/slot resolver/insertion/core/vfprintf engine; real full published append/upperbound/length/heap wrapper and genuine unchanged Newlib sprintf/atexit/string bodies execute. No supporting recovery or stack/bulk-read/data credit.'),indent=2)+'\n')
 assert not run.returncode and not run.stderr,(run.stdout,run.stderr);print(run.stdout,end='')
if __name__=='__main__':main()
