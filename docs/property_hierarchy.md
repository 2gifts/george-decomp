# Property hierarchy utilities

This connected batch reconstructs six complete routines, 1,032 original bytes
and 258 instructions, in `src/game/property_hierarchy.c`:

| Entry | Bytes | Behavior |
| --- | ---: | --- |
| `002B9928` | 132 | First exact-name match in one rooted hierarchy |
| `002B99B0` | 164 | Collect substring matches in preorder |
| `002B9A58` | 156 | Collect matching key words in preorder |
| `002B9B68` | 156 | Flag-controlled callback traversal with early zero return |
| `002B9D88` | 172 | First exact-name match across an outer intrusive list |
| `002B9E38` | 252 | Collect key matches across an outer intrusive list |

The partial node prefix reuses the reviewed next/previous `GeorgeListNode` and
embedded `GeorgeList` layouts. Observed fields are key word +0C, embedded children
list +60 and optional name pointer +B0; intervening bytes remain untouched.
Offset/size assertions apply to target and native 32-bit builds. The complete
original unowned constructor `002B9440` calls the reviewed list initializer
`002AD9A8` with `node + 60`, at `002B9470`/delay `002B9474`. This proves an
embedded list rather than a separate child-container pointer. Its body is used
only as layout evidence and is not counted as recovered. The shared sentinel's
next word is zero; only nodes with a nonzero successor are recursively visited.
The 180-byte prefix does not assert the object's complete original class or size.
The key's broader engine meaning remains unknown.

All traversals capture each next sibling before the recursive call, then load
that saved sibling's own successor after the call. Callback changes to the
current sibling's next do not replace the captured successor. Changing that
successor's next can prevent it from being visited, as the original checks it
before recursion. Own-node comparisons or callbacks happen before a fresh child
head load. Collectors write the matched output pointer before this load, so
aliases of output storage and live node fields remain observable. Output advances
and accumulated counts use the observed low 32-bit shift/add arithmetic.

Exact-name search NULL-gates the captured name, uses the original comparator and
returns the first non-NULL recursive result immediately. Substring collection
NULL-gates the name and records every non-NULL numeric substring result. The two
outer wrappers keep the original inline root logic through shared ordinary C
templates; recursive calls still target the original `002B9928` or `002B9A58`
entries. Captured outer successors survive comparison and child recursion. No
additional engine calls or instruction fallback is introduced.

The type/key collector receives a remaining-capacity word, passes a wrapping
`remaining - collected` value to recursive children, and never checks it itself.
The outer-list collector tests the unsigned accumulated count against capacity
only after a complete root and all its children. A capacity of zero still
processes the first whole subtree; any subtree can overshoot the stated limit.
The reconstruction preserves this behavior and requires storage for the actual
number of matches. It does not add a capacity guard or truncate results.

Callback traversal starts with result one. Flag bit 0 enables the current node's
callback; bit 1 enables child recursion. Every descendant forces bit 0, retaining
the other flags. Zero stops immediately, while a nonzero callback word survives
unless a later child changes it. Empty children preserve the current result.
The 32-bit signed callback declaration retains negative word results in the
tested ABI; its broader source-level typedef is not recovered. Callback arguments
are the current node and captured user data. Fresh child-head observations and
saved siblings are retained across calls.

Input nodes and list heads must be valid, child lists must have the original
sentinel geometry, strings must be terminated, callback arguments must satisfy
the selected ABI, and traversal must terminate within valid memory. The source
adds no NULL-node guard, cycle detection or output bound. The scoped trace uses
bounded authored graphs to test these preconditions, not to invent runtime
guards. Names, class identity and broader external use of the two outer wrappers
remain uncertain.

The original `.text` contains seven genuine direct JAL references to these
entries, including recursive calls and external wrapper `002B80D4`. All reference
instructions, complete containing bodies and full original hashes/bounds/returns
and excluded alignment are audited. The six bodies have nine encoded direct
helper/recursive calls plus one linked indirect callback at `002B9B9C`.
Numeric string comparator `00393A28`, substring `00398628` and original recursive
bindings remain intact. There are no original literal or readonly table
dependencies. All 18 standard compiler candidates naturally link completely
with zero remaining relocations and zero exact matches. GCC 2.9 with the
authentic save128 macro is the canonical recipe; all six remain reconstructed.

