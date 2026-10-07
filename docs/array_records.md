# Array records: nine connected functions

Nine complete functions cover 868 original bytes (217 instructions). The source
uses the observed 16-byte prefix: element size, capacity, used count and data
pointer. These names describe field behavior; they do not establish a complete
engine class or a capacity guarantee. Seven additional unrooted functions in the
source-free survey remain deferred. The existing count getter at `002AAF88` is
reused and receives no additional recovery credit.

| Address | Bytes | Behavior |
| --- | ---: | --- |
| `002AAF50` | 56 | Signed index/used checks, low32 byte-offset pointer |
| `002AAF98` | 64 | Allocate and initialize the 16-byte prefix |
| `002AAFD8` | 8 | Clear used count |
| `002AAFE0` | 64 | Release data and clear fields |
| `002AB020` | 72 | Release data, clear fields, release header |
| `002AB068` | 164 | Reserve a signed-positive number of additional slots |
| `002AB110` | 244 | Append, growing capacity by ten when necessary |
| `002AB330` | 148 | Erase by copying the last element, then decrement count |
| `002AB3C8` | 48 | Forward nonzero count, stride, data and comparator to qsort |

The C preserves wrapping arithmetic, signed gates and observed reloads. Reserve
publishes capacity before data; append publishes data before capacity. Both read
the data pointer again after copying before freeing it. Append increments the
fresh count after its final copy. Erase retains the original checked-pointer
behavior, including a copy call with a NULL operand when checks fail; no new
production guard is introduced.

The immutable source-free packet is `build/array_records_scope/frozen_inputs.json`
(416 raw inputs). It proves all five executable-section scans, 227 encoded
incoming references, 136 containing caller geometries, the allocated-data and
bounded address-construction surveys, and disjointness from its historical
catalog snapshot. The author read all selected instructions, 17 complete small
callers (411 instructions), larger callers' bounded argument/result windows,
and the supporting allocation/free/copy/count originals. Whole large-caller
semantics are not claimed. Parent review of the initial production C/header
found no mandatory defect; final parent and independent peer review passed,
preserving all664 frozen inputs unchanged.

Three established compiler recipes produce 27 complete natural links, with no
unresolved relocations or extra allocated sections. GCC 3.2.3, GCC 2.9 and GCC 2.9
with the supported SAVE128 attribute are recorded without a flag search. The
complete eight-byte reset matches exactly under each recipe after independent
parent and peer review. The other eight entries remain reconstructed. Both genuine compilers prove
pointer/u32/s32 widths of four, long width of eight, the 16-byte prefix offsets,
and eighteen whole argument-passing callers. Eleven actual compiler `-M`
closures capture 29 distinct files using the exact recorded recipes/includes.

The original-code observer generates 449 authored fixtures, executes 27,409
instructions (maximum 271 per fixture), and visits every selected instruction.
It also executes 42/43 memcpy instructions, 48/50 aligned allocation-wrapper
instructions, 40/42 default allocation-wrapper instructions and all seven free
forwarder instructions. The unvisited memcpy instruction is the taken unaligned
bulk branch's delay at `00393514`. Helper coverage is partial and gives no helper
recovery award. Nine guards reject unsupported/reserved encodings, uninitialized
or unowned memory, invented returns/entries, control transfers in delay slots,
and unsupported hook/copy domains. Without the local original, five synthetic
guards pass and four original-dependent checks skip explicitly.

Six separate native translation units pass 464,677 checks against the original
fixtures, comparing complete 1,024-word arenas, return observations, and event
arguments/order. The production source/header are unchanged from the initial
parent-reviewed draft. A cached-count negative control fails fixture 278 after
the copy changes the count. Initial smaller fixture coverage and its positive
native output are retained in `build/array_records/history_initial`; the final
fixtures add aligned bulk-copy coverage without changing production behavior.

Native reuse includes the exact complete published allocation macro with its
two actual invocations, the exact published free-forward macro invocation, the
whole published count-accessor TU and the unchanged pinned qsort source. Source
span, invocation, generated-TU and full-source hashes are recorded. The core
allocator/free and diagnostic calls are controlled argument/result/mutation
observers; this does not test heap, OS, kernel, locking or concurrency behavior.
The diagnostic string is authored test data, not a recovered original string.

The generic Newlib memcpy source comes unchanged from
[`b595ded606227e93b8c4a447446c1d2ac093827d`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/memcpy.c)
and uses `PREFER_SIZE_OVER_SPEED` solely as native compatibility support. It
receives no retail helper C/assembly identity or match award. Its Cygnus notice
and the applicable license are retained in the pinned source and
`LICENSES/newlib-1.8.1.txt`. This product includes software developed by Cygnus
Support, Inc.

The existing unchanged
[qsort source at the same revision](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib/qsort.c)
retains its Berkeley notice. This product includes software developed by the
University of California, Berkeley and its contributors. Native qsort is tested
only for signed32 keys, four-byte elements and counts 0..12. The original observer
models the sorted result and a controlled comparator mutation, rather than the
full retail sort algorithm or callback timing/count. Host long32 versus target
long64 algorithm equivalence is not claimed. Only that unchanged compatibility
TU uses `-Wno-sign-compare`; the initial legacy int/size_t warning transcript is
preserved. All other native TUs compile without warning suppression.

The model permits only initialized owned arena/stack memory and the one proved
global pointer. Copy domains are nonoverlapping with at most 64 readable bytes;
invalid pointers, huge/negative copies, malformed headers, upper EE lanes,
MMI hardware exceptions and timing are excluded from observations, without
adding guards to production. Proven pointer cells are rebased for the host;
wrapping indexed results are never dereferenced. No float or VU behavior is
involved. Incidental v0 values from void entries receive no return claim.

Existing manager/actor numerical getter declarations return `u32 *` from a
`void *` input. The genuine new header returns `void *` from a const 16-byte
prefix. Both use the same observed four-byte pointer ABI; frozen callers and
their proofs have not been rewritten or represented as typed integrations.
Public SGI vector source was checked and rejected as an original identity:
its pointer layout, growth and erase behavior differ. The new engine bodies
are reconstructed from the original instructions and reuse project types and
support code; no unrelated licensed algorithm is transplanted.
