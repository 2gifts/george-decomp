# Vector and quaternion operations

Seven connected routines recover 1,412 original bytes. The complete 120-byte
hemisphere adjustment at `002A32E0` matches the pinned GNU 2.9 compiler. The
remaining six compile and fully link at their original addresses, but differ
and remain reconstructed. Function padding is excluded throughout.

The source reuses existing vector layouts and the original engine's numeric
trigonometric entry points. Those callees contain lookup tables and approximate
arithmetic; replacing them with standard library calls would change behavior.
The componentwise negative-left multiplication at `002A3138` is retained as
observed. It does not substitute a quaternion product based on inferred names.

The spherical blend clamps the dot product through ordered comparisons, calls
the original acos routine, and copies the current vector when the angle is
exactly zero or the original single-precision pi. The fallback copies components
one at a time. The general path reloads both vectors after all trigonometric
calls, computes all output components, then stores z, x and y. The normalizer
returns its original length and supplies `(1,0,0)` for zero length. The scaled
normalizer reloads y and z after earlier stores, so a shifted output alias can
affect later inputs. Tests cover these distinct alias behaviors.

`george_ee_square_root` is a scoped hardware primitive with float input/output
constraints. R5900 SQRT.S consumes the **ft** field, while standard MIPS uses fs.
The authentic pinned [GNU EE opcode table](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/opcodes/mips-opc.c)
specifies the R5900 encoding; all five retail square-root words were independently
decoded against it. This resolves generic disassemblers' `c1` placeholders,
including the nonzero ft at `002A35EC`.

The legacy [GNU builtin expansion](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/expr.c)
adds a conditional errno/library path unless global fast-math is enabled. The
primitive expresses retail's direct hardware operation without enabling broader
arithmetic transformations. It contains no fixed opcodes, scheduling patches
or function-body assembly. The native fallback uses host sqrt for algorithm
checks; it does not reproduce all EE arithmetic or FCR effects.

Independent review covered all seven complete original bodies and reproduced
all 21 comparisons across three explicit compiler recipes. The tracked native
harness passes 77 checks for zero vectors, scale signs, exact thresholds, dot
clamps, antipodal fallback, callback mutation, component captures and shifted
aliases:

```powershell
.venv/Scripts/python.exe tests/native/run_utilities.py --harness vector_math
```
