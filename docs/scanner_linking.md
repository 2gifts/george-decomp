# Compiler-generated readonly tables

`link_codegen_readonly` is a distinct, explicit opt-in for a compiler-generated
readonly section that includes local case pointers. It does not relax the
existing exact-original `link_data` or readonly self-pointer policies. It is
intended to let an authentic reconstructed function genuinely link while its
case addresses still differ with the nonmatching code.

Each manifest mapping records the original section address, file offset, whole
size, complete SHA256 and actual original `LUI`/`ADDIU` pointer bindings inside
the complete original function. The verifier checks the mapped span against an
allocated, nonwritable, nonexecutable original `SHT_PROGBITS` section. Pointer
bindings must use nonzero registers and have an adjacent pair or one intervening
nonclobbering shift; unsupported instructions, altered words, escaped ranges,
duplicate pairs and arithmetic that fails to materialize the specified pointer
are rejected. The original function's full size supplies its independent target
boundary, including its last return delay and excluding subsequent padding.

The linker planner then requires one entire plain allocated readonly source
`SHT_PROGBITS` section with the same size and an aligned original VMA. Only one
nonempty genuine ELF `REL` table associated with the actual symbol table is
supported. Every entry must be a unique, aligned, bounded `R_MIPS_32` relocation
to an anonymous section symbol or local unsized label in the selected dedicated
function section. Each full symbol-plus-addend target must be aligned and
strictly inside that function symbol, excluding its section padding. Undefined,
absolute, global, external-function, data, sized-label and other-section targets
are rejected even when an external binding is supplied. Corresponding original
table pointers must independently be aligned and inside the complete original
function. Empty tables, other relocation types, writable/NOBITS/merge sections,
partial sections, overlapping mappings and unused mappings are rejected.

Every byte outside those actual compiler relocation words must already equal
the original, including literal constants and alignment bytes. This proves
preservation of the source literals; it does not award a partial table match.
No caller provides a mask, relocation rewrite, invented label or target list.
Input code, initialized data and relocation entries remain untouched. The
existing explicit external-symbol normalization is the same symbol-only
operation used for ordinary linked function comparisons.

The real pinned GNU linker places the complete generated section at the
original address and resolves the actual local label relocations. Output
geometry and every resulting local pointer are checked again. The entire linked
section is returned, with its complete original and actual hashes and a full
byte-equality result. Neither pointers nor literal bytes are masked in that
comparison. The verifier's `identical` result requires both complete code
equality and complete equality of every generated readonly section. A candidate
with equal code but a differing table remains reconstructed and earns no match.
The public report records the separate code and data outcomes. The hybrid build
also reopens the genuine linked ELF and checks every complete table against the
original before allowing any code substitution; a forged code-only match or
altered table fails. Differing reconstructed candidates are excluded entirely.

The initial source candidate is unchanged public newlib 1.8.1
[`vfscanf.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/vfscanf.c).
Its full scanner at `0x00395558..0x00396068` occupies **2,832 bytes**; the adjacent
complete `__sccl` helper occupies 240 bytes at `0x00396068..0x00396158`.
The genuine supported `-DMB_CAPABLE` source configuration agrees with the
original format-decoder calls. Both available historical compilers reject
`-fno-jump-tables`; no unsupported flag, source-body edit or backend patch is
used to avoid the generated tables.

The original scanner pool at `0x00456230..0x004566A0` is 1,136 bytes: seventeen
`basefix` shorts, alignment and four independently decoded switch tables.
The format table has 121 entries; conversion dispatch has five; integer scanning
has 78; floating scanning has 59. The authentic GCC 2.9 object has exactly 263
actual local `R_MIPS_32` entries at those same locations. All remaining 84 bytes
already equal the original. Its actual full link emits 2,784 code bytes and a
different complete table, so this candidate is explicitly nonmatching.
This source-identity evidence and successful link do not establish the original
compiler or a runtime table match.

Sixteen dedicated tests use synthetic ELF instructions and data, including
actual pinned GNU assembler/linker runs. They cover whole-section and literal
proofs, real relocation association and addends, cross-function and padding
escapes, readonly geometry/overlap/unused mappings, original code pointer guards,
complete code-plus-data matching, and independent hybrid rejection of altered
data even when a report claims equality. Existing strict readonly and NOBITS
tests remain separate regression gates. Both scanner functions are registered
as reconstructed after independent full-body, source and dependency review;
the unchanged imports and their proof are documented in [reuse.md](reuse.md).
