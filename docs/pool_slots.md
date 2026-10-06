# Fixed-slot pool

Thirteen complete routines recover 1,316 original bytes at `002AD460` through
`002AD9A8`. Three compile and link exactly under the pinned GCC 2.9 profile:
initialization (`002AD460`, 36 bytes), pop (`002AD700`, 72 bytes) and push
(`002AD748`, 80 bytes). The other ten remain reviewed reconstructions.

The observed 28-byte prefix contains signed capacity, unsigned byte stride,
data and snapshot pointers, a halfword free head, signed free count, signed
ownership and lock halfwords. Free slots store their successor index in their
first two bytes. Initialization writes `index + 1` into every slot, including
the final slot; the exhausted count prevents reading that final sentinel as a
valid slot. Default initialization leaves capacity, stride, free count and
unobserved padding untouched.

The source reuses the recovered heap allocation/free entries. A nonnull external
buffer changes the pool to borrowed storage; a null buffer preserves its current
ownership. Allocation products wrap at 32 bits. Initialization reloads fields
after allocation callbacks and each slot store, then resets head and count.
`002AD8E0` repopulates links and count while preserving the head and lock.

Pop tests the count for zero, so a negative nonzero count still enters the
original path. It reads the successor before storing the decremented count and
new head. Push stores the previous head into the slot before reloading stride
and data, computes a wrapped unsigned address difference, divides by unsigned
stride, and truncates the resulting index to a halfword. A nonzero lock blocks
both operations; null push does nothing. Valid slots and nonzero stride retain
the original caller contract, including the retail divide-by-zero break.

Freeze operates only on borrowed storage. With at least two free slots, it
allocates and saves exactly `free_count - 1` successor halfwords. It reloads the
count and head after allocation, and the snapshot pointer after stores. Lock
is set even when no snapshot is needed. Unfreeze restores those links, frees
the reloaded snapshot, clears it and unlocks. The final free-slot sentinel is
not saved or restored. Repeated freeze retains the original lack of a lock
guard; no new idempotence or replacement cleanup is added.

Release frees owned data first, captures the snapshot after that callback,
clears data before releasing the snapshot, then clears the snapshot. The
deleting form finally frees the pool when the captured flags contain bit zero.
This order allows callbacks to change fields in the same places as the original.

The overlap predicate applies only to borrowed storage and uses unsigned wrapped
addresses. It accepts a start in `[base, end)`, a limit in `(base, end]`, or strict
enclosure of the entire pool. Endpoints are deliberately asymmetric: even the
zero-length range `(end, end)` returns true through the limit test. It adds no
normalization for inverted or wrapped ranges.

| Address | Retail bytes | Candidate compiler | Candidate bytes | Exact |
| --- | ---: | --- | ---: | --- |
| `002AD460` | 36 | gcc29 | 36 | Yes |
| `002AD488` | 68 | gcc29 + SAVE128 | 68 | No |
| `002AD4D0` | 68 | gcc29 + SAVE128 | 68 | No |
| `002AD518` | 132 | gcc29 + SAVE128 | 124 | No |
| `002AD5A0` | 140 | gcc29 + SAVE128 | 132 | No |
| `002AD630` | 88 | gcc29 + SAVE128 | 92 | No |
| `002AD688` | 116 | gcc323 | 124 | No |
| `002AD700` | 72 | gcc29 | 72 | Yes |
| `002AD748` | 80 | gcc29 | 80 | Yes |
| `002AD798` | 172 | gcc29 + SAVE128 | 180 | No |
| `002AD848` | 148 | gcc29 + SAVE128 | 156 | No |
| `002AD8E0` | 76 | gcc323 | 60 | No |
| `002AD930` | 120 | gcc29 | 104 | No |

Both pinned compiler profiles and the documented optional GCC 2.9 save-width
attribute were checked once. Natural source macros reuse the repeated inline
initialization and release sequences. No compiler backend, instruction bytes,
alignment padding or comparison masks are changed to establish matches.

Root reviewed every original instruction; a second agent independently reviewed
every source body, including aliases, callback reloads and wrapped arithmetic.
The checked-in native 32-bit harness passes 1,442 checks covering initialization,
all one-to-eight-slot allocation depths and push order, ownership, overwritten
borrowed storage, snapshot restoration, callback changes, negative counts,
halfword truncation, unsigned wrap and overlap boundaries. See
[native harness instructions](../tests/native/README.md).
