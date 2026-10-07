# Numeric vector transforms

Two complete entries recover 240 original bytes / 60 instructions. Their full
raw and assembly intervals, terminal returns and excluded four-byte alignment
padding are proved by the isolated `build/vector_transform` packet. The scope
has 204 genuine encoded incoming references with complete containing-caller
identities. It does not award those callers or imply original class names.

| Entry | Bytes | Observed output |
| --- | ---: | --- |
| `002A1C60` | 124 | Three-component basis product plus fourth-row translation. |
| `002A1D78` | 116 | Three-component basis product, excluding translation. |

Both original bodies capture input Z, Y, X, pack vector lanes through the real
MMI/quad transfers, load complete 16-byte matrix rows, execute broadcast VU
accumulator operations and publish exactly Z, X, Y. All required source values
and results precede the first store, including shifted input, matrix and output
overlaps. No fourth output word is written. Production C adds no normalization,
pointer, capacity or other guard.

## Reuse and implementation limits

The batch uses the existing `GeorgeMathVec3`, `GeorgeMathVec4` and
`GeorgeRotationMatrix` layouts. Its product and left-associated addition shape
reuses the previously reviewed scalar accumulator expression. The strict tracer
inherits the existing record collision VU/MMI decoder unchanged and executes
both entire original bodies; no helper call or numerical substitution is needed.
The native executable compiles the new actual source as a separate translation
unit, rather than substituting its implementation in the harness.

The independently expressed scalar arithmetic and decoder provide a bounded
finite normal/zero binary32 model. Each product rounds before its accumulator
addition. They do not establish VU extended ACC precision, EE FCR flags,
nonfinite/subnormal behavior, timing or identical register effects. The original
also reads and computes unused fourth lanes; ordinary C computes only the three
published components, so their access-fault and arithmetic exception effects
are outside the model. Valid aligned matrix storage is assumed; the point entry
reads a 64-byte original matrix prefix, and the direction entry reads 48 bytes.

[PCSX2's pinned VU interpreter](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/VUops.cpp)
is read-only evidence for broadcast operands and the distinction between ACC
updates and result-vector writes. The 145,702-byte source has SHA256
`37d5098321e062956880637e90b6a975a8f28d88092a2f333f8f0d2e3750bd16`.
No GPL-3.0+ emulator implementation is copied, and this evidence is not a proof
of complete hardware rounding or flag semantics.

Reviewed published callers consume output memory and ignore `f0`; the original
incidentally leaves output Z there. The void source interface does not promise
that register effect. A bounded linear scan of all 204 incoming calls finds no
direct `f0` read before its first overwrite or control boundary. Forty-three
windows stop at a conditional or caller boundary and remain incomplete
control-flow proofs. Complete raw/assembly caller identities do not constitute
manual semantic review of all 204 callers.

## Validation

Six natural whole-function compiler/link comparisons use GCC 3.2.3, GCC 2.9 and
the genuine GCC 2.9 optional SAVE128 profile. The leaf routines need no such
save annotation. Every link has zero unresolved relocations and zero exact
candidates. Scalar compiled sizes are 148 and 124 bytes, compared with the
retail 124 and 116 bytes. Both functions remain **reconstructed**; no assembly
or intrinsic is used to force a match. Actual target data objects from both
compilers independently prove nine width/layout words.

The original tracer produces 2,060 synthetic fixtures / 61,800 executed original
instructions / maximum 31. The warning-free native harness passes 136,999
checks, comparing all 64 arena words per fixture. Cases cover signed zero,
separately rounded cancellation, nonsymmetric matrices, arbitrary fourth lanes,
shifted matrix/input/output overlap and fourth-output preservation. Independent
known point/direction and in-place assertions supplement the fixture replay.
Six strict decoder guards cover complete entries, initialized aligned memory,
quad lane packing/extraction, accumulator production and preservation, finite
domains, encoded delay control and actual `JR31` returns. These are bounded
synthetic checks, not exhaustive engine or PS2 hardware validation.

The author read both complete originals, source/header and reused decoder.
Independent peer and parent complete selected/source/proof review and fresh whole-packet/native reproduction passed. Complete supporting
original reads from the prior record collision review remain supporting
evidence; no duplicate supporting-function award is claimed here.

From the repository root:

```powershell
.venv/Scripts/python.exe build/vector_transform/scope.py
.venv/Scripts/python.exe build/vector_transform/abi.py
.venv/Scripts/python.exe build/vector_transform/probe.py
.venv/Scripts/python.exe tools/trace_vector_transform.py --output build/vector_transform/trace.json
.venv/Scripts/python.exe tests/native/run_vector_transform.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_vector_transform.py
.venv/Scripts/python.exe build/vector_transform/finalize_draft.py
```

The synthetic golden header contains no original instructions, tables or assets.
Intentional regeneration adds `--golden-header
tests/native/vector_transform_golden.h` to the trace command. The freeze records
owned sources, reused headers and decoder dependencies plus isolated proof
inputs. Central registration and global verification remain parent-coordinated.

Central-only six full candidate comparisons, canonical verification and the retail-identical hybrid build passed. Both functions remain reconstructed with zero exact awards. The historical initial 36-input freeze is preserved alongside the final freeze: only the actual-original alias guard gained an exact-file-presence skip. All six methods execute locally; without the private executable, five synthetic methods pass and that one method skips. All 35 other input fingerprints remain unchanged. The independent parent and peer also proved that simulated missing-original behavior without moving or deleting the game.
