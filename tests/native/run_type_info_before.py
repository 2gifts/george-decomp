"""Exact GNU method/destructor byte spans and authentic headers, native only.

Target method uses name0/bool4; this host uses name4/bool1. Typed constructors
bridge semantic name fields only. No target ABI identity or helper award.
"""
from pathlib import Path
import hashlib,json,os,subprocess
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'build/native/type_info_before'
B=ROOT/'tools/vendor/ps2dev-20181019/MinGW';F=ROOT/'tools/vendor/vector-insert-native323'
PIN='b595ded606227e93b8c4a447446c1d2ac093827d';SHA=lambda b:hashlib.sha256(b).hexdigest()
# Deliberate failing observer mutation only; default always copies exact method.
WRONG_SIGN=False
def main():
 OUT.mkdir(parents=True,exist_ok=True);records=[];spans=[]
 def extract(path,start,end):
  raw=(ROOT/path).read_bytes();lo=raw.index(start);hi=raw.index(end,lo)+len(end);span=raw[lo:hi]
  spans.append(dict(source=path,source_size=len(raw),source_raw_sha256=SHA(raw),offset=lo,size=len(span),span_raw_sha256=SHA(span),unchanged=True))
  return span
 method=extract('src/runtime/gcc/cp/tinfo2.cc',b'bool\ntype_info::before',b'\n}')
 assert len(method)==99 and SHA(method)=='555b66b5da62501ba795aa0fe629ab5d6335e8a3163ab88f9ae4196fb76b059c'
 if WRONG_SIGN:method=method.replace(b'< 0',b'> 0')
 destructor=extract('src/runtime/gcc/cp/tinfo.cc',b'std::type_info::\n~type_info ()',b'{ }')
 equality=extract('src/runtime/gcc/cp/tinfo.cc',b'bool type_info::\noperator==',b'\n}')
 pragma=extract('src/runtime/gcc/cp/tinfo.cc',b'#pragma implementation',b'"typeinfo"')
 raw=(ROOT/'src/runtime/gcc/cp/tinfo2.cc').read_bytes();notice=raw[:raw.index(b'/* CYGNUS LOCAL embedded c++ */')]
 spans.append(dict(source='src/runtime/gcc/cp/tinfo2.cc',source_size=len(raw),source_raw_sha256=SHA(raw),offset=0,size=len(notice),span_raw_sha256=SHA(notice),unchanged=True,whole_upstream_notice=True))
 raw2=(ROOT/'src/runtime/gcc/cp/tinfo.cc').read_bytes();notice2=raw2[:raw2.index(b'#pragma implementation')]
 spans.append(dict(source='src/runtime/gcc/cp/tinfo.cc',source_size=len(raw2),source_raw_sha256=SHA(raw2),offset=0,size=len(notice2),span_raw_sha256=SHA(notice2),unchanged=True,whole_upstream_notice=True))
 generated=notice+notice2+b'\n#include <string.h>\n'+pragma+b'\n#include <typeinfo>\nusing std::type_info;\n'+method+b'\n'+destructor+b'\n'+equality+b'\n'
 (OUT/'genuine_before.cpp').write_bytes(generated)
 blob=subprocess.check_output(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',PIN+':ee/newlib/libc/string/strcmp.c'])
 (OUT/'generic_strcmp.c').write_bytes(blob)
 env=os.environ.copy();env['PATH']=str(B/'bin')+os.pathsep+str(F/'bin')+os.pathsep+env['PATH']
 cflags=[str(B/'bin/gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing']
 cppflags=[str(F/'bin/gcc.exe'),'-B'+str(B/'bin')+'/', '-O2','-fno-builtin','-fno-exceptions','-fno-rtti','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Isrc/runtime/gcc/cp/inc','-I'+str(B/'include')]
 def run(cmd):
  if '-o' in cmd:assert Path(cmd[cmd.index('-o')+1]).resolve().is_relative_to(OUT.resolve())
  p=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
  records.append(dict(command=cmd,returncode=p.returncode,stdout=p.stdout,stderr=p.stderr));(OUT/'commands.json').write_text(json.dumps(records,indent=2)+'\n')
  if p.stderr:print(p.stderr,end='')
  assert p.returncode==0,(p.stdout,p.stderr);return p
 objects=[];dependencies=[];inventory=[]
 for name,path,flags,extra in [('test',ROOT/'tests/native/type_info_before.cpp',cppflags,[]),('genuine',OUT/'genuine_before.cpp',cppflags,['-Dstrcmp=before_support_strcmp']),('strcmp',OUT/'generic_strcmp.c',cflags,['-I'+str(B/'msys/1.0/local/ps2dev/ee/ee/include'),'-DPREFER_SIZE_OVER_SPEED','-Dstrcmp=before_support_strcmp'])]:
  obj=OUT/(name+'.o');run([*flags,*extra,'-c',str(path),'-o',str(obj)]);objects.append(str(obj))
  dep=run([*flags,*extra,'-M',str(path)]);dependencies.append(dict(name=name,command=records[-1]['command'],stdout=dep.stdout,stderr=dep.stderr))
  nm=run([str(B/'bin/nm.exe'),str(obj)]);inventory.append(dict(name=name,path=str(obj),size=len(obj.read_bytes()),raw_sha256=SHA(obj.read_bytes()),symbols=nm.stdout))
 run([str(B/'bin/gcc.exe'),'-m32',*objects,'-Wl,--gc-sections','-o',str(OUT/'checks.exe')]);p=run([str(OUT/'checks.exe')]);print(p.stdout,end='')
 (OUT/'source_reuse.json').write_text(json.dumps(dict(spans=spans,generated=dict(path='genuine_before.cpp',size=len(generated),raw_sha256=SHA(generated)),generic_strcmp=dict(revision=PIN,upstream='ee/newlib/libc/string/strcmp.c',size=len(blob),raw_sha256=SHA(blob),unchanged=True,native_support_only=True,license='newlib1.8.1 default Cygnus1994/1997 clause9'),
     qualification='Exact whole method/destructor/operator==/pragma/notices; unchanged authentic typeinfo and exception headers. Destructor/equality/host runtime/generic strcmp receive no target award. Host typed NameProbe uses actual protected constructor. Native name4/bool1/vptr0 differ from measured target name0/bool4/vptr4; logical name fields/result bridged, no ABI or class-lifetime identity claim. Native-only no-rtti omits unused host RTTI metadata. Deleting-dtor operator-delete and unused exception::what are typed link-only abort controls, counters remain zero; exception/RTTI/what/deleting-dtor paths excluded.'),indent=2)+'\n')
 (OUT/'dependencies.json').write_text(json.dumps(dependencies,indent=2)+'\n')
 (OUT/'symbols.json').write_text(json.dumps(inventory,indent=2)+'\n')
if __name__=='__main__':main()
