# Property callbacks, deferred destruction and transform caches

This batch reconstructs five complete scalar functions, 696 original bytes and
174 instructions, in `src/game/property_updates.c`:

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `002B9628` | 52 | Defer destruction during a callback, otherwise destroy the node |
| `002B9748` | 188 | Update one node, refresh its transform and recursively update children |
| `002B9898` | 144 | Refresh the cached transform or its translation according to flag bits |
| `002B9C08` | 216 | Update an intrusive list using the original inline node body |
| `002B9D28` | 96 | Defer or perform destruction for each node in an intrusive list |

The previously reviewed ownership prefix, recursive destructor, intrusive lists
and vector/matrix layouts are reused unchanged. The new header asserts callback
word size and the existing time/callback offsets. A private ordinary C template
shares the repeated deferred gate and inline update body without adding a call
to an engine function that the original outer loops do not call. All nine direct
numeric calls and both original indirect callbacks remain explicit.

The single-node entry stores the captured input time at +18 before loading the
+20 callback. The outer entry captures that callback before storing time. When
a callback exists, bit 0x10 is set before invoking it with the captured node;
afterward flags are freshly read and bit 0x10 is cleared. A fresh pending bit
0x20 selects deferred destruction or the existing recursive destructor. The
deferred gate preserves the complete captured 16-bit flags when setting 0x20.
It does not invent validity, ownership or reentrancy guards.

One actual +20 callable is established by the complete allocating caller
`00278EB0`: it constructs the prefix and stores `00278C48` at +20. The complete
612-byte callback captures a0 as its node and consumes no incoming float
register. This supports the declared one-node semantic callback contract. The
original source typedef, unused callback return and incidental incoming f12
remain uncertain. In particular, the outer loop does not restore f12 before
every self callback; the reconstruction does not invent a float callback
argument. Time is observable in node+18. Children receive the captured original
input time explicitly, even if a callback overwrites +18 or recursively updates
another child with a different time.

Transform mode uses the captured high flag bits. Mode 0x8000 changes only the
translation: load/store X, load/store Y, capture Z, store W=1, then store Z.
The first twelve matrix cells remain untouched. Mode 0xC000 changes no matrix
cells. The other two high modes call the actual numeric transform `002A1F18`
with output at node+70, rotation at +38 and position at +2C. After that call,
fresh flags are ORed with 0xC000; lower bits are retained. The call remains a
complete original VU routine. Its instructions, scheduling and exceptional
hardware behavior are not replaced or counted as recovered source.

After transform processing, the child head is loaded fresh. Each successor is
captured before the recursive call. After return, the captured successor's next
word is loaded fresh. The outer loops separately capture their successor before
the entire inline callback/destruction/child body and reload that saved node's
next word afterward. Removing the current node does not replace the saved
successor. Removing the saved successor can terminate traversal before it is
visited, because original unlink clears its next word. Callback replacement and
flags changes are observed at the corresponding fresh loads.

Inputs retain the original valid intrusive-list/sentinel geometry, finite
termination and ownership/callback requirements. Callbacks must provide valid
live storage for every original subsequent access. The preserved VU routine
has its original input requirements. The C declarations expose side effects;
unknown source-level typedefs and incidental unused v0 values are not inferred
as useful results.

Whole raw/disassembly hashes, complete boundaries, terminal JR/delays, every
local branch and excluded zero alignment are checked. Twelve actual encoded
entry JALs are audited inside their complete containing bodies. `002B9D28` has
no verified incoming reference: the entire 2,949,800-byte .text J/JAL scan, all
aligned pointer words in file-backed allocated nonexecuting sections and a
broad same-body earlier-LUI plus ADDIU/ORI search find none. That search includes
signed carry, different destination registers and long instruction gaps without
assuming overwrite or reachability. Other arithmetic, cross-body construction,
unaligned fields or dynamic/computed dispatch remain possible. Its complete
boundary and behavior proof do not resolve runtime reachability.

All fifteen standard compiler candidates link naturally with zero remaining
relocations. Only the 52-byte deferred wrapper is completely identical under
both GCC 2.9 recipes and is registered matched using GCC 2.9. The other four
are registered reconstructed. The candidate packet records complete bytes,
size, hashes and relocation gates. No scheduling patch,
body fallback, mapped constant data or compiler tuning grid is used.

