# Actor state and movement methods

This batch reconstructs 20 complete connected game functions covering 8,540
original executable bytes. The methods service state transitions, jump and
vehicle requests, collection updates, model positioning and movement vectors.
Address names and numeric offsets remain intentional: original class and state
names have not been established. No function in this batch is byte matched yet.

`src/game/actor_movement.c` reuses the observed goal/entity/vector layouts,
adjusted virtual pairs, the established vector normalizer and explicit
64-bit runtime operands. Additional storage is accessed by numeric byte offset;
the existing `GeorgeGoalEntity` view is not asserted to be the entire actor
allocation. In particular, full word accesses at `0x738` coexist with earlier
observed byte accesses. The 20-byte collision record only models its first
observed word and leaves the other 16 bytes opaque.

## Original body and dispatch evidence

Every selected interval covers the complete generated `glabel` through
`endlabel`, including all branch and return delay slots. Original bytes and
SHA-256 are checked against the executable; trailing alignment zeros are
excluded. The function manifest records all 42 complete 28-byte state records
at `0x003F83F0`, including their three adjusted member encodings and unconsumed
final word. These actual records use direct or absent members; the C also
preserves the original positive-selector virtual binding path.

Member binding captures the selector, function and complete virtual pair
before loading the current actor state for its adjustment. The two signed
halfword adjustments are added at full signed 32-bit width. Teardown loads
the state again after the callback, then stores state `-1` and the captured
previous state in that order. Fixed state initializers use the same reviewed
binding mechanism without conflating the binding index and adjustment index.

The complete 27-, 14- and 42-entry original branch tables at `0x0042C480`,
`0x0042C4F0` and `0x0042C530` are recorded with full bytes, hashes and internal
targets. Source branch groups implement their complete entries and unsigned
defaults. Generated compiler-local tables are not claimed to occupy the retail
table addresses.

| Functions | Observed behavior |
| --- | --- |
| `00170538` | State teardown, current adjustment and post-callback previous-state capture |
| `00173648`, `00173720`, `00173818` | State gates and complete one- or two-stage transitions |
| `001739A0`, `00173B20` | Horizontal length/vector equality gates and full 64-bit flag transition |
| `00174770`, `00177160` | Callback-dependent direction/model positioning and matrix copies |
| `001765D0` | Three collection update families with captured bounds and fresh per-item pointers |
| `00176E10`, `00176F58`, `00177270` | Captured vehicle transfer, state-based jump preparation and live target copies |
| `001773F0`, `001778C8` | Signed byte predicates/configuration with full command word storage |
| `001775A8` | Vehicle direction/soft-double gates and two-pass collision-based command selection |
| `001779F8`, `00177B80`, `00177C40` | Reference setup, default state transition and seven-integer idle request |
| `00177E48`, `00179070` | Full movement dispatch, projection/scaling/flags and direction classification |

## Difficult paths retained

The caller at `0x001903E8` supplies a live `f12` argument to `00174770`.
That method forwards it unchanged into `00192078`, whose complete body
consumes it. The idle request `00177C40` has seven integer arguments in
`$4..$10` plus its independent `f12` argument. Its six unconditional rejecting
states remain rejecting even with a nonzero override; only state 12 uses that
seventh integer as an override.

The movement method retains both projection paths, the strict signed `0.3`
clamp, the complete three-component length in state 31, ordered weighted
products and data/state/flag reloads after normalization and mask callbacks.
Scaled, direct and reference copies preserve interleaved source loads and
destination stores, including when the input overlaps the actor's output.
The full 64-bit flag mask preserves the upper bits. Direction classification
retains the zero-times-Y term in its first dot product and the unordered
fallthrough in its final comparison.

The vehicle collision path reads both returned vectors only after its second
virtual call. Soft-double comparisons and subtraction use complete 64-bit
operands; the threshold `0x3FEE666660000000` is the exact original shifted
immediate. Record identities are read after the virtual identity callback,
and a mismatching record sets a persistent flag rather than ending the loop.
The second pass reuses the captured collision mode and start vector while
reloading the vehicle and height. No new count/capacity clamp is introduced.

The official m2c draft omitted projection writes without producing an error,
so it served only as a navigation aid. Every accepted body was checked against
the complete original instructions. R5900 square roots use the existing
`george_ee_square_root` platform primitive: their source operand is `ft`, not
standard MIPS `fs`. Host tests model finite arithmetic and unordered branches;
they do not model EE FCR effects or all nonfinite arithmetic encodings.

## Validation and byte matching

Run the native callback/alias harness without original game files:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_movement.py
```

The current runner passes **752 checks**, including all three state-table
branch families, full-width adjusted member sums, callbacks that change the
actor/vehicle/data/array pointers, live float arguments, post-callback reads,
strict equal threshold boundaries, unordered comparisons and shifted
overlapping vector/reference copies. Tests use an independent finite host
model for existing engine/runtime calls rather than copied retail code.

All 60 individual actual links succeeded under GCC 3.2.3, GCC 2.9 and the
guarded GCC 2.9 128-bit-save recipe. None passed complete size/bytes/hash
equality. The manifest keeps each function `reconstructed` and records all
three concrete comparisons and the compiler/source-shape gap. No instruction
bytes are masked or patched to award a match.

An independent reviewer checked all 20 complete original bodies, their 8,540
bytes and terminal delay slots, every one of the 42 state records and 126
member descriptors, and all three actual jump-table groups. All 60 freshly
compiled and individually linked comparisons reproduced the recorded results
with zero remaining relocations. The reviewer also independently reran the
expanded native harness and confirmed all 752 checks. No source defect was
found; the linked compiler differences remain recorded as differences.
