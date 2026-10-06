# Quaternion and rotation routines

Twelve complete recovered C functions cover 2,572 original instruction bytes.
All 36 individual comparisons under the pinned GCC 2.9, GCC 2.9 SAVE128 and
GCC 3.2.3 recipes link at their original addresses. None currently matches.
Function names retain numeric addresses; descriptive names below are behavioral
inferences from the instructions.

| Entry | Bytes | Observed operation |
| --- | ---: | --- |
| `002A2658` | 112 | Skew matrix |
| `002A26C8` | 184 | Quaternion multiplication |
| `002A2780` | 524 | Spherical quaternion blend |
| `002A2990` | 696 | Quaternion curve control |
| `002A2C48` | 164 | Quaternion-vector rotation |
| `002A2CF0` | 128 | Axis-angle conversion |
| `002A2D70` | 84 | Z-axis quaternion |
| `002A2DC8` | 84 | Y-axis quaternion |
| `002A2E20` | 84 | X-axis quaternion |
| `002A2E78` | 236 | Three-angle conversion |
| `002A2F68` | 116 | Linear quaternion blend |
| `002A2FE0` | 160 | Three spherical blends |

The recovered code reuses existing engine trig functions and the actual
quaternion multiplication and spherical blend calls. Curve control reuses the
reviewed component-log arithmetic of `002A3138` as a source helper, matching
retail's two inline blocks. Shared EE square-root and minimum primitives retain
the observed instruction operand behavior. No borrowed standard quaternion
formula replaces retail arithmetic.

## Preserved behavior

Skew-matrix writes interleave with new input loads. Several inputs must be
captured before intervening zero stores when the buffers overlap. Multiplication
captures all eight input components before its first output store. Linear and
spherical blends instead reload each component pair between output stores.

Spherical blending uses strict comparisons with tolerance bits `358637BD`.
Its normal branch clamps the dot product before calling the custom acos/sine
entries. The antipodal fallback copies a freshly loaded `left.z` into the final
`w` component after writing `z`; that also applies to an in-place buffer. Curve
control reloads current/next after the first angle callback and reloads current
again after the exponential callbacks before multiplication.

Axis builders preserve their different zero-store positions around sine calls.
The axis-angle routine captures its half-angle before callbacks, then reloads
axis components between output stores. The three-angle routine calls cosine,
sine, cosine, sine, cosine, sine and preserves the complete observed expression.
At three zero angles its output is `(0, 1, 0, 1)`. The last routine takes five
general-purpose-register arguments plus `f12`, and captures its final blend
weight before the first callback.

Both root and a separate agent reviewed every original instruction in all 12
bodies, including branches, delay slots, input captures and output order.
Trailing alignment words are excluded from function sizes.

## Verification

Run the 32-bit native harness:

```powershell
.venv\Scripts\python.exe tests/native/run_utilities.py --harness rotation
```

Its 2,530 checks cover identities, point rotation, shifted aliases, unusual
fallbacks, callback mutations, trig/store order and captured curve weights.
Seventy-four synthetic fixtures check all 32 initialized words, including
untouched memory, for the complete skew, multiplication and linear-blend bodies.
Their reference comes from a bounded instruction evaluator reading the local,
hash-validated original ELF. The evaluator reuses `trace_geometry.py`'s decoder
and exports only synthetic inputs/outputs, with no original instruction arrays.
An independent regeneration reproduced the complete golden-header SHA-256
`8797971b2073bf823a25f218b04d28af16f1eb95eb191771a1b77e53b09a10ae`.

Regenerate into an ignored file and compare it with the committed header:

```powershell
.venv\Scripts\python.exe tools/trace_rotation.py --golden-header build/rotation_golden.h
```

The reference models finite host IEEE arithmetic for these scoped fixtures.
It does not establish EE exceptional values, FCR behavior or timing. Controlled
native trig callees check argument and mutation contracts. Native success does
not award instruction matching. The close X-axis candidate still differs in
six bytes because of public compiler register-save slot/order choices; stack
padding, opcode patches and comparison masks were not used.
