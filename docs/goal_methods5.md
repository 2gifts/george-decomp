# Reference, endpoint and route action methods

This batch recovers 21 complete routines covering 7,332 original bytes. It
connects reference-backed target construction and destruction, endpoint
resolution, five-candidate scoring, route selection and route-state dispatch.
All 21 remain **reconstructed**, with zero new byte matches. They compile and
individually link under GCC 3.2.3, GCC 2.9 and the supported GCC 2.9 SAVE128
recipe; their complete differences are recorded in the manifest. The angle
constructor's closest candidate still differs in save slots and restore order.

Existing goal, owner, road, motion and virtual member layouts are reused.
New numeric views describe only observed fields: a union target value, five
candidate vectors and scores, signed-halfword endpoint modes, and the route
action's timer/motion overlay. Compile-time assertions verify offsets and
sizes. Original class names and geometry allocation capacities remain unknown.

The manifest proves two state member pairs at `003F8EB0`, selected by the
route action's captured byte index. It also proves the six complete adjusted
virtual method pairs following the null prefix of each table at `004372F0`,
`004373F0`, `00437478` and `00437518`. Their exact byte intervals and hashes
are recorded; retained no-op and base methods are not counted as new routines.

The endpoint resolver retains its captured entity and target pointers while
reloading their contents after calls. Its planar branch accepts only a
strictly positive fraction no greater than one, leaving the output untouched
for other fractions. Its ray branch retains the observed normalized direction
and original length, copies the target with interleaved reads/stores, and
releases a fresh reference pointer after a successful intersection. Invalid
mode/null-target behavior follows the original caller contract.

Candidate generation uses a square-root distance margin followed by the
observed reciprocal **squared** vector scaling. Each candidate captures a
fresh owner/entity position before its stores. Scoring reloads callback outputs,
uses wrapping score arithmetic, and deliberately retains the two success
flags across all five candidates. Maximum-score ties use the original signed
random remainder; the random helper's nonnegative-result contract is retained.

Route methods preserve full route keys, signed sentinels and one-byte index
wraps. After a road lookup they reread the low halfword of the originally
selected neighbor field, even if the callback changes the direction byte.
Geometry calls reload the owner between calls while retaining the previously
captured target index. Member dispatch preserves the full 64-bit virtual pair
and a 32-bit sum of signed adjustments. The plane helper stores its cross
product before reloading geometry pointers and reading the anchor, including
when the anchor overlaps its output.

The exact binary32 constants are recorded in the manifest, including pi
`40490FDB`, two-pi `40C90FDB`, reciprocal-squared threshold `3727C5AC`,
random-mode factor `38D1B717`, plane bias `3DCCCCCD`, blend `3F400000` and
turn factor `42340000`. Shared EE minimum and narrow `SQRT.S` helpers are
reused from `ee_math.h`; complete function bodies contain no inline assembly
or original instruction arrays. The minimum value model's primary behavior
evidence is [PCSX2's pinned implementation](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/FPU.cpp#L97).
No external game or emulator implementation was imported for this batch.
Host arithmetic does not establish all EE nonfinite arithmetic or FCR effects.

The native harness builds recovered C with stubs and needs no original files:

```powershell
.venv/Scripts/python.exe tests/native/run_goal_methods5.py
```

Its 169 checks cover callback-dependent owner/reference/content mutations,
interleaved scalar aliases, byte reference-count wrap, signed-halfword modes,
untouched constructor state, plane anchor aliases, route sentinels, captured
indices, post-lookup neighbor mutation, full member-adjustment sums, strict
and unordered timer/fraction gates, motion state changes and persistent scoring
flags. These semantic checks supplement complete target byte comparison.

Independent instruction review passed all 21 bodies without finding a semantic
defect. Independent metadata review confirmed all original hashes and complete
boundaries, all 26 stored state/virtual member pairs, and fresh reproduction
of all 63 individual linked candidate comparisons. None passed the exact
retail-byte gate.
