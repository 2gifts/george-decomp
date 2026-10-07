"""Pinned historical frontend executes the actual complete SGI template.
 * Headers retain their historical warnings. Native support is not a match
 * award; complete licensed memmove and ordinary comparator are separate TUs.
"""
from pathlib import Path
import hashlib,json,os,subprocess
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'build/native/vector_insert'
F=ROOT/'tools/vendor/vector-insert-native323';B=ROOT/'tools/vendor/ps2dev-20181019/MinGW'
REV='b595ded606227e93b8c4a447446c1d2ac093827d'
def main():
    OUT.mkdir(parents=True,exist_ok=True);commands=[]
    env=os.environ.copy();env['PATH']=str(B/'bin')+os.pathsep+str(F/'bin')+os.pathsep+env['PATH']
    raw=subprocess.run(['git','-C',str(ROOT/'tools/vendor/ps2-ee-toolchain'),'show',REV+':ee/newlib/libc/string/memmove.c'],capture_output=True,check=True).stdout
    source=OUT/'memmove.c';source.write_bytes(raw)
    (OUT/'licensed_support.json').write_text(json.dumps(dict(source='ee/newlib/libc/string/memmove.c',revision=REV,raw_sha256=hashlib.sha256(raw).hexdigest(),license='Newlib1.8.1 default clause9 Cygnus1994/1997',unchanged=True,no_helper_award=True),indent=2)+'\n')
    flags=[str(F/'bin/gcc.exe'),'-B'+str(B/'bin')+'/', '-O2','-fno-builtin','-fno-exceptions','-fno-weak','-fno-strict-aliasing','-ffunction-sections','-fdata-sections',
       '-Isrc/runtime/sgi/include','-Isrc/runtime/sgi/libio','-Isrc/runtime/sgi/gcc','-Isrc/runtime/sgi/config','-Iinclude','-I'+str(B/'include')]
    sources=[('selected',ROOT/'tests/native/vector_insert.cpp',[]),('opaque',ROOT/'tests/native/vector_insert_unreachable.c',[]),('compare',ROOT/'src/game/vector_insert_compare.c',[]),('memmove',source,['-I'+str(B/'msys/1.0/local/ps2dev/ee/ee/include'),'-DPREFER_SIZE_OVER_SPEED','-Dmemmove=insert_real_memmove'])]
    def run(command):
        r=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,text=True,timeout=60)
        commands.append(dict(command=command,exit_code=r.returncode,stdout=r.stdout,stderr=r.stderr));(OUT/'commands.json').write_text(json.dumps(commands,indent=2)+'\n');assert r.returncode==0,(r.stdout,r.stderr)
        return r
    objects=[]
    for name,path,extra in sources:
        obj=OUT/(name+'.o');r=run([*flags,*extra,'-c',str(path),'-o',str(obj)]);objects.append(str(obj))
        if name!='selected':assert not r.stderr,r.stderr
    run([flags[0],'-B'+str(B/'bin')+'/', '-B'+str(B/'lib')+'/',*objects,'-L'+str(B/'lib'),'-Wl,--gc-sections','-o',str(OUT/'checks.exe')])
    r=run([str(OUT/'checks.exe')]);assert not r.stderr;print(r.stdout,end='')
if __name__=='__main__':main()
