# Deimos value and table recovery

Seventeen routines in `src/game/deimos_values.c` and
`src/game/deimos_lifecycle.c` reconstruct 2,712 bytes of retail code. All compile
and link with explicit retail bindings. They remain nonmatching, and their
original class/function names are unknown. A second review compared each
control-flow path, access width, and return against the disassembly.

| Address | Observed behavior |
| --- | --- |
| `0x002CD0B8` | Increment a 16-bit reference count; remove newly referenced nodes from an active list |
| `0x002CD130` | Release the last reference and return the node index to the free list |
| `0x002CD248` | Pop a free-list index and append the node to the active list |
| `0x002CD528` | Allocate as above, then store type byte 1 and a 32-bit payload |
| `0x002CD990` | Search a secondary 17-bucket hash table before the local table |
| `0x002CDA20` | Visit local hash entries through an indirect callback |
| `0x002CDB88` | Preserve listed keys; clear other values after releasing references |
| `0x002CDC00` | Apply that clear operation to all local hash entries |
| `0x002CDF90` | Obtain the running script frame's context member |
| `0x002D0560` | Convert the unsigned time counter to floating-point seconds |

The pool node is 16 bytes, with references at +4, a signed active-list slot at
+6, and payload at +12. The first-reference path uses the last active entry
to replace the removed entry, repairs that moved node only when the new count
is positive, and writes -1 to the referenced node's slot. Reference increments
wrap at 16 bits. The original allocation paths do not check exhaustion.

The lifecycle batch adds the type-specific payload destructor and six
constructors for buffers, table buckets, and callable records. Destructor
dispatch was checked against all five original jump-table destinations, with
types 0, 2, 3, and 4 taking distinct release paths. When freeing hash entries,
the destructor saves the next link before releasing values or freeing entries.
This differs deliberately from callback traversal, which reads it afterward.
The reconstructed destructor currently uses explicit type comparisons while
the original jump-table data mapping is unresolved.

Hash entries have a next pointer at +0, a 32-bit key at +4, and an inline 8-byte
tagged value at +8. The iterator reads `next` after its callback and reloads the
bucket array between buckets; retaining these reads preserves callback
mutation behavior. The secondary table's meaning is not yet established.

Global spacing suggests 256 tagged registers and 2,560 pool nodes/list
entries. Those capacities are an inference, so the source declares unsized
arrays. Count arithmetic assumes the valid bounded pool state used by callers;
extreme signed-overflow inputs are not established behavior of the reconstructed
C. Runtime execution and complete class layouts have not been validated.

`src/game/deimos_callbacks.c` also reconstructs the `DVDist` and `DFMod`
callbacks. Retained strings support those interface names. `DVDist` looks up
four keys in each table, including an unused return that is preserved, then
computes a three-component distance with the EE scalar square root. The first
three keys equal the independently confirmed CRCs of `x`, `y`, and `z`.
`DFMod` calls the identified newlib `fmodf` wrapper. Both compute their result
even when no destination register is requested, and continue after logging a
type error if the logger returns. These callbacks add 596 reconstructed bytes;
their exact matching is still pending.

Four script-object routines in `src/game/script_object.c` add 300 recovered
bytes. They install the object's vtable, create and reference its table lazily,
invoke adjusted virtual methods, and release the table during cleanup. The
24-byte constructor matches exactly. Other routines preserve table and vtable
reloads after calls that can change the object. The cleanup key `0x66A3F26C`
equals the confirmed CRC of the retained string `_deimosobject`; its assignment
uses the original tag-6 zero value before releasing the table reference.
