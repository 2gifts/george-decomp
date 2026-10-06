# Actor states 20–24

This batch reconstructs 20 complete functions covering 9,212 original text
bytes. All names and layouts remain numerical because state-table membership
and field access do not establish the original class or source names.
No function has earned an exact byte match. The complete linked bodies from
three genuine compiler recipes have zero unresolved references, but differ
from the original executable.

| State | Initializer | Update | Cleanup | Request duration callback |
| --- | --- | --- | --- | --- |
| 20 | `001959C0` | `00187648` | Not recovered in this batch | `00195978` |
| 21 | `00187990` | `00187BA0` | `00195B08` | `00195A88` |
| 22 | `00187EA8` | `00188290` | `00195B78` | `00195B30` |
| 23 | `00188968` | `00188B10` | `00195C30` | `00195BE8` |
| 24 | `00195D08` | `00189310` | `00195D50` | `00195CC0` |

The remaining function, `00188590`, selects one of five numerical directions
for state 23. It calls the existing map-motion routine even on its stationary
path. The moving path captures the parent before a query, normalizes a
direction and a general cross product, then compares captured projections
through the existing 64-bit soft-float ABI. Equal projection magnitudes take
the side-direction branch.

Fourteen complete adjusted member descriptors from the already reviewed
42-record actor table identify the initializer/update/cleanup entries. The
actual call at `00188B40` independently identifies the selector. Thirteen
reviewed LUI/ADDIU pairs materialize request callbacks. These references are
entry and ABI evidence; the existing table is not awarded again as recovered
data. The full entry-to-endlabel geometry includes each terminal return delay
and excludes following alignment padding.

The implementation reuses the existing actor partial types, request API,
effect registry and upper-bound insertion behavior, vector/matrix APIs and
adjusted member dispatcher. Three duration callbacks share a complete C
template only after their entire original bodies prove the same behavior.
State 24 instead increments an unsigned halfword and reads it through signed
halfword phase gates. It must retain that narrowing and signed interpretation.

The large updates preserve repeated angle queries, fresh versus captured
object fields and all implemented phase branches. State 23 phase 200 falls
through to movement even when the request callback changes the phase. Its
wrapped-angle path captures fresh target/current values after a comparison,
then retains those values through later calls. State 24 captures elapsed time
and the associated object before passing the live float argument to the
sampling routine. Its phase 102 calls the same vector virtual method twice;
the second call re-reads the output local after intervening callbacks. Phases
201 and other unimplemented halfword values return after the common preamble.

The source keeps soft-float calls as explicit full-width integer operands and
results. PI and two-PI are `400921FB60000000` and `401921FB60000000`, with
separate positive/negative five and negative-one constants. Every embedded
float literal has an encoded binary32 entry in the proof packet. The optional
guarded `GEORGE_SAVE128` annotation uses the unmodified old compiler's genuine
register-precision attribute; it does not imply that scheduling, stack slots
or entire functions match retail.

Run the asset-free native harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states4.py
```

It currently passes 2,438 checks covering all 20 recovered functions. Tests
specify phase outcomes, timer boundaries, projection directions and ties,
wrapped finite angles, callback mutations during soft arithmetic, retained
output pointers, request/handle/route reloads, matrix translation aliasing,
effect range insertion, unsigned duration wrapping and virtual adjustments
whose full signed 32-bit sum exceeds a signed halfword. Host math mocks check
finite algorithm behavior and call order; they do not certify EE FCR behavior
or every nonfinite arithmetic bit pattern.

The author and research agent independently reviewed all 20 complete original
bodies against the C source/header, with no semantic defects found. The parent
reviewed the complete C/header/native packet and the full state23/state24
instruction paths. Research and parent separately verified all 20 full hashes,
terminal delay slots and local branches, 14 genuine table members, the selector
call, 13 callback materializations, every literal and numeric binding, and all
60 freshly compiled and fully linked comparisons. Both independently ran the
2,438-check native harness. Every function remains reconstructed; none earned
an exact byte match. The reviewed manifests are registered under
`config/functions/actor_states4.json` and `config/symbols/actor_states4.json`.
