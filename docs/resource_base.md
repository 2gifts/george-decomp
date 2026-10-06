# Resource base and callback ownership

This batch reconstructs eight complete functions, 1,152 original bytes
and 288 instructions, in `src/game/resource_base.c`. None of its 24 genuine
whole-function links matches the original. These are reconstructed candidates;
author, peer and parent review passed, and canonical registration is complete.
Canonical verification passed all 280 tooling tests (one platform-specific
skip), and the hybrid rebuild reproduced the retail ELF SHA-256.

| Entry | Bytes | Observed behavior | Actual incoming J/JALs |
| --- | ---: | --- | ---: |
| `00225E80` | 48 | Manager lookup through the existing map implementation | 31 |
| `002267D0` | 380 | Base destruction, pending callback release, allocator selection and poisoning | 8 |
| `00226BC0` | 160 | Five-integer base constructor and manager initialization | 16 |
| `00226D78` | 100 | Clear deferred flag, increment narrowed count, dispatch readiness | 10 |
| `00226E38` | 156 | Immediate callback or allocation and list insertion | 4 |
| `00226ED8` | 104 | Remove the first callback with an equal key | 5 |
| `00226F40` | 112 | Remove, invoke and release pending callbacks | 3 |
| `00226FB0` | 92 | Count/flag gate for the table `0x18` method | 6 |

Each complete original body has a terminal `JR $31` and its delay instruction;
zero alignment after the return is excluded. All 83 actual incoming transfers
have independently checked containing-body extents and full hashes. The base
destructor also appears as an aligned read-only table pointer at `0043B1D4`.
No new entry relies on an unverified heuristic boundary or invented incoming
reference. The packet records all 33 local branches, 13 direct calls, four real
`JALR` instructions and 51 stores, including stack saves.

The existing resource header stays unchanged. The new header describes an
observed `0x24` base prefix, a 16-byte callback node, a manager field at `0x168`
and selected allocator fields through `0xFC8`. The 1,000 slot-pointer entries
are corroborated by the full existing original allocation helper's `0x3E8`
index test. These partial declarations do not identify a complete original
class or justify arbitrary index, object or pointer validity.

The constructor leaves size `0xC` and word `0x14` untouched before its manager
call. It preserves all original initialization stores and reads the payload and
flags again afterward. Destruction frees callback nodes without invoking them;
each captured successor overwrites the head after the pool call. Its later key
removal and accounting calls precede the fresh payload, manager and size loads.
The override-pool path retains the original unmasked pointer and optional pool.
The ordinary path masks to 28 bits, compares both unsigned endpoints inclusively
and retains the wrapped `(size + 127) >> 7` index. It adds no bounds or safety
check to this original algorithm.

Subscription returns zero after an immediate callback or allocation failure,
and one after insertion. Callback argument one contains the resource pointer's
32-bit representation; argument two and the removal key are opaque words.
Actual existing callbacks use both observed lanes. Dispatch publishes the
successor before calling the current node, reloads the global pool afterward,
releases the captured node and then reloads the head. Nested insertion, removal
and changing the global pool therefore affect subsequent work in the original
order. The void routines do not claim a value from incidental original `v0`.

The `00226FB0` table slot is a resource-preparation dispatch, rather than the
destructor slot. The known tables point it to resource preparation methods
`0020E920` and `0020F128`, among others; its second argument is zero. Readiness
uses the separate pair at `0x28` and only consumes the adjusted record argument.
The original caller's incidental live second GPR is not invented as a parameter.
Both pairs retain the signed 16-bit adjustment. Complete 48-byte read-only
prefixes of the base and eight known derived tables are hashed, and the two
actual base-table address constructions are independently decoded. The genuine
eight-byte base no-op targets are reviewed and excluded from this recovery
count. No original table initializer is copied into the project.

Reused numeric callees are the existing `00219FF0` map lookup, `002AD700` pool
allocation, `002AD748` pool release, and recovered free wrappers `0020E5C0` and
`002AF100`. The subscription caller really passes pool, size 16 and mode zero.
Its local declaration preserves those three GPR lanes; the known allocation
body consumes only the pool. The native bridge explicitly accepts all three
before executing the unchanged one-parameter pool C. Frozen pool headers and
source are unchanged. The full unknown original bodies `00224AB8` (1,040 bytes),
`00225C28` (92 bytes) and `00224750` (212 bytes) were read for their ABI and
layout evidence; their numeric calls remain in place and their source is not
counted as recovered.

`tools/trace_resource_base.py` reads the validated local ELF and executes the
eight entire original bodies plus the three real lookup/pool helper bodies.
Manager, heap and virtual calls have explicit authored observation contracts.
Some mutation hooks stress permitted caller-visible reloads beyond a complete
model of those external routines. Nested callbacks execute the real subscription
and removal entries. The trace only implements the required integer subset;
it rejects unknown instructions and calls, reserved operand forms, invalid
memory, zero division and unsupported transfers. Ordinary branch delays execute
even when untaken; likely delays annul, and only an actual `JR $31` to the
selected return ends an invocation, including recursive returns within a body.

There are 290 synthetic fixtures, 18,201 executed original instructions, and
a maximum of 191 instructions per case. Eight focused decoder guards pass.
The native harness passes 795,118 checks. It executes production resource C,
the existing map template and the unchanged pool C through observation bridges.
Every case compares all 2,560 memory words, both global pointers, all 12 call
counters, eleven-word call events, and only the four actual result APIs.
Lossless runs encode authored initial memory and changed-word patches keep the
fixture header compact; these do not omit unchanged-word comparisons.

Cases cover signed virtual adjustments, count wrap, all relevant flags,
constructor post-call mutation, first/interior/missing callback keys, node/header
aliasing, pool failure and freeze, nested insertion/removal, changing pool/head
during dispatch, masked pointers, inclusive range boundaries, size-index wrap,
override selection, and deletion order. Native storage must lie wholly below
bit 28 because these cases exercise the original pointer mask on actual native
pointers. Only known callback-node function fields normalize the retained high
16 bits after the real free helper overwrites their low 16 bits. No malformed
capacity, null-pool, arbitrary-pointer or arbitrary-alias safety is claimed.
These checks do not establish heap, full engine, register upper-bit, EE exception
or timing fidelity.

Fixture input SHA-256:
`d9685fa633a7541d12f512c834ee6f809baee6577be2a85b4f4e1c3ed0aee946`.
Header SHA-256 with LF:
`d5678b0033aada91d026f572655722098d5759dfacb72088c460b5a26a79febd`;
with Windows CRLF:
`3cd199428a4149d99ff4f80c4c3f5d58e633c093029a70aadd2917202210f32d`.
The ignored `.local/resource_base` packet contains complete original hashes,
caller geometry, table/global evidence and every full linked comparison.

The standalone native command uses the bundled 32-bit MinGW compiler with
`-m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude`,
`tests/native/resource_base.c` and `src/game/resource_base.c` as separate
translation units, followed by `-lm`. The compiler's directory is prepended
to `PATH` before running. Fixture regeneration requires the local original;
the tracked synthetic header and native checks do not distribute or require
original game bytes.

The parent independently read all 288 selected original instructions and the
complete source/header, scoped decoder, native harness and eight guard tests.
Peer and parent reproduced every full comparison and proof dictionary, all
290 fixtures, 795,118 warning-free native checks and eight decoder guards.
Supporting helper complete manual reads are attributed to the author and peer;
the parent verified their geometry and ABI evidence. The tracked packets are
`config/functions/resource_base.json` and `config/symbols/resource_base.json`.
