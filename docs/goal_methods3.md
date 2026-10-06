# Connected goal state methods

This batch reconstructs 20 routines covering 7,556 original bytes. The four look
states, three intersection states, three road states, four vehicle-entry states
and six related helpers form one connected gameplay path. All 20 currently
remain **reconstructed**, with zero new byte matches. Each compiles and links
with the supported compiler profiles; differing bytes and sizes are recorded
in the function manifest. No original class/function names are asserted.

The selected methods are proven entries in mutable encoded member tables:

| Table address | Reviewed entries | Caller |
| --- | --- | --- |
| `003F8D50` | four look states | `001D9D10` |
| `003F8E38` | three intersection states | `001DF630` |
| `003F8E68` | three road states | `001DFDB8` |
| `003F8EE0` | four vehicle-entry states | `001E82D8` |

Each reviewed entry contains a zero this adjustment, negative direct-call
selector and function pointer. The manifest hashes those precise table
intervals. Adjacent float data and separate method groups are excluded.

Existing goal, owner, entity and virtual-pair layouts are reused. New structs
model observed geometry records and alternate float-vector views of the
previously recovered word-zeroed timers. Offset/stride assertions check every
new layout. Indexed vectors use observed byte indices and 32-bit address
arithmetic; their full allocation capacities are still unknown.

The movement helper preserves the original squared-distance behavior: one
path scales a vector by **1 / squared length**, while other paths invoke the
separate normalizer. It also keeps the complete 64-bit state mask, owner/entity
reloads after calls and scalar capture before x/z/y stores. Geometry copying
uses interleaved reads and writes, including the original behavior when source
and destination overlap. Route changes retain signed neighbor sentinels,
full signed-byte mode comparisons and wrapped byte indices. The route output
mode is a complete word, proven by the callee's stores; only its low byte is
used later by this caller.

Look and angle-blend methods pass complete software-double values through
integer registers. Their constants are the exact binary64 extensions of the
original float values, including pi `400921FB60000000`, two-pi
`401921FB60000000` and epsilon `3FB99999A0000000`. They retain repeated
conversions, comparisons and callback-dependent captures instead of replacing
the calls with host-library expressions. The manifest audits all 14 distinct
embedded binary32 constant words and records their exact rounded decimals.

The inline `george_ee_minimum` helper expresses the EE instruction's value
selection using signed encodings, with reversed ordering when both operands
are negative. It selects negative zero over positive zero and retains the
observed nonfinite encoding ordering. This behavior is corroborated by
[PCSX2's pinned `fp_min` and `MIN_S` implementation](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/FPU.cpp#L97),
commit `9fffbdbd59b962d63a2259b150f419ad3773e7b4`, file SHA-256
`204dcb290f9f56e9112622ab313ef12d2192eda5b43d24efcf2dbc8ca3fa7344`.
That implementation is behavior evidence for independently written code; it
is not imported into this project. The C value model does not represent FCR
cause flags. The unchanged legacy
[GCC machine description](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/config/mips/mips.md)
also establishes finite conditional-expression lowering to `min.s`; an IEEE
conditional by itself would not preserve every EE nonfinite encoding.

The native harness builds recovered C directly and requires no original files:

```powershell
.venv/Scripts/python.exe tests/native/run_goal_methods3.py
```

Its 106 passing checks cover shifted vector aliases, index narrowing and full
signed modes, route sentinels and flags, timer zero/negative/unordered branches,
callback owner mutation, look/angle wrapping, movement projection and scaling,
resource virtual calls, and signed quiet/signaling NaNs, zeros, infinities and
subnormals in the minimum value model. Host arithmetic/stubs support semantic
review; they do not independently establish every EE arithmetic/FPU effect or
replace complete target byte comparison. An independent full-instruction review
passed all 20 bodies and the four table hashes, including callback captures,
soft-double constants, route outputs, aliases and virtual-call sequencing.