The reuse agent independently confirmed that the complete 116-byte substring
callee corresponds to unchanged public newlib 1.8.1
[strstr.c](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strstr.c).
The native adapter includes that unchanged tracked import `src/runtime/strstr.c`
with only symbol and historical-header macros, under the existing distribution
license in `LICENSES/newlib-1.8.1.txt`. Its canonical source SHA-256 is
`09437b966be6a045479e511b84d9ef0156823a5c9ccd29dc4c93d0f7c53998e7`.
LF-normalized vendor bytes were independently confirmed equal before replacing
the isolated vendor include; the final harness has no vendor dependency.
The separately owned runtime import passed the parent's complete algorithm,
license and link review. The production call binding remains numeric. Native exact-name
comparison uses a controlled host comparator on authored ASCII strings; it
tests equality and caller observations, not the full libc implementation.

`tools/trace_property_hierarchy.py` reuses the reviewed scoped decoder unchanged
and adds only the observed SUBU form, rejecting reserved shift operands and
retaining its wrapping low-word subtraction. All six complete original bodies
execute recursively, including their real call and return delays. Only an actual
JR31 may return to an external stop; transfers and delay slots must stay inside
the active complete body except proven calls. Unknown memory/code/table scopes,
calls, callbacks and unterminated strings fail. Six focused guards cover
subtraction, whole-body scope, memory/call bounds, actual returns and delays,
synthetic nested recursion with a saved link, and inherited reserved forms.

The 153 authored fixtures cover exact/substring/empty/missing keys, NULL names,
leaf and multi-level graphs, empty outer lists, all flag combinations and early
zero points, negative callback results, zero and small capacities with overshoot,
output-field aliases, fresh child-head changes, captured-sibling changes and
successor termination. They execute 15,132 original instructions including
nested calls and delay slots, at most 180 per fixture. Every buffer word, returned
pointer/count, call count and event is compared exactly. Native execution passes
100,057 checks. Heap allocation and hardware/FCR/timing behavior are outside this
integer caller-observation model; arbitrary invalid callback mutations are not
claimed. Complete target-byte comparison remains the exact-match gate.

Regenerate the authored fixture header with:

```text
.venv/Scripts/python.exe tools/trace_property_hierarchy.py --golden-header tests/native/property_hierarchy_golden.h
```

Sorted-key UTF-8 JSON with comma/colon separators over `routine`, `key`, `shape`,
`mutation`, `alias`, `flags`, `remaining`, `stop`, `callback_value`, `initial`
gives input SHA-256
`0da3e54efd50e395e097ce1e849ebc360b225483cc64a161f69efd6456d2fba5`.
The header's CRLF SHA-256 is
`c035023447977f015600bda944b98004bec5c962539cc1c4f7863b5cd3cb4b3b`;
canonical LF is `ab5f79276010c27923c23dbf1a3c3d87680d7eb0ba39f1eb909a73482980ad93`.
Only synthetic integer input/output/event words are exported, never original
instructions, tables or game assets. The isolated comparison/identity packet is
in `.local/property_hierarchy`. The parent independently reviewed all 258 original instructions and the complete
source, header, private template, tracer, native adapter, six guards and documentation.
Its audit verified the entire original hashes, bounds, returns, delays and excluded
alignment, all seven genuine entry JALs with complete containing bodies, all nine
helper calls, the JALR19-to-31 callback and the constructor/list-argument proof.
All 18 fresh complete comparison dictionaries and the entire identity packet
reproduce exactly. Regenerated 153 fixtures retain 15,132 executed instructions,
maximum 180, the input hash and both header hashes; independent native execution
passed 100,057 checks and all six guards. The native command compiles
`tests/native/property_hierarchy.c` and `src/game/property_hierarchy.c` as separate
translation units. No semantic defects were found. Registration is approved;
all six entries remain reconstructed with zero complete byte matches.
