"""Separate full production/test TUs; no substitute clear or old-node identity."""
from pathlib import Path
import argparse,json,os,subprocess
ROOT=Path(__file__).resolve().parents[2]

def run(output,source=None,only_fixture=None):
    output=output.resolve();output.relative_to(ROOT);output.mkdir(parents=True,exist_ok=True)
    binary=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin';gcc=binary/'gcc.exe'
    env=dict(os.environ);env['PATH']=str(binary)+os.pathsep+env['PATH']
    flags=['-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-Iinclude']
    commands=[];objects=[]
    for label,path in [('selected',source or ROOT/'src/game/hashtable_clear_duplicates.c'),('test',ROOT/'tests/native/hashtable_clear_duplicates.c')]:
        extra=['-DCLEAR_ONLY_FIXTURE='+str(only_fixture)] if label=='test' and only_fixture is not None else []
        obj=output/(label+'.o');cmd=[str(gcc),*flags,*extra,'-c',str(path),'-o',str(obj)]
        assert obj.is_relative_to(output)
        result=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=cmd,returncode=result.returncode,stdout=result.stdout,stderr=result.stderr))
        (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
        assert result.returncode==0 and not result.stderr,commands[-1];objects.append(str(obj))
    cmd=[str(gcc),'-m32',*objects,'-o',str(output/'checks.exe')]
    result=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=cmd,returncode=result.returncode,stdout=result.stdout,stderr=result.stderr))
    (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    assert result.returncode==0 and not result.stderr,commands[-1]
    result=subprocess.run([str(output/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
    commands.append(dict(command=[str(output/'checks.exe')],returncode=result.returncode,stdout=result.stdout,stderr=result.stderr))
    (output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n')
    (output/'execution.json').write_text(json.dumps(commands[-1],indent=2)+'\n',newline='\n')
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/native/hashtable_clear_duplicates')
    a=p.parse_args();r=run(a.output);print(r.stdout,end='');assert r.returncode==0,(r.stdout,r.stderr)
