The two complete routines at `0029F370` (700 bytes) and `0029F630` (404 bytes)
are recovered as ordinary C in `src/game/plane_intersection.c`. Both remain
reconstructed: six genuine whole-function compiler/link trials resolve all
relocations, and none matches the original bytes. No helper or data bytes
are credited to this batch.

`0029F370` crosses the supplied XYZ normals, rejects an L1 cross magnitude
strictly below binary32 `38D1B717`, chooses the dominant absolute component,
and solves the two remaining coordinates. Its ties select Y when X and Y
tie above Z, and Z when Z ties either maximum. It publishes a point and the
point plus the **raw**, nonunit cross normal. The point stores occur Z/X/Y;
the endpoint stores occur X/Z/Y after all local coordinates are captured.
The two original zero-fill calls each initialize a distinct local XYZ span.
Parallel rejection leaves both output spans untouched.

`0029F630` captures the endpoint difference, normal dot difference and
offset-minus-dot-origin before calling the three software-double entries.
Its absolute denominator gate uses the full binary64 operand
`3F1A36E2E0000000`, assembled by the original ORI/DSLL sequence. It intersects
the infinite line; it imposes no fraction-range or unit-normal guard.
Success first stores origin X, freshly reads/stores origin Y, and freshly
reads origin Z before final X/Z/Y publication. Shifted output/origin aliases
therefore preserve the original changes to those later reads. Rejection
leaves the output untouched. Valid aligned XYZ/plane/output spans are caller
preconditions; no invented null, normalization or degeneracy check is added.

The compatible cross-normal/two-equation solve and line-plane formula are
adapted from the primary [Coin3D SbPlane source](https://github.com/coin3d/coin/blob/da9c1330c618cff65598b97e881bb896d9ac84ad/src/base/SbPlane.cpp)
at commit `da9c1330c618cff65598b97e881bb896d9ac84ad`. The complete 11,748-byte
upstream file has SHA256
`a7ab9e3c14f037da1c1e524b3d147b4a1e8a9462a373fecf70a58f41bcc59a97`.
Its plane-plane method credits Graphics Gems III, Priamos Georgiades.
The full Kongsberg BSD redistribution notice is retained in the derived C
and `LICENSES/coin3d-plane-intersection.txt`. This is an adaptation, not an
unchanged import or evidence that the game used Coin3D. Coin's squared
component/FLT_EPSILON tests, tie choices, reciprocal multiplication,
normalized direction and class operations differ. The adaptation keeps
the original game's L1/full64 gates, tie decisions, separate divisions,
nonunit endpoint representation and alias-visible output sequence.
The supplementary pinned SbLine read provides context only; its source is
not imported or awarded.

All 276 selected instructions, complete original terminal JR31/delay pairs,
four excluded zero alignment bytes after each body and immediate preceding
bodies are proved against the hash-validated original ELF and generated
assembly. Both genuine encoded JALs (`0029F85C` and `0029F8B4`) belong to
the complete, byte-identified 808-byte caller `0029F7C8`, which the author
read in full. Its observed four-GPR arguments form 16-byte-stride plane
entries and stack XYZ point/endpoint/output spans. No actual whole-text
J/JAL or allocated nonexec aligned pointer root for that caller was found.
Its computed entry reachability remains unresolved. The unreferenced
neighbor `0029E8D8` is explicitly deferred; neither neighbor receives a
source award. No switch mapping, SDK/class identity or table capacity is
inferred from these entries.

The scoped `tools/trace_plane_intersection.py` executes the complete selected
original instructions from the locally supplied ELF. It reuses the reviewed
scalar decoder with a bounded signed64 SLTI extension. The complete 184-byte
zero-fill entry `003936A0` and software entries `00374848`, `00373250`,
`00372CC0` were read for their actual selected ABI contracts. The zero-fill
model accepts only initialized aligned stack locals, fill zero and count 12;
it is a controlled helper contract, not an executed helper-instruction count.
The software calls have explicit pure finite-normal/zero host-double models
with full 64-bit arguments. All selected branches/delays remain within the
owned bodies; unknown instructions, operands, calls, uninitialized memory,
control transfers in ordinary delays, wrong JR31 returns and instruction
bound violations fail closed. No original instruction arrays or assets are
included in the public fixture header.

The 1,317 finite fixtures execute 134,392 selected original instructions
(maximum 125 per invocation), covering axes, signs, maximum ties,
nonunit/parallel normals, thresholds and their adjacent binary32 values,
infinite-line fractions, input overlaps and shifted output aliases.
The input digest is
`3380ddac6e8ff6e7b626be8f46435a5b62f4ffaa0f3b2a1b1c40581b9f41f1ae`.
The real production C compiles as a separate native TU and passes 149,517
warning-free checks: all 64 memory words, result, four call counts and the
complete twelve-word argument/publication events agree. Independent
orthogonal-plane equations, raw cross length and infinite-line solutions
supplement the instruction fixtures. Nine synthetic decoder guards run
without an original ELF. The host uses scalar SSE binary32 operations;
these tests exclude NaNs, infinities, nonzero subnormals, zero division,
EE FCR/exception behavior, hardware timing and full gameplay reachability.
They do not prove EE special-value arithmetic.

The three natural recipes preserve ordinary C. GCC 3.2.3 emits 596/380 bytes
for the two routines, GCC 2.9 emits 584/380, and GCC 2.9 with the proven
save128 attribute emits 584/380. Each comparison includes every emitted
function byte and size; no relocation mask, matching prefix, fallback
assembly, scheduling patch or compiler flag grid is used. Both pinned
compilers also emit the same fourteen-word real target layout observer.
Independent peer and parent final review passed, including all selected
originals, source, target ABI, fixtures and six complete compiler comparisons.
Canonical-only reproduction agrees on every comparison dictionary. Both
registered functions remain reconstructed.

Reproduce the scoped evidence with the locally supplied original:

```powershell
.venv/Scripts/python.exe tools/trace_plane_intersection.py --golden-header tests/native/plane_intersection_golden.h
.venv/Scripts/python.exe tests/native/run_plane_intersection.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_plane_intersection.py
```
