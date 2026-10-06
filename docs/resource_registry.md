# Resource registry and pooled maps

Thirteen centrally registered complete functions recover, 1,728 bytes /
432 original instructions. All 39 genuine whole-function links under the three
established compiler recipes resolve without remaining relocations. None is an
exact match. Author, independent peer and parent source review passed.

| Entry | Bytes | Observed operation |
| --- | ---: | --- |
| `00217F78` | 52 | Query the global registry with kind 14 |
| `00217FB0` | 52 | Query with kind 0 |
| `00217FE8` | 52 | Query with kind 6 |
| `00218020` | 52 | Query with kind 3 |
| `002193A0` | 120 | Walk enabled provider links, excluding the sentinel |
| `0021A0B8` | 212 | Replace or insert a pooled map entry |
| `0021A190` | 168 | Remove the first map entry matching a key |
| `0021A238` | 176 | Remove and return that entry's value |
| `00229020` | 152 | Resolve a provider address and rounded size |
| `00229FF0` | 204 | Resolve an indexed provider, with optional outputs |
| `002A77F0` | 52 | Select a kind-indexed data map and query it |
| `002AF9A0` | 252 | Search indexed or linear data records |
| `002A8250` | 184 | Search a self-relative bucket index |

The four wrappers share one ordinary C definition macro. A complete relative
word comparison proves that their entire original bodies differ only at the
kind-setting instruction, relative offset `+24`. The deletion pair shares an
ordinary C template parameterized by its output action; each retains a complete
natural function definition, compiler symbol and independently compared bytes.
The implementations are custom algorithms. No public library identity or source
license substitution is inferred from these short patterns.

The existing `GeorgeGenericMap` and its 12-byte node are reused. The already
published `00219FF0` lookup is excluded from the new count; its same ordinary C
macro supplies the native insertion/deletion sequence's lookup. The published
`002A8228` scalar getter is also excluded and retains its actual entry binding.
Native tests link its production source; the scoped trace executes its original
two instructions. The whole resource-manager source remains unchanged.

## Pool and alias ordering

Complete constructors `00219D58` and `00219EB8` independently establish a
16-byte raw pointer stack: capacity at zero, index at four, pointer-slot storage
at eight, and 12-byte node storage at twelve. It differs from `GeorgeSlotPool`.
The pooled map extends the existing 16-byte map prefix with that pool pointer
at `+10`. Constructors and allocator calls are supporting evidence, not newly
recovered entries or a model of allocator failure recovery.

Insertion first captures key, value and bucket. With flag mask `1` clear, the
first equal-key node receives only a replacement value. Otherwise it captures
the old index and slots, obtains the selected node, increments index and clears
the old slot. A null node still consumes that slot and index step. Successful
insertion reads a fresh bucket head, stores key / next / value, reloads the
bucket base for publication, and reloads count for a wrapped increment.

Both removals unlink the first matching node. The output variant then writes
the value **before** loading the current pool pointer. Its output may change
that pointer, pool index, slot base or map count. Both routines capture the
current index and slots before decrementing index and returning the retained
node; the final count load is fresh. Missing entries return zero and leave the
value output untouched. Insertion's incidental original `v0` is not assigned
a return meaning: its declaration is `void`, and the harness excludes it.

No pool-capacity, zero-divisor, kind or allocation guard is invented. Original
callers must provide accessible buckets, slots and records. Low32 underflow and
wrap remain observable. The native sequence explicitly tests that the mutators
do not consult the pool's capacity word.

## Provider and index lookup

Provider traversal captures the next link before querying the current record.
It queries only when the disabled word equals zero, stops at the first nonnull
result, and never queries the terminal link with null successor. The root slot
`003F9468` is writable `.data`, initially zero. Four actual LUI/load pairs and
its whole four-byte hash are recorded. They establish the global slot's access
and geometry without establishing an original class name or readonly table.

The index retains its observed 32-byte prefix. Bucket and row locations are
self-relative integer offsets; records use a captured runtime stride. Lookup
selects the unsigned key modulo the bucket count, then scans to the next
bucket's row index or the final row count. Multiplication/address arithmetic
wrap to low32 and endpoint comparison is unsigned. A zero bucket count returns
null; no zero-stride or malformed-index safeguard is introduced.

