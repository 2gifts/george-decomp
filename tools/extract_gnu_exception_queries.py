"""Exact licensed lexical spans for native observations, not new retail source."""
from pathlib import Path
import argparse,hashlib,json
R=Path(__file__).resolve().parents[1];O=R/'build/gnu_exception_queries'
p=argparse.ArgumentParser();p.add_argument('--output',type=Path,default=O/'native_sources');args=p.parse_args();D=args.output.resolve();D.relative_to(R);D.mkdir(parents=True,exist_ok=True)
H=lambda b:hashlib.sha256(b).hexdigest();cpp=(R/'src/runtime/gcc/cp/exception.cc').read_bytes();c=(R/'src/runtime/gcc/libgcc2.c').read_bytes()
assert H(cpp)=='531ff987405ba67f07d68b2d1f049ab464b6ee3f8d042ed51d9e87a5270ec83e'
assert H(c)=='629b902c36ff32b0c14b372f49995988260def3d0850c8e9211395a5ab5a1d47'
spans=[]
def span(source,path,a,z):
    lo=source.index(a);hi=source.index(z,lo)+len(z);b=source[lo:hi];spans.append(dict(path=path,offset=lo,size=len(b),raw_sha256=H(b),complete_source=b.decode()));return b
cpp_notice=cpp[:cpp.index(b'#pragma implementation')]
c_notice=c[:c.index(b'#include')]
record=span(cpp,'src/runtime/gcc/cp/exception.cc',b'struct cp_eh_info\n',b'\n};')
old=span(cpp,'src/runtime/gcc/cp/exception.cc',b'extern "C" void *\n__cp_exception_info',b'\n}')
predicate=span(cpp,'src/runtime/gcc/cp/exception.cc',b'bool\nstd::uncaught_exception',b'\n}')
macro=span(cpp,'src/runtime/gcc/cp/exception.cc',b'#define CP_EH_INFO',b'__get_eh_info ())')
interface=span(cpp,'src/runtime/gcc/cp/exception.cc',b'extern "C" cp_eh_info **__get_eh_info',b'// actually void **')
helper=span(c,'src/runtime/gcc/libgcc2.c',b'void **\n__get_eh_info ()',b'\n}')
types=cpp_notice+b'#ifndef GEORGE_NATIVE_GNU_QUERY_TYPES\n#define GEORGE_NATIVE_GNU_QUERY_TYPES\n#include "gansidecl.h"\n#include "eh-common.h"\n'+record+b'\n'+interface+b'\nnamespace std { bool uncaught_exception(); }\n#endif\n'
selected=cpp_notice+b'#include "query_types.h"\n'+macro+b'\n'+old+b'\n'+predicate+b'\n'
# Native-only controlled seam replaces runtime initialization/OS selection;
# the entire actual getter body below is unchanged and calls this mutable seam.
support=c_notice+b'#include "gansidecl.h"\n#include "eh-common.h"\nstatic struct eh_context *(*get_eh_context) ();\nstatic void *query_old_info;\nvoid query_native_old_info(void *p) { query_old_info = p; }\nvoid query_native_provider(struct eh_context *(*p) ()) { get_eh_context = p; }\nunsigned query_native_provider_is(struct eh_context *(*p) ()) { return get_eh_context == p; }\n'+helper+b'\n'
files=[]
for name,b in [('query_types.h',types),('selected.cpp',selected),('helper.c',support)]:
    (D/name).write_bytes(b);files.append(dict(path=(D/name).relative_to(R).as_posix(),bytes=len(b),raw_sha256=H(b)))
out=dict(complete_spans=spans,generated=files,cpp_notice_raw_sha256=H(cpp_notice),c_notice_raw_sha256=H(c_notice),qualification='Complete unmodified methods/struct/macro/interface and full getter are extracted with whole GNU notices. Generated scaffolding, declarations and mutable typed provider seam are native-only support. Native macro rename is disclosed in commands; no retail helper/type/data or whole original runtime-model award.')
(D/'extraction.json').write_text(json.dumps(out,indent=2)+'\n',newline='\n');print('GNU query exact native closure',H((D/'extraction.json').read_bytes()))
