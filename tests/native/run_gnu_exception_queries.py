"""Native exact GNU query/getter spans with typed initialized provider fixtures."""
from pathlib import Path
import argparse,ctypes,json,os,subprocess,sys
ROOT=Path(__file__).resolve().parents[2]
def run(output,negative=None):
    output=output.resolve();output.relative_to(ROOT);output.mkdir(parents=True,exist_ok=True)
    sources=output/'sources'
    extract=ROOT/'tools/extract_gnu_exception_queries.py'
    cmd=[sys.executable,str(extract),'--output',str(sources)]
    r=subprocess.run(cmd,cwd=ROOT,capture_output=True,text=True);assert r.returncode==0,(r.stdout,r.stderr)
    if negative:
        path=sources/('helper.c' if negative=='stale_slot' else 'selected.cpp')
        raw=path.read_bytes()
        replacements={'stale_slot':(b'return &eh->info;',b'(void)eh; return &query_old_info;'),
            'caught':(b'return p && ! p->caught;',b'return p && p->caught;'),
            'null':(b'return p && ! p->caught;',b'return ! p->caught;'),
            'hook_value':(b'return &((*__get_eh_info ())->value);',b'return (*__get_eh_info ())->value;')}
        old,new=replacements[negative];assert raw.count(old)==1
        (sources/(path.name+'.before_negative')).write_bytes(raw)
        path.write_bytes(b'/* Deliberate native-only negative; no original-source award. */\n'+raw.replace(old,new))
    binary=ROOT/'tools/vendor/ps2dev-20181019/MinGW/bin';front=ROOT/'tools/vendor/vector-insert-native323/bin/gcc.exe'
    env=dict(os.environ);env['PATH']=str(binary)+os.pathsep+env['PATH']
    include=['-Isrc/runtime/gcc','-Isrc/runtime/gcc/include','-I'+str(sources),'-I'+str(ROOT/'tests/native'),'-I'+str(ROOT/'tools/vendor/ps2dev-20181019/MinGW/include')]
    rename=['-D__get_eh_info=george_native_get_eh_info','-D__cp_exception_info=george_native_cp_exception_info']
    flags=['-O2','-fno-builtin','-fno-strict-aliasing',*include,*rename]
    commands=[];objects=[]
    for name,src,cpp in [('selected',sources/'selected.cpp',True),('helper',sources/'helper.c',False),('test',ROOT/'tests/native/gnu_exception_queries.cpp',True)]:
        obj=output/(name+'.o');compiler=front if cpp else binary/'gcc.exe'
        extra=['-B'+str(binary)+'/', '-fno-exceptions','-fno-weak'] if cpp else ['-m32','-Wall','-Wextra','-Werror']
        cmd=[str(compiler),*flags,*extra,'-c',str(src),'-o',str(obj)]
        assert obj.parent==output and src.is_file()
        r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True);commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr));(output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n');assert r.returncode==0,(r.stdout,r.stderr);objects.append(str(obj))
    cmd=[str(binary/'gcc.exe'),'-m32',*objects,'-o',str(output/'checks.exe')];r=subprocess.run(cmd,cwd=ROOT,env=env,capture_output=True,text=True);commands.append(dict(command=cmd,returncode=r.returncode,stdout=r.stdout,stderr=r.stderr));assert r.returncode==0,(r.stdout,r.stderr)
    # Own child processes inherit this host diagnostic mode. A deliberate null
    # fault must return its real status without an interactive crash dialog.
    # This is observer infrastructure, not an original runtime/OS model.
    kernel=ctypes.windll.kernel32;previous=kernel.SetErrorMode(3)
    try:r=subprocess.run([str(output/'checks.exe')],cwd=ROOT,env=env,capture_output=True,text=True,timeout=30)
    finally:kernel.SetErrorMode(previous)
    commands.append(dict(command=[str(output/'checks.exe')],returncode=r.returncode,stdout=r.stdout,stderr=r.stderr,host_fault_dialog_mode=3));(output/'commands.json').write_text(json.dumps(commands,indent=2)+'\n',newline='\n');(output/'execution.json').write_text(json.dumps(commands[-1],indent=2)+'\n',newline='\n');return r
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'build/gnu_exception_queries/native');p.add_argument('--negative',choices=('stale_slot','caught','null','hook_value'));args=p.parse_args();r=run(args.output,args.negative);print(r.stdout,end='');assert r.returncode==0,(r.stdout,r.stderr)