Flag mask `4` selects indexed data lookup. On success, its optional size store
precedes fresh origin and row-offset loads. Linear lookup uses the complete
signed count and the first matching 12-byte row. It captures the returned
origin-plus-offset address before its optional size store. These paths differ
when size aliases origin or the found row. Private dead scan-index scratch in
the original stack frame is omitted; it has no modeled caller output and its
instruction difference receives no matching credit.

The ordinary provider writes zero to its mandatory size output before loading
the data map. Failure leaves the address output untouched. Success reloads
origin, tag and base, captures both packed address and wrapped 128-byte-rounded
size, then stores address followed by size. The indexed provider first uses
the real getter and freshly reloads its index slot. Its optional address store
precedes a fresh row-size load and 16-byte rounding; both outputs may be null.
The fixture set includes overlapping outputs and aliases into these prefixes.

## Complete evidence and bounded validation

The isolated packet records full original SHA256 hashes, matching assembly/raw
bytes, every return and delay, excluded zero alignment and preceding/following
boundaries. All 86 actual incoming J/JALs have complete containing-body geometry
and hashes. The aligned allocated non-executable pointer scan supplies no new
computed roots. The selected bodies contain 10 actual nested JALs, 51 decoded
scalar/stack stores and 17 actual returns, with all local branches audited.
Whole containing callers are not claimed to be manually reconstructed, and no
full transitive resource graph is inferred from their entry references.

The author read every selected instruction and the full C/header/template,
decoder and native harness, plus ten complete supporting layout/ABI bodies.
Before ownership approval the parent independently audited all thirteen full
raw/assembly extents, hashes, terminal delays and all 86 containing-call proofs.
Independent semantic review of all selected original/source/native/proof inputs
passed. Both reviewers reproduced all 39 complete links, five entire packet
dictionaries, the fixtures and golden header, nine guards and the warning-free
native harness. The parent additionally read 150 supporting instructions; the
other supporting and incoming caller reviews are attributed as recorded.

The scoped trace generates **414** authored fixtures executing **24,495**
original instructions, at most 136 per fixture. Every nested call runs an
owned original function or the actual existing getter; there are no external
library or callback substitutes. The native harness runs all production new
bodies, the production getter and the published lookup template, passing
**424,710** warning-free checks. This includes protected whole-window output
comparison and an independent insertion/replacement/duplicate/removal sequence.
Nine decoder guards cover memory/code bounds, uninitialized memory, unsupported
operand forms, division, low32 products/comparisons, delay execution/annulment,
actual JR31 returns including interior selected stops, authentic nested helper
execution, unknown calls and bounded loops.

Synthetic input SHA256 is
`f565d3e23fa807b2d36f77ae07cee4a9ba932f2189e43b7ed6f8cfc2221981a6`.
The header's canonical LF SHA256 is
`d1ae93b4f0fbb67b1ca425e74bea5d304c2c75b3df472590b3cbd79d5df5d74b`;
the generated Windows CRLF file SHA256 is
`0489a0fc8e3dccd5ccebe3d32d8f9f9450ccb4e19c3d0678249572d456b24f4c`.
No original instruction, index, string or asset arrays are exported.

Regenerate with the project Python environment:

```text
python tools/trace_resource_registry.py --output build/resource_registry_trace.json --golden-header tests/native/resource_registry_golden.h
python tests/test_trace_resource_registry.py
gcc -m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude tests/native/resource_registry.c src/game/resource_registry.c src/game/accessors.c -o build/resource_registry_native.exe
```

The native command uses the existing MinGW runtime, with its `bin` directory
prepended to inherited PATH. Its entire authored storage must fit below
`80000000`. Pointer canonicalization is confined to that authored window.
These are valid-memory low32 integer/alias models, not full allocator safety,
resource lifetime, original class identity, EE timing or hardware execution
proof. Whole compiled bytes remain the sole function matching gate.

Canonical checkpoint verification and the hybrid build passed, together with
307 tooling tests (one platform-specific symlink skip). The hybrid executable
retains retail SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`;
remaining original assembly contributes no additional C matching credit.
