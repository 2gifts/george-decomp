Recovered packet construction and encoding covers four C entries plus one C++
instantiation: 996 original bytes. The entry names remain numeric because the
call graph does not establish a network protocol or original class name.

`002BCB50` allocates a 12-byte range object, allocates `length+8` bytes, fills that
range, writes two low-byte tags and a full length, then publishes the range
through its captured holder. `002BCCA0` captures both string lengths before
allocation. Its nested `002BD000` encoder independently calls the length helper
again after allocator callbacks and fill. Shrinking strings can leave a zeroed
tail; growing strings can make the nested capacity test fail. The constructor
still publishes the range in either case. `002BCFA8` and `002BD000` retain their
header-before-copy order, wrapping unsigned arithmetic, and observed pointer
captures. The later object check does not protect earlier original object
stores; no new null, overflow, or exhaustion guards were added.

`002BD1F0` uses the complete unchanged HP/SGI generic `fill_n` method from
`src/runtime/sgi/include/stl_algobase.h`, explicitly instantiated for byte
pointers, an unsigned 32-bit count, and a const byte reference. This is an
explicit instantiation, not a specialization or a hand-written loop. The full
primary headers and notices already published by the vector work are reused.
The source-local C allocation expression is an explicitly modified adaptation
of the SGI default allocator; its original notice and changes are retained.
See [SGI-STL.txt](../LICENSES/SGI-STL.txt) and the pinned provenance record at
`src/runtime/sgi/provenance.json`, revision
`b595ded606227e93b8c4a447446c1d2ac093827d`. Generic native memcpy is the complete
unchanged Newlib 1.8.1 Git blob from that same distribution, covered by its
Cygnus 1994/1997 default clause 9. It is native support, not a new memcpy award.

The original 249 instructions, 154 instructions in three complete outer
callers, and 364 instructions in primary supporting entries were manually
read by the author during the approved source-free survey. Seven real encoded
JALs were checked across all five executable sections, with complete containing
caller geometry and hashes. The outer ancestry of `002BB8D0` and `002BBA70`
remains unresolved; this does not establish that they are unused. Historical
source-free FINAL91 and the separate timer FINAL100 remain immutable. The
constructor-length prose qualification from the independent scope reviewer
is retained separately: a constructor does not directly recompute its lengths,
but its nested encoder does. Scope approval is not a final source award.

Eight entire natural C functions link under GCC 2.9 and 3.2.3. The natural
GCC 3.2.3 C++ object emits a weak, executable linkonce section that the strict
linker rejects. The parent approved a single genuine `-fno-weak` recipe, keeping
the source and all other options unchanged. It emits the complete global
40-byte fill function, resolves naturally, and differs in six bytes. GCC 2.9
fails in the unchanged historical stream headers; its complete diagnostics
remain evidence. The old mangled spelling is proven only by a genuine caller
and declaration ABI, not by a successfully compiled old helper definition.
All nine successfully linked selected candidates are nonmatching.

Both target compilers produced the actual seven-word range-layout arrays and
12 complete selected-signature caller functions across four objects. These
all link through canonical numeric or actual mangled aliases. The largest
selected call uses six integer registers, and fill uses three; no eight-lane
signature is asserted. All 21 entire selected/caller ET_EXEC functions have
no remaining relocations or allocated data. Seventeen genuine `-M` closures
pin 62 distinct actual source/header files, including preprocessing of the
honest frontend-failed C++ recipe. A preprocessor closure does not erase that
frontend failure.

The strict original observer reuses `RegistryTrace` unchanged, adds no opcode,
and executes the selected entries plus full original length, memcpy, heap
wrappers and SGI OOM/refill/chunk bodies. Heap core, allocation diagnostics and
an OOM handler are controlled effects. Its 1,273 initialized fixtures execute
1,214,059 original instructions, at most 12,640 per invocation. They exercise
247 of the 249 selected instructions. The two unexecuted instructions choose
null when buffer size wraps to zero; subsequent original encoder stores are
outside the tested valid object domain. This limitation earns no match credit
and introduces no production guard.

