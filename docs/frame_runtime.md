# GNU frame runtime reuse

Fifteen complete functions cover 3,988 bytes / 997 original instructions. Nine
independently reproduced functions match all 2,380 bytes under the pinned
GNU EE GCC 2.9 default-builtin recipe and passed canonical verification.
The complete unchanged source is [frame.c](../src/runtime/gcc/frame.c), accompanied
by four unchanged historical headers. Three wrappers without a demonstrated
encoded entry remain deferred; their 128 bytes receive no recovery or matching credit.

| Entry | Source function | Bytes | Reviewed result |
| --- | --- | ---: | --- |
| `00374888` | `decode_uleb128` | 68 | Canonical whole-code match |
| `003748D0` | `decode_sleb128` | 80 | Canonical whole-code match |
| `00374920` | `fde_merge` | 252 | Canonical whole-code match |
| `00374A20` | `end_fde_sort` | 612 | Canonical whole-code match |
| `00374C88` | `count_fdes` | 72 | Canonical whole-code match |
| `00374CD0` | `add_fdes` | 136 | Canonical whole-code match |
| `00374D58` | `frame_init` | 316 | Canonical whole-code match |
| `00374E98` | `find_fde` | 228 | Reconstructed, whole link |
| `00374F80` | `extract_cie_info` | 260 | Canonical link blocked |
| `00375088` | `execute_cfa_insn` | 920 | Reconstructed |
| `00375420` | `__register_frame_info` | 40 | Reconstructed, whole link |
| `00375478` | `__register_frame_info_table` | 40 | Reconstructed, whole link |
| `003754D0` | `__deregister_frame_info` | 120 | Reconstructed, whole link |
| `00375568` | `__frame_state_for` | 472 | Canonical whole-code match |
| `00375740` | `fde_split` | 372 | Canonical whole-code match |

The source is pinned to public revision
`b595ded606227e93b8c4a447446c1d2ac093827d` of
[SSXModding/ps2-ee-toolchain](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc).
All five files are exact Git blobs. Each original GNU notice remains in its
file. `frame.c` carries the historical GCC linking exception reproduced in
[LICENSES/GCC-frame-exception.txt](../LICENSES/GCC-frame-exception.txt);
[LICENSES/GPL-2.0.txt](../LICENSES/GPL-2.0.txt) and the individual header notices
are retained. The project's tooling license does not replace those notices.
The machine-readable [provenance](../src/runtime/gcc/frame_provenance.json)
records every original source path and hash.

| Source | Git blob SHA-256 |
| --- | --- |
| `frame.c` | `6a1e53f3c256085ce5b198ef684b3ca083286a2369648d4b529ea096780b9a19` |
| `frame.h` | `aa0a90f0ac5b930fe92cf4d8d9095d0df7d9ecacee9f750e61f3f071149f3a93` |
| `dwarf2.h` | `5c34f81af2af292f1a9bc435c3109b5c3df280ef5c8b067e10674f32e93dd832` |
| `gthr.h` | `3047c4ff0059592b86f21e60108a4a230c63a338dd9a07e5203cafadc5ac1ef2` |
| `gthr-single.h` | `20e7c4ac1f4eb4b8ff71f7b87f9ca0ef2c450473b253006e31a12cf0f1fb5a08` |

Four transparent recipes retain GNU default builtin behavior or explicitly
add `-fno-builtin`, under the two pinned compilers. These are source-preserving
historical compilation choices, without a flag sweep. Default compilation
expands the original `memcpy` calls into the observed copy loops, including the
complete 472-byte `__frame_state_for`. All 60 comparison records remain in the
function evidence: 50 genuine whole links, two missing inlined source symbols,
two unsupported outlined heap-helper links, two incompatible GCC 3.2.3 decoder
pool geometries, and four mixed-pool `extract_cie_info` blockers. No fragment,
masked instruction, padded substitute or patched object earns a match.

The canonical verifier now supports fourteen selected links. `extract_cie_info`
remains honestly blocked by its mixed literal pool. Four global users resolve
through a separately reviewed original-code proof extension, using the complete
four-byte zero pointer at original NOBITS `0048F2B8`. Explicit `lw32`/`sw32`
effective-address proofs and an opt-in preserved sequence check only the
observed LUI with LW/SW or twelve-byte LUI/ADDIU patterns. The whitelist excludes
calls, control transfers, unknown instructions and base clobbers. Whole storage,
zero initialization, genuine GNU linking and full code comparison remain required.

The parent read all 107 instructions of those four originals and the complete
verifier/test/documentation diff. Parent and peer separately replayed all sixteen
whole links across the four recipes, with zero unresolved relocations and no
exact matches. Twenty-two focused NOBITS tests cover genuine synthetic GNU links
and rejection cases. No linker implementation changes were needed. The unused
mutex's initialized four-byte section is unreferenced and remains unmapped.

The decoder's whole 208-byte readonly pool at `004546A0` contains sixteen
literal/padding bytes, 47 real local case pointers and four trailing zero bytes.
Existing generated-readonly support links the entire compatible GCC 2.9 pool,
validating every literal and local target. Both code and complete linked data
differ. GCC 3.2.3's incompatible 200-byte section remains unmapped. The original
pool is evidence only; no game byte array is checked into the source.

All actual `-M` dependencies are hashed. Four genuine target-computed 31-word
ABI probes agree: int/pointer/size_t four bytes, long eight bytes,
`FIRST_PSEUDO_REGISTER` 79, frame state 752 bytes, saved-register offset
668 (`29C`), and internal frame state 760 bytes (`2F8`). Per-source numeric
macros bind the original engine allocator/free/strlen entries at `002AF140`,
`002AF1E8` and `00295050`; the general library aliases remain separately scoped.
`abort` binds to the actual nonreturn adapter at `00396250`. The original
`gthr.h` default selects unchanged `gthr-single.h`, with no emitted mutex calls.

Thirty-four actual encoded entry references carry complete containing-body
hash and raw/assembly geometry proof. Merged scanner regions at `00375088`
and `003754D0` are independently split at complete source counterparts,
terminal returns and following prologues. The selected inventory contains
117 local branches, 136 stores and 41 direct calls.

The author and independent disc reviewer read all 997 selected original
instructions, the complete five pinned files, proof scripts and supporting
allocator/string/abort bodies. The parent read all 595 instructions of the
nine matching functions and all five pinned files/scripts, then independently
reproduced eleven complete dictionaries, all 60 comparisons and four ABI
arrays. The parent also read all 107 original instructions of the four global
users. All 62 source/dependency inputs remained unchanged. For the other two
reconstructed routines, the parent validates complete raw geometry, control,
entry, ABI and data proof without claiming a third full semantic read.
Tracked source with only central bindings reproduces all 60 comparison records
and the complete source/data/ABI/dependency packets.

This retains upstream requirements for valid DWARF/CIE/FDE data, bounded LEB
encodings/register indices, valid heap/output objects and target pointer
comparison/subtraction domains. Original bit-test priority, including the
`0xC0` advance/offset tests, is preserved. No native unwinder, OS, thread safety,
malformed-input safety or EE hardware fidelity is claimed. Only complete
natural linked code and selected data equality earns matching credit.

Canonical verification and the hybrid build passed with 321 tooling tests
(one platform-specific symlink skip). The hybrid executable retains retail
SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`.
Remaining original assembly contributes no additional C matching credit.
