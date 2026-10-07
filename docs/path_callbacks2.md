# Remaining table-backed curve callbacks

Five complete original entries add 692 bytes / 173 instructions of ordinary C reconstruction. They remain **reconstructed**, with no exact-byte awards: all fifteen genuine linked comparisons under GCC 3.2.3, GCC 2.9 and guarded GCC 2.9 SAVE128 resolve completely but differ from retail.

| Entry | Complete bytes | Observed behavior |
|---|---:|---|
| `002C19F0` | 224 | Four-vector cubic blend with two span-scaled control coefficients |
| `002C1CA0` | 188 | Cubic blend over four endpoint/control vectors |
| `002C1D60` | 192 | First derivative of the latter four-vector blend |
| `002C2180` | 44 | Forward quaternion pointers at endpoint +4 to the published interpolation helper |
| `002C2390` | 44 | Same complete wrapper with endpoint quaternion offset +0x18 |

The original observed 216-byte prefix at `00448518` proves six concrete nonnull cells for these five entries: kind 5 and 13 share `002C19F0`, kind 9 contains `002C1CA0`/`002C1D60`, and kinds 2/8 contain the quaternion wrappers. The complete 72-byte projection prefix at `004485F0` and all four actual getter materializations are retained as existing dispatch proof. These are observed prefixes, without an invented format capacity or bound check. There are zero encoded direct incoming J/JAL references to the five callbacks. Complete selected bodies, preceding-body geometry, terminal JR delay slots and zero alignment padding were independently checked against the validated original ELF before scope approval. The 28-byte scalar copy and true no-op are excluded.

All three polynomials read every input component before the first result store. The C helper computes Z, X and Y results into local values, then stores Z/X/Y; it does not turn those bodies into the earlier callbacks' per-component fresh-load loops. Captured endpoint times, exact binary32 one-third, separately rounded coefficient order, original endpoint offsets and unbounded time inputs remain explicit. Each component follows the original `MULA.S`, `MADDA.S`, `MADDA.S`, `MADD.S` order. The wrappers share an ordinary source macro after complete body comparison establishes that only their pointer offsets differ. Their call executes the existing `rotation.c` production source in a separate translation unit; callback-mutated endpoint data stays live. Their inherited f0 register contents are ignored under the reviewed dispatch call contract.

The accumulator decoder and C arithmetic are independently expressed as a bounded finite-normal/zero binary32 product/add model. Original MULA initializes ACC, MADDA updates ACC, and final MADD writes its destination without changing ACC. A cancellation guard distinguishes separately rounded product/add from a fused or postponed-double expression. [PCSX2's pinned FPU implementation](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/FPU.cpp) supplies read-only operation-order evidence; its full source SHA256 is `204dcb290f9f56e9112622ab313ef12d2192eda5b43d24efcf2dbc8ca3fa7344`. No GPL-3.0+ emulator implementation is copied. This evidence does not establish complete EE accumulator precision, FCR flags, exceptional/subnormal arithmetic, timing or original stack placement. Those remain implementation limits, and no exact match is claimed.

The strict tracer fetches complete selected/helper instructions from the locally supplied validated ELF. It rejects unreviewed entries/instructions, reserved accumulator destinations, uninitialized ACC/memory, unsupported calls, misaligned or out-of-range memory, nonfinite/subnormal accumulator arithmetic, cross-body branches and every encoded control instruction in a delay slot regardless of whether that inner branch would be taken. The reviewed earlier callback decoder, shared control-transfer classifier and real quaternion instruction body are reused; trigonometric calls are explicit finite observation/mutation contracts rather than a claim about retail approximation.

The 1,512 synthetic fixtures execute 114,120 original instructions (maximum 108 per fixture). They compare complete 128-word buffers, call observations and shifted endpoint/output overlap after actual selected and quaternion production C execute in separate translation units. Two wrapper formats exercise equal, opposite and orthogonal quaternion cases and controlled trigonometric mutation. The native runner passes **208,890 warning-free checks**, including eighteen independent endpoint/derivative assertions on integer-valued inputs. Six decoder guard methods pass. Native SSE scalar binary32 arithmetic excludes x87 excess precision; these fixtures make no whole EE, arbitrary format/capacity, heap or trigonometric accuracy claim.

```powershell
.venv/Scripts/python.exe tools/trace_path_callbacks2.py --golden-header tests/native/path_callbacks2_golden.h
.venv/Scripts/python.exe tests/native/run_path_callbacks2.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_path_callbacks2.py -v
.venv/Scripts/python.exe build/path_callbacks2/scope.py
.venv/Scripts/python.exe build/path_callbacks2/probe.py
.venv/Scripts/python.exe build/path_callbacks2/finalize_draft.py
.venv/Scripts/python.exe build/path_callbacks2/freeze.py
```

The private scope, draft manifest/symbols, three complete linked comparison reports, fixture identity, native command and frozen input fingerprints are under `build/path_callbacks2`. They contain no source award for the reused quaternion/trigonometric/getter bodies. The author read all 173 selected originals, complete production C/header/tracer/native/guards and the seven bounded supporting originals (quaternion, two trig routines and four getters), retaining previously reviewed helper contracts. Parent independently approved the complete source-free scope and six actual cells. Independent final peer and parent complete selected/source/proof review and fresh full-packet/native reproduction passed.

The producer freeze remains a historical author snapshot. This final publication note records subsequent central-only fifteen whole comparisons, canonical verifier/build and 379 reviewed tooling tests (one platform symlink skip). All five routines remain reconstructed; no new exact bytes are awarded.
