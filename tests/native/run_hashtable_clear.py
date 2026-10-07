"""Separate complete production TU and typed initialized native observers."""
from pathlib import Path
import argparse, json, os, subprocess
ROOT=Path(__file__).resolve().parents[2]

def run(output,source=None,only_fixture=None):
    output=output.resolve();output.relative_to(ROOT);output.mkdir(parents=True,exist_ok=True)
    binary=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
    env=dict(os.environ);env['PATH']=str(binary)+os.pathsep+env['PATH']
    gcc=binary/'gcc.exe'
    flags=['-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-Iinclude']
    commands=[];objects=[]
    for label,path in [('selected',source or ROOT/'src/game/hashtable_clear.c'),('test',ROOT/'tests/native/hashtable_clear.c'),('support',ROOT/'tests/native/hashtable_clear_sgi_support.c')]:
        extra=['-DCLEAR_ONLY_FIXTURE='+str(only_fixture)] if label=='test' and only_fixture is not None else []
        obj=output/(label+'.o');cmd=[str(gcc),*flags,*extra,'-c',str(path),'-o',str(obj)]
        assert Path(cmd[cmd.index('-o')+1]).resolve().parent==output
        r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr))
        (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
        assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr);objects.append(str(obj))
    front=ROOT/'tools/vendor/vector-insert-native323/bin/gcc.exe'
    cppflags=['-B'+str(binary)+'/', '-O2','-fno-exceptions','-fno-weak','-fno-builtin','-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Dmemmove=clear_sgi_memmove','-Isrc/runtime/sgi_resolver/include','-Isrc/runtime/sgi/include','-Isrc/runtime/sgi/libio','-Isrc/runtime/sgi/gcc','-Isrc/runtime/sgi/config','-I'+str(ROOT/'tools/vendor/ps2dev-20181019/MinGW/include')]
    obj=output/'sgi.o';cmd=[str(front),*cppflags,'-c',str(ROOT/'tests/native/hashtable_clear_sgi.cpp'),'-o',str(obj)]
    assert Path(cmd[cmd.index('-o')+1]).resolve().parent==output
    r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr))
    (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert r.returncode==0,(r.stdout,r.stderr);objects.append(str(obj))
    cmd=[str(gcc),'-m32','-Wl,--gc-sections',*objects,'-o',str(output/'checks.exe')]
    assert Path(cmd[cmd.index('-o')+1]).resolve().parent==output
    r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr))
    (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert r.returncode==0 and not r.stderr,(r.stdout,r.stderr)
    r=subprocess.run([str(output/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(output/'checks.exe')],returncode=r.returncode,stdout=r.stdout,stderr=r.stderr))
    (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    (output/'execution.json').write_text(json.dumps(commands[-1],indent=2)+'\n',newline='\n')
    return r

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/native/hashtable_clear')
    args=p.parse_args();r=run(args.output);print(r.stdout,end='');assert r.returncode==0,(r.stdout,r.stderr)
