# Render records and arena ownership

This isolated batch reconstructs five complete functions, 368 bytes / 92
original instructions, in two source files. It reuses the published heap
bindings and arena descriptor implementation without changing them. Author,
research peer and parent review passed. All five functions are centrally
registered; the startup wrapper matches its complete 28 original bytes.

| Entry | Bytes | Source | Recovered behavior |
| --- | ---: | --- | --- |
| `002B8A68` | 136 | `render_records.c` | Allocate a 48-byte record, then initialize it when allocation succeeds |
| `002B8AF0` | 88 | `render_records.c` | Write the observed scalar record prefix and leave word `+2C` untouched |
| `002B8B68` | 64 | `arena_ownership.c` | Allocate an owned arena and initialize its descriptor using fresh capacity |
| `002B8BA8` | 52 | `arena_ownership.c` | Free the current owner, then clear owner and capacity |
| `002B8BE0` | 28 | `arena_ownership.c` | Call the published eight-byte arena no-op during startup |

All 15 comparisons use complete, genuine GNU linker output at the original
function addresses, with zero unresolved relocations. Only the startup wrapper
matches all 28 original bytes, under GCC 3.2.3. The other four functions remain
reconstructed. GCC 2.9 naturally emits an eight-byte tail call for the startup
wrapper; that candidate is compared against all 28 original bytes and fails.
No function is truncated, patched, masked, transferred from another match or
replaced with an assembly fallback.

## Record layout and capture order

The record allocation size is exactly 48 bytes. The observed fields are scalar
offsets: three incoming floats at `+04/+08/+0C`, three `1.0f` words at
`+10/+14/+18`, the raw word `80808080` at `+1C`, two opaque integer arguments at
`+20/+24`, and zero words at `+00/+28`. Position, scale and color field names
describe likely use; complete rendering or GPU meaning is not established.
The word at `+2C` is untouched.

The constructor captures two integer arguments and three floating arguments
before calling the real allocation binding. On failure it skips the initializer
and returns the captured null pointer. On success it restores the original
arguments for the initializer and returns the captured allocation regardless
of incidental initializer `v0`.

The initializer first stores X/Y/Z and the two opaque words, then stores X
again, clears `+00`, stores Z/Y again, writes the raw `+1C` word, writes the
three scale words in X/Z/Y order, and finally clears `+28` in the actual return
delay. The source retains those duplicate scalar statements. All three
compilers naturally remove redundant stores and emit a 64-byte body instead
of the original 88-byte body. No volatility or scheduling workaround is added.
The proof accounts for the original derived store bases `record+4` and
`record+0x10`; encoded instruction offsets alone do not establish the record
write footprint.

The complete external caller at `0023D998` supplies a resource word, zero and
three zero floats, then stores the returned pointer at a separate record's
`+10`. The recovered interface preserves general incoming values rather than
specializing to that caller.

## Ownership and callback-visible state

Allocation publishes the requested capacity at `003FD3E4` in the allocator
call's delay slot. After allocation it reloads that global capacity, publishes
the captured allocation at `003FD3E0` in the arena initializer's delay slot,
and calls the genuine published initializer. The initializer's complete
original body and production C execute in the scoped tests. No null-allocation
guard or additional arena reset is added.

Release reads the current owner in the free call's delay, calls the genuine
free binding unconditionally, then clears owner before capacity. Changes made
by a controlled free hook to the separate arena descriptor survive. The scope
does not establish whether the underlying heap accepts null or malformed
pointers.

The startup caller has a live `f12` value, but the complete wrapper and no-op
never read it. Their recovered declarations are `void`; no floating argument
or consumed return meaning is invented. Only the record constructor has a
proved pointer result.

## Original boundaries and reachability

The hash-validated ELF and complete generated assembly agree for each function,
including its actual `JR31` and delay instruction. Release and startup each
have four following zero alignment bytes; the other three have none. Previous
complete function boundaries and following entries are recorded separately.
The excluded record-free wrapper at `002B8B48` is already published. Six actual
callee JALs, all local branches and all 30 scalar/stack stores are audited.
There is no switch table, indirect call or original data array in these bodies.

