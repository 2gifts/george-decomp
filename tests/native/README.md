# Native semantic checks

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
