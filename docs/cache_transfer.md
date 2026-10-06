# Cache transfer and copy wrapper

Two complete routines recover 584 original instruction bytes. Every candidate
links naturally at its original address with zero unresolved relocations under
the three standard recipes. Neither passes complete byte equality, so both
remain reconstructed. The canonical recipe uses pinned GCC 2.9 with the reviewed
128-bit callee-save attribute; it does not establish the original compiler.

| Address | Observed operation | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B2F40` | Backward key lookup, insertion and overlap invalidation | 524 | 456 | 516 |
| `002B3150` | Original copy call and wrapped endpoint return | 60 | 64 | 60 |

The observed table at `00469E00` contains ten 12-byte records: source word,
offset word and byte-count word. The cursor is `003FD250`; the offset is
`003FD24C`. Field/size assertions describe only this observed layout. The cache
lookup starts at the cursor's predecessor modulo ten and walks backward while
source words remain nonzero. A matching source pointer returns immediately,
even when the requested byte count differs from the cached count. A zero source
word ends the search, so a key behind a hole is not consulted.

On a miss, the original wrapper copies first. The cache routine then reloads
both global cursor words, writes the selected record's source/offset/count and
reloads its stored offset. It scans the preceding live run and clears source
words whose raw unsigned endpoints satisfy the two original strict comparisons.
Clearing a record leaves its offset/count unchanged and does not immediately
stop traversal: the routine advances to the next predecessor before testing
its source. This differs from a scan that stops on the just-cleared record.

Endpoint addition wraps at 32 bits; the reconstruction neither splits a wrapped
interval nor substitutes a circular-range overlap rule. The strict comparisons
also retain retail's zero-length behavior: an empty new range strictly inside
an older nonempty range can still clear that record, and an empty old range
strictly inside a new nonempty range can likewise clear. Endpoint equality does
not satisfy overlap. After scanning, the cursor reloads once more, advances
modulo ten, and the fresh offset advances by the wrapped 64-byte-rounded size
then masks to 12 bits. The global offset store precedes the final cursor store.

The copy wrapper retains the observed destination expression
`offset + 0x11000000` and original `003934F8` call with three integer-register
arguments: destination, source, byte count. It returns the wrapped sum of its
captured offset and count, ignoring the copy callee's return. This batch asserts
no undocumented hardware-memory identity and imports no SDK implementation.
The numeric copy binding is reused from other recovered subsystems. Actual
callers at `0011D3D8` and `0011D3F8`, plus the inspected copy callee, independently
establish the wrapper's three-argument ABI.

Pinned m2c drafts assisted recovery, but its clean wrapper draft omitted the
unchanged source/size argument registers. The manually typed source restores
those arguments from complete instruction and caller evidence. No decompiler
placeholder, function-body assembly or byte patch is included.

Runtime preconditions remain explicit. The insertion cursor must select slot
zero through nine. A miss search must encounter a zero source word, and the
post-insertion scan must eventually encounter a hole or a cleared record.
Retail has no full-cycle guard; a full live table lacking the necessary hit/hole
can loop indefinitely. No invented limit or guard is added. The original copy
callee retains its normal valid-memory/length requirements; this wrapper does
not add a split transfer or a bounds check.

`tests/native/cache_transfer.c` passes 12,540 checks. Controlled copy calls record
the destination/source/count without dereferencing the numeric memory address.
Coverage includes pointer hits with changed sizes, every cursor slot, holes,
64-byte rounding and 12-bit offset wrap, overlapping/nonoverlapping positive
intervals, zero sizes, unsigned endpoint overflow, continuation beyond a cleared
record, preserved metadata, null keys and callback-mutated global/table state.
Large/wrapped inputs exercise integer algorithm behavior with the controlled
substitute, not physical memory transfers. No hardware-memory behavior is
emulated, and no original instructions/assets are embedded.

Independent full-instruction review of both bodies and the header found no
defects in lookup, capture/reload order, endpoint comparisons, traversal,
rounding/wrap or the copy ABI. Full-table and valid-slot preconditions were
independently confirmed.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness cache_transfer`.
