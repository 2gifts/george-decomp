# Property record production

This connected pair reconstructs 468 original bytes and 117 instructions in
`src/game/property_pack.c`: linked-node serialization `002B9F38` occupies 312
bytes, and node construction `002BA070` occupies 156. The following four zero
bytes at `002BA10C` are alignment and are excluded. All complete original body
words, terminal returns/delays and local branches are checked against the
validated ELF. Neither entry has an encoded direct JAL reference in `.text`;
their source-level names and broader registration/use context remain unknown.
Their complete bodies provide the pointer roles and scalar widths documented
here. They are kept together as a coherent record-production component.

`GeorgePropertyTextNode` is an observed 148-byte partial layout, beginning with
the already reviewed `GeorgeListNode` next/previous prefix. Its 128-byte name
starts at +8, full-word kind at +88, length at +8C and payload pointer at +90.
Size and offset assertions apply to the target and native 32-bit platform. The
existing `GeorgeList` head/tail/tail-previous layout explains the overlapping
sentinel: an empty head points to the list's +4 tail word, whose next is NULL.
Only a real node is accessed through fields past the sentinel's link prefix.
These types do not assert a complete scene/text object class.

Serialization first writes zero to the caller's output-size word, then loads
the head and tests the exact sentinel address. It captures a wrapping 32-bit
total of eight header bytes plus each node's full 32-bit payload length, as well
as the initial record count. A NULL allocation returns with the output size
zero. A successful allocation reloads the list head: mutations through the heap
call are visible. The next input node is captured before each CRC call and
retained across CRC, copy and next-record calls. A changed current-node next
pointer therefore does not replace that saved successor; the successor's own
next pointer is loaded fresh after the copy/query sequence.

For each record, the key is the result of the reviewed CRC-string entry. Kind's
low byte is loaded after the key store but before the zero flags store. The
last-record marker uses the captured first-pass count, rather than recounting a
mutated list. Step is `(u16)(8 + (u16)length)`, while the copy call reloads the
full 32-bit length and payload pointer. Thus step truncation does not narrow the
copy size. The original next-record routine then reads the newly stored, live
flags/step; copy callback mutations remain visible. The final output-size store
uses the captured allocation total, even when callbacks changed the traversed
nodes. The returned allocation pointer is retained independently of the current
output-record cursor.

Construction requests exactly 148 bytes and returns NULL if that request fails.
On success it stores previous zero, then next zero, before the string-copy call.
It stores the captured full kind and length after that callback, requests the
captured payload length, and assigns the returned payload pointer before the
NULL test. Payload allocation failure still returns the partially constructed
node with a NULL payload; the original does not free it or return NULL. A
successful payload allocation invokes the numeric copy entry with the captured
allocation, input pointer and length. Stores made by copy callbacks are not
overwritten afterward. The unused name tail and untouched allocation bytes are
left as received from the allocator; no deterministic contents are introduced.

These routines require a valid terminating intrusive list and node payload
extents, a writable output-size word, sufficient allocation capacity, a
terminated constructor name fitting its observed 128-byte field, and valid
copy arguments. Record-step truncation and 32-bit total wrapping are retained,
so inputs must also keep the subsequent original accesses valid. There are no
invented overflow checks, NULL-list guards, cycle limits or failure cleanup.
The tests' fixed-capacity allocator is an authored callback model rather than
an assertion that a real allocator always returns every requested capacity.

Production retains the reviewed heap `002AEC28`, CRC-string `0029C648` and
next-record `0029A890` bindings, along with numeric string-copy `00393B74` and
buffer-copy `003934F8`. All eight direct call instructions inside the two bodies
are audited. Native tests compile the actual recovered CRC/length and
next-record source unchanged through macro-only symbol adapters to record calls.
Their CRC table is generated from the independently identified standard
reflected polynomial `EDB88320`; no original table bytes are embedded. The
instruction trace uses Python's standard CRC-32 over the same authored strings.
No external source identity is claimed for these application-specific routines.

All six standard compiler candidates link naturally and completely with zero
remaining relocations, with no mapped data, inline instruction fallback or
compiler tuning grid. The constructor's GCC 2.9 candidates have the original
156-byte size but still differ by 36 and 28 bytes. Serializer candidates are
328/332 bytes, against 312 original bytes. GCC 2.9 with the authentic save128
macro is the canonical comparison recipe; both entries remain reconstructed.
Complete byte hashes and comparison dictionaries are recorded in the registered
function metadata and isolated `.local/property_pack` comparison packet.

`tools/trace_property_pack.py` reuses the reviewed scoped decoder unchanged and
adds only observed byte/halfword stores. It executes both complete production
bodies and genuine nested execution of the complete next-record body, including
return delays. The complete scopes, unknown calls, cross-body transfers,
uninitialized/out-of-window memory, unterminated names and invalid controlled
copy extents are rejected. Returns to an external stop require the actual
JR31 instruction, and every transfer's delay must remain inside its complete
owned body. Six focused guards check store truncation and signed
offsets, code and memory scopes, unknown calls/copy bounds, nested return delays
and inherited reserved operands. Heap/string/copy callbacks are explicit
controlled substitutes; the actual heap or copy algorithms are not simulated.

The 101 authored integer fixtures execute 6,068 original instructions including
delay slots and nested next-record calls, at most 152 per fixture. They cover
empty sentinel lists, non-sentinel heads with NULL next, zero payloads, one to
three real nodes, first/second allocation failures, huge sizes failing before
copy, output-size aliases, and callback mutations of head, captured successor,
kind, length, payload, record step and termination. Constructor callbacks test
the before/after initialization order and retained partial-node ownership.
Every buffer word, returned pointer, call count and event is compared exactly;
the native 32-bit harness passes 54,214 checks.

Regenerate the synthetic header with:

```text
.venv/Scripts/python.exe tools/trace_property_pack.py --golden-header tests/native/property_pack_golden.h
```

Input keys `routine`, `count`, `length`, `kind`, `mutation`, `failure`, `alias`,
`initial` use sorted-key UTF-8 JSON with comma/colon separators, giving SHA-256
`272bc0586655802dad361cea413798d27e2cdc2d7b1d0a2a59601dc860a5a9d8`.
The header's CRLF SHA-256 is
`b9bf57584c8d8ec03d5dbf1751c4854886a34f7fa3ef394aaf446161120690f7`;
canonical LF is `367dd2b80bd57f87a044131d628e84c3458e40706dcc33ae9111c4b27a347ff3`.
Only authored input/output/event words are exported, never original game code,
tables or assets. These tests support the bounded integer/callback behavior;
they exclude full hardware timing/wide-register semantics, invalid runtime
inputs and arbitrary allocator/copy behavior. Complete target-byte comparison
remains the exact-match gate.

The parent independently reviewed all 117 original instructions and the complete
source, header, documentation, tracer, native harness and six guards. Its audit
confirmed both full raw/disassembly hashes, all boundaries, delays and excluded
padding, all eight numeric helper calls, zero encoded whole-text JAL entries
and zero aligned entry-pointer words in allocated non-executing data. Six fresh
actual complete links reproduce every comparison dictionary; 101 regenerated
fixtures, all 6,068 executed instructions, both header hashes and the input hash
agree. Independent native execution passed 54,214 checks and all six guards.
Its additional return/delay guards leave the fixtures unchanged and were
reviewed locally. No source semantic defects were found. Registration is
approved; both entries remain reconstructed without a complete byte match.
