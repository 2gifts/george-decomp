# Native semantic checks

The `geometry_bounds` utility harness passes 8,738 checks including 134 finite
original-instruction synthetic fixtures, destructive affine inverse aliases,
inclusive/unordered comparisons, signed count gates, wrapped allocation/address
math, closest-output aliases and captured dimensions across mutable callbacks.
Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness geometry_bounds`.

Run the 32-bit goal state-update harness from the repository root:

```powershell
.venv/Scripts/python.exe tests/native/run_goal_methods2.py
```

The default native compiler is MinGW GCC supplied by the candidate PS2 toolchain
bundle. Pass `--compiler PATH` to use another native GCC with 32-bit support.
The harness compiles recovered C directly and uses callback stubs with
instruction-derived observable expectations. It requires no ISO, executable,
original bytes, or generated assembly. The 127 checks cover member dispatch,
timers and overlapping storage, callback mutations, route fallback ordering,
idle call arguments and signed arithmetic, stack vector outputs, and complete
software-double values.

Native tests support semantic review. Matching still requires the separate
target compiler/linker and complete comparison against the original code.

Run the heap, list, fixed-slot pool, interpolation, interpreter and tree harnesses with:

```powershell
.venv/Scripts/python.exe tests/native/run_utilities.py
```

Use `--harness NAME` for a single batch; omit it to run them all.
The pool checks cover free-index order, borrowed/owned storage, freeze/restore,
callback changes, wrap and endpoint behavior. The interpolation checks cover
endpoints, intervals, overlapping output and untouched output when no interval
is found. Native float tests use the host IEEE model; they do not independently
establish the EE hardware's treatment of nonfinite encodings.

The heap and list harnesses cover coalescing/allocation/callback behavior and
intrusive sentinel/link operations. The interpreter covers every opcode and
callback mutations; tree checks cover sorted membership, recursive destruction
and captured successor behavior. These tests compile the actual recovered
sources, including the licensed AROS adapter for list operations.

The tree update harness uses an explicit native Count-register substitute for
its 610 algorithm checks, including deadlines, callback gates, context changes,
captured successors and wrapped cycle arithmetic. It does not model physical
CP0 timing. Run it with `--harness tree_updates`; the runner uses `tree_walk.exe`
to avoid Windows treating an executable name containing `update` as an installer.

The input-state harness covers controller edges/history, consumed axis flags,
shifted scalar aliases, disabled updates, allocation and field preservation,
repeat endpoints and all 256 key-byte encodings. Run it with
`--harness input_state`; its 2,475 checks use synthetic input values.

The vector-math harness passes 77 checks for vector/quaternion arithmetic,
thresholds, zero/antipodal fallbacks, callback captures and shifted aliases.
Run it with `--harness vector_math`. Its square-root fallback tests host
arithmetic, without claiming full EE floating-point or FCR behavior.

The pad-input harness checks startup aliases, all 65,536 raw button patterns,
stick deadzones, output aliases, signed pressure, connection gates, rumble and
callback-dependent state reloads. Run it with `--harness pad_input`; its 610,739
checks include callee argument/sequence assertions. Pad and angle callees are
controlled substitutes, and native square root is a finite host arithmetic model.

The pad-device harness passes 4,047 checks for connection-state transitions,
packet/button/axis/pressure decoding, repeated SDK queries, field preservation,
callbacks and shifted outputs. Run it with `--harness pad_device`. Its SDK calls
are controlled substitutes; original paths reading an uninitialized local
packet are documented and excluded from deterministic fixtures.

The geometry harness passes 783 checks for perspective-plane construction,
zero extents and shifted frame/output aliases. Run it with `--harness geometry`.
Its 13 synthetic golden fixtures reproduce from `tools/trace_geometry.py`, a
bounded finite host-IEEE instruction reference that reads the local validated
ELF. See `docs/geometry.md` for regeneration and its EE precision/FCR limits.

Run `tests/native/run_goal_methods4.py` for the drive and traffic batch's 383
checks. They cover callback mutations, signed route bytes, queue budget reloads,
geometry aliases and motion calls. See `docs/goal_methods4.md` for the recovered
state contracts and remaining platform limitations.

The rotation harness passes 2,530 checks for quaternion arithmetic, shifted
aliases, callback mutations, trig/store ordering and retail fallback behavior.
Run it with `--harness rotation`. Its 74 finite synthetic alias fixtures
reproduce from `tools/trace_rotation.py`; see `docs/rotation.md` for the bounded
reference and EE precision/FCR limits.

The scalar matrix harness passes 3,245 checks for callback captures, shifted
input/output aliases, live position reloads and axis builders. Run it with
`--harness matrix_scalar`. Sixty synthetic instruction-derived fixtures check
all initialized words; regeneration and limits are in `docs/matrix_scalar.md`.

Run `tests/native/run_goal_methods5.py` for the route/reference batch's 169
checks, including persistent scoring flags, signed route modes, NaN timers,
wide member adjustments and callback mutations. See `docs/goal_methods5.md`.

The byte-order harness passes 146,760 checks for exhaustive halfwords, raw word
encodings, finite float bit conversion, fixed counts, untouched storage, shifted
forward overlaps and wrapped products. Run it with `--harness byte_order`.
Float argument/return and float-store tests avoid nonfinite encodings because
the native x87 ABI can quiet signaling NaNs. See `docs/byte_order.md`.

The cache-transfer harness passes 12,540 checks for key hits/holes, every cursor
slot, strict unsigned overlap endpoints, zero lengths, wrapped rounding and
callback-mutated globals. Run it with `--harness cache_transfer`. Its controlled
copy substitute records the original numeric destination and argument sequence
without emulating or dereferencing physical memory. See `docs/cache_transfer.md`.

The text-token harness passes 22,855 checks for quote/delimiter behavior, leading
bounds, high bytes, counter wrap, complete-buffer aliases, shared pointer cells
and callback-mutated delimiter pointers/source/counter. Run it with
`--harness text_tokens`. Its 172 synthetic byte fixtures regenerate through
`tools/trace_text_tokens.py`; controlled length/compare substitutes do not model
the original vectorized library. See `docs/text_tokens.md` for buffer limits.

The text-lookup harness passes 66,408 checks for sparse initialization, untouched
context bytes, sentinel/code reloads and table aliases, valid signed ctype indices,
ordered duplicate/fallback value scans, callback mutation and output aliases.
Run it with `--harness text_lookup`. It links the already reviewed token helper
and unchanged licensed newlib character table; compare/copy use controlled ABI
substitutes. See `docs/text_lookup.md` for capacities and signed-index limits.

The typed-text harness passes 454 checks for all lookup precedences, sequential
truth comparisons and output mutations, vector scanner pointer ABI/ignored count,
optional lowercase/copy order, initial output aliases, wrapped depth accumulation
and fresh EOF/count gates. Run it with `--harness text_values`. The scanner
substitute writes every lane; unwritten lanes from real partial conversion remain
outside defined C semantics. See `docs/text_values.md` for limits and reuse proof.
## Identical game algorithm bodies

`run_utilities.py --harness algorithm_duplicates` compiles all 61 recovered
entries using shared ordinary C definitions. It passes 2,375,547 checks against
an independent sorted-range oracle, callback mutations, map lookup/visitor
effects and gated action reload/overlap cases. The 58 search entries share one
algorithm; repeated checks do not imply 58 distinct gameplay behaviors.
