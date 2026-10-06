# Connected actor state updates and lifecycle

This batch recovers 20 complete functions covering 5,440 original executable
bytes. It includes the complete state 6, 12 and 13 updates, the state 13
initializer, related initialization and cleanup, four duration callbacks,
timer logic, scalar release logic and the companion request lookup. State and
function names remain numeric because the original class names are unresolved.
The large state 4/5 initializers and state 14 update are deferred in full.

Two complete 28-byte primary-request stop methods (`00194C08`, `00194D38`)
compile and individually link to identical retail bytes under the GCC 3.2.3
candidate profile. The other 18 methods remain reconstructed. All three
supported recipes compiled and completed actual individual links for every
method, yielding 60 comparisons with no remaining relocations. The GCC 2.9
SAVE128 duration callbacks reproduce every instruction except the four
save/restore stack-offset bytes: `s0` and `ra` slots are interchanged. The
manifest records this gap without claiming those callbacks are matched.

## Reused evidence and source

The source reuses the established partial actor/entity/vector storage, adjusted
virtual pairs, Deimos callback signature and explicit 64-bit software runtime
operands. New views assert only accessed storage. The actor control object at
field `0x20` has its table pointer at offset `0`; the vehicle at field `0x730`
uses offset `4`. Each virtual pair keeps its signed halfword adjustment and
function pointer at offset `4`.

The manifest references the already proven 42 complete 28-byte state records at
`0x003F83F0` in `config/functions/actor_movement.json`. Thirteen actual selected
members connect this batch to states 4, 5, 6, 12, 13 and 14. This is a reference
to prior data evidence and awards no additional recovered or matched data.
Each code interval independently checks its complete entry-through-endlabel
bytes and SHA-256, includes branch/return delay slots, and excludes alignment.

Four complete duration callbacks share one typed C template. Their observed
state/time offsets differ; the original bodies share the same operation
sequence. Both request-stop methods likewise reuse an independently reviewed
typed template. No no-op methods or instruction arrays are included.

| Functions | Observed behavior |
| --- | --- |
| `0017D908`, `0017F588` | Strict velocity/timer gates, callback timer reload and unconditional field clear |
| `0017EB20`, `0017EBF0` | Captured release arguments, fresh post-call data and full-width flag/scalar effects |
| `00182B98` | Complete reference motion, software-angle update and request/repeat/exit state machine |
| `00183380` | Complete state 12 timer, motion, repeat count and paired-request transitions |
| `00183730`, `001839D0` | Full hash-selected initialization, live status flags, matrix notifications and exit calls |
| `00194B70`, `00194C98`, `00194D60`, `00194DE0` | Callback duration scaling and wrapping fresh phase increments |
| `00194BB8`, `00194CE0`, `00194E28` | Control dispatch before phase/flag initialization |
| `00194C08`, `00194D38`, `00194DA8`, `00194EA8` | Request/object cleanup with exact null and reload behavior |
| `00195D88` | Captured signed count, full-word first-key match and absent sentinel |

## Behavior retained

The two request APIs take nine integer/pointer arguments. Their floating values
use the independent `f12`/`f13` bank. The main request receives captured input;
companion key, object and scale are reloaded after callbacks. The duration
callbacks consume source in `a2`, entity in `a1`, and multiply by the exact
binary32 factor `0x395A740E` before their two stores.

State 6 uses the original software conversion, comparison, subtraction,
multiplication and addition calls on complete 64-bit operands. The exact
constants are `0x400921FB60000000` and `0x401921FB60000000`, rather than native
double pi constants. Source captures and fresh scalar reads retain the original
call order. Phase/count increment and decrement use unsigned arithmetic before
signed interpretation so the observed 32-bit wrap remains defined.

State 12's positive repeat count is decremented; zero or negative values keep
repeating. A strict squared-motion comparison falls through on unordered
inputs. State 13 queries the live primary object three times, takes the original
null early exits, keeps a captured matrix object across copy callbacks, then
reloads notification objects. Its final phase increment follows all stop and
script callbacks. The state 5 timer method clears field `0x74` on every path,
including NaN, because the original store is an unconditional branch delay slot.

## Validation

Run the standalone 32-bit native harness without original data:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states.py
```

The harness passes 629 checks using independently expressed finite software
arithmetic and mutating callbacks. It covers strict and unordered timer gates,
duration phase wrap, request argument/float values, companion key/object/scale
reloads, scalar data capture across releases, preserved high flag bits, fresh
virtual tables after release, full signed phase transitions, count overflow,
status-driven null exits and the three soft-angle paths. Native finite double
arithmetic models callback values; it does not claim to emulate EE FCR or every
nonfinite runtime arithmetic bit pattern.

An independent complete-instruction review passed all 20 source bodies,
including the software-double sequence, callback ABI, numeric offsets and
load/store/call ordering. Independent metadata review also passed all 20 complete
original hashes, the 13 selected state-table members and literal identities.
All 60 fresh links reproduce the recorded comparisons without unresolved
relocations, and the independent native rerun passes all 629 checks. Candidate
profiles and full comparison results remain in the function manifest.
