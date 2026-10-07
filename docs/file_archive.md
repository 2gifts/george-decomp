# File-slot close and archive lookup

Two complete original functions are reconstructed: `002B0998` (92 bytes) and
`002B1BA8` (92 bytes). Their two four-byte alignment pads are excluded. The first
has sixteen encoded incoming callers; the second is called by the observed file
opener at `002B07B4`. No adjacent deferred body or helper is counted.

Close tests the global mode before the low-level close call, then freshly reloads
and decrements the wrapping 32-bit open count. It clears slot words in the
original order `+18,+0,+4,+8,+C,+14`; `+10` retains any value left by callbacks.
The observed caller paths discard or overwrite incidental V0. The proposed void
interface expresses those effects without identifying the original return type.

Lookup resolves the path into an uninitialized local scratch view, finds the
first `cdrom0:` substring, skips its seven bytes when present, computes the real
project CRC, then freshly loads the context's map pointer at `+50`. The result is
the real full-key map lookup. The existing opaque `void *` interface is retained.
Scratch size 64 models the observed position before saved RA and establishes no
safe capacity or valid-input check in production.

Both C/header initial hashes remain unchanged. There is no public source identity
for either engine body. Existing exact complete project resolver, CRC, length,
map, heap-wrapper and close function spans/macros run in separate native TUs.
The old MinGW linker rejects unresolved references from unexecuted functions in
whole support TUs despite garbage collection; the failed command, diagnostics
and object hashes are preserved in the private packet. Complete lexical spans,
their source offsets/hashes and generated-TU hashes prove the unchanged reuse;
no dummy support data or implementation is supplied. The close helper's file10
source was parent-reviewed and immutable at FINAL453; its separate final peer
review is pending at this author freeze.

The strict original observer inherits the published registry decoder and executes
every one of the 46 selected instructions plus genuine complete helper scopes.
There are 500 authored initialized fixtures, 223,192 executed original
instructions and 1,222,668 warning-free native checks. Fixtures cover mode bypass,
zero/high-bit encodings, wrapping counts, both cache calls and controlled SDK
mutations, marker positions and empty suffix, full-key collisions, missing keys,
lazy resolver allocation failure, and context-map replacement during allocation.
The native harness compares the entire 2,048-word arena, globals, text buffers,
returns and call events. A deliberately cached pre-close count fails fixture 17.
Nine decoder/domain tests pass; without the local original, five run and four
skip. Actual fetched bounds, initialized memory, actual terminal JR31 and both
taken/untaken branch delay guards are required.

Six complete natural selected links and four authentic whole ABI callers use
canonical bindings only. Both target layouts are
`[4,4,4,28,16,24,80,16,12,12]`; pointer arguments occupy the observed GPR4/GPR5
lanes. Each selected function is compared in full at its original address and
has zero remaining relocations or extra allocated sections. GCC 3.2.3 emits
92/84 bytes with 27/54 differing bytes. GCC 2.9 emits 96/84 bytes with 52/55
differences; its optional supported save-width recipe emits 52/54 differences.
Both entries remain reconstructed, with no exact code or data award. Twenty-one
actual compiler `-M` closures pin all 43 distinct dependency files.

Native generic strings are unchanged public Newlib source at
`b595ded606227e93b8c4a447446c1d2ac093827d`, compiled with the bytewise compatibility
recipe. They support initialized ASCII, valid terminated/nonoverlapping strings;
the strict original observer separately executes optimized retail helpers and
requires initialized aligned overread lanes. This does not identify retail
`strcat`/`strcpy`/`strcmp`/`memcpy` source or recover another helper. Published
ctype, lowercase and substring source is reused unchanged. The historical
Cygnus clause 9 notice is preserved in those source files and the pinned license:
**This project uses software developed at Cygnus Solutions.**

Controlled heap/cache/SDK/diagnostic hooks describe caller observations only.
No kernel, OS, concurrency, archive format, original class/capacity, invalid-input
behavior, prior-stack identity, EE upper-lane precision, exception or timing
equivalence is claimed. Production adds no owner, count, null, capacity or string
validation guards. Author full original/source/support review is complete;
Final independent parent and peer reviews passed, followed by canonical registration. Each reproduced nineteen complete evidence dictionaries, five RAW target objects and ten whole selected/ABI ELFs, and independently relinked all ten through current canonical default bindings. All348 frozen and105 historical RAW inputs were preserved before this sole archived documentation status qualification. The parent independently checked9,640 finite typed arena pointer translations and all global/result pointer values. The reused file-operation helper was pending at the historical snapshot and has now passed parent/peer review and been published at checkpoint52. Both archive functions remain reconstructed, with zero exact matches.

The private final packet is `build/file_archive/frozen_inputs.json`. All proof,
dependency, comparison, negative, native command and source-free historical
inputs are immutable. Private replays must redirect every output, preserve the
read-only canonical ABI input filename for ECOFF identity, compare complete
dictionaries/artifacts, and only qualify measured unittest duration text.
