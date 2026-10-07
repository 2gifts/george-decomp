# Unchanged newlib random runtime

`srand` at `0x397168` (16 bytes) and `rand` at `0x397178` (48 bytes) both
match their complete original functions under GCC 3.2.3. Together they add
64 exact C bytes. GCC 2.9 emits the same complete sizes with two and nine
bytes different, respectively. Alignment padding and data receive no credit.

[Production rand.c](../src/runtime/stdlib/rand.c) is the unchanged 2,294-byte
public GNU EE newlib Git blob at revision
`b595ded606227e93b8c4a447446c1d2ac093827d`, path
`ee/newlib/libc/stdlib/rand.c`, SHA256
`d1cb734d07e5e1f168b1e18eebf2d408370ac13c79e0d1058d4b7593e25a4561`.
The existing unchanged compatibility `sys/reent.h` is reused. All 14 actual
dependencies per compiler/observer are pinned separately; the 2018 compiler's
newer headers do not establish the version of this source distribution.

The exact pinned `ee/COPYING.NEWLIB` equals the retained
[distribution notices](../LICENSES/newlib-1.8.1.txt), including default clause 9,
Copyright (c) 1994, 1997 Cygnus Solutions. The source has no overriding notice.
The notice and its acknowledgement are retained: **This software was developed
at Cygnus Solutions.** The initial private source-free proposal incorrectly
cited the newer default clause 10; that historical record is retained with an
explicit correction, without changing source, function geometry or results.

All 16 original instructions were read and reproduced. `srand` stores the full
unsigned 32-bit seed into the current reentrancy structure at offset `0x58`.
`rand` multiplies its state by 1103515245, adds 12345 with unsigned 32-bit wrap,
stores the entire new state, and returns its lower 31 bits. Both original global
load sequences prove `_impure_ptr` at `0x405694`; both terminal return delays
contain the state store. Neither body calls another function or contains local
allocated data.

Both genuine seven-word target ABI observers prove int/unsigned/state width 4,
long width 8, pointer width 4, state offset `0x58` and `RAND_MAX=0x7FFFFFFF`.
All three executable sections were independently scanned for 189 actual encoded
calls, and all 67 containing raw/assembly intervals checked. The entire 40-byte
seed caller forwards the low 32 bits of CP0 Count; no entropy or timing claim is
made. Large outside callers have full geometry evidence, without a claim of
complete semantic interpretation.

Author, independent peer and parent reproduced four whole-function natural
links, including actual complete STT_FUNC extents and zero unresolved
relocations. Four further canonical-only links retain every comparison result.
The global verifier confirms both exact matches. Source bytes, compiler output,
relocations and linker policy are unchanged; no masking, cropping or padding
creates these matches.

No native game/startup execution, multithreading, random quality/security,
CP0 timing or hardware upper-register behavior is claimed. Reproduction uses
the documented project verifier with your supplied original executable:

```powershell
.venv/Scripts/python.exe tools/verify.py --publish-report
```
