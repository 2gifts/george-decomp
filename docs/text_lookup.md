# Text lookup, classification and initialization

Four complete routines recover 572 original instruction bytes. All targets
naturally link with no remaining relocations under the three standard compiler
recipes. No complete candidate matches the original bytes; every entry remains
reconstructed. The canonical pinned GCC 2.9 recipe uses the reviewed callee-save
attribute where applicable and does not identify retail's compiler.

| Address | Observed operation | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B4458` | Sparse parser storage initialization | 80 | 80 | 80 |
| `002B44A8` | Name-to-code table scan with sentinels | 140 | 140 | 144 |
| `002B45A0` | Classification-backed in-place lowercasing | 80 | 80 | 76 |
| `002B45F0` | Ordered case-insensitive value lookup | 272 | 244 | 240 |

The new offset-asserted storage view models four 4-KiB buffers at `+1C`, `+1030`,
`+2030` and `+3030`, plus the observed scalar words; its extent is `0x4030`.
This does not assert a complete engine class. The earlier reviewed token-context
prefix is unchanged. The initialization routine takes three integer-register
inputs and `f12`; complete callers at `00232ECC` and `0023A8F0` independently
establish that ABI. It stores the float, captures the input pointer in both
base/cursor fields, writes length/default code, and zeros only the observed
words and first bytes. Other buffer bytes remain untouched. It returns one.
The field/store order follows the full body instead of substituting a bulk clear.

The code table consists of 8-byte code/pointer records. Every scan first writes
`0x8001DEAD` as the default code. A null table returns with that default; code
`0x8000DEAD` ends the table, while `0x8002DEAD` is stored immediately. Other
records compare the context string at `+1030` with their name. A successful
comparison reloads the record's code after the call. An unsuccessful comparison
advances eight bytes and reads the next record afresh, so comparison-side record
mutations remain visible. Retail supplies no independent table length or null
name guard before non-sentinel comparisons.

The lowercase helper reuses the existing unchanged public newlib 1.8.1
`src/runtime/ctype_table.c` table. The full 257-byte object was independently
proved equal to original readonly data at `00456118`, SHA-256
`8b55a0d9c781d7001042c99e21523fef362440f572faef950e8e600fae5de813`;
the observed `00456119` lookup is exactly `_ctype_ + 1`. No table is copied from
the disc, no duplicate implementation is imported, and the existing licensed
source/notices remain intact. See `docs/reuse.md` for its pinned provenance.
Each nonzero input byte is sign-extended before adding one to the table index.
Bit one selects addition of `0x20`; the resulting low byte is stored in place.
This is not replaced with an unsigned-byte index or a new ASCII range gate.
The proven table supports signed input values zero through 127 and minus one
(byte `FF`, the EOF entry). Bytes `80` through `FE` would index before the
proved table; those accesses lie outside the reconstruction's valid-index
precondition and are not invented as deterministic table entries.

Value lookup first scans name/value pairs in the context's `+2030` string using
the already reviewed `002B3EE0` helper, selecting the first matching key. If
none matches, it compares the key with the whole context name at `+1030` and
copies the whole `+3030` string on success. Only if that comparison fails does
it scan `+3030` as a second name/value list. Duplicates therefore preserve both
source precedence and first-match behavior. Missing values yield null from the
token helper and are not compared. No match returns zero with the output
untouched; success copies and returns one, ignoring the copy callee's return.

The original `003983E8` comparison is case-insensitive: its complete body calls
the independently identified original `tolower` at `0039CA58`, compares while
the first string byte is nonzero, then returns a lowercase byte difference.
The reuse agent independently identified this complete 124-byte algorithm as
public newlib 1.8.1 `strcasecmp.c`, including signed-byte loop inputs and unsigned
final-byte conversions at the pinned [upstream source](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strcasecmp.c).
The original `00393B74` complete body establishes string-copy argument/return
behavior. This batch retains both numeric bindings; the separate runtime agent
owns any unchanged licensed comparator import. Scratch name/value buffers retain their observed
`0x400` capacity, without inventing truncation or bounds checks. Returned scan
cursors remain captured across comparisons; context strings used by later
fallbacks are read after the intervening calls.

The native harness passes 66,408 checks. It compiles actual recovered
`text_lookup.c` and `text_tokens.c`, plus the unchanged licensed ctype table
through a macro-only native adapter. Coverage checks every initialized-context
byte against independent sparse-write expectations, untouched buffer storage,
signed-zero float initialization, all valid ASCII classification values and
EOF, sentinel/default/matching codes, initial table/context aliases, fresh
code/next-entry mutations, ordered
duplicates/fallbacks, missing/empty values, callback-mutated strings, unchanged
output on misses, key/output and context/output aliases, and ignored copy returns.
Case-insensitive comparison and copy use controlled ABI substitutes; they do
not emulate the original vectorized string copy, hardware exceptions or timing.
String/table lengths and output/scratch capacities must remain valid, as required
by the original routines. Existing source/reviewed helper reuse is explicit;
no original instruction arrays or parser buffers are tracked.

The parent independently reviewed all 143 original instructions, all four
source bodies, header layouts, native adapter/harness and documentation without
finding a defect. The ordered lookup now uses the shared ordinary C template
also consumed by `text_values.c`; all twelve full candidate comparisons across
the four existing entries and three recipes remain byte-identical to the
pre-refactor baseline, and the same 66,408 native checks pass.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness text_lookup`.
