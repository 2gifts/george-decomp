# Vehicle entry, jump and paired actions

This batch recovers 22 complete routines covering 5,540 original bytes. It
contains the 3,140-byte vehicle entry candidate generator, vehicle stop,
five jump lifecycle/state methods, fourteen paired action lifecycle/dispatch
methods, and the child destructor/pool-return helper. All remain reconstructed:
none of the 66 individual linked comparisons passed complete retail byte
equality under GCC 3.2.3, GCC 2.9 or supported GCC 2.9 SAVE128. Every comparison
records its full size, hash and differences without masking bytes or relocations.

The source reuses established owner, entity, reference, vehicle, vector,
virtual-pair and slot-pool layouts and engine calls. New numeric storage views
describe accessed jump fields and two children with signed-halfword active
flags. Original class names and vehicle candidate allocation capacity remain
unknown. Four complete pairs of child methods have identical original hashes;
their typed templates are shared while each entry is compiled and linked
separately. No external game implementation, original instruction array or
inline assembly was imported into these function bodies.

The manifest records 30 actual adjusted virtual pairs following verified null
prefixes at `004375A0`, `00437678`, `00437700`, `00437828` and `00437878`.
Each interval has an original hash and constructor binding. Unrecovered base
and no-op methods are kept as dispatch evidence and excluded from this batch's
recovered count. The command data at `003F8EC0` contains two groups of four
words; the generator consumes only their low bytes, using a signed predicate
argument and reloading an unsigned byte after a successful callback.

Vehicle entry captures the entity before the position callback and reloads
the vehicle after calls. It retains returned basis pointers across later
callbacks and reads their contents after the scalar callback. Repeated paths
are factored with the same captured index and load/store order. One direction
uses the front extent plus two; the other uses the rear extent minus two.
Mirrored lateral candidates use opposite signs. Scalar sources are read before
X/Z/Y stores, and one-byte count/index wraps are preserved. The factored
candidate code currently compiles substantially smaller than retail; this is
recorded as a compiler/source-shape gap, never an exact-match claim.

Jump initialization resolves inline or reference targets with the observed
validity gates. It adds only a strictly positive target/entity height difference
and passes the live target vector in `a1` to the reviewed jump callee at
`00177270`, which consumes it. Success reloads the owner and copies the timer
after the call. The wait method changes phase when the entity's full word
differs from four, then completes only when the decremented timer is strictly
negative; zero and unordered comparisons keep waiting.

Paired calls retain their original word/output argument and reload the second
child after the first callback. Signed adjustments are applied at full pointer
width. Destructors capture the second child before clearing the first child
and active flag, release each captured unadjusted slot, then clear the second.
Updates reload the second active gate after the first call but delay both
completion stores until both callbacks finish. The alternate update restarts
a freshly loaded second child instead of clearing its active flag.

The native callback harness needs no original executable or assets:

```powershell
.venv/Scripts/python.exe tests/native/run_goal_methods6.py
```

Its 353 checks exercise both mirrored vehicle sides and extent branches,
captured entity/reloaded vehicle, scalar callbacks mutating previously returned
matrix contents, signed commands and post-predicate unsigned reloads, byte
count wrap, output callback command mutation, strict/unordered direction gates,
interleaved constructor overlap, reference-count wrap, target ABI, owner reloads,
strict/NaN timer gates, signed child adjustments, callback-dependent child/gate
replacement, delayed flag stores and fresh child restart. Host checks do not
establish complete EE FCR effects or nonfinite arithmetic.

All original body boundaries, bytes and hashes have been checked against their
complete disassembly intervals; alignment padding is excluded. Root independently
reviewed every complete instruction interval against the source, checked all
22 body hashes and terminal returns, 66 linked comparison records, 30 actual
adjusted virtual pairs, the complete 32-byte command table and all four pairs
of identical method templates. The 353 native checks also passed independently.
