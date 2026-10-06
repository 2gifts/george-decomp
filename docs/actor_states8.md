# Actor states 39–41 and connected controls

This batch recovers 17 complete functions covering 6,060 original text bytes.
Nine actual adjusted members in the existing 42-state table identify the last
three actor states. Fourteen encoded JALs and eight callback materializations
corroborate the connected controls and duration methods. Numerical names and
observed partial layouts remain in use. The existing actor table, request,
registry, matrix lookup, math and virtual-pair representations are reused.

| State | Initializer | Update | Cleanup | Duration |
| --- | --- | --- | --- | --- |
| 39 | `001A7708` | `001A7778` | `001A7810` | `001A7698` |
| 40 | `00180458` | `00180580` | `00194A00` | `001949B8` |
| 41 | `00196920` | `0018E0E8` | `00196930` | `001968D8` |

State39 retains its full64 flag operations and raw config comparison against
`0x45A78000`. Initializer data is captured before the flag store, while cleanup
reads flags after the optional control-field store. Its actual five-word table
at `0x0042FF10` selects return, blend and control-word paths. Connected
`001A7378` keeps full signed32 mode gates, the distinct request keys, live
float arguments and mode2's phase, refresh, control and vector reload order.
The duration callback captures its signed phase before storing the timer, but
copies a fresh phase after the optional `00185EA0` call.

The orientation helper `001A7600` stores X, zero Y and Z before reading fresh
config for the final Y store. That initial zero matters when the config aliases
the output vector; the native harness checks this. Its cosine result survives
the sine callback, and the config and blend reload afterward. Two actual
predicates used by the reviewed state38 cleanup retain full32 state equality
and the compound signed-byte phase check. Their complete bodies match under
each of the three tested compiler recipes; they remain candidate matches until
registration review finishes. The eight-byte neighboring setter was excluded.

State40's complete 2,972-byte update preserves its signed halfword phase capture
before the timer store and each request's primary capture before the request
field write. Phase1 reads the timer after the vector callback, retains the
lookup word across cleanup, and captures the effect handle before storing its
next phase. Phases100/200 retain the different existing-record and new-record
vertical bounds, motion normalization, fresh config and record pointers, and
phase200's `0.02` motion offsets. Phase400 retains its separate direction and
position vectors, including the scaled input at actor+`0x4C`.

Phase501 intentionally makes two separate guarded predicate calls. The first
can invoke a control method; the timer reload afterward can end the state.
The exit callback occurs before the second fresh flags/predicate check decides
between adjusted state3 and state0 dispatch. The harness mutates predicate
results and flags across those calls to distinguish these paths.

State41's complete 1,212-byte update has five actual table words at
`0x0042DAA0`. Missing lookup input sets the completion field but execution
continues. Full32 animation identifiers compare against unsigned record bytes,
and matrix lookup uses the source at actor+`0xF0`. The constructed object remains
captured across its copy while the state phase increments fresh after callbacks.
Timer expiry in that path clears the object pointer without an invented release.

Its ballistic path obtains three separate object results through the captured
component's table+`0x98`. The property callback occurs before fresh object/target
position reads. Horizontal length uses the existing real EE square-root value
helper. The vertical velocity uses the exact binary32 `-4.9` constant and mass
scaling from a later object result. Complete position and displacement snapshots
are built before the third object call. Preparation precedes a fresh embedded
table read at object+`0xA0`; the final signed-adjusted two-vector method reuses
the existing `GeorgeActorVirtualFrameParts` ABI. Later callbacks can change the
fresh state increment and timer inputs.

Run the asset-free native harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states8.py
```

It passes 1,341 checks across all 17 functions, signed and unsigned phase gates,
unordered timer/blend boundaries, wrapped duration callbacks, raw config/flag
bits, request/control/vector mutations, the orientation Y/config alias,
create/update bounds differences, phase501's distinct predicates, animation
identifier/mask/array reloads, changing component pointers/tables, ballistic
snapshots and callback-mutated phase increments. Host finite arithmetic and
mocked engine calls omit EE FCR effects and do not certify every nonfinite
arithmetic encoding. The runner suppresses Windows crash dialogs so a native
fault reports a reproducible failing process exit.

Author, research and parent each read all 17 complete original bodies against
stable C/header, explicitly checking lifecycle store inventories; no mandatory
source defects were found. Research and parent independently audited complete
hashes/boundaries, nine actual members, fourteen encoded JALs with complete
caller bodies, eight callback pairs, eleven binary32 literals, 112 local
branches, both actual tables and all numerical bindings. Each reproduced all
51 fresh genuine full linked comparisons with zero unresolved references and
reran all 1,341 native checks. The two complete called predicates reproduce
52 bytes; all 17 entries are registered as reconstructed until the canonical
verifier awards exact matches. The other fifteen candidates differ from retail.
Canonical metadata is `config/functions/actor_states8.json`, with numerical
bindings in `config/symbols/actor_states8.json`. No global verifier or build was
run by the batch author during registration.
