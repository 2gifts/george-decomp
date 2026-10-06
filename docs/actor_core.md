# Actor core lifecycle and update

This batch reconstructs five complete routines containing 10,600 original
text bytes and 2,650 instructions. Numeric names and explicit offsets describe
the observed actor layout without asserting original class or source names.

| Entry | Complete bytes | Observed behavior |
| --- | ---: | --- |
| `0016FC98` | 1,880 | Owned-record cleanup, attachment callbacks and control destruction |
| `001703F0` | 328 | Lazy effect-registry creation and gated effect installation |
| `00170600` | 6,584 | Full actor update, state/control dispatch, transforms, contacts and effects |
| `00171FB8` | 1,432 | Attached-object positions, matrices and transformed output publication |
| `00172550` | 376 | Signed state/substate gates and captured-record transition |

Eight actual encoded incoming JALs prove every entry. Their complete containing
intervals are hashed and checked against original disassembly. All five bodies
include their terminal JR and delay instruction; zero alignment is excluded.
The complete recognized-body hash inventory found no other identical interval.
Merged middleware intervals with uncertain geometry were excluded from that
scan, so it is not an assertion about arbitrary computed entries.

The proof packet inventories 293 stores, 272 bounded branches, 234 direct calls
and 18 indirect calls. Fourteen callback LUI/ADDIU materializations retain all
intervening GPR writes, including JAL's RA write. Sixteen distinct binary32
literals have 41 genuine MTC1 sites. Three full immediate chains separately
prove the soft-double pi, two-pi and minus-one operands. Dynamic state calls
reuse the previously reviewed full 42-record table at `003F83F0`; the table
gets no new recovery award.

The lifecycle retains each captured pointer and reloads later slots where the
original does. Manager unlink callbacks can change the released record. The
post-release `2FC` pointer is captured before `304` is cleared. Collection
counts and inline signed-halfword loops preserve their distinct captured/fresh
rules, including unconditional nullable releases. The final control pointer is
captured before actor field 8 and bit 36 are cleared; its second virtual call
reloads the control after the first callback.

The main update retains its early bit-32 route, positive timer subtraction,
contact callbacks, delayed state request and member adjustment rules. The
bit-42 store precedes the height read through the already captured data pointer.
Repeated angle calls reload the matrix every time. Vector publication retains
the original interleaved reads and stores, including the Z capture before a
shifted W store. The companion update reads its retained local vector after
intervening engine callbacks; its length can differ from the primary update.

The emission gate reads a complete 64-bit word spanning actor `3E8` and `3EC`.
Its two status calls remain separate. Projection captures the original dot
product before transformation, then reloads basis and emission fields after
callbacks. The particle call consumes five GPR arguments and four independent
float arguments. The optional emission frame reloads its key after that call.

The original local output regions overlap. `GeorgeActorCoreScratch` retains
those observed views in one 208-byte aligned union: frame at 00, Vec4 at 10,
frame at 20, vectors at 30/40 and frames at 50/90. This is a representation of
the observed storage, not a claim about original source locals. The caller
does not initialize outputs the engine is required to supply. The final
position reads the translation written by the frame at 90, rather than an
earlier overlapping region.

Complete callee reads corroborate the nullable pointer result from `00175590`,
the five-GPR/four-FPU particle call, Vec4 inputs to `003565B8`, the three-GPR/
three-FPU `00272C40` setter, and pointer outputs from `00146AC8`, `00191B00`
and `00191AB8`. Those callees remain external engine bindings and receive no
new source recovery award. Compile-time checking found a missing float return
declaration for `0029B940` in the draft; the explicit prototype was corrected
before the final target compiles, links and native packet.

Run the authored asset-free checks with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_core.py
```

The native runner currently passes 2,296 checks warning-free. It executes all
five complete production bodies with controlled engine callbacks and output
buffers. Cases cover signed/NaN timer gates, state dispatch and full adjustment
sums, callback-replaced controls and contact arrays, registry alternatives,
cleanup mutation, shifted XYWZ aliases, retained scratch, projection and frame
outputs, pointer return publication, emission ABI and soft-angle wrapping.
The count includes observer bounds checks as well as behavior assertions.

Matrix multiplication is a controlled copy-output test double; transformation
and output calls likewise supply authored values. Host double arithmetic is
used only to test caller operand/order behavior. Its unordered comparison
returns +1, following the already imported GNU `__fpcmp_parts` contract.
These mocks do not prove engine/VU behavior, EE exception state, denormal
handling or arbitrary nonfinite arithmetic. The existing raw-bit EE maximum
and square-root representation are reused unchanged in production C.

The author and research peer have read all five original bodies against the
complete C/header and found no semantic defect. The peer independently audited
the full hashes, caller geometry, decoded inventories, callback preservation,
literal chains, numeric bindings and reused state-table hash, then reproduced
all 15 complete compiler-link dictionaries and the warning-free 2,296 native
checks. The supporting engine callee hashes and ABI evidence were audited;
their complete manual instruction reads are attributed to the author.

All 15 genuinely linked full symbols under GCC 3.2.3, GCC 2.9 and supported
SAVE128 resolve without remaining relocations; none match the original whole
bytes and size. All five remain reconstructed. The parent also read all 2,650 selected original instructions and the complete
source/header/native harness, reproduced the full proof packet, all 15 genuine
links and 2,296 native checks, and registered the five reconstructed routines.
The registered packets are `config/functions/actor_core.json` and
`config/symbols/actor_core.json`. Canonical verification passed all 280 tooling tests (one platform-specific
skip), and the hybrid rebuild reproduced the retail ELF SHA-256. External engine callee complete manual reads
remain attributed to the author.