The scoped tracer reuses the existing bounded decoder unchanged. Five complete
scalar bodies and the reviewed destructor, record clear/destruction and unlink
bodies execute their actual instructions and delays. Unknown code, memory,
calls and unsupported operand forms fail. Returns require the actual JR31
instruction targeting this invocation's selected stop, with its delay inside
the owned body. Reaching that stop by fallthrough or a branch does not end an
invocation, including a recursive return inside the same function. Ordinary
untaken branches execute and validate their delay; annulled likely delays are
skipped. Seven focused tests cover
code scopes, memory bounds, strict transform pointers/quaternion, the node-only
callback contract, real returns/delays/local transfers, untaken delays and
internal return stops, and reserved operands.

The preserved VU call uses a deliberately narrow caller-visible contract in
tests: exact raw quaternion `(0,0,0,1)` with positive zeros and three same-node
pointers. Its complete original body is read for ABI and store order. For those
inputs, authored output models the fixed orientation rows and subsequent fresh
position loads. This is neither a VU emulator nor independent proof of VU
register, precision, pipeline, FCR or exceptional-value behavior. No arbitrary
post-transform mutation hook is added.

The 272 authored fixtures execute 28,636 original instructions, including
nested calls and delays, at most 203 per fixture. They exercise all high cache
modes and flag gates, absent/present callbacks, empty/flat/nested lists,
deferred destruction during callbacks, recursive ticks with different time,
fresh flags/position, current/saved-sibling removal, callback replacement,
destructor alternatives, negative and negative-zero time inputs. Known
callbacks use explicitly authored actions, including calls to the actual defer,
tick and unlink bodies. Callback substitutes clobber f12 in the trace to test
saved child time; f12 is absent from the callback event contract.

The native 32-bit harness compares all 640 buffer words, nine call counts and
the complete event sequence. It passes 186,429 checks. Production update and
ownership sources are separate translation units. The harness includes actual
reviewed `list.c` and licensed `list_aros.c` with observation wrappers and
authored unused diagnostic stand-ins. Free/destructor substitutes retain dead
storage for snapshots, so this is not a heap or arbitrary invalid/free-input
model. No original instructions, diagnostic strings, tables or assets are
copied into the harness or generated header.

Regenerate the authored fixture header with:

```text
.venv/Scripts/python.exe tools/trace_property_updates.py --golden-header tests/native/property_updates_golden.h
.venv/Scripts/python.exe tests/test_trace_property_updates.py
```

The isolated native command is the MinGW 32-bit compiler with
`-m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude`, followed by
`tests/native/property_updates.c`, `src/game/property_updates.c` and
`src/game/property_lifecycle.c`. Prepend that compiler's directory to PATH when
running it. The neutral output name `native.exe` avoids the Windows installer
heuristic for unmanifested executables containing “update”.

Input keys `routine`, `flags`, `callback_mask`, `destructor_mask`, `graph`,
`mutation`, `time`, `time_bits` and `initial` use sorted-key UTF-8 JSON with
comma/colon separators, giving SHA-256
`4577cf5bde672ba84f4d8e811c7257225581fe796f236f90c1a703e5e51e654b`.
The header's CRLF SHA-256 is
`5e2e7703b4ce27e22f6609f43026ea40eb163d3fa854eb0254f4d4da0d9b3ef2`;
canonical LF is
`8483e6a653a33fb2ec886c0e6769920cfa9b81bcc87596408638ad228effeef2`.
The whole comparison, caller/ABI/boundary, reachability, fixture and separate-TU
native command packet is `.local/property_updates`.

The parent independently read all 174 original scalar instructions, the
complete C/header/template/native/tracer/docs and the entire `00278EB0`,
`00278C48` and `002A1F18` original bodies. Fresh fifteen genuine complete links
reproduce the full comparison dictionaries, including the exact 52-byte
wrapper. The broad entry-reachability scan reproduces identically. All 272
fixtures, 28,636 original executed instructions, input and both header hashes
remain unchanged after the stricter control guards. Native 186,429 checks and
all seven guards pass independently. The source review found no defects;
registration preserves the explicit unresolved reachability of `002B9D28`.
