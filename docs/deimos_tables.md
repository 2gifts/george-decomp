# Deimos table and global dictionary mutation

Eight routines are reconstructed as ordinary typed C in `deimos_tables.c`,
covering 1,124 original bytes. The 40-byte generic-to-Deimos visitor adapter
matches completely after original-address linking with the pinned GCC 3.2.3
profile. The other seven routines are reviewed reconstructions and do not yet
match. No upstream game implementation was identified or copied.

| Original address | Observed role | Original bytes | Candidate bytes | Exact match |
| --- | --- | ---: | ---: | --- |
| `002CCB10` | Mutate fixed-bucket Deimos table value | 324 | 304 | No |
| `002CDCA0` | Mutate global dictionary value | 192 | 180 | No |
| `002CDD60` | Look up global dictionary key | 40 | 36 | No |
| `002CDD88` | Adapt generic map callback to Deimos visitor | 40 | 40 | Yes |
| `002CDDB0` | Visit global dictionary with callback/context | 52 | 60 | No |
| `002A7C08` | Generic map lookup | 80 | 88 | No |
| `002A7CD0` | Generic map replace or prepend | 236 | 220 | No |
| `002A8130` | Generic map callback traversal | 160 | 160 | No |

The two representations are distinct. A Deimos table is the existing 16-byte
pool node interpreted through its observed hash-table fields: secondary table
at offset 8 and 17-bucket array pointer at offset 12. Its 16-byte chain entries
contain next, key and an inline eight-byte tagged value. The generic map has
flags, bucket count, entry count and bucket pointer at offsets 0, 4, 8 and 12;
its 12-byte entries contain next, key and a pointer payload. The global dictionary
at `D_00481760` uses the generic map to hold separately allocated Deimos values.
The header asserts all modeled sizes and field offsets for the 32-bit target.

`002CCB10` uses the previously recovered secondary-first lookup. An existing
secondary-table value is updated there; a missing value is inserted in the
local table. Signed value tags 3 through 5 cause release of the previous pool
payload before the input is copied, followed by acquisition of the new payload.
The input is read after release, including when it aliases a record affected
by destruction. New nodes receive key, old chain head and value before the
bucket base is reloaded and the node is inserted. The reference decision then
reads the input again, preserving possible alias effects from insertion.
Allocation failure calls the existing error routine and returns.

`002CDCA0` applies the same release/copy/acquire order to global records. For a
missing key it allocates eight bytes, reloads the global dictionary after the
allocator call, inserts the allocated pointer, and then copies the value. The
retail routine assumes that value allocation succeeds and does not check for
failure of the generic map's separate node allocation. The reconstruction
preserves that behavior.

Generic map insertion replaces the first matching value when flags bit zero
is clear. When that bit is set it allows duplicates and prepends a new node.
The bucket index is computed before allocation; bucket base and entry count
are loaded again after allocation and node stores. A failed node allocation
leaves the map untouched. Modulo operations retain the original assumption
that bucket count is nonzero.

Traversal snapshots the value, key and next pointer before invoking a callback.
Saving next lets a callback remove the current node without requiring a later
read from it. Bucket count and bucket base are reloaded after each chain, so
callback mutations affect later buckets. The iterator never consumes callback
return registers. `002CDD88` forwards only key, value and the context stored in
its eight-byte adapter record; declaring these callbacks `void` preserves the
observed contract. The adapter's complete compiled bytes also match retail.

The upstream m2c workflow produced drafts for all eight routines. It flagged
the paired `ldl`/`ldr` and `sdl`/`sdr` value copies as unsupported; those errors
were manually reconstructed as full eight-byte value assignments. No error
macros or assembly fallbacks remain in the recovered source. Differences in
copy instruction selection, saved-register width, scheduling and branch
selection remain visible in the exact byte comparison. Those bytes are never
masked or accepted as matching.

An ignored native 32-bit harness ran 33 semantic checks with zero failures.
It exercised secondary-first replacement, release callbacks mutating the input,
tag-range reference handling, allocator-induced bucket/global changes, a new
node aliasing its table and replacing its bucket pointer, allocation failures,
generic replacement/duplicate insertion, and callbacks changing the current
node's next pointer plus the map's bucket base/count. It also verified global
visitor forwarding. All eight target functions were compiled and individually
linked with no unresolved relocations. The manifest records original and
candidate hashes and complete size/byte comparison results; only the adapter
is marked matched.
