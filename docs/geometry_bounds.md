# Bounds, plane tests and affine inversion

This batch recovers 13 complete C routines covering 2,056 original bytes. The
eight-byte zero-return leaf and 28-byte frame-builder wrapper match naturally;
the remaining routines are reconstructed after complete comparisons with all
three supported compiler recipes. Small wrapper matches do not establish
matching geometry algorithms.

Existing vector, geometry-frame and rotation-matrix views, normalizer and heap
allocator are reused. The related free wrapper at `002A0218` already uses the
project's identical heap-forwarding body in `duplicate_helpers.c` and is not
counted again. Original class names and the zero leaf's role remain unknown.

Bounds have inclusive endpoints through strict separating comparisons; NaN
comparisons do not reject by themselves. The two plane APIs preserve their
different `dot + w` and `dot - w` conventions. Unsigned counts control point
and sphere loops; signed plane tests reject zero counts but accept negative
counts. Closest-distance writes tolerance before any nonzero count, reloads
point and plane values after output stores and retains the original double
conversion/comparison/subtraction path. Frame dimensions and center values are
captured before writes and normalization calls, leaving position.w untouched.
Allocation and record-address arithmetic retain word-width wrapping and the
original capacity preconditions.

The affine inverse stores cofactors while still reading the input matrix,
retains selected cofactors for its determinant, uses `1.0e7f` for zero
determinants and captures scaled coefficients before their stores. Translation
reloads after prior translation stores. In-place and shifted overlaps propagate
intermediate values; copying the entire input first would change this behavior.

The native harness passes 8,738 checks, including 134 original-instruction
synthetic fixtures that compare full buffers and return values. Further checks
cover inclusive/unordered gates, signed zero, count and allocation wrap,
closest-output aliases and callbacks changing previously captured bounds.
Tests need no game files. The scoped trace reuses `trace_geometry.py` and adds
only observed SLT/SLTU/BNE/BLEZ/C.LT.S/C.LE.S operations. It reads the local
hash-validated ELF during generation and exports synthetic words alone. Finite
host arithmetic does not establish EE exceptional arithmetic, FCR or timing.

An independent agent reviewed all original bodies/source/header layouts,
full original hashes and bounds, all 39 fresh complete linked comparisons,
regenerated the 134 fixtures and reran the harness. The header SHA-256 is
`69a1644b19ee24cf2422c8d62de1ad3fb9e97d5d2c996b0d888b2032d80e90a3`.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness geometry_bounds`.
Regenerate with `.venv/Scripts/python.exe tools/trace_geometry_bounds.py
--golden-header tests/native/geometry_bounds_golden.h`.
