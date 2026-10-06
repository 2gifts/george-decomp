# Engine inverse-angle routines

Three complete connected routines recover 484 original instruction bytes.
Two authentic compiler profiles produce six fully linked candidates without
unresolved relocations. None matches the complete original bytes.

| Address | Observed operation | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `0029C230` | Cosine-input inverse angle | 108 | 96 | 96 |
| `0029C2A0` | Sine-input inverse angle | 92 | 88 | 84 |
| `0029C300` | Recursive arctangent with convergent series | 284 | 264 | 256 |

The inverse-cosine and inverse-sine entries preserve strict negative tests and
their original self-calls. The cosine-input path squares its input, computes
the reciprocal, subtracts one, takes an EE square root, then calls the original
arctangent. Negative inputs reflect the recursive result about the original
binary32 pi. The sine-input path retains the squared input before subtracting
it from one; EE `RSQRT.S` divides the original numerator by the square root
of that result in one target operation. Negative inputs negate their recursive
result. Domain guards and host `acosf`/`asinf` substitutions are absent from
the original and are not introduced.

Arctangent first squares the captured input. Squared magnitudes above one use
the reciprocal and a sign-dependent original half-pi reflection. Values above
the strict squared threshold `3E2FB0CD` use the original square-root reduction
and double its recursive result. The remaining path accumulates alternating
odd-denominator terms, comparing each new binary32 sum with the preceding sum.
It advances the power only after an unequal comparison and finally multiplies
the captured input by the preceding, converged sum. No tolerance, iteration
limit, sign simplification or replacement library algorithm is invented.

The source reuses the existing narrow EE square-root primitive. The added
RSQRT primitive contains a single authentic target instruction with explicit
numerator/radicand operands. Its native fallback supplies a finite arithmetic
model, using rounded square root then division; it does not establish the EE
instruction's precision, exceptional results, FCR effects or timing. No external
licensed implementation was identified for these engine algorithms; the public
newlib math implementations remain separate entries with different algorithms.

The author and an independent agent reviewed all 121 original instructions,
the complete source/header and operand ordering. The independent agent also
reproduced every complete linked comparison. A strict decoder reuses
`trace_geometry.py`, adding only less-than and the scoped finite RSQRT model.
It follows all recursive calls, ordinary and likely delay slots and stack
restores through the actual validated original instruction bodies.

The 1,552 synthetic finite fixtures cover both signs, signed zero, inverse
inputs, reciprocal and square-root reductions and adjacent binary32 threshold
inputs. They contain only synthetic inputs and results. Their complete header
SHA-256 is `c5931d280b8102c4f7343d8df902f2b03cd1a724410acedcfc641987d91bc525`.
The native harness passes 3,106 checks, including exact finite-model fixture
bits and an independent host-libm numerical sanity check. Native `-ffloat-store`
prevents x87 excess precision from changing the modeled binary32 convergence.
Six decoder tests reject unreviewed code/padding, unsupported instructions,
step-limit violations and undefined native RSQRT stages, and check operand
aliases and strict comparisons. Native exceptional arithmetic and hardware
behavior remain outside this evidence.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness engine_angles`.
Regenerate fixtures only with the supported local executable:
`.venv/Scripts/python.exe tools/trace_engine_angles.py --golden-header tests/native/engine_angles_golden.h`.
