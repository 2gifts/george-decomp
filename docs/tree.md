# Intrusive priority tree

The 17 routines from `002ACDB0` through `002AD400` cover 1,676 original
instruction bytes. The typed C source preserves the 64-byte node prefix and
16-byte controller without assigning an unproven original class name. Four
complete eight-byte subsystem entries match exactly; the substantive constructor,
sorted insertion and recursive destruction routines remain reconstructions.

| Address | Observed operation | Original bytes | Canonical candidate bytes |
| --- | --- | ---: | ---: |
| `002ACDB0` | Bootstrap and construct node | 236 | 244 |
| `002ACEA0` | Recursively destroy node | 188 | 180 |
| `002ACF60` | Set immediate-child flags | 56 | 52 |
| `002ACF98` | Destroy immediate children | 228 | 220 |
| `002AD080` | Move attached node into active roots | 72 | 68 |
| `002AD0C8` | Reparent node | 148 | 144 |
| `002AD160` | Reinsert with new priority | 164 | 156 |
| `002AD208` | Empty subsystem entry | 8 | 8, exact |
| `002AD210` | Insert into current children or roots | 188 | 204 |
| `002AD2D0` | Roots address | 8 | 8, exact |
| `002AD2D8` | Current node | 8 | 8, exact |
| `002AD2E0` | Set current node | 8 | 8, exact |
| `002AD2E8` | Insert into roots | 100 | 96 |
| `002AD350` | Initialize controller | 36 | 36 |
| `002AD378` | Allocate controller | 64 | 60 |
| `002AD3B8` | Destroy roots | 72 | 68 |
| `002AD400` | Destroy roots and controller | 92 | 92 |

Every canonical candidate compiles with pinned GCC 3.2.3 and links at its
original address with no unresolved relocations. GCC 2.9 with the independently
reviewed 128-bit register-save attribute was also compared; it adds no full
match. A 64-byte controller factory candidate differed in four saved-register
stack-slot instructions and remains reconstructed. Complete code and exact size
are the match gate. Alignment padding is excluded and separately checked.

Existing reviewed list initialization, unlinking and owning-head traversal are
called at their original entries. The reused list implementation includes its
licensed AROS behavioral adapters. No upstream identity has been established for
the tree code itself. Local m2c drafts helped identify control flow, followed by
manual complete-instruction review and explicit field types. No generated error
placeholders or assembly fallback bodies remain.

Node construction bootstraps `003FD1E8` and `003FD1EC` in order, then assigns
the first controller to `003FD1E4` only if the active controller is null. The
node allocation request wraps `data_size + 64` as a 32-bit word and passes
alignment value 6. A nonzero data-size argument selects the address immediately
after the node even if that request wrapped to zero. The original initializes
only links, attachment, callback pointers, data pointer, priority 2000, elapsed,
remaining, flags and the child list. Fields at `28`, `38` and `3C` stay untouched.
The active controller is reloaded after allocation before insertion. Bootstrap
allocation failures are not given new guards.

The callback prefix is independently visible in the preceding `002AC820`
dispatcher: integer arguments are the node and its data pointer. Constructor
callers supply callback addresses such as `00154CD8`, `0021CB40` and `0027B990`.
The recovered range contains no virtual-table construction or assertion
diagnostic; neither is invented from surrounding metadata. Additional floating
register values inherited by a particular dispatcher are outside this minimum
two-argument callback prefix.

Insertion compares signed 32-bit priorities in ascending order. New equal
priorities precede existing entries; sentinel priorities are never read. The
intrusive stores retain the original intervening pointer reloads. A current
child insertion reloads `controller->current` after link writes to set its
attachment marker. The explicit reparent routine `002AD0C8` writes the inserted
node itself into that marker. This surprising self marker is preserved.
Reprioritization captures the owning head before removal and retains the marker;
moving to roots detaches only when the marker is nonzero. Valid linked-node and
controller invariants are unchanged.

Destruction clears the active current pointer when it equals the node, unlinks
a node whose next or previous pointer is nonzero, then calls its captured
destruction callback with the current data pointer. It reloads the child head
after that callback, recursively destroys children and releases the parent
last. Child/root loops capture the immediate successor before destroying each
node, and read that successor's next after callbacks and release. A callback
can therefore add a preceding root that this pass leaves behind or add children
that this pass destroys. The immediate-child routine inlines the complete
destruction sequence for its first level and retains recursive calls for deeper
descendants. Controller teardown releases the controller only after roots.

`tests/native/tree.c` passed 8,052 checks using the actual recovered tree source
and the reviewed list sources. It exercises all 120 five-node insertion
permutations with extreme signed priorities and ties, list sentinel consistency,
reparenting, marker preservation, flags, controller bootstrap and failure,
wrapped allocation size, untouched fields, callback/free order, callback-created
children, captured successors and active-controller changes. Tests use synthetic
nodes and allocator/callback stubs; original game bytes are not embedded. Run
`.venv/Scripts/python.exe tests/native/run_utilities.py --harness tree` with the
configured native 32-bit compiler.

The root reviewer independently checked all 17 complete original instruction
bodies, the callback ABI, shared layouts and sentinel/reload ordering. No defects
were found. The focused native harness passed all 8,052 checks with zero failures.
