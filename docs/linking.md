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

The hybrid executable build replaces only verified function bytes in the
original executable. It retains the retail file's original section layout,
NOBITS geometry and metadata; per-function test links are verification
artifacts. A zero-storage proof alone awards no code or data-identity progress.

`tests/test_nobits.py` uses synthetic ELF/code and checks genuine GNU NOLOAD
linking, original signed-low pointer decoding, full-size initialization proofs,
alignment and overlap, local symbol/addend bounds, relocation rejection,
malformed/initialized storage, unused mappings, clobbered pointer pairs and
preservation of the original read-only rejection guards. It does not contain
game bytes or mirror one runtime function's implementation.
