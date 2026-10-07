"""One whole positive or explicitly selected genuine wrong-source native build."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=ROOT/'build/native/word_cursor');p.add_argument('--production',type=Path,default=ROOT/'src/game/word_cursor.c');p.add_argument('--report',type=Path);p.add_argument('--expect-failure',action='store_true');a=p.parse_args()
    out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin','-fno-strict-aliasing','-I',str(ROOT/'include')]
    commands=[];objects=[];closures=[]
    def run(command):
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr));return r
    for name,source in [('harness',ROOT/'tests/native/word_cursor.c'),('production',a.production.resolve())]:
        obj=out/(name+'.o');r=run(flags+['-c',str(source),'-o',str(obj)])
        assert r.returncode==0 and not r.stdout and not r.stderr,commands[-1]
        objects.append(str(obj));r=run(flags+['-M',str(source)]);assert r.returncode==0 and not r.stderr,commands[-1]
        closures.append(commands[-1])
    exe=out/'checks.exe';r=run([flags[0],'-m32',*objects,'-o',str(exe)]);assert r.returncode==0 and not r.stdout and not r.stderr,commands[-1]
    r=run([str(exe)])
    if a.expect_failure:assert r.returncode==1 and 'fixture ' in r.stderr and not r.stdout,commands[-1];checks=None
    else:assert r.returncode==0 and not r.stderr,commands[-1];checks=int(re.fullmatch(r'word cursor: (\d+) checks\n',r.stdout)[1])
    report=dict(commands=commands,native_checks=checks,actual_failure=a.expect_failure,warning_free=True,actual_M=closures,separate_translation_units=2,
        source_sha256=hashlib.sha256(a.production.read_bytes()).hexdigest(),limit='GNU32 pointer/sign narrowing on complete initialized typed scalar arenas; no original class/portable out-of-range conversion/upper128/capacity claim.')
    if a.report:a.report.parent.mkdir(parents=True,exist_ok=True);a.report.write_text(json.dumps(report,indent=2)+'\n')
    print(r.stderr if a.expect_failure else r.stdout,end='')
if __name__=='__main__':main()
