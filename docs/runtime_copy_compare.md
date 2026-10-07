Two complete retail string functions are reconstructed using unchanged licensed
Newlib C: `strcmp` at `0x00393A28` (332 bytes, 83 instructions) and `strcpy` at
`0x00393B74` (280 bytes, 70 instructions). These candidates are intended to
replace exactly the two existing reconstructed assembly rows at those addresses.
The old assembly sources, complete comparisons and rows remain preserved. This
migration does not add duplicate function, assembly, helper, data or exact-match
credit. Parent and distinct peer reviews passed, and both existing assembly rows are now replaced by reviewed C rows.

The full generic source files are exact LF Git blobs from the pinned
[public EE toolchain](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string).
The default Cygnus 1994/1997 notice in `LICENSES/newlib-1.8.1.txt`, clause 9,
applies. This software was developed at Cygnus Solutions. The complete notice
and source files remain unchanged. The corresponding old R5900 assembly has
its separate Jeff Johnston/Cygnus 1999 notice. Reusing the generic algorithm
does not establish the game's original source revision or compiler identity.

Both genuine default compiler recipes are retained: `-O2 -G0 -mfp64
-fno-builtin -fno-common -ffunction-sections`, with the established GCC 2.9 or
GCC 3.2.3 command prefix and headers. The complete `strcmp` candidates link at
the real address with no unresolved relocations or allocated data. GCC 2.9 emits
196 bytes (304 differing bytes against the full 332-byte original), and GCC
3.2.3 emits 228 bytes (300 differing bytes). Neither is exact.

Both default `strcpy` objects have a single global `STT_FUNC` beginning at offset
zero in executable `SHT_PROGBITS` section `.text.strcpy`, flags 6 and alignment
8. The original address is four modulo eight. The strict linker rejects both
with `Target address conflicts with function-section alignment`. GCC 2.9 emits
a complete 228-byte symbol, and GCC 3.2.3 emits 176 bytes; each object has no
relocations or undefined helpers. Whole unlinked-object comparisons, exact
section/symbol geometry and the strict failures are retained. These default alignment8 failures remain immutable history. A separate single parent-approved GCC2.9 recipe appends `-malign-functions=2 -malign-loops=2`, as documented by the exact primary R5900 backend. The complete unchanged C naturally emits a224-byte alignment4 symbol and strict original-address link, differing in241 bytes against the complete280-byte original. The full emitted assembly has three genuine align2 directives. This recipe supplies the registered nonmatching `strcpy` candidate. No source, ELF, instruction, linker, crop or padding edits were used; no GCC3.2.3 placement option was tried. Its ordinary `-S` inspection timed out and remains failed history.

The author read all 153 selected original instructions, both complete generic C
files and both complete primary R5900 assembly files. The frozen source-free
survey proves 284 actual executable references and full geometry/hashes for
165 containing scanner intervals. Large-caller semantics are qualified to
bounded ABI windows, rather than all 165 complete caller bodies. The historical
100-byte scanner interval at `0x00375F48` contains a 64-byte getter followed by
an anonymous 36-byte comparator at `0x00375F88`; it receives no whole-interval
comparator or caller award. The selected terminal returns include their real
delay slots, and the four bytes after `strcpy` are excluded padding.

Both target compilers and genuine headers produce the same 17-word ABI probe:
64-bit `long`/`unsigned long`, 32-bit pointer/integer/`size_t`/`ptrdiff_t`, signed
plain char, and measured long limits/alignment. Four complete canonical-only
ABI caller ELFs preserve GPR4/GPR5, call the actual registered target and return
GPR2. The six target compilation closures and six native closures retain all
raw `-M` output, commands and 30 unique dependency fingerprints.

The strict original-code observer inherits the complete published
`RuntimeStringsTrace.execute` decoder unchanged. Its entire local decoder
import closure is pinned. Only complete leaf fetching, original return/delay
controls and fixture memory management are local. No instruction model or
shared decoder changed. The original functions call no helpers and do not use
stack storage. The observer rejects out-of-range or uninitialized loads,
invalid packed operands, calls, invalid return targets and execution past the
budget before mutation. Full 128-bit saved registers, SP and RA are checked.

42,217 synthetic fixtures execute all 153 selected instructions, with all 17
actual conditional branches taken and untaken: 8,432,793 instructions in total,
maximum 2,313 per fixture. Independent ordinary-byte specifications verify the
exact unsigned-byte difference for `strcmp`, and the original destination
pointer plus NUL-terminated copy for `strcpy`. Every byte of three owned padded
512-byte objects is compared, including bytes outside the written interval.

Native tests compile both entire unchanged selected C files separately at
`-O2`, with no `PREFER_SIZE_OVER_SPEED`. The native GNU recipe uses
`-fno-strict-aliasing -fwrapv`. Its measured 32-bit `long` changes block widths
and the access schedule compared with target long64 and original LD/LQ paths.
The unchanged `strcpy` assignment condition produces one authentic
`-Wparentheses` warning, which is retained; compilation is not wholly
warning-free. 64,887,529 byte/return checks pass. A sign-only `strcmp` result
mutation and a wrong advanced-pointer `strcpy` return both fail the real golden
harness. Nine guard methods pass; without the local executable, six pass and
three original-dependent methods skip.

The signed `long` subtraction in the generic `strcpy` zero detector needs a
separate qualification. For 42,033 long32 fixtures and 42,071 long64 fixtures,
every executed detection subtraction is representable. These arithmetic
subsets are recorded per fixture. Native `-fwrapv` execution for the remaining
fixtures is a GNU compiler contract, not universal ISO-C validity. Aligned
long casts also depend on the stated GNU alias/alignment contract.

Reproduce the public local checks from the repository root:

```text
python tools/trace_runtime_copy_compare.py --golden-header tests/native/runtime_copy_compare_golden.h
python tests/native/run_runtime_copy_compare.py
python -m unittest discover -s tests -p test_trace_runtime_copy_compare.py -v
```

The native runner accepts `--output`; every compile/link `-o` must stay inside
that directory before invocation. Fixtures contain only authored synthetic
input/output data. Original execution requires the user's local executable.
Full private replay preserves all immutable input hashes and retains complete
native PE images. Native timestamp/checksum variation is observed only through
parsed PE fields and a read-only Windows checksum check; whole native PE raw
equality is not asserted. Target objects, complete target ELFs, native objects,
source files, fixtures and golden headers remain strict RAW comparisons.

The domain is readable, terminated, fully initialized padded ordinary objects,
disjoint source/destination storage, and representable low-32-bit addresses
without wrapping. Retail wide reads can access up to 15 initialized bytes past
a terminator. No capacity, null, overlap or empty-input safety guards were
invented. Faults, MMIO, volatile/concurrent observation, hardware timing,
original EE stack/register identity and universal language validity are outside
the claim. Initial author observer/proof corrections are preserved separately;
neither imported production source changed.

Parent and distinct peer reproduce22 entire qualified dictionaries,17 complete RAW target artifacts and full emitted assembly. All1001 authored inputs (including961 unchanged defaults) were RAW-equal before this archived annotation;1000 current RAW inputs plus this archived document remain equal. Four complete native PEs permit only actual read-only ImageHlp-validated timestamp/checksum changes. Root exact comparisons enumerate the six default and three supplement generated link-directory pairs, without a general link-name pattern. Whole old catalog/rows/source history remains preserved privately; old assembly sources/notices remain public. No duplicate primary entry or new exact/assembly credit.
