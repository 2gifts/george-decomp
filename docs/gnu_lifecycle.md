# GNU constructor, destructor and exit runtime

Four complete entries reuse the existing unchanged historical GNU `libgcc2.c`:
`__do_global_dtors` at `0x36FB60` (84 bytes), `__do_global_ctors` at
`0x36FBB8` (176), `__main` at `0x36FC68` (32), and `exit` at
`0x3712E8` (48). They recover 340 original bytes /85 instructions.
The entire GCC 2.9 `exit` function matches all 48 bytes. The other three remain
reconstructed; no writable data, callbacks or helper bytes receive credit.

The source remains the exact public GNU EE Git blob at revision
`b595ded606227e93b8c4a447446c1d2ac093827d`, already documented in
[GNU source provenance](../src/runtime/gcc/provenance.json).
Only the missing, unchanged `ee/gcc/gbl-ctors.h` is added: 3,363 bytes,
SHA256 `5c513245ac6d054058f57d51536589d6185398a097600cb470111e3be8c86e16`.
All prior source/header records remain unchanged. The full original GPL notices
and [historical libgcc2 linking exception](../LICENSES/GCC-libgcc2-exception.txt)
are retained. Neither candidate compiler identifies the game's original compiler.

The destructor cursor advances before each callback, then reloads its current
value after that callback. The constructor array supports the source's full
unsigned-long sentinel/count scan and reverse invocation order. The one-time
`__main` guard is stored before entering constructors. `exit` preserves the
signed 32-bit status across destruction and cleanup, then ends with the genuine
original tail jump to `_exit` at `0x100218`. The underlying OS handler and
arbitrary constructor/destructor callbacks remain outside this recovery.

All 85 selected original instructions, complete boundaries including the exit
tail delay, 132 encoded references and 130 containing-caller raw/assembly
intervals were checked. Large callers have geometry evidence rather than a
claim of complete semantic interpretation. The complete 700-byte constructor
array and 476-byte destructor array prove counts, aligned text callbacks and
null termination. The current destructor pointer is a real four-byte writable
initializer at `0x4021AC`, pointing to `0x4062C4`.

Both genuine compiler recipes reproduce that entire initializer in a separate
GNU linker identity check, without changing the objects. The function matcher
still rejects `__do_global_dtors`' local `.data`; no writable mapping or code
patch is added. The one-time guard uses a complete four-byte zero NOBITS object
at the independently evidenced original `0x48F26C`, under the existing strict
BSS proof rules.

Eight whole-function comparison rows were reproduced privately by the author,
independent peer and parent: six actual complete links and two genuine `.data`
blockers. Canonical-only reproduction retains every comparison result, code
hash, size, input relocation and mapping. Both actual target ABI observers
agree on nine layout words, including four-byte function pointers and eight-byte
longs. Each of the four compiler/selector dependency sets contains 18 actual
files; the new canonical header replaces only its byte-identical private path.

The final canonical verifier confirms only the complete 48-byte `exit` match.
These proofs do not claim native startup/OS execution, malformed array behavior,
original callback effects, EE upper-register/FCR behavior or hardware timing.
Reproduction uses the project commands with your supplied retail executable:

```powershell
.venv/Scripts/python.exe tools/verify.py --publish-report
.venv/Scripts/python.exe tools/build.py
```
