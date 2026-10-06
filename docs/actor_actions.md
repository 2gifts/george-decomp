# Connected actor action methods

This isolated batch reconstructs 15 complete numerical entries spanning 5,012
original text bytes. Thirteen entries have 52 actual incoming JALs; the two
duration callbacks have concrete function-pointer materializations in included
callers. The ignored manifest preserves whole caller geometry, original hashes,
all 128 local branches, two callback pairs and the complete 36-word angle table.
No original class names or source symbols are asserted.

| Entries | Observed behavior |
| --- | --- |
| `1784D0` | State-gated angle selection and soft-double distance comparisons |
| `1787B0` | Five effect-selection branches and lazy registry creation |
| `178D10` | Full-width visibility flag and callback-sensitive object iteration |
| `178E80`, `192B28`, `192BC0` | Pose reset, paired requests and duration callback |
| `179168` | Matrix-derived ray setup |
| `191E88`, `191F78`, `191FF0`, `192078`, `192180` | Health publication, damage notification and key selection |
| `190FC0`, `195850` | Duration callback and bool-return paired request |
| `195DC8` | Collision wrapper with two output vectors and live float input |

The angle method publishes its original float to actor+`0x64` before inspecting
the configuration and optional direction request. Its virtual direction result
supplies Z and X to atan. The soft-double algorithm retains each conversion,
absolute-value comparison, subtraction and optional negative-one multiply.
Its PI, TWOPI and HALFPI operands are exactly `400921FB60000000`,
`401921FB60000000` and `3FF921FB60000000`, derived from the original binary32
constants. The final state reload occurs after those calls. The real table at
`0042C5E0` has three outcomes: state0 captures old angle58, writes the new angle58
and then old angle504; twenty other states write angle58; fifteen states
preserve angle58. Unsigned states at least36 write angle58. The explicit C
conditions follow every actual table target; the table receives no extra award.

The effect method captures its full 64-bit flag and key before choosing five
original branches. Each branch reuses the complete previously reviewed effect
registry block. Its initial configuration gate precedes construction, while
the subsequent configuration key and global reload follow registry callbacks.
Visibility likewise captures its mode while reloading each later object after
earlier callbacks. Its loop consumes a signed-halfword count after the optional
object calls and reloads that count each iteration. It forwards a null record
object when present in counted storage, preserving the existing engine contract.

The two pose methods share the complete flag/control/physical reset block.
The physical object stays captured across the reset call, but its inline
component's table reloads afterward. Four explicit word stores initialize the
zero Vec4; aggregate initialization introduced a compiler `memset` dependency
and was replaced before the frozen proof packet. The request API consumes
eight integer arguments in GPR4 through11, a ninth on the caller's stack and
separate f12/f13 floats. The original request uses time0 and scale5. The later
actor request has seven integer arguments and f12=5. Main and companion stores
retain their individual pointer reloads, including a scalar write that can
replace the actor's primary pointer before the next flag store.

The health methods reuse only the identical notification block. Arithmetic
and gates stay explicit: one sets an incoming value with a signed offset;
another adds and applies the EE upper minimum without inventing a lower clamp;
scaled damage releases the first captured object and then the fresh second
object before loading configuration. Notification captures the current scalar,
publishes payload/tag/subtype and reloads the callable afterward. This matters
when argument publication aliases the callable field. Scaled damage stores
the original key after notification. The higher-level damage event accepts
unordered input through its `change <= 0` rejection gate and reloads actor
state and configuration after callbacks.

The bool request publishes its phase words before looking up the captured
reference. Primary receives eight integer arguments with the actual duration
callback and actor context; companion reloads its object and owner afterward.
Its optional message uses fresh configuration and flags. It returns one even
when lookup fails, as the original does. The collision wrapper preserves its
incoming f12 all the way to the engine call. Both original callers explicitly
load that float from configuration+`0x244`. Five integer arguments provide the
engine object, derived point, two output pointers and full mode. The caller
uses the returned object pointer, supporting the observed pointer-return ABI.
All point components are computed before that call can overwrite aliased output.

Run the asset-free checks with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_actions.py
```

The current harness passes 1,091 checks across all 15 bodies. It covers all
angle-table groups and exact PI/HALFPI/TWOPI boundaries, fresh state after soft
calls, full keys and flag words, signed count reloads, changed physical tables,
paired-request objects and owners, alias-sensitive primary and VM publication,
all five effect branches, registry insertion alternatives, duration reloads,
matrix/ray outputs, collision snapshots and signed-zero incoming float bits.
Engine calls are mocked; finite host arithmetic does not model EE FCR effects
or prove general nonfinite instruction behavior. Existing bounded storage and
engine input contracts are preserved.

The author read all 15 complete original bodies against the stable C/header.
All 45 genuine complete compiler/link comparisons under the three supported
profiles have zero unresolved relocations and zero exact matches. The peer independently read all 15 complete originals (1,253 instructions)
and the entire C/header/native packet. It reproduced all 52 genuine incoming
JALs and whole caller hashes, 128 bounded branches, both callback materializations,
the complete 144-byte table, all 16 immediate float sites (nine values), five
soft64 chains (four values), request ABI and collision input/return evidence.
Its 45 fresh complete links reproduce every comparison dictionary, and its
native run passes all 1,091 checks. No semantic/source defect was found. The
parent also independently read all 1,253 original instructions and the entire
C/header/native packet, reproduced the complete entry/table/literal/ABI proof,
all 45 fresh full links and all 1,091 native checks. The approved manifest and
separate numeric bindings are registered in `config/functions/actor_actions.json`
and `config/symbols/actor_actions.json`. All 15 functions remain reconstructed;
no candidate matches retail. The peer audit is
`.local/actor_actions_peer/audit.json`. No canonical verifier was run by this
author.
