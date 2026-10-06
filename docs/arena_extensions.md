# Arena descriptor and four-array extensions

This batch reconstructs five complete functions, 496 bytes / 124
original instructions. It reuses the published 24-byte `GeorgeArenaBuffer`
layout and ordinary C reserve/alignment template. The published arena files
remain unchanged. Author, peer and parent reviews passed and central registration is complete.
Canonical verification passed all 280 tooling tests (one platform-specific
skip), and the hybrid rebuild reproduced the retail ELF SHA-256.

| Entry | Bytes | Recovered behavior |
| --- | ---: | --- |
| `002B8C00` | 380 | Reserve and publish four rounded arrays, strides 12 / 12 / 4 / 2; zero unused lanes |
| `002B8D80` | 28 | Initialize the generic descriptor, null callback and context |
| `002B8DA0` | 12 | Store opaque context, then callback |
| `002B8DB0` | 32 | Raise unsigned high-water when needed, reset cursor to base |
| `002B8DD0` | 44 | Align the generic descriptor cursor |

`002B8DB0` has the same complete original 32-byte body as published
`002B8E88`. Its separate ordinary C definition copies that reviewed algorithm
and emits a genuine natural function symbol. All three established compiler
profiles produce exactly the complete original body, without unresolved
relocations. The other four entries remain reconstructed. The recorded 15
comparisons use actual GNU linker output; no body is truncated or patched, and
no assembly fallback or match transfer is used.

## Original boundaries and incoming references

Every declaration extent agrees byte for byte with the hash-validated local
ELF and complete generated assembly. The extent includes each actual `JR31`
and its delay instruction. Four entries have four following zero alignment
bytes; reset has none. The previous complete function and next entry are
recorded independently. All seven local branches, four actual `JAL 003936A0`
instructions, 28 scalar/stack stores and the signed-carry global-address
materialization are audited. There is no switch table or indirect call in
these bodies.

All five entries have **unresolved incoming reachability**. The complete
2,949,800-byte `.text` direct J/JAL search, aligned address-word search of all
file-backed allocated non-executable sections, and broad earlier-LUI plus
ADDIU/ORI search in the same inferred function found no candidates. This does
not exclude computed calls built through other arithmetic, cross-function
address construction, unaligned fields or dynamic initialization. Complete
boundary and semantic reconstruction does not establish runtime reachability.
The declarations are `void`; incidental retail `v0` values and consumed return
semantics are not claimed. The setter stores a callback but does not invoke it.

## Capture and publication order

The four-array routine rounds `(count + 3U) & ~3U`, with all size/address
arithmetic retaining low-32-bit wrap. The published template preserves the
strict unsigned fit test: equality resets. The high-water update precedes the
fresh base load; cursor is published before each output slot. The final output
store is the padding branch's genuine delay instruction.

The padding gate compares the full signed 32-bit count with the rounded value.
Four actual zero-fill calls retain the captured array bases and original count
through external calls. The first two lengths and offsets use stride 12; the
last two use shifts by two and one. Output slots can alias each other,
descriptor fields or padding memory; controlled post-call mutations do not
cause captured zero destinations to be reloaded. No new count, capacity or
failure guard is added.

The constructor stores capacity, high-water, null context, base and cursor,
then null callback in its actual return delay. The setter stores context before
callback. Reset compares cursor/high-water unsigned and reloads base before its
cursor store. Alignment masks the exponent to five bits and changes cursor
only when its remainder is nonzero.

## Synthetic validation and reproduction

`tools/trace_arena_extensions.py` inherits the reviewed scalar decoder without
changing it and restricts instruction fetch/control flow to these five complete
bodies. The original numeric memset binding is retained. Tests provide an
authored bounded zero-store, caller-clobber and exposed-state mutation contract;
the 184-byte MMI memset body is read for ABI evidence but is not executed by the
scoped tracer. No original instruction/data arrays are exported.

The 628 fixtures execute 31,444 original instructions, maximum 94 per case, and
876 controlled zero calls. Coverage includes count/sign/rounding boundaries,
strict fit/equality/reset, high-water comparisons, masked alignment exponents,
local/global descriptors, unsigned numeric wrap, all four output aliases and
four post-zero mutation modes. The compact header losslessly reconstructs an
authored 512-word initial buffer and every changed word. Native code still
compares all 512 buffer words, six global descriptor words and each eleven-word
call event. Production C executes as a separate translation unit. Fresh native
validation passes **337,948 checks** without compiler warnings. Eight focused
decoder guards pass, including memory/fetch bounds, reserved forms, low-32-bit
signed/unsigned arithmetic, controlled-call bounds, branch delays, actual JR31
stops and bounded loops.

Native storage is 64-KiB aligned and its complete extent remains below bit 31,
preserving the authored alignment bits. Raw numeric wrap pointers are compared
without dereference. Cases that would pad inaccessible storage after a bit-31
alignment are outside this bounded call contract. These tests do not establish
malformed-pointer/count/capacity safety, callback invocation, MMI/hardware/FCR
behavior, full EE register upper bits or timing fidelity.

Input SHA256:
`4c63925f7897508106b23773d5b9369df857b1fc87c8945fcbcca4cf78029eea`.
Synthetic header LF SHA256:
`7785e13b60aa9717877ae6bc6ddb504251b498dd175c9f58fe44eb0196b9f6e1`.
Windows CRLF SHA256:
`2d0c1351bdc1e7d6a1cdc662233fbbb0ab6732c8fb190a5d2aae2bfc0fd470e6`.

```powershell
.venv/Scripts/python.exe tools/trace_arena_extensions.py --output build/arena_extensions_trace.json --golden-header tests/native/arena_extensions_golden.h
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_arena_extensions.py -v
```

Compile the native harness with MinGW `bin` prepended to `PATH`:

```text
gcc.exe -m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude tests/native/arena_extensions.c src/game/arena_extensions.c -lm -o build/arena_extensions_native.exe
```

The ignored `.local/arena_extensions` packet records complete function hashes,
boundaries, reachability limits, template/duplicate evidence, all 15 genuine
link comparisons, fixture identities and the exact native command. Author, research peer and parent read every selected original instruction and
complete source/header/templates/decoder/native harness. Both independent
reviewers reproduced all 15 complete comparison dictionaries, full proof
packets, 628 fixtures, 337,948 warning-free native checks and eight guards.
The registered packets are `config/functions/arena_extensions.json` and
`config/symbols/arena_extensions.json`; incoming reachability remains unresolved.
