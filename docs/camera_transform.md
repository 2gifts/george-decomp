# Camera transform utilities

Five complete routines reconstruct 828 original instruction bytes, all 207
instructions and every return delay slot. They reuse the frozen input-driven
motion prefix, reviewed vector/matrix types, existing heap allocator and affine
inverse `002A0E20`. Cached-transform, fallback, original VU-matrix and custom
trig callees retain their proven numeric bindings. The existing `0029A168`
destructor is reused. No shared source/header refactor, original data arrays or
replacement runtime implementation is introduced.

| Address | Behavior | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `0029A100` | Allocate and initialize transform | 104 | 108 | 104 |
| `0029A188` | Initialize scalar storage | 216 | 196 | 188 |
| `0029A308` | Produce forward/inverse matrices | 152 | 156 | 156 |
| `0029A3A0` | Produce projection matrix | 248 | 248 | 248 |
| `0029A498` | Translate both position groups | 108 | 100 | 100 |

All fifteen standard candidates naturally link at their original addresses
without remaining relocations. None is a complete code/size/hash match; all
five entries remain reconstructed. Metadata retains every full original body
hash, all thirty encoded actual direct JAL references, eight instruction-built
float encodings with full-word hashes and materialization sites, and complete
profile comparisons. All local branches stay within their reviewed bodies;
there are no jump tables or split entries. Excluded following padding was
checked zero. The unused original `v0`/`f0` values of void routines do not
establish a normalized result, and none is invented in the source.

The observed transform allocation is `0x68` bytes. A new union embeds the
existing 60-byte motion prefix unchanged and gives the same storage a scalar
view: position at `+04`, another position group at `+10`, four rotation floats
at `+1C`, and scalar fields through `+38`. The extended storage has four floats
at `+3C` through `+48`, words at `+4C`/`+50`, three floats at `+54`, and words
at `+60`/`+64`. Offset/size assertions establish these views without claiming
complete original class names or semantics for unused fields. The new matrix
pair declaration retains the original prefix-pointer ABI and adds the existing
candidate wide-save attribute only for this newly recovered definition.

Allocation preserves the incoming three scalar arguments across the heap call,
conditionally initializes the captured returned object, and returns that same
pointer regardless of the initializer's unused return registers. Failure returns
NULL without object stores. Initialization follows the original sparse order:
mode zero, X/Z/Y position stores, the second group zero, scalar defaults,
rotation zeros, then the original cache call. The callee sees every initialized
head field and the previously untouched tail. Only after it returns are the
seven trailing words cleared, preserving any head-field changes by the callee.
Defaults retain the exact original encodings rather than rounded substitutes:
speed 1000, `+3C` one, `+40` 4000, `+44` `420A1062`, and `+48` `3FAAAAA8`.

The matrix pair first invokes the original cache predicate on the captured
prefix. True calls the original rotation-matrix constructor using `+1C`, then
loads and stores the three `+10` translation components sequentially into the
forward matrix. Z is captured before writing the homogeneous one. Source
reloads each component after the matrix call, preserving output/source aliases
that the call changes. It then passes the second output pointer and complete
forward matrix to the reviewed affine inverse. False forwards the same captured
three arguments to the original fallback. The argument roles remain inverse
second, forward third; no extra matrix construction or return result is added.

Projection captures its degree-scaled angle and both `+3C`/`+40` values before
the two ordered custom trig calls. It retains the first result and same angle
across the second call, computes their ratio, and freshly loads aspect `+48`
afterward. All sixteen output words use the original store order. In particular,
the first three off-diagonal zeros precede the first coefficient, and the depth
terms retain the original `(-2 * far) * near` multiplication sequence. Output
overlapping the source must preserve those captures and fresh-load timing;
no near/far/aspect validity checks or hardware exceptional behavior are invented.

Translation captures all three deltas and the first `+10` group before its
X/Y/Z stores. It then reloads the deltas and captures the second `+04` group
before its X/Y/Z stores. A delta overlapping the first group therefore changes
what is read for the second group. Reusing the first captured delta for both
groups would lose this original behavior. Properly aligned valid scalar views,
complete storage/output capacities and original callee preconditions remain
caller obligations.

The native 32-bit harness passes 13,178 checks against 176 synthetic finite
controlled-call fixtures. It checks all 256 initialized buffer bytes as words
and seven per-callee call counts, constructor failure and initializer head/tail
ordering, callback changes, true/fallback matrix paths and overlapping output,
inverse and source regions. Projection callbacks change captured near/far/angle
or freshly read aspect. Twenty shifted delta aliases cover the position,
rotation and neighboring scalar views. Comparisons are exact for every word
except four projection coefficients, which use tolerance
`3e-6 * (1 + abs(expected))`; zero stores, tail flags and untouched memory remain
exact. The build uses `-ffloat-store` and `-fno-strict-aliasing` for the modeled
physical scalar views.

Regenerate synthetic fixtures explicitly with:

```powershell
.venv/Scripts/python.exe tools/trace_camera_transform.py --golden-header tests/native/camera_transform_golden.h
```

The fixtures execute 5,892 original instructions, including delay slots, at
most 80 per case. The input packet keys are `routine`, `mutation`, `rebuild`,
`output`, `inverse`, `delta`, `allocation_fail` and `initial`, encoded as UTF-8
JSON with sorted keys and separators comma/colon. Its SHA-256 is
`660f2ef3d132fda9d4afe8a18f387779ddfff1ea9f9abff639dec82b2d482575`.
The synthetic header SHA-256 with Windows CRLF line endings is
`ee6134ca6272b550f49e651befacab720b4dd5641af0b9ea6145fde3441c03f1`.
The same authored header with canonical LF line endings has SHA-256
`5740556c29bace8cc6ae4dbce8d344a4cbaa6378f7ff0f7f89b1d376f2bae470`.
Only authored input/output words are exported; no original instructions or
assets are included.

The scoped tracer inherits the reviewed `CameraTrace` decoder unchanged and
only overrides code boundaries and explicit controlled numeric callees. Five
new scope/callee guards reject padding, other helpers, invalid operand-pointer
roles, unknown memory/calls, wrong allocation size and changed captured angle.
These reuse the existing reserved-operand, finite-operation and instruction
budget checks. Heap/cache/rotation/inverse/fallback/trig substitutes exercise
observable call/alias behavior, without executing the full original engine or
asserting that its callees perform the synthetic mutations. Inversion's fixture
substitute deliberately tests pointer/argument propagation, not the actual
inverse algorithm. Low-word addresses and finite host operations do not prove
EE exceptional values, VU/SQRT precision, FCR flags, wide-register or cycle
behavior. Target complete-byte comparisons remain the independent match gate.

An independent agent reviewed all five complete original bodies and the source,
header, scoped tracer and native harness, finding no semantic defects. The
review independently checked all thirty encoded entry JALs, eight instruction
constant identities, terminal delays, excluded padding and local branches;
reproduced all fifteen natural linked comparisons with zero remaining
relocations and zero full matches; regenerated the 176 fixtures byte-for-byte;
and passed all 13,178 native checks and five new decoder guards. Captured versus
fresh projection operands, sparse callback-visible initialization, sequential
matrix aliases and the second translation delta reload received explicit review.
The parent independently reviewed all 207 original instructions and the complete
source, header, tracer, native checks, guards and metadata. Its fresh proof also
confirmed the hashes, boundaries, all thirty actual JAL references, eight float
materializations, fifteen links, both header line-ending hashes and 13,178 native
checks. Registration is approved; all five entries remain reconstructed.
