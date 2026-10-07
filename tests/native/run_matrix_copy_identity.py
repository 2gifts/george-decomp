"""One ordinary separate-TU native build; explicit outputs and read operands."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess
R=Path(__file__).resolve().parents[2];O=Path(__file__).resolve().parent
BIN=R/'tools/vendor/ps2dev-20181019/MinGW/bin';H=lambda b:hashlib.sha256(b).hexdigest()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--copy-source',type=Path,default=R/'src/game/matrix_copy.c')
    parser.add_argument('--identity-source',type=Path,default=R/'src/game/matrix_identity.c')
    a=parser.parse_args();out=a.output.resolve();assert out.is_relative_to(R.resolve())and not out.exists();out.mkdir(parents=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH'];records=[]
    compiler=BIN/'gcc.exe';base=[str(compiler),'-m32','-O2','-Wall','-Wextra','-I',str(R/'include'),'-I',str(O)]
    sources=[('copy',a.copy_source.resolve()),('identity',a.identity_source.resolve()),('harness',O/'matrix_copy_identity.c')]
    for label,source in sources:
        assert source.is_file();obj=out/(label+'.o')
        for kind,tail in [('M',['-M',str(source)]),('compile',['-c',str(source),'-o',str(obj)])]:
            command=base+tail
            if '-o'in command:assert Path(command[command.index('-o')+1]).resolve().is_relative_to(out)
            p=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60)
            (out/(label+'_'+kind+'.stdout')).write_bytes(p.stdout);(out/(label+'_'+kind+'.stderr')).write_bytes(p.stderr)
            records.append(dict(label=label,kind=kind,command=command,returncode=p.returncode,stdout=p.stdout.decode(errors='replace'),stderr=p.stderr.decode(errors='replace')))
            (out/'commands.json').write_bytes((json.dumps(records,indent=2)+'\n').encode())
            assert p.returncode==0,p.stderr.decode(errors='replace')
    exe=out/'checks.exe';command=[str(compiler),'-m32',*[str(out/(label+'.o'))for label,_ in sources],'-o',str(exe)]
    p=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60);records.append(dict(kind='link',command=command,returncode=p.returncode,stdout=p.stdout.decode(errors='replace'),stderr=p.stderr.decode(errors='replace')))
    (out/'commands.json').write_bytes((json.dumps(records,indent=2)+'\n').encode());assert p.returncode==0,p.stderr.decode(errors='replace')
    p=subprocess.run([str(exe)],cwd=R,env=env,capture_output=True,timeout=60)
    (out/'run.stdout').write_bytes(p.stdout);(out/'run.stderr').write_bytes(p.stderr)
    artifact=lambda path:dict(path=path.relative_to(R).as_posix(),bytes=path.stat().st_size,raw_sha256=H(path.read_bytes()))
    packet=dict(compiler=artifact(compiler),commands=records,actual_read_sources=[artifact(source)for _,source in sources],
        output_preflight=str(out),run=dict(command=[str(exe)],returncode=p.returncode,stdout=p.stdout.decode(errors='replace'),stderr=p.stderr.decode(errors='replace')),
        artifacts=[artifact(out/(label+'.o'))for label,_ in sources]+[artifact(exe)],
        limits='Whole new production TUs execute on aligned initialized float storage accessed as unsigned-char object representations for copying. Identity source final contents only; actual host CRT memset standard closure, no retail helper/ordered-store/atomicity/hardware claim.')
    (out/'native.json').write_bytes((json.dumps(packet,indent=2)+'\n').encode());print(p.stdout.decode(),end='')
    return p.returncode
if __name__=='__main__':raise SystemExit(main())
