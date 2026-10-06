# Byte-order conversion helpers

Fourteen complete nearby routines recover 940 original instruction bytes.
Both pinned compiler profiles link every candidate at its original address with
zero unresolved relocations. Four candidates reach complete byte equality:
three fixed-count wrappers under GCC 3.2.3 and the 16-bit in-place loop under
both profiles, totaling 136 bytes. Independent review compared all fourteen
complete original bodies with the source/header and found no defects, including
wrapped product gates and forward pointer/store behavior.

| Address | Observed operation | Original bytes | GCC 3.2.3 bytes | Full equality |
| --- | --- | ---: | ---: | --- |
| `002B2430` | Reverse a zero-extended 16-bit input | 24 | 24 | No |
| `002B2448` | Reverse a 32-bit word | 40 | 40 | No |
| `002B2470` | Reverse the raw encoding of a float argument | 56 | 56 | No |
| `002B24A8` | Identical second 32-bit scalar entry | 40 | 40 | No |
| `002B24D0` | Capture and reverse three raw float components | 152 | 136 | No |
| `002B2568` | Copy three words per wrapped signed input count | 116 | 96 | No |
| `002B25E0` | Call the float-array converter with 16 elements | 28 | 28 | Yes |
| `002B2600` | Call the float-array converter with 6 elements | 28 | 28 | Yes |
| `002B2620` | Call the float-array converter with 15 elements | 28 | 28 | Yes |
| `002B2640` | Reverse an in-place array of raw float words | 100 | 84 | No |
| `002B26A8` | Reverse an in-place array of 16-bit words | 52 | 52 | Yes |
| `002B26E0` | Copy 16-bit words using a wrapped product count | 76 | 68 | No |
| `002B2730` | Reverse an in-place array of 32-bit words | 92 | 80 | No |
| `002B2790` | Copy 32-bit words using a wrapped product count | 108 | 88 | No |

The two scalar 32-bit bodies are byte-for-byte identical in retail. They reuse
one ordinary inline C expression, also used for array elements. The 16-bit
expression is shared similarly. This is standard byte reversal; no particular
external library or upstream implementation is established, so no unrelated
library source or license attribution is invented. These source helpers produce
no new engine call. The fixed-count wrappers retain the observed call to
`002B2640` rather than replacing it with an inline array loop.

The three-component routine reads all three raw words before any write and
stores the converted X, Z, then Y components. Float conversions reinterpret
32-bit representations through a union rather than numerically converting
integers to floats. No floating arithmetic is introduced. The candidate compiler
may emit integer stores in place of retail register-transfer/float stores; the
full byte comparison retains that difference.

In-place loops accept only a positive signed count. Copy loops first multiply
in unsigned 32-bit C, then interpret the wrapped result as the target signed
count. This preserves negative-times-negative copies, positive inputs wrapping
to zero or negative, and negative inputs wrapping to a small positive result.
No signed C multiplication overflow is introduced. Once positive, the counter
decrements to zero without signed overflow. All pointers advance by one element:
two bytes for halfwords and four bytes for words. Each copy reads a whole element
before its corresponding store, then reloads the next element. Forward overlaps
therefore propagate earlier writes; no overlap-safe bulk copy is substituted.
Zero and nonpositive wrapped counts do not dereference either pointer.

Pinned official m2c produced 14 diagnostic-free local drafts. Several drafts
nevertheless advanced an inferred `s32 *` by 4 for an observed four-byte address
increment, which would skip 16 bytes in typed C. Manual instruction review
corrected those increments, type-punned float transfers and wrapped signed
count arithmetic. Clean decompiler diagnostics alone were not used as evidence
of correctness. Full body boundaries include the final return delay slot and
exclude only checked zero alignment padding.

`tests/native/byte_order.c` passes 146,760 checks against a separate byte-based
reference. Coverage includes all 65,536 scalar halfwords, 20,000 generated word
patterns, single-bit inputs, signed zero/infinity/NaN raw encodings through
integer operations, finite float conversions, untouched array prefixes/suffixes,
all shifted overlaps in a bounded buffer, zero/negative counts, fixed wrappers,
and wrapped product/triple counts. Float argument/return and float-store tests
are limited to finite encodings in both directions: native x87 can quiet a
signaling NaN, so those tests do not independently prove EE exceptional-value
or FCR behavior. Integer bit-pattern tests remain exact for all tested encodings.
The harness embeds no original instructions or assets.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness byte_order`.
