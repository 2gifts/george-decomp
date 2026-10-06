# Reuse of identical complete game functions

This batch recovers 61 complete C function bodies covering 9,368 original
instruction bytes. Four already reviewed algorithms provide the source. Each
new original body has the same complete SHA-256 and length as its source seed,
including the entry, control flow, return and delay slot. All 183 new comparisons
under the three standard compiler recipes link at their original addresses
without unresolved relocations. Every new entry remains reconstructed.

| Algorithm | Reviewed seed | New bodies | Original bytes |
| --- | --- | ---: | ---: |
| Indirect upper-bound search | `00100C30` | 58 | 9,048 |
| Chained-map first-key lookup | `002A7C08` | 1 (`00219FF0`) | 80 |
| Chained-map visitor | `002A8130` | 1 (`0021A458`) | 160 |
| Gated timer/action restart | `001E0F50` | 1 (`001E1028`) | 80 |

The 58 search copies have no discovered direct calls or allocated-data pointer
references. Their complete byte identity and adjacent return boundaries establish
the recovered extents: the preceding body ends with `jr ra` and its delay slot,
with only zero alignment padding before the new entry. Every local branch stays
within the complete body. Each copy ends with the same return and stack-restoring
delay slot as the seed. Their original template types, class names and runtime
reachability remain unknown. Counting these source entries does not imply 58
distinct gameplay behaviors. The three other helpers have 37 decoded original
direct `jal` references collectively.

`include/george/algorithm_templates.h` supplies ordinary C definition macros,
used by both seeds and recovered copies. Each expands to a full function body
under its original symbol. The compiler generates the code naturally; there are
no forwarding wrappers, embedded instructions, patched output or byte masks.
The four seed bodies preserve their entire pre-refactor compiler/link candidates
under all three recipes: 12 size, SHA and relocation comparisons against the
published seed source passed.

The search computes a signed entry count from related array pointers. A positive
range is halved; the comparator receives the key payload and midpoint payload.
A nonzero result retains the left half; zero advances beyond the midpoint.
The key payload is loaded again for every comparison, while the midpoint and
range state survive callbacks. Empty and reversed valid ranges return the
initial cursor without invoking the comparator. A sorted-range interpretation
requires a comparator returning nonzero for a key preceding an element.

Map lookup preserves unsigned key modulo bucket count, first-key selection and
null-chain failure. Its original nonzero-divisor and valid-storage preconditions
remain. The visitor captures each key, value and successor before invoking its
callback, so removing the active node does not change the captured successor.
Bucket count and bucket storage are read after each callback chain; callbacks
can therefore affect subsequent buckets.

The action restart captures its entity argument before the original gate call.
On a nonzero result, it reloads the owner and duration, captures them before the
timer store, then sets the owner's 16-bit flag bit and clears the action byte.
This retains callback mutations and overlapping owner/timer storage effects.

The native harness passes 2,375,547 checks. Each search entry runs against an
independent linear oracle over sorted unique, repeated and uniform values and
valid subranges. Focused cases check fresh keys and captured payloads across
mutating comparisons, negative/empty counts, map collision precedence, saved
successors, live bucket changes, gate success/failure, fresh owners and overlapping
flag/timer stores. Most checks repeat one search algorithm across 58 entries;
they do not represent independent gameplay coverage or full EE execution.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness algorithm_duplicates`.
