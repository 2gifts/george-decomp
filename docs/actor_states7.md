# Actor states 34–38

This batch recovers 20 complete functions covering 9,532 original text bytes.
Fifteen actual adjusted members in the existing 42-record actor table prove
the lifecycle and update entries. Nineteen encoded materializations prove the
five duration callbacks. No direct JAL references to these table/callback-only
entries were found. Numerical names and observed partial layouts are retained;
existing table evidence is reused without another data award.

| State | Initializer | Update | Cleanup | Duration |
| --- | --- | --- | --- | --- |
| 34 | `001943C8` | `0017CA18` | `001943F8` | `00194388` |
| 35 | `001947F0` | `0017FC10` | `00194800` | `001947A8` |
| 36 | `0019E580` | `0019DDB8` | `0019E598` | `0019E538` |
| 37 | `0019DBE8` | `0019D8E8` | `0019DBF8` | `0019DBA8` |
| 38 | `001A71E8` | `001A6630` | `001A7288` | `001A71A0` |

State34's complete 3,820-byte update keeps its signed halfword phase capture
after the orientation callback, occurrence-specific effect/handle reloads,
animation-matrix search, bounds construction, motion normalization and adjusted
member dispatch. Phase200 can issue two independent `0xBD` request pairs in the
same update: losing flag1 triggers its first transition, but execution continues
through the elapsed-time test and can trigger the second. The later guarded
predicate can still dispatch state3. Captured data/primary/record pointers stay
in the same order relative to stores and callbacks; cleanup reads later handles
fresh after earlier releases.

State35's complete 740-byte update uses full signed32 phase and count values,
captures each slot and full32 identifier, and compares that identifier against
an unsigned byte in each animation record. An identifier outside byte range
does not match by narrowing. Command3 maps to2 before selecting the array.
Array records, matrix arrays, copied objects, primary pointers and per-object
bit masks reload at the original call boundaries. Bit selectors use the low
five index bits. Absent matrices are passed to the original matrix helper with
its original callee contract; the C reconstruction does not invent a fallback.

States36/38 retain the six-entry table at `0x0042F4C0` and seven-entry table at
`0x0042FED0` as original control-flow evidence. The C uses explicit branches,
including signed-byte defaults. Local jump-table layout differences remain
visible in the compiler comparison. Motion components preserve their original
X/Z/Y capture order, followed by tenfold scaling. Timers are read after the
vector callback, so callback mutation affects the subsequent expiry test.

State38's complete 2,316-byte update preserves acceleration clamps, a single
angle wrap, target-crossing tests and deceleration. Its two shortest-angle
calculations have different current/target capture points around soft-double
calls; they remain distinct. The exact constants are
`0x400921FB60000000` (pi), `0x401921FB60000000` (two pi) and
`0xBFF0000000000000` (negative one). Deceleration captures velocity before
conversion calls and reloads the current angle after the sign comparison.
The orientation object and target angle are captured before the next-phase
byte store, then the object reloads for the following state call. The shared
signed-bit EE MIN/MAX value helpers are reused; host finite arithmetic tests
do not certify EE FCR side effects or all nonfinite arithmetic encodings.

Duration methods use the proven binary32 factor `0x395A740E`. States34/37 only
write their timers. State35 increments a wrapped32 phase before the duration
call. States36/38 call duration first, set their byte phase to1, then multiply
and write the timer. Sparse initializers preserve adjacent bytes. State36
cleanup clears `0x978`, `0x948`, `0x94C`, `0x96C`, `0x970` and `0x974` after
cancellation. A mistaken peer-review finding temporarily removed the last
store. Parent checked the original ELF word `0xAE000974` at `0x0019E5CC`,
and the author independently confirmed those bytes and restored the store.
Two regressions prove that `0x974` becomes zero for both an initial nonzero
value and a value changed by cancellation.

Run the asset-free native harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states7.py
```

It passes 2,266 checks across all 20 entry bodies, signed phase/command and timer
boundaries, NaN timer gates, wrapping duration callbacks, complete identifier
matching, mask toggles, matrix/output captures, callback-mutated cleanup fields,
state34's independent transitions, bounds and normalized motion, finite wrapped
soft-angle crossings and occurrence-specific current/object reloads. Engine
calls are mocked and host arithmetic has the value-model limits described above.

Author and research independently reviewed all 20 complete original bodies
against the C/header; parent caught and reversed the erroneous review finding
described above. Research freshly reproduced the restored six-store cleanup,
all 60 actual compiler links with zero unresolved relocations, and all 2,266
native checks. Exactly two full bodies match: `001943C8`, 48 bytes under
plain GCC2.9, and `0019DBF8`, 28 bytes under GCC3.2.3. Research also audited all
full hashes/boundaries/return delays, 15 actual members, 19 callback pairs and
every intervening integer-register write, both complete jump tables and their
13 targets, all 13 binary32 literals, six immediate chains proving the three
soft64 constants, and all numeric bindings. No further defects were found.
Parent reviewed the complete C/header/native/docs, the six cleanup stores,
complete state37 update and critical state38 capture/soft64 paths. Parent
independently audited all metadata, 241 local branches, both switch tables,
all 60 fresh full links and 2,266 native checks, then approved registration.
The other 18 functions remain reconstructed. Canonical metadata is
`config/functions/actor_states7.json` with numeric bindings in
`config/symbols/actor_states7.json`.
