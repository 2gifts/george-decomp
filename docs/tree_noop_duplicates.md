# Called empty-entry source reuse

The author packet verifies 59 complete eight-byte entries (472 bytes), each
exactly `JR ra; NOP`, by linking the already reviewed `func_002AD208` symbol at
each original address. Every whole linked body matches both original words,
with zero relocations and no mapped data. The original entries have 99 encoded
CPU JAL references in 66 complete mechanically bounded caller intervals.

The counterpart is the existing empty `void func_002AD208(void)` definition in
`src/game/tree.c`. Its unchanged 8,000-byte GCC 3.2.3 translation-unit object and
the authentic prior seven-file whole-TU dependency measurement are reused.
No new implementation, wrapper, compiler invocation, dependency query, ABI
caller, native fixture or instruction model is introduced. The packet retains
59 whole object copies, 59 full linker scripts and 59 complete ET_EXEC files.

The source-free review read all 118 selected instructions and both seed words,
the whole tree source and header, 15 complete small callers (636 bytes / 159
instructions), and all 99 bounded call windows. Complete semantics of the
other caller bodies are not claimed. All five executable sections and the
allocated-pointer and next-100 construction scans are retained; neither latter
scan found a selected root. Other unrooted entries remain outside this batch.

These bodies read no input register or memory and construct no return value.
Unused integer, pointer and floating arguments, and an incidental incoming
return lane, can remain in their callers. The effects-only empty counterpart
does not establish original prototypes, classes, ownership, Tree ancestry or
portable whole-source caller compatibility.

The parent approved only `source_kind: "c"` and the existing seven dependencies
for the primary seed. Each clone uses the genuine compiled-symbol reuse API
and a numeric per-function `link_symbols` entry; that API removes only the
defined seed symbol for the selected link. All real global bindings retain
their values. The earlier source-free prerequisite for 58 global aliases is
qualified by a separate read-only policy audit. This author adds no global
alias or canonical matching credit.

The immutable author evidence is under `build/tree_noop_duplicates_final`:
whole original/source/dependency records, historical catalogs and binding
maps, captured tool versions, all strict artifacts, exact linker commands,
and a contained matching-only replay. Two authentic source-reuse rejection
controls check that a wrong selected symbol or original hash fails before
linking. Final canonical registration and matching receipts are separate from
the preserved author packet; inherited source-free, getter and publication
snapshots retain their original bytes and qualifications.
