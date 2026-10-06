# Ring indexing, removal and peeking

Four generic engine helpers in `src/game/ring.c` reconstruct 480 original
bytes, reusing the ring layout proved by the script-call queue. They compile
and link fully, but remain nonmatching. Function and class names are unproved.

`002AF208` returns an indexed element by advancing the read cursor and wrapping
at the end address. `002AF358` copies the first element without popping it;
`002AF398` copies the last element, retreating from the write cursor and wrapping
to the final buffer slot. Empty rings return null or zero without copying.

`002AF258` removes an indexed element by copying subsequent entries toward it,
including across the wrap boundary. It captures the stopping slot before the
copies, reloads size and cursor bounds during shifting, then retreats the
current write cursor and decrements the current count. It uses the existing
memcpy implementation and retains its aliased-memory behavior. Unsigned
32-bit address arithmetic handles retreat from begin before choosing the
wrapped slot; the C source does not form a pointer before the allocated buffer.
The original has no index-to-count guard, and neither does the reconstruction.

An ignored native 32-bit harness passed 292 checks. It exercised every valid
start slot, count and removal index of a four-slot buffer, checking ordered
remaining values, read/write positions, count, indexed reads, front/back peeks,
and empty operations. This establishes ordinary bounded ring behavior rather
than the complete EE memory model or unusual copies into ring metadata.

An independent instruction review also checked all four helpers. The separate
call-queue harness exercised metadata changes during the shared pop/push
helpers; their accesses agree with the original post-copy reloads.
