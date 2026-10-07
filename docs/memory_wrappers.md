The complete `bcopy` entry at `003983A0` (36 bytes) and `index` entry at
`003983C8` (28 bytes) reuse unchanged public GNU EE newlib source from
[revision b595ded606227e93b8c4a447446c1d2ac093827d](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string).
Their raw source hashes are recorded with the actual compiler dependencies in
`config/runtime_functions.json`. The original distribution is newlib 1.8.1;
the separately pinned GCC 3.2.3 compiler headers come from a later distribution.
The complete default clause 9 notice remains in `LICENSES/newlib-1.8.1.txt`.
This software was developed at Cygnus Solutions.

All 16 selected original instructions were reviewed independently by the
parent and peer. `bcopy` exchanges the source and destination arguments and
calls the existing `memmove` at `003935A4`; its void interface does not promise
the incidental callee return register. `index` forwards its string and integer
character to `strchr` at `00393888`, returning the full pointer. Both complete
supporting originals, totaling 167 instructions, were read. They receive no
new source award.

Two genuine incoming calls establish entry use. The full 444-byte `index`
caller was read; the larger `bcopy` caller was reviewed only in a 72-byte
argument and return window. Complete caller byte identities were checked
separately. Both selected entries and both caller geometries exclude their
four-byte zero alignment padding. Executable scans cover three CPU text
sections and the two executable VU overlay records; the incoming calls are in
CPU `.text`.

Four naturally linked whole-function candidates reproduce. Both GCC 3.2.3
bodies match all 64 original bytes with no unresolved relocations. GCC 2.9
also matches the 28-byte `index`; its natural 16-byte tail-call `bcopy` differs
from the original 36-byte body and remains nonmatching. Candidates are never
padded, cropped, patched or masked. Canonical source imports and central
numeric bindings independently reproduce these complete comparisons.

Both compilers emit the seven actual ABI words: 4-byte integer, unsigned
integer, size and pointer widths; 8-byte `long`; 1-byte `char`; signed plain
`char`. Each of the six source/ABI compilations has seven actual dependency
files, checked by raw hashes. Moving the source into its canonical filename
changes only nonallocated ECOFF `.mdebug` data; both actual object hashes are
retained, and all other complete sections remain equal. All 22 author inputs
and 23 raw baselines were independently reproduced before registration.

This evidence concerns complete source, interface and linked-code recovery.
It does not add a native execution, hardware, malformed-memory, full VU/MMI,
or large-caller semantic claim. The repository's normal verifier and hybrid
builder validate the canonical registrations with the privately supplied ELF.
