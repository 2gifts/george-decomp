# Camera target and fallback basis

Two complete routines recover 1,312 original instruction bytes, all 328
instructions and return delay slots. They connect directly to the already
reviewed camera transform initialization and matrix pair. Existing partial
transform, vector and matrix layouts are reused unchanged. The normalized-vector
entry `002A3538` is already recovered; the original custom trig, angle, matrix
inverse and software floating-point calls retain their numeric bindings. These
custom wrappers are newly reconstructed C, with no asserted upstream identity.

| Address | Behavior | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `00299BE0` | Mode-selected target update and predicate | 392 | 328 | 328 |
| `00299D68` | Construct forward and inverse camera basis | 920 | 848 | 840 |

All six standard candidates naturally link completely at their original
addresses without remaining relocations. None matches every original code byte
and size; both entries remain reconstructed. The instruction-backed C switch is
retained; its ordinary compiler output happened to need no jump table. No linker
mapping expansion, assembly body, hand-patched bytes or flag sweep is used.

The original five-word switch at `004455A0`, file offset `003465A0`, is completely
within readonly `.rodata`, flags `2`. Its 20-byte SHA-256 is
`1af4994c85ca60c5868e1399b54e143dfc1e3cf03f82a7af29aa5ee36468aed3`.
Unsigned mode `<5` is checked before the original table load/indirect jump.
Targets are `00299C24`, `00299CA8`, `00299D48`, `00299D40`, `00299D48`,
respectively. Modes zero and one update the target and return zero. Mode three
returns one without calls or stores. Mode two, four and all out-of-range words
return zero without calls or stores. The tracer reads and hash-checks this entire
table from the validated local image; no original table bytes are published.

Modes zero and one each perform four ordered original trig calls, freshly
loading pitch for each of the first two and yaw for each of the last two. The
four results are retained across calls. Mode zero freshly captures radius and
position afterward, preserving `(radius * sin_pitch) * sin_yaw/cos_yaw` and
`radius * cos_pitch`. Mode one retains multiplication by the exact float
`BE4CCCCD` before multiplying the horizontal components by `sin_pitch`.
Both preserve target X/Y/Z stores and the original position capture timing.
A callback changing the mode during those calls does not select a new case.

The fallback first captures target-minus-position, normalizes that local vector
and retains the first returned length across every later call. Roll is loaded
separately for the two original trig calls. A full 64-bit conversion of the
normalized Y component is compared with zero; negative values pass through the
original software subtraction before the second comparison. Its threshold is
exactly `3FEFAE1480000000`, double `0.9900000095367432`, constructed by the two
original 64-bit shifts. The complete eight-byte encoding SHA-256 is
`79e0637b2549c386ae7ee63a5dab585ce58041451378d96b01ed3aaa7656dd37`.
This chooses reference Z only for absolute Y strictly greater than that
threshold, and reference Y otherwise. The source preserves all 64 operand bits
and the original compare/subtract calls instead of substituting a float test.

The remaining arithmetic rotates that reference about the direction, then
normalizes it, its cross product with the direction, and the subsequent direction
cross product. Every multiply, subtraction and nested addition retains the
original order. Saved local vectors can be modified by later callbacks, so fresh
reads after those calls remain meaningful. Forward matrix stores retain their
observed sparse order, including the Y/X/Z first row, other basis rows, live
target X/Y/Z reloads, and final homogeneous zero/one fields. Source/output aliases
can alter later target reads or the mode word. Only after all those stores is
the mode freshly tested for two; that path stores the retained first length and
performs two ordered original angle calls. The second call freshly reads its
local vector after the first returns. The original inverse receives the captured
second output pointer and the complete forward matrix as its input.

Both full body hashes, all local branches, terminal instructions and any excluded
following zero padding are tracked in draft metadata. Three actual encoded entry
JALs establish direct context: initialization `0029A22C` and matrix pair
`0029A324` call the predicate; matrix pair `0029A380` calls the fallback. The
predicate result controls that pair's branch, while initialization ignores it.
The fallback's unused original return registers do not establish an additional
normalized result. Metadata also records all genuine materialization sites for
float encodings `BE4CCCCD`, `3F800000` and `3F000000`.

The focused native 32-bit harness passes 88,568 checks against 473 authored finite
controlled-call original-instruction fixtures. These cover all switch cases and
large unsigned modes, trig callback changes, positive/negative vertical direction,
the exact Y threshold and either neighboring float encoding, zero direction,
shifted forward/inverse/source aliases, fresh post-output mode changes and saved
local-vector mutations. Distinct normalization return values expose use of the
first length. Events capture the ordered numeric calls and their float/full-double
arguments, including the complete forward matrix before the inverse substitute.
Untouched words, integer mode stores and zero/one stores are exact. Other finite
float outputs and derived event inputs use tolerance `3e-6 * (1 + abs(expected))`;
zero and threshold software-double operands are exact across all 64 bits.
The build uses `-ffloat-store` and `-fno-strict-aliasing`.

The tracer reuses the reviewed `CameraTrace` instruction decoder and finite
normalization model unchanged. A separate subclass adds only instruction forms
present in these two bodies: word/64-bit shifts, full-width DADDU, unsigned
immediate comparison and the two signed branch forms. Six guard tests cover
preserved full-width moves and threshold construction, signed branches, unsigned
gates, reserved operand forms, complete readonly table identity/bounds, and
unknown code/memory/calls. No shared decoder is changed.

Regenerate synthetic fixtures explicitly with:

```powershell
.venv/Scripts/python.exe tools/trace_camera_basis.py --golden-header tests/native/camera_basis_golden.h
```

They execute 98,863 original instructions including delay slots, at most 225 per
case. The input packet keys `routine`, `mode`, `mutation`, `direction`, `output`,
`inverse`, `initial` use sorted-key UTF-8 JSON with comma/colon separators. Its
SHA-256 is `9e87b5252d84a747e6d5d6760aff3cb1a89849f9b4e86524ff23df43faa27ea1`.
The synthetic header SHA-256 with Windows CRLF line endings is
`e76fdf8dddc573e2240c74d25481f0a5e6906303821d83b81c1b427677fb5c05`;
canonical LF is `624cc1cfbd357ce63d545e58e03342ab9b5e88e8e4843b0cfcc88dfb112c1891`.
Only synthetic input/output/event words are exported.

Trig, angle, inverse and software-double operations are explicit finite host
call substitutes. Normalization's threshold fixtures additionally author its Y
output to exercise strict equality and either neighboring encoding; callback
mutations intentionally expose caller observations and are not assertions about
the real engine's callbacks. The inverse substitute captures and negates matrix
words to test pointer roles/order; it does not test the actual inverse algorithm.
These models exclude EE special arithmetic, FCR flags, VU/SQRT precision,
wide-register/cycle behavior and unmodeled invalid pointers or capacities. Target
complete-byte comparison remains the match gate.

The parent independently reviewed all 328 original instructions and the complete
source, header, tracer, native checks, six decoder guards and documentation. Its
separate audit confirmed both full hashes and boundaries, three real entry JALs,
all float materializations, the full 64-bit threshold and complete switch table.
Six fresh natural link comparison dictionaries exactly reproduce the packet;
473 regenerated fixtures, all 98,863 executed instructions, both header hashes
and 88,568 native checks also agree. No semantic defects were found. Registration
is approved; both entries remain reconstructed with no complete byte match.
