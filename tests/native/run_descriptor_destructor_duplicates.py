"""Fresh descriptor-only harness over unchanged genuine native helper objects.

The actual original24-PC observations execute independently. Native outcomes
use the single complete unchanged recovered seed and real historical C++
construction/storage closures; no 24 native wrappers or original class claim.
"""
from pathlib import Path
import argparse,hashlib,json,os,subprocess
ROOT=Path(__file__).resolve().parents[2]
H=lambda b:hashlib.sha256(b).hexdigest()
BIN=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin'
NAMES=('bridge','heap','aliases','memmove','bindings','selected','sprintf','format')

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'build/native/descriptor_destructor_duplicates')
    args=parser.parse_args();out=args.output.resolve();out.relative_to(ROOT)
    source=ROOT/'tests/native/descriptor_destructor_duplicates.c'
    inherited=ROOT/'build/native/completion_resolver'
    objects=[inherited/(n+'.o') for n in NAMES]
    # Exact actual native read paths are declared before any subprocess.
    records=[]
    for p in objects:
        raw=p.read_bytes();records.append(dict(path=p.relative_to(ROOT).as_posix(),
            bytes=len(raw),raw_sha256=H(raw),role='Unchanged whole inherited native object, reached real helper paths only.'))
    golden=source.parent/'descriptor_destructor_duplicates_golden.h'
    assert golden.is_file() and source.is_file()
    inputs=[dict(path=p.relative_to(ROOT).as_posix(),raw_sha256=H(p.read_bytes())) for p in (source,golden)]
    out.mkdir(parents=True,exist_ok=True);env=dict(os.environ);env['PATH']=str(BIN)+os.pathsep+env['PATH']
    obj=out/'test.o';exe=out/'checks.exe';commands=[]
    def command(argv,timeout):
        if '-o' in argv:
            assert argv.count('-o')==1
            assert Path(argv[argv.index('-o')+1]).resolve().parent==out
        else:assert Path(argv[0]).resolve().parent==out
        p=subprocess.run(argv,cwd=ROOT,env=env,capture_output=True,text=True,timeout=timeout)
        commands.append(dict(command=argv,returncode=p.returncode,stdout=p.stdout,stderr=p.stderr))
        (out/'commands.json').write_bytes((json.dumps(commands,indent=2)+'\n').encode())
        assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
        return p
    command([str(BIN/'gcc.exe'),'-m32','-O2','-Wall','-Wextra','-Werror','-fno-builtin',
        '-fno-strict-aliasing','-ffunction-sections','-fdata-sections','-Iinclude',
        '-c',str(source),'-o',str(obj)],120)
    command([str(BIN/'gcc.exe'),'-m32','-Wl,--gc-sections',str(obj),
        *map(str,objects),'-o',str(exe)],60)
    result=command([str(exe)],120)
    for row in records+inputs:assert H((ROOT/row['path']).read_bytes())==row['raw_sha256']
    (out/'reuse.json').write_bytes((json.dumps(dict(
        inherited_whole_objects=records,native_read_inputs=inputs,
        fresh_object=dict(path=obj.relative_to(ROOT).as_posix(),bytes=len(obj.read_bytes()),raw_sha256=H(obj.read_bytes())),
        native_PE=dict(path=exe.relative_to(ROOT).as_posix(),bytes=len(exe.read_bytes()),raw_sha256=H(exe.read_bytes())),
        qualification='Only the fresh descriptor fixture harness compiles warning-free. Actual unchanged seed, real historical C++ construction and SGI teardown, whole pointer ledger,16384 arena words/globals/events execute. Controlled core/release callbacks, finite initialized separate descriptor/global storage and GNU prefix/lifetime alias contract remain. Old upstream diagnostics/object counts are inherited; no original class/node/prototype/invocation/data/helper/PC identity or old fixture credit.'),indent=2)+'\n').encode())
    print(result.stdout,end='')

if __name__=='__main__':main()
