# Actor control requests and attachment helpers

This isolated batch reconstructs 19 complete numerical entries spanning 4,204
original text bytes. It reuses the observed actor, member, virtual-pair,
matrix and effect-registry representations from earlier reviewed batches.
No original class names are asserted. The reviewed packet is registered;
canonical exact-byte awards remain the responsibility of the full verifier.

The 157 encoded JAL references identify sixteen entries. Three constructor
bodies, `00190EB0`, `001911F0` and `001912C8`, have no proven incoming caller or
stored function pointer. A full text J/JAL scan, a LUI plus ADDIU/ORI search
and aligned pointer scan of non-executable sections found no references.
Their complete instruction extents, independent prologues/returns and coherent
constructor/release call families support their body boundaries and observed
signatures. Their runtime reachability remains unresolved; registration review
must retain this limit. None is treated as an exact match.

| Family | Complete entries | Observed behavior |
| --- | --- | --- |
| Reference setup | `185EA0` | Two control resets, repeated angle queries, basis and position writes |
| Movement | `18B710`, `18D8A0` | State-gated vectors and motion/effect control |
| Flags | `18FD30`, `18FD40` | Full 64-bit OR and AND-complement |
| State/request | `190E00`, `192C58` | Signed member dispatch and paired live-time request |
| Dual/single object | `190EB0`, `190F70`, `191DC8`, `191E50` | Construction and callback-sensitive release |
| Stored object | `1911F0`, `1912C8`, `1913A0`, `1913E0` | Saved-key construction and release |
| Attachment | `191D08`, `191670` | Matrix transform, publication and notification |
| Matrix lookup | `192748`, `192818` | Primary/companion record and matrix search |

`00185EA0` clears flag `0x4000` twice through the actual AND-complement helper.
Each following control call can change the matrix at actor+`0x484`. The angle
algorithm makes two or three separate atan calls, reloading that matrix each
time. Strict positive and negative pi comparisons preserve unordered behavior.
The optional input uses interleaved component loads/stores. Three subsequent
basis operations overwrite the same output; each captures its coefficient,
basis and translation before storing X, Z and Y. The next operation reloads
its matrix and coefficient. This order affects a matrix whose translation
aliases the output, which the native harness tests. The final control/angle
sequence replaces the output with an interleaved translation copy, then updates
full 64-bit flag bits 19, 41 and 40.

`0018B710` has an actual 41-word switch table at `0x0042D8F0`. Only states
0, 2, 4, 29, 30 and 40 enter the vector setup; every other unsigned state
returns zero without writes. The accepted path returns one and stores bit49
in its return delay slot. Both actual returns prove the `s32` result ABI.
Optional input copies retain their component load/store ordering; absent
direction negates the actor's existing direction, and absent position clears
X, Z and Y. The harness checks every table selector, out-of-range states,
signed zero and shifted aliases.

Two earlier callers, `00174770` in `actor_movement.c` and `00180580` in
`actor_states8.c`, ignored the result through a `void` declaration. After the
parent's checkpoint, their declarations were aligned to the proven `s32`
result. The two existing native stubs now return defined zero with all prior
effects preserved. Six fresh before/after full linked comparisons across the
three supported recipes have identical complete hashes, sizes and differences;
existing actor-movement752 and states8 1,341 native checks still pass. This was
a separate parent-approved narrow patch, recorded under
`build/prototype18B710`. The isolated controls header declares the same ABI.

`0018D8A0` captures all three components of object+`0x74` before any soft-double
call. It calculates absolute Z, Y and X in that order, summing in binary64
before converting back to binary32. Effect construction preserves the registry
and global capture before hashing, publishes the handle before configuration,
and reloads it before starting. Cleanup reloads the handle after stopping.
The motion path reads the input after these calls, uses the existing EE SQRT
value primitive, compares its length strictly, and retains
`config260 * (length / config10)` operand order. The alternative path preserves
the exact `0x3F733333` decay factor and original optional cleanup.

`00192C58` differs from the previously reused zero-time request templates:
both companion and main receive the incoming float argument. The companion
sees the live first-call f12 value; main restores the captured value and
reloads its pointer and owner word after the companion callback. Callback and
context remain the original inputs, including a nonnull context paired with
a null callback. Main's result is returned; absent main returns zero after
the request-field write.

The matrix finders capture command3's remapping to2 and its wrapped offset
before callbacks. A canonical unsigned getter supplies the word interpreted
as a signed count by these callers. Record storage reloads after the count
call, and matrix storage reloads after the record call. A full 32-bit identifier
compares against an unsigned byte, with record stride32 and matrix stride64.
The primary and companion variants share their complete source template but
retain their distinct gate and storage offsets.

Release paths cache the saved word before invoking the original release
callback, then clear the field afterward even if the callback replaces it.
Creation retains its captured return across matrix copying and modifies that
same object's flags, while notification reloads its stored pointer afterward.
`00191DC8` has no original null-reference guard; callers must satisfy the
existing callee contract. The two stored-object release bodies are exactly
60 bytes, excluding four bytes of trailing alignment each. Whole repeated
blocks use source macros without invented private helper addresses.

Run the asset-free native checks with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_controls.py
```

The current harness passes 1,335 checks across all 19 bodies. It includes
callback-mutated references, matrices, owners, objects and handles, complete
64-bit masks, signed member adjustments whose sum exceeds 16 bits, signed
counts, full identifiers, saved keys, sparse state gates and shifted aliases.
Twenty-four exact finite 3-4-5 vectors test equality versus strict length
thresholds. Host arithmetic and mocked soft-double calls omit EE FCR effects
and do not certify every nonfinite arithmetic instruction encoding.

The first native compile caught an author macro-shadowing bug in the angle
output: an internal `angle` hid the caller's variable. Its internal probe was
renamed before this frozen packet, and all 57 genuine full compiler links
were regenerated. All now have zero unresolved references. The two actually
called flag helpers reproduce 36 complete bytes under each of the three
genuine profiles. Larger routines remain reconstructed. Research independently reviewed all 19 complete originals (1,051 instructions)
against C/header, enumerated nonstack store inventories and reproduced all
57 links and 1,335 native checks. Its complete metadata audit passed all
hashes/boundaries, 78 branches, 157 JALs with whole caller bytes, six binary32
literals, the 164-byte bool table/all returns and numerical bindings. The
three reachability limits were independently reproduced. Parent independently
read all 19 complete original bodies against C/header, reviewed the native
harness and documentation, reproduced all 57 actual full links and audited the
78 branches, 157 complete caller proofs, six constants, complete switch table
and return ABIs. Its fresh native run passed all 1,335 checks. Registration
preserves all three unresolved reachability cases and keeps every function
reconstructed pending canonical verification. The registered packet is
`config/functions/actor_controls.json`, with numerical bindings in
`config/symbols/actor_controls.json`; no canonical verifier was run by this
batch author.
