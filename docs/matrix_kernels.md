# Matrix kernels

The complete numeric functions at `0x002A1DF0` (136 bytes) and `0x002A2200`
(120 bytes) are reconstructed as ordinary scalar C. The first transforms a
four-component input using all sixteen matrix words; the second computes a
four-by-four product. Inputs and results are captured before the vector
W/X/Y/Z stores or matrix row stores, preserving full and shifted overlaps in
initialized flat float storage. The declared dimensions describe actual
accessed words, not a discovered arbitrary container capacity.

These are independently compiled memory-effect counterparts. Existing actor
translation units declare `func_002A2200` using `GeorgeRotationMatrix *`,
whereas this module uses flat `float *` pointers. Identical measured pointer
transport and layout do not make those ISO-C function declarations compatible
across a complete source program. Interface unification remains open; existing
actor sources are unchanged. The three-pointer void conventions do not prove
original prototypes, classes, invocation or incidental register return lanes.

Validation covers positive `k/16` values with `0 <= k <= 64` only. Four-term
products and sums are exactly representable integer multiples of `1/256`
within 64. The complete original kernels execute in 2,565 unique initialized
arena cases, covering all 64 instructions and 82,342 executed instructions.
An independent integer oracle checks every result and actual store order.
The separate-TU native harness passes 496,329 checks, including arbitrary W,
matrix orientation, identity and full/shifted aliases. This does not establish
general EE/VU accumulator precision, FCR flags, exceptional values, subnormal
or signed-zero behavior, atomicity, concurrency, timing or hardware identity.

The bounded tracer adds only six approved exact PEXTUW/SQC2 PC/word pairs to
unchanged published decoder methods. Eight synthetic guard methods cover
full-width operands, alias captures, initialized aligned storage, failure
before mutation, instruction budget, register zero, and delay control. All
eight run without the proprietary executable; only original fixture
generation needs a user-provided retail image.

Run from the repository root:

```powershell
python tools/trace_matrix_kernels.py --output build/matrix_kernels_public/trace.json --golden-header build/matrix_kernels_public/golden.h
python -m unittest discover -s tests -p test_trace_matrix_kernels.py -v
python tests/native/run_matrix_kernels.py --output build/native/matrix_kernels_public
```

The native runner requires the existing local MinGW toolchain. Its golden
values are mathematical fixture data, without original instructions or assets.
Original tracing and generated golden bytes remain separate from target
matching. Natural GCC2.9/GCC3.2.3 compiled counterparts are nonmatching;
neither function earns an exact-match award. Complete source, object, section,
relocation and linked-ELF provenance is retained without instruction masking.

The private proof measured whole declaration-only callers and eleven layout
words, retaining four bytes of alignment tail around the 44-byte layout
symbol. COMMON marker storage and caller VMAs were controlled observer
placements, not retail data. That complete ABI evidence is reused because
the public header is byte-identical; promotion does not claim fresh ABI
execution. The unused SAVE128 macro produced identical earlier whole objects
and is not compiled again. Three earlier genuine whole-source controls fail
fixtures 189, 1663 and 190; their complete logical spans/transforms remain
applicable after path-only promotion, without repeated mutation compiles.

All 194 actual executable references are retained, including 193 mechanically
bounded complete caller geometries and the unresolved containing boundary of
the real `.rentext` call at `0x003D1548`. This is not 194 manual caller/root
proofs. Author, parent and distinct private reviews read both complete bodies,
the source/model and bounded supporting contexts. Initial producer failures,
the exact relative-Windows output-path comparison correction, full native PE
clock/checksum observations and the later profile-catalog snapshot
qualification remain separately preserved. Only measured native PE metadata
is qualified for observer replay; target objects, ELF/code, golden values and
original results stay whole-byte checks. No supporting helper or data credit
is awarded, and no PS2SDK incompatible source or GPL implementation is copied.
