# Property ownership and initialization

This connected batch reconstructs six complete functions, 732 original bytes
and 183 instructions, in `src/game/property_lifecycle.c`:

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `002B9440` | 240 | Initialize the scalar node prefix and call list/matrix initialization |
| `002B9660` | 192 | Release records and name, recursively destroy children, then detach and destroy self |
| `002B9720` | 40 | Detach the node and clear field +08 |
| `002B9AF8` | 52 | Destroy the previous record list, then install the captured replacement |
| `002B9B30` | 56 | Destroy a non-NULL record list, then clear its owner field |
| `002BA1D0` | 152 | Detach and release record payloads/nodes, then release the list container |

The new observed prefix through +B4 preserves all established hierarchy offsets,
the reviewed embedded intrusive-list layout, existing vector/matrix types and
the 148-byte text-record producer's payload field. It exposes optional destructor
+24 and owned record-list pointer +B4 without changing the frozen hierarchy
header. Offset/size assertions apply to target and native 32-bit builds. The
prefix is 184 bytes; allocating caller `002B9530` reserves 192 bytes and the
derived caller `00278EB0` reserves more. Neither trailing storage nor a complete
original class or source typedef is reconstructed here. Fields keep neutral
offset names when their engine meaning remains unknown.

The constructor captures three scalar float arguments before the list call.
It clears previous/next links before that call, then performs the original
sparse scalar stores, position X/Z/Y stores, quaternion zero/one stores and
flags mask before the matrix call. The remaining vector/scalar/name/record stores
follow the matrix call in their original order. Field +6C and trailing storage
remain untouched. Mutations made by either preserved call can survive in fields
that the constructor does not subsequently overwrite; the captured input floats
survive the calls. Actual callers pass node in a0 and floats in f12/f13/f14, and
do not consume a constructor return value.

The matrix initializer `002A1C30` remains an actual numeric engine call. Its
complete 48-byte body uses VU macro instructions with VF0-based constant lanes.
The native output contract models its fixed caller-visible identity stores,
in observed final-row/first/second/third-row order. It neither executes the VU
body nor models VU registers, pipeline timing or exceptional hardware state.
No VU instructions, assembly fallback or scheduling patch is inserted in this
batch's scalar C, and the initializer's bytes are not counted as new recovery.

Node destruction checks the owned-list pointer, clears it through the original
entry, then reloads the name. A non-NULL captured name is freed and the name field
is cleared after the call. Child head is loaded after these calls. Before each
recursive destruction, the next sibling is captured; afterward the captured
sibling's next word is loaded fresh. Changes to the destroyed child's next do
not replace this captured sibling. Changes to the captured sibling's own next
can terminate traversal before it is visited. After children, the node is detached
when either link is nonzero. Its optional destructor is loaded after detachment;
the callback receives the captured node. A missing callback invokes original free.

Record-list destruction captures each successor before unlink/free calls. The
payload pointer is loaded after unlinking, payload free precedes node free, and
the captured next node's successor is loaded after both frees. Payload fields are
not cleared. The captured list container is freed last, including an empty list.
The owner-clear wrapper clears +B4 only after the entire list destruction returns,
even if a call changed that field. Replacement retains its original new-list
argument through all previous-list calls and installs it after clearing. Replacing
an owned list with itself preserves the original dangling-pointer result; the
reconstruction adds no identity guard or automatic ownership repair.

Inputs must satisfy the original list/sentinel and ownership geometry. Detachment
requires valid neighbors when a link is nonzero. Record containers, payloads,
names and nodes must satisfy the selected free/destructor contract; recursive
traversal must terminate within valid memory. No NULL-node, cycle, duplicate-free
or output-size guard is invented. Return declarations are void where original
operations and known caller consumption support only side effects; unknown
source-level typedefs and incidental unused v0 values are not inferred as results.

All complete raw/disassembly hashes, terminal JR/delays, local branches and
excluded alignment are audited. Ten encoded entry JALs are checked inside their
complete containing bodies, including the two constructor callers. The six
bodies preserve 14 genuine numeric calls and the optional JALR destructor.
Reviewed list initialization/detachment C and the existing text-node record layout
are reused. All 18 standard compiler candidates link naturally and completely,
with zero remaining relocations and zero complete byte matches. GCC 2.9 with the
authentic save128 macro is the canonical candidate recipe; all six remain
reconstructed.

