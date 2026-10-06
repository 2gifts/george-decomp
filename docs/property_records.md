# Property records

This connected batch reconstructs three complete routines, 976 original bytes
and 244 instructions, in `src/game/property_records.c`:

| Entry | Bytes | Behavior |
| --- | ---: | --- |
| `0029A508` | 896 | Apply linked records through the first matching descriptor |
| `0029A890` | 36 | Return the next record or NULL according to flags bit 7 |
| `0029A8B8` | 44 | Swap a record's key through the reviewed byte-order helper |

Each fixed record header is eight bytes: a key word, kind byte, flags byte and
unsigned 16-bit step. Payload begins at +8. Each descriptor occupies 24 bytes:
key, type, destination offset, mask, conversion callback and one unused word.
The partial layouts assert these offsets and sizes on the target and native
32-bit test platform. Storage extents remain caller-owned.

The full original parser reads only its first four argument registers. Its 103
encoded direct-entry JAL references include `0010DEB4`: that caller supplies the
object's +18 destination, a record pointer, count 10 and descriptor table
`003F2EC0`, then additionally supplies registers 8–10. These extra values are
unused by this complete body; the reconstruction does not assign them invented
roles. The next-record entry has actual references `002366C0` and `002BA024`;
both consume its returned pointer. No encoded direct JAL to the key-swap entry
was found. That body leaves its byte-order callee's returned word in V0 after
storing it, so the C declaration exposes this passthrough while the original
source-level return declaration remains uncertain.

The parser stops on NULL or kind zero. A nonzero descriptor count first captures
the record key and searches for the first match; count zero reads neither the
key nor descriptor data. The found index retains the original signed-negative
application guard after the unsigned search. Descriptor types 1, 2, 4, zero and
out-of-range values copy one payload word. Types 3, 7 and 8 copy three or seven
float words with the original forward load/store order. The seven-word cases
reload the destination offset after their first three stores, then retain that
new base for the last four. Types 6 and 9 store the payload pointer, type 5 calls
the original string-copy entry and type 10 sets or clears descriptor-mask bits.

Types 11–13 retain the exact angle literals `3C8EFA36` and `3C0EFA36`; the latter
two call the original numeric cosine binding. Type 16 calls the captured
conversion callback with the payload pointer. Offset reloads after cosine or
conversion calls are preserved; string-copy destination is captured before its
call. Types 14 and 15 capture the packed source word, convert unsigned bytes and
multiply by exact literal `3B808081`. Red captures the first destination offset;
subsequent color stores reload it after each preceding store. Alpha is stored
at +16, leaving +12 untouched. This odd layout is observed in the original and
is deliberately retained. Every path then reloads record flags, and only a
clear bit 7 loads the step and advances using the observed low 32-bit ADDU.

These operations support the observed source/destination and descriptor aliases.
They require valid initialized payloads, a descriptor array of the stated
extent, valid callback/string-copy arguments and sufficient destination storage.
Advancing records must eventually terminate. No bounds check or cycle guard is
invented for invalid original inputs. The standalone next-record routine
requires a non-NULL header, as the original immediately reads its flags.

The already reviewed ordinary C scalar byte-order implementation at `002B2448`
is reused unchanged. The parser retains numeric string-copy `00393B74`, cosine
`0029C168` and indirect conversion bindings. No external source identity is
claimed for this application-specific descriptor algorithm.

The original complete 16-entry switch table is readonly `.rodata` at `004455D0`,
file offset `003465D0`, with SHA-256
`1bf06df5949b582209f0b84f3e61a8045478fe9a1f2c41e83f3b72a649f48889`.
All targets remain within the complete parser. Unsigned `(type - 1) < 16` guards
it, and original LUI `0029A59C` plus ADDIU `0029A5A4` prove its address; the
intervening instruction shifts a different register. The natural C switch
retains explicit cases 1, 2 and 4 joined with default, reflecting those original
table entries. Both GCC 2.9 variants emit a complete 64-byte generated table.
The existing strict generated-readonly mapper links all 16 local R_MIPS_32 case
pointers using actual GNUld, returns the complete linked data and compares it
against the complete original table. Full code and full generated data must
both agree for a match. No instruction, table byte, relocation or scheduling
result is patched or excluded.

