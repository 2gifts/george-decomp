# Script goal behavior and cleanup

This batch recovers 28 behavior/destruction methods reached through the original
goal dispatch tables, plus the shared referenced-object release and position
resolver used by the constructors. The 30 routines contain 2,916 original
instruction bytes. Four routines reproduce all 156 of their original bytes:

| Address | Observed behavior | Exact compiler profile | Bytes |
| --- | --- | --- | --- |
| `001DC5F8` | Decrement a halfword counter while its enable halfword is nonzero | GCC 3.2.3 | 32 |
| `001DC698` | Save the previous entity word and replace it with the goal's value | GCC 3.2.3 | 32 |
| `001E1FE8` | Invoke the stored Deimos callable with zero count and offset | GCC 3.2.3 | 36 |
| `001E8E88` | Save owner flag bit 0x40 and apply the requested flag state | GCC 2.9 | 56 |

Each match checks complete linked code, exact function-symbol size, and resolved
relocations at the original address. Alignment padding, original dispatch-table
data, and assembly retained elsewhere do not contribute to these C totals. The
other 26 routines are reviewed reconstructions. Both candidate compiler profiles
and the supported GCC 2.9 save-width annotation were tried once for this batch;
their unresolved code differences are recorded in the manifest.

The recovered methods connect several previously separate paths. Four action
initializers gate their state changes on a nonzero entity-operation result,
then copy the configured scalar, set owner flag bit 8, and clear a status byte.
Their destructors clear that owner bit before shared base destruction. Idle
cleanup invokes the entity operation before base cleanup. Script-call cleanup
releases the Deimos node retained by its constructor and clears the pointer only
after release returns. Set-position and enter-vehicle cleanup similarly release
their referenced objects before clearing each stored pointer.

The shared referenced-object helper decrements byte +5 modulo 256. A zero
result sets byte +6 bit 4, performs the observed destructor call, and returns
the object to allocator `0045C640`. This establishes the counterpart to the
constructor's byte increment without assigning an original class name. The
position resolver dispatches on signed byte +4: types 1, 2, 3, and 5 select an
offset pointer, an adjusted virtual method, an inline vector, or a separate
entity helper. Other values return null.

Position output methods preserve individual read/store order when storage
overlaps. Set-position first writes the output tag and reference word, then
reloads the goal's reference. An unresolved referenced object leaves the output
vector untouched. The home-position method clears the output reference before
setting its tag and loading the owner. The interaction-position method clears
the reference after its three vector stores. These order differences are
explicit in the source and instruction evidence.

Look-around snapshots the current entity angle, adds two converted offsets,
and applies at most one wrap to each result. Comparisons are strict and retain
the original unordered behavior. The exact conversion factor is
`0.01745329424738884f`, bits `3C8EFA36`. Every embedded float literal in this and
the constructor batch was audited against the original instructions:

| Use | Original bits |
| --- | --- |
| Look offset factor | `3C8EFA36` |
| Pi, negative pi, two pi | `40490FDB`, `C0490FDB`, `40C90FDB` |
| Random remainder scale | `38D1B717` |
| Stunned-operation scalar 5 | `40A00000` |
| Constructor timer bound 1e9 | `4E6E6B28` |
| Unit, half, three-quarter | `3F800000`, `3F000000`, `3F400000` |

Walk initialization captures the entity vector, calls the observed road/context
resolver, and applies the original result-specific state writes. Intersection
initialization reads a float from a 0x60-byte road record indexed by the owner's
low halfword, then resets selected owner words and unit-valued scalar. Road
initialization delegates the success path to a separate helper. The random
method uses signed remainder modulo 10001 before converting and scaling.

The exit-vehicle loop refreshes its count through an adjusted virtual call on
each iteration. Its counter remains 32 bits with wrap; only the indexed method
argument narrows to signed 8 bits. The decompiler draft's 8-bit loop counter was
corrected by reviewing the `addiu`, signed `slt`, and argument sign extension.
The retained virtual ABI stores a signed 16-bit this adjustment followed by a
function pointer. Source address arithmetic explicitly preserves 32-bit wrap.
Unused temporary registers are excluded from inferred call signatures.

The drive destructor `001DC820`, idle selector `001E06D8`, and vehicle predicate
`001E8988` remain assembly for a later reviewed batch. No no-op table entries
were added to inflate the recovered count. Partial layouts and constructor
bindings are shared with [goals.md](goals.md), and the canonical callable
prototype comes from `deimos_calls.h`.

Independent instruction and partial-layout review passed the six exceptional
paths: angle initialization, intersection state, aliased set-position output,
vehicle iteration, referenced-object release, and tagged vector resolution.
