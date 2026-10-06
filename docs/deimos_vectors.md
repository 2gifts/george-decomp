# Deimos vector and word-stream conversions

Four routines in `src/game/deimos_vectors.c` reconstruct 712 original bytes.
They reuse the existing tagged values, pool/table operations and three-float
vector type. All four compile and link against explicit retail addresses;
their complete bytes still differ from the original. An independent review
checked each path, memory width and call boundary.

`002D00A0` looks up the confirmed CRC keys for x, y and z, then a fourth key
`0x00814509`. All lookups precede all payload loads. The three vector scalars
are captured before stores in x, z, y order. The optional fourth scalar is
read after these stores, preserving possible output/table aliasing. A missing
fourth key gives positive zero; a present one is multiplied by the exact
single-precision constant `0x3C8EFA36`. The retained callers support an angle
conversion role, but the fourth key's original spelling remains unproved.

`002D0178` creates a table without a secondary table and stores x, y, z, then
the supplied fourth scalar. Each vector component is read after the preceding
setter returns. The fourth scalar is stored directly, without the getter's
conversion. Each setter receives the same stack value, initialized to tag 2
and subtype zero again before the call.

`002CFFC0` creates a table from a zero-key-terminated word stream. After each
key it reads a signed type byte from a full word. Type 0 consumes one payload
word and assigns tag 1; type 1 consumes one float word and assigns tag 2.
Unsupported types consume the type word but leave the payload word for the
next key. Adding a default payload skip would change the original behavior.

`002CFE18` releases a nonnull shared node at `D_003FDBA0`, then clears that
global. The source preserves this order if the release changes global state.
The reconstruction assumes valid input streams and required x/y/z entries,
matching the original's lack of bounds and null guards. Execution on the EE
has not been tested.