There are four actual entry JALs: the external record constructor caller, the
constructor's initializer call, the arena allocation caller at `00154CD8`, and
the startup caller at `00100370`. Their complete containing bodies, argument
instructions and return use are recorded. The arena release entry has
**unresolved incoming reachability**.

The complete `.text` J/JAL search, aligned address-word search of file-backed
allocated non-executable sections, and broad earlier-LUI plus ADDIU/ORI search
find no verified incoming reference for release. A broad candidate within
`002CDFF0` is false: an earlier `LUI 002C` is overwritten by `LUI 0045` before
the lower immediate, constructing readonly name address `00448BA8`, not code
entry `002B8BA8`. The analogous allocation candidate constructs `00448B68`.
These searches do not exclude other arithmetic, cross-function construction,
unaligned fields or dynamic initialization; boundaries and reconstruction do
not establish runtime reachability.

The owner, capacity and existing arena descriptor occupy allocated writable
regions with proved geometry. No initializer identity is claimed. The two
record literals are proved from actual instruction materializations and full
word hashes; no original data bytes are exported.

## Synthetic validation and reproduction

`tools/trace_arena_ownership.py` inherits the reviewed bounded arena decoder
unchanged. Its instruction scope contains the five selected complete bodies
and two published arena helpers. The added COP1 support is limited to finite
raw scalar loads, stores, `MTC1` and `MOV.S`; reserved operands, unknown forms,
nonfinite inputs and unreviewed code fail. Ordinary branch delays execute when
the branch is untaken; likely branch annulment and actual JR31 returns are
checked explicitly. Nested controlled calls execute real recovered release
and startup bodies while preserving the outer invocation's captured registers.

The 886 authored fixtures execute 43,026 original instructions, maximum 69 per
fixture. They cover positive/negative zero, finite subnormal and maximum
values, shifted output records, opaque words containing synthetic buffer
addresses, allocation failure, zero and unsigned capacity boundaries, global
capacity/base mutation, descriptor mutation, nested allocation/free/startup
calls, untouched `+2C`, and post-free clear order. Allocation/free hooks expose
the actual captured call argument and published globals in ten-word events;
they model caller-visible observations rather than the heap graph.

The compact synthetic header losslessly reconstructs a 256-word authored
initial buffer and every changed word. Native tests compare all 256 buffer
words, owner and capacity, all six descriptor words, call counters and complete
events. Both new source files and the published arena source execute as
separate production translation units. Native validation passes **246,561
checks** with no compiler warnings. Nine focused decoder guards pass.

These tests establish only the bounded finite scalar and controlled call
contract. They do not establish heap/MMI/OS behavior, nonfinite EE/FCR behavior,
full upper register contents, timing, or malformed-pointer/capacity safety.
Neither original code/data arrays nor assets are copied into the fixture header.

Input SHA256:
`096b06249be211f965b7719ebaad96791ca2f47b4342d46813a482d82755078c`.
Synthetic header LF SHA256:
`d6d57da7f91cb403b38c900330f0e0bc19920aea65ba5c0a4fe9a4f7434057eb`.
Windows CRLF SHA256:
`d47cdc17133920196ced52684dc934abd8d63aed3792ce738b21a7cf83534cfe`.

```powershell
.venv/Scripts/python.exe tools/trace_arena_ownership.py --output build/arena_ownership_trace.json --golden-header tests/native/arena_ownership_golden.h
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_arena_ownership.py -v
```

Compile the native harness with MinGW `bin` prepended to `PATH`:

```text
gcc.exe -m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude tests/native/arena_ownership.c src/game/render_records.c src/game/arena_ownership.c src/game/arena_buffers.c -lm -o build/arena_ownership_native.exe
```

The ignored `.local/arena_ownership` packet contains complete original and
source identities, boundary/caller/store/global/reachability evidence, all 15
natural comparisons, synthetic hashes, and the exact native command. Author
review read all 92 selected original instructions, complete source/header,
incoming callers, allocation/free/helper bodies, scoped decoder and native
harness. Research peer and parent independently reproduced the entire packet,
all fifteen genuine whole links, 886 fixtures, the warning-free native run
and nine decoder guards. Both read all selected original/source/header
and complete incoming caller bodies. Their ignored audits are
`build/reuse/arena_ownership_review/audit.json` and
`.local/arena_ownership_root/audit.json`.
