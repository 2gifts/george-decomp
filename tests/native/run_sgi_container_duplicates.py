"""Fresh finite fixtures over cached genuine SGI native counterpart objects.

Only this new fixture harness compiles. Complete historical object methods,
typed static storage/placement bridge and helper objects are reused unchanged.
Their old counts and unrelated methods receive no new recovery/source award.
"""
from pathlib import Path
import argparse, hashlib, json, os, subprocess

ROOT=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,default=ROOT/'build/native/sgi_container_duplicates')
OUT=parser.parse_args().output.resolve()
OUT.relative_to(ROOT)
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
H=lambda b:hashlib.sha256(b).hexdigest()

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    env=dict(os.environ);env['PATH']=str(BIN)+os.pathsep+env['PATH']
    source=ROOT/'tests/native/sgi_container_duplicates.c'
    inherited=ROOT/'build/native/completion_resolver'
    names=('bridge','heap','aliases','memmove','bindings','selected','sprintf','format')
    records=[]
    for name in names:
        p=inherited/(name+'.o');raw=p.read_bytes()
        records.append(dict(path=p.relative_to(ROOT).as_posix(),bytes=len(raw),raw_sha256=H(raw),
                            role='Whole immutable old native support object; only reached full genuine methods and helper paths observed, no new source/helper/data award.'))
    obj=OUT/'test.o';exe=OUT/'checks.exe';commands=[]
    command=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror',
             '-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections',
             '-Iinclude','-c',str(source),'-o',str(obj)]
    assert command.count('-o')==1 and Path(command[command.index('-o')+1]).resolve().parent==OUT
    p=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=120)
    commands.append(dict(command=command,returncode=p.returncode,stdout=p.stdout,stderr=p.stderr))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    command=[str(BIN/'gcc.exe'),'-m32','-Wl,--gc-sections',str(obj),
             *[str(inherited/(name+'.o')) for name in names],'-o',str(exe)]
    assert command.count('-o')==1 and Path(command[command.index('-o')+1]).resolve().parent==OUT
    p=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=command,returncode=p.returncode,stdout=p.stdout,stderr=p.stderr))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    p=subprocess.run([str(exe)],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(exe)],returncode=p.returncode,stdout=p.stdout,stderr=p.stderr))
    (OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    for x in records:assert H((ROOT/x['path']).read_bytes())==x['raw_sha256']
    (OUT/'reuse.json').write_text(json.dumps(dict(inherited_whole_objects=records,
        inherited_frontend_evidence='build/completion_resolver/identity_packet.json; build/native/completion_resolver/commands.json and reuse.json',
        fresh_source=dict(path=source.relative_to(ROOT).as_posix(),raw_sha256=H(source.read_bytes())),
        fresh_object=dict(path=obj.relative_to(ROOT).as_posix(),raw_sha256=H(obj.read_bytes())),
        native_PE=dict(path=exe.relative_to(ROOT).as_posix(),bytes=len(exe.read_bytes()),raw_sha256=H(exe.read_bytes())),
        qualification='Fresh clone-PC fixture outcomes, whole finite arena/globals/events against genuine inherited SGI methods. Old constructed node12/pair/value8/native prefix/lifetime are a counterpart only. Historical upstream warnings are inherited; only this new C harness is warning-free. Nonnull OOM callback, controlled core/release/diagnostic, finite initialized storage, GNU prefix alias contract and generic memmove effects only. Null-handler stream/exit branch remains unexecuted; fail hook retained. No original class/type/heap/EE/native pointer hashing, helper/data or old493/native8094485 credit.'),indent=2)+'\n',newline='\n')
    print(p.stdout,end='')

if __name__=='__main__':main()