All nine standard compiler candidates compile. Eight naturally link with zero
remaining relocations and zero complete matches. The parser's GCC 3.2.3 object
instead emits a 68-byte table and fails the strict whole-section geometry gate;
that candidate remains an honest linking limitation. Its table is not cropped
or placed at an invented address. GCC 2.9 with the authentic save128 macro is the
canonical complete-link recipe; all three routines remain reconstructed.
Registered metadata records original hashes, all 105 direct-entry JAL sites,
local branches and returns, three exact float literal identities and the
complete switch proof. The neighboring eight-byte payload-pointer getter at
`0029A888` is a separate excluded entry, rather than parser padding.

`tools/trace_property_records.py` executes only these reviewed original bodies
and the complete validated readonly switch table. It inherits the reviewed
camera decoder unchanged, adding observed byte/halfword loads, likely branches,
signed shifts, low-word R5900 three-operand multiplication, integer mask/order
operations, linked JALR and byte-scoped CVT.S.W. Reserved operands, unsupported
forms, unknown calls, other code/table scopes and uninitialized memory fail.
Seven focused guard tests check these boundaries. The decoder's unused HI/LO
state is not a general multiply model; the complete bodies never read it.

The 163 authored finite fixtures cover all 16 cases and defaults, descriptor
counts zero/one/four, first-match search, empty records, shifted vector copies,
self-descriptor offset mutation, payload pointers, packed channels, alpha's
untouched gap, and callbacks that change offset, termination flags or step.
They also exercise next-record flag combinations and steps 0/8/40/65535, plus
the key-swap passthrough. Regenerate them with:

```text
.venv/Scripts/python.exe tools/trace_property_records.py --golden-header tests/native/property_records_golden.h
```

The fixtures execute 14,159 original instructions including delay slots, at most
276 per case. Sorted-key UTF-8 JSON with comma/colon separators over `routine`,
`kind`, `mutation`, `alias`, `count`, `search`, `packed`, `empty`, `initial` has
input SHA-256 `18dd8e41aa8b6b0300bb66fd601ac81d6f5d65a5362f672637eb30dfa0508189`.
The generated header has CRLF SHA-256
`c22bb9a4b19325fa369db09aa849c02585f20effa60e0b6bf847c78ffd4eb75c`;
canonical LF is `46037ad72b9ed85236dcd2e36a11d4eeb1aa805ca623aaee1bc0871c21dc3006`.
Only authored input/output/event words are exported, never original instructions
or table bytes. The native 32-bit harness passes 21,951 checks with
`-ffloat-store -fno-strict-aliasing`; it reuses the actual recovered byte-swap C
and records string/cosine/conversion calls, pointer roles and mutations.

The finite cosine/conversion/string-call substitutes intentionally test caller
observations and do not establish those engine callees' arithmetic or mutation
behavior. Finite host float comparisons exclude EE special values, FCR flags,
wide-register/cycle behavior and invalid memory/capacity inputs. Complete target
bytes and complete generated data remain the exact-match gate.

The parent independently reviewed all 244 original instructions and the complete
source, header, documentation, tracer, native harness and seven decoder guards.
Its audit confirmed complete hashes, boundaries and returns, all 105 real entry
JALs within their complete containing bodies, all three exact literals and the
complete 64-byte switch identity. Eight fresh actual link dictionaries reproduce
the packet exactly; the GCC 3.2.3 whole-section limitation remains documented.
The independent native rerun passed 21,951 checks and all seven guard tests.
No semantic defects were found. Registration is approved; all three entries
remain reconstructed without a complete byte match.
