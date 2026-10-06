# Direct numeric text lookups

Twelve complete routines reconstruct 3,808 original instruction bytes, all 952
instructions and every return delay slot. They reuse the already reviewed
ordinary C lookup template unchanged: first matching attribute in `+2030`,
whole-name/content fallback, then first matching attribute in `+3030`. Each
entry owns its three original `0x400`-byte scratch strings and preserves the
original token, case-insensitive comparison, copy and scanner call addresses.
No new library implementation, engine call or shared-source refactor is added.

| Address | Direct scanner outputs | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B4840` | One signed word | 288 | 300 | 280 |
| `002B4960` | Two signed words | 304 | 316 | 296 |
| `002B4A90` | Three signed words | 320 | 332 | 312 |
| `002B4BD0` | Four signed words | 336 | 348 | 328 |
| `002B4D20` | One float word | 288 | 300 | 280 |
| `002B4E40` | Two float words | 312 | 320 | 304 |
| `002B4F78` | Three float words | 328 | 340 | 320 |
| `002B50C0` | Four float words | 352 | 360 | 344 |
| `002B5220` | One signed halfword | 288 | 300 | 280 |
| `002B5340` | Two signed halfwords | 312 | 320 | 304 |
| `002B5478` | Three signed halfwords | 328 | 340 | 320 |
| `002B55C0` | Four signed halfwords | 352 | 360 | 344 |

All 36 complete candidates naturally link at their original addresses with no
remaining relocations. None matches the full retail code bytes; all entries
remain reconstructed. The canonical pinned GCC 2.9 recipe uses the reviewed
authentic callee-save attributes, without claiming retail compiler identity.
Excluded following alignment bytes were independently checked zero. Several
overloads have no direct JAL reference in the full original text section; their
boundaries are supported by complete adjacent prologues, coherent register
capture/call sequences and matching stack restoration/return delays. Metadata
records actual direct entry references where present, without inventing calls
for the otherwise unreferenced overloads.

The three families intentionally initialize outputs differently. Signed-word
entries clear only the first output word, even when there are two, three or
four outputs. Float entries clear every output word in argument order, using
the observed positive-zero bit pattern. Halfword entries clear every output
halfword in argument order, retaining subsequent bytes in surrounding storage.
All these stores precede lookup, so outputs aliased with the key or context
strings can change which lookup path succeeds. A missing key performs no copy
or scanner call and returns zero with precisely these initial stores.

Success calls original `00395350` on the captured local text with one through
four captured output pointers. The two fixed text/format arguments plus those
pointers occupy three through six integer argument registers. The original
pointer order is preserved for every overload, including the different saved
register allocation used by the three/four-output float and halfword variants.
Readonly formats are `%d`, `%f` and `%hd` repeated with spaces. Original literal
addresses and complete NUL-terminated data hashes are retained as metadata;
source uses external declarations and does not copy original data.

The scanner writes directly to the caller's outputs and its conversion count
is ignored. The wrapper returns lookup presence rather than successful numeric
conversion. A failed/partial scan leaves each unwritten output at its current
value: the first word zero and other prior words for signed-word entries,
all initially zero words for floats, or initially zero halfwords for shorts,
subject to intervening callback writes. This differs from the separate reviewed
`002B4310` vector helper's uninitialized local parse lanes: these twelve entries
introduce no extra uninitialized temporary results. Aliased output pointers are
passed unchanged, preserving the scanner's sequential writes to the same or
reordered storage. The original copy return is also ignored.

Valid NUL-terminated key/context strings, scratch/destination capacities and
properly aligned writable outputs remain caller preconditions. No missing-key
or conversion-result guard is added. The shared lookup/token limits documented
in `text_lookup.md` and `text_tokens.md` still apply; these wrappers do not
repair overlong or malformed input. The scanner remains an original numeric
binding, whose full runtime implementation is outside this batch.

The scoped native 32-bit harness links the actual recovered name/value helper
and shared source template. It uses controlled compare/copy/scanner calls; the
scanner validates the exact format identity and every typed variadic pointer,
then runs host `vsscanf` to exercise real partial writes. It substitutes reported
counts of minus one, zero and seven after those writes, proving the wrappers
ignore that result. Integer/float/halfword values, all three lookup precedences,
first duplicate selection and misses are exercised with complete, partial and
invalid numeric strings. Output storage is distinct, completely shared or
reordered. Comparator callbacks change the first output before scanning, and
scanner callbacks change the last output before conversion; unwritten values
must preserve those changes. Additional cases alias initial output stores with
the key, a later output with the key, and context attribute storage.
All 159,630 checks pass. The harness models initialized ordinary scalar memory
and controlled call behavior; it does not assert retail scanner exception,
vector, hardware timing or out-of-range conversion semantics.

Parent independent review passed all 952 original instructions and the full
source/header/shared-template/native/docs packet. The reviewer independently
audited all twelve full hashes, boundaries and local branches, 77 encoded
actual direct JAL references, and twelve complete readonly format hashes. All
36 fresh isolated natural links reproduce the recorded comparison data exactly
after JSON parsing, and the independent native rerun passes 159,630 checks.
