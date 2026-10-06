# Typed text values and depth traversal

Four complete routines recover 1,144 original instruction bytes. All three
standard recipes naturally link every complete target without remaining
relocations. None equals the retail code bytes, so every entry remains
reconstructed. The canonical GCC 2.9 recipe with reviewed callee-save attributes
does not identify the original compiler.

| Address | Observed operation | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B4188` | Boolean word lookup and four accepted-value comparisons | 392 | 392 | 380 |
| `002B4310` | Three-float scanner lookup | 328 | 340 | 320 |
| `002B4538` | Wrapped signed-depth accumulation and EOF traversal | 104 | 116 | 104 |
| `002B4700` | String lookup with optional original lowercase call | 320 | 320 | 304 |

The first, second and fourth entries reuse the complete ordered lookup from
`text_lookup.c`: first matching attribute in `+2030`, whole-name/content fallback,
then first matching content attribute in `+3030`. The reviewed name/value helper,
numeric original case-insensitive comparison and original copy calls remain
intact. `text_lookup_template.h` contains that ordinary C loop sequence, with
success/missing control-transfer actions. The legacy entry expands to its
previous early-return body; the new entries finish their own typed conversions
after lookup. No new helper call, assembly body or output patch is introduced.
Before refactoring, all four existing text-lookup entries and all three recipes
were preserved. All twelve complete candidate comparisons, including sizes,
SHA-256 hashes and unresolved-relocation fields, remain identical afterward;
the existing 66,408 native checks also pass.

The boolean routine writes output zero before any lookup. On a found value it
compares, in order, against original readonly strings `yes`, `on`, `true` and
`1`, case-insensitively. All four calls occur even after a match; a matching
call stores output one. The returned status reports lookup presence, so a
present false value still returns one with a zero output. Initial output writes
can affect aliased key/context storage, and intervening comparison mutations
remain visible. The original string addresses are retained as external bindings,
not copied into recovered source.

The vector routine initially clears its three output words, then looks up text
into its own `0x400`-byte buffer. A found value calls original `00395350` with
the readonly `%f %f %f` format at `00447730` and three consecutive float pointers:
five integer-register arguments in total. The scanner's return count is ignored.
All three scalar results are loaded before the original X/Z/Y output store
order. No match leaves three positive-zero bit patterns. Retail does not
initialize the local parsed lanes: partial/failed conversion leaves unwritten
lanes as indeterminate stack data, which this reconstruction does not invent
as zeros or another deterministic value. Reading such an unwritten automatic
lane is outside defined C semantics. Native tests fully write all three lanes;
varying the controlled return count verifies that it is ignored without claiming
deterministic results for real partial conversion.

The original scanner inventory groups later internal routines after this entry;
only the inspected first call ABI is used here. No scanner implementation or
oversized recovered-function extent is registered by this batch. The scoped
first entry returns at `003953E0` with its delay slot at `003953E4`, before the
next prologue at `003953E8`.

The string routine clears only output byte zero before lookup. Success optionally
calls original `003984D8` on the local text when its integer flag is nonzero,
then copies that text to the captured output pointer. The inspected complete
124-byte lowercase callee uses the same signed-index public character table
and original `tolower` binding already identified by runtime recovery. Its
returned pointer and both copy returns are ignored. No match leaves the empty
output start while subsequent bytes remain untouched.

The traversal routine starts with the wrapped word difference `+101C - +1020`.
While its signed 32-bit interpretation is positive and `+102C` remains zero,
it calls the original larger parser `002B3460(context, NULL)`, reloads both
count words, and adds their wrapped difference to the captured accumulator.
It does not replace that accumulator with the latest difference. Word subtraction
and addition use unsigned C arithmetic; only the original positivity predicate
uses the signed interpretation, preserving overflow without signed-C arithmetic
undefined behavior. The parser's return is ignored. The original parser remains
unrecovered in this batch, and terminating-count/EOF behavior is a runtime
precondition; no loop limit is invented.

All readonly literal identities were independently read within the validated
original section. Their complete NUL-terminated hashes are:

| Address | Bytes | SHA-256 |
| --- | ---: | --- |
| `004476D0` | 4 | `357c2320b43a6c0482a5da2f108e4de667fbf36fcb00d4a8a8c1e7b47f914dd7` |
| `004476D8` | 3 | `7396b2dfcc8e6f5c6b317af0ce5e1279b04f424d002adabc4c0aeb25a7b416f7` |
| `004476E0` | 5 | `debc2f07db78d52d2def07b7bc620d7042367501d9439a62ba09b559a98e0957` |
| `004476E8` | 2 | `e79e418e48623569d75e2a7b09ae88ed9b77b126a445b9ff9dc6989a08efa079` |
| `00447730` | 9 | `9d9949291b3b8e3c232368c93b65e79990accb06a9529b2849f12be443b13192` |

The native harness passes 454 checks using actual recovered name/value code and
the shared C template. It covers all three lookup precedences, duplicate values,
truth/false/empty values and case variants, all four truth calls and intervening
output mutation, initial output/key/context aliases, scanner pointer ABI and
fully written scalar capture, ignored return counts, optional lowercase/copy
order and ignored returns, cleared missing outputs, untouched trailing storage,
wrapped signed accumulators and callback-updated EOF/counts. Controlled scanner,
parser, comparison, lowercase and copy substitutes test the caller algorithms;
they do not emulate library vectors, hardware exceptions, timing or undefined
unwritten stack values. Original string/table termination and scratch/output
capacity requirements remain explicit, without invented guards.

The parent independently reviewed all 286 original instructions, four complete
source bodies, headers and shared finish-action template without finding a
semantic or call-order defect. Exact status still requires full target byte
equality; no such match is claimed for this batch.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness text_values`.