The native comparison runs the selected C, the genuine fill and full SGI
allocator templates in separate translation units. It also runs exact complete
published heap-wrapper macros and the complete published length function.
Their byte-extraction offsets, entire source hashes, span hashes, generated
translation-unit hashes and actual dependency closures are retained. Native
symbol aliases bind the observed numeric interfaces to actual complete native
C++ methods and typed static data. Both sides have four-byte pointers and
counts; the actual native ABI reports size_t and long as four bytes and the
range as 12 bytes. A typed original pointer-store ledger translates only
known pointer fields, including owned one-past pointers. Arbitrary integer
words are not translated as pointers.

All 5,252,952 native checks pass, including full-word memory/global checks,
callback string growth/shrinkage, refill, OOM retry, header/input aliases and
holder aliases. Authored C and native support compile without warnings; the
unchanged historical C++ headers retain their genuine warnings. Two deliberately
wrong C variants fail the real native comparison: recomputing constructor
allocation lengths after callbacks, and publishing the string header after
copies. An initial negative-test abort dialog timed out; it is preserved as
history, not a semantic result. The authored assertion handler now exits with
status 1, and both final wrong algorithms fail deterministically.

Nine focused decoder tests pass locally. With only the exact private original
path absent, six pass and three skip, so clean public CI does not require the
game file. The tests cover memory/image bounds, reserved encodings, the zero
register, the inherited instruction budget, real returns, taken and untaken
delay-slot rejection, reference-read counts, callback lengths and capacity.

The fill loop reads its reference on every positive iteration in the original
and compiled method. In ordinary initialized nonvolatile storage, an overlapping
fill store writes that same value, so these alias tests do not establish a
hardware or concurrent-read effect. String and copy tests require initialized,
readable padded objects, terminated strings when strlen runs, original header
alignment, and nonoverlapping memcpy ranges. Native pointers must remain in the
proved low32 owned address domain. Null faults, impossible huge counts,
uninitialized storage, scheduler/SDK behavior and original C++ object identity
remain outside this observation proof.

The complete author packet is under ignored `build/packet_construction`.
Reproduce using the pinned tools and the privately supplied exact game ELF:

```powershell
.venv/Scripts/python.exe build/packet_construction/compile.py
.venv/Scripts/python.exe build/packet_construction/no_weak.py
.venv/Scripts/python.exe build/packet_construction/abi_prelink.py
.venv/Scripts/python.exe build/packet_construction/link.py
.venv/Scripts/python.exe tools/trace_packet_construction.py --output build/packet_construction/trace.json --golden-header tests/native/packet_construction_golden.h
.venv/Scripts/python.exe tests/native/run_packet_construction.py
.venv/Scripts/python.exe build/packet_construction/dependencies.py
.venv/Scripts/python.exe build/packet_construction/negative.py
.venv/Scripts/python.exe build/packet_construction/guards.py
.venv/Scripts/python.exe build/packet_construction/proof.py
.venv/Scripts/python.exe build/packet_construction/metadata.py
```

Author, distinct peer and parent source/packet reviews passed, followed by canonical registration of five reconstructed routines. Fifteen entire qualified dictionaries, all21 target executables,8 target objects and24 native objects reproduced. Three full native PE observations have only actual parsed timestamp/checksum changes, with whole ImageHlp-valid checksums; full PE RAW equality is not asserted. All379 frozen inputs were unchanged before this archived publication annotation. The current freeze has378 RAW-equal files plus this archived document. Sources/headers and authored proof producers remain unchanged; no new exact/helper/data award is made.

Some genuine SGI chunk fixtures start with both free-storage pointers null. Their subtraction and native typed static-data aliases follow the measured historical GNU32 ABI, not a universal ISO C++ pointer/type/lifetime guarantee. The actual native compiler and complete genuine methods remain recorded. Original class/protocol identity and outer reachability remain unresolved.

The initial private-link attempt retained a fixed author output directory and rewrote two metadata files. It is preserved, along with20 extra whole artifacts and the failed restoration attempt. Both complete metadata files were restored only when their known frozen SHA256 values matched exactly, without rebaselining. The final corrected replay preserved every frozen byte. Its initial native-record redirection also left three reads canonical; distinct peer and attributed root clones counted and redirected the actual path prefix in dependencies1/proof2, checked actual private -M closures and full native artifacts, and reproduced the complete packet. These qualifications do not modify the frozen producers. A separate parent provenance package pins complete owned source carriers and chosen-recipe dependencies.
