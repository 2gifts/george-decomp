"""Optional pinned historical MinGW frontend for the SGI native observer.

The shared2018 MinGW assembler/CRT supplied by bootstrap.py is also required.
This frontend is observer-only and does not change either target compiler.
"""
import argparse,hashlib,subprocess,tarfile,urllib.request
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
URL='https://downloads.sourceforge.net/project/mingw/OldFiles/gcc-3.2.3-20030504-1.tar.gz'
EXPECTED='115762ee663f9c909c29c540ae42a8148a8b6b373a5bb16bbde8394c0e89c663'
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'tools/vendor/vector-insert-native323');args=p.parse_args()
 out=args.output.resolve();out.mkdir(parents=True,exist_ok=True);archive=out/'gcc-3.2.3-20030504-1.tar.gz'
 if not archive.exists():
  with urllib.request.urlopen(urllib.request.Request(URL,headers={'User-Agent':'GeorgeDecomp-native-observer/1'}),timeout=60) as response:data=response.read()
  assert hashlib.sha256(data).hexdigest()==EXPECTED,'Published package SHA256 mismatch'
  archive.write_bytes(data)
 assert hashlib.sha256(archive.read_bytes()).hexdigest()==EXPECTED,'Existing package SHA256 mismatch'
 with tarfile.open(archive) as package:
  members=package.getmembers()
  for member in members:
   relative=Path(member.name)
   if relative.is_absolute() or ':' in member.name or '..' in relative.parts or not (member.isfile() or member.isdir()):raise ValueError('Unsafe native compiler archive member')
   (out/relative).resolve().relative_to(out)
 subprocess.run(['tar','-xf',str(archive),'-C',str(out)],check=True)
 result=subprocess.run([str(out/'bin/gcc.exe'),'--version'],capture_output=True,text=True,check=True)
 assert '3.2.3 (mingw special 20030504-1)' in result.stdout;print(result.stdout.splitlines()[0])
if __name__=='__main__':main()
