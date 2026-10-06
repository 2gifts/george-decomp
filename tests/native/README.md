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
