"""Execute whole unchanged licensed sources; all compile/link outputs contained.

Both selected full sources remain optimized, with native long32 GNU
-fno-strict-aliasing/-fwrapv contracts and separately recorded arithmetic subsets.
"""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
EE=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/ee/include'

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,default=ROOT/'build/native/runtime_copy_compare');parser.add_argument('--strcmp-source',type=Path,default=ROOT/'src/runtime/strcmp.c');parser.add_argument('--strcpy-source',type=Path,default=ROOT/'src/runtime/strcpy.c');a=parser.parse_args()
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True);env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-builtin','-fno-strict-aliasing','-fwrapv'];commands=[];objects=[]
    def run(command):
        if '-o' in command:
            target=Path(command[command.index('-o')+1]).resolve();assert target.is_relative_to(out),(out,target)
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,stdout=r.stdout,stderr=r.stderr,returncode=r.returncode));return r
    def save(): (out/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    abi_source=ROOT/'build/runtime_copy_compare/native_abi.c';obj=out/'native_abi.o';exe=out/'native_abi.exe'
    r=run([*flags,'-c',str(abi_source),'-o',str(obj)]);assert not r.returncode,(r.stdout,r.stderr)
    r=run([flags[0],'-m32',str(obj),'-o',str(exe)]);assert not r.returncode,(r.stdout,r.stderr)
    r=run([str(exe)]);assert not r.returncode and json.loads(r.stdout)==[8,1,4,4,4,4,4,4,4,4,4294967295,2147483647,-2147483648,1],(r.stdout,r.stderr)
    (out/'abi.json').write_text(json.dumps(dict(words=json.loads(r.stdout),compiler_contract=['-fno-strict-aliasing','-fwrapv'],source_raw_sha256=hashlib.sha256(abi_source.read_bytes()).hexdigest()),indent=2)+'\n',newline='\n')
    sources=[('test',ROOT/'tests/native/runtime_copy_compare.c',[]),('strcmp',a.strcmp_source,['-Dstrcmp=compare_real_strcmp']),('strcpy',a.strcpy_source,['-Dstrcpy=compare_real_strcpy'])]
    for name,source,extra in sources:
        obj=out/(name+'.o');includes=[] if name=='test' else ['-I',str(EE)];r=run([*flags,*includes,*extra,'-c',str(source),'-o',str(obj)])
        if r.returncode:save();raise RuntimeError(r.stdout+r.stderr)
        objects.append(str(obj))
    r=run([flags[0],'-m32',*objects,'-o',str(out/'checks.exe')])
    if not r.returncode:r=run([str(out/'checks.exe')])
    save();assert not r.returncode,(r.stdout,r.stderr)
    print(r.stdout,end='');print('retained compiler warning records:',sum(bool(x['stderr']) for x in commands))
if __name__=='__main__':main()
