# Actor states 25–29

This reviewed batch recovers 19 complete functions covering 8,764 original
text bytes. Fifteen genuine adjusted member descriptors from the existing
42-record actor state table identify the initializer/update/cleanup entries;
six actual callback materializations identify the duration callbacks. Names
and partial layouts remain numerical because this evidence does not establish
the original classes or source names. The existing state table and referenced
12-byte up-vector data are reused as evidence without another data award.

| State | Initializer | Update | Cleanup | Duration callback |
| --- | --- | --- | --- | --- |
| 25 | `00195ED8` | `00189E88` | `00195F10` | `00195E88` |
| 26 | `0018ACC8` | `0018AE08` | `00195FB0` | `00195F68` |
| 27 | `00196030` | `0018B528` | `00196050` | `00195FE8` |
| 28 | `001960E8` | `0018B7E8` | `00196120` | `00196088` |
| 29 | `0018BC28` | `0018BDA0` | `00196210` | None in this batch |

State 25's complete 3,648-byte update has eight implemented phases. It queries
a point and normal, uses a captured 12-byte up-vector snapshot, and keeps the
local query outputs alive across callbacks. Its phase250 path continues to
the second control callback after switching to state3. Phase301 compares the
remaining timer strictly against a fresh timestep and can switch through the
full adjusted state0 member; phase401 instead returns to phase200 when the
timer is at most zero. These paths retain their distinct position store orders
and captured/fresh request gates.

The point/normal API at `0013D118` writes two vector outputs. Callers load an
additional float into `f12`; the complete callee never consumes that incoming
float before overwriting it. The reconstructed call retains the observed
argument without assigning it a radius interpretation. The existing
`003936A0` call is `memset`, with its reviewed destination/value/size ABI.

State 26's complete 1,824-byte update has distinct missing-object, failed
collision, active-input and inactive-input paths. The missing-object path
preserves input fields; failed collision clears them and the object. Active
input uses the existing full64 soft-float calls and the exact encoded threshold
`0x3FB99999A0000000`, then reads the vector and weight after a construction
callback. Direction callbacks may change the local direction index before its
later use. The reversal path saves a normalized opposite vector, zeros the
current vector and sets bit38; some successful active paths intentionally
retain that bit. Inactive paths clear it after request callbacks.

The duration callbacks share source only where their complete original
bodies agree. State25 increments its wrapped32 phase before the duration
callback and stores timer870 before86C. States26/27 evaluate duration first,
then increment a fresh phase. State28 captures fresh data before storing its
timer, reads the scalar afterward and stores phase200 before dividing. These
differences are preserved even when data fields alias output fields.

All 57 whole-body comparisons use genuine GCC3.2.3, GCC2.9 and the supported
GCC2.9 register-precision profile. They have zero unresolved references. The
52-byte state25 initializer reproduces all retail bytes under the plain GCC2.9
recipe and is registered as byte matched after independent reproduction. The
other 18 routines remain reconstructed. No comparison masks
instructions, changes generated code, inserts padding, or substitutes assembly.

Run the asset-free native harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states5.py
```

It passes 1,692 checks across all 19 functions, including signed phase
boundaries, timer and float threshold gates, wrapped callback increments,
retained point/weight/direction mutations, full bit38 flags, request/control
reloads, sparse state stores, registry insertion, output/data aliases, and a
virtual adjustment whose full signed32 sum exceeds a signed halfword. Host
finite arithmetic and callee mocks do not certify EE FCR behavior or every
nonfinite arithmetic encoding.

The author and research agent independently reviewed every complete original
body against the stable C/header, finding no semantic defects. Parent reviewed
the complete source/header/native harness, critical original paths, all full
boundaries/hashes, 15 actual members, six callback materializations, and every
literal/binding. Research and parent independently recompiled and genuinely
linked all 57 comparisons and ran all 1,692 native checks. The canonical manifests
are `config/functions/actor_states5.json` and `config/symbols/actor_states5.json`;
only the 52-byte initializer earns an exact-byte award.