The scoped tracer reuses the established decoder unchanged, adding only the
observed byte/halfword stores with low-word addresses and exact store widths.
It rejects unaligned halfword stores, unknown memory/calls/code and unsupported
operand forms. Both actual reviewed list helper bodies execute with their real
delays; only known callbacks, free and the VU initializer use explicit substitutes.
Only an actual JR31 returns outside an owned body, with its delay inside that body.
Six guards cover store widths/alignment, complete code/helper scopes, memory and
readonly rejection, controlled arguments/VU exclusion, real returns/delays and
inherited reserved operand forms.

The 93 authored fixtures execute 8,958 original instructions, including reviewed
list helpers and real nested calls/delays, at most 208 per fixture. They cover
empty/full/NULL record lists, payload NULLs, detached/attached roots, child
recursion, name/callback alternatives, negative-zero constructor inputs, callback
and call-visible mutations, fresh payload/name/destructor/head reloads, captured
successors, post-return owner clearing, captured replacement and self-replacement.
Every buffer word, call count and event is compared exactly. Native tests pass
64,188 checks. The harness includes the actual reviewed `list.c` and licensed
`list_aros.c` with symbol macros for observation wrappers; unrelated diagnostic
entries have authored stand-ins and are never called. No original diagnostic
strings, instructions, tables or game assets are copied.

Free substitutes retain dead storage for snapshots and apply explicit authored
mutations to test caller observations. This does not claim that ordinary retail
free exposes a callback or mutates arbitrary live fields. It is not a model of
heap coalescing, post-free storage, arbitrary invalid inputs or VU/FCR/timing
behavior. The complete compiled-code/size/relocation comparison remains the
exact-match gate.

The constructor and unlink mutation fixtures separately add authored post-call
hooks after the unchanged scalar helper bodies or fixed VU output contract.
Those extra hooks test the scalar caller's capture, reload and sparse-store
schedule; they are not claimed effects of the original pure list/VU helpers.
Instruction counts include executed original instructions only, excluding the
authored substitute effects and hooks.

Regenerate the authored fixture header with:

```text
.venv/Scripts/python.exe tools/trace_property_lifecycle.py --golden-header tests/native/property_lifecycle_golden.h
```

Input keys `routine`, `mode`, `mutation`, `replacement`, `coordinates`,
`float_words`, `initial` use sorted-key UTF-8 JSON with comma/colon separators,
giving SHA-256 `1476d9b15290803239c3fe19ecc6242fdeb3954acfff1c293568db8cb65ab755`.
The header's CRLF SHA-256 is
`4c83715c9a1bfd56d22ae304eed3c6231a3a34dcb8b3c06ca521829144fef226`;
canonical LF is `621b8b8dc0914bc804e3924956ece9b23c5724336efb0948013438bafe419cc8`.
The isolated full comparison/identity/native command packet is in
`.local/property_lifecycle`.

The parent independently reviewed all 183 original instructions and the complete
source, header, scoped tracer, native harness, six guards and documentation. It
also read all 48 VU initializer bytes, 20 list-initializer bytes and 36 unlink
bytes. All 18 fresh genuine links and the entire metadata/caller/helper hash packet
reproduce exactly; 93 regenerated fixtures retain 8,958 instructions, maximum 208,
the input digest and canonical LF header hash. Independent native execution passed
64,188 checks and six guards, compiling harness and production as separate
translation units. No semantic defect was found. Registration is approved; all six
remain reconstructed, with zero complete byte matches.

Two entries have unresolved incoming reachability: `002B9720` and `002B9AF8`.
Their complete prologues, bodies, returns, delays and adjacent boundaries are
verified. The entire text has no encoded J/JAL target to them; file-backed
allocated non-executing sections have no aligned word equal to either entry. A
complete simple same-body earlier-LUI plus ADDIU/ORI candidate search also finds
none, including signed carry and different destination registers. This deliberately
broad candidate search does not establish reaching definitions or computed use.
Other arithmetic/data paths, cross-function register values, unaligned pointer
fields, dynamic initialization and computed/table dispatch remain unresolved.
No incoming reference or runtime execution is asserted for these two controls.
