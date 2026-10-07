Five pooled-release callbacks reuse the entire unchanged, reviewed
[actor_controller.c](../src/game/actor_controller.c) translation unit. The new
entries are `0019A3D0`, `0019D3F0`, `001F0120`, `0022CB40` and `00276C60`, each
84 bytes. Every complete original body is byte-identical to `0016CED0`.
Four zero padding bytes following each entry are excluded. These are final
reviewed reconstructions registered after parent and distinct final review; no exact match is claimed.

Each function captures its object and mode, calls `003064F0(object, 0)`, then
tests the captured mode's low bit. When set, it reloads manager `004961F4` and
the unsigned halfword at object offset 4 after the base callback, and calls
`002E2CD8(manager, object, size, 0x27)`. A callback can change either value.
The void declaration describes observable effects only; incidental `v0` is
not a promised result. The original saves and restores complete 128-bit
`s16`/`s17`, checked separately in the instruction observer.

Five observed readonly, zero-adjusted callback pairs and their actual table
publications prove entry identity. They do not prove complete class names,
ownership, allocator extent, table capacity or complete runtime ancestry.
Only the consumed unsigned halfword at offset 4 is typed. The authored
six-byte prefix carrier is not an original full object. Manager storage is
NOBITS; there are no initialized file bytes or a data mapping award. The small
`0022C888` caller is fully read; the four larger constructors are reviewed in
bounded publication windows. Callback replacement by external code remains
possible, so publication alone is not a blanket invocation claim.

The source-free FINAL2335 and historical FINAL1475/descriptor FINAL2281 inputs
are retained unchanged. The parent and distinct source-free gates reviewed
the full original/source/interface evidence. The parent metadata-only seed
supplement declares the exact source/header fingerprints without changing any
source, compiler flags, original bytes, prior row fields or matching status.

The primary cached GCC 3.2.3 object emits 84 bytes with 56 differing bytes at
each new original address. The cached GCC 2.9 object emits 96 bytes with 63
differences; its separately labelled SAVE128 recipe emits 96 with 59
differences. All fifteen complete functions are naturally linked with no
remaining relocation. Public `reuse_compiled_symbol` drops only the selected
seed-definition binding; every helper/global stays canonical and the global
seed address remains unchanged. No private symbol overrides or byte patches
are used. Historical whole-source compiler command output and actual `-M`
closures were not recorded; five newly measured target `-M` closures are
current evidence, not reconstructed historical records.

Two genuine current target ABI objects and ten complete linked callers prove
the two low-word GPR arguments, pointer/u32/u16/long layout, and no change to
the seeded `f12` lane. They do not establish native-to-EE ABI identity. The
source is reused as a whole translation unit for target links. Host execution
uses seven exact byte spans from that source: required include, global and
callee declarations, ADDRESS/FIELD macros, and the complete 175-byte method.
Five explicit symbol-renaming compiler defines instantiate that same source
body. Each entire source/header, span, generated TU, command and actual
dependency closure is hashed. The unchanged old native controller TU supplies
controlled base/release callbacks; its historical 1,488 checks are neither run
nor included in the new count. Other compiled but unreached old methods gain
no award.

The strict observer executes the five actual original PC ranges through the
unchanged published integer decoder. There are 10,080 initialized, aligned
synthetic-carrier fixtures, 190,080 original instructions, maximum 21 per
invocation, all 105 selected instructions and both mode branches. Base and
release calls are explicit controls rather than allocator or SDK models.
Fixtures change release size, manager and unknown first word in the base
callback and clobber caller-saved scalar registers. Null manager cases cover
only that controlled caller contract, not real allocator validity. The native
harness executes all five host symbols and passes 286,569 checks, comparing
every carrier word, manager and callback argument through an exact known
pointer translation ledger. Twenty-four native `-M` closures and five current
target closures identify 56 distinct whole input files.

Nine guard methods pass; absence of the private original yields six passes
and three explicit skips. Three actual native wrong-source controls fail:
capturing size too early, capturing manager too early, and replacing the
low-bit predicate with a nonzero test. An early test-only guard incorrectly
expected valid `JR $1` to be reserved, and an unknown-external test ran without
fixture initialization. The exact failed scripts/diagnostics remain archived;
the final guard uses a genuinely reserved encoding and rejects unknown
targets before accessing parameters. No production, cached source, fixture
result or shared decoder changed. Whole target/native objects are checked RAW.
Native PE comparison permits only actual ImageHlp-parsed timestamp/checksum
fields; target matching never uses that exception.

Reproduction uses the ignored `build/pooled_destructor_duplicates` producers
and fixed cached target recipes. Public checks are:

```text
.venv/Scripts/python.exe tools/trace_pooled_destructor_duplicates.py --golden-header tests/native/pooled_destructor_duplicates_golden.h
.venv/Scripts/python.exe tests/test_trace_pooled_destructor_duplicates.py
.venv/Scripts/python.exe tests/native/run_pooled_destructor_duplicates.py
```

No helper/source/data/class/SDK identity or extra historical native count is
credited. Final review and canonical registration are owned by the parent.

Parent and distinct whole contained replays passed:18 entire dictionaries/97 strict RAW artifacts/25 whole target links and4 ImageHlp-qualified native PE images. All2534 inputs were RAW equal before this separately archived publication annotation;2533 current inputs plus the complete archived document retain frozen identities. The complete56-file dependency inventory differs only in the proven rotation of its first five complete records after exact private-path translation; the remaining51 records and every per-command prerequisite list, file size and hash are unchanged. Author/private initial observer failures remain preserved. The legacy seed gained only three source fingerprint/declaration metadata fields; no old source, status, flags, binding or report row changed.
