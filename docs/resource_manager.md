# Resource resolution and accounting

Four complete centrally registered functions recover 1,556 bytes / 389 original
instructions. Author, independent peer and parent review and fresh complete
packet/native/link reproductions passed. Existing resource
prefixes, manager/allocator geometry, pool source and numeric helper bindings
are reused without changing their frozen files.

| Entry | Bytes | Behavior |
| --- | ---: | --- |
| `00224AB8` | 1,040 | Resolve a resource, allocate or find its payload, queue a request and publish its map/count state |
| `00225C28` | 92 | Remove a key and decrement the current manager's count when removal succeeds |
| `00224750` | 212 | Subtract independently gated resource accounting |
| `00224678` | 212 | Add independently gated resource accounting |

The accounting pair shares one ordinary C template with separate natural
function definitions. A complete relative-word comparison proves that the two
original 212-byte bodies differ at exactly six arithmetic instructions: three
byte/count updates, the two alternative location counts, and the separately
gated final byte total. All other instructions, branches, reloads and offsets
agree. Each symbol is compiled and compared separately; no matching status or
bytes are transferred between them.

## Complete source and linking evidence

The three compact routines have nine genuine complete GNU linker comparisons,
all with zero unresolved relocations and zero exact matches. The large routine
naturally emits a seven-entry readonly switch table under all three established
recipes. Its `R_MIPS_32` data relocations target its own code section. No approved
mapping is supplied for these objects, so each attempt explicitly reports
unmapped `.rodata` before a complete relocated code comparison. The current
mapper can support separately proved function-local case-label data; this packet
does not establish such a final data identity.

GCC 2.9 emits a complete 976-byte unlinked function and a 32-byte readonly
section; GCC 3.2.3 emits 1,000 bytes and a 28-byte section. These object identities
are recorded, but they are not relocated-data proof or a final code match.
Both actual pinned compilers reject the explicit `-fno-jump-tables` attempt:
GCC 3.2.3 reports an unrecognized option, and GCC 2.9 reports an invalid option.
No output/link comparison is claimed for those two unsupported recipes.

A separate reviewed trial now uses the existing guarded code-derived readonly
mapper for the actual GCC 3.2.3 whole 28-byte table at `0043B0E0`. Its authentic
seven local `R_MIPS_32` pointers and original LUI/ADDIU binding are verified.
Real GNU linker output has zero unresolved relocations: the full 1,000-byte
code differs from the 1,040-byte original in 790 bytes, and the entire 28-byte
table differs in eight bytes. Both complete code and data gate matching credit.
The factory is centrally linked under that genuine GCC 3.2.3 recipe and remains
reconstructed. The original three attempts without a mapping remain recorded;
GCC 2.9's complete 32-byte data section remains unmapped.

The C switch remains unchanged. No mapper exception, invented geometry, table
patch, masked comparison, partial function or assembly fallback is used.

## Resolution, allocation and publication order

The resolver examines providers whose enabled word equals exactly one. It
captures the current provider, resource kind and key before resolving. Provider
kind nine first performs the actual readonly availability query, then uses the
complete original switch for resource kinds zero through six. The commands are
respectively 1 / 5 / 4 / 3 / 0 / unsupported / 2. Other providers resolve only
when their kind equals the captured resource kind. Failures continue against a
fresh global provider count.

After success, auxiliary metadata comes from a freshly loaded manager/provider,
using object offset `+244` for provider kind nine and `+A8` otherwise. The size
is captured before the resource's address store. The allocator then comes from
the current global manager. A live cursor with mode one selects the override
pool or a rounded cursor reservation; override-pool failure and cursor exhaustion
do not fall back to the general heap. Outside that mode, flag `40` tries the
128-byte size class when its index is below 1,000. Missing, frozen or exhausted
ordinary pools fall back to the general allocator using the captured rounded
size. Every addition and rounding operation retains low-32-bit wrap.

The selected allocation is stored even when null. Actual production accounting
then runs before auxiliary publication. The queued request receives eight GPR
lanes and a ninth caller-stack word derived from **exactly** `field1C == 1`.
The field is captured before the auxiliary store; payload, key, address and size
are freshly read for the request. The callback at `00225D38` consumes one record
pointer. The complete queue helper stores it and its context without invoking
it in this call; a floating parameter or synchronous callback is not invented.

After the request, resource kind and key are reloaded for publication. The
map call receives those captured operands. The following count increment
reloads the manager and retains the captured kind index. The unsigned peak
comparison reloads the manager again. Fallback lookup covers only kinds
0 / 1 / 3 / 6. A returned payload publishes flag `10`; a missing or unsupported
payload sets flag `08` and stops before map/count publication.

## Accounting, aliases and observed layouts

The new manager declaration is a partial layout through `+168`; scalar field
names describe access rather than a complete class. It embeds the established
allocator pointer at the same offset as the published prefix. Providers start
at `+100`, but no capacity guard is added. Kind-indexed accesses retain the
original address arithmetic and valid-memory precondition.

Accounting's first and second gates each test the complete incoming word for
nonzero. Between updates, the source reloads record size/kind and the global
manager in the original order. The payload interval test uses its raw unsigned
address and includes both endpoints; the destructor's separate high-nibble mask
is not introduced here. Arithmetic and underflow retain low-32-bit wrap.
Record/manager overlaps can change later loads, and the tests exercise them.

