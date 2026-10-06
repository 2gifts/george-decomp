# Rail-camera state and script callbacks

Fifteen centrally registered complete routines recover, 5,108 bytes and
1,277 instructions, around the retained `railcamera` configuration and script
command strings. It reuses the recovered pose-controller initializer,
camera-transform interface, vector normalization, Deimos value/node types, and
the reviewed EE minimum value helper. Function names and object fields remain
numeric; the strings establish command context, not a recovered original class
declaration.

| Address | Bytes | Observed behavior |
| --- | ---: | --- |
| `001670A8` | 300 | Base initialization, configuration, four point vectors and numeric fields |
| `001671D8` | 1,796 | Timed/configured point blending, offsets, soft-double threshold, camera updates |
| `001678E0` | 696 | Idempotent registration of fifteen native command variants |
| `00167B98` | 296 | Typed point-time offset, clamping and mode-selected output |
| `00167CC0` | 724 | Two-path point projection, optional offset and endpoint/interpolation branches |
| `00167F98` | 212 | Typed point-time evaluation with captured mode |
| `00168290` | 204 | Negative-height reset or strict middle-phase velocity adjustment |
| `00168398` | 84 | Typed forward-offset scalar setting |
| `001683F0` | 220 | Typed closest-point evaluation |
| `001684D0` | 40 | Nullable controller lookup and reset of field `120` |
| `001684F8` | 88 | Nullable typed scalar setting at `128` |
| `00168550` | 88 | Nullable typed scalar setting at `124` |
| `001685A8` | 132 | Typed path lookup and two scalar resets |
| `00168630` | 48 | Nullable controller lookup and field `15C` publication |
| `00168660` | 180 | Typed closest-time result in an eight-byte VM output slot |

The actual constructor caller allocates `0x170` bytes, passes a retained matrix
pointer, and stores the returned controller. A complete bounded 88-byte table
at `0042B680` contains the selected update and registrar methods; the constructor
publishes its actual address. The registrar has fifteen genuine calls to the
existing Deimos registration routine, eleven distinct materialized callbacks,
and thirty complete NUL-terminated command/name strings. Four callbacks are
reused under a second flag. Their saved register addresses survive earlier
registration calls, whose complete original body saves and restores the
corresponding registers. A second encoded entry call leads to `00168290` through
a genuine table member. Unreferenced neighboring constructors, no-ops, getters
and a redundant key predicate are excluded.

`001671D8` has an explicit reconstruction limitation. Its jitter branch
normalizes a vector at original caller-stack offsets `20/24/28`, then loads a
different, unwritten vector at `10/14/18`. The full selected body contains no
overlapping stores there, and the reviewed normalization leaf writes only its
provided vector. The soft wrappers store within their own frames and the random
wrapper updates a global seed. The source retains an **uninitialized volatile
local**, with no zeroing, replacement by the normalized vector, or host fixture
injection. This records indeterminate original stack input; it does not establish
defined portable C output or identical compiled stack placement. The function's
metadata carries this limit separately from its ordinary reconstructed status.

The source preserves two different blending paths, original ordered zero/one
clamps, the signed suppression decrement, target reads after source stores,
full soft-double operands and the exact promoted-binary32 threshold
`3FB99999A0000000`, strict phase gates, captured transform before gravity stores,
and independent transform reloads between outputs and callbacks. Script type
gates consume signed halfwords. The projection callback captures its second key
before the first lookup; mode flags occur before point queries in that callback
and after the final query in the other variants. Output payload/tag/subtype
stores retain their original order and eight-byte wrapped slot arithmetic.

The author read all fifteen selected originals and the supporting bodies listed
in the isolated ABI packet. That establishes the narrow call contracts used in
this source, including pointer/null returns, float returns and FPU arguments.
The constructor's input/return window was read; its complete caller bytes are
proved, without claiming a full semantic review of that caller. Supporting
functions receive no new recovery award. Lower engine path and matrix algorithms
remain external; authored native callbacks provide controlled values and mutation
events rather than asserting their full numerical equivalence.

The reproducible asset-free native command is:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_camera_state.py
```

It currently passes **3,625 checks** using the real recovered vector normalization
in a separate translation unit. The cases cover defined update branches,
remaining-time crossings, negative elapsed, signed suppression, global elapsed
selection, ordered NaN gates, exact full-double threshold arguments, callback
mutation, config/point/transform aliases, registrar idempotence and fresh globals,
every callback family and type failure, optional projection offset, nullable
lookup paths, and VM output sentinels/store widths. **The jitter branch is not
executed or numerically validated.** Native ordinary floating-point and controlled
soft-double fixtures do not model EE FCR flags or general engine nonfinite
arithmetic. The compile and test commands do not suppress uninitialized warnings;
the current native build produces no diagnostics.

All forty-five natural full compiler/link comparisons resolve under the three
established recipes. `001684D0` reproduces its entire 40-byte retail body under
all three recipes; `00168630` reproduces its entire 48-byte body under plain and
SAVE128 GCC 2.9. Canonical verification confirms these two independently reviewed complete matches, totaling 88 bytes. Their
central-only whole comparisons reproduced. The other thirteen complete
functions remain reconstructed. No hand-written function assembly, forced
layout, instruction mask, padding substitute, or fragment contributes a match.

The ignored proof packet is `build/actor_camera_state/manifest.json`, with
separate symbols, scope, source/native/script fingerprints, full raw/assembly
geometry, 88 local branches, 185 stores, real entry and registration evidence,
21 binary32 immediate sites for eleven constants, and the complete soft-double
chain. Author, independent research peer and parent read all selected original
instructions and complete production source, native harness and proof scripts.
Both independent reviewers reproduced the complete packets, all 45 whole links
and 3,625 warning-free defined-path checks. All fifteen are centrally registered.
Canonical verification and the retail hybrid build passed. The hybrid executable
retains retail SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`.
The checkpoint passed 307 tooling tests with one platform-specific symlink skip.
The hybrid equality includes remaining original assembly and does not award source credit.
