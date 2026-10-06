`curve_query.c` reconstructs the complete 284-byte `0029CF28` and 228-byte
`0029DD60` routines. The first returns squared distance from a point to a segment
and optionally writes its endpoint-clamped fraction. The second applies the
original query-radius test to a supplied ray. Both numeric names are retained;
the descriptive names explain observed behavior without asserting class names.

The distance routine captures all input coordinates before its optional output
store. The store can overlap any endpoint or point coordinate without changing
the retained residual used for the return. Strict positive dot selects the
right-hand calculation, and strict dot below squared segment length selects the
interior calculation. A degenerate segment follows the zero-fraction path without
division. The returned value is squared distance, without a square root.

The query routine first accepts strict containment, even when the supplied extent
is zero or negative. It then rejects a negative projected distance or a strictly
excessive perpendicular square. Tangency remains eligible, while the final entry
distance must be strictly below the extent. Radius is squared verbatim, and the
direction is used verbatim: the source adds no normalization or zero-direction
guard. The original extent load precedes the narrow target `SQRT.S` primitive.

The existing `GeorgeMathVec3`, `GeorgeMathVec4` and 28-byte `GeorgePathRay` layouts
are reused unchanged. The reviewed `ee_math.h` primitive emits the actual EE
square-root instruction, whose source is `ft`. It represents this single hardware
operation; traversal, comparisons and arithmetic remain ordinary C. No public
library algorithm identity has been inferred for the two complete bodies.

The isolated scope packet validates every original byte against its complete
disassembly, both terminal `JR31` instructions and stack-adjust delay slots, four
excluded zero alignment bytes per body and disjoint neighboring entries. Six
actual encoded incoming calls have complete containing-body identity evidence.
The author manually read complete callers `296560`, `2C23C8`, `2C2440` and
`1CD728`; only the bounded call context of the 4,580-byte `1C8BE0` was manually
read. Its entire raw/assembly identity is checked without claiming a complete
manual caller analysis. No aligned allocated non-executable entry pointer was
found, and no switch table or helper call occurs in either selected body.

All six natural whole-function links succeed under GCC3.2.3, GCC2.9 and the
established GCC2.9 precision recipe, with zero unresolved relocations and zero
exact matches. The distance candidates are 224/216/216 bytes with 220/226/226
whole-byte differences against 284 original bytes. Query candidates are
204/212/212 bytes with 181/175/175 differences against 228 original bytes. Both
remain reconstructed; no relocation masking, instruction replacement or partial
byte credit is used.

The strict scoped tracer reads the two actual bodies from the locally supplied,
hash-validated ELF and uses the existing finite scalar decoder. It exports only
synthetic inputs and outputs. Its 1,083 fixtures execute 57,815 original
instructions, at most 65 per invocation, including actual delay instructions.
They cover left/interior/right distance cases, degenerate segments, nullable
fraction pointers, coordinate output aliases, shifted overlapping vectors,
strict containment/tangent/endpoint boundaries, signed radii, negative/zero
extents and supplied nonunit directions. The whole 64-word authored memory
window and actual result are compared for each case. Nine focused decoder tests
reject unowned code, unknown/unaligned memory, reserved operand forms, calls,
invalid finite arithmetic and delay-slot/control-flow violations.

The separate-TU native harness executes the real recovered production C and
passes 78,724 warning-free checks. Axis-aligned closed-form distances, segment
reversal, degenerate segments and independently specified query boundary cases
supplement the original-instruction fixtures. It uses `-ffloat-store` to give the
native x87 test arithmetic binary32 storage points. These finite tests do not
prove EE nonfinite/denormal arithmetic, FCR state, timing or complete callers.

Reproduce from the workspace root:

```powershell
.venv/Scripts/python.exe .local/curve_query/scope.py
.venv/Scripts/python.exe .local/curve_query/probe.py
.venv/Scripts/python.exe tools/trace_curve_query.py --output .local/curve_query/trace.json --golden-header tests/native/curve_query_golden.h
.venv/Scripts/python.exe tests/test_trace_curve_query.py
.venv/Scripts/python.exe tests/native/run_curve_query.py
.venv/Scripts/python.exe .local/curve_query/finalize_draft.py
```

The producer freeze is retained as a review snapshot. Independent peer and parent review and canonical registration have passed; final documentation annotations follow that frozen research record.

The independent peer reviewed both complete original bodies, C/header, nine
decoder guards and the complete isolated proof packet, then reproduced all six
whole links, eight metadata dictionaries, 1,083 fixtures and 78,724 native checks.
The shared decoder's subsequent encoding-based delay-slot guard refinement is a
new tooling baseline: the author regenerated the fixtures and passed all nine
guards and 78,724 native checks again. Both golden-header bytes and fixture input
hash remain identical. The refreshed producer freeze records the changed shared
helper hashes; the earlier peer audit remains historical evidence.

The peer refreshed all 27 frozen inputs after the shared guard revision. Parent reproduction checked all 128 selected original instructions, full source/header/strict tracer/native/guard code, eight complete proof dictionaries and six complete natural linked functions. Both fixtures and native checks passed. Central-only binding reproduction, canonical verification and the retail-identical hybrid build passed; neither query function matches exactly. The checkpoint passed 364 reviewed tooling tests (one platform-specific symlink skip).
