# Connected curve selection and evaluation

Seven complete functions are reconstructed from 3,740 original bytes / 935
instructions. Their entries have 85 actual incoming JAL references, each with
the entire containing original body and delay instruction identified. The
numeric names describe addresses; no original class or proprietary API names
are assumed. Author, independent peer and parent reviews passed; all seven are centrally
registered. Canonical checkpoint verification passed; all seven remain
reconstructed because their complete natural links differ.

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `00295EB8` | 1700 | Select adjacent endpoints, clamp or wrap time, and write fraction |
| `00296560` | 396 | Scan adjacent segments with query and squared-distance helpers |
| `002966F0` | 1120 | Choose closest callback result, retain boundary invalidation flags |
| `00297178` | 144 | Evaluate first format callback column with mutable position |
| `00297208` | 136 | Evaluate second callback column with private mutable time |
| `00297290` | 136 | Evaluate third callback column with private mutable time |
| `00297318` | 108 | Evaluate endpoint vectors using the published spherical interpolation |

The record prefix reuses `GeorgePathRecordHeader`. Count is unsigned 16-bit,
stride is an unsigned byte measured in four-byte words, and flag accesses retain
their byte offsets. Format 16 reads an unsigned time byte multiplied by 160;
the other selected paths read a float. Offset arithmetic preserves the original
low-32-bit multiplication, shift, addition and subtraction.

`295EB8` distinguishes one, two, zero and larger counts. The two-endpoint high
wrap uses `remainder(time-base, period)`, whereas the larger-count high wrap
uses `remainder(time-base+period, period)`. Searches retain strict comparisons,
fresh header reads after stores, and the final fresh flag check before writing
the adjusted time. The original lacks a capacity or zero-span guard. No new
guard, initialization or alternate callback is added.

`2966F0` writes distance before position and captures callback outputs before
stores can alias the resource or reference. Its signed range arithmetic wraps
in 32 bits, with an unsigned upper clamp and signed lower clamp. Boundary flags
accumulate across selected candidates; a final invalid result retains the
chosen outputs and optional index. Both 136-byte wrappers reuse one ordinary
C template after full original reads establish identical argument/load/store
sequences with only the actual getter target differing.

The getters prove three 12-byte callback columns at `448518` and a separate
projection pointer prefix at `4485F0`. Evidence hashes the concrete 216-byte
and 72-byte observed prefixes and all 18 records. Only formats 5 and 9 have
non-null projection pointers in that prefix. The getters have no capacity
check; these observations do not establish arbitrary-format validity or an
original object-size boundary. Original table data and callback functions remain
external bindings. No original arrays or assets are copied into production C.

The author read all seven selected originals and 34 complete supporting bodies:
the four getters, 22 distinct table targets, the normalization/spherical helpers,
query/distance helpers, remainder wrapper/core and two trigonometric entry
contracts. Supporting reads establish narrow integer/FPU argument and output
contracts; they receive no additional source or matching credit. Callback output
width varies by format and includes a true no-op. The parent also completed all
935 selected instruction/source/header reads without finding a defect. Independent peer and parent final reproduction passed: seven complete packet
dictionaries, all21 full links,755 fixtures,397158 warning-free checks and six
guards. All31 frozen inputs and the parent initial source/header remained unchanged.
Tracked source using only central bindings reproduces all21 full comparisons.

All 21 natural whole-function comparisons under the three pinned recipes link
with zero unresolved relocations. None equals the retail body. The seven entries
remain **reconstructed**, with complete size/hash/difference dictionaries retained
in the isolated packet. No forced placement, patched instruction, masked comparison
or partial-byte award is used.

The strict scalar harness runs 755 authored fixtures through the complete selected
originals and actual `2A3390` / `2A3538` instruction bodies: 113,140 instructions,
at most 357 per fixture. Native tests execute both actual production translation
units and compare the complete 512-word authored memory window, returns, and call
observations. They pass 397,158 warning-free checks. Six decoder guard tests cover
scoped initialized memory/code, signed compares and conditional moves, operand
validation, ordinary/likely delays, unknown calls and bounded transfers.

```powershell
.venv/Scripts/python.exe tools/trace_path_curves.py --golden-header tests/native/path_curves_golden.h
.venv/Scripts/python.exe tests/test_trace_path_curves.py
.venv/Scripts/python.exe tests/native/run_path_curves.py
```

The numerical model is finite ordinary single arithmetic in authored low32
memory. Getters, format callbacks, query/projection, remainder and engine trig
functions have explicit controlled observation contracts. Actual vector helpers
execute production C and original instructions, but the harness does not claim
retail callback/trig approximations, global error handling, exceptional EE/FCR
arithmetic, cycles or exact stack placement. Invalid/null columns, zero spans and
out-of-memory scans remain unguarded retail paths and are outside fixtures. Safe
projection count-zero bounds are exercised; overflowing ranges that leave the
authored memory are excluded. Pointer slots translate native addresses, so no
numeric claim is made for aliases that reinterpret pointer encodings as floats.

The checkpoint passed 331 tooling tests (one platform-specific symlink skip) and
a hybrid build with the retail executable SHA-256
`01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`.
A later decoder review added encoding-based delay checks, including untaken
branches. The selected retail bodies contain no control transfers in delays.
The reviewed tooling baseline was explicitly refreshed; source, compiler
comparisons and synthetic golden outputs remained unchanged.
