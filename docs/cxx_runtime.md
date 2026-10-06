# GNU C++ runtime reuse

Twelve complete runtime functions / 2,612 original bytes are recovered from
unchanged historical GNU `tinfo.cc`, `tinfo2.cc` and `exception.cc`. Canonical
verification confirms eleven complete matches / 1,388 bytes. The 1,224-byte
`__throw_type_match_rtti` remains reconstructed: its genuine GCC 2.9 candidate
is 1,232 bytes and differs in 883 byte positions.

The matched functions cover type-name equality, three hierarchy casts,
`__dynamic_cast`, active-handler bookkeeping, exception allocation/type
matching, exception-chain removal, uncaught-state bookkeeping and
`exception::what`. Numeric addresses and genuine compiler symbol spellings
remain in `config/runtime_functions.json` and `config/symbols/runtime.json`.

## Source and license

All three sources and four private headers are exact Git blobs from
[SSXModding/ps2-ee-toolchain](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/cp),
revision `b595ded606227e93b8c4a447446c1d2ac093827d`. They reuse the already
published `eh-common.h` and `gansidecl.h`. The nine paths, byte lengths and SHA-256
hashes are recorded in `src/runtime/gcc/cxx_provenance.json`. Copyright notices,
GPL-2.0-or-later terms and each source's exact historical GNU linking exception
are retained; the existing license files contain that exception verbatim.

The original `cp/Make-lang.in` rules establish `-fexceptions` for `exception.cc`.
The other two sources retain their original rule without that flag. Target,
function-section and allocator-alias options are transparent project recipes;
they do not establish the retail compiler or its complete original command.
Per-source `malloc` and `free` macros bind the actual engine allocator entries
`002AF140` and `002AF1E8`. The central standard-library allocator names retain
their existing bindings.

## Evidence and limits

The author and independent peer read all 653 selected original instructions,
the full unchanged sources and private headers, and eight complete supporting
getter/free/pointer-test bodies. Parent review reread all 347 instructions of
the eleven matching functions, the complete source/header and proof material,
and independently reproduced the full packet. Thirty-four actual encoded
incoming transfers, four complete observed 24-byte readonly callback prefixes,
their original pointer publications and the type-matcher's actual EH callback
dispatch establish concrete entry and ABI evidence. Larger enclosing caller
intervals have complete byte identities without a claim of full semantic recovery.

Two genuine GCC 2.9 object probes establish the 29-word target layout, including
4-byte pointers, 8-byte `long`, old GNU virtual records and 48-byte `cp_eh_info`.
The three bitfield examples retain their complete two-word representations.
These are target-object observations; the probe does not execute an exception
or cast on the host.

Four transparent recipes produce 48 comparison records. Eighteen complete
functions link naturally; the remaining records retain actual compiler/linker
failures. GCC 3.2.3 rejects the unchanged historical headers and EH primitives.
The no-builtin recipe also exposes the historical sources' implicit `strcmp`
declarations. No source compatibility rewrite conceals those failures.

Parent reproduction through the checked-in sources, headers and central
bindings confirms the eleven matches. Diagnostic comparisons normalize only
path changes and whitespace reflow from shorter tracked paths; code, data,
size, hashes and comparison results are unchanged. Generated writable,
readonly and COMMON inventories are identity evidence only. No new storage
mapping or data replacement is awarded. Three entries supported only by frame
descriptors remain deferred.

The source assumes valid historical GNU RTTI objects, bounded acyclic base
graphs and valid EH chains/handler records. There is no native RTTI/EH execution,
heap/OS, corrupt-object or full EE hardware claim. The ignored isolated packet
is under `build/reuse/cxx_runtime`; `build/reuse/probe_cxx_runtime.py` and
`build/reuse/cxx_runtime_proof.py` reproduce its compiler and identity evidence
in this research workspace. Public `tools/verify.py --publish-report` rebuilds
and compares the selected checked-in source through the canonical manifests.

Canonical verification, the retail-identical hybrid executable and 364 reviewed
tooling tests passed (one platform-specific symlink skip). C and C++ progress
are now counted separately and together; original assembly remains separate.
