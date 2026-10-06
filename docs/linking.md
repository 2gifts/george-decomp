# Original-address function linking

The checker compiles a dedicated `.text.<symbol>` section and uses the pinned
GNU linker to resolve its references at the original function address. It
compares the entire resulting function with the retail bytes. Symbol bindings
and explicitly proven local storage are the inputs to linking; original
instruction bytes never drive an instruction replacement or wildcard match.

Read-only compiler data remains in the separate `mapped_sections` interface.
That path requires the entire original allocated read-only byte sequence and
its SHA256, rejects writable or NOBITS input, and permits only the explicitly
reviewed same-section `R_MIPS_32` self-pointer policy when data has relocations.

Compiler-local zero-initialized storage uses the distinct `mapped_nobits`
interface. Each input must name one complete `.bss` or `.bss.<name>` section
and provide its fixed original address, full size, and SHA256 of its all-zero
initialization. The supported section is `SHT_NOBITS`, with exactly allocated
and writable flags, zero input VMA, no data relocations, no entry-size format,
and a power-of-two alignment respected by the requested address. Partial
sections, initialized writable data, unknown storage, unsupported symbols,
out-of-range addends, unused mappings and overlapping mapped addresses fail.
Each complete section is bounded to 65,536 bytes.

The source object and local storage symbols are retained. Only bounded local
data/section symbols can refer to this storage, including zero-size local
object symbols emitted by the old GNU compiler. Actual GNU `ld` places the
unchanged input section using `NOLOAD`. The output must retain the exact
NOBITS type, flags, address, size and alignment. Unexpected allocated sections,
executable sections and remaining relocations fail. This does not manufacture
initialized file bytes or convert writable storage into read-only data.

Function manifests opt into this path through `link_bss`. Each proof records
the input section name, original NOBITS section, fixed address, complete size,
zero-initialization digest, and actual original code pointer bindings. There
is no file offset: NOBITS memory has no original initialized file payload.
The verifier independently checks the fixed range lies inside original
allocated writable `SHT_NOBITS` memory, then decodes the specified original
`LUI`/same-base `ADDIU` pair. The low immediate is signed. A pair must be
adjacent or separated by one proven nonwriting NOP, non-linking branch or
return instruction; its full resulting pointer must equal the recorded
storage address plus a bounded relative offset. The linker independently
checks all actual source-local relocation addends against the complete input
section. Fully linked code equality remains required to award a match.

A binding can instead declare `access: "lw32"` or `access: "sw32"` to prove
the effective address of an original same-base `LUI`/`LW` or `LUI`/`SW`.
The complete four-byte access must be aligned and lie inside the proven
storage; `LW` must write a nonzero register different from the address base.
An adjacent access needs only this explicit declaration. A gap of eight or
twelve bytes also requires `preserved_sequence: true`. That opt-in permits
only NOP, ordinary `SD`/`SW`, `ADDIU SP,SP` when SP is not the address base,
and `LW` into a nonzero register different from the base between the pair.
Calls, control transfers, other instructions and base clobbers fail. The same
opt-in allows a twelve-byte `LUI`/`ADDIU` pointer pair. Only literal `true` is
accepted; the legacy three-field four/eight-byte pointer proofs retain their
existing behavior. All offsets still refer to the complete original function.

For example, a word-load binding is
`{"hi_offset": 0, "lo_offset": 12, "relative_offset": 0, "access": "lw32", "preserved_sequence": true}`.
Its actual original words must pass those checks; the declaration supplies no
replacement instructions. These proofs leave the full NOBITS geometry,
zero digest, genuine GNU link, relocation checks and complete code comparison
unchanged. They permit no initialized writable data or storage byte credit.

The hybrid executable build replaces only verified function bytes in the
original executable. It retains the retail file's original section layout,
NOBITS geometry and metadata; per-function test links are verification
artifacts. A zero-storage proof alone awards no code or data-identity progress.

`tests/test_nobits.py` uses synthetic ELF/code and checks genuine GNU NOLOAD
linking, original signed-low pointer decoding, full-size initialization proofs,
alignment and overlap, local symbol/addend bounds, relocation rejection,
malformed/initialized storage, unused mappings, clobbered pointer pairs and
preservation of the original read-only rejection guards. Explicit word-access
tests also cover signed-low carry, alignment/full-word extent, the strict
intervening-instruction whitelist, unknown fields, malformed opt-ins and
unchanged legacy proof behavior. It does not contain
game bytes or mirror one runtime function's implementation.
