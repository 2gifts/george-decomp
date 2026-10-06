# Resource groups and connected publication helpers

Ten complete bodies recover 2,132 original bytes and 533 instructions. The
observed group prefix contains two arrays of sixteen child pointers, ending
at +A8. The factory allocates E8 bytes; the remaining tail is unexamined.
Numeric names describe the evidence without asserting an original class.

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| 20F128 | 536 | Resolve holder arrays, create children and register readiness |
| 20F340 | 256 | Activate or retain children, then notify the holder |
| 20F440 | 408 | Publish ready child payloads and complete the group |
| 20F5D8 | 288 | Release children and detach pending registrations |
| 20F6F8 | 348 | Find, retain, request or allocate a group |
| 2B6DC0 | 56 | Relocate three nonzero relative pointer fields |
| 2B7088 | 84 | Copy primary keys with a captured limit and fresh count |
| 2B70E0 | 108 | Sum secondary counts, copy payloads and clear their slots |
| 2B6F28 | 28 | Publish a primary payload, then clear its +10 word |
| 2B6F48 | 20 | Publish a secondary payload |

The source reuses the reviewed resource ownership prefix, signed virtual-pair
layout and numeric engine interfaces. Initialization retains the captured
holder across callbacks while reloading counts, flags, keys and array entries
where the original does. Child publication precedes readiness registration.
The complete publication loops stop at the first nonnull unready child.
Independent review caught this early-exit requirement in both loops; fixtures
place ready and null entries after an unready entry to retain the regression.

Release calls the child's adjusted virtual method before reloading its array
slot for detachment. The pending flag is captured after the initial holder
call and retained through both arrays. Factories preserve original allocation,
constructor, halfword-clear, table-publication and flag-store order, including
the original nonnull allocation contract and low-halfword reference wrapping.

Whole original hashes, all 55 stores, 83 local branches and sixteen actual
incoming J/JAL instructions with complete containing intervals are recorded
in the manifest. Two other entries have aligned readonly references only;
their enclosing dispatch and runtime reachability remain unresolved. The
actual 16-byte readonly prefix at 43A458 is hash checked. Its observed pair
at +8 has adjustment zero and target 20F9B0. This establishes that binding,
without claiming the complete table, class layout or unrecovered destructor.
The fallback global's contents are supplied only by authored fixtures.

The model executes all ten original bodies, including the five real helpers.
It accepts only those bounded code intervals, initialized aligned fixture
memory, the hash-checked table prefix and the specifically permitted fallback
global. Only an actual JR31 with its owned delay may end execution. Unknown
calls, invalid operands, unowned branches and unsupported VU forms are rejected.
Engine and virtual callbacks use explicit authored observations and mutations.
They do not establish engine, heap, timing, register or hardware equivalence.

The 220 fixtures execute 22,913 original instructions, at most 466 per case.
They cover mode/flag gates, null and unready children, fresh callback values,
reference wrapping, signed virtual adjustments and helper output aliases.
The native 32-bit harness compiles the production source separately and passes
173,269 full-buffer, event, call and result checks. It translates only proven
pointer fields: packed counts 2/2 numerically resemble a fixture address.
Its 64-KiB-aligned storage preserves low pointer bits for a halfword-count alias.
Both counts and output capacities must remain within the sixteen-slot fixture
domain; valid holders, payloads and virtual tables remain caller contracts.

```powershell
.venv/Scripts/python.exe tools/trace_resource_groups.py --golden-header tests/native/resource_groups_golden.h
.venv/Scripts/python.exe tests/native/run_utilities.py --harness resource_groups
.venv/Scripts/python.exe tests/test_trace_resource_groups.py
```

The fixture input hash is
`fa12eff4402a8a74355e5c0e1289c9893dc1985c1a4d6371ffd8e458f8790607`.
The golden header hash is
`aaf8faf094dc0098b3d2152745acbdf7e6cce4f3e976cc0eda1091a6e573f70e`
with LF and
`1ed4dcd1d9f93cc4fdc172a40730172535a6b06ad0da69fc06e51d11e3ac6afb`
with CRLF. Eight strict decoder guards pass.

The parent and two independent peers reviewed the complete original bodies,
C and header. The reuse peer reproduced all thirty genuine complete links;
the disc peer reviewed the model and native tests and repeated those links.
The parent repeated the corrected thirty links, fixture generation and native
tests. All have zero unresolved relocations. Only the complete 56-byte
relocator and 28-byte primary publisher match under GCC 2.9, totaling 84 bytes.
The other eight bodies remain reconstructed. An earlier pre-correction score
packet is superseded by fresh complete comparisons; no masking, patching,
assembly fallback or partial-byte matching is used. Registered manifests
preserve source/header fingerprints, original geometry and all three profiles.
Canonical global verification and the hybrid build passed after registration.
All 246 tooling tests passed (one platform-specific skip), and the complete
retail ELF hash reproduced. Hybrid assembly coverage earns no C recovery credit.
