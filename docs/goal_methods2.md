# Goal state updates

This batch recovers 22 completion/update/restore methods from the script goal
dispatch tables and two movement-state helpers. Its 24 routines cover 3,472
original instruction bytes. Nine functions reproduce all 752 of their bytes:

| Address | Observed behavior | Exact compiler profile | Bytes |
| --- | --- | --- | --- |
| `001D99F8` | Wait for entity state to change, then advance action timer | GCC 2.9 | 136 |
| `001D9B78` | Corresponding timer with a different entity-state condition | GCC 2.9 | 136 |
| `001E0FA0` | Timer gated by entity word +2D8 | GCC 2.9 | 136 |
| `001E1188` | Corresponding timer for another entity-state condition | GCC 2.9 | 136 |
| `001DC6B8` | Restore the entity word saved by the mood initializer | GCC 3.2.3 | 20 |
| `001E1F20` | Store and return completion when entity state changes | GCC 2.9 | 40 |
| `001E8900` | Resolve a resource and compare its entity state | GCC 2.9 | 44 |
| `001E8DE0` | Invoke an optional vehicle virtual operation | GCC 3.2.3 | 60 |
| `001E8EC0` | Restore owner flag bit 0x40 from the saved goal word | GCC 2.9 | 44 |

Each match compares the complete linked function at its original address, with
an exact symbol length and every relocation resolved. Alignment padding and
dispatch-table data do not count as recovered C. The other 15 functions are
reviewed reconstructions. The small restart predicate belongs to this batch's
goal lifecycle; its 24 bytes are not a separate subsystem accomplishment.

The four action timers have two distinct phases. Phase zero waits until the
selected entity state changes and then sets phase one. It does not subtract
time in the same call. Phase one subtracts owner float +60 from the remaining
scalar and always stores the result. Completion requires a strictly negative
result; zero and unordered comparisons remain active. Completion also clears
owner flag bit 8. Unknown phases preserve the state. The wait goal instead
tests strict positivity before subtracting and reports completion on the next
call after an overshoot. These differences are explicit in the C source.

Look, drive, intersection walking, road walking, and entering a vehicle use the
older compiler's eight-byte member-pointer encoding. A zero signed selector
skips invocation; a negative selector uses a direct function pointer. A
positive selector reads a virtual-table pointer through a signed field offset,
then loads an eight-byte adjusted-this/function pair. Both adjustments are
signed halfwords, but their sum remains 32 bits. The generated draft narrowed
that sum to a halfword, which was corrected from the original `addu`.

State indices are captured before dispatch for look, walking, and entering.
Drive reloads its state byte before the final adjustment lookup. Drive first
calls a resource predicate, updates a status word and increments the original
global counter only on a false-to-true transition. It then calls three helpers
in their observed order before invoking the selected member. Captured objects,
status reads after callbacks, and owner reloads after angle calculation are
preserved.

The facing and walking helpers calculate horizontal direction. Squared length
above the exact float `3727C5AC` scales X/Z by the reciprocal of squared length,
then passes Z and X to the retained angle helper. The facing update first uses
the exact threshold `3C23D70A`, excludes entity states 3 and 4, and builds its
temporary vector in X, Z, Y store order. It writes the angle and `40C90FDB`
(two pi) only when the reloaded owner's control word is zero. These constants
were derived from instruction words; no approximate replacements are used.

The route-selection helper tries the existing road word before the existing
intersection word. The intersection test reads only the low byte of another
word. Its two fallback callbacks either replace the road word from result
offset +40 or replace the intersection word from result offset zero. The final
callback receives a byte pointer and can change only that byte while retaining
the word's other bytes. Callback-dependent loads and writes preserve aliases.

Idle uses signed state and mode bytes, four-word inline records, seven integer
arguments and an independent float argument. Its record-address shift wraps at
32 bits, while counters and remainders use their observed signed comparisons.
Only positive repetition counts are decremented. Unknown modes still reset
the state; unknown states leave it alone. The random callback runs before the
denominator is reloaded. Modulo paths require a valid nonzero count, including
after callbacks; retail emits a divide-by-zero break for an invalid count.

The stop-vehicle predicate converts a captured vehicle scalar to a software
double, takes its absolute value through retained helpers, and compares the
complete 64-bit representation against double one. Its final virtual call
receives float zero through `f12`. The draft's 32-bit software-double values and
inferred integer argument were corrected by reviewing the full-register
operations and callee entries. The interaction-position update passes an
uninitialized stack vector as output; the callee writes exactly three floats.

Primary goal-table proofs cover the eight-byte header and six eight-byte
entries, ending at offset +38. Adjacent `bad_alloc` strings and exception/runtime
tables are excluded from gameplay bindings. The unchanged constructor and
script associations are described in [goals.md](goals.md) and
[script_gameplay.md](script_gameplay.md).

Both candidate compiler profiles and the guarded GCC 2.9 save-width annotation
were checked once. Timer matches came from the observed switch/XOR source
shape. The optional annotation produces the correct `sq`/`lq` widths in two
near matches, but different stack slots remain: the voice predicate differs
in four bytes and the interaction-position update in six. Neither is counted
as matched. No inline assembly, compiler-backend change, byte masking, or broad
flag sweep was used.

Root instruction review passed every nonmatching body. A second reviewer
independently checked the exceptional member, idle, route, stack-output, and
software-double paths. A native 32-bit harness passed 127 checks covering direct,
zero and virtual member selectors, a 65,534-byte adjustment sum, signed bytes,
timer zero/negative/NaN boundaries and overlapping flag/scalar storage, callback
mutations and owner reloads, record arguments, signed counter wrap, route
fallbacks, and complete software-double values. This supports semantic recovery;
the target byte comparison remains the only match gate.
