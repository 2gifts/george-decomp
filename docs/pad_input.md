# Pad polling, deadzones and rumble

Four complete routines reconstruct 2,420 original instruction bytes. All four
compile and fully link under both pinned compiler profiles with no unresolved
relocations. Their complete linked bytes differ from retail; none is marked
matched. Canonical recipes use GCC 3.2.3.

| Address | Observed operation | Original bytes | Canonical candidate bytes |
| --- | --- | ---: | ---: |
| `00295080` | Allocate frame/key states and initialize four pads | 236 | 232 |
| `00295170` | Poll four pads, normalize sticks and advance rumble | 1,648 | 1,828 |
| `00295B18` | Clamp and apply scalar deadzone | 184 | 200 |
| `00295BD0` | Radial stick transform with ordered outputs | 352 | 456 |

The batch reuses the recovered input-state allocation and frame-update entries,
the authentic runtime `memset` and `fmodf` entries, and the existing shared
square-root primitive. Pad SDK wrappers and the custom angle routine retain
their original addresses and observed ABIs. No SDK source identity is asserted
or imported. Official pinned m2c produced startup and scalar drafts; its two
other drafts could not handle the non-f0 R5900 square-root alias, so those bodies
were reconstructed manually from their complete instructions. The broad platform
startup at `002957E0` and already recovered destructor at `00295A78` are excluded.

Startup creates four mode-1 states, captures their four alias pointers before
allocating a fifth mode-2 state, clears that fifth state's conversion request
bits `18`, creates the key state, and initializes four 384-byte pad records.
It preserves the retail unchecked allocation contract. The new record layout
models only the connection-state word at `11C`; the SDK wrapper owns the rest.

Polling always calls the pad reader and frame updater for each slot. Only
connection state 99 enables the delta getter and input conversion. Kind 1 uses
buttons only, kind 2 adds both sticks, and kind 3 also converts twelve pressure
bytes. Unknown kinds and disconnected records yield the original zero frame.
Stick bytes and pressure bytes are interpreted as signed, including pressure
values 128 through 255; no unsigned correction is introduced. All sixteen raw
button bits map to the original game masks. Bits above the low sixteen are ignored.

Each stick computes length and radial deadzone scaling, using exact float
encodings `3C010204` for the reciprocal of 127 and `40C90FDB` for two-pi. The
scaled radius is clamped below by the EE maximum operation and the output
magnitude above by its minimum operation. Direction components use the radius
before the upper clamp, then clamp individually to [-1,1]. The angle is produced
through the original custom angle callee and `fmodf`. The standalone helper
stores magnitude before those calls, then angle, x and y; overlapping output
pointers retain this order. A zero length yields two zero components after the
angle calls. No new threshold-range guards are added.

The poller reloads the frame-state pointer and threshold between stick
calculations, then reloads the state after the frame updater. The mixed ABI uses
six integer arguments and five float arguments for that call. The rumble
accumulator at state offset `D4` is accessed through union value conversions to
preserve the shared neutral word layout and its untouched initialization state.
Flag 2 enables the rumble path. A zero lock timer resets the accumulator to 0.5;
otherwise the original `(value * 30) * elapsed` increment is retained. An
accumulator at least one enables motors `(1,96)`, then reloads that accumulator
after the motor callback before subtracting `30 * elapsed`. The captured state
pointer survives a callback changing the global pointer. Disabled rumble clears
the lock timer, resets the accumulator and sends `(0,0)`.

The shared `george_ee_minimum` and `george_ee_maximum` helpers express value
selection through signed encodings, reversing order when both operands are
negative. This preserves signed-zero and nonfinite encoding selection instead
of substituting IEEE library min/max. The independently written model is
corroborated by the public [PCSX2 `fp_min`/`fp_max` and `MIN_S`/`MAX_S`
implementation](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/FPU.cpp#L92),
pinned to commit `9fffbdbd59b962d63a2259b150f419ad3773e7b4`, whole-file SHA-256
`204dcb290f9f56e9112622ab313ef12d2192eda5b43d24efcf2dbc8ca3fa7344`.
That source supplies behavior evidence and is not copied into this project.
The helpers do not model FCR cause flags. The previously reviewed minimum body
was moved unchanged from `goal_methods3.h` into `ee_math.h`; its existing
106-check native gameplay harness still passes.

The root reviewer independently checked all four complete original bodies
through their epilogues, including startup captures, SDK mapping, radial
scale/magnitude separation, mixed-register frame arguments and the captured
state reload after the motor callback. No defects were found.

Square root uses the shared narrow `sqrt.s` primitive with floating-register
constraints, preserving the actual EE hardware operation without a new libm
call. Normalization, traversal, button mapping, callback ordering and rumble
remain ordinary C. Native builds substitute a host square-root model; no
instruction arrays or function-body assembly fallback are present.

`tests/native/pad_input.c` passes 610,739 checks, including callback argument
and sequence checks. It covers all 65,536 low-sixteen button patterns, scalar
deadzone endpoints, finite radial grids, all 256 four-output alias combinations,
signed pressure boundaries, connection/kind gates, startup pointer captures,
rumble boundaries and callbacks that replace frame pointers, thresholds and
accumulators. Shared encoding selection is checked over zeros, signed finite
values, subnormals, infinities and signed quiet NaNs. The pad, angle and fmod
callees are controlled substitutes: these checks support the recovered caller's
semantics and ABI, not the unrecovered angle implementation or all EE arithmetic,
exception flags and special-value behavior. No original bytes are embedded.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness pad_input`.
