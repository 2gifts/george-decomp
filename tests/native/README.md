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
