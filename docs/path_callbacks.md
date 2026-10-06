# Path callback formats

Fifteen complete routines cover 3,036 bytes and 759 original instructions. Their entries are established by seventeen actual cells in the observed path dispatch prefixes at `0x00448518` and `0x004485F0`, together with the four complete getter bodies and their table-address materializations. A complete text scan found no encoded direct incoming call to these fifteen entries. The 216-byte and 72-byte prefixes establish the accessed rows and columns; they do not prove a general table capacity or original class identity. No original table or instruction arrays are copied into production source.

| Entry | Bytes | Reconstructed behavior |
| --- | ---: | --- |
| `002C03E8` | 372 | Component cubic with fresh endpoint loads before each output store |
| `002C0560` | 380 | Component derivative; final Z expression overwrites output X |
| `002C0C18` | 360 | Packed signed-halfword quaternion decoding and shared interpolation |
| `002C19A8` | 68 | Endpoint selection at exactly fraction one, with sequential copies |
| `002C1AD0` | 220 | Three-component derivative loop with captured span coefficients |
| `002C1BB0` | 236 | Second derivative loop with the original six/twelve factors |
| `002C1E20` | 212 | Component derivative polynomial with fresh per-component inputs |
| `002C1EF8` | 84 | Captured XYZ interpolation, stores Z/X/Y |
| `002C1F50` | 60 | Captured XYZ difference, stores X/Z/Y |
| `002C1F90` | 248 | Signed-halfword quaternion wrapper |
| `002C2088` | 248 | Signed-byte quaternion wrapper |
| `002C21B0` | 168 | Quaternion interpolation followed by fresh vector interpolation |
| `002C2300` | 140 | Scalar polynomial |
| `002C23C8` | 120 | Projection using endpoint vectors at `+0x1C`, then time conversion |
| `002C2440` | 120 | Projection sibling using endpoint vectors at `+0x04` |

The two compressed wrappers share a normal C template after complete original bodies prove the same snapshot and call sequence, with signed width, step and scale retained as explicit differences. The two projection wrappers similarly share their complete reviewed access sequence. Component helpers retain fresh reads between output stores instead of snapshotting all input components. The existing `src/game/rotation.c` implementation of `002A2780` is called directly; native testing compiles it as a separate production translation unit. That reused helper receives no additional recovery or match award.

The unusual final store in `002C0560` is present in the original and remains unchanged. `002C21B0` reads its vector portion after the quaternion helper returns, including mutations made by controlled trigonometric callbacks or overlapping quaternion outputs. Projection wrappers store distance before rereading endpoint times and fraction. Inherited `f0` contents are ignored under the reviewed callback contract. Format storage remains opaque, with only actual numeric offsets modeled.

The author read all selected originals and eight complete supporting bodies: the quaternion helper, projection helper, two trigonometric entries and four getters. The proof packet independently checks full original hashes, assembly interval bytes, terminal return delays, zero alignment padding, preceding boundaries, four local branches, seventy decoded stores, six calls, fifteen returns and thirty-three immediate float sites. Seventeen distinct binary32 constants were checked, including the exact compressed scales `0x38000100` and `0x3C010204`. During draft verification, the author corrected three factor-three terms in `002C1BB0` to the original factor six. The final source was then recompiled and retested.

Strict fixtures execute all fifteen original bodies and the complete original quaternion helper. There are 1,536 authored fixtures, 133,933 executed instructions and a maximum of 187 instructions per fixture. The native runner passes 210,928 warning-free checks; eight decoder guard tests pass. Cases cover signed packed extremes, fraction-one boundaries, deterministic varied coefficients, shifted source/output aliases, untouched output Z, snapshot decoding, post-helper vector reloads and aliased projection/time outputs. Native compilation uses `-msse2 -mfpmath=sse` so each scalar float operation rounds to binary32; default x87 excess precision produced a distinguishable polynomial result immediately below fraction one.

Projection and trigonometric externs have explicit finite observation and mutation contracts. These checks do not establish the real library approximations, exceptional EE/FCR behavior, subnormal handling, cycle timing or original stack placement. All forty-five genuine whole-function compiler/link comparisons have zero unresolved relocations and zero exact matches. Every selected routine remains reconstructed. Independent peer and parent reviews passed: all selected original instructions, complete source/header, strict tracer/native/guard code and the full isolated proof packet were checked. Parent fresh reproduction confirms all forty-five complete natural links, fixtures, native checks and strict guards. Central-only binding reproduction and canonical verification retain zero exact matches.

Reproduce the isolated checks:

```powershell
.venv/Scripts/python.exe tools/trace_path_callbacks.py --golden-header tests/native/path_callbacks_golden.h
.venv/Scripts/python.exe tests/test_trace_path_callbacks.py
.venv/Scripts/python.exe tests/native/run_path_callbacks.py
.venv/Scripts/python.exe build/path_callbacks/probe.py
.venv/Scripts/python.exe build/path_callbacks/finalize_draft.py
```

The ignored scope, three compiler reports, complete manifest, symbols, fixture identity and frozen fingerprint records are under `build/path_callbacks`. The three accumulator-only neighboring polynomials, true no-op and unproven adjacent entries remain excluded from this batch.

Canonical verification and the retail-identical hybrid build passed with 364 reviewed tooling tests (one platform-specific symlink skip). The producer's 32-file freeze and earlier peer packet are retained as historical review snapshots; these final documentation and registration annotations are recorded separately after successful canonical verification.
