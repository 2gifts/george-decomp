# Arena reservations and parallel arrays

This batch recovers eleven complete connected bodies at `002B8E00`
through `002B9388`: 1,580 bytes / 395 original instructions. Nine substantive
reservation, setup and array routines accompany the actual 32-byte reset and
8-byte no-op. Parent and independent peer review and registration are complete;
the canonical checkpoint passed 264 tooling tests and a retail-identical
hybrid rebuild.

The ordinary C template shares the observed reservation algorithm. It aligns
the cursor, compares wrapped unsigned `cursor + bytes` strictly below wrapped
`base + capacity`, and resets unless the wrapped endpoint is strictly below the wrapped limit. Reset updates the high
water value before reloading and publishing the base, then aligns that base.
Reservation publishes the final cursor before the array output stores. It
neither invokes the stored callback nor introduces an allocation-failure guard.

| Original entry | Complete bytes | Behavior |
| --- | ---: | --- |
| `002B8E00` | 132 | Generic descriptor reservation |
| `002B8E88` | 32 | High-water update and cursor reset |
| `002B8EA8` | 64 | Global descriptor setup and actual no-op call |
| `002B8EE8` | 8 | Actual empty body |
| `002B8EF0` | 52 | Global alignment |
| `002B8F28` | 140 | Global variable-alignment reservation |
| `002B8FB8` | 136 | Global fixed-16 reservation |
| `002B9040` | 220 | One stride-12 array and padding zero |
| `002B9120` | 276 | Two stride-12 arrays and padding zeros |
| `002B9238` | 336 | Two stride-12 arrays and one stride-4 array |
| `002B9388` | 184 | Stride-16, stride-8 and stride-4 arrays, no zeroing |

The six observed descriptor fields occupy 24 bytes: base, unsigned capacity,
cursor, high-water pointer, reset callback and opaque context. Actual writable
state is at `0046A0D8`; eight preserved instruction pairs establish that address.
The adjacent descriptor initializer/setter/reset/alignment bodies corroborate
the field offsets. No original writable initializer identity is claimed.
The reset destination is genuinely constructed at `002B8EAC/002B8EBC` and stored
at `002B8ECC`. The destination consumes a descriptor in `a0`; no indirect
invocation or consumed reset return has been established. Its retail `v0` base
value is excluded from the void source/test contract.

All array counts retain low-32-bit `(count + 3) & ~3` arithmetic. Padding uses
the actual **signed full-word** `count < rounded` gate, including its wrap and
negative-count behavior. Zero destinations, lengths and array bases stay
captured across the preserved numeric `003936A0` calls. Output arguments may
alias each other, the descriptor or the padding being cleared. The third array
in `002B9238` uses separate count/difference shifts by two. `002B9388` does not
clear padding. Output element types remain neutral; the sole verified caller
of `002B9040` subsequently fills six XYZ float triplets.

The author read all 395 original instructions and the complete source/header,
checked raw/disassembly equality, full hashes, terminal delays and excluded
zero alignment, 33 local branches, 66 stores and seven direct calls. Four actual
incoming JALs reach three entries: setup, no-op and the one-array routine.
Whole containing intervals and the relevant argument/return uses were checked.
Seven other entries have unresolved incoming reachability after the complete
`.text` J/JAL, aligned allocated non-executable pointer-word and broad same-body
LUI/lower scans. The reset's materialization proves a stored code pointer, not
its execution. Complete-body reconstruction and incoming reachability are
separate evidence.

All 33 natural full-function GNU links have zero unresolved relocations. Reset
and no-op match all original bytes under all three established profiles; the
nine other routines remain reconstructed. Genuine symbol extents, save choices
and scheduling differences are retained. GCC 2.9 naturally removes the local
empty call in setup; the source preserves that call, and its complete linked
44-byte result is honestly compared with the original 64 bytes.

The scoped decoder reads only these complete original bodies from the validated
local ELF. It reuses the reviewed scalar decoder and adds the observed SLLV,
SUBU, signed/unsigned SLT, R5900 MULT destination/low-word state and MFLO forms
with strict operand checks. MFLO before a reviewed MULT, unknown memory,
unreviewed code/calls, COP1/COP2/MMI forms, cross-body transfers, malformed returns
and missing delays fail. Only actual `JR31` to the selected stop ends an
invocation; ordinary untaken delays execute and likely untaken delays annul.

The 452 authored fixtures execute 15,908 original instructions, at most 72 per
case. They cover endpoint equality, reset and high-water comparisons, masked
exponents 31/32/63, numeric unsigned wrap, signed count boundaries, output aliases
and explicit post-zero mutations. The controlled zero call records ten words:
its three arguments, four published descriptor fields and three output words.
The native harness compiles **production C as a separate translation unit** and
passes 238,300 checks; eight portable decoder guards pass.

```powershell
.venv/Scripts/python.exe tools/trace_arena_buffers.py --output .local/arena_buffers/trace.json --golden-header tests/native/arena_buffers_golden.h
.venv/Scripts/python.exe tests/test_trace_arena_buffers.py
```

Run `python tests/native/run_utilities.py --harness arena_buffers` for the
asset-free native harness. The original local three-profile and independent
parent/peer proof packets are retained under `.local/arena_buffers` and
`build/reuse/arena_buffers_review`.
Prepend the native MinGW compiler directory to `PATH`. Native flags are
`-m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude`; compile
`tests/native/arena_buffers.c` with `src/game/arena_buffers.c` as separate inputs.
The harness reserves storage below bit 31 and aligns it to 64 KiB to preserve
authored address alignment. Numeric wrap-case pointers are never dereferenced.

Fixture input SHA-256 is
`3b97c32b0984535f8469fc253dc426158018494b89bcac6a6b225fbc89e164fa`.
Golden header canonical LF SHA-256 is
`c06dc26bdec38f630656dabee6ecae68db956cb42f6314de1346decb17cebad3`;
Windows CRLF file SHA-256 is
`aa66e40230482ebcf72eae3104c6a88364731b10889a7c2ca05caa1e1dad10b4`.

The tests assert caller observations in bounded synthetic storage. Their memset
zero-store contract and exposed post-call mutations do not execute its MMI
implementation or prove authentic callback/allocator behavior. Wrapped and
negative-count cases test numeric/store ordering with deliberately accessible
destinations; they do not establish safe capacity for arbitrary invalid counts.
No register upper-half, EE exception/FCR/timing, allocator, original initializer
or general malformed-pointer behavior is claimed. No original instruction,
table or asset arrays are copied into tracked files.

Parent and independent peer each read all 395 complete original instructions
and the full C, header, inline template, scoped trace, native harness and guard
sources, and reproduced all 33 whole links, geometry/reachability packets,
452 fixtures, 238,300 native checks and eight guards. Parent review refined
only a native harness address guard to permit the complete authored buffer
extent below bit 31. Production source and golden fixture hashes are unchanged.
