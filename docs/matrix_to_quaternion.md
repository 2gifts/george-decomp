The 480-byte body at `0x002A0B00` has an effects-only counterpart named
`george_matrix_to_quaternion`. It adapts the complete Coin3D
`SbRotation::setValue(const SbMatrix&)` method at commit
`da9c1330c618cff65598b97e881bb896d9ac84ad`, with the complete BSD notice in
the C source and `LICENSES/coin3d-matrix-to-quaternion.txt`.

The adaptation uses the original transposed signs/indexing, literal one in
the radicands, strict diagonal tie comparisons and actual writable numeric
table loads. It omits Coin3D's homogeneous scale. Positive trace computes the
reciprocal before W, then reloads each matrix pair after the preceding output
store. The alternate branch captures its indices, writes the selected
half-root before an optional reciprocal, and reloads operands for each later
store. A zero root skips division. These changes are algorithm adaptation;
they do not establish unchanged source identity or library ancestry.

`D_003FC940` is an undefined writable signed-word table binding. The three
observed initialized words do not prove its full extent or an immutable
runtime invariant. Production defines no table storage or initializer.
Synthetic valid index triples in the tests are observation inputs, not
evidence that the game changes this table.

The source reuses `GeorgeMathVec4`, `GeorgeRotationMatrix`, and the complete
published EE square-root primitive. The convention consumes 16-byte output
and 64-byte input prefixes; it proves no original class, prototype or return
declaration. Existing numeric declarations in other game TUs use incompatible
`const float*` and `const GeorgeRotationMatrix*` types. Complete-source ISO-C
interface integration remains open. The initialized scalar arena, indexed
struct-prefix views and shifted aliases are a bounded GNU observation
contract, not a general portability claim.

The owned observer reads the supplied validated original, routes actual
integer instructions to unchanged `ResourceBaseTrace.execute`, and routes
scalar instructions/loads/stores to unchanged `CameraTrace.execute`.
Exact PC/word, budget, initialized ownership, index, operand-form and normal
finite arithmetic checks precede mutation. Actual branch and JR delay effects
are retained; untaken likely branches annul their delay. The inherited
`ResourceBaseTrace` scalar whitelist is not bypassed by pretending it accepts
those instructions directly.

The synthetic family contains proper cube rotations plus strict ties,
nonorthogonal inputs, writable-table variants and shifted aliases. Complete
arena words and store ledgers are compared. An independent integer/Fraction
ledger proves exact rational operations where applicable, with perfect-square
roots and exactly representable divisions. Signed-zero evidence is separate;
unproved cancellation/product zero signs keep a case in the nominal set.
The `[0,1,2]` table and `[1,-2,-2]` diagonal provide a genuine initialized
synthetic zero-root witness. With the original `[1,2,0]` cycle, that witness
does not establish a reachable canonical-table zero root.

Native observations use the unchanged production TU and an explicitly
renamed initialized table owned by the harness. The existing host square-root
fallback and actual host math library are finite observation support. Neither
nominal agreement nor rational exactness proves general EE low-bit rounding,
FCR flags, traps, subnormal/extended-exponent behavior, timing or hardware
identity. Geometric invariants apply only disjoint proper rotations. Five
real source controls test transposed signs, strict ties, table replacement,
preloaded alias operands and unconditional zero-root division.

The original has 24 actual incoming calls. Bounded caller and full interval
evidence is retained in the private scope and proof packet; no universal main
ancestry or full-caller semantics is inferred. Public records retain geometry,
hashes and proof references, not original instruction arrays or manual pages.
