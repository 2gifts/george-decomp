# Actor states 30–33 and interaction records

This batch reconstructs 18 complete functions covering 7,472 original text
bytes. Twelve actual adjusted members in the existing 42-record actor table
identify the state entries. Six encoded callback materializations and 36
actual direct calls corroborate the duration callbacks and connected record
creator/remover. These references reuse existing table evidence without another
data award. Function and partial-layout names remain numerical.

| State | Initializer | Update | Cleanup | Duration |
| --- | --- | --- | --- | --- |
| 30 | `001962E0` | `0018C0C8` | `00196368` | `00196280` |
| 31 | `00196748` | `0018CE18` | `0018D648` | `00196700` |
| 32 | `00196468` | `00196500` | `00196608` | `00196420` |
| 33 | `0018C708` | `0018C8A0` | `001966C0` | `00196680` |

State30's complete 1,596-byte update retains both returned vector pointers
before reading the first vector's contents. A later virtual call can change
that first vector. The code keeps the timer/elapsed capture order, normalization
and scaling, fresh actor/data/angle reads, the complete two-vector bounds
snapshot, and the signed counter/limit comparison after a callback. State33's
complete 1,396-byte update preserves its signed halfword phases, strict negative
timer gate, request fallbacks and the distinct angle operand-negation rules.

State31's complete 2,092-byte update has animation-request, repeated sampling,
matrix/basis and motion-configuration branches. Phase500 samples `00141590`
up to three times: the first result gates nonnegative input, the second gates
the encoded `0x3F7D70A4` maximum, and the third result is stored without another
clamp. Those calls reload the object each time. It constructs a complete local
basis and translation through the reviewed control-table+`0x100` virtual pair.
The basis captures its orientation before later normalization callbacks while
the cross product reads the orientation again afterward.

Phase700 similarly chooses its subtraction branch from the first sample but
uses a fresh second result. Point/direction outputs remain live across reset,
control, normalization and angle callbacks. The squared-length computation
uses the existing real EE `SQRT.S` value primitive, preserving its target
operand encoding. The original unused projection is expressed in C; compilers
can remove that pure arithmetic. General FCR side effects are outside this
project's current C value model. The original unconditional primary-object
stores retain their valid-object precondition. A store can alias and clear a
later request pointer; the reconstruction preserves the later null gate.

The interaction creator `0018E5A8` captures input bounds before allocation,
publishes the record before allocating its 56-byte callback object, reloads
the record before publishing callback fields, and reloads it again for final
registration. The remover `00196980` requires both initial pointers, reloads
the record between removal and release, captures the callback before clearing
the record field, invokes its signed-adjusted word3 method, and then clears the
callback field. Cleanup methods preserve each stop/free/release callback's
fresh versus captured pointer order.

The duration methods reuse source only where complete original behavior agrees.
State31 increments its unsigned halfword before the duration callback;
states30/32 evaluate duration before incrementing a fresh wrapped32 phase.
State33 stores only its timer. State30 computes the reciprocal before storing
its phase and timer. All use the proven binary32 factor `0x395A740E`.

Run the asset-free 32-bit harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states6.py
```

It passes 964 checks covering all 18 functions, signed phase/timer boundaries,
unordered comparison gates, wrapping duration callbacks, retained query/vector
mutations, repeated angular sampling, frame capture and signed virtual
adjustments, request/record cleanup reloads, sparse storage and output/data
aliases. Mocked engine calls and host finite arithmetic do not certify EE FCR
behavior or every nonfinite arithmetic encoding.

The author and research agent independently reviewed all 18 complete original
bodies against the stable source/header and found no semantic defects. Parent
reviewed the complete C/header/native harness and critical original state31
paths. Research and parent separately audited all full hashes/terminal delays,
12 actual members, 36 real calls, six callback pairs, literal/binding identities,
and reproduced all 54 genuinely linked comparisons and 964 native checks.
All comparisons have zero unresolved references and none are byte matched.
Canonical metadata is `config/functions/actor_states6.json` with numeric
bindings in `config/symbols/actor_states6.json`; all 18 remain reconstructed.
