# Deimos pool maintenance and table iteration

Ten routines in `src/game/deimos_pool.c` cover 1,224 original bytes. Two
ordinary C accessors match their complete linked code and exact sizes, totaling
36 bytes. The other eight remain reviewed reconstructions. The source reuses
the previously reviewed inlined pool allocation sequence from this project's
`deimos_lifecycle.c`; no upstream game implementation was identified or copied.

| Original address | Observed operation | Original bytes | GCC 3.2.3 candidate bytes | Exact match |
| --- | --- | ---: | ---: | --- |
| `002CCF88` | Destroy active pool nodes and return indices | 192 | 196 | No |
| `002CD048` | Retain a tagged pool value | 56 | 48 | No |
| `002CD080` | Release a tagged pool value | 56 | 48 | No |
| `002CD1A0` | Partial payload destruction and invalidation | 164 | 136 | No |
| `002CD410` | Adapt table-copy visitor arguments | 44 | 40 | No |
| `002CD440` | Create table and copy local entries | 232 | 236 | No |
| `002CD8F8` | Read first payload word | 12 | 12 | Yes |
| `002CDAB0` | Advance table iterator into register slot | 212 | 240 | No |
| `002CDFD8` | Read observed type-name pointer table | 24 | 24 | Yes |
| `002CD908` | Resolve script value into hash bits | 132 | 120 | No |

The canonical recipes use the pinned GCC 3.2.3 profile and explicit
original-address linking. GCC 2.9 with the reviewed 128-bit register-save
attribute was also compared for this coherent source batch; it did not produce
an additional full match. Source shapes were not changed to manipulate a
byte count. Every linked function has zero unresolved relocations. The manifest
records hashes, complete byte/size comparisons and instruction-reviewed
boundaries; alignment padding is excluded.

`002CCF88` walks the unreferenced active list. Each signed 16-bit pool index is
captured before its destructor runs. The destructor can change both global
counts: the routine reloads the free-stack position and active count afterwards,
pushes the captured index, and uses the reloaded active count to decide whether
to continue. It clears active count even when the original count was nonpositive.
This routine does not reset references or the payload itself.

The value helpers interpret the tag as signed 16-bit and retain or release only
tags 3 through 5. The partial destructor `002CD1A0` handles raw buffers, objects
and callables while deliberately leaving type-2 table payloads alone. Its
callable branch releases optional data, reloads the node payload, and then frees
that payload. Every branch invalidates type and active slot; references and
other fields retain their original behavior. The type dispatch was checked
directly against all five original jump-table entries at `00448AC0`.

Table copying allocates a pool node and seventeen bucket pointers, establishes
its secondary table and type, then clears the buckets through the node's payload
pointer. It uses the already reviewed local-table traversal and forwards each
key/value through `002CD410` to the existing table mutation routine. Neither a
secondary table traversal nor allocation guards have been added.

The iterator payload consists of table pointer, current chain entry and signed
bucket index at offsets 0, 4 and 8. `002CDAB0` first advances a current chain,
then scans local buckets below 17. It writes every probed head to the iterator
and advances the stored bucket only when it finds an entry. Successful steps
copy the full eight-byte value to the current global argument window. Exhaustion
sets bucket 17 before reloading that window, clears only subtype and tag of the
iterator register, reloads the window again, and clears only subtype and tag of
the output register. Both payloads remain untouched. Repeated exhausted calls
and an output register aliasing the iterator register are supported as observed.

The hash resolver returns the payload bits for tags 2 and 6. Tag 3 hashes the
string pointed to by its pool-node payload using the already reconstructed CRC
wrapper. Other tags obtain their observed type-name pointer, call the existing
error routine, and return zero. All eight jump-table destinations at `00448AE0`
were independently checked. The type-name accessor preserves the original
unchecked signed index; its caller invariants are not expanded into new guards.

Upstream m2c generated ten local drafts. Two lacked their jump-table data and
one could not reconstruct the unaligned eight-byte copy. Direct instruction and
table review resolved those diagnostics into typed C. No error macros,
untyped generated fields or assembly fallback bodies are counted as recovery.

An ignored native 32-bit harness passed 80 semantic checks with zero failures.
It covers destructor-induced shrinking and growth of active count, free-stack
changes, signed tag boundaries, partial destruction with callable-payload
replacement, clone allocation state and visitor arguments, first-word and
type-name accessors, hash branches, bucket skipping, chain progression,
exhaustion without payload clearing, repeated exhaustion and aliased output.
These tests establish reconstructed behavior under the reviewed contracts;
complete target byte comparison remains the separate matching gate.

The root reviewer independently checked the full original instruction bodies,
the two dispatch tables, pool allocation fields and iterator store/reload order,
then reran the 80-check native harness without failures.
