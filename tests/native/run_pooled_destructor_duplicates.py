"""Exact published body/macro spans, five host instantiations; no target edits."""
from pathlib import Path
import argparse,ctypes,hashlib,json,os,subprocess
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'build/native/pooled_destructor_duplicates'
NAMES=('func_0019A3D0','func_0019D3F0','func_001F0120','func_0022CB40','func_00276C60')
H=lambda b:hashlib.sha256(b).hexdigest()
def record(p):
 b=p.read_bytes();return dict(path=p.resolve().relative_to(ROOT).as_posix(),bytes=len(b),raw_sha256=H(b))
def extract():
 p=ROOT/'src/game/actor_controller.c';raw=p.read_bytes();assert H(raw)=='51b175709e58fefaa60f9152662676ab1da217923b4bc57a375d9dd81d536a23'
 needles=[b'#include "george/actor_controller.h"',b'extern void *D_004961F4;',b'extern void func_003064F0(void *, u32);',b'extern void func_002E2CD8(void *, void *, u32, u32);',b'#define ADDRESS(object, offset)',b'#define FIELD(object, offset, type)']
 pieces=[];spans=[]
 for needle in needles:
  start=raw.index(needle);end=raw.index(b'\n',start)+1;part=raw[start:end];pieces.append(part);spans.append(dict(start=start,end=end,bytes=len(part),raw_sha256=H(part)))
 start=raw.index(b'void func_0016CED0(');end=raw.index(b'\n}',start)+2
 if raw[end:end+2]==b'\r\n':end+=2
 elif raw[end:end+1]==b'\n':end+=1
 part=raw[start:end];pieces.append(part);spans.append(dict(start=start,end=end,bytes=len(part),raw_sha256=H(part)))
 return b''.join(pieces),dict(entire_published_source=record(p),entire_published_header=record(ROOT/'include/george/actor_controller.h'),exact_byte_spans=spans,qualification='Whole cached TU is target source. Host-only exact lexical source/macro/extern extraction; five explicit preprocessor symbol aliases. No normalization/reimplementation/whole-native-TU identity for generated helpers. Entire inherited native actor_controller.c also compiled unchanged in harness solely for controlled helpers; old1488 checks not run/count.')
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--mutant',choices=('size','manager','mode'));a=ap.parse_args()
 OUT.mkdir(parents=True,exist_ok=True);compiler=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin/gcc.exe';env=os.environ.copy();env['PATH']=str(compiler.parent)+os.pathsep+env.get('PATH','')
 if os.name=='nt':ctypes.windll.kernel32.SetErrorMode(3)
 source,identity=extract();original=source
 if a.mutant=='size':
  source=source.replace(b'    func_003064F0(object, 0);',b'    u16 captured_size = FIELD(object, 4, u16);\n    func_003064F0(object, 0);').replace(b'object, FIELD(object, 4, u16), 0x27',b'object, captured_size, 0x27')
 elif a.mutant=='manager':
  source=source.replace(b'    func_003064F0(object, 0);',b'    void *captured_manager = D_004961F4;\n    func_003064F0(object, 0);').replace(b'func_002E2CD8(D_004961F4, object',b'func_002E2CD8(captured_manager, object')
 elif a.mutant=='mode':source=source.replace(b'(mode & 1U) != 0',b'mode != 0')
 assert (source==original)==(a.mutant is None)
 common=[str(compiler),'-m32','-O2','-Wall','-Wextra','-Werror=implicit-function-declaration','-fno-strict-aliasing','-I',str(ROOT/'include')];commands=[];objects=[];deps=[]
 path=OUT/'published_body.c';path.write_bytes(source);identity['generated_source']=record(path);identity['mutant']=a.mutant
 for name in NAMES:
  obj=OUT/(name+'.o');cmd=[*common,'-Dfunc_0016CED0='+name,'-c',str(path),'-o',str(obj)];r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True)
  commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout.decode(errors='replace'),stderr=r.stderr.decode(errors='replace')));assert not r.returncode,commands[-1]
  objects.append(record(obj));d=subprocess.run([*common,'-Dfunc_0016CED0='+name,'-M',str(path)],cwd=ROOT,env=env,capture_output=True);assert not d.returncode
  deps.append(dict(command=[*common,'-Dfunc_0016CED0='+name,'-M',str(path)],returncode=d.returncode,stdout=d.stdout.decode(errors='replace'),stderr=d.stderr.decode(errors='replace')))
 exe=OUT/'semantic_harness.exe';cmd=[*common,str(ROOT/'tests/native/pooled_destructor_duplicates.c'),*[str(ROOT/x['path'])for x in objects],'-lm','-o',str(exe)];r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True);commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout.decode(errors='replace'),stderr=r.stderr.decode(errors='replace')));assert not r.returncode,commands[-1]
 dcmd=[*common,'-M',str(ROOT/'tests/native/pooled_destructor_duplicates.c')];d=subprocess.run(dcmd,cwd=ROOT,env=env,capture_output=True);assert not d.returncode;deps.append(dict(command=dcmd,returncode=d.returncode,stdout=d.stdout.decode(errors='replace'),stderr=d.stderr.decode(errors='replace')))
 run=subprocess.run([str(exe)],cwd=ROOT,env=env,capture_output=True,timeout=40);commands.append(dict(command=[str(exe)],returncode=run.returncode,stdout=run.stdout.decode(errors='replace'),stderr=run.stderr.decode(errors='replace')))
 q=dict(extraction=identity,actual_commands=commands,actual_M=deps,whole_objects=objects,whole_executable=record(exe),compiler=record(compiler),runner=record(Path(__file__)),native_qualification='Actual five host function symbols on initialized aligned32-bit carriers. Controlled base/release callbacks, no pool/SDK/lifetime/vtable/full-object ABI or hardware identity; only effects. GNU native integer-pointer and valid representation contract; existing unchanged full old native TU supplies callbacks, no old method counts included.')
 (OUT/'commands.json').write_text(json.dumps(q,indent=2)+'\n',newline='\n');print(run.stdout.decode(errors='replace'),end='')
 if a.mutant:assert run.returncode!=0 and 'FAIL'in run.stderr.decode(errors='replace'),q
 else:assert not run.returncode,q
if __name__=='__main__':main()
