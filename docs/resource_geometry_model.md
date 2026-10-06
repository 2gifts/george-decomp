The resource test model covers six complete original bodies: `20E920` (384
bytes), `20EAA0` (356), `20EC10` (104), `20EC78` (40), `20ECA0` (208) and
`20EEC8` (152), totaling 1,244 bytes and 311 instructions. The peer read the
complete original instructions, C and header, including all decoded nonstack
stores and return/branch delays. This is a caller model: engine allocation,
lookup, release, finish, payload, transform and classification calls are
controlled substitutes. Their implementations receive no recovery award.

The public trace entry point first validates the local ELF. Instruction
fetches stay within the six reviewed intervals. Ordinary taken and untaken
branch delays are checked explicitly; likely annulment is retained. Returns
require the actual `JR $31` instruction and the selected return address.
Unknown calls, cross-body transfers, control in delays, unsupported JALR
operands, unknown memory, writes to readonly storage and excessive execution
are rejected. Nine dedicated guard tests cover these restrictions.

The authored arena supplies valid record, neutral holder and payload storage.
The holder model asserts only its byte at offset 2 and pointer at offset 8.
Tests cover modes zero/nonzero, initial flags, a wrapping 16-bit count,
constructor callbacks that change flags, lookup/allocation paths, immediate
and deferred completion, and callback mutations of holders, payload pointers,
flags and related records. The deferred model invokes the included complete
original callback body and the native production callback respectively.
Neither model clears an empty constructor payload or substitutes unrelated
engine code.

Geometry cases use finite authored vectors. The transform substitute writes
XYZ and retains the caller's preloaded radius; classifier substitutes record
the full sphere/frame contents. Signed classification values `1` and `2`
alone select the second classifier, including boundary cases zero, negative,
three and both signed extremes. Mutation cases check the retained payload
and matrix pointers, copied radius and fresh callback-related loads. These
models do not certify EE arithmetic precision, FCR flags, timing, hardware
exceptions, malformed/null objects or exhaustive engine behavior.

The original whole 16-byte readonly table at `0x0043A408` is checked by
geometry and SHA-256 before the trace reads its observed second pair at
offset 8. Its signed adjustment is zero and its target is `0x0020F070` with
word argument three. The native harness uses its own synthetic pair and mock
target. It exports neither the original table words nor original code. This
validates the caller ABI; reconstruction of that callee belongs to the
separate resource lifecycle batch.

The 236 authored fixtures execute 8,521 actual original instructions and
produce 72,357 native checks. The native harness and production source compile
as separate translation units. Their synthetic input SHA-256 is
`4b1a47065b35e2416d6df5d3e24ca47ede2ad54523e563a55e9752d4deb9750a`.
Golden data contains only authored memory states, expected results and
controlled events. Run `tools/trace_resource_geometry.py --golden-header
tests/native/resource_geometry_golden.h` to regenerate from the validated
local image, then `tests/native/run_resource_geometry.py` for native checks.

GCC2.9 outlined the two ordinary static inline tails, creating helper calls
without original helper-address evidence. Only those repeated source tails
were changed to scoped ordinary-C macros. The native 236-fixture/72,357-check
result and generated fixtures stayed unchanged. All eighteen complete actual
GNU links across GCC3.2.3, plain GCC2.9 and GCC2.9 SAVE128 now succeed without
unresolved relocations; all remain nonexact. No forced helper binding,
instruction edit, masking or speculative compiler adjustment is used.

The ignored proof packet records all six hashes/full boundaries, 33 bounded
branches, 34 actual incoming JALs with 20 whole containing-body byte proofs,
three real pointer materializations, and the complete nonstack store
inventory. `20EEC8` has a genuine callback materialization; `20E920` also has
an aligned readonly pointer at `0x0043A424`, whose complete surrounding
container/dispatch is not asserted. `20EC10` has complete standalone
prologue/return geometry but unresolved entry reachability: the packet found
no direct JAL or aligned allocated nonexec absolute pointer. Other computed
or generated pointers are not excluded. These limits must accompany any
registration; none supplies an exact code award.

The six functions are registered as reconstructed after root reviewed the
final C macros/header, complete model, guard tests and proof packet. Root
independently reproduced all eighteen complete linked comparisons, the nine
guard tests and all 72,357 native checks. Root also read the complete original
124-byte `2A1C60` callee: it writes Z, X and Y (Y in its return delay) and leaves
W untouched. This supports caller radius retention; the fixture transform
remains an authored caller substitute. The peer's original/C review and the
root's source/model review are distinct. Six additional links using only the
registered central numerical bindings reproduce the selected comparisons.
