# Connected actor state 15–19 methods

This batch reconstructs 21 complete functions covering 5,908 bytes of original
text. All remain `reconstructed`: the 63 complete individual compiler/link
comparisons have zero remaining relocations and zero exact matches. Numeric
field/function names retain the uncertainty about original classes.

The handlers reuse observed actor/control layouts, signed member adjustments,
the existing request, script-value, vector-normalization and matrix APIs, and
the reviewed upper-bound registry algorithm. The two timed-request handlers,
four duration callbacks and four cancellation methods use C templates after
their complete original bodies were compared. No empty methods are included.

State15 preserves its seven implemented phases, callback-visible script
tag/payload/subtype store orders, effect registration and cleanup. Its position
copy captures all three components before an unaligned64-bit plus32-bit copy
at handle+0x14; movement then reloads the potentially aliased actor fields.
The initializer repeats the original angle calls, with captured operands
and strict/unordered wrapping decisions.

State16 preserves notification ordering and callback-mutated timer reloads.
The original branch-likely store in phase102 executes only when field7C4 is
nonzero. Independent review caught an incorrect unconditional store in the
draft; the final source retains the full32-bit gate, with zero/nonzero/high-bit
regression cases. State18 retains full signed command selection before wrapped
index arithmetic, fresh primary loads across array calls, captured matrix
inputs, and the different captured/fresh attachment behavior of its two paths.
Its translation Z is read before the W store, including callback-induced aliasing.

The author and parent independently reviewed all 1,477 original instructions
and the complete C/header. The parent also audited all 21 original hashes and
boundaries including return delay slots, 11 actual state members from the
previously proved42-record table, nine encoded direct calls, nine callback
materializations, all 12 binary32 constants and symbol bindings. It reproduced
all 63 fresh compiler/link results and all 1,621 native checks after the fixes;
no remaining defects were found. Five duration callbacks differ by only four
bytes under GCC2.9 SAVE128, entirely callee-save slot offsets, and receive no
matching credit.

Run the asset-free native harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_states3.py
```

Its checks cover full signed phase boundaries, wrapping counters, callback
mutations, captured gates versus fresh request objects, strict and unordered
timer tests, registry insertion paths, unaligned position aliasing, matrix
record scans and attachment cleanup. Native arithmetic/callee mocks do not
assert complete EE nonfinite arithmetic or FCR status behavior. Original
instruction files and the retail executable are unnecessary to run the tests.
