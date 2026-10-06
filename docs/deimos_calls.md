# Script calls and deferred records

Nine routines in `src/game/deimos_calls.c` reconstruct 1,320 original bytes.
They compile and link with explicit original addresses. No source identity
was established for these game routines; shared tagged values, pool nodes,
frames, callables and generic-map types are reused from prior recovery.
The exact compiler comparison still reports differences.

`002CCC58` pushes a script frame, advances the register-window base by the
supplied offset, updates the available-register count, and computes an optional
destination relative to the previous base. A callable with a native function
pointer receives argument count and destination while the global argument
count is temporarily replaced. Otherwise its bytecode pointer is dispatched
to the existing interpreter. After either path, the saved register base and
window are restored and the current frame depth is decremented. Reading the
depth again after the call preserves nested-call behavior. Other global
changes made by a callback are retained. `002CDF18` obtains the callable from
the global map; `002D0258` and `002D0280` provide zero-offset call adapters.

The ring representation has count, read cursor, write cursor, element size,
capacity, begin and end at consecutive four-byte offsets. `002AF3F0` and
`002AF470` reject empty/full operations respectively, copy an element with
the identified memcpy, then reload count and cursors before changing them.
They compare cursor addresses unsigned and wrap to begin when the new cursor
reaches or exceeds end. Reloads preserve aliased copy effects.

A deferred record is 88 bytes: signed count, ten eight-byte values, and a
callable pointer at offset `0x54`. `002D02A8` returns when the queue is full;
otherwise it copies arguments, retains payloads with tags 3, 4 or 5, retains
the callable, and enqueues the record using the global queue reloaded after
those calls. Unused record values remain uninitialized as in the original.
The argument source is reloaded during each iteration. The retail routine
does not guard argument counts; valid callers must fit the ten-value record.

`002CEA80` copies a record's values into the current argument window, invokes
its callable with destination -1, releases the callable, clears its pointer,
then releases referenced argument payloads. Count and value reads after calls
are preserved. `002D03A0` repeatedly pops and executes records, reloading the
global queue after execution so callbacks can affect later iterations.

Frame depth, register offsets, record counts and ring state must remain in
the valid bounded domain used by callers. The reconstruction adds no new
guards or failure recovery. Full interpreter execution and the EE memory
model have not been tested.

An independent instruction review found no semantic defects in these nine
routines. An ignored native 32-bit harness passed 36 checks for recursive
native calls, register-window/base/depth/argument-count restoration, bytecode
dispatch, reference callbacks replacing source windows and queues, queues
becoming full after references are acquired, record/count mutations during
execution and release, queue replacement while draining, and memcpy changing
ring state before the subsequent updates. These checks do not claim EE runtime
execution or a complete interpreter build.
