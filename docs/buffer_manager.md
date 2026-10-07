The three connected buffer-manager routines cover 912 complete original bytes:

| Entry | Bytes | Behavior | Candidate result |
|---|---:|---|---|
| `002A4B70` | 584 | Signed character, history submission and command callbacks | Reconstructed; complete linked candidates differ |
| `002A4DB8` | 312 | Terminated text append and line rotation | Reconstructed; complete linked candidates differ |
| `002A56C0` | 16 | Raw high flag bit | Whole GCC 2.9 candidate matches |

The optional 2,408-byte UI proposal is excluded. Names describe observed roles;
the header models only field prefixes. It establishes neither original classes
nor storage capacities. The immutable source-free proposal and root approval
remain under `build/buffer_manager_scope` and
`.local/buffer_manager_scope_approval.json`.

Submission captures the current line and text before calling length. History
rotation uses fresh capacity and array loads, clears the retained text, then
stores count before navigation. After the actual copy it stores navigation
before count. Line rotation uses the loop index for its final slot, clears the
retained line's count, reloads its text pointer, and publishes current last.
Both rotations read a first entry even when their signed count is below one.
The command loop keeps the captured old line, reloads its text after search,
and reloads the command-list pointer and signed count after each callback.
Callback arguments are manager, matched text and offset-4 data in three GPRs.

Text append performs no lowercase/history processing. It appends the byte,
stores incremented count, reloads the data pointer for the terminator, then
freshly reads limit/count. Input advances before a full-line rotation and is
fetched anew on the next iteration. This preserves input/output and field
aliases. Increment and array-address products/sums explicitly wrap to 32 bits.
No extra capacity or null guard is present.

Single-character classification retains signed base-plus-byte addressing:
`456118 + 1 + signed character`. Negative characters below EOF address bytes
before the declared ctype object. The target retains that readable-address
precondition. Native and original-observer classification tests cover only
nonnegative signed bytes and EOF inside the proven 257-byte table. They make
no claim about adjacent ROM bytes and supply no fabricated prefix. Zero input
is still appended if the signed count gate permits it.

Native validation executes the byte-exact complete published length function
from `src/game/string_algorithms.c`, the entire unchanged `src/game/accessors.c`
count implementation, and complete unchanged `src/runtime/strstr.c` and
`src/runtime/ctype_table.c` translation units. The isolated length TU retains
the exact raw function span; the unrelated CRC functions are omitted because
the PE linker would otherwise demand their globals. Entire source, span and
generated-TU hashes, explicit compiler symbol aliases, actual commands and
successful compiler `-M` closures record this limited function-source reuse.
It is not a claim that the whole string-algorithm TU was compiled natively.

The supporting generic `strncpy` is an unchanged raw Git blob from the pinned
[newlib source](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strncpy.c),
SHA-256 `02668aeb2461c074f110b2cdda739cb0346a239196c23bf38f78db0917084da2`.
It is written only to ignored native output and compiled with the authentic
`PREFER_SIZE_OVER_SPEED` selector. Its original source is unchanged; the
default Cygnus 1994/1997 clause 9 in `LICENSES/newlib-1.8.1.txt` applies. The
already published `strstr` raw source comes from the same pin. Ctype retains
its complete Berkeley notice. This product includes software developed by
the University of California, Berkeley and its contributors.

These supporting sources receive no new code award. In particular generic
`strncpy` is not claimed as the retail implementation. The original optimized
`394010` executes its real alignment gate and byte/padding path under
deliberately unaligned or small-count, nonoverlapping fixtures. Aligned bulk,
MMI/overread and overlapping copy paths are rejected by the observer; production
still binds the genuine numeric original. Unknown indexed helper `2AAF50` also
executes original instructions. Its native implementation is explicitly a
faithful bounded low-word observer, not recovered helper source or an SDK
identity.

There are 867 authored fixtures, 165,053 executed original instructions and
2,233,020 warning-free native checks. All 2048 authored memory words, actual
helper/callback arguments and invocation counts are compared per fixture.
The sole unexecuted selected site is the unreachable internal NOP at `2A4CEC`,
retained in the complete original extent. Pointer and function translation
applies only to listed storage cells; other words remain literal. Partial
pointer-byte aliases require the disclosed 256-byte-aligned native arena.
Fixtures exercise signed count gates, EOF/uppercase/zero input, history
capacity mutation, loop-index line rotation, captured/fresh callback state,
changed command lists and counts, fresh old-line text, source/output overlap
in append, and fresh pointer/count loads after stores.

Both actual target compilers emitted the expected 24 layout words and eight
complete ABI callers, proving signed low-byte input, one/two GPR entry calls
and three GPR callback calls without COP1 lanes. Twelve successful compiler
`-M` closures pin 30 actual inputs. Nine strict guard methods pass; a fresh
import with only the exact original-file presence mocked absent runs eight
synthetic methods and skips the one private-original method. The tests require
neither an original game file nor an ignored build directory in public CI.

The author read all 228 selected instructions, all 171 primary helper
instructions, the complete source/header and every primary incoming argument
window. A fresh five-executable scan reproduces all 23 actual encoded entry
references with complete containing-caller identities. This is bounded caller
ABI review, not a claim to have semantically reviewed all large callers or
proved complete indirect reachability. The original ctype table has full
257-byte SHA-256 proof; no original game table/code arrays enter the fixtures.

Root's early manual read covered the initial full C/header and all 228 original
instructions without a mandatory defect. Its raw fingerprints are retained
in `initial_parent_read.json`. The subsequent disclosed refinement uses
explicit low32 integer-address arithmetic for shifted array slots and signed
ctype displacement. Final independent peer and parent review passed, preserving all216 frozen inputs unchanged.
The complete16-byte flag getter matches under GCC2.9 and its SAVE128 recipe.
The two larger functions remain reconstructed after whole-function comparisons.
Larger bodies have complete natural links with no unresolved relocations and
remain nonmatching. No function cropping, relocation masking or patched code
is used. Finite initialized low-word observations do not establish EE upper
register/FCR behavior, faults, concurrency, callback implementations or whole
game behavior.

From the workspace root:

```powershell
.venv/Scripts/python.exe tools/trace_buffer_manager.py --header tests/native/buffer_manager_golden.h
.venv/Scripts/python.exe tests/native/run_buffer_manager.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_buffer_manager.py
.venv/Scripts/python.exe build/buffer_manager/probe.py
.venv/Scripts/python.exe build/buffer_manager/abi.py
.venv/Scripts/python.exe build/buffer_manager/native_command.py
.venv/Scripts/python.exe build/buffer_manager/dependencies.py
.venv/Scripts/python.exe build/buffer_manager/missing_original.py
.venv/Scripts/python.exe build/buffer_manager/finalize_draft.py
```

Private reproduction must preserve the canonical read-only ABI source filename
for ECOFF debug identity and change only output prefixes. Historical scopes,
production inputs and upstream source paths remain immutable; no canonical
manifest, shared runner, README or provenance file is changed by this batch.
