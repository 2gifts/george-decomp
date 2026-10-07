"""Genuine complete SGI source/header native compilation, no helper substitute."""
from pathlib import Path
import hashlib,json,os,shlex,subprocess,sys
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'build/native/sgi_upper_bound'
B=ROOT/'tools/vendor/ps2dev-20181019/MinGW';F=ROOT/'tools/vendor/vector-insert-native323'
SOURCE=ROOT/'src/runtime/sgi_upper_bound/upper_bound.cpp'
EXTRA_CPPFLAGS=[]
H=lambda b:hashlib.sha256(b).hexdigest()
def record(p):
 p=Path(p).resolve();b=p.read_bytes();return dict(path=p.relative_to(ROOT).as_posix(),bytes=len(b),raw_sha256=H(b))
def main():
 OUT.mkdir(parents=True,exist_ok=True);commands=[];objects=[];dependencies=[];inventories=[]
 env=os.environ.copy();env['PATH']=str(B/'bin')+os.pathsep+str(F/'bin')+os.pathsep+env['PATH']
 flags=[*EXTRA_CPPFLAGS,'-B'+str(B/'bin')+'/', '-O2','-fno-builtin','-fno-exceptions','-fno-weak','-fno-strict-aliasing',
  '-ffunction-sections','-fdata-sections','-Isrc/runtime/include','-Isrc/runtime/sgi_resolver/include',
  '-Isrc/runtime/sgi/include','-Isrc/runtime/sgi/libio','-Isrc/runtime/sgi/gcc','-Isrc/runtime/sgi/config',
  '-Iinclude','-I'+str(B/'include')]
 compiler=str(F/'bin/gcc.exe')
 def run(cmd):
  if '-o' in cmd:assert Path(cmd[cmd.index('-o')+1]).resolve().is_relative_to(OUT.resolve())
  p=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=120)
  q=dict(command=cmd,returncode=p.returncode,stdout=p.stdout,stderr=p.stderr);commands.append(q)
  (OUT/'commands.json').write_bytes((json.dumps(commands,indent=2)+'\n').encode())
  if p.stderr:print(p.stderr,end='')
  assert p.returncode==0,q
  return p
 for name,path in [('selected',SOURCE),('test',ROOT/'tests/native/sgi_upper_bound.cpp')]:
  obj=OUT/(name+'.o');run([compiler,*flags,'-c',str(path),'-o',str(obj)]);objects.append(str(obj))
  m=run([compiler,*flags,'-M',str(path)])
  rhs=m.stdout.split('.o:',1)[1].replace('\\\n',' ')
  for s in (str(ROOT),ROOT.as_posix(),str(ROOT).replace(' ','\\ '),ROOT.as_posix().replace(' ','\\ ')):
   rhs=rhs.replace(s,'__ROOT__')
  paths=[]
  for token in dict.fromkeys(shlex.split(rhs.replace('\\','/'))):
   q=ROOT/token.removeprefix('__ROOT__/');assert q.resolve().is_relative_to(ROOT)
   paths.append(record(q))
  dependencies.append(dict(role=name,command=commands[-1],files=paths))
  nm=run([str(B/'bin/nm.exe'),str(obj)]);dump=run([str(B/'bin/objdump.exe'),'-h',str(obj)])
  inventories.append(dict(object=record(obj),symbols=nm.stdout,sections=dump.stdout))
 exe=OUT/'checks.exe';run([str(B/'bin/gcc.exe'),'-m32',*objects,'-Wl,--gc-sections','-o',str(exe)])
 (OUT/'dependencies.json').write_bytes((json.dumps(dependencies,indent=2)+'\n').encode())
 (OUT/'inventories.json').write_bytes((json.dumps(inventories,indent=2)+'\n').encode())
 p=run([str(exe)]);assert 'PASS' in p.stdout and not p.stderr
 (OUT/'results.json').write_bytes((json.dumps(dict(passed=True,source=record(SOURCE),compiler=record(compiler),
  flags=flags,executable=record(exe),actual_output=p.stdout,whole_source_compiled=True,
  qualification='Initialized typed same-array ranges with int payloads and pointer/reference mutations. GNU32 native alias contract, no target/native class or original source spelling identity.'),indent=2)+'\n').encode())
 print(p.stdout,end='')
if __name__=='__main__':main()
