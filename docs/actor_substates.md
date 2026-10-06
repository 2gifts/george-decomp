# Connected actor substate methods

This isolated batch reconstructs eleven complete entries containing 7,008
original text bytes. The dispatcher observes actor+`0x2D8`, separately from the
already reviewed actor+`0x0C` state table. Numeric names and opaque field offsets
preserve the evidence without asserting original classes or source names.

| Entries | Observed behavior |
| --- | --- |
| `1726C8` | Seven-phase movement, animation requests and effect scaling |
| `172C08` | Physical or registry-backed bounds toggling and updates |
| `1732F0` | Component publication, bounds creation and ready-list progression |
| `174868` | Timed request sequencing and attached-object frame updates |
| `175AF0` | Configuration, owned-record replacement and bounds creation |
| `181120` | Paired requests, full-identifier selection and attached-object updates |
| `190C18`, `190C70` | Destructor and configuration installation |
| `191000`, `1910C8`, `191180` | Complete substate dispatch and cleanup |

Fourteen encoded incoming JALs and their complete containing caller bodies
prove ten entries. The destructor has a real zero-adjustment member pair at
`0042E7B0`, inside the bounded 56-byte region at `0042E7A8`. Its own original
instructions materialize that table and publish it at object+4. This proves
the entry and observed table binding without claiming a complete class layout.
Nearby array methods lacking proven callers or stored pairs are omitted.

The original eight-word table at `0042E7E0` dispatches every substate from zero
through seven. States zero and three return; state two always publishes its
timer subtraction before the ordered clearing gate. States one, four and five
forward incoming f12. States six and seven read actor+`0x35C` instead. The
seven-word phase table at `0042C340` independently proves the movement method's
branch groups. Nine callback materializations retain every intervening GPR
write; 178 local branch targets stay inside complete function bounds. The two
ordinary state transitions reuse actual member descriptors 18 and 22 from the
already reviewed 42-record state table. No readonly data receives a new award.

The methods preserve captured pointers and values across callbacks, then reload
each later operand where the original does. This includes matrix contents,
effect globals, primary and companion objects, configuration, collection
counts, linked successors and signed member adjustments. The selection loop
compares an unsigned record byte with a complete 32-bit identifier. Its mask
operation uses a 32-bit shift with the low five index bits before narrowing to
the signed low halfword. Null transform input on a miss and nullable bounds
targets remain explicit existing engine contracts.

The physical constructor consumes seven integer arguments in GPR4 through10
and five float arguments in f12 through16, corroborated by the complete engine
callee. The paired request uses the previously reviewed eight-register integer
ABI. The control's B0 method is described only as a signed-adjusted pointer
output call; no identity for its object or output layout is invented.

Vector publication retains load/store order even when outputs overlap the
actor. Independent review found that `1732F0` initially loaded actor+`0x128`
after storing record+`0x4C`. Original instructions at `00173338` through
`00173340` capture Z before the W store. The source now captures Z in that
position. The regression uses record=actor+`0xDC`, making W overwrite the Z
source, and separately retains record=actor+`0xE4` to test earlier interleaved
loads. Both cases preserve the original result.

Run the asset-free 32-bit checks with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_substates.py
```

The current harness passes 851 checks across all eleven bodies. It exercises
complete dispatch and phase groups, callback-mutated captures and reloads,
strict and unordered timer gates, shifted vector aliases, all 32 shift indices,
full-width member adjustments, linked successor mutation, registry insertion
alternatives and low-halfword reference-count wrapping. Engine calls are
mocked. Host arithmetic does not model EE FCR effects or prove general
nonfinite instruction behavior. EE minimum uses the existing reviewed raw-bit
helper; required bounded storage and nonnull engine inputs remain contracts.
The six unused scratch bounds in `174868` are ordinary optimizer-removable C,
reflecting original computations without forcing volatile accesses.

The author read all 1,752 original instructions against the C/header. The
research peer completed the same whole-body review and identified the Z
capture correction, independently confirmed by the parent. Both then reviewed
the corrected C/header/native packet and reproduced all 33 fresh complete
compiler links under GCC 3.2.3, GCC 2.9 and its supported SAVE128 profile.
All comparisons have zero unresolved relocations and zero exact matches.
Both independent native runs passed all 851 checks. Their proof audits cover
whole function/caller hashes and geometry, all branch targets, callback pairs,
both switch tables, the stored dispatch region, state members and float sites.
The parent also read the complete 548-byte physical constructor ABI callee.

At `00172EA0`, JAL writes RA and the pointer's low ADDIU is its actual delay
slot at `00172EA4`. The callback register survives that write and is completed
before control enters the callee. The metadata explicitly records RA as GPR31,
including this delayed materialization. The approved manifests are registered
in `config/functions/actor_substates.json` and `config/symbols/actor_substates.json`.
All eleven entries remain reconstructed. The independent peer packet is
`build/reuse/actor_substates_review/audit.json`. The corrected source, header,
native harness and profiles are frozen. The parent canonical verifier and
hybrid build subsequently passed, reproducing the complete retail ELF hash;
246 tooling tests passed with one platform-specific skip.
