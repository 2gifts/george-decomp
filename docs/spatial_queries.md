# Numeric provider and polygon queries

Four complete original routines recover 1,440 bytes / 360 instructions. The
isolated `build/spatial_queries` packet proves complete raw/assembly geometry,
terminal return delays, zero alignment padding, all 68 encoded incoming calls
and their full containing-body identities. None duplicates a registered full
original body. Numeric names and partial field views do not establish original
class names or an asset format.

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `001CC960` | 200 | Provider bounds lookup; first hit's validated nested pointer. |
| `001CCA28` | 124 | Provider lookup using incoming word masked by `0x7FFF0000`. |
| `001CF340` | 580 | First matching transformed polygon/height record at `0x60` stride. |
| `001CF588` | 536 | Polygon/height predicate for the keyed record index. |

The two lookup routines retain first-hit behavior even if that hit's nested
pointer is null or its first word is zero; neither resumes searching. The point
lookup reloads the manager and byte count after each failure. The key lookup
captures both at entry. Provider flag byte bit 4 gates a header pointer at
`+0x10`; header bounds start at `+4`, and its nested road pointer is at `+0x2C`.
The existing field-`0x300` map-owner view remains compatible with this separate
numeric view of the same global pointer. Existing caller declarations are
unchanged.

Both polygon bodies share an authored source template backed by the complete
original control and value sequences. They call the real point transform for
each XYZ vertex, reload the polygon byte count after every call, then capture
the transformed count for the signed modulo edge loop. Strict component tests
between exact binary32 `-0.1` and `+0.1` skip tiny edges and preserve the previous
side value. Other edges use the original separate products and addition, with
a positive side rejecting the record. The first transformed Y supplies the
strict height band `Y - 2 < pointY < Y + 2`.

## Reuse and limits

Production uses the unchanged published bounds predicate `002A00A0`, point
transform `002A1C60`, vector/road/result layouts and the existing compiler
annotation. The native executable compiles bounds, transform and the new
queries as separate translation units; unused allocator/trig hooks fail if
called. The original tracer executes both complete real helper bodies, with
the existing bounded scalar and VU/MMI decoders. No polygon library algorithm,
emulator implementation or substitute transform is copied or injected.

The transform's existing finite normal/zero binary32 model rounds each product
before the accumulator addition. It does not establish extended VU ACC
precision, nonfinite/subnormal arithmetic, FCR flags, exception/access effects
of unused lanes, timing or identical native stack placement. Numeric bindings
are observed entries, not inferred original ownership or class contracts.

Original scratch is 64 uninitialized bytes below the saved return address.
Initial zero vertices still read never-written first Y: the ordinary C keeps
an uninitialized volatile local, and its output is not claimed to be defined
C behavior or identical caller-stack placement. The decoder rejects the
actual unknown memory read, rather than fabricating a value or installing a
production zero-count guard.

`001CF340` allocates scratch once for the whole record scan and never clears it.
A later empty polygon can read Y actually initialized by an earlier rejected
record. Original/native sequence fixtures cover earlier Y equal to zero and
one, including strict inherited height boundaries. They distinguish genuine
scratch reuse from a fabricated clearing operation. The observer permits these
initialized later-empty reads; it does not reject every zero count.

Counts above five overlap the original saved stack. The decoder rejects that
overlap as its observation limit, and native numerical tests use safe positive
counts one through five plus the proved initialized later-empty sequences.
Production adds no capacity check. The original writable global is initially
null; bounded actual caller windows and three complete short route callers do
not establish a positive count or an asset maximum. No exhaustive asset or
format validation is claimed.

## Validation

Twelve genuine whole-function compiler/link comparisons cover GCC 3.2.3,
GCC 2.9 and its supported optional SAVE128 annotation. All resolve completely;
none matches all retail bytes, so every routine remains reconstructed. Initial
GCC 3.2.3 naturally outlined the large inline polygon helper, producing an
honest missing-retail-binding blocker. The authored `always_inline` directive
for newer GCC expresses the repeated complete original template without a
fake helper binding; old GCC naturally inlines it. This is a transparent source
directive, not evidence of the original source syntax. Comparisons retain the
complete natural compiled sizes and bytes, with no cropping, patches,
instruction masks, assembly or intrinsics.

Both actual compiler ABI objects prove thirteen layout words, including
pointer/float widths, published road `0x60` stride, blob/result offsets and
64-byte scratch. Original inventory has 45 local branches, 21 stores including
saved registers/delays, five helper calls and four returns. Six immediate float
sites independently prove `0xBDCCCCCD`, `0x3DCCCCCD` and `0x40000000`; all source
float literals and zero transfers are audited.

The 1,074 synthetic fixtures execute 285,810 original instructions, at most
997 per invocation. The warning-free 32-bit native runner passes 799,611 checks,
including full synthetic memory preservation and returns, masking/first-hit
semantics, exact tiny-edge boundaries, aliased point/matrix input and nonzero
inherited scratch Y. Eight decoder guard methods cover initialized memory,
complete entry/body boundaries, reserved encodings, signed division, finite
VU operands, actual return/delay control, first-empty reads, later-empty reuse,
sticky tiny-edge side and saved-stack overlap. Original-dependent guard methods
explicitly skip when the private ELF is absent.

```powershell
.venv/Scripts/python.exe tests/native/run_spatial_queries.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_spatial_queries.py -v
# Requires the locally supplied, hash-validated original:
.venv/Scripts/python.exe tools/trace_spatial_queries.py --output build/spatial_queries/trace.json
```

The author read all selected original instructions, complete bounds/transform
originals and published sources, three complete short route callers and bounded
actual argument/return windows. Complete containing-caller hashes do not claim
manual semantic review of every large caller. Parent and independent peer completed final full selected/source/ABI,
fixture and whole-link review. Twelve canonical-only comparisons retain every
complete result. All four registered functions remain reconstructed, with no
exact matching award.
