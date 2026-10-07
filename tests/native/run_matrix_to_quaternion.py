"""Separate unchanged source TU, initialized host scalar/table contract only."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

R=Path(__file__).resolve().parents[2];T=Path(__file__).resolve().parent
BIN=R/'tools/vendor/ps2dev-20181019/MinGW/bin'
H=lambda b:hashlib.sha256(b).hexdigest()

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=R/'build/native/matrix_to_quaternion')
    p.add_argument('--source',type=Path,default=R/'src/game/matrix_to_quaternion.c')
    p.add_argument('--golden',type=Path,default=T/'matrix_to_quaternion_golden.h')
    p.add_argument('--report',type=Path)
    a=p.parse_args();out=a.output.resolve();source=a.source.resolve();golden=a.golden.resolve()
    assert out.is_relative_to(R.resolve()) and source.is_file() and golden.is_file()
    out.mkdir(parents=True,exist_ok=True)
    if a.report:assert a.report.resolve().is_relative_to(out)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    compiler=BIN/'gcc.exe'
    base=[str(compiler),'-m32','-O2','-Wall','-Wextra','-fno-strict-aliasing',
          '-msse2','-mfpmath=sse','-DD_003FC940=matrix_quaternion_observer_table',
          '-I',str(R/'include'),'-I',str(golden.parent)]
    records=[]
    for label,input_path in (('production',source),('harness',T/'matrix_to_quaternion.c')):
        obj=out/(label+'.o')
        for kind,tail in (('M',['-M',str(input_path)]),('compile',['-c',str(input_path),'-o',str(obj)])):
            command=base+tail
            if '-o' in command:assert Path(command[command.index('-o')+1]).resolve().is_relative_to(out)
            result=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60)
            (out/(label+'_'+kind+'.stdout')).write_bytes(result.stdout)
            (out/(label+'_'+kind+'.stderr')).write_bytes(result.stderr)
            records.append(dict(label=label,kind=kind,command=command,returncode=result.returncode,
                                stdout=result.stdout.decode(errors='replace'),stderr=result.stderr.decode(errors='replace')))
            assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
    exe=out/'checks.exe'
    command=[str(compiler),'-m32',str(out/'production.o'),str(out/'harness.o'),'-lm','-o',str(exe)]
    result=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60)
    records.append(dict(kind='link',command=command,returncode=result.returncode,
                        stdout=result.stdout.decode(errors='replace'),stderr=result.stderr.decode(errors='replace')))
    assert result.returncode==0 and not result.stderr
    result=subprocess.run([str(exe)],cwd=R,env=env,capture_output=True,timeout=60)
    (out/'run.stdout').write_bytes(result.stdout);(out/'run.stderr').write_bytes(result.stderr)
    packet=dict(output_preflight=dict(output=str(out),
           actual_read_inputs=[str(p.resolve()) for p in (source,T/'matrix_to_quaternion.c',golden,R/'include/george/matrix_to_quaternion.h')]),
       commands=records,run=dict(command=[str(exe)],returncode=result.returncode,
           stdout=result.stdout.decode(errors='replace'),stderr=result.stderr.decode(errors='replace')),
       inputs=[dict(path=p.relative_to(R).as_posix(),bytes=p.stat().st_size,raw_SHA=H(p.read_bytes())) for p in (source,T/'matrix_to_quaternion.c',golden)],
       artifacts=[dict(path=p.relative_to(R).as_posix(),bytes=p.stat().st_size,raw_SHA=H(p.read_bytes())) for p in (out/'production.o',out/'harness.o',exe)],
       limitation='Host normal/zero arithmetic and initialized GNU scalar/struct prefix views. Native-only consistent table rename defines observer storage, not retail data. Full host sqrt primitive/library is a finite observer, no FCR/RTZ/trap/hardware equivalence. Old numeric C prototype integration is unresolved.')
    (a.report or out/'native.json').write_text(json.dumps(packet,indent=2)+'\n')
    print(result.stdout.decode(),end='')
    assert result.returncode==0 and not result.stderr

if __name__=='__main__':main()
