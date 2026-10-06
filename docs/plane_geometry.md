Seven complete connected geometry entries are reconstructed in `src/game/plane_geometry.c`, covering 1,788 original bytes and 447 instructions. Numeric names remain until stronger original identity evidence exists. All entries have concrete encoded incoming calls; the source-free scope packet records nineteen references and the complete original bytes, assembly intervals and boundaries of their containing callers.

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `0029C6C0` | 236 | Captured triangle cross product, genuine normalization, fresh plane offset and output pointer return |
| `0029C7B0` | 172 | Captured triangle cross product, genuine normalization and output pointer return |
| `0029CA28` | 248 | Normalized direction and radius-adjusted length/projection comparison |
| `0029CB20` | 560 | Three strict cross-product sign tests against a triangle normal |
| `0029D048` | 168 | Supplied-direction line projection, nullable parameter and squared residual |
| `0029E448` | 200 | Bounds growth with point reloads after lower writes |
| `0029E720` | 204 | Sphere/bounds distance comparison with squared radius |

The two normal entries share a C macro after complete original reading proves the same captured differences, last-minus-middle edge, cross product, Y/Z/X stores and normalization call. The four-component entry then rereads the first vertex after normalization, including overlapping output writes, and negates the complete dot product into W. Existing `GeorgeMathVec3`, `GeorgeMathVec4` and `GeorgeBounds` model only observed storage. The published normalization routine runs as a separate production translation unit in native checks and executes its real original instructions in the tracer. It receives no new recovery award.

The triangle test excludes edges through strict positive comparisons and retains no plane-distance test. The line routine uses the direction exactly as supplied, with an unbounded parameter. Bounds growth captures lower choices first, writes X/Y/Z, rereads the point and upper bounds, then writes upper X/Z/Y. Sphere testing retains the actual logical negation of a strict less comparison; it adds no radius sign or normalization guard. No original class, ownership or caller capacity is inferred.

The author read all 447 selected instructions, the complete 34-instruction normalization helper and its published source, and actual bounded argument/return windows at all nineteen incoming references. Full large caller identities are checked without claiming their complete semantic reconstruction. The proof packet records eighteen local branches, seventy-five stores including stack and delays, four direct calls, seven returns and two zero-register float transfers. The helper's two exact `0x3F800000` immediate chains are checked separately.

Strict fixtures execute all seven complete original bodies and real normalization, with no controlled numerical or callback substitute. There are 1,123 fixtures, 89,426 executed instructions and a maximum of 211 instructions per fixture. The native runner passes 165,110 warning-free checks; six synthetic decoder guard methods pass. Shifted aliases, captured inputs, fresh plane offset/bounds reads, zero-normal fallback, strict triangle boundaries, negative radius, nonunit direction and nullable parameter cases are covered. Independent closed-form axis line, triangle winding and sphere/bounds invariants supplement the instruction-derived memory and return comparisons.

Native compilation uses scalar SSE rounding to binary32 and host square root. Fixtures cover finite normal and zero values, with no EE exceptional/subnormal arithmetic, FCR, timing or original stack-placement claim. Unexpected trigonometric hooks from the separate vector translation unit fail explicitly. All twenty-one genuine whole-function compiler/link comparisons have zero unresolved relocations and zero exact matches; every selected entry remains reconstructed. Independent peer and parent reviews passed all 447 selected instructions, complete source/header/tracer/native/guard code and full proof dictionaries. Both independently reproduced all twenty-one complete natural links, fixtures, native checks and strict guards. Central-only binding reproduction and canonical verification retain zero exact matches.

Reproduce the isolated checks:

```powershell
.venv/Scripts/python.exe tools/trace_plane_geometry.py --golden-header tests/native/plane_geometry_golden.h
.venv/Scripts/python.exe tests/test_trace_plane_geometry.py
.venv/Scripts/python.exe tests/native/run_plane_geometry.py
.venv/Scripts/python.exe build/plane_geometry/scope.py
.venv/Scripts/python.exe build/plane_geometry/probe.py
.venv/Scripts/python.exe build/plane_geometry/finalize_draft.py
```

The ignored packet and fingerprint records are under `build/plane_geometry`. Four unproven adjacent entries remain excluded: `0029C860`, `0029CD50`, `0029E510` and `0029E5E0`. Original code, assets and table arrays are not copied into the fixture headers.

Canonical verification and the retail-identical hybrid build passed with 364 reviewed tooling tests (one platform-specific symlink skip). The producer's 31-file freeze remains the historical source/proof review snapshot; final documentation and registration annotations follow successful canonical verification.
