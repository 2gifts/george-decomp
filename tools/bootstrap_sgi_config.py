"""Reproduce the retained historical libio configuration with its exact recipe.

This cross-configuration describes the public headers/compiler, not the game's
original compiler or available runtime APIs. No imported header is modified.
"""
import argparse,hashlib,json,os,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SHA=lambda b:hashlib.sha256(b).hexdigest()

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'build/sgi_configuration')
    args=parser.parse_args();work=args.output.resolve();work.mkdir(parents=True,exist_ok=True)
    profile=json.loads((ROOT/'config/compiler_profiles.json').read_text())['profiles']['gcc29']
    sys.path.insert(0,str(ROOT/'tools'));from verify import prepare_compiler
    setup=prepare_compiler(profile)
    source=ROOT/'src/runtime/sgi/config/gen-params'
    expected=ROOT/'src/runtime/sgi/config/_G_config.h'
    (work/'gen-params').write_bytes(source.read_bytes())
    rel=Path(os.path.relpath(ROOT,work)).as_posix()+'/'
    compiler=rel+profile['compiler'];flags=[]
    for f in profile.get('command_prefix',[]):flags.append('-B'+rel+f[2:] if f.startswith('-B') else f)
    for p in profile.get('include_dirs',[]):flags+=['-I'+rel+p]
    flags+=['-I'+rel+'src/runtime/include','-G0','-mfp64']
    # The primary shell recipe splits CC into words; these relative arguments
    # avoid workspace spaces without quoting changes to the upstream recipe.
    assert all(' ' not in x for x in (compiler,*flags))
    cc=' '.join([compiler,*flags]);nm=rel+'tools/vendor/binutils-v0.10/mips-ps2-decompals-nm.exe'
    script=("set -eu\nexport CC='"+cc+"'\nexport CXX='"+cc+"'\nexport CPP='"+cc+
            " -E'\nexport CONFIG_NM='"+nm+"'\n./gen-params > _G_config.h\n")
    (work/'configure.sh').write_text(script,newline='\n')
    bash=ROOT/'tools/vendor/ps2dev-20181019/MinGW/msys/1.0/bin/bash.exe'
    env=dict(setup['env']);env['PATH']=str(bash.parent)+os.pathsep+env.get('PATH','')
    result=subprocess.run([str(bash),'configure.sh'],cwd=work,env=env,capture_output=True,text=True)
    (work/'configuration.log').write_text(result.stdout+result.stderr)
    record=dict(command=[str(bash),'configure.sh'],returncode=result.returncode,
      generator_sha256=SHA(source.read_bytes()),expected_header_sha256=SHA(expected.read_bytes()),
      compiler_version=setup['version'],compiler_sha256=setup['sha256'],
      generated_sha256=SHA((work/'_G_config.h').read_bytes()) if (work/'_G_config.h').exists() else None,
      qualifications='Cross-header/compiler configuration only; feature checks do not establish original runtime API availability.')
    (work/'configuration.json').write_text(json.dumps(record,indent=2)+'\n')
    assert result.returncode==0,(result.stdout,result.stderr)
    assert (work/'_G_config.h').read_bytes()==expected.read_bytes(),'Generated header differs from retained complete configuration'
    print('Exact historical SGI/libio configuration reproduced')

if __name__=='__main__':main()
