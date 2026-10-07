"""One harness object, seven whole production TUs, eight actual dependency queries.

This future native round requires the actual prelink gate; final replays copy and
execute these accepted images rather than rebuilding the harness or querying M.
"""
import argparse,hashlib,json,os,re,shlex,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
H=lambda b:hashlib.sha256(b).hexdigest()
def record(path):
    path=Path(path);data=path.read_bytes()
    return dict(path=path.relative_to(ROOT).as_posix(),bytes=len(data),raw_sha256=H(data))
def make_files(data):
    text=data.decode();assert '.o:' in text
    rhs=text.split('.o:',1)[1].replace('\\\r\n',' ').replace('\\\n',' ')
    for prefix in (ROOT.as_posix().replace(' ','\\ '),str(ROOT).replace(' ','\\ '),str(ROOT),ROOT.as_posix()):
        rhs=re.sub(re.escape(prefix),'__ROOT__',rhs,flags=re.I)
    result=[]
    for token in dict.fromkeys(shlex.split(rhs.replace('\\','/'))):
        assert token.startswith('__ROOT__/'),token
        path=(ROOT/token.removeprefix('__ROOT__/')).resolve();assert path.is_relative_to(ROOT)
        result.append(record(path))
    return result
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--controls',type=Path,required=True);args=parser.parse_args()
    out=args.output.resolve();assert out.is_relative_to(ROOT) and not out.exists();out.mkdir(parents=True)
    controls=json.loads(args.controls.read_bytes());assert len(controls)==6
    env=os.environ.copy();env['PATH']=str(BIN)+os.pathsep+env['PATH']
    flags=[str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror',
           '-fno-builtin','-fno-strict-aliasing','-I',str(ROOT/'include')]
    commands=[];dependencies=[]
    def run(command,stem):
        if '-o' in command:
            output=Path(command[command.index('-o')+1]).resolve();assert output.is_relative_to(out)
        p=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,timeout=60)
        (out/(stem+'.stdout')).write_bytes(p.stdout);(out/(stem+'.stderr')).write_bytes(p.stderr)
        row=dict(command=command,returncode=p.returncode,stdout=record(out/(stem+'.stdout')),stderr=record(out/(stem+'.stderr')))
        commands.append(row);return p,row
    p,version=run([flags[0],'--version'],'version');assert p.returncode==0 and not p.stderr
    sources=[('harness',ROOT/'tests/native/resource_pointer.c'),('positive',ROOT/'src/game/resource_pointer.c')]
    for i,q in enumerate(controls):sources.append(('control_%d'%(i+1),ROOT/q['whole_source']['path']))
    objects={}
    for tag,source in sources:
        obj=out/(tag+'.o');p,row=run([*flags,'-c',str(source),'-o',str(obj)],tag+'_compile')
        assert p.returncode==0 and not p.stdout and not p.stderr,row
        objects[tag]=obj
        p,row=run([*flags,'-M',str(source)],tag+'_M');assert p.returncode==0 and not p.stderr,row
        dependencies.append(dict(source=record(source),command=row,ordered_files=make_files(p.stdout)))
    images=[]
    for i,tag in enumerate(['positive']+['control_%d'%n for n in range(1,7)]):
        exe=out/(tag+'.exe');p,row=run([flags[0],'-m32',str(objects['harness']),str(objects[tag]),'-o',str(exe)],tag+'_link')
        assert p.returncode==0 and not p.stdout and not p.stderr,row
        p,row=run([str(exe)],tag+'_execute')
        if i==0:
            assert p.returncode==0 and not p.stderr,row
            checks=int(re.fullmatch(rb'resource pointer: (\d+) checks\r?\n',p.stdout)[1])
            failed_fixture=None
        else:
            assert p.returncode==1 and not p.stdout and re.match(rb'fixture \d+ ',p.stderr),row
            checks=None;failed_fixture=int(re.match(rb'fixture (\d+) ',p.stderr)[1])
        images.append(dict(tag=tag,whole_executable=record(exe),whole_production_object=record(objects[tag]),
                           reused_harness_object=record(objects['harness']),execution=row,native_checks=checks,failed_fixture=failed_fixture))
    assert len(sources)==8 and len(dependencies)==8 and len(images)==7
    report=dict(commands=commands,compiler=record(BIN/'gcc.exe'),version=version,fixed_flags=flags[1:],
        whole_objects=[record(q) for q in objects.values()],actual_M=dependencies,images=images,
        harness_compiles=1,positive_production_compiles=1,wrong_source_compiles=6,native_M_queries=8,
        limitation='Real initialized GNU32 raw scalar arenas and integer expectations; no original class/capacity/portable narrowing/hardware/runtime ancestry or raw synthetic/native equality claim.')
    (out/'native_report.json').write_bytes((json.dumps(report,indent=2)+'\n').encode())
    print('resource native: one harness, seven images, eight whole actual-M')
if __name__=='__main__':main()
