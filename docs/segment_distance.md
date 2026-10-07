`segment_distance.c` recovers the complete 1,932-byte routine at `0029D5D0`.
It returns squared distance between two segments and optionally publishes each
closest fraction. Its six GPR arguments are two origins, two supplied
displacement vectors and two nullable fraction pointers. The original uses
displacements verbatim and adds no normalization or degeneracy guard.

The implementation reuses the full nine-region and parallel branch algorithm
from [Coin3D's pinned `SbTri3f::sqrDistance`](https://github.com/coin3d/coin/blob/da9c1330c618cff65598b97e881bb896d9ac84ad/src/collision/SbTri3f.cpp#L898).
Commit `da9c1330c618cff65598b97e881bb896d9ac84ad`, complete source SHA256
`def1ed2bdb02bb39e6c481b633697956521debe2b19963bbf575df9af787905a`,
contains the compatible region formulas and `1e-6f` tolerance. The complete
BSD-3-Clause copyright, conditions and disclaimer are retained atop the adapted
C and in `LICENSES/coin3d-segment-distance.txt`. The correspondence establishes
a reusable public algorithm; it does not identify the game's original SDK,
source file or compiler. No separate Eberly authorship/license assertion is
needed to reuse this concretely licensed Coin file.

The adaptation changes upstream endpoint arguments to the observed
origin/displacement ABI, expands vector operations into the original XYZ scalar
grouping and expresses repeated additions in their original order. Its ordinary
C region flow is retained. The upstream `fabs` calls are replaced by the exact
observed `fptodp`, `dpcmp`, conditional `dpsub`, `dptofp` sequence. This comparison
leaves negative zero unchanged, unlike a general sign-clearing absolute helper.
The actual threshold word is `358637BD`. Optional output zero is stored before
output one, and both stores precede the final soft absolute sequence. All values
needed for the result have been captured before either output store, so outputs
may alias coordinates or each other; a shared output holds the second fraction.

The existing `GeorgeMathVec3` layout and four already source-identified runtime
bindings are reused without modifying their source or registration. The author
read all four complete soft ABI entry bodies and their relevant unchanged
`fp_bit.c` conversion, compare and subtract paths. Their 64-bit bit-pattern
arguments/return occupy individual EE GPRs; the compare result is signed32.
Both pinned compilers independently produce the six-word layout identity
`[4,4,8,12,4,8]` for float, signed32, soft bits, vector size and Y/Z offsets.
These dependencies receive no new recovery or byte credit.

All 483 original instructions were manually reviewed. The isolated packet
checks every raw byte against the complete assembly, the terminal `JR31` and
stack-adjust delay, four excluded zero alignment bytes, disjoint neighboring
entries, all 71 local branches, 17 stores including stack/delay stores, eight
actual helper calls and eleven actual float materializations. Seven decoded
incoming `JAL`s have complete containing-body geometry and hash proof. The
author manually read the entire 656-byte `270A00` caller and the recorded bounded
contexts in the much larger `1C8BE0` and `1E2EC8` callers. Those latter complete
caller identities are verified without claiming full manual caller analysis.
There is no switch table or mapped compiler data dependency in the selected body.

Three genuine complete links resolve all relocations. GCC3.2.3 produces 1,528
bytes with 1,567 whole-byte differences, GCC2.9 produces 1,776 bytes with 1,517
differences, and the established GCC2.9 save128 recipe produces 1,776 bytes with
1,515 differences. Each is compared against all 1,932 original bytes, including
all relocated calls and the terminal delay. No exact match is claimed. No
relocation mask, code patch, assembly fallback or partial-byte award is used.

The strict original-instruction tracer reads only the selected complete body
from the validated local ELF. Its 2,747 synthetic fixtures execute 411,154
original instructions, at most 168 per invocation. They reach all nine region
labels and the parallel/acute/obtuse paths, include arbitrary displacement
lengths, determinants on either side of the literal threshold, zero directions,
both signs of zero, null/shared outputs, shifted input overlap and coordinate
outputs. Deterministic uncorrelated vectors and coincident interior points test
secondary clamps and cancellation. The packet records actual branch outcomes
without claiming every possible outcome was exercised.

The separate-TU native harness executes the real adapted production C and passes
391,307 warning-free checks. Every fixture compares all 64 authored memory
words, the result bits, four soft-call counts and the actual consumed soft
arguments plus observed output memory at each call. Unused ABI lanes are not
invented as arguments. A second closed-form perpendicular-axis model verifies
distance and independently clipped fractions. Its zero fractions compare
numerically; exact signed-zero bits are tested against original instructions.
The native flags `-msse2 -mfpmath=sse` give binary32 scalar storage/operations.
Nine focused decoder guards test code/memory bounds, operand forms, the real
64-bit soft lane, signed compare branches, finite/normal domain, actual return,
instruction bound and ordinary/likely delay semantics. The shared encoding
predicate rejects control instructions in a delay before execution, even when
the encoded branch would be untaken.

The four lower calls are explicit finite normal/zero IEEE caller models of
already identified runtime algorithms. They do not prove EE nonfinite or
nonzero-subnormal behavior, FCR flags, rounding-mode changes, timing or arbitrary
hardware memory. The source retains the original operations and does not add
the tracer's domain guards. No part of the selected production body is stubbed.

Reproduce from the workspace root:

```powershell
.venv/Scripts/python.exe .local/segment_distance/scope.py
.venv/Scripts/python.exe .local/segment_distance/adapt.py
.venv/Scripts/python.exe .local/segment_distance/abi.py
.venv/Scripts/python.exe .local/segment_distance/probe.py
.venv/Scripts/python.exe tools/trace_segment_distance.py --output .local/segment_distance/trace.json --golden-header tests/native/segment_distance_golden.h
.venv/Scripts/python.exe tests/test_trace_segment_distance.py
.venv/Scripts/python.exe tests/native/run_segment_distance.py
.venv/Scripts/python.exe .local/segment_distance/finalize_draft.py
```

The author, independent peer and parent completed full selected/source/ABI/proof
review. Each independently reproduced the complete packet and native checks;
the upstream primary source and complete retained license were independently
checked. All three natural whole comparisons also reproduce using only central
bindings. Canonical verification, the retail-identical hybrid build and 379
reviewed tooling tests passed (one platform symlink skip). The function remains
reconstructed with zero exact bytes. The producer freeze is a historical
snapshot; this final publication note records the later checks.
