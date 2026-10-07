"""Private separate-TU native observer; bounded nominal integer lattice and raw translation moves."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess
R=Path(__file__).resolve().parents[2];O=Path(__file__).resolve().parent
BIN=R/'tools/vendor/ps2dev-20181019/MinGW/bin'
H=lambda b:hashlib.sha256(b).hexdigest()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,default=R/'build/native/matrix_basis');parser.add_argument('--source',type=Path,default=R/'src/game/matrix_basis.c')
    args=parser.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    assert out.is_relative_to(R.resolve());source=args.source.resolve();assert source.is_file()
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    compiler=BIN/'gcc.exe'
    version=subprocess.run([str(compiler),'--version'],cwd=R,env=env,capture_output=True,timeout=60)
    assert version.returncode==0 and not version.stderr
    base=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing','-msse2','-mfpmath=sse','-I',str(R/'include'),'-I',str(O)]
    records=[]
    for label,input_path in(('production',source),('harness',O/'matrix_basis.c')):
        obj=out/(label+'.o')
        for kind,tail in(('M',['-M',str(input_path)]),('compile',['-c',str(input_path),'-o',str(obj)])):
            command=base+tail
            if '-o'in command:assert Path(command[command.index('-o')+1]).resolve().is_relative_to(out)
            result=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60)
            (out/(label+'_'+kind+'.stdout')).write_bytes(result.stdout);(out/(label+'_'+kind+'.stderr')).write_bytes(result.stderr)
            records.append(dict(label=label,kind=kind,command=command,returncode=result.returncode,stdout=result.stdout.decode(errors='replace'),stderr=result.stderr.decode(errors='replace')))
            assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
    exe=out/'checks.exe';command=[str(BIN/'gcc.exe'),'-m32',str(out/'production.o'),str(out/'harness.o'),'-o',str(exe)]
    result=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60)
    records.append(dict(kind='link',command=command,returncode=result.returncode,stdout=result.stdout.decode(errors='replace'),stderr=result.stderr.decode(errors='replace')))
    assert result.returncode==0 and not result.stderr
    result=subprocess.run([str(exe)],cwd=R,env=env,capture_output=True,timeout=60)
    (out/'run.stdout').write_bytes(result.stdout);(out/'run.stderr').write_bytes(result.stderr)
    packet=dict(compiler=dict(path=str(compiler.relative_to(R)),whole_RAW_SHA=H(compiler.read_bytes()),version_command=[str(compiler),'--version'],version_stdout=version.stdout.decode(),version_stderr=version.stderr.decode()),
        output_preflight=dict(actual_output_root=str(out),
            private_compile_outputs=[str(out/'production.o'),str(out/'harness.o')],private_link_output=str(exe),
            actual_read_inputs=[str(p.resolve())for p in(source,R/'include/george/matrix_basis.h',O/'matrix_basis.c',O/'matrix_basis_golden.h')]),
        commands=records,run=dict(command=[str(exe)],returncode=result.returncode,stdout=result.stdout.decode(errors='replace'),stderr=result.stderr.decode(errors='replace')),
        inputs=[dict(path=str(p.relative_to(R)),raw_SHA=H(p.read_bytes()),bytes=p.stat().st_size)for p in(source,R/'include/george/matrix_basis.h',O/'matrix_basis.c',O/'matrix_basis_golden.h')],
        artifacts=[dict(path=str(p.relative_to(R)),raw_SHA=H(p.read_bytes()),bytes=p.stat().st_size)for p in(out/'production.o',out/'harness.o',exe)],
        limits='Ordinary C effects execute as a separate TU on initialized typed float arrays. Nominal binary32 comparisons only: bounded coefficients and raw translation on a full initialized arena. Separate typed objects use ordinary C; shifted struct views use the explicit GNU alias/storage compiler contract. No hardware arithmetic/flags/timing/concurrency or original class identity.')
    (out/'native.json').write_text(json.dumps(packet,indent=2)+'\n')
    print(result.stdout.decode(),end='');assert result.returncode==0 and not result.stderr
if __name__=='__main__':main()
