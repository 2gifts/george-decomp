# Record collision queries

Two complete numeric entries recover 920 original bytes / 230 instructions.
The selected routines have four real encoded incoming JALs, complete raw and
assembly identities, final return delay slots and verified adjacent boundaries.
The historical isolated author packet is `build/record_collision`. Both selected
entries are now centrally registered after independent peer and parent review;
existing supporting functions receive no additional award.

| Entry | Original bytes | Behavior |
| --- | ---: | --- |
| `00270A00` | 656 | Transform a supplied segment, walk records and signed maps, test closest-segment squared distance against the fresh record radius, then publish the first hit's fraction, full record word and normalized residual. |
| `00270C90` | 264 | Zero output first, find the first full-word key, resolve its signed map and project the stored scalar midpoint through its basis; publish Z, X, Y. |

The incoming second GPR argument to `270A00` is the matrix passed as the live
second argument to `2A1098`; the original does not explicitly rewrite that GPR
before the call. Selector 3 becomes 2. Observed maps hold signed halfwords,
records have 20-byte stride and basis records have 64-byte stride. Partial types
describe these accesses without identifying original classes or proving format
capacities. Production adds no output-null, count, map or selector guards.

The hit path captures its six interpolated coordinates before publishing the
fraction. It loads `record+4` afterward, so an output alias into that word changes
the subsequent full-word output. The native tests specifically assert this
reload using a perpendicular crossing, as well as the original normalization's
zero-residual `(1,0,0)` fallback. The midpoint path's initial zero stores can
change a live selector, record pointer, count or key before their subsequent
reads; shifted alias fixtures exercise those cases.

## Reused source and supporting ABI

The native executable uses four actual production translation units:
`record_collision.c`, `matrix_rigid.c`, `vector_math.c` and the existing licensed
`segment_distance.c` adaptation. The supporting distance routine's full BSD
notice remains intact; this batch does not copy or re-award its implementation.
The compiler and native builds reuse the existing vector, matrix, square-root
and compiler-annotation headers.

The author read both complete selected originals and C/header, the six complete
inverse/point/direction/normalization/distance supporting original bodies, the
three small actual callers `1928F0`, `192658`, `1926D0`, and the bounded call
context in `17ADA8`. The latter's complete 1,140-byte raw/assembly/hash identity
is checked, without claiming a complete semantic review of that caller. The
four complete soft ABI entry originals and relevant unchanged `fp_bit.c`
conversion, comparison and subtraction paths were independently read in the
immediately preceding distance peer review; they receive no new source award.
Final independent peer and parent review remain pending.

`2A1C60` and `2A1D78` remain numeric production externs. Their complete 124- and
116-byte VU/MMI originals capture XYZ, pack four lanes, load full basis vectors,
execute broadcast accumulator operations and publish exactly Z/X/Y. The new
bounded decoder executes these entire original bodies, including lane packing,
quad transfers, upper-half extraction and instruction delay slots. The native
harness has explicit finite observation contracts for these two entries; it
does not claim they are recovered production helper implementations. Their
separate source-free scope is `build/vector_transform_scope`, with 204 actual
incoming references, for a potential later reviewed source batch.

The VU model rounds each product before each accumulator add and preserves ACC
when a result-producing MADD writes a vector. It is a finite normal/zero model;
it does not claim VU extended accumulator precision, EE exceptional/subnormal
behavior, FCR flags, timing or identical stack placement. Four soft-double calls
use the previously reviewed finite models and preserve full 64-bit operands and
signed zero. Supporting code and numerical models are not awarded new bytes.

## Validation

The author packet reproduces six natural complete compiler/link comparisons
under GCC 3.2.3, GCC 2.9 and the genuine GCC 2.9 SAVE128 annotation profile. All
links have zero unresolved relocations and zero exact candidates. The two
functions remain **reconstructed**. Actual compiler data objects independently
prove fourteen scalar, pointer, record, matrix and offset words under both
compiler versions.

The strict original decoder produces 718 fixtures / 170,738 executed original
instructions / maximum 942 instructions. The warning-free native harness passes
1,157,227 checks using the actual four source translation units. It checks all
1,408 memory words per fixture, canonicalizes only nine observed pointer fields,
checks used transform/soft-call operands, and adds independent midpoint and
crossing/reload assertions. Eight decoder guard methods cover whole entries,
initialized aligned memory, quad lane packing, ACC production and preservation,
product rounding, finite domains, signed maps, encoded control instructions in
delays, real return registers and transfer/instruction bounds. These synthetic
fixtures are not exhaustive whole-engine or hardware validation.

From the repository root:

```powershell
.venv/Scripts/python.exe build/record_collision/scope.py
.venv/Scripts/python.exe build/record_collision/abi.py
.venv/Scripts/python.exe build/record_collision/probe.py
.venv/Scripts/python.exe tools/trace_record_collision.py --output build/record_collision/trace.json
.venv/Scripts/python.exe tests/native/run_record_collision.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_record_collision.py
.venv/Scripts/python.exe build/record_collision/finalize_draft.py
```

The existing golden header contains only sparse synthetic memory and observed
outputs. Intentional regeneration adds `--golden-header
tests/native/record_collision_golden.h` to the trace command. No original code,
assets or original table data is exported. The freeze fingerprints source,
headers, reused dependencies, decoder/harness/tests and isolated proof inputs;
registration and global verification remain parent-coordinated.

Independent peer and parent complete selected/source/proof review and fresh nine-dictionary, six-whole-link reproduction passed. The 718-fixture native suite reproduced 1,157,227 warning-free checks and eight guards. Central-only six full comparisons, canonical verification and the retail-identical hybrid build passed; both entries remain reconstructed with zero exact awards. The historical author freeze is preserved.