Deletion captures the shifted kind and selected map before calling the helper.
Any nonzero helper result triggers a decrement in the freshly loaded manager's
corresponding count. The peak is untouched. The complete destructor caller
ignores the incidental result; all four recovered declarations are `void`.
Allocation, valid manager/provider pointers and accessible fields are original
preconditions, not newly imposed safety behavior.

## Boundaries, incoming references and data identity

The complete generated assembly agrees byte for byte with the hash-validated
ELF for all four bodies, including actual returns and delay instructions. The
large routine has no following padding; each compact body has four zero
alignment bytes. Previous complete bodies and following entries are recorded
separately. All 19 actual incoming JALs have complete containing-caller hashes,
owned boundaries, argument instructions and return-use evidence. The whole
allocated non-executable aligned address-word scan adds no incoming roots.

The packet audits all local branches, 14 actual callee JALs, 41 scalar/stack
stores, every global manager load, the exact callback materialization and the
queue helper's ninth stack load. Complete immediate callees and supporting
layout/ABI bodies are read independently; their presence in evidence does not
add new recovered-function counts.

The full original 28-byte switch at `0043B0E0` is readonly and has SHA256
`432edbed9f952c1a4eaabbf1f6fcfc33c6b122edbed52ab78a85dd8991aaf911`.
All seven actual targets lie within the large function. The scoped tracer reads
and verifies that complete table locally; no original table or instruction array
is exported. The global manager pointer has proved writable allocated geometry,
without an initializer identity claim.

## Bounded synthetic validation

The scoped tracer inherits the published resource decoder unchanged. Its fetch
scope contains the four selected complete bodies, the readonly availability
query and the published pool primitive. The only permitted switch jump is the
exact `JR4` at `00224B70` to a destination in the verified table. All returns
require actual `JR31` to the selected invocation stop; ordinary untaken branch
delays execute, and likely branch annulment is retained. Memory windows,
alignment, unknown bytes, readonly stores, event capacity and execution counts
are bounded explicitly.

The 561 authored fixtures execute 49,081 original instructions, maximum 212 per
fixture. They cover every switch command and unsupported case, provider
failure/skip/continuation, captured keys and provider metadata, fresh manager
loads, nested accounting, pool freeze/exhaustion, null heap results, cursor
equality/exhaustion, rounding and size-class boundaries, independent accounting
gates, inclusive allocator endpoints, count wrap and unsigned peaks, exact
field-word truth, and shifted record/manager aliases.

Native code executes all four production routines in a separate translation
unit. A macro-only symbol bridge executes the unchanged published one-argument
pool source after observing all three original caller lanes. Availability-query
behavior is a small independently reviewed scalar model. Filesystem resolution,
heap, queue, map and lookup operations have explicit authored call contracts.
Their controlled mutations test caller capture/reload behavior; the fixtures do
not claim that every mutation schedule occurs in the retail helper graph. The
queue completion function is identity-only and aborts if unexpectedly invoked.

Every fixture compares all 4,096 buffer words, the global pointer, twelve call
counters and complete twenty-word observation events. The compact header
losslessly stores full authored input runs and every changed word. Native
validation passes **2,447,135 checks**, without compiler warnings, and nine
focused decoder guards pass. Guard switch data is independently authored,
with a test-only expected-hash substitution; a separate guard proves that
mismatching full data is rejected.

These checks do not execute the full filesystem/queue/map/heap graph or prove
malformed-pointer/count safety, EE upper-register contents, timing, MMI/OS/FCR
behavior. Native storage and its complete extent stay below bit 31 to preserve
the authored unsigned address ordering. No original assets or data arrays are
copied into fixtures.

Input SHA256:
`f785aa7b768488392dab9a41208d1fd2ac277c45288722ec3435fe8660dab410`.
Synthetic header LF SHA256:
`c141035f3de3625bacefca310e366005ec16b152afefa821c0d4920baef919af`.
Windows CRLF SHA256:
`9755cdcdbb49c1750011ecc36eb37c8427f67d2fc8fe3aece9044138e40cfd39`.

```powershell
.venv/Scripts/python.exe tools/trace_resource_manager.py --output build/resource_manager_trace.json --golden-header tests/native/resource_manager_golden.h
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_resource_manager.py -v
```

Compile the native harness with MinGW `bin` prepended to `PATH`:

```text
gcc.exe -m32 -O2 -Wall -Wextra -fno-strict-aliasing -Iinclude tests/native/resource_manager.c src/game/resource_manager.c -lm -o build/resource_manager_native.exe
```

The ignored `.local/resource_manager` packet contains complete original/code/data
identities, selected and helper/caller boundaries, callback/stack/store/global
proof, the six accounting differences, all nine complete comparisons, three
switch blockers, both unsupported flag attempts, full synthetic identities and
the warning-free native command. Independent peer and parent review passed
all selected original/source/scoped validation/proof inputs and fresh complete
packet reproduction. The supplementary actual GCC 3.2.3 code/table comparison
is recorded centrally without exporting original instruction or table arrays.
