# Heap allocation and release

Twenty-four routines in `src/game/heap.c` recover 2,860 original bytes. Ten
complete C functions match their linked retail code and exact sizes (240 bytes).
The larger allocator and related helpers remain reviewed reconstructions.
The original engine source identity has not been established; the observed
circular coalescing pattern alone is insufficient to import a different allocator.
The shared allocation macro reuses this batch's proven wrapper pattern.

The 64-byte heap header contains a 24-byte name, next heap, flags, owner,
total/used/minimum-free counters, alignment exponent, free-list cursor and an
initial eight-byte free block. Free blocks carry next pointer and payload size;
allocated blocks use the same first word for their heap owner. Numeric pointer
operations explicitly use unsigned32-bit guest addresses.

`002ADF60` redirects small requests to observed size-class heaps unless the
override depth is nonzero. Its complete circular scan chooses the smallest
suitable free block, preserving the first encountered equal-size block. It
allocates from that block's end after alignment, shrinks its free payload and
writes the allocated owner/size header. The cursor becomes the scan predecessor
of the original cursor, independently of the selected block. It then updates
used bytes, reports overflow, reloads counters after that callback, and updates
minimum free space. Failed allocation reports the original size/alignment and
returns null. Alignment shifts preserve the R5900 low-five-bit shift behavior.

`002AE158` inserts the freed header between address-ordered free blocks,
including circular wrap, subtracts the occupied span and coalesces adjacent
blocks on either side. It retains the insertion predecessor as cursor. Its
unconditional entry has no null guard; only the observed optional-free wrappers
add that guard. No block ownership checks or extra allocation fallback are added.

Allocation wrappers use the original strict unsigned threshold, not an inclusive
capacity check. They reload a heap's next pointer after the allocation callback,
including successful allocation. Failure reporting preserves that final cursor.
The variable-alignment wrappers use overhead `(2 << exponent) + 8`; fixed
variants use alignment3/overhead24 or alignment4/overhead40. The override stack
increments depth before saving a nonnull heap; a null argument pops only when
depth is nonzero. Index bounds retain the original caller contract.

Size queries mode1/2 return largest/smallest free payload minus8, scanning the
cursor block too. Other signed modes return total minus used. The initialization
routine creates the circular first block and terminates the bounded name copy.

| Address | Retail bytes | Candidate compiler | Candidate bytes | Exact |
| --- | ---: | --- | ---: | --- |
| `002ADF60` | 504 | gcc323 | 508 | No |
| `002AE158` | 184 | gcc323 | 184 | No |
| `002AE6C0` | 12 | gcc323 | 12 | Yes |
| `002AE6E8` | 16 | gcc323 | 16 | Yes |
| `002AE6F8` | 24 | gcc29 | 24 | Yes |
| `002AE710` | 12 | gcc323 | 12 | Yes |
| `002AE730` | 120 | gcc323 | 100 | No |
| `002AE7B0` | 128 | gcc323 | 124 | No |
| `002AE990` | 28 | gcc323 | 28 | Yes |
| `002AEA30` | 28 | gcc323 | 28 | Yes |
| `002AEAF0` | 108 | gcc323 | 108 | No |
| `002AEB60` | 200 | gcc323 | 212 | No |
| `002AEC28` | 168 | gcc323 | 176 | No |
| `002AECD0` | 200 | gcc323 | 212 | No |
| `002AED98` | 168 | gcc323 | 176 | No |
| `002AEE40` | 28 | gcc323 | 28 | Yes |
| `002AEE60` | 168 | gcc323 | 176 | No |
| `002AEF08` | 168 | gcc323 | 176 | No |
| `002AEFB0` | 168 | gcc323 | 176 | No |
| `002AF058` | 168 | gcc323 | 176 | No |
| `002AF100` | 32 | gcc323 | 32 | Yes |
| `002AF120` | 32 | gcc323 | 32 | Yes |
| `002AF140` | 168 | gcc323 | 176 | No |
| `002AF1E8` | 28 | gcc323 | 28 | Yes |

Both pinned GNU profiles were compared once for this coherent source batch.
GCC2.9's documented 128-bit save attribute corrects save widths where requested;
it does not repair the remaining slot-order, scheduling or source differences.
Only complete linked bytes and exact symbol sizes establish the ten matches;
alignment padding and reconstructed bytes contribute no matching progress.

Independent review of all24 bodies found a missing scan-predecessor cursor
update in the first draft. That was corrected before publication. An ignored
native32-bit harness then passed1,215 checks with zero failures, covering all
six free-order permutations, coalescing and payload integrity, best-fit and
equal-size tie order, alignment exponents0..8/35, every size-class redirect,
signed stats queries, all allocation/free wrappers, override push/pop, strict
threshold equality and unsigned wrap, and diagnostics that mutate heap state.
