# Actor construction and control

Five complete numeric routines cover 10,684 original instruction bytes, or
2,671 instructions. This batch uses existing actor, member, matrix, script,
soft-double and EE minimum representations. Original class names remain unknown.

| Address | Bytes | Observed behavior |
| --- | ---: | --- |
| `0016CF30` | 6,928 | Four-wheel control update, contact/fallback queries, suspension, steering, slip curves, drive/brake/coast forces and two virtual vector outputs |
| `0016EA40` | 152 | Manager unlink, fresh child reference decrement, adjusted release and base destruction |
| `0016EE80` | 48 | Destruction table publication and optional allocation release |
| `0016EEB0` | 104 | Retained-object constructor and 40-byte buffer allocation |
| `0016EF18` | 3,452 | Actor initialization, components, effects, physical object, vehicle binding and initial state |

The complete bodies equal the validated executable through the final return
delay instruction; zero alignment is excluded. Each has a concrete entry proof.
Four actual incoming JALs have whole containing-body hashes. The two entries
without direct callers occur in zero-adjusted pairs in bounded regions at
`0042BF48` and `0042BF90`; real constructor table stores are recorded with
preserved register paths. Three regions total 208 bytes. The `0042C018` evidence
asserts only its 16-byte prefix. No data-recovery award or guessed full class
table follows from these proofs.

The initializer's actual caller and callee establish eight integer register
arguments, seven stack slots spaced eight bytes apart, and a separate `f12`
argument. Its return is unused. The small constructor returns its retained input.
The control object's `+1C` points to a separate controlling context; the real
constructor stores that relationship and publishes the child into parent
`+278`. These partial layouts do not equate it with actor `+18` data.

The author read every selected original instruction, the complete initializer
and allocation callers, and the supporting engine routines explicitly listed in
the proof packet. That review preserves interleaved position reads, signed
member adjustments, wrapped halfword reference counts, callback-sensitive data
and vehicle reloads, and all effect-list count captures. Signed random remainder
retains the original valid-divisor caller contract. Full-width vehicle indices
remain full-width in stored state even when a virtual call receives a signed
byte.

The control update keeps a shared local region that matters when a curve helper
leaves output untouched. Original projected XYZ stores at `0016DFBC` through
`0016DFCC` populate stack `+E0`. The two-component curve calls at `0016E14C`,
`0016E42C` and `0016E500` all pass this same region. Complete helper `002ADC80`
review established its unwritten-output paths; the source now aliases its curve
output to that projected vector. This author-found correction preceded the final
packet. A native regression makes the curve callback write nothing and checks
that `(2, 0, 3)` survives to the resulting force. Negative-up components use unary
negation, retaining the original `NEG.S` operation.

The isolated 32-bit native harness passes **6,146 checks**:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_construction.py
```

It executes all five complete production C bodies with authored controlled
callbacks. Cases cover contact/fallback/plane outcomes, suspension and antiroll,
all drive modes, brakes/coast, slip scaling, direction thresholds, callback
mutations and fresh input values, signed adjusted virtual calls, initializer
component/effect paths, list shrinkage, random callback mutation, vehicle index
narrowing, shifted position aliases and reference-count lifecycle behavior.

Supporting helper contracts have deliberately narrow meanings: `001F6350`
selects the first index whose signed-byte callback result is zero; no matching
actor meaning is established. `0029F080` requires a valid fraction-output pointer
on its computed-ratio path. The selected effect-helper calls discard their result;
the local `00239E40` pointer-return declaration is provisional, without evidence
of a stable pointer return across its original capacity paths.

Host tests model ordinary floating-point values and GNU soft-double comparison's
documented positive unordered result. Matrix, collision and curve callbacks are
limited observations; the quaternion matrix mock uses identity plus translation.
They do not verify the original engine algorithms. Scalar expressions model the
observed VU XYZ values; EE accumulator, flags, saturation and rounding are not
proved by native execution. The reviewed bit-value minimum is reused unchanged.
Uninitialized output regions retain original helper contracts.

All **15 genuine complete compiler/link comparisons** under GCC 3.2.3, GCC 2.9
and guarded GCC 2.9 SAVE128 resolve at original addresses with zero unresolved
relocations. None reproduces the full original symbol. All five remain
**reconstructed**, with no masking, assembly fallback, adjusted padding or
progress awarded for the linked helper code.

The ignored packet is `build/actor_construction/manifest.json` and
`symbols.json`, generated by its frozen `finalize_draft.py`; comparison reports
come from `probe.py`. It records 154 bounded branches, all 530 decoded stores
including delay slots and VU stores, eight callback materializations, eleven
binary32 values at 29 genuine immediate sites, and six complete immediate chains
for three soft-double values. The complete 42-state table hash is reused as a
binding, without another data award.

Independent research review passed all five selected complete bodies and the
refreshed shared-scratch and unary-negation sites. The peer also read all fourteen
supporting bodies separately and corrected four overly broad ABI descriptions:
the `001F6138` wrapping `+38` counter increment, callback-zero index selection,
the plane probe's required output pointer and the effect helper's provisional
return type. These were proof-description corrections; production call behavior
was unchanged.

The peer independently reproduced the complete metadata packet, all fifteen
fresh genuine links and the warning-free 6,146-check native run. Its ignored
audit is `build/reuse/actor_construction_review/audit.json`. Supporting helper
reads receive no additional recovery award. Parent review passed all selected
original/source/header and native/proof scripts, fresh packet reconstruction,
fifteen genuine whole links and the warning-free 6,146-check native run.
All five functions are centrally registered as reconstructed; none is exact.
