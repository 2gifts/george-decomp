"""Compile the five entire unchanged licensed C sources as genuine native TUs.

Native GNU long-pointer/wrapping contract is explicit; target defaults unchanged.
No size-over-speed/source-body substitute, unawarded helper or game allocator.
"""
import hashlib,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'build/native/runtime_strings'
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'

def main():
    OUT.mkdir(parents=True,exist_ok=True);env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-fwrapv']
    commands=[];objects=[]
    def run(command):
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,returncode=r.returncode))
        return r
    abi_source=ROOT/'build/runtime_strings/native_abi.c';abi_object=OUT/'native_abi.o';abi_exe=OUT/'native_abi.exe'
    r=run([*flags,'-c',str(abi_source),'-o',str(abi_object)]);assert r.returncode==0,(r.stdout,r.stderr)
    r=run([flags[0],'-m32',str(abi_object),'-o',str(abi_exe)]);assert r.returncode==0,(r.stdout,r.stderr)
    r=run([str(abi_exe)]);assert r.returncode==0 and json.loads(r.stdout)==[8,1,4,4,4,4,4,4,4,4,4294967295,2147483647,-2147483648,1],(r.stdout,r.stderr)
    (OUT/'abi.json').write_text(json.dumps(dict(words=json.loads(r.stdout),compiler_contract=['-fno-strict-aliasing','-fwrapv'],source_raw_sha256=hashlib.sha256(abi_source.read_bytes()).hexdigest()),indent=2)+'\n',newline='\n')
    for name in ('test','memchr','memset','strncat','strncmp','strncpy'):
        source=ROOT/'tests/native/runtime_strings.c' if name=='test' else ROOT/'src/runtime'/(name+'.c')
        extra=[] if name=='test' else ['-I',str(EE),'-D'+name+'=runtime_strings_real_'+name]
        obj=OUT/(name+'.o');r=run([*flags,*extra,'-c',str(source),'-o',str(obj)])
        if r.returncode:
            (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n');raise RuntimeError(r.stdout+r.stderr)
        objects.append(str(obj))
    r=run([flags[0],'-m32',*objects,'-o',str(OUT/'checks.exe')])
    if not r.returncode:r=run([str(OUT/'checks.exe')])
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert not r.returncode,(r.stdout,r.stderr)
    print(r.stdout,end='')
    print('retained compiler warning records:',sum(bool(x['stderr']) for x in commands))
if __name__=='__main__':main()
