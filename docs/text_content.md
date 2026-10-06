# Direct parsed-content utilities

Eleven complete adjacent routines reconstruct 704 original instruction bytes,
all 176 instructions and every return delay slot. They reuse the reviewed
`GeorgeTextLookupContext` storage and the original scanner, case-insensitive
comparison and string-copy bindings. Format/truth declarations come from the
existing text-number and text-value headers; no library implementation or
original data is copied into this batch.

| Address | Behavior | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B5720` | Four float outputs | 60 | 64 | 60 |
| `002B5760` | Three float outputs | 56 | 56 | 52 |
| `002B5798` | Two float outputs | 44 | 48 | 44 |
| `002B57C8` | One float output | 40 | 40 | 40 |
| `002B57F0` | Three signed-word outputs | 56 | 56 | 52 |
| `002B5828` | Two signed-word outputs | 44 | 48 | 44 |
| `002B5858` | One signed-word output | 40 | 40 | 40 |
| `002B5880` | Copy content | 36 | 40 | 36 |
| `002B58A8` | Write short-circuit truth result | 140 | 140 | 152 |
| `002B5938` | Return short-circuit truth result | 120 | 140 | 148 |
| `002B59B0` | Rewind and reset sparse fields | 68 | 68 | 68 |

All 33 candidates naturally link at their original addresses without remaining
relocations. The full 36-byte copy wrapper matches under both GCC 2.9 variants;
the other ten bodies remain reconstructed. Parent independent fresh links reproduce every comparison exactly. The copy
entry is registered matched; the canonical verifier retains the complete-byte
gate. Complete original-body hashes, excluded zero alignment,
literal hashes, and all profile comparisons are retained in the review packet.

The seven scanners pass `context + 3030`, the original readonly format and one
through four captured output pointers to `00395350`. Pointer order is preserved
in all three through six integer argument registers. These wrappers perform no
output initialization and no lookup. Partial conversion leaves unwritten caller
outputs at their prior values, including intervening callback mutations. The
known scanner integer return remains in `v0` throughout the original epilogue;
the reconstructed prototypes expose that conversion count unchanged. Similarly,
the content-copy entry forwards its two captured arguments to `00393B74` and
preserves that known original string-copy pointer return.

A full scan of encoded JAL instructions in the validated original text section
found no direct reference to any of these eleven entries. Their complete
prologues, coherent argument/callee sequences, local branches, stack restores,
return delays and adjacent overload boundaries provide entry evidence. They do
not establish whether unused original source declarations were void or typed.
The scanner/copy result types above describe observable known-callee ABI
propagation rather than an invented caller use. The output-taking truth entry
is void: its failure path retains the arbitrary final comparison result in
`v0`, and success sets `v0` to one for the output store. The separate returning
truth entry explicitly normalizes its result to zero or one.

Both truth routines capture the content address once and compare the four
original strings in order: `yes`, `on`, `true`, `1`. They stop at the first
case-insensitive match. The output-taking entry clears its output word before
the first comparison, so an output overlapping the content can erase it. A
successful path writes one; all failures perform no further output store and
preserve callback mutations. This differs from the separate lookup-taking
`002B4188` entry, which executes all four comparisons. Neither routine trims
whitespace or accepts a truth-string prefix with trailing characters.

Rewind first captures the original `+08` base pointer, clears only the first
content byte and stores the captured pointer at `+0C`. It sets code `+14` to
`8001DEAD`, clears the full flag/count words at `+00`, `+18` and `+101C` through
`+102C`, and clears only the first raw/name/attribute bytes. It preserves the
float at `+04`, base and length fields, every other string byte and all remaining
storage. No allocation, parser invocation or string dereference is introduced.

Valid context storage, NUL-terminated content, correctly aligned writable
outputs and sufficient copy capacity remain caller preconditions. The scanner
and copy keep their original numeric callee bindings. This batch makes no
claim about out-of-range conversion, original vectorized copy overlap or EE
floating-point exceptional behavior.

The scoped native 32-bit harness passes 231,402 checks. Its scanner validates
the exact format identity and all typed variadic output pointers before using
host `vsscanf` for complete, partial, invalid and empty conversions. Substituted
returns of minus one, zero and seven must propagate unchanged. Distinct, shared
and reordered output storage, outputs inside otherwise untouched context fields,
and callbacks changing an unwritten output or the content are checked. Truth
tests verify case handling, exact call order/count, short-circuit behavior,
post-comparison content changes, output mutation and content/output aliasing.
Copy tests cover both destination regions, untouched trailing bytes and controlled
callee-return propagation. Rewind checks every context byte against explicit
sparse expected stores with five base-pointer aliases and repeated calls.
The harness supplies controlled callee behavior and ordinary initialized host
memory; it does not independently reproduce the original runtime or hardware.

Parent independent review passed all 176 original instructions and the full
source/header/native/docs packet. All eleven full hashes, return boundaries
and local branches, absence of actual direct entry JALs, and all seven format
plus four truth complete readonly identities were independently audited. All
33 fresh actual links reproduce the recorded comparison data exactly after
JSON parsing, including the full 36-byte GCC 2.9 copy match. The independent
native rerun passes 231,402 checks.
