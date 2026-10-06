# Property allocation, names and record management

This connected batch reconstructs six complete utility bodies in
`src/game/property_management.c`, 440 original bytes and 110 instructions:

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `002B9530` | 132 | Allocate a node, initialize it, then assign its name |
| `002B95B8` | 108 | Free and replace an owned name |
| `002BA110` | 92 | Detach a record, free its payload, then free the record |
| `002BA170` | 60 | Allocate and initialize a 12-byte intrusive list |
| `002BA1B0` | 28 | Append a node to the list tail |
| `002BA268` | 20 | Return whether head is the exact tail sentinel |

The header reuses the existing 184-byte observed ownership prefix, 148-byte
record prefix, list sentinels, vector/matrix types and numeric heap APIs. The
node factory requests 192 bytes with alignment parameter 4; the final eight
bytes remain outside the modeled ownership prefix. No complete original class,
extra object fields, diagnostics, table data or source typedef is inferred.

The factory captures its incoming name pointer and three floats before the
allocator. A NULL result skips both constructor and name setter. Success calls
the existing sparse constructor and then the name setter, returning the
captured allocated node rather than an incidental helper result. The input
pointer survives calls while its pointed-to bytes are read at the original
time: a name pointing into a field cleared by the constructor becomes empty.

Name replacement captures both node and input pointer. It frees a non-NULL
old name before clearing +B0, computes the input length through the original
unguarded string helper, requests wrapped unsigned `length + 1`, and stores
the replacement at +B0 before testing allocation success. Copy sees that
published field and still receives the captured input pointer. NULL input
does not allocate. The observed self-name path frees the input before reading
it; no identity guard or valid dangling-storage semantics is invented. The
verified caller does not consume the setter's incidental v0 value.

Record destruction uses the same observed inline algorithm as the existing
record-list destructor. Either nonzero link triggers unlinking. The payload
is loaded after unlink returns, freed if non-NULL, then the captured record is
freed. No payload clear or validity check is added. The list factory preserves
its allocator result across initialization. The append wrapper calls the
reviewed tail helper; the query compares head to `&list->tail` and returns an
unsigned Boolean even for other head values, without dereferencing the head.

The actual list initializer and AROS-derived tail/removal C are reused with
their existing provenance and [AROS license](../LICENSES/AROS-Public-License-1.1.txt).
This is behavioral reuse, not evidence that Papaya used AROS. The ownership
constructor and string-length C remain unchanged. Numeric heap and original
newlib assembly-copy calls keep their original addresses. The preserved VU
matrix initializer remains an engine call; no VU body is reconstructed here.

All six complete original hashes, raw/disassembly equality, terminal JR31
and delay instructions, local branches and excluded zero alignment are
recorded in the isolated proof packet. These bodies have no switch tables or
indirect transfers. A whole 2,949,800-byte .text scan finds exactly one incoming
direct JAL: `002B9588` in the complete factory calls `002B95B8`. The other five
entries have no verified incoming reference. Searches include every aligned
pointer word in allocated file-backed non-executing sections and a broad
same-body earlier-LUI plus ADDIU/ORI scan. Its sole extra candidate is stale:
`0015D168` replaces register 4 with high `0043` before `0015D170` forms data
address `0042A268`, so combining the earlier `0015D0CC` high word would falsely
suggest `002BA268`. The complete caller and its hash were checked. These
searches do not exclude other arithmetic, cross-body, unaligned or dynamic
computed references. Complete body recovery and runtime reachability are
separate claims; the five entries remain explicitly unresolved.

All 18 established compiler candidates link naturally with zero remaining
relocations. The complete 28-byte tail wrapper matches under GCC 3.2.3, and
the complete 20-byte sentinel query matches under all three profiles. The
other four bodies remain reconstructed. No byte masking, code patch,
assembly-body fallback or broad flag search is used. Parent independent review passed all 110 original instructions, the complete
70-word strcpy and 139-word rejected caller, full C/header/native/tracer/guards/
documentation, all original hashes and references, 18 fresh whole links and
91 regenerated fixtures/7,626 instructions/61,865 native checks/seven guards.
All proof/comparison dictionaries reproduced. The six entries are registered
with the two full-byte matches and five unresolved incoming entries preserved.
The canonical verifier and hybrid build passed at this checkpoint; the full
retail ELF SHA-256 reproduced with 246 tooling tests passing (one skipped).

`tools/trace_property_management.py` executes the six full original bodies
and the actual constructor, list initializer/tail/unlink and length bodies.
It also executes the original strcpy's misaligned byte branch; aligned
64-bit/MMI paths are rejected. Numeric allocations and free use authored
observations with retained storage. The VU initializer has the already
reviewed fixed VF0 identity-store output contract, without VU instructions,
registers, pipeline timing, exceptional state or FCR emulation. Explicit
post-initializer/unlink hooks stress later scalar stores and fresh payload
loads beyond the pure helpers' own effects. Self-name experiments retain
freed storage and therefore do not model a valid heap, valid dangling reads
or general overlapping-string behavior.

There are 91 authored fixtures, 7,626 executed original instructions including
delays and helper bodies, and a maximum of 248 instructions per case. They
cover allocation failures, captured coordinates/name, sparse constructor
aliases, NULL/empty/high-byte names, field clear/publication order, attached
and detached records, NULL and mutated payloads, captured frees, tail append
and exact sentinel queries. Only synthetic buffers and events are exported.
The native 32-bit harness passes 61,865 full-buffer, result, call and event
checks. It includes unchanged reviewed list/string C via macro adapters and
compiles the unchanged ownership source as a separate translation unit.
The unused sibling CRC functions receive an algorithm-generated standard
polynomial table; no game table bytes are copied. The native copy adapter
implements only the forward byte contract used by the executed scalar path.
Seven focused code/memory/operand/call/return/delay guards pass.

The fixture-input SHA-256 is
`4859068782425625ede18be46ae1ee6d84847ef2f67b50500725140dfe313354`.
The generated header SHA-256 is
`dd2432008fd0dd7f44ee2411a89171ea8172009252fa4a358653ce69648e2eac`
with LF line endings and
`f382fac7dd29512c48a43a77fa92bc566f35eec5670c97c3ec007d5144ee03a5`
with Windows CRLF line endings. Regeneration uses the locally hash-validated
ELF:

```text
.venv/Scripts/python.exe tools/trace_property_management.py --golden-header tests/native/property_management_golden.h
.venv/Scripts/python.exe tests/test_trace_property_management.py
```

The isolated native command uses 32-bit MinGW GCC with `-m32 -O2 -Wall -Wextra
-fno-strict-aliasing -I include`, compiling `tests/native/property_management.c`,
`src/game/property_management.c` and `src/game/property_lifecycle.c` separately,
then linking `-lm`. Prepending the compiler's directory to PATH is required
for its DLLs. The shared runner uses these separate translation units:

```text
.venv/Scripts/python.exe tests/native/run_utilities.py --harness property_management
```
