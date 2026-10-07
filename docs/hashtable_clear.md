# Hashtable clear: licensed C adaptation

`func_002BF418` is the complete 156-byte, 39-instruction leaf at
`0x002BF418`. Its actual return is at `0x002BF4AC`, with the full element-count
zero store in the delay slot at `0x002BF4B0`. The following four-byte NOP is
excluded. Its one encoded caller is `0x002BEEA8`, in the complete 336-byte
destructor at `0x002BEDF0`; that caller passes the captured owner-plus-four
table and ignores the return register. All five executable sections were
scanned. Five other complete byte-identical bodies remain excluded and
unawarded.

This is an explicitly modified licensed C translation of the complete SGI
`hashtable::clear()` traversal and fixed-size default deallocation. The
unchanged primary headers are pinned to
`SSXModding/ps2-ee-toolchain@b595ded606227e93b8c4a447446c1d2ac093827d`.
The source retains the complete HP1994/SGI1996–1997 permission notice and
records the modifications dated 2026-10-07. Both notices must accompany
copies and supporting documentation; the full project notice is retained
in `LICENSES/SGI-STL.txt`. This does not establish original source identity.

The source-free proposal initially serialized the wrong allocator member:
an unqualified first-match search selected `debug_alloc` lines260–265.
The separate immutable author erratum under
`.local/hashtable_clear_scope_author_erratum` acknowledges that defect and
proves the complete correct `__default_alloc_template::deallocate` span at
lines427–445, raw SHA256
`16f9b857bc47f6be657438c4a6dc7459459a79d52d00faedc993f5f4e25aa7af`.
The original scope45, prior source-free452, and the old resolver935 index
remain unchanged. Resolver935 has exactly934 current RAW-equal inputs plus
the separately approved checkpoint58 public documentation annotation;
its original authored document is archived. No claim of all935 current
RAW equality is made.

The existing numeric node12/table20/range12 header is reused unchanged.
For this fixed node, the authentic eight-byte alignment maps size12 to
free class1 in the existing `D_003F21B8` array. The code captures the old
free head before the node's next pointer, overlays the node's first word,
publishes that node into the free list, then advances the captured next.
It reloads bucket begin after those stores, clears that bucket, and captures
fresh end before fresh begin for the next wrapping32 count calculation.
The licensed upstream traversal captures next before calling deletion;
retail's earlier free-head load is explicitly retained rather than claimed
to be an identical upstream evaluation schedule. Scalar-pair destruction
has no runtime operation. This C translation does not perform or claim
general C++ lifetime-ending/nontrivial destruction semantics.

The two approved default compiler recipes both link through canonical
bindings with zero unresolved relocations. GCC3.2.3 produces a complete
152-byte function differing in117 bytes from retail; GCC2.9 produces152
bytes differing in113. Both comparisons cover the entire source symbol
and entire original body, without cropping or masking. There is no exact
match. Two genuine layout objects independently prove
`[4,4,4,8,12,20,4,8,12,16,0,4,8,16,4]`, including target pointer/int32 and
long64 widths. Two actual whole caller functions establish the one-pointer
interface: GCC2.9 naturally uses an eight-byte tail jump; GCC3.2.3 uses a
full JAL caller. In total there are six target objects and four linked
function ELFs, comprising two selected functions and two ABI callers.
No ABI caller receives recovery credit. Ten exact `-M` closures pin67
unique raw files for the selected/layout/caller and native commands.

The strict observer executes all39 actual original instructions in435
synthetic initialized fixtures, totaling58,155 instructions with a maximum
of366. Every conditional branch has both outcomes. Reads, writes, branch
delays, likely annulment, actual terminal JR31, owned ranges, alignment and
initialization are checked. Scalar execution delegates to the unchanged
published `RegistryTrace.execute` through a narrow original-family
allowlist and a1200-instruction budget checked before mutation. The initial
handwritten observer and partial proof are preserved under
`build/hashtable_clear_history_initial_decoder`; the entire435 observations,
full trace JSON and synthetic golden header remain RAW-identical after
the tooling reuse change. Nine guards pass, including a budget state
snapshot. A simulated absent-original checkout passes six guards and
skips the three original-dependent tests without moving any original file.

The complete production C is a separate native translation unit. It runs
435 ordinary initialized disjoint C table/node/array graphs, compared both
against every golden word and an independent membership-order oracle that
does not traverse production links. This provides50,460 checks. A separate
reference executes the unchanged genuine SGI clear method on six
constructed hashtables with genuine separately instantiated allocator
storage, adding36 checks:50,496 total. The SGI reference retains historical
GNU object-overlay, empty-storage/null-pointer-subtraction and linker
storage-view assumptions; it is not a universal ISO-C++ object-model proof.
Host malloc/free provide reference backing only. The three authored C TUs
compile under `-Wall -Wextra -Werror`; actual historical ios/typename header
warnings from the genuine C++ unit are retained. The overall build is not
called warning-free. An inactive `cerr` link label supplies no constructed
ostream object; every diagnostic operator immediately fails, and that
path is unexecuted. No kernel, OOM, hardware allocator or helper award follows.

An actual capture-after-overlay mutant fails ordinary fixture27 (two nodes,
one bucket, empty old free list), proving why next must be saved before
overwriting the first word. Its first broad run created a cycle on an
earlier nonempty-free-list fixture and timed out; all created artifacts
and the explicit failure history were preserved before selecting the
finite negative fixture in the native-only harness. The production source
was never changed. Other retained observer corrections concern a GCC2.9
tail-call assertion, pointer-role normalization for adjacent native
objects, and the inactive diagnostic linkage closure. They are not source
algorithm or compiler-flag match adjustments.

The supported domain is initialized ordered contiguous buckets, finite
acyclic distinct writable nodes and disjoint existing free-list ownership.
Fixture capacity16 is owned test storage, not an inferred engine capacity.
No null, cycle, capacity or ownership guard was added. Integer subtraction,
signed shift and pointer representation are the measured GNU target ABI.
There is no arbitrary object-alias, concurrent/MMIO/upper-register,
exception/FCR/cache/timing, general EE or underlying allocator claim.
Native replay preserves whole images and permits differences only in the
parsed PE TimeDateStamp and CheckSum fields after read-only Windows
ImageHlp validation. All target ELF/native object/golden bytes remain strict.

Author, distinct independent peer and parent reviews passed, followed by canonical registration. Eleven entire qualified dictionaries and18 strict RAW target/native artifact pairs reproduce; all370 inputs were unchanged before this archived publication-status annotation. The current freeze has369 RAW-equal inputs plus this explicitly archived document. All sources/headers and initial/failure histories remain unchanged. Two complete native PEs retain only genuinely observed parsed timestamp/checksum differences. A separate parent package pins the measured chosen-recipe dependencies and owned modified source without an unchanged upstream identity claim. The function remains reconstructed; no clone/helper/data/exact award is made.
