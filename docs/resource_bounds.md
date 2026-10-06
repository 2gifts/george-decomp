# Resource bounds, constructors and connected group helpers

Fourteen complete bodies recover 1,356 original bytes and 339 instructions.
The source reuses the reviewed resource/group prefix and virtual-pair layout.
The kind-six factory allocates 40 bytes in hex: the resource prefix followed
by two Vec3 fields at +28 and +34. Its +24 payload has a different observed
ownership prefix, with a gate at +4 and reference count at +6. No original
class or extra payload layout is inferred.

| Entries | Observed behavior |
| --- | --- |
| 20F858, 20F860 | Group count and indexed child queries |
| 20F870 | Explicit kind-two constructor |
| 20F8D0, 2B6F60 | Conditional child publication and indexed/all-slot helper |
| 20F990, 20F9B0 | Group release wrapper and destructor |
| 20FA08, 20FA88 | Bounds projection/update and kind-six factory |
| 20FC18, 20FC20, 20FC28 | Payload and vector address queries |
| 20FC30, 20FCC0 | Explicit bounds constructor and destructor |

Initialization preserves the exact scalar store order around table publication.
The factory retains its original inline initialization; it does not gain an
invented call to the explicit constructor. A fresh flag after the projection
callback can select destruction and a NULL return. Existing objects use the
original request, activation and wrapping halfword-count gates. Allocation is
assumed nonnull where the original immediately dereferences its result.

The projection caller sets flags before the engine call and captures all point
coordinates before publishing the returned payload. It stores X, Z, Y in that
order for each vector. The complete 1,520-byte engine callee was read to prove
its three integer arguments, payload return and first three output frame rows.
Its implementation remains an engine call. The controlled test writes a
16-byte point and 48-byte frame prefix. Writing a complete 64-byte frame would
overwrite saved RA in the original caller's stack; no such output contract is
claimed. The controlled values are finite and include negative zero.

The publication method captures the holder before its child gate and forwards
incoming a2 unchanged to 2B6F60. The helper either writes the selected entry or
walks every entry for FFFFFFFF. Each payload store precedes clearing payload+10;
later iterations reload the holder's pointer and halfword count. Fixtures make
the payload clear alias that count and test overlapping primary storage. The
original signed comparison has a nonnegative halfword-loop domain. No range or
NULL guard absent from the original is introduced.

Both destructors publish their actual table before later gates and callbacks.
Group destruction executes the existing complete group-release body unless
flag80 skips it, then forwards the captured mode to the base release. Bounds
destruction captures its payload, stores the decremented low halfword before
testing zero, invokes the signed-adjusted payload pair when required, then
clears +24 after that callback. A mutation of +24 is therefore overwritten.

The manifest records complete original hashes, raw/disassembly equality,
terminal returns and owned delays, 29 local branches, 64 stores and thirteen
actual incoming JALs with full containing interval hashes. Two checked 16-byte
readonly prefixes at 43A458 and 43A4B0 have zero-adjustment destructor pairs
targeting 20F9B0 and 20FCC0. Actual table materializations establish these
bindings without claiming complete tables or classes. The wrapper 20F990 has
an aligned readonly reference whose enclosing dispatch remains unresolved.
Seven other entries have no observed incoming reference: the count/child
queries, explicit group constructor, three bounds queries and explicit bounds
constructor. Whole-body recovery and runtime reachability are distinct claims.
The broad direct/pointer/LUI searches and their limits are retained in metadata.

The strict model executes all fourteen original bodies plus the already
reviewed complete group release and primary publication helpers. The actual
checked bounds table selects and executes the real bounds destructor in both
the trace and native harness. Other engine/payload virtual calls use explicit
authored observations and mutation hooks. Only the two hash-checked readonly
prefixes and aligned initialized fixture/stack memory are permitted. Execution
ends only at an actual JR31 with its owned delay. Unknown calls, cross-body
branches, invalid operands and unowned readonly memory are rejected.

```powershell
.venv/Scripts/python.exe tools/trace_resource_bounds.py --golden-header tests/native/resource_bounds_golden.h
.venv/Scripts/python.exe tests/native/run_resource_bounds.py
.venv/Scripts/python.exe tests/test_trace_resource_bounds.py
```

The 239 fixtures execute 8,627 original instructions, at most 140 per case.
They cover mode/flag gates, constructor publication, captured/fresh callback
values, full helper indices, wrapping counts, signed virtual adjustments,
nullable payloads, aliases and returned pointers. The native harness compiles
the unchanged production bounds and group sources as separate translation
units and passes 179,846 checks. Only proven pointer fields are translated;
64-KiB-aligned storage retains low pointer bits for count aliases. Index/count
storage fits the authored four-slot domain. These observations do not prove
arbitrary engine behavior, allocation failure, dangling lifetime or EE timing.

The fixture input SHA-256 is
`3ec71f0ffebe92ae22f4c3e6b407b7c713ec78766e946f242fe95a8e89d1e6bb`.
The complete golden header's LF SHA-256 is
`56a670beda913928f858116f27921e22b4222216c9d07c762b227912e54984c3`.

The parent and independent actor peer read all 339 original instructions,
C/header and the complete projection ABI callee. All forty-two genuine
complete compiler links reproduce with zero unresolved relocations. Only
the six complete count/child/payload/vector accessors and group wrapper match,
totaling 76 bytes under GCC 3.2.3. The other eight bodies remain reconstructed.
The independent peer also reviewed the full tracer/inherited decoder and
native/metadata packet, reproduced all fixtures and forty-two links, and checked
both readonly prefixes, thirteen incoming references and fifteen real returns.
The parent read the full native harness and ten guards, then reproduced all
239 fixtures, 179,846 checks, ten guards and forty-two fresh whole links.
The fourteen entries are registered; the canonical checkpoint passed 264 tooling tests and a retail-identical
hybrid rebuild. No byte masking, code patch, assembly fallback or data award is
used for these matches.
