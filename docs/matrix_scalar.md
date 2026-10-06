# Scalar matrix routines

Nine complete C functions recover 1,684 original instruction bytes. All 27
comparisons under the three established compiler recipes genuinely link at the
original addresses. The row-dot transform and transpose match under GCC 2.9,
totaling 212 bytes; the other seven remain reconstructed.

| Entry | Bytes | Observed operation |
| --- | ---: | --- |
| `002A1CE0` | 148 | Three scalar row dots |
| `002A1FD8` | 256 | Axis-angle matrix |
| `002A20D8` | 292 | Axis-angle matrix with position |
| `002A2278` | 180 | Four-component translated row |
| `002A2330` | 64 | Interleaved transpose |
| `002A2370` | 144 | Z-axis matrix |
| `002A2400` | 144 | Y-axis matrix |
| `002A2490` | 144 | X-axis matrix |
| `002A2520` | 312 | Three-angle matrix |

Existing recovered matrix/vector layouts and the numeric custom sine/cosine
entry points are reused. The two axis-angle builders share one inline C helper
for the observed arithmetic. Their different trailing store orders remain in
the individual wrappers. Numeric function names avoid asserting unavailable
original engine names. Nearby VU-macro routines are not part of this batch.

The axis-angle builders call cosine, reload the angle for sine, then capture
the three axis components before their first matrix write. This preserves
callbacks changing the angle or axis, plus input/output overlap. The translated
builder clears the position row and sets its last component before reading and
writing each supplied position component. A position pointer into that row
therefore observes those earlier stores.

The row-dot transform reloads both inputs after writing each component. The
transpose also interleaves each load and store. In-place transpose follows the
retail forward loop and can propagate earlier writes. A temporary whole-matrix
copy would change that behavior. Translation computes four components before
its first write. Axis builders preserve cosine/sine order and every output
store. The three-angle builder uses full angles, six C,S,C,S,C,S calls and the
complete original arithmetic grouping; zero angles produce an identity matrix.

Root and a separate agent reviewed every instruction of all nine bodies,
including callback reloads, branch/delay slots, scalar product grouping and
aliased store order. Independent compiled symbol extents and original return
delays determine complete sizes; alignment padding is excluded.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness matrix_scalar`.
The native harness passes 3,245 checks. Sixty synthetic fixtures for the three
complete pure scalar bodies check every word of a 48-word buffer, including
untouched memory and shifted overlaps. Their reference reads the locally
supplied, hash-validated original ELF through the bounded decoder reused from
`trace_geometry.py`. Its one additional BGEZ opcode handles the four-row
transpose loop. Original instructions are never embedded in exported fixtures.
Controlled trig callbacks separately check argument captures, axis mutation,
visible pre-call stores and the unusual position alias behavior.

A separate agent independently reran all 3,245 native checks and regenerated
the complete golden header with SHA256
`a7936c2a4c4f45c19113e0e25eab9b3cb6ea5aa83247cd4bfbf1c64f99502297`.
The additional signed BGEZ gate and its delay-slot behavior were also reviewed.

Regenerate with:

```powershell
.venv\Scripts\python.exe tools/trace_matrix_scalar.py --golden-header build/matrix_scalar_golden.h
```

The reference covers finite host IEEE arithmetic for these fixtures. It does
not establish EE exceptional values, FCR behavior, precision quirks or timing.
Native success awards no matching credit. Only the two complete actually linked
byte-identical C bodies count as matches; there are no opcode patches, masks or
invented stack padding.
