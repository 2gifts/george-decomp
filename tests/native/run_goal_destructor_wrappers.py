"""Separate-TU initialized forwarding checks using the approved native protocol.

The helper in the harness is controlled and unawarded. --production may select
a complete changed source and --expect-failure checks its genuine first failure.
The reviewed controls replace exactly the one published helper-call statement
with either `(void)storage; (void)flags;` or the same call with `flags ^ 1U`.
This runner does not generate controls or read private author artifacts. Saved
author images used private carrier filenames; this public spelling has no
fresh compile, dependency or native-execution claim from the publication bridge.
"""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];O=Path(__file__).resolve().parent
BIN=R/'tools/vendor/ps2dev-20181019/MinGW/bin'
H=lambda b:hashlib.sha256(b).hexdigest()

def artifact(path):
    path=Path(path);b=path.read_bytes()
    return dict(path=path.relative_to(R).as_posix(),bytes=len(b),raw_sha256=H(b))

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--production',type=Path,default=R/'src/game/goal_destructor_wrappers.c')
    parser.add_argument('--report',type=Path);parser.add_argument('--expect-failure',action='store_true')
    args=parser.parse_args();out=args.output.resolve();source=args.production.resolve()
    report=args.report.resolve()if args.report else out/'native.json'
    assert out.is_relative_to(R.resolve())and not out.exists()
    assert report.is_relative_to(out)and source.is_relative_to(R.resolve())and source.is_file()
    harness=O/'goal_destructor_wrappers.c';golden=O/'goal_destructor_wrappers_golden.h'
    assert harness.is_file()and golden.is_file()
    inputs=[artifact(p)for p in (source,harness,golden,R/'include/george/goal_destructor_wrappers.h',R/'include/george/goals.h')]
    out.mkdir(parents=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    compiler=BIN/'gcc.exe';base=[str(compiler),'-m32','-O2','-Wall','-Wextra',
        '-fno-strict-aliasing','-I',str(R/'include'),'-I',str(O)]
    commands=[]
    def run(command,label):
        if '-o'in command:assert Path(command[command.index('-o')+1]).resolve().is_relative_to(out)
        q=subprocess.run(command,cwd=R,env=env,capture_output=True,timeout=60)
        (out/(label+'.stdout')).write_bytes(q.stdout);(out/(label+'.stderr')).write_bytes(q.stderr)
        commands.append(dict(label=label,command=command,returncode=q.returncode,
            stdout=q.stdout.decode(errors='replace'),stderr=q.stderr.decode(errors='replace'),
            raw_stdout=artifact(out/(label+'.stdout')),raw_stderr=artifact(out/(label+'.stderr'))))
        (out/'commands.json').write_bytes((json.dumps(commands,indent=2)+'\n').encode())
        return q
    sources=[('production',source),('harness',harness)]
    for label,input_path in sources:
        assert input_path.is_file()
        for kind,tail in [('M',['-M',str(input_path)]),('compile',['-c',str(input_path),'-o',str(out/(label+'.o'))])]:
            q=run(base+tail,label+'_'+kind)
            assert q.returncode==0,(label,kind,q.stderr)
    exe=out/'checks.exe'
    q=run([str(compiler),'-m32',str(out/'production.o'),str(out/'harness.o'),'-o',str(exe)],'link')
    assert q.returncode==0,q.stderr
    q=run([str(exe)],'run')
    if args.expect_failure:
        assert q.returncode==1 and not q.stdout and b'forwarding fixture' in q.stderr,q.stderr
        checks=None
    else:
        assert q.returncode==0 and not q.stderr and b'PASS 1056 forwarding checks;'in q.stdout,q.stderr
        checks=1056
    packet=dict(compiler=artifact(compiler),commands=commands,actual_input_sources=inputs,
        separate_translation_units=2,ordered_M_queries=2,native_checks=checks,actual_expected_failure=args.expect_failure,
        artifacts=[artifact(out/'production.o'),artifact(out/'harness.o'),artifact(exe)],
        qualification='Initialized128B typed arena/32unique parameters. Only genuine field0C pointer cells translated; exact-typed helper is a controlled observer, not execution/recovery of the published destructor or heap. Native void result ignored; no original class/lifetime/null/upper128/invocation identity.')
    report.parent.mkdir(parents=True,exist_ok=True);report.write_bytes((json.dumps(packet,indent=2)+'\n').encode())
    print((q.stderr if args.expect_failure else q.stdout).decode(),end='')
    return 0

if __name__=='__main__':raise SystemExit(main())
