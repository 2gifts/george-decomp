# Actor state4/state5 and vehicle update recovery

Six complete methods cover 10,912 bytes of original text. All remain reviewed
reconstructions: none reproduces the complete original compiled bytes yet.
Numeric function and field names retain uncertainty about original classes.

| Function | Original bytes | Observed behavior |
| --- | ---: | --- |
| `0017DD10` | 1,676 | State4 initializer: ballistic or scaled-motion impulse, animation selection, effects, flags and timers |
| `0017E3A0` | 1,920 | Full state4 update: timer/minimum tracking, optional secondary impulse, movement callbacks and predicate/map exits |
| `0017ED18` | 276 | Unusual local matrix, matrix multiply and mode-selected effect creation/release |
| `0017EE30` | 344 | Indexed effect gate, lazy registry setup and current-position emission |
| `0017EF88` | 1,532 | State5 initializer: impact thresholds, collection cleanup, effect selection and motion projection |
| `00183C58` | 5,164 | Full state14 vehicle update: signed phase machine, requests, status callbacks, position and angle transitions |

The source reuses established partial entity, virtual pair, vector and matrix
views. Actor control objects load their table from offset0; vehicle objects use
offset4. The state14 output calls receive a full signed32 command, a three-float
vector output and a separate float output. Member adjustments remain signed16.
Four lifecycle/update member references are tied to the already verified
42-record actor state table in `config/functions/actor_movement.json`. Direct
JAL references prove the two effect helpers and an additional state14 caller.
These references award no extra data bytes.

Original registry code occurs four times in the initializer/update pair. The
reconstruction reuses an independently expressed helper after reviewing the
actual upper-bound, comparator, insert and hash APIs. It captures range inputs
before record stores and reloads the end pointer after callbacks and stores.
The global effect object is captured before the hash callback. Repeated
animation requests likewise preserve the first request-presence gate, fresh
main object/context reloads after the companion call, and their separate time
arguments. GCC3's supported `always_inline` annotation keeps the two large
helpers inside the original functions; GCC2.9 already inlines them.

State14 retains all eight wait phases and every unknown signed phase's entry
fallback. Phase400 writes a new command immediately only when its animation
word pair is absent; successful request paths wait for the existing duration
callback. Interpolation loads source components before output stores, loads
angles afterward, and preserves the original XYZ or XZY store ordering.
The full software-double angle call sequence retains the exact64 pi/two-pi,
minus-one and five encodings, including the original branch decision when
callbacks later change the target scalar.

The matrix in `0017ED18` has ones at element indices2,4,9,15 and zeros elsewhere;
it is preserved as observed. SQRT uses the R5900 instruction's FT source.
The reused EE MIN/MAX helpers select values using signed binary32 encodings,
including negative zero and special bit patterns; they do not model FCR status.
Their cited primary evidence is the pinned
[PCSX2 FPU implementation](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/FPU.cpp#L97),
as documented in `docs/goal_methods3.md` and `include/george/ee_math.h`.

Run the asset-free native harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states2.py
```

It currently passes 1,712 checks. Cases cover signed phase/command boundaries,
request failures and callback mutations, retained output pointers, ballistic
and projection results, strict/unordered timer gates, registry insertion paths,
captured versus fresh pointers, negative collection counts, exact matrix and
float encodings, and finite soft-angle wrapping. Native double arithmetic is an
independent mock for the called software runtime, not a claim of complete EE
nonfinite arithmetic or FCR equivalence.

The author reviewed every original instruction and independently audited the
source again through a second agent, with no semantic defects found. All six
full extents include return delay slots and exclude alignment padding. All
18 actual individual links under GCC3.2.3, GCC2.9, and GCC2.9 SAVE128 complete
without remaining relocations. Their full comparisons are retained in the
manifest; zero exact matches are awarded. A separate independent audit reproduced
all 18 fresh compiler/link comparisons and all 1,712 native checks. It also
verified the six complete hashes and return boundaries, four actual state-table
members, five encoded direct calls, binary32 constants, and symbol bindings;
no defects were found.
