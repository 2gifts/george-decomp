# Input-driven camera motion

Five connected routines reconstruct 1,344 original instruction bytes, all 336
instructions and every return delay slot. Two substantive scalar updates share
the existing input-state, matrix/vector types, EE maximum helper, vector
normalizer and heap allocator. Their constructor, mode setter and dispatcher
stay with the controller; the existing reviewed `002B5EF0` destructor is reused.
No new matrix engine implementation or original data is copied into this batch.

| Address | Behavior | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B59F8` | Clamped flag-selected update | 784 | 804 | 816 |
| `002B5D08` | Current-axis update | 416 | 416 | 416 |
| `002B5EA8` | Allocate controller | 72 | 68 | 68 |
| `002B5F10` | Set mode | 8 | 8 | 8 |
| `002B5F18` | Dispatch update | 64 | 60 | 32 |

All fifteen candidates naturally link at their original addresses without
remaining relocations. The complete eight-byte mode setter matches all three
standard recipes; the four other bodies remain reconstructed. Parent independent fresh links reproduce every comparison. The setter is
registered matched; the canonical verifier retains the complete-byte gate.
No switch tables, external block transfers or internal entry splits occur. All
local branch targets stay within the complete reviewed body, and excluded
following alignment words were checked zero.

The controller's established allocation is twelve bytes: mode, transform and
input pointer. Only the touched transform prefix through `+38` is typed; this
does not assert a complete transform class. Original `0029A100` allocates `0x68`
bytes for that separate object. Constructor consumers `00101678` and `00154CD8`
pass a transform and the existing input-state pointer, retain the allocated
controller return, and subsequently write the transform's position/rate fields.
Actual encoded direct JAL references are retained individually in metadata:
two constructor calls, one setter call, two dispatcher calls and the two
internal dispatcher-to-updater calls. Update consumers pass elapsed in `f12`
and do not use `v0` or `f0`; no scalar update result is invented.

Complete original `0029A308` proves the matrix-pair ABI. The third pointer is
passed to matrix construction and receives translation/homogeneous stores;
the second is the affine-inverse destination. Its fallback `00299D68` captures
the same roles, stores the complete forward matrix through its third argument,
then invokes inversion with the second destination. The first motion routine
places the inverse at local `+40` and forward at local zero, while the second
uses the reverse local placement. Source passes distinct full 64-byte matrices
in the proven parameter order. The complete 152-byte primary callee hash is
`2af62388b49f7c1f9487f8535965b4c490a997020189a1259deb52661e68414d`.
Its underlying matrix construction remains an original numeric binding.

Both updates capture the transform before the matrix call and reload the input
pointer afterward. The first clamps elapsed with the existing EE MAX value
model to the exact `3C888889` encoding. It tests held masks in this priority:
the complete `01400000` combination, `00800000`, `00400000`, then `01000000`.
These respectively select vertical translation, angular deltas, speed change,
or horizontal translation using normalized forward-matrix rows zero and two.
Other masks leave deltas zero. The two horizontal vectors have their Y values
cleared before normalization. After both normalizations, the input pointer is
reloaded again; values of the first local vector also remain live across the
second call. Translation uses the captured transform's speed times `0.12` and
the original signed axis-delta directions. Position scales by `elapsed * 5`;
angular/speed deltas double before multiplication by elapsed.

The first updater stores its intermediate yaw, then updates speed and pitch,
and finally wraps that retained yaw once. A negative value adds one full turn;
otherwise the negation of `angle <= turn` selects one subtraction, retaining
the original unordered branch behavior. It is not repeated normalization:
values still outside the interval after one adjustment remain outside. Exactly
one full turn remains unchanged. The second updater applies no elapsed clamp
or angle wrapping. It normalizes both full matrix rows, translates from the
two current axes at scale `0.1`, captures both negated/doubled angular inputs
before any transform writes, clears the transform word and forces speed `0.1`.
Both preserve scalar position store order, including capturing old Z before
the Y store. A single ordinary C inline helper shares these proven stores.

The constructor allocates exactly twelve bytes, captures its inputs across
allocation, and returns NULL without stores on failure. Success stores transform,
input and mode zero in that order. Only mode exactly one chooses the second
update; every other signed/unsigned mode word takes the first. Valid object and
input storage remain preconditions. No extra allocation, null guard, timer
normalization or angle-range clamp is introduced.

All eight float encodings are materialized in instructions, rather than loaded
from a readonly table. Metadata retains each original LUI/ORI/MTC1 site and the
SHA-256 of the full little-endian numeric word:

| Bits | SHA-256 |
| --- | --- |
| `00000000` | `df3f619804a92fdb4057192dc43dd748ea778adc52bc498ce80524c014b81119` |
| `3C888889` | `8b28d83cb5ddc0a51f24f23094cbfa145fee5d22bd205753291d42ccfef0ccc9` |
| `3F800000` | `e00e5eb9444182f352323374ef4e08ebcb784725fdd4fd612d7730540b3e0c8c` |
| `3DF5C28F` | `7632daccb6cf9fee26fe90bbbd5005d3268efa78578beffbc8b3092ac0e23437` |
| `3F400000` | `9a8208635e00348ab64aac2b759e76391fd47089e9a749bbcec770d9eb5c6421` |
| `40A00000` | `fca31f1667a6aa1bba12fca4e4ea1becd503379d80da3213af07f6cc5702828d` |
| `40C90FDB` | `12d85026b5109a3119231704608d5991a63d8d397d2867a9fc522305810ddb76` |
| `3DCCCCCD` | `fb360f7241a770dc32cade9a1d1273c92b13bd4959b1c130931312b7fe95e63f` |

The native 32-bit harness passes 13,122 checks. Its 144 synthetic fixtures trace
the original two complete update bodies with controlled matrix/normalization
calls, three matrices including zero rows, all seven selected priority masks
and six callback variants. These change the input or transform after the
matrix/second normalization, or alter the retained first vector during the
second normalization. Direct and dispatcher paths must preserve the same
captured transform, fresh input and intermediate data. Additional checks cover
allocation failures, sparse mode writes, every non-one dispatch mode, elapsed
and single-step angle boundaries, and transform prefixes overlapping input at
offsets `54`/`64`. Those overlaps preserve captured angular inputs before stores
overwrite input current/delta words. Untouched integer/raw words are exact;
finite float comparisons use tolerance `3e-6 * (1 + abs(expected))`.

Regenerate the synthetic fixture header explicitly with:

```powershell
.venv/Scripts/python.exe tools/trace_camera_motion.py --golden-header tests/native/camera_motion_golden.h
```

The 144 fixtures execute 15,570 original instructions, including delay slots,
at most 149 each. The input packet consists of each case's `variant`, `held`,
`mutation`, `elapsed`, `matrix` and `initial` keys, encoded as UTF-8 JSON with
sorted keys and separators comma/colon. Its SHA-256 is
`e72d875c7e31a89493d75473396aac2f698dfbece9680336c50eecb6d68b4365`.
The synthetic header SHA-256 is
`5fb336194622ef5b2a2c3c4eb40ffdfbc9c8a87ffe55191f7b38b6eb568ba0fd`.
No original instructions or asset arrays are exported.

The scoped tracer reuses the bounded geometry decoder without changing it.
Eight new guard tests cover signed-encoding MAX selection, comparison/branch
condition fields, reserved SPECIAL/unary/transfer operands, low-word masks,
unknown code/memory/calls, instruction bounds and EE SQRT's `ft` operand with
an aliased destination. Unsupported forms and nonfinite operations fail.
Matrix/normalization calls use explicit finite host models and synthetic
callback mutations; they do not execute the full original matrix engine or
establish that it performs those mutations. Normalization models the reviewed
scalar algorithm, without proving EE SQRT precision. The native build uses
`-ffloat-store`. Low-word integer addresses, finite host float operations and
initialized ordinary memory do not model EE exceptional arithmetic, FCR flags,
128-bit register behavior or hardware timing. The original MAX value helper's
encoding proof and these finite fixtures do not establish those omitted effects.

Parent independent review passed all 336 original instructions and the complete
C/header/native/scoped-tracer/strict-eight-guards/docs packet. All five full
hashes, terminal bounds/local branches, seven actual direct JALs, eight literal
word hashes/materialization sites and complete matrix-pair/caller ABI evidence
were independently audited. All fifteen fresh actual links reproduce the
recorded comparisons, including the full eight-byte setter match. The reviewer
independently regenerated all 144 fixtures byte-identically, verified both
header and input digests, and reran all 13,122 native checks with the documented
`-ffloat-store` build flag.
