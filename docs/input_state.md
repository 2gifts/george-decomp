# Input frame state and key transitions

Eight complete routines recover 1,540 original instruction bytes. The 32-byte
state reset wrapper `002AC6C8` matches its complete original-address linked code;
the other seven remain reconstructed. Both pinned compilers compile and fully
link all eight with no unresolved relocations. Canonical recipes use GCC 3.2.3.

| Address | Observed operation | Original bytes | Canonical candidate bytes |
| --- | --- | ---: | ---: |
| `002AC1C0` | Initialize frame state | 332 | 264 |
| `002AC310` | Update axes, button edges and repeat state | 788 | 680 |
| `002AC628` | Allocate state with clamped mode | 108 | 100 |
| `002AC6B8` | Set lock timer and value | 12 | 12 |
| `002AC6C8` | Reset through stored mode/device | 32 | 32, exact |
| `002AC6E8` | Clear four edge/repeat words | 20 | 20 |
| `002AC700` | Allocate 256-key state | 84 | 84 |
| `002AC778` | Advance key transitions and selected repeat | 164 | 164 |

The existing reviewed `duplicate_helpers` destructors at `002AC698` and
`002AC758` are reused without duplicate source or progress registration. Heap
allocation and the authentic `memset` entry retain their original call targets.
No upstream identity is asserted for this custom input code. Pinned official
m2c drafts supplied control-flow context, followed by manual full-instruction
and caller review; no generated error placeholders or assembly fallback remain.

The state occupies 220 bytes. Initialization sets flags `1A`, repeat interval
and timer with float encodings `3E088889` and `3E888889`, and threshold
`3E23D70A`. It clears the observed history, pressure, axis, output and lock
fields. The released-button word at `28` and byte-pressure values at `8C` through
`8F` remain untouched. The allocator clamps signed modes outside 0 through 2 to
zero, but the initializer itself retains its unchecked caller contract.

Caller `00295170` independently confirms six integer-register inputs and five
scalar float inputs in `f12` through `f16` for the frame updater. Bit zero in
state flags disables the entire update, including timers and mode assignment.
Non-null axis and pressure inputs are copied scalar by scalar. Each axis stores
its previous current value before reading its new input, then stores the new
value and difference. Byte pressure and twelve float pressure values use forward
reads and stores. Shifted source/destination aliases therefore retain propagation
effects; these copies have not been replaced by `memcpy` or a captured-array copy.

The four previous output-axis values are captured before their history stores,
then the four independent float arguments become current outputs. Flag `8`
converts the first current axis at strict thresholds above `0.5` and below
`-0.5` into button masks `20000000` and `80000000`. Flag `10` converts the second
axis into `40000000` and `10000000`. Each request bit is consumed after that
frame; other flags are retained. Exact endpoints and unordered comparisons do
not trigger those masks. Button labels remain unassigned until the corresponding
device mapping is recovered.

The updater stores previous held buttons, new held buttons, pressed and released
edges, and initially copies pressed bits into repeat state. A six-slot ring
stores only fresh presses; their union forms recent press state. The stored
previous word is loaded again after pressure copies to select repeat behavior.
A changed button set resets the repeat timer to twice the interval. An unchanged
set subtracts elapsed time and repeats all currently held bits only when the
result is strictly negative, then resets to one interval. The lock timer also
subtracts elapsed and clamps only strictly negative results to zero. No new
elapsed-time or history-index guards are introduced.

The key state occupies 264 bytes: repeat timer, one selected marker byte,
256 key bytes and three untouched padding bytes. Construction clears only the
key bytes. Every update reduces each key byte to its held/prior low bits:
low encoding 0 maps to 0, 1 to 15, 2 to 0 and 3 to 3. This copies current held
state into the prior bit and marks rising edges as press/repeat. Other old bits
are discarded. Selected marker zero skips timer processing. For a nonzero
marker, a timer result less than or equal to zero sets repeat bit 8 in that
selected byte and resets the timer to float encoding `3D888889`. The selected
key need not be held; the original has no added hold guard.

The root reviewer independently checked all eight complete original bodies,
the caller ABI, interleaved alias effects, global/state reloads and the distinct
strict/nonpositive repeat thresholds. No defects were found.

`tests/native/input_state.c` passed 2,475 checks with zero failures using the
actual recovered source. Cases cover field preservation, allocation failure,
mode boundaries, full disabled no-op, scalar alias propagation, output history,
64 axis-threshold combinations, six-frame press expiry, timer endpoints,
all 256 key byte encodings and every nonzero selected key index. Synthetic
host IEEE special-value cases validate source branch shape, not independent EE
nonfinite behavior. No original bytes are embedded. Run
`.venv/Scripts/python.exe tests/native/run_utilities.py --harness input_state`.
