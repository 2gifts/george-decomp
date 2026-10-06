# String, search, and CRC routines

Four substantive routines were reconstructed in `src/game/string_algorithms.c`.
Their addresses and signatures remain evidence-based names. All four compile
and link with every relocation resolved, but remain **reconstructed**, because
the resulting complete function bytes are not identical to the retail image.

| Address | Reviewed behavior | Original bytes | Compiled bytes |
| --- | --- | ---: | ---: |
| `0x00295050` | Byte string length | 48 | 44 |
| `0x0029C588` | Float binary search | 104 | 104 |
| `0x0029C5F0` | Streaming CRC update | 84 | 80 |
| `0x0029C648` | String CRC wrapper | 120 | 120 |

The binary search currently differs in one byte: the compiler emits a
branch-likely instruction where the original has an ordinary branch. That byte
is compared and prevents a match claim. The string CRC also retains the known
candidate-compiler `sd`/`ld` versus original `sq`/`lq` callee-save difference.
Neither discrepancy is patched or masked. Function sizes exclude alignment
after the final return delay slot.

## CRC identity and reuse

The table at `0x00445650` is a 256-word reflected CRC-32 table. Every word equals
the result of independently applying eight right-shift/XOR steps with polynomial
`0xEDB88320` to its byte index. Its little-endian 1024-byte SHA256 is
`12f3e0576d447eb37b36d82ba0c1c5481b8f0d12fdc70347ce4a076b229d4c86`.
It occupies `.rodata` at file offset `0x00346650`.

The updater complements its previous finalized CRC, processes each unsigned
byte through this table and a logical right shift by eight, then complements
the result. This identifies the standard CRC convention described by the
[PNG specification](https://www.w3.org/TR/2003/REC-PNG-20031110/#D-CRCAppendix).
The check input `123456789` gives `0xCBF43926`; processing that input in two
chunks gives the same result.

This confirms algorithm identity, but does not identify a particular original
library. For example, [zlib 1.1.4's CRC source](https://raw.githubusercontent.com/madler/zlib/v1.1.4/crc32.c)
has an eight-byte unrolled path absent from this routine. No zlib provenance or
copied-source claim is made. These functions are fresh reconstructions; the
mathematical CRC identity is reused. The tracked source declares the original
table as an external symbol and does not contain a copied disc table.

## Boundary behavior

The streaming updater returns zero for a null buffer regardless of its other
arguments. A non-null buffer with zero length preserves the previous finalized
CRC. The string wrapper starts from a zero finalized CRC and excludes the
terminating zero byte. It calls the unguarded length routine **before** its
later null check. Consequently `CRC(null)` has no safe contract even though a
null-return branch appears later in the disassembly. The reconstruction
preserves this order.

The float search is bespoke. It uses signed division toward zero when forming
the middle index. It stops when the lower and upper bounds meet or cross,
returning the lower index without checking the remaining element. Counts zero
and one both return zero without loading an array element. For `[1, 3, 5, 7]`,
a query above every element returns index three, whereas a conventional lower
bound would return four. Equal repeated values can return the middle element.
Two unordered comparisons take the equality return path; ordinary host IEEE
NaN checks confirm that control flow without claiming a complete PS2 floating
point model.

## Validation

All four functions were linked at their original addresses with the pinned
GNU PS2 linker. The table and length-call bindings are recorded in
`config/symbols/string_algorithms.json` and the relevant function entries.
Complete linked bytes and symbol sizes were compared; remaining relocations
were empty. Individual fingerprints and discrepancy counts are recorded in
`config/functions/string_algorithms.json`.

A locally generated host harness passed 17 checks covering empty/high-byte
strings, the published CRC vector, incremental updates, null and zero-length
buffer handling, embedded string terminators, small search counts, exact and
missing values, duplicates, and unordered comparisons. The harness's CRC
table was generated from the polynomial rather than copied from the disc.
Harness products stay in ignored `.local/string_algorithms/`.

The adjacent math cluster has useful research leads: `0x0029C090` and
`0x0029C168` use sine/cosine-style polynomial approximation; `0x0029C230`,
`0x0029C2A0`, and `0x0029C300` have acos-, asin-, and atan-style reductions;
`0x0029C420` builds a 1024-element tangent-like table. These identities remain
provisional. The EE accumulator and range-reduction behavior requires a
separate reconstruction; no portable approximation was substituted here.
