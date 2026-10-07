# Verified runtime reuse

Four runtime functions were located by comparing symbol-sized, relocation-free
object code from the official [PS2DEV Windows homebrew toolchain release of
2018-10-19](https://github.com/ps2dev/ps2toolchain/releases/tag/2018-10-19) with the
USA retail executable `SLUS_216.68`. Each byte sequence occurs exactly once in
the executable's `.text` section. This scan tested 123 relocation-free functions
between 24 and 4,096 bytes from `libc.a`, `libm.a`, and `libgcc.a`.

The extended scanner in `tools/runtime_scan.py` also masks only actual ELF
relocation bits and derives external symbol bindings from the linked retail
instructions. It found `atoi` with its `strtol` call relocated. The call target
was corroborated by the number parser's character classification, sign, radix,
and digit conversion behavior. Short generic wrappers can match uniquely by
chance: a candidate named `_mallinfo_r` was rejected because its target has
unrelated semantics. Scanner candidates alone do not establish function names.

These ten earlier verified functions were independently compiled or assembled,
and their complete function bytes compared with the retail executable again.
The GNU floating-point runtime batch below adds twenty-four further verified C
functions, the GNU integer conversion batch adds five, and the later cosine
wrapper adds one, all from unchanged upstream source.

| Symbol | Retail address | Bytes | Reused source | Source form | Verification |
| --- | --- | ---: | --- | --- | --- |
| `memcmp` | `0x00393464` | 148 | `src/runtime/memcmp.S` | Upstream assembly | Exact |
| `memcpy` | `0x003934f8` | 172 | `src/runtime/memcpy.S` | Upstream assembly | Exact |
| `memmove` | `0x003935a4` | 252 | `src/runtime/memmove.S` | Upstream assembly | Exact |
| `fabsf` | `0x0037dd58` | 28 | `src/runtime/fabsf.c` | Upstream C | Exact |
| `atoi` | `0x00396320` | 40 | `src/runtime/atoi.c` | Upstream C | Exact after linking `strtol` |
| `matherr` | `0x0037db30` | 36 | `src/runtime/matherr.c` | Upstream C | Exact under GCC 2.9 after linking `dpcmp` |
| `__errno` | `0x00393368` | 12 | `src/runtime/errno.c` | Upstream C | Exact after linking `_impure_ptr` |
| `_localeconv_r` | `0x0039cc68` | 12 | `src/runtime/locale.c` | Upstream C | Exact with all readonly self-pointer relocations linked |
| `sinf` | `0x0037b0c0` | 240 | `src/runtime/sinf.c` | Upstream C | Exact under GCC 2.9 after linking reviewed kernels and reducer |
| `tanf` | `0x0037b1b0` | 136 | `src/runtime/tanf.c` | Upstream C | Exact under GCC 2.9 after linking reviewed kernel and reducer |

The 572 bytes of reused assembly remain separate from high-level C
decompilation progress. The 504 bytes of compiled `fabsf`, `atoi`, `matherr`,
`__errno`, `_localeconv_r`, `sinf`, and `tanf`, plus the 3,264 bytes of the GNU
floating-point runtime batch and 820 bytes of the GNU integer conversion batch
below, plus 232 bytes from the cosine wrapper, total 4,820 bytes from 37 verified
C functions.
Function addresses, file offsets, byte hashes, provenance, flags, and source
classification are recorded in `config/runtime_functions.json`.

One further upstream C routine, `fmodf` at `0x0037b238` (316 bytes), is
reconstructed with strong identity evidence but has not matched. Its exception
name is the literal `fmodf` at `0x004550e0`, and the entire wrapper's flow agrees
with newlib: core remainder call, NaN checks, library-version handling, the
zero-divisor domain exception, `matherr`, errno 33, and float/double conversion
helpers. `src/runtime/fmodf.c` preserves the unchanged upstream implementation.
Its candidate builds differ in size and instruction scheduling. The entire
16-byte compiler `.rodata` section matches the original exception name and
double NaN at `0x004550e0`; the verifier independently maps and checks those
bytes when linking. Every external reference has an evidenced retail binding.
This routine contributes no matched bytes.

Two further reconstructed sources are `finitef` at `0x0037dd78` and `isnanf` at
`0x0037de88`, each 36 retail bytes. Their full exponent/sign-bit algorithms agree
with the older newlib implementations, and the latter is called by `fmodf`.
The GCC 2.9 candidate reproduces their instruction choices but its bundled
assembler inserts one extra NOP after `mfc1`. Their sources remain reconstructed
and contribute no matched bytes; no instructions are removed to award a match.

The core `__ieee754_fmodf` at `0x0037b9c8` (592 bytes) and `floorf` at
`0x0037dda0` (228 bytes) also have unchanged upstream C reconstructions. The
core remainder routine's shift/subtract normalization, subnormal exponent
loops, exceptional `(x*y)/(x*y)` return, and signed-zero result agree with the
entire original body. Its 16-byte `one`/`Zero[]` pool independently matches
retail data at `0x00455120`; the original indexes the signed-zero table at
`0x00455128`. The floor routine's exponent/mantissa masks, negative fractional
adjustment, `1e30` inexact check, and infinity/NaN branch establish its identity.
Both complete source builds differ from the retail functions and remain
reconstructed, contributing no matched bytes.

## Source provenance and licenses

`memcmp.S`, `memcpy.S`, and `memmove.S` are Jeff Johnston's R5900 implementations,
dated February 10, 1999, copyright Cygnus Solutions. Their source notices grant
permission to use, copy, modify, and distribute while preserving the notice.
They were copied without code changes from the added-file hunks of the
[PS2DEV newlib 1.10.0 patch at release
2018-10-19](https://github.com/ps2dev/ps2toolchain/blob/2018-10-19/patches/newlib-1.10.0-PS2.patch).
The exact Git blob is `f1971c9a39b37c9d28f80444638aee383d4e48ff`.

`fabsf.c` is an unchanged copy of `newlib/libm/math/sf_fabs.c`, and its companion
`fdlibm.h` is an unchanged copy of `newlib/libm/common/fdlibm.h`, from the official
[newlib 1.10.0 source archive](https://sourceware.org/pub/newlib/newlib-1.10.0.tar.gz).
The source archive SHA-256 is
`69b62ad4c746a9acaf4f898772549f6da49f228f83a95efce7e88ae1d88c5a84`.
These files preserve Sun Microsystems' 1993 permissive copyright notice and the
float-conversion credit to Ian Lance Taylor. The release's complete collection
of license notices is retained in `LICENSES/newlib-1.10.0.txt`. These sources keep
their upstream terms; the project's own tooling license does not replace them.

`atoi.c` is an unchanged copy of `newlib/libc/stdlib/atoi.c` from the same
newlib archive. It retains Andy Wilson's authorship credit. It is covered by
the newlib distribution notices, preserved in `LICENSES/newlib-1.10.0.txt`.
Its exact source build links the symbol `strtol` at `0x0039ca20`; the required
binding and supporting symbol evidence are in `config/symbols/runtime.json`.

`fmodf.c` is an unchanged copy of `newlib/libm/math/wf_fmod.c` from the same
archive, preserving its Sun Microsystems notice and Ian Lance Taylor credit.

`matherr.c` is unchanged `newlib/libm/common/s_matherr.c` from the same official
archive. The file is also byte-identical to the older public GNU EE source.
It links its compiler-generated soft-double comparison helper `dpcmp` to the
retail call at `0x00373250`; the complete 36-byte function matches with the
pinned `gcc29` profile. The other runtime C matches retain their existing
`gcc323` recipes.

`errno.c` is unchanged `newlib/libc/errno/errno.c` from official newlib 1.10.0,
covered by the preserved distribution notices. Its `__errno` function returns
the address of the reentrancy structure's errno field. Retail `fmodf` calls this
helper before storing domain errno 33, independently confirming the role.
The complete 12-byte function matches the `gcc323` source build when
`_impure_ptr` is linked to its known retail global address `0x00405694`.

## Integer conversion, character classification, and locale

Ten further functions are reconstructed from unchanged files in the pinned
public newlib 1.8.1 tree. Their complete original control flow and return
boundaries were reviewed. `_localeconv_r` now matches exactly; the other nine
retain reconstructed status.

| Retail address | Functions | Upstream file |
| --- | --- | --- |
| `0x0039C7F0`, `0x0039CA20` | `_strtol_r`, `strtol` | `libc/stdlib/strtol.c` |
| `0x0039CA58`, `0x0039CA78` | `tolower`, `toupper` | `libc/ctype/tolower.c`, `toupper.c` |
| `0x0039CA98`, `0x0039CC68`, `0x0039CC78`, `0x0039CCA8` | `_setlocale_r`, `_localeconv_r`, `setlocale`, `localeconv` | `libc/locale/locale.c` |
| `0x0039CCD0` | `floor` | `libm/math/s_floor.c` |
| `0x0039CEC0` | `__ieee754_fmod` | `libm/math/e_fmod.c` |

The parser skips whitespace, handles signs and inferred radix, converts digits,
checks cutoff/remainder overflow, sets errno 34 and selects the end pointer.
Retail cutoff arithmetic uses 64-bit `long` while pointers remain 32 bits.
Its explicit `-mlong64` recipe uses that supported combination in the unmodified
EE backend; a compilation assertion checks both widths and errno's offset.
The argument-forwarding wrapper is also the call target established by the
already matching `atoi` implementation. Central bindings retain the reviewed
low-64 multiplication, unsigned division and unsigned remainder targets.

`ctype_table.c` preserves the entire upstream character table. All 257 bytes
match retail data at `0x00456118`, SHA-256
`8b55a0d9c781d7001042c99e21523fef362440f572faef950e8e600fae5de813`.
The parser and two case-conversion leaves access that table at `c+1`; the latter
use its upper/lower bits and conditional ASCII offset. Data equality strengthens
their identities and does not count as recovered function bytes.

The locale implementation includes the `MB_CAPABLE` branch. Its redundant `C`
comparison, accepted `C-JIS`/`C-EUCJP`/`C-SJIS` strings, previous/current locale
copies, 8/2/1 multibyte limits and reentrancy field writes all agree with retail.
The complete initialized 24-byte writable block matches at `0x00405E90`.
The 96-byte readonly locale table/string block agrees at `0x00456C30` after
independently resolving its ten actual pointer relocations. The unchanged
`_localeconv_r` source links those ten self-section `R_MIPS_32` entries using
GNU ld and matches all 12 code bytes and all 96 readonly data bytes. The two
public wrappers also link fully, while their code differs. `_setlocale_r` retains
visible unresolved object relocations because verification does not map writable
local data; it remains reconstructed.

Double `floor` independently maps its entire 16-byte pair of `1.0e300` constants
at `0x00456C90`; its masks, carry, negative rounding and infinity/NaN paths agree
with the source. Double remainder maps the entire 24-byte `one`/signed `Zero`
pool at `0x00456CA0`. Its exceptional-value, normal/subnormal exponent,
shift/subtract and signed-zero paths agree through the final return. These
sources link to the original soft-double arithmetic helpers without unresolved
references, while their generated instruction bytes still differ.

The parser and locale recipes explicitly include unchanged GNU
`ee/gcc/glimits.h` as `src/runtime/include/limits.h`, with provenance and the
GNU license retained in `LICENSES/GPL-2.0.txt`. This supplies an authentic header
omitted by the first optional local compiler package without changing that
package or the backend. All source file hashes and candidate recipes are in
the runtime manifest.

The Berkeley notices in `strtol.c` and `ctype_table.c` are preserved verbatim:
This product includes software developed by the University of California,
Berkeley and its contributors.

Run `python tools/check_runtime_data.py` to reproduce the three complete ctype
and locale data identity checks. It validates the original executable hash,
source hashes, non-executable allocated data geometry, and each actual pointer
relocation's linked symbol table. It writes its local report under ignored
`build/reuse/runtime_data` and awards no function progress.

`finitef.c` and `isnanf.c` are unchanged Sun Microsystems implementations from
the newlib 1.8.1 tree in the [public GNU EE source at commit
`b595ded606227e93b8c4a447446c1d2ac093827d`](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib).
Their upstream paths are `ee/newlib/libm/common/sf_finite.c` and
`ee/newlib/libm/math/sf_isnan.c`. Their notices and source hashes are preserved,
and `LICENSES/newlib-1.8.1.txt` retains the release's license collection. The
existing permissively licensed `fdlibm.h` provides compatible word-access macros.

`ieee754_fmodf.c` and `floorf.c` are unchanged copies of
`ee/newlib/libm/math/ef_fmod.c` and `ee/newlib/libm/math/sf_floor.c` from that same
pinned public newlib 1.8.1 tree. They preserve Sun Microsystems' notices and Ian
Lance Taylor's conversion credit. Their source hashes, reviewed boundaries,
candidate profiles, and independent constant-pool mapping are retained in the
runtime manifest.

## Reproducing the tool setup

On Windows, run these commands from the repository root using a working Python
3 interpreter:

```powershell
python tools/bootstrap_toolchain.py
python tools/bootstrap_toolchain.py --verify-only
```

The bootstrap script downloads the two official, openly published tool archives
into the ignored `tools/vendor` directory, verifies their pinned SHA-256 hashes,
and extracts only the needed directories. The Windows built-in `tar.exe` can
read the official PS2DEV `.7z` archive. No machine-wide installation is needed.
The complete download URLs and individual tool hashes are recorded in
`tools/toolchain_manifest.json`.

The binutils v0.10 archive hash was checked against the SHA-256 digest supplied
by GitHub's official release-asset API. The older PS2DEV release has no published
API digest; its official release download was hashed locally and that hash was
pinned for reproducibility. This difference is retained in the manifest.

The candidate compiler is
`tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/bin/ee-gcc.exe`.
Its DLL dependencies require
`tools/vendor/ps2dev-20181019/MinGW/bin` on the process `PATH`. The compiler's own
`bin` directory should also be on `PATH`. Compile the runtime sources with
`-O2 -G0 -fno-builtin -fno-common -c`; the driver uses the bundled GNU `ee-as`
2.14. The imported assembly relies on that assembler's pseudo-instruction
encodings, so do not silently substitute the modern assembler for these files.
Modern binutils v0.10 is useful for ELF inspection and later baseline assembly.

## Scope of the evidence

These exact runtime matches establish reusable implementations of the specific
functions documented here. They do not identify the compiler used for Papaya's game code, prove
that all newlib/SDK code is the same version, or establish a shared game engine
with any other decompilation. The original compiler remains unidentified.

The ELF contains Havok client identifiers and explicit references to Deimos
tables and `DScriptMgr`. No literal Lua identifier was found in its printable
strings. This makes the game's scripting implementation a research target;
Lua must not be imported solely because its syntax looks similar. No Havok
implementation or proprietary SDK source is copied into this project.

Other useful primary tooling references are [Splat](https://github.com/ethteck/splat)
(MIT; PS2 splitting support), [Ghidra Emotion Engine:
Reloaded](https://github.com/chaoticgd/ghidra-emotionengine-reloaded) (Apache 2.0;
R5900 analysis), and [CCC](https://github.com/chaoticgd/ccc) (MIT; symbol recovery
when debug/linker symbols exist). This executable is stripped, so CCC's debug
type recovery does not presently apply. Their licenses permit appropriate
reuse, but none of their game-function identities are being assumed here.

## Explicit save precision in the older compiler

The unchanged public GCC 2.9 backend supports
`__attribute__((register_precision(s0,128)))`, with a bare register identifier.
`mips.c` applies the requested mode to its save/restore table; its ordinary
default remains 64 bits. No global command-line switch forcing 128-bit saves
was found in its target option tables.

`include/george/compiler.h` defines `GEORGE_SAVE128` only when the recipe opts in
with `-DGEORGE_USE_SAVE128`; unsupported compiler versions or targets produce an
error. It specifies `s0` through `s7` and `fp`, leaving `ra` at 64 bits. The final
repeated `s0` repairs a historical attribute-list behavior: generic `tree.c`
overwrites the first entry while the backend appends subsequent registers.
The repeated final entry preserves the head register too, using the existing
backend rather than modifying its target implementation.

A probe with nine live saved registers confirms `sq`/`lq` for every requested
register and `sd`/`ld` for `ra`. Full linked comparisons of `func_001D9920` and
`func_002CD990` confirm that corrected widths alone do not produce matches:
stack-slot order, scheduling, or remaining source reconstruction still differ.
This macro provides an explicit compiler capability, not automatic progress.

## AROS list behavior adaptation

`src/game/list_aros.c` adapts the public AROS `AddTail`, `AddHead`, `RemHead`,
and `Remove` algorithms from commit
[`e8e543e6ca866e26671c8f586d545f80609ef3dd`](https://github.com/aros-development-team/AROS/tree/e8e543e6ca866e26671c8f586d545f80609ef3dd/rom/exec).
The source preserves the AROS notices, records the dated changes, and remains
under [AROS Public License 1.1](../LICENSES/AROS-Public-License-1.1.txt).

These are behavioral adaptations with George's structure layout, original store
and reload order, explicit empty sentinel check, and cleared removed-node links.
The empty check agrees for valid lists but can differ on corrupt inputs. This
reuse establishes no original AROS source identity or byte match. The APL applies
to this dedicated source file.

## Additional identified upstream string assembly

`strcmp.S` and `strcpy.S` are unchanged Jeff Johnston/Cygnus Solutions 1999
implementations from the pinned public newlib 1.8.1 tree. They are also
byte-identical source files in the previously pinned PS2DEV newlib 1.10 patch.
Their preserve-notice licenses remain in each file.

Retail `strcmp` at `0x00393A28` is 332 bytes; `strcpy` at `0x00393B74` is
280 bytes. Both match every upstream algorithm instruction outside two `DLI`
macro expansions and the resulting initial branch displacement. The two 64-bit
constants agree exactly. The pinned assembler expands those constants with
shorter `LUI` sequences and produces 324 and 272 bytes respectively. They remain
reconstructed assembly, earn no exact-match credit, and never count as
high-level C recovery. These macro differences describe assembler behavior and
do not identify the original compiler.

## Additional fdlibm source identities

Six more complete routines reuse unchanged public newlib 1.8.1 sources at
[`b595ded606227e93b8c4a447446c1d2ac093827d`](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm).
Each file retains Sun Microsystems' 1993 permission notice. The manifest pins
the exact source URL and SHA-256 as well as the complete original body hash.

| Source function | Original address | Complete body bytes | Selected compiler | Current result |
| --- | --- | ---: | --- | --- |
| `__kernel_sinf` | `0x0037D790` | 260 | GCC 3.2.3 | Fully linked, differs |
| `__kernel_tanf` | `0x0037D898` | 660 | GCC 3.2.3 | Fully linked, differs |
| `rint` | `0x0037DB58` | 508 | GCC 2.9 with `-mdebuga` | Same size, 30 differing bytes |
| `scalbnf` | `0x0037DEB0` | 352 | GCC 2.9 | Fully linked, differs |
| `copysignf` | `0x0037E010` | 48 | GCC 2.9 | Fully linked, differs |
| `__ieee754_logf` | `0x0037BC18` | 804 | GCC 3.2.3 | Fully linked, differs |

The complete control flow and arithmetic paths were reviewed against original
instructions: sine's tail correction; tangent's reduction, split polynomial and
accurate reciprocal; double rounding's exponent/mask/sign cases; float scaling's
normal, subnormal and extreme exponent cases; bitwise sign copying; and
logarithm's normalization, small-input and compensated polynomial branches.
This adds 2,632 original bytes represented by licensed high-level source, with
zero new exact matches. It does not establish the source version of the entire
runtime or guarantee that a host's IEEE floating-point behavior reproduces all
Emotion Engine exception and rounding behavior.

Five complete constant objects or sections provide separate identity evidence:
sine's 28-byte pool at `0x0045561C`, tangent's 52-byte `T` object at
`0x00455648`, rounding's 16-byte `TWO52` pool at `0x00455680`, scaling's
16-byte pool at `0x00455694`, and logarithm's 48-byte pool at `0x00455130`.
`python tools/check_runtime_data.py` now checks sixteen complete runtime data
identities in total; data checks award no function bytes.

The rounding and logarithm candidates map their complete readonly sections and
use the normal linker to resolve references. Tangent's older-compiler 72-byte
section has four final padding bytes that disagree with the adjacent original
`__fdlib_version` value. That mapping is rejected; its selected GCC 3.2.3 recipe
embeds the constants and fully links without changing data or instruction
bytes. The isolated 52-byte `T` data identity is not used as a partial linker
mapping. Rounding's supported `-mdebuga` option suppresses folded
label-plus-register addresses in the unchanged backend; the remaining 30 bytes
differ in register selection and constant-address construction and remain
explicitly reconstructed.

## Exact sine and tangent wrappers

`sinf.c` and `tanf.c` are unchanged copies of the pinned newlib 1.8.1
[`sf_sin.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/sf_sin.c)
and
[`sf_tan.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/sf_tan.c).
Both retain Sun Microsystems' 1993 permission notice and Ian Lance Taylor's
float-conversion credit. Their exact source hashes are recorded in the manifest.

The unchanged GCC 2.9 baseline recipe links the complete sine and tangent
wrappers to their independently reviewed kernels and pi/2 reducer. All 240
and 136 original bytes respectively match, including branch offsets and call
targets, with no unresolved relocations or masked bytes. The magnitude
thresholds differ by two low bits (`0x3F490FD8` and `0x3F490FDA`); sine then
selects four quadrants, while tangent uses `1-((n&1)<<1)` for the signed
reciprocal result. These are complete C matches, separate from the remaining
unmatched kernel bodies. The reducer's entire 960-byte constants section
matches unchanged upstream data, corroborating the call binding; it does not
award code progress before source registration and comparison.

## Cosine, square root, logarithm and argument reduction

Five further unchanged sources from the same pinned public newlib 1.8.1 tree
cover 4,356 original code bytes. They retain the Sun Microsystems 1993 notice
and Ian Lance Taylor's conversion credit; exact file hashes and URLs are in
the manifest. All selected recipes compile and fully link, but none match the
complete original code bytes, so every entry remains reconstructed.

| Source function | Original address | Complete body bytes | Selected compiler recipe |
| --- | --- | ---: | --- |
| `__kernel_cosf` | `0x0037CCE8` | 344 | GCC 2.9 baseline |
| `__ieee754_sqrtf` | `0x0037CBB0` | 312 | GCC 2.9 baseline |
| `logf` | `0x0037B378` | 328 | GCC 2.9 baseline |
| `__ieee754_rem_pio2f` | `0x0037C7D0` | 992 | GCC 2.9 baseline |
| `__kernel_rem_pio2f` | `0x0037CE40` | 2,380 | GCC 2.9 with `-fdata-sections` |

The reviews cover the entire original control flow, not only calls or constants:
cosine's compensated polynomial and tail; software square root's bit trials,
normalization and final rounding; logarithm's version-dependent exception and
errno paths; pi/2 reduction's signed special case, cancellation refinements and
large-input decomposition; and the reduction kernel's byte convolution,
carry complement, recomputation loop and every precision-output branch.
Software square root here is the runtime's iterative implementation, separate
from the game's hardware `SQRT.S` operations.

The complete 960-byte reducer section at `0x004551E8` matches unchanged source
and is mapped during actual linking. The reduction kernel's baseline 80-byte
combined pool fails whole-section comparison because its final alignment bytes
disagree. The supported `-fdata-sections` option instead emits separate input
sections: all 16 bytes of `.rodata.init_jk` at `0x004555D0`, and all 48 bytes of
`.rodata.PIo2` at `0x004555E0`, match original allocated readonly data including
padding. The genuine linker resolves both sections without source or byte
patches; the code still differs. This uses complete sections rather than
partial linker mappings.

The cosine 28-byte pool at `0x004555B0` and square-root 8-byte pool at
`0x004555A8` also match as complete data identities. Their selected code embeds
the constants, so these unused pools are not mapped by the code linker. The
logarithm wrapper's full 16-byte exception-name/negative-HUGE pool at
`0x004550F0` is genuinely linked, along with its separately evidenced positive
double-infinity reference at `0x004550D0`. The data checker verifies sixteen
complete upstream data identities in total and awards no code progress for them.

## Power wrapper and core

`powf.c` and `ieee754_powf.c` preserve the complete unchanged pinned newlib
[`wf_pow.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/wf_pow.c)
and
[`ef_pow.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/ef_pow.c),
including Sun Microsystems' 1993 permission notice and Ian Lance Taylor's
float-conversion credit. The manifest records both exact source hashes.

The original wrapper at `0x0037B4C0` is 1,288 bytes. Its complete exception
paths agree: NaN inputs, zero exponents and bases, finite negative exponents,
nonintegral negative bases, overflow sign selection using `rint(y/2)`, and
underflow. The review includes full double argument/result conversions,
version-dependent zero/one/HUGE/infinity/NaN returns, `matherr`, errno 33/34,
and the optional exception error override. Its whole 32-byte name and constant
section at `0x00455100` matches and genuinely links, as does the independently
evidenced infinity binding at `0x004550D0`.

The original power core at `0x0037BF40` is 2,188 bytes, ending with the return
delay instruction at `0x0037C7C8`; the following four zero alignment bytes are
excluded. Every original branch and arithmetic path agrees with the unchanged
source: odd/even exponent classification, special values, subnormal scaling,
interval selection, compensated logarithm polynomial, split exponent product,
128/-150 overflow and underflow limits, exponential reconstruction and the
subnormal `scalbnf` call. All 136 bytes of its array and constant section at
`0x00455160` match, including padding, and the normal linker resolves its
references.

Both selected GCC 2.9 recipes compile and fully link without unresolved
relocations. They differ from the complete retail code and remain
reconstructed, adding 3,476 represented original code bytes and zero exact
matches. Complete readonly data checks provide source identity evidence and
award no function progress.

Independent review of these seven newly registered math bodies covered every
original instruction and the complete unchanged sources, including return
delays, soft-double exception calls and all argument-reduction output branches.
No identity or boundary defects were found. The data checker reproduces all
sixteen complete constant identities from the pinned source files.

## GNU EE software floating-point runtime

Twenty-four complete runtime functions now reproduce 3,264 complete
retail code bytes from one unchanged upstream C file, `src/runtime/fp_bit.c`.
The source is pinned to the public GNU EE toolchain commit
[`b595ded606227e93b8c4a447446c1d2ac093827d`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/config/fp-bit.c).
Its complete SHA256 is
`c44e6a9dcd898b2689c62ed2be770a445dd30d15afe78d4010429552e07d2190`.
The original FSF copyright, GPL terms, unlimited permission to link the
compiled file, and additional GCC linking exception are preserved verbatim.
The two exception notices are also in `LICENSES/GCC-fp-bit-exception.txt`;
the complete GPL text is in `LICENSES/GPL-2.0.txt`.

The authentic [R5900 build recipe](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/config/mips/t-r5900#L14)
defines `US_SOFTWARE_GOFAST` and `NO_DENORMALS`, and defines
`FLOAT_BIT_ORDER_MISMATCH` for the little-endian target. The single-precision
copy additionally defines `FLOAT`. The repository supplies these defines as
per-function compiler flags while preserving the imported file unchanged.
`NO_DENORMALS` reproduces the original unpackers' treatment of zero-exponent
inputs as zero; it is part of the public EE recipe, not a match-specific source
change. These routines implement numeric packing, comparison, arithmetic and
integer/precision conversions without a floating-point exception-state API.

| Symbol | Original address | Complete bytes | Precision |
| --- | --- | ---: | --- |
| `__pack_d` | `0x00372858` | 300 | Double |
| `__unpack_d` | `0x00372988` | 156 | Double |
| `dpadd` | `0x00372C68` | 88 | Double |
| `dpsub` | `0x00372CC0` | 100 | Double |
| `__fpcmp_parts_d` | `0x00373138` | 276 | Double |
| `dpcmp` | `0x00373250` | 76 | Double |
| `litodp` | `0x003732A0` | 184 | Double |
| `dptoli` | `0x00373358` | 148 | Double |
| `dptoul` | `0x003733F0` | 160 | Double |
| `__negdf2` | `0x00373490` | 56 | Double |
| `__make_dp` | `0x003734C8` | 44 | Double |
| `dptofp` | `0x003734F8` | 84 | Double |
| `__pack_f` | `0x00373CA8` | 268 | Single |
| `__unpack_f` | `0x00373DB8` | 144 | Single |
| `fpadd` | `0x00374080` | 88 | Single |
| `fpsub` | `0x003740D8` | 100 | Single |
| `__fpcmp_parts_f` | `0x00374498` | 276 | Single |
| `fpcmp` | `0x003745B0` | 76 | Single |
| `sitofp` | `0x00374600` | 184 | Single |
| `fptosi` | `0x003746B8` | 140 | Single |
| `fptoui` | `0x00374748` | 152 | Single |
| `__negsf2` | `0x003747E0` | 56 | Single |
| `__make_fp` | `0x00374818` | 44 | Single |
| `fptodp` | `0x00374848` | 64 | Single |

Seventeen entries have real original decoded direct `JAL` references. Each unchanged
source symbol is compiled with the pinned `gcc29` profile, independently sized
by its ELF `STT_FUNC` symbol, and genuinely linked at its original address with
the established runtime bindings. The complete resulting bytes, including all
relocated calls and the return delay, match the retail SHA256. No relocation
mask, instruction rewrite, or data replacement is used for awarding matches.
These runtime matches identify a compatible recipe for these functions;
they do not identify the compiler used for every game translation unit.

The preliminary disassembly heuristic merges some adjacent library functions.
For example, its 216-byte region at `0x003733F0` contains the complete 160-byte
`dptoul` followed by a separate 56-byte negate helper. Both independent
functions are now registered with their own complete extents.
`__fpcmp_parts_f` occupies 276 bytes of the heuristic's larger
region at `0x00374498`. The manifest records the independently compiled
complete function extents, decoded call entry proofs, original full hashes,
and disjoint bounds; heuristic file length is not used as a function size.
The seven functions without direct call references are established by complete
independently compiled symbols, fresh original stack frames, matching entire
linked bodies and source call graphs, adjacent independent function boundaries,
and valid preceding/final return delays. Every local branch stays inside its
function; zero alignment bytes are excluded explicitly. The negate helpers
are bounded by the independently verified unsigned conversion and make
functions. The single compare and signed conversion sequence sits between
the exact parts comparator and unsigned conversion. The single add/subtract
frames follow the observed parts-adder return at `0x00374074` with its delay
at `0x00374078`, excluding zero padding at `0x0037407C`, and precede the
multiply frame at `0x00374140`. Absence of a direct caller is recorded openly.

Both double arithmetic wrappers call the same file-local parts adder at
`0x00372A28`. Their per-function binding records that address and its proven
position between the exact unpack and pack calls. That helper remains
unmatched and is now represented in the reconstructed core batch below.
The whole imported source also compiles other routines; their mere
presence in an object does not award recovery or matching progress. The
current batch needs no mapped constant sections or writable-data exception.

Root independently checked all seventeen original complete-body hashes,
terminal returns and delay extents, every recorded direct-call entry, the
unchanged source hash and complete linked comparisons. The full verifier
reproduced all seventeen matches, and the hybrid build retained the retail
executable SHA256 after their substitution.

The six remaining authentic parts-addition, multiply and divide cores add
3,032 represented original bytes from the same unchanged source. They are
fully linked with no unresolved references, and all remain reconstructed.

| Recovered label | Upstream compiled symbol | Original address | Complete original bytes | Linked comparison |
| --- | --- | --- | ---: | --- |
| `_fpadd_parts_d` | `_fpadd_parts` | `0x00372A28` | 576 | Same size, 120 differing bytes |
| `dpmul` | `dpmul` | `0x00372D28` | 680 | Same size, 2 differing bytes |
| `dpdiv` | `dpdiv` | `0x00372FD0` | 360 | Same size, 15 differing bytes |
| `_fpadd_parts_f` | `_fpadd_parts` | `0x00373E48` | 564 | Compiled 572 bytes, 401 differing bytes |
| `fpmul` | `fpmul` | `0x00374140` | 500 | Same size, 2 differing bytes |
| `fpdiv` | `fpdiv` | `0x00374338` | 352 | Same size, 15 differing bytes |

Every complete original graph was reviewed against the source: ordered NaN,
infinity and zero classification, sticky mantissa shifts, signed-zero rules,
cancellation and carry normalization, multiplication high/low products and
128-bit carry combination, XOR signs, exponent offsets, long division,
round-to-even guards and sticky remainders. Calls use the exact pack/unpack
functions and the independently identified low64 multiplication helper.
The single parts-adder ends at return `0x00374074` and delay `0x00374078`;
the following zero alignment word and adjacent add/subtract/multiply/divide
functions are excluded. The single multiply similarly excludes padding at
`0x00374334`. `_d`/`_f` distinguish recovered labels for two file-local source
copies; the unchanged source symbol remains `_fpadd_parts`.

The cores reference the source's static zero-initialized NaN record. The
double object has one complete 24-byte NOBITS section, bound to original
memory `0x0048F290`; the single object has one complete 16-byte section at
`0x0048F2A8`. Their exact 24-byte spacing and every original `LUI`/`ADDIU`
pointer are checked, and both complete extents lie inside retail writable
NOBITS `.bss`. The full zero-initialization digests are recorded in
`link_bss` metadata. These memory sections have no initialized original file
bytes and therefore have no fake file offsets or byte-identity data award.
The [strict separate NOBITS linking path](linking.md) retains source symbols
and genuinely links the whole unchanged sections with GNU `NOLOAD`, checking
all geometry and referenced addends. No initialized bytes are fabricated,
and the earlier read-only mapping rejection guards remain intact.

The two-byte multiply differences are address temporary register choices:
retail loads the NaN address upper half into `v0` before constructing `a0`,
while the candidate constructs it directly in `a0`. The already supported
GNU `-mdebuga` option was tested because it controls a related backend
address-folding path, but it did not alter these bodies. No source changes,
assembler rewrites or masked matches were substituted for full equality.

Root independently verified the 24 complete fp-bit exact comparison records,
the six core body hashes and return extents, unchanged upstream source hash,
all recorded direct-call entries, each original NaN-record pointer pair and
both complete NOBITS extents and zero digests. The 14 dedicated storage-linking
tests passed alongside the existing tooling suite.

## GNU integer multiplication and conversions

Five complete functions from unchanged GNU EE GCC `libgcc2.c` add 820 exact
high-level C bytes. The source comes from the same pinned public GNU repository,
[revision `b595ded606227e93b8c4a447446c1d2ac093827d`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/libgcc2.c).
The imported LF source has SHA256
`629b902c36ff32b0c14b372f49995988260def3d0850c8e9211395a5ab5a1d47`.
The preliminary vendor working copy had CRLF line endings and a different
file hash; the registered source is byte-for-byte equal to the pinned Git blob.

| Symbol | Original address | Complete bytes | Original direct JAL references |
| --- | --- | ---: | ---: |
| `__muldi3` | `0x00371CE0` | 96 | 18 |
| `__fixunsdfdi` | `0x00371318` | 236 | 3 |
| `__fixdfdi` | `0x0039F518` | 92 | 1 |
| `__fixunssfdi` | `0x00371408` | 244 | 10 |
| `__floatdidf` | `0x00371500` | 152 | 7 |

Each uses the existing `gcc29` profile and its authentic `L_` routine selector,
`IN_LIBGCC2`, `__GCC_FLOAT_NOT_NEEDED` and `inhibit_libc` build defines. The
source's GNU mode types explicitly represent SI32, DI64, SF32 and DF64 values;
host C type sizes are not substituted for their runtime ABI. Complete source
and header notices remain intact. `LICENSES/GCC-libgcc2-exception.txt` preserves
the source's original GPL notice and GCC linking exception, and
`LICENSES/GPL-2.0.txt` supplies the complete GPL. The exception belongs to this
source; the imported public GNU headers retain their own license notices.

`src/runtime/gcc/provenance.json` pins the whole source and fifteen required
unchanged GNU header files, including the target configuration headers,
machine-mode definitions and `longlong.h`. Two small configuration aggregators,
`tconfig.h` and `tm.h`, are copied from the genuine pinned GNU configure output
for `mips64r5900-sky-elf`; their generated status, exact bytes and recipe are
recorded separately. These include target headers without changing the backend.
The per-function manifest records explicit include paths and build flags, so
the runtime imports compile from their tracked layout.

Independent complete linked comparisons from that layout reproduce all five
retail bodies, with no unresolved references, constant-section mappings,
instruction rewrites or byte masking. External calls use the reviewed soft
floating-point pack/arithmetic/conversion runtime and the other independently
matched functions in this batch. The multiplication source computes the low64
product from low32 multiplication and both high-word cross-products. The
unsigned conversions derive a high word using division by `2^32`, subtract its
converted contribution and correct the low part according to its sign; the
single input is first promoted to double. The signed conversion handles the
sign around the unsigned helper. Integer-to-double conversion combines the
signed upper32 contribution, scaled twice by `2^16`, with the unsigned lower32.

Every entry has decoded original direct-call evidence, a complete independent
compiled function symbol, its full original SHA256 and terminal return/delay
extent. The original heuristic grouped an unrelated `J 0x00370388` and its NOP
after `__muldi3`; those eight bytes are excluded from its complete 96-byte body.
All five original extents are disjoint from already registered functions.

Root independently reviewed all five complete original bodies (205 instructions,
820 bytes), their genuine source algorithms and helper ABIs, eighteen exact
source/header/configuration hashes, all thirty-nine direct JAL entry references,
full function hashes and terminal delays. Every internal original JAL target
agrees with the established runtime binding set, and all five actual linked
comparison records are exact. The complete global verifier remains the
checkpoint authority for reproducing these matches from the tracked layout.

Four further complete functions reuse that same unchanged source and pinned
header package. Their full original control flow and runtime ABI have been
reviewed, and each full candidate compiles and genuinely links. All four remain
reconstructed because complete code equality has not been achieved.

| Symbol | Original address | Original / compiled bytes | Direct JAL references |
| --- | --- | ---: | ---: |
| `__floatdisf` | `0x00371598` | 224 / 220 | 2 |
| `__moddi3` | `0x00371678` | 1640 / 1624 | 2 |
| `__udivdi3` | `0x00371D48` | 1488 / 1472 | 12 |
| `__umoddi3` | `0x00372318` | 1344 / 1328 | 7 |

The signed integer-to-float source preserves a representative discarded bit
before its intermediate double conversion, avoiding double rounding outside
the open interval `(-2^53, 2^53)`. Its first 88 linked bytes are equal, but later
scheduling and the complete length differ. The arithmetic routines use the
authentic normalized two-half division implementation in `longlong.h`:
one-word and two-word divisors, sixteen-bit partial quotients, trial corrections,
the intentional zero-divisor trap, and the special paths that avoid a shift by
32. Signed remainder preserves the numerator sign after magnitude conversion;
unsigned quotient and remainder return their respective complete 64-bit result.
All branches, carry/borrow operations and return-delay extents were checked
against each entire original instruction interval.

Each of the three division/remainder objects contains its own complete static
256-byte `__clz_tab` read-only section. Those unchanged tables exactly equal
retail sections at `0x00454348`, `0x00454448` and `0x00454548`, respectively,
with SHA256 `14a5d850c255623f9472e3c650abce0c78d32f0276b315b3a276a0462d97a1ac`.
Three original `LUI`/`ADDIU` pointer pairs in each complete function establish
the corresponding base. Per-function whole-section mappings preserve these
distinct file-local copies; no single global `__clz_tab` alias replaces them.
The actual GNU linker resolves the references and checks complete mapped data
equality. Table identity alone earns no function matching credit.

These four original bodies total 4,696 bytes. Their complete SHA256 hashes,
twenty-three decoded original JAL entries, terminal return/delay instructions,
independently established adjacent runtime boundaries and disjoint registered
ranges are recorded. Fresh full linked comparisons have no unresolved
references; their differing code remains explicitly reconstructed.

## Case-insensitive string comparison

The complete 124-byte function at `0x003983E8` agrees with unchanged public
newlib 1.8.1
[`strcasecmp.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strcasecmp.c).
The imported source SHA256 is
`59bfbc23edcc0ba86cd71d82f8fd90acd88922d02a5886ca5824c7eb9af11872`.
It has no file-specific copyright notice, so the default Cygnus Solutions
1994/1997 permissive notice in section 9 of `LICENSES/newlib-1.8.1.txt` applies.
This software was developed at Cygnus Solutions.

All 31 original instructions were checked: the loop first tests the left
signed byte for zero, calls `tolower` on both signed bytes, and advances both
pointers only when those results agree. The final difference calls `tolower`
on unsigned bytes. The four original calls target the already reviewed
`tolower` at `0x0039CA58`. Sixty-five decoded original direct calls establish
the entry; the final return at `0x0039845C` and its delay at `0x00398460` exclude
the following alignment word at `0x00398464`.

`src/runtime/ctype_function_calls.h` is an explicit macro-only build adapter.
It undefines the GNU header's `tolower` and `toupper` macros to retain those
observed library calls; the imported upstream C stays byte-for-byte unchanged.
This preserves the original signed-char loop behavior, including negative
byte values, rather than substituting an ASCII-only comparison. The full
genuine linked GCC 2.9 candidate is also 124 bytes, but differs in twenty bytes
of register-save widths/slots and prologue/epilogue scheduling. GCC 3.2.3 emits
140 bytes. The function therefore remains reconstructed, with both full
comparison records retained and no match credit.

## Declared source provenance checks

`tools/source_provenance.py` validates every explicitly declared upstream source
SHA256 and adapter dependency before the verifier starts compiling any function.
The hybrid build performs the same complete preflight before invoking its
subprocesses. The nine GNU `libgcc2.c` entries explicitly name their package
manifest, so all eighteen source, header and genuine generated-configuration
hashes are checked as well; the package must include the exact function source
with the same declared hash. Missing files, corrupted headers, conflicting or
malformed declarations and paths outside the workspace fail the preflight.
The optional fields remain compatible with reconstructed project code that has
no upstream provenance declaration. These checks establish byte preservation
of the recorded inputs; they do not infer source identity or license eligibility.
Fourteen focused tests cover corruption and missing dependencies, complete
package binding, optional metadata, adapter changes and both entry points
rejecting a later bad declaration before any compiler or build subprocess runs.

## Cosine wrapper and rejected-character prefix

The complete cosine wrapper at `0x0037AFD8` adds 232 exact high-level C bytes
from unchanged public newlib 1.8.1
[`sf_cos.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/sf_cos.c).
Its source SHA256 is
`f8f2fed57fdf4fb434ec0f74af85a8c4b8d648ecd3d53a6b2733f8429b72759a`;
the full Sun Microsystems 1993 permissive notice and Ian Lance Taylor
conversion credit remain intact. The same GCC 2.9 flags used for the reviewed
sine wrapper produce complete linked equality, with SHA256
`7821faaad4076dacd4c14d5e0f228390c8c224be263f83268e6bdfe12c3eae71`.
Every magnitude threshold, nonfinite `x-x` branch, reducer call and all four
`n & 3` cosine/sine/sign selections agree with the original instructions.
Ten original direct calls and the complete return/delay extent independently
identify the entry. Its common `fdlibm.h` dependency is the already imported,
unchanged official newlib 1.10.0 header described above, with its own explicit
dependency hash; it is not claimed to come from the older source repository.

The adjacent complete 108-byte string routine at `0x00398468` reuses unchanged
newlib 1.8.1
[`strcspn.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strcspn.c),
SHA256 `042b87f217f959a6845b6dd5710a7f52179db07a1e403f14e4a2da96641d6085`.
It has no file-specific notice; the default Cygnus Solutions 1994/1997
permissive notice in section 9 of `LICENSES/newlib-1.8.1.txt` applies. This
software was developed at Cygnus Solutions. All 27 original instructions agree
with the outer source-prefix loop, inner reject-string search and final pointer
difference. Seven original direct calls establish the entry; the following
alignment word is excluded. Full genuine links have no unresolved references,
but emit 104 bytes with GCC 3.2.3 and 92 with GCC 2.9. `strcspn` remains
reconstructed. The runtime exact totals are now 37 C functions / 4,820 bytes
and three separate upstream assembly functions / 572 bytes.

## Lowercase conversion, string scanning and rounding copies

Five further complete functions reuse unchanged public newlib 1.8.1 source
from the same pinned GNU repository revision
`b595ded606227e93b8c4a447446c1d2ac093827d`. Their original bodies total 968
bytes. Four bodies totaling 960 bytes remain reconstructed; the eight-byte
scanner callback is a complete high-level C match.

| Function | Original address | Original / selected compiled bytes | Status |
| --- | --- | ---: | --- |
| `strlwr` | `0x003984D8` | 124 / 124 | Reconstructed |
| `sscanf` | `0x00395350` | 152 / 136 | Reconstructed |
| Local `eofread` from `sscanf.c` | `0x00395348` | 8 / 8 | Exact C |
| `ceilf` | `0x0037AF00` | 212 / 224 | Reconstructed |
| Numeric entry `func_0037AD28`, floor algorithm | `0x0037AD28` | 472 / 480 | Reconstructed |

The imported
[`strlwr.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strlwr.c)
has SHA256
`723eb1a5dc76754d025bca292a3c7fe917528c4da0c472f1690397345bf87aa6`.
All 31 original instructions agree with its signed-byte cursor loop, uppercase
classification test, conditional `tolower` call, byte store and initial-pointer
return. Seventeen decoded original calls establish the entry. The original
`LUI`/`ADDIU` establishes `_ctype_+1` at `0x00456119`, whose complete unchanged
table was already reviewed. The explicit macro-only forced include keeps the
observed external `tolower` call while preserving the upstream C bytes. This
file has no separate notice; the default Cygnus Solutions 1994/1997 permissive
notice in section 9 of `LICENSES/newlib-1.8.1.txt` applies. This software was
developed at Cygnus Solutions.

The public
[`sscanf.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/sscanf.c)
has SHA256
`23969007e1f29c448b24da604cb416ef71fba0d3923796c38fca93626506e432`.
All 38 original wrapper instructions agree with the source's temporary
`FILE`, flags, string pointers and lengths, EOF callback, null ungetc/line
buffers, reentrancy data and variadic scanner call. Both authentic compiler
profiles independently compiled the genuine `FILE` size and field offsets:
88 bytes, with `_read` at `0x20` and `_data` at `0x54`. Twenty-four decoded
original calls establish the wrapper entry. Its terminal return and delay end
at `0x003953E8`; the heuristic's following unrelated prologue is excluded.

The local `eofread` callback is independently compiled as a local `STT_FUNC`
symbol of exactly eight bytes by both compilers. Both genuine full links equal
the retail bytes with SHA256
`008d26890102af179c703d77fae42cb7b3f424a11b8db552383dd8e5313f4061`.
Original `LUI`/`ADDIU` instructions in `sscanf` materialize `0x00395348`, and
the subsequent store installs it in the temporary `FILE._read` field. The
preceding formatter's complete return/delay and the following `sscanf` entry
establish disjoint boundaries without requiring a direct JAL to this callback.
The function returns zero at string EOF, exactly as the unchanged source does.
The full Berkeley 1990 notices remain in `sscanf.c` and its private `local.h`.
This product includes software developed by the University of California,
Berkeley and its contributors.

The unchanged public
[`sf_ceil.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/sf_ceil.c)
preserves its Sun Microsystems 1993 notice and Ian Lance Taylor conversion
credit. Every exponent branch, signed-zero/positive-one case, fractional mask,
positive bias, `1e30` inexact check and nonfinite `x+x` result agrees with the
complete 53-instruction original. Five decoded calls establish the entry, and
the following alignment word is excluded. The constant bits `0x7149F2CA` are
independently present in the original instructions. The full candidate differs.

The second complete double-floor body at `0x0037AD28` reuses the already
imported, unchanged
[`s_floor.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libm/math/s_floor.c),
SHA256 `bfcaab777d6cde5ebb3402b56ae57f8f17155cb6ec7a81793dfe67290616ddda`.
Its original source name remains unknown; the explicit
`-Dfloor=func_0037AD28` build definition renames only the source definition and
prototype, preserving the complete body and the separate existing `floor`
binding at `0x0039CCD0`. All 118 original instructions agree with the high/low
fractional masks, negative adjustment and carry, signed-zero handling,
`1e300` inexact checks and nonfinite result. Ten decoded calls establish the
entry. Three genuine original `LUI`/`LD` pairs prove the constant at
`0x004550C8` inside the entire 16-byte readonly source section at `0x004550C0`.
That whole section, including alignment, matches SHA256
`b872a7ff1f9de62bf2879a07bc8b2264db3987e2460a65ec5807c0b712d182fe`.
The real linker resolves it without data or instruction edits. Data identity
earns no code-match credit. The Sun Microsystems notice remains unchanged.

Actual compiler dependency lists are retained with per-file SHA256 checks.
The existing system headers come from the pinned public PS2DEV
[`2018-10-19` archive](https://github.com/ps2dev/ps2toolchain/releases/tag/2018-10-19)
and explicitly retain their newlib 1.10.0 provenance; they are not attributed
to the older C source pin. Compiler headers retain their separate GNU profile
provenance. The math imports use the unchanged, separately pinned official
newlib 1.10.0 `fdlibm.h`. Header files remain in the hash-verified bootstrap
paths, and the declared-source preflight checks every recorded dependency
before compiling. A fresh matching setup runs both documented bootstraps
before build or verification.

Root independently reviewed all five entire original/source bodies and hashes,
38 actual header dependency files, 56 genuine JAL entry references, the three
floor pointer pairs and whole readonly object, the scanner's callback store
and both authentic `FILE` layouts before registration. All ten independent
full links have no unresolved references. Exact runtime totals are now 38 C
functions / 4,828 bytes and three separate assembly functions / 572 bytes.

The complete scanner core and scanset helper now reuse unchanged public
[`vfscanf.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/vfscanf.c),
SHA256 `824bad6c45dd382a66a3e5f0cdc0d770dd68d4e39cbe89359abc0bac7afcce57`.
The original `__svfscanf` interval is `0x00395558..0x00396068`: 2,832 bytes,
including the terminal return and its delay instruction. The independently
entered `__sccl` interval is `0x00396068..0x00396158`: 240 bytes. The pair
recovers 3,072 original code bytes, with both functions reconstructed. Their
selected genuine GCC2.9 linked bodies have 2,784 and 260 bytes respectively;
neither full body matches the original.

The full 708-instruction scanner agrees with the upstream multibyte format
traversal and persistent state enabled by `-DMB_CAPABLE`, signed format and
classification bytes, suppression/length/width flags, whitespace and literal
handling, all five conversion categories, refill/ungetc calls, assignment/read
counts and the 350-byte numeric buffer with its 349-character cap. The
original integer conversion and `%n` paths store 64-bit `long` values with
`SD`, 32-bit integers/pointers with `SW`, and shorts with `SH`. Floating
`%l`/`%L` stores use the original soft double result; the ordinary float path
calls the already proven conversion helper. Both authentic compiler profiles
independently passed `sizeof(long)==8`, pointer/int size 4, double size 8 and
`FILE` size 88 checks. The full 60-instruction scanset helper agrees with
the optional inversion, 256-byte initialization, closing-bracket and hyphen
handling, signed format cursor, early NUL result and V7 chained-range rule.
Genuine original direct calls establish both function entries.

Six original `LUI`/`ADDIU` pairs establish the whole readonly scanner pool at
`0x00456230`, size 1,136 bytes, SHA256
`c8183412d76019600c6dc08ceeec9eaf8eb48b295d02aab2c08c995934f34e59`.
It contains the complete 17-short base table, four switch tables with 263
decoded original targets, and all intervening literal/alignment bytes.
The explicitly opted-in
[generated readonly linker path](scanner_linking.md) permits only actual
compiler `R_MIPS_32` entries targeting proven labels strictly inside the
selected complete function; every other byte must independently equal the
original. The actual GNU linker places the entire source section at its
original address. All 84 unrelocated bytes agree, while the complete linked
table differs because its internal case labels differ. This earns no data
identity or code-match credit, and the hybrid build retains the original
scanner code/table until both complete comparisons pass.

The unchanged private
[`floatio.h`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/floatio.h)
has SHA256 `ceb13c8d94569d2ea6eeaecdd2953be30c003222733808e685118aaf12269045`;
the existing unchanged `local.h` is reused. Complete Berkeley 1990 notices
remain in all three files. This product includes software developed by the
University of California, Berkeley and its contributors.

A fresh tracked-layout compiler dependency scan found the same 20 source and
header files and hashes as the independently reviewed probe. The source and
private headers retain their public newlib 1.8.1 Git pin, system headers retain
the pinned PS2DEV archive's newlib 1.10.0 provenance, and GNU headers retain
their separate compiler release provenance. Every dependency is guarded
before compilation. Root independently reviewed all 768 original instructions,
unchanged source/private headers, 21 original direct calls, entry/branch
boundaries, six pointer pairs and their nonclobbering intervening shifts,
all switch targets/base values/literal bytes and actual ABI checks, then
reproduced both full linked comparisons before approving registration.
The tracked-layout reproduction has zero unresolved relocations and exactly
the same complete code/table hashes. Exact runtime totals remain 38 C
functions / 4,828 bytes and three separate assembly functions / 572 bytes.

Five further complete stdio functions recover 848 original code bytes from
unchanged public newlib 1.8.1 source at the same pinned Git revision:
[`sprintf.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/sprintf.c),
[`fread.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/fread.c),
[`fwalk.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/fwalk.c),
[`fflush.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/fflush.c)
and the local callback in
[`refill.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/refill.c).
Their full source SHA256 values are respectively
`c9b9e12b5f91e95a53b1d32aab4bf5cec68b29d2612fd01612e951adef37a7c2`,
`aa3b9711739619c69010a7efcaac0fac4ffa47452e0234817f0f291724831d76`,
`d0e051dab8bfa43ea17eb54d0b11a298a6871fe710d46260936002593b12a73f`,
`734ccf4f8b90b2e690cc9bddd165260b450f3efa3048b4ca73f71a1818084513`
and `5b62c3eab3818b89d7606b13265b2237514635b9c0d9f225dae5e3730b120cf7`.
The original intervals are `sprintf` at `0x003952C8` / 128 bytes,
`fread` at `0x00394808` / 276 bytes, `_fwalk` at `0x00394D30` / 148 bytes,
`fflush` at `0x00394290` / 268 bytes and local `lflush` at `0x00395080` /
28 bytes. All returns and delay instructions are included; neighboring
functions and alignment words are excluded. The former heuristic `sprintf`
interval included the separately proven eight-byte EOF callback, which is
excluded here.

The four larger complete bodies remain reconstructed. The authentic local
`lflush` function matches all 28 bytes under both genuine compiler profiles,
SHA256 `88550787cdb91a0be9a3859481501ee51adbf65f5bb3b4ea6f24d6784a4d55e9`.
Its source preserves an original precedence bug:
`(flags & (__SLBF | __SWR)) == __SLBF | __SWR` evaluates as
`((flags & 9) == 1) | 8`, so it always calls `fflush`. The original upper
instruction at `0x00395194` and lower `ADDIU` at `0x003951A0`, executed in
the `_fwalk` call's delay slot, establish the callback argument and entry.
The previous `puts` return/delay, the complete compiler local function symbol,
following padding and independent refill entry corroborate its full extent.
Only this local function is registered from `refill.c`; the remaining refill
body differs from the public source in its ungetc-buffer release path and is
not claimed as recovered.

The unchanged `fread` source uses explicit `-Dmemcpy=func_003947D8` to bind
its two copy calls to the observed custom 48-byte forward byte loop. Its
original source and return contract remain unknown. Both genuine encoded
calls occur in `fread`, and both overwrite the return register before reading
it, so the renamed declaration's unused pointer result introduces no observed
caller behavior. The custom copy itself is unregistered. The separate
formatter called by `sprintf` also retains a proven numeric binding without
claiming its source identity. The complete FILE, reent and linked-glue field
layouts were checked with both authentic compilers against original offsets,
including the 88-byte FILE stride and reent glue offset `0x1D8`.

The imported source files and reused private `local.h` retain their complete
Berkeley 1990 notices. This product includes software developed by the
University of California, Berkeley and its contributors. The GNU limits
header is the unchanged pinned `ee/gcc/glimits.h`; system headers retain the
separate public PS2DEV archive's explicit newlib 1.10.0 provenance. Fresh
tracked-layout dependency scans reproduce all reviewed hashes across 39
distinct actual source/header files, and every selected dependency is guarded
before compilation. Root independently reviewed all 212 original instructions,
the complete source/private headers, actual callers and branch boundaries,
callback materialization, discarded copy results, both target layouts and all
ten fresh full links. The five tracked-layout links reproduce the approved
complete hashes with no unresolved relocations. Exact runtime totals are now
39 C functions / 4,856 bytes and three separate assembly functions / 572 bytes.

The complete `qsort` body at `0x00396788` / 2,524 bytes reuses unchanged
[`qsort.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib/qsort.c)
from that public newlib 1.8.1 revision. The source SHA256 is
`7368f5037292be52f4ac2486a704b8c1863faeba2fc16462e4258bd34e907bad`.
The original full body SHA256 is
`2512c55c0283be6bca118d713daf759f40994e7d57bc484a07c91c7876960a42`.
All 631 original instructions agree with the Bentley/McIlroy algorithm,
including its three swap representations, short-array insertion sort,
median-of-three and larger-array ninther pivots, equal-key partitions,
zero-swap insertion fallback, vector swaps, left recursion and right tail
iteration. The actual target has eight-byte `long`, four-byte `int` and
four-byte pointers, which matters for both the aligned swaps and arithmetic.
All 21 encoded callers include the original recursive call; all 98 direct
branch destinations remain inside the complete body. Its final return and
delay instruction end at `0x00397160`; padding at `0x00397164` is excluded.

Root independently reviewed the full original body, pinned source and notice,
caller/branch/boundary evidence, both compiler dependency sets and two fresh
full links before approving import. A tracked-layout reproduction uses the
same eight authentic source/header dependencies and guarded hashes. GCC 2.9
produces 2,596 bytes with 1,963 differing bytes and zero unresolved relocations;
GCC 3.2.3 produces 2,476 bytes with 2,230 differing bytes. This complete source
recovery remains reconstructed and adds no exact-byte award. The Berkeley
1992/1993 notice remains intact in the source. This product includes software
developed by the University of California, Berkeley and its contributors.

The complete David Gay string-conversion source is reused unchanged from
[`strtod.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib/strtod.c)
and its private
[`mprec.h`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib/mprec.h)
at that public newlib 1.8.1 revision. Their source SHA256 values are
`fe27824e57ec2080b4cb81d87eb90cc3ddf74d6fae8d55b2d28c3b5bf9a46dc8`
and `5b97ce36307d50b8f90a3595e4836be1e41e016b8b3b381ebf9ddcd8395c09da`.
The isolated compatibility directory contains the unchanged pinned 1.8.1
`sys/reent.h` and the unchanged canonical target `float.h` from the public
PS2DEV 2018 archive. Each actual dependency is fingerprinted before compilation.
Both authentic compilers establish `int` = four bytes, `long` = eight bytes,
pointers = four bytes, `ULong` = four bytes and the original 24-byte Bigint
layout. The older reent header supplies that genuine `Long`/`ULong` contract;
the remaining archive system headers have explicit newlib 1.10.0 provenance.

The original `_strtod_r` at `0x00397208` is 3,832 bytes / 958 instructions,
SHA256 `c696c52c72639999d2c50b01c7852744865dbe2265adabb014ca53e4e844ca2e`.
Its whole algorithm agrees with the pinned source: whitespace/sign parsing,
digit accumulation, malformed and clamped exponents, quick and long scaling,
Bigint refinement, half-ULP ties, overflow/underflow and final end-pointer/sign
handling. All 74 original helper calls, 141 bounded branches, 46 switch
destinations and tolerance constants were checked against the full source.
Root independently reviewed every original instruction and the complete
source, private header, authentic ABI and dependency sets.

The main body remains reconstructed with `link:false`. GCC 2.9 emits a
3,788-byte unlinked function retaining 106 actual relocations; GCC 3.2.3 emits
4,016 bytes retaining 104. These object comparisons are explicitly unlinked
and confer no exact-byte award. The actual input readonly section is 208 bytes:
a 184-byte character table followed by 24 bytes of tolerance constants.
The original table at `0x004566E0` is only 184 bytes; its following bytes have
a different identity. The original tolerance constants occur inline in code,
and an equal separate 24-byte pool at `0x00453A88` does not establish contiguous
section geometry. No source/object section is split or patched, and no mapped
data or per-function binding forces the main body to link. Its helper-role
bindings identify the original call graph without claiming those helpers'
source recovery.

The separately bounded wrappers add 80 bytes. `strtod` at `0x00398100` is
44 bytes and remains reconstructed after actual full linking with zero
unresolved relocations. `strtodf` at `0x00398130` matches its complete 36 bytes
under both compilers, SHA256
`f795a10a201d741b444dd9c96e5cec9f9ca75ee7262fe6d6427c28f11d9021c0`.
Its unchanged source calls `strtod` followed by the proven double-to-float
helper. Authentic complete compiled symbol extents, original returns and delay
instructions, and independently bounded preceding/following entries establish
the unreferenced float wrapper's extent. The former 84-byte heuristic region
combined both wrappers and alignment; padding at `0x0039812C` and `0x00398154`
is excluded. Root independently reproduced all four genuine wrapper links,
and fresh tracked-layout dependency/link checks reproduce the approved hashes.
These three entries recover 3,912 original bytes; exact runtime totals are now
40 C functions / 4,892 bytes and three separate assembly functions / 572 bytes.

The full AT&T notice below is retained in both unchanged source files and is
included here as required for supporting documentation:

```text
/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991 by AT&T.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR AT&T MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/
```

The 17 decimal-support and Bigint functions in
[`mprec.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib/mprec.c)
are now reused unchanged from the same pinned public newlib 1.8.1 revision.
The source SHA256 is
`67f94f948941463ddc14645368d3f104f7bd0de4ec3232248c7f303cfac49732`.
Its full David Gay / AT&T 1991 notice remains in the source and the supporting
documentation above. The existing private `mprec.h`, genuine reent layout and
canonical target `float.h` are reused with their guarded provenance.

These complete bodies recover 4,076 original bytes / 1,019 instructions:
Bigint allocation/free, multiply-add, string accumulation, leading/trailing
zero scans, small-integer conversion, limb multiplication, cached powers of
five, shifts, comparison/difference, ULP construction, Bigint/double conversion,
ratio and decimal power lookup. The author and root each read the full original
instruction intervals and unchanged source. Root independently checked all
17 original hashes/boundaries, 53 actual direct entry calls and their containing
bodies, 29 helper calls, 96 bounded branches, real target ABI layouts and 51
fresh complete linked comparisons. A tracked-layout reproduction checks the
actual dependency hashes and reproduces every full comparison without patching
source, instructions, relocations or initialization bytes.

Four complete C functions match under authentic GCC 2.9: `_hi0bits` at
`0x0039E770` / 132 bytes, `_lo0bits` at `0x0039E7F8` / 192 bytes, `__mcmp` at
`0x0039ED50` / 104 bytes, and `_Bfree` at `0x0039E4F8` / 52 bytes. The free
routine uses the supported `-fno-strict-aliasing` option to retain the original
freelist reload after writing the freed node. These four full matches add
480 bytes; the other thirteen actually linked bodies remain reconstructed.
Exact runtime totals are now 44 C functions / 5,372 bytes and three separately
reported assembly functions / 572 bytes.

The former 304-byte heuristic interval at `0x0039F328` contains two independent
functions. `_ratio` ends with its return/delay at `0x0039F3E0` / `0x0039F3E4`;
`_mprec_log10` begins with a separate prologue at `0x0039F3E8` and ends at
`0x0039F44C` / `0x0039F450`. Its excluded padding at `0x0039F454` precedes the
independently called calloc entry. The complete log function is 108 original
bytes; genuine compiled extents are 108 bytes under GCC 2.9 and 120 under GCC
3.2.3. Both full compiled bodies are compared and remain reconstructed. Its
original signed `dig < 24` table lookup preserves the source's absence of a
lower-bound guard.

Four whole readonly source sections have independent byte and pointer proofs:
the 16-byte powers-of-five section at `0x00456D88` (12 initialized bytes plus
four authentic padding bytes), the 200-byte powers-of-ten array at
`0x00456D98`, and the 40-byte large/small exponent arrays at `0x00456E60` and
`0x00456E88`. Both compilers emit all 296 bytes identically, with the verified
alignment/flags and no data relocations. Real original instruction pairs bind
each address. The powers-of-five and ten sections are mapped whole for their
actual source references; these data identities receive no function or text
matching award.

The complete byte substring search at `0x00398628` / 116 bytes now reuses
unchanged public newlib 1.8.1
[`strstr.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string/strstr.c).
Its source SHA256 is
`09437b966be6a045479e511b84d9ef0156823a5c9ccd29dc4c93d0f7c53998e7`.
No file-specific notice appears in that source; section 9 of
`LICENSES/newlib-1.8.1.txt` supplies the applicable Cygnus 1994/1997 permissive
notice. This software was developed at Cygnus Solutions.

Both author and root read all 29 original instructions against the complete
unchanged source. The empty-string guards, signed-byte comparisons, 32-bit
inner index, outer advance and return paths agree. Root independently checked
121 encoded entry calls and their containing assembly bytes, three returns
with their delay slots, six bounded branches, seven signed-byte loads and the
real target ABI. Two fresh complete links and both seven-file dependency sets
reproduce after importing the tracked source. GCC 2.9 emits 108 bytes and GCC
3.2.3 emits 88 bytes; both differ from the complete original body and remain
reconstructed. No exact-text or data award is added.

Two complete allocator functions now reuse unchanged
[`mallocr.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib/mallocr.c)
from the same public newlib 1.8.1 pin: `_calloc_r` at `0x0039F458` / 188 bytes
and `_malloc_r` at `0x0039B508` / 1,840 bytes. The unchanged source SHA256 is
`8a3fe01c7c61463488c7bcb5cc284277961ef493e94587e335d4134958bdea3e`.
Its complete header retains Doug Lea's public-domain dedication and version
2.6.4 identification. The genuine source options `INTERNAL_NEWLIB`,
`MALLOC_ALIGNMENT=16` and the appropriate `DEFINE_CALLOC` / `DEFINE_MALLOC`
select these functions without modifying their source.

The author and root read all 507 original instructions against the complete
source and supporting macros. The original wrapped 32-bit calloc product,
small clearing ladder, exact-fit and remainder bins, ordered unlink/reinsert,
block mask traversal, top-chunk split and extend/recheck paths agree. Actual
ABI probes confirm int/size_t/pointer 4, long 8, chunk 16 and mallinfo 40 bytes.
Root independently reproduced four complete links, actual 17/16-file
dependency sets, all original interval hashes and branches, nine encoded entry
calls and seven preserved global-address pairs. The tracked-layout source and
dependency guards reproduce the same full nonexact comparisons with no
unresolved relocations. Both functions remain reconstructed.

The local `malloc_extend_top` entry at `0x0039B2A0` / 612 bytes is separately
reconstructed from the same unchanged source, as described below. Its original
malloc call and genuine compiler-local function symbols establish independent
entry and source boundaries. The custom original lock/unlock wrappers remain
bindings. The free/trim/realloc continuation below adds three more bodies from
the unchanged source.

The writable 1,032-byte bin initializer at `0x00405A38` has separate source
identity evidence. Genuine GNU ld resolves all 256 original self `R_MIPS_32`
entries at the original address under both compilers, and all 1,032 output
bytes match. Six adjacent complete scalar/struct globals also match, including
the target's 64-bit long counters. Root independently reproduced these data
checks. They establish bindings only: no writable-data mapper, initialization
edits, text-match award or readonly-data-progress award is used.

The same pinned public-domain source now also reconstructs `_free_r` at
`0x0039BC38` / 756 bytes, `_malloc_trim_r` at `0x0039BF30` / 364 bytes and
`_realloc_r` at `0x0039C0A0` / 1,404 bytes: three complete functions totaling
2,524 original bytes. Authentic `INTERNAL_NEWLIB`, `MALLOC_ALIGNMENT=16` and
`DEFINE_FREE` / `DEFINE_REALLOC` select the unchanged active source. The
trim symbol is the source's genuine reentrant macro name. All selected
callee/global references are centrally bound from actual original calls and
preserved address pairs; no per-function forced bindings are needed.

The author and root independently read all 631 original instructions and the
complete active source/macros. They checked null/free paths, backward and
forward chunk coalescing, the last-remainder/bin links, source unsigned 64-bit
trim arithmetic with 32-bit sbrk arguments, and realloc's exact 3/5/7/9-word
copy ladder, adjacent growth, fallback, split and nested free order. Root
reproduced all six full links and dependency sets, 22 genuine ABI words, real
entry calls with whole containing-body byte proofs, 80 bounded branches and
11 preserved global pairs. Actual `long` is eight bytes while size/pointers
are four; the source's overflow, alias and call behavior is retained.

All six complete linked comparisons are nonexact and have zero unresolved
relocations. These three functions remain reconstructed; trailing original
and compiled alignment is excluded according to their independent full body
and symbol extents. Source/dependency guards pass, and a second link using
only the registered central aliases reproduces each comparison. This is
source/control/data identity evidence, without a native allocator behavioral
harness or exhaustive malformed-heap/EE exception claim. Writable initializer
evidence remains identity-only.

Two more complete bodies now reuse that unchanged `mallocr.c`:
`malloc_extend_top` at `0x0039B2A0` / 612 bytes and `_memalign_r` at
`0x0039C620` / 464 bytes. The internal extend helper has a real direct call
from `_malloc_r`, a separate original prologue and terminal return, and a
genuine `LOCAL STT_FUNC` under both compilers. Its complete compiled extents
are 620 and 572 bytes; neither is substituted for the independent 612-byte
original interval. The `DEFINE_MALLOC` object also contains `_malloc_r`, which
is selected and counted separately.

The aligned allocation body proves the supported upstream option
`SIZE_T_SMALLER_THAN_LONG`. Its original unsigned 32-bit size comparison forms
a negative full 64-bit difference when the chunk is smaller than the padded
request, and otherwise forms the positive difference, before the signed
minimum-chunk comparison. With this option the unchanged source retains that
behavior. GCC 2.9 emits the full 464-byte body with 26 differing bytes; it
remains reconstructed. No source, instruction or compiler backend adjustment
was made to resolve those differences.

Seven connected reentrant syscall wrappers also reuse unchanged public
newlib 1.8.1 sources from the same pinned revision:

| Source | Function | Original address | Bytes |
| --- | --- | --- | ---: |
| `reent/closer.c` | `_close_r` | `0x0039DAC8` | 88 |
| `reent/fstatr.c` | `_fstat_r` | `0x0039DB20` | 92 |
| `reent/lseekr.c` | `_lseek_r` | `0x0039DB80` | 96 |
| `reent/openr.c` | `_open_r` | `0x0039DBE0` | 96 |
| `reent/readr.c` | `_read_r` | `0x0039DC40` | 96 |
| `reent/sbrkr.c` | `_sbrk_r` | `0x0039DCA0` | 92 |
| `reent/writer.c` | `_write_r` | `0x0039DD00` | 96 |

These files have no separate copyright notice. The distribution's default
Cygnus Solutions 1994/1997 notice is retained in
`LICENSES/newlib-1.8.1.txt`, section 9. This software was developed at Cygnus
Solutions. All seven source files are imported unchanged; their exact source
and actual header dependency hashes are checked before compilation.

The nine bodies total 1,732 original bytes. Author and root independently read
all 433 original instructions, the complete active sources and supporting
macros. Root reproduced 16 encoded entry references with whole containing
assembly-byte intervals, 41 bounded branches, 13 preserved global-address
pairs, both genuine 26-word ABI probes and all 18 natural full links. The ABI
uses int, size and pointer widths of four bytes, `long`/`off_t` widths of eight,
and the reentrant error field at offset zero. The wrappers preserve the actual
syscall arguments, clear the source global error word, propagate a nonzero
error only on the original failure sentinel, and return the syscall result.

The accessed four-byte global `errno` at `0x00496158` is independently proved
inside the original writable NOBITS section, including all seven original
clear-store address pairs. The genuine `sbrkr.c` definition produces a
four-byte zero-initialized global under `-fno-common`. It is bound to the
original address for code linking; neither initialized data nor NOBITS is
mapped or counted here. The OS functions and custom locking routines remain
numeric bindings, without importing generic replacement stubs.

After registration, 18 fresh tracked-layout links using only the central
bindings reproduce every full comparison and actual dependency set. All nine
bodies remain reconstructed with zero unresolved relocations and no new exact
code award. No native OS or allocator behavioral harness, complete enclosing
function claim for heuristic caller intervals, or exhaustive malformed-input
or EE exception claim is made.

Five connected stdio helpers now reuse unchanged public newlib 1.8.1
[`stdio.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/stdio.c)
and
[`flags.c`](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/flags.c)
from the same pinned public revision:

| Function | Original address | Bytes | Status |
| --- | --- | ---: | --- |
| `__sread` | `0x003953E8` | 100 | Reconstructed |
| `__swrite` | `0x00395450` | 128 | Reconstructed |
| `__sseek` | `0x003954D0` | 104 | Reconstructed |
| `__sclose` | `0x00395538` | 32 | Matched |
| `__sflags` | `0x0039DD60` | 168 | Reconstructed |

The complete Berkeley 1990 notices remain in both sources, with the full
distribution notice in `LICENSES/newlib-1.8.1.txt`, section 1. This software
was developed by the University of California, Berkeley. Existing unchanged
private `local.h` and `sys/reent.h`, and the actual bootstrap headers, are
reused with source and dependency fingerprint guards.

Author and root independently read all 133 original instructions, the full
unchanged sources and 22 original constructor instructions. Eight original
function-address materializations store the four callbacks in the actual
`FILE` fields; the mode parser also has a genuine encoded call from `fopen`.
The successful-fd branch in that caller executes the read-callback LUI in its
delay slot and bypasses the failure path, which wipes the register and exits.
This path proof avoids treating the two routes as one linear materialization.
Both genuine 34-word ABI probes establish the 88-byte `FILE`, signed halfword
file descriptor, four-byte cached offset and eight-byte `long`, `off_t` and
`fpos_t`. The target headers leave `O_BINARY` and `O_TEXT` undefined.

The callbacks preserve the append seek before write, fresh `FILE` reloads
after that seek, full-width seek result and failure sentinel, and the distinct
cached offset narrowing. The flags parser preserves the original short
circuit reads, invalid-mode error store, access/create/append flags and final
output store. Original terminal returns, delay slots and separate callback
prologues establish the full intervals; the heuristic scanner grouping under
`sscanf` is rejected. The source compiler also emits four separate complete
`STT_FUNC` symbols from `stdio.c` and one from `flags.c`.

After import, ten fresh tracked-layout links using only the central bindings
reproduce all original comparison scores and actual dependency sets, with
zero unresolved relocations. Both compilers match every byte of the complete
32-byte `__sclose`; the other four complete bodies remain reconstructed. The
five functions total 532 original bytes. This is source, control and entry
identity plus the stated complete match, without a native OS harness,
complete enclosing caller or dispatch claim, or EE precision/exception claim.


The stdio setup/open batch reuses unchanged public newlib 1.8.1
[findfp.c](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/findfp.c)
and [fopen.c](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/fopen.c).
This software was developed by the University of California, Berkeley.
Both complete 1990 Berkeley notices are retained under section 1 of
`LICENSES/newlib-1.8.1.txt`. Raw source SHA-256 pins are
`c287d8427ec98c15dfbd1bc435db3b60bc1643852badc9d192de3a08dcf14ff1`
and `b9074c67e46244b093080b5a13039f2a6e6314ce719cba6461aa874ffcb2353c`.
The authentic private header, reentrancy layout and compiler headers remain
unchanged, with every selected compiler dependency fingerprinted.

Seven complete bodies recover 860 original bytes: the genuinely separate
local `std` helper, `__sfmoreglue`, `__sfp`, `__sinit`, `_fopen_r`, `fopen`
and `_cleanup_r`. The local helper has three actual incoming JALs and a whole
88-byte LOCAL STT_FUNC under both compilers; its source name is not claimed
as retail debug information. All seven entries have actual encoded incoming
references. The original boundaries, terminal delays, neighboring entries,
16 incoming J/JALs, 15 local branches and 60 stores were checked directly.
Their containing assembly intervals have full hash/byte proof, without
asserting complete enclosing caller semantics.

The glue walk preserves signed decrement/scan order, next publication before
failure, ENOMEM and the sparse free-slot initialization. Cookie and callbacks
are intentionally untouched until initialization/open succeeds. Reentrancy
initialization publishes its real cleanup callback before initializing three
embedded FILEs. Open preserves every failure gate, signed descriptor, captured
FILE return and append seek with a full 64-bit long argument. The successful
open's read-callback LUI is in the BGEZ delay; the failed path cannot be treated
as a linear callback materialization. Cleanup materializes the actual fflush
callback in the fwalk JAL delay. Previously reviewed callback paths are reused.

Author, disc peer and parent each read all 215 original instructions and the
complete unchanged sources. Both independent probes reproduced all fourteen
natural complete GNU links and the full identity packet. Two actual 45-word
ABI probes agree on FILE=88, glue=12, reent=748, int=4, long=8 and pointer=4,
including every used offset and flag. All seven bodies remain reconstructed:
zero links match exactly and none has unresolved relocations. Genuine compiler
symbol sizes are retained, including the GCC 2.9 cleanup tail call. No code
patching, symbol slicing, size coercion or generic OS substitute is used.
The source's additional `_cleanup` body is outside this seven-entry award and
is separately recovered in the following writer batch;
fseek is only an ABI/address binding. Native filesystem behavior, malformed
objects, EE exceptions and original data initialization are unclaimed.
The parent repeated all fourteen actual tracked-source links using only central
bindings. Complete code comparisons, symbol geometry and dependency hashes
reproduced the isolated proof. The canonical checkpoint passed all 264 tooling tests and reproduced the
retail ELF SHA-256 in a hybrid build.


## Buffered writers and output wrappers

The writer batch reuses unchanged public newlib 1.8.1 `fvwrite.c`, `fwrite.c`,
`fprintf.c`, `printf.c`, `fileno.c` and `fvwrite.h` at the same pinned revision
`b595ded606227e93b8c4a447446c1d2ac093827d`. The previously imported `findfp.c`
is reused byte for byte for `_cleanup`. Every source and compiler dependency
has a SHA-256 pin in the runtime manifest. Full notices remain intact:
`fvwrite.c`, `fwrite.c`, `fprintf.c`, `findfp.c` and `fvwrite.h` use the 1990
Berkeley notice in `LICENSES/newlib-1.8.1.txt` section 1. This product includes
software developed by the University of California, Berkeley and its
contributors. `printf.c` and `fileno.c` use the newlib default Cygnus Solutions
1994/1997 notice in section 9. This product includes software developed by
Cygnus Solutions. The complete upstream license is retained.

Seven complete bodies recover 1,468 original bytes: `__sfvwrite`, `fwrite`,
`fprintf`, `_printf_r`, `printf`, `fileno` and `_cleanup`. Author, disc peer and
parent read all 367 original instructions, the complete unchanged sources,
private IO and authentic reentrancy headers, and the three full binding bodies
for memchr, memmove and __swsetup. Two genuine 34-word compiler ABI probes
establish FILE=88, iov=8, uio=12 with a signed four-byte residual, long=8,
va_list=4 and all observed field offsets/flags. The original 100 incoming
J/JALs, 45 branches, 69 stores, three write callbacks and four preserved
central-pointer pairs have complete byte and geometry evidence.

The writer preserves zero-residual return, zero-length-vector skips, partial
writes, errors, string-buffer truncation with full-length accounting, fresh
residuals, line-buffer newline distance and callback publication order. The
wrappers preserve unsigned product/quotient behavior, actual variadic lanes,
stdout data publication before a fresh central-pointer reload, signed fileno
and initialization, and cleanup. `_printf_r` is a genuine independently
returned 76-byte original body and compiler STT_FUNC; no incoming encoded call
or aligned allocated pointer was found, so its caller reachability remains
unresolved. Formatter and __swsetup source identities are not imported.
The existing matching memmove assembly is reused through its numeric binding.

The parent and peer independently reproduced all fourteen natural whole GNU
links and complete identity packets. A tracked-source repetition using only
central bindings produced identical code comparisons, symbol geometry and
dependency fingerprints. Only GCC 3.2.3 `_cleanup` matches all 36 original bytes;
the other six routines remain reconstructed with their genuine size and code
differences. One isolated scheduling experiment yielded no further exact
match and is excluded from the registered baseline. Valid IO buffers and C
arithmetic domains are required; native OS/filesystem, malformed IO objects,
original data initialization and EE exceptions are unclaimed. The canonical checkpoint passed 264 tooling tests and reproduced the retail
ELF SHA-256 in a hybrid build.


## Unsigned long conversion

Unchanged pinned public newlib 1.8.1 `strtoul.c` recovers `_strtoul_r`
(528 bytes) and `strtoul` (52 bytes), with the full 1990 Berkeley notice
retained in source and `LICENSES/newlib-1.8.1.txt` section 1. This product
includes software developed by the University of California, Berkeley and
its contributors. Raw source SHA-256 is
`3561c247567ebcb5872ecfec7a52d5648f8fcedfb493e133a40a6f8824650356`.
The existing exact 257-byte licensed ctype table, GNU unsigned64 division,
remainder and multiplication bindings, and authentic limits/reent headers
are reused; no additional table/code award is claimed.

Author, disc peer and parent read all 145 original instructions, full source
and relevant authentic declarations/macros, plus both complete external
callers. They preserve signed-char classification, whitespace/sign/radix,
64-bit cutoff/accumulation and overflow return with errno34, low64 negation,
and captured endptr. The wrapper returns a full unsigned64 value; actual
callers perform their own signed32 narrowing afterward. Three real incoming
JALs, 24 branches, 17 stores, complete return/alignment geometry and preserved
ctype/impure pointer constructions are checked.

Parent and peer reproduced four natural complete GNU links and the entire
source/dependency/caller/16-word ABI/ctype identity packet. Long and unsigned
long are eight bytes, int/pointers four, char signed, and ULONG_MAX full64.
Both bodies remain reconstructed: all four links have zero unresolved
relocations and genuine visible code differences. Readable strings, valid
base and in-table ctype domains are required. Malformed radix/division traps,
high-bit out-of-table indexing, arbitrary aliases and native/EE exception
behavior are unclaimed. The parent repeated all four tracked-source links using only central
bindings; complete code, symbol geometry, dependencies and both ABI/ctype
proofs reproduced exactly. The canonical checkpoint passed 280 tooling tests (one platform-specific
skip) and reproduced the retail ELF SHA-256 in a hybrid build.


## Exit registration and va_list wrappers

Four more complete bodies reuse unchanged newlib 1.8.1 source at the same
pinned revision: `atexit` (156 bytes), `vprintf` (44 bytes), `vsprintf`
(84 bytes) and `atof` (28 bytes). The three exit/stdio files retain the
1990 Berkeley notices, including the original atexit sccs redistribution
placeholder; the full applicable notice is retained in
`LICENSES/newlib-1.8.1.txt` section 1. This product includes software
developed by the University of California, Berkeley and its contributors.
`atof.c` uses the default Cygnus Solutions 1994/1997 notice in section 9.
This product includes software developed by Cygnus Solutions. Every raw
source and actual compiler dependency has a SHA-256 pin in the manifest.

Author, disc peer and parent read all 78 selected original instructions,
complete sources and relevant authentic declarations, and all 42 instructions
of the actual engine allocator. They verified 259 encoded incoming transfers
and complete containing byte intervals, four branches, 19 stores and five
preserved reentrancy-pointer constructions. The atexit list retains its
embedded 32-entry node, signed index gate, allocation failure and fresh central
pointer reloads. A per-source `-Dmalloc=func_002AF140` binds its actual
136-byte engine allocation without changing the public source or central
generic malloc alias. The target allocator retains its size/pointer and
full return-register behavior.

`vprintf` forwards its incoming va_list and fresh stdout; `vsprintf` builds
the observed sparse 88-byte FILE and writes NUL through the post-formatter
FILE pointer. The complete 84-byte original and compiler STT_FUNC end before
the neighboring prologue. That neighbor's source identity remains unproved
and is excluded. The preceding 16 bytes before atexit have no terminal JR,
so no complete-body or noreturn assertion is made about them.

Both independent reviewers reproduced every proof dictionary, eight genuine
whole linked functions and two 24-word authentic ABI probes. The parent
repeated all eight tracked-source links using only central bindings; full
comparisons, dependencies, source fingerprints and ABI packets reproduced.
Int/pointers/size_t/va_list are four bytes, long/double eight, atexit-node
136 and reent 748. `atof` reuses the existing strtod binding and matches all
28 original bytes under both compilers. The other three bodies remain
reconstructed with zero unresolved relocations. Valid runtime objects,
strings, buffers and va_list domains are assumed; formatter/OS/heap/EE
implementation fidelity is outside this source-identity proof. The canonical checkpoint passed 280 tooling tests (one platform-specific
skip) and reproduced the retail ELF SHA-256 in a hybrid build.

## Double logarithms

Unchanged pinned Sun 1993 `e_log.c` and `e_log10.c` identify the complete
1,556-byte natural-log and 424-byte base-ten-log bodies. Both source files
retain Sun's permission notice. Their raw source hashes and every actual
compiler dependency are pinned in the runtime manifest. The existing fdlibm
header and software-double helpers are reused.

Author and parent independently read all 495 original instructions and both
complete sources. They verified normalization, subnormal scaling, the
close-to-one path, both seven-coefficient polynomial paths, compensated final
expressions, 64 real helper calls, 27 branches, two incoming JALs and fifteen
whole declared constant sections. Both genuine seven-word ABI probes confirm
64-bit doubles/longs and 32-bit ints/pointers. The external formatter's complete
interval is hash-validated; parent manual review covers its argument/result
window, without awarding formatter source recovery.

The retail negative-input path returns zero directly. An explicit genuine
`-ffast-math` recipe reproduces that folding from unchanged source; default
IEEE compilation differs there. This recipe does not establish general IEEE
accuracy or hardware behavior. All twelve complete object comparisons across
three recipes retain their actual relocations: the whole generated literal
pools, 136 and 32 bytes, have no contiguous retail match. Strict linking rejects
both functions. They are registered **reconstructed, `link:false`**, with no
code/data matching award. The advertised assembler `--construct-floats` option
leaves their generated code and data unchanged under both pinned compilers.

The parent reproduced the entire isolated identity packet and repeated all
four selected tracked-source objects using only central bindings. No source,
constant-pool splitting, data placement or instruction patch is used to force
a match. Native numerical accuracy and EE/FCR behavior remain unverified.

## Character search and uppercase strings

Three unchanged pinned newlib 1.8.1 sources recover `strpbrk` (124 bytes),
`strrchr` (76 bytes) and `strupr` (124 bytes). Their default Cygnus Solutions
1994/1997 permission notice is retained in `LICENSES/newlib-1.8.1.txt` section 9.
This software was developed at Cygnus Solutions. Source/dependency hashes and
the exact upstream revision are recorded in the runtime manifest.

Author and parent read all 81 original instructions, complete sources and the
eight-instruction published `toupper` helper. The search routines preserve
empty strings/accept sets, signed character truncation, first/last occurrence
and terminator searches. Uppercase conversion retains signed-byte ctype
indexing, the actual `toupper` call and the saved initial pointer. The existing
macro-only forced include selects function-call form without changing upstream
source. The complete previous ctype section, including seven alignment bytes,
matches retail under both compilers and receives no new data award.

Six incoming transfers and their complete byte intervals, twelve branches,
five stores and the actual `_ctype_+1` address construction are verified.
Both genuine thirteen-word ABI probes confirm signed char, 32-bit ints,
pointers and size_t, 64-bit long, and the observed classification bits.
Parent fresh packet reproduction and all six central-only tracked-source
whole links pass with zero unresolved relocations; none matches exactly.
Readable NUL-terminated strings and writable uppercase storage are required.
Portable ctype behavior is limited to its defined input domain; invalid signed
indices, malformed buffers and side-effecting replacement helpers are unclaimed.


## Multibyte decoding and stream output

Four complete functions reuse unchanged sources from the same pinned newlib
1.8.1 revision: `_mbtowc_r` (828 bytes), `_puts_r` (132), `puts` (40), and
`__srget` (72). The multibyte source and its `mbctype.h` retain the default
Cygnus Solutions notice in `LICENSES/newlib-1.8.1.txt` section 9. This software
was developed at Cygnus Solutions. The stream sources retain their 1990
Berkeley notices and section 1 terms. This software was developed by the
University of California, Berkeley. All four raw files and actual dependencies
have exact source SHA256 pins; no upstream source is edited.

Author, disc peer and parent read all 268 selected instructions and complete
sources. Full control, stores, real entry references, authentic declarations
and eight preserved code/data address constructions agree. The `puts` and
`__srget` boundaries were independently split from scanner intervals that
joined a following function; only their complete 40/72-byte source bodies count.
Six actual incoming calls have complete containing-body geometry and hashes;
large callers are reviewed at argument windows without a full caller semantic
claim. The parent also read the full 48-byte strlen and 360-byte refill helpers.

Both independent reviewers reproduce the complete ten-dictionary proof packet,
including scope, and two genuine 24-word ABI probes. Int/pointers/size_t/wchar_t
are four bytes, long eight, FILE 88, each enum four; both original 12-by-9 JIS
tables are 432 bytes. Ten explicit rows and two implicit zero rows are preserved.
Their entire writable sections match as identity evidence, with no mapping,
qualifier change or data award. All eight JIS actions, state/output writes,
locale selection and signed/unsigned character ranges are accounted for.

Six complete stream links have zero unresolved relocations and remain
reconstructed. The entire eight-byte newline section matches its original
readonly region. The multibyte function remains `link:false`: genuine
GCC 2.9/3.2.3 readonly sections are 64/56 bytes and retain all eight actual local
case pointers, while its compiled functions are 836/756 bytes versus 828 retail.
The strict relocation blocker and sixteen unrelocated code relocations remain
recorded; no final linked equality or partial pool mapping is claimed.

SJIS/EUC retain their original reads before null/length checks. Undefined null
or insufficient-readable-buffer paths receive no invented safety behavior.
The implicit-int strlen declaration under `-fno-builtin` differs from the
original unsigned length observation; accessible PS2 string lengths preserve
locale routing, while malformed/huge-string fidelity remains unclaimed.
Readable strings and valid writable wchar/state/FILE/reent objects are required.
The parent repeated all six tracked-source links, both honestly blocked main
objects and six complete comparison/dependency/source/ABI/data/binding packets
using only central symbols. Native OS, encoding safety, thread synchronization
and EE exception fidelity are outside this source-identity proof.

Canonical checkpoint verification and the hybrid build passed, together with
307 tooling tests (one platform-specific symlink skip). The hybrid executable
retains retail SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`;
remaining original assembly contributes no additional C matching credit.

## GNU frame sorting and state decoding

[The frame runtime evidence](frame_runtime.md) covers fifteen complete
functions / 3,988 bytes from unchanged historical GNU source. Nine independently reproduced
whole linked functions match all 2,380 bytes in canonical verification.
Four BSS users resolve through separately reviewed whole zero-storage proofs;
the decoder's complete
208-byte pool remains nonmatching. The five exact Git blobs, individual GPL
notices and historical frame linking exception are retained. The proof uses
actual target/header dependencies and 31-word ABI probes under four transparent
recipes, with no native unwinder or hardware claim.

The frame checkpoint passed canonical verification, the retail-identical hybrid
build and 321 tooling tests (one platform-specific symlink skip).

## GNU exception handling

[Exception runtime evidence](exception_runtime.md) covers fourteen complete
functions /2852 bytes from the unchanged published L_eh source and pinned
eh-common/frame/thread headers. Eight complete independently reproduced natural
linked matches total 1,780 bytes, confirmed by canonical verification. Original
callback/context roots and31-word ABI probes preserve the historical compiler
primitives. Static context links through existing complete32-byte zero-storage
proof rules and remains nonmatching. The full1528-byte original writable EH
initializer differs from the complete1520-byte candidate and receives no mapping
or credit. GNU notices and the exact libgcc2 exception remain unchanged.

The exception checkpoint passed the retail-identical hybrid build and 331 tooling
tests (one platform-specific symlink skip).

## GNU C++ RTTI and exceptions

[GNU C++ runtime evidence](cxx_runtime.md) covers twelve complete functions /2,612 bytes from unchanged historical tinfo, tinfo2 and exception sources plus four private headers. Eleven complete natural linked functions match all1,388 bytes in canonical verification. The larger type matcher remains reconstructed. Nine exact Git blobs, original copyright/license notices and the exact GNU source linking exception are retained. Target ABI objects, original callback publications and central-only reproduction support the recovery; generated data receives no new mapping or award.

The C++ checkpoint passed canonical verification, the retail-identical hybrid build and364 tooling tests (one platform-specific symlink skip). C and C++ counts are separate in the report, with combined exact source progress; assembly remains separate.

## Segment distance and further GNU C++ source reuse

The [complete segment-distance routine](segment_distance.md) adapts Coin3D's
pinned, BSD-3-Clause nine-region and parallel algorithm. Its full copyright,
conditions and disclaimer are retained in the production source and
`LICENSES/coin3d-segment-distance.txt`. The actual origin/displacement ABI,
operation grouping, soft-double absolute sequence, signed zero and output order
are backed by the complete retail instructions. This establishes reusable public
algorithm correspondence without an original SDK or compiler identity claim.
The 1,932-byte routine is reconstructed; three genuine whole links differ.

The [GNU C++ follow-up](cxx_runtime.md#additional-unchanged-source-reuse) identifies
another 12 complete entries/864 bytes from the same unchanged licensed sources.
Five link naturally under the default historical recipe; seven generated getter
COMMON objects retain genuine linker blockers. All 12 remain reconstructed and
add no exact bytes, data mappings or source imports.

## Further collision and unchanged signed division

[Segment/triangle intersections](segment_intersection.md), [record collision queries](record_collision.md) and [vector transforms](vector_transform.md) recover six complete functions /2,764 original bytes with existing real bounds, matrix, normalization and licensed distance/runtime sources. Each batch retains its explicitly stated alias, finite arithmetic, helper and caller-context limits; no original library/class identity is inferred.

[Signed division](gnu_signed_division.md) adds one complete 1,772-byte routine from the already pinned, unchanged GNU libgcc2 source and18 dependencies. A genuine GCC2.9 whole function/table link passes; a newer outlined local helper remains honestly unbound. All seven entries are reconstructed, with no new exact matching bytes. Independent full selected/source/proof review, private whole reproduction, central-only comparisons, canonical verification and the retail-identical hybrid build passed. The integrated reviewed tooling suite contains402 tests (one platform-specific symlink skip).

## Plane intersections, spatial queries and further runtime reuse

[Plane intersections](plane_intersection.md) adapt two complete Coin3D-compatible methods with the full pinned BSD notice retained in source and `LICENSES/coin3d-plane-intersection.txt`; game-specific thresholds, raw direction, separate divisions and fresh aliased output reads remain explicit. [Spatial queries](spatial_queries.md) reuse published bounds and point transforms in four full original bodies. All six entries remain reconstructed.

[GNU lifecycle](gnu_lifecycle.md) reuses four entries from existing unchanged libgcc2 source and one additional exact raw header; only the complete48-byte `exit` matches. [Random state](random_runtime.md) adds the unchanged Cygnus newlib `rand.c`, with both complete functions matching64 bytes. This software was developed at Cygnus Solutions. The exact defaultclause9 distribution notice is retained; an initial private version/clause citation is explicitly corrected. Prior source/provenance records and matching gates remain intact.

Independent parent/peer full selected/source review, private complete-link reproduction and central-only comparisons passed. Canonical verification now reports327 exact C/C++ functions /16,540 bytes. The reviewed tooling suite passes419 tests (one platform-specific symlink skip). The hybrid build is byte-identical to the supplied retail ELF.

## Recursive collision, six-face construction and unchanged memory wrappers

[Recursive collision](actor_collision.md) recovers the complete7,220-byte node construction/publication pair with real previously published geometry, pool, angle and query sources. Independent review corrected full32 route-key reads before registration; twelve new upper-half fixtures reject the archived initial C. [Six-face construction](geometry_frustum.md) recovers another complete1,832-byte constructor with the real existing normalizer. All three game bodies remain reconstructed. Their native runners pass594,275 and112,158 checks; bounded models retain finite arithmetic, controlled engine contracts and the original unwritten query-mode limitation.

[Memory wrappers](memory_wrappers.md) import unchanged pinned newlib1.8.1 `bcopy.c` and `index.c`; both complete GCC3.2.3 functions match64 original bytes. Separate source/compiler-header provenance and the complete defaultclause9 notice are retained. This software was developed at Cygnus Solutions. Canonical source filename changes affect only nonallocated debug data; actual object hashes and all complete code/relocation comparisons are checked separately.

Independent author/parent/peer review, isolated whole-function and mixed-ABI reproduction, and canonical-only comparisons passed. Canonical verification reports329 exact C/C++ functions /16,604 bytes, with1,389 recovered functions covering347,444 original bytes. The reviewed tooling suite passes437 tests (one platform-specific symlink skip). No supporting helper, retained original data, alignment padding or assembly contributes new C/C++ credit.

The checkpoint44 hybrid build completed successfully and reproduced retail ELF SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8` byte for byte. Its reused assembly remains excluded from source progress.

## Road queries, actor routes and allocator wrappers

[Road queries](road_queries.md), [actor route consumers](actor_route.md) and [route setup](route_setup.md) recover eight complete game functions /3,536 original bytes. They reuse published provider, bounds, registry, vector and goal helper source, unchanged licensed Sun/newlib `ceilf` and GNU `fp_bit`, and the existing EE square-root primitive. Native goal helper extraction preserves complete original published function and macro spans, with raw hashes and actual dependencies; it does not claim that entire goal translation units are compiled. Controlled supporting getter/setup bridges remain limited to the actor packet, while setup receives a separate source award.

[Allocator wrappers](allocator_wrappers.md) adapt pinned newlib1.8.1 `malign.c` and `realloc.c`: two complete96-byte bodies. The original Cygnus notice, dated modifications and baseline hashes remain explicit. This software was developed at Cygnus Solutions. Their three fresh reentrancy-pointer reads, captured result and observed lock/unlock calls differ from the upstream single-call baseline; unchanged-source identity and original retail source origin are not claimed.

Independent complete parent and peer review, frozen private replay, actual compiler ABIs and26 complete canonical-only links passed. All ten functions remain reconstructed. Canonical source totals are1,399 functions /351,172 original bytes, with329 exact functions /16,604 bytes unchanged. The reviewed tooling suite passes472 tests (one platform-specific symlink skip). Original helpers, data, padding and assembly receive no duplicate C/C++ award.

The checkpoint45 hybrid build passed and reproduced the supplied retail ELF SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8` byte for byte. This retains unresolved original code/data and is not a complete source build.

## Allocator hooks, stream closure and route initialization

[Allocator locking](allocator_locks.md) reconstructs two complete hooks /160 original bytes. The pinned generic Newlib hooks and surveyed public PS2SDK implementation differ from the executable and were excluded. Three syscall interfaces and three global words are proved numeric bindings, with no kernel or data award. Fresh count loads and ordered semaphore callbacks pass 80,640 native checks and a stale-count negative control.

[Stream closure](stdio_close.md) adapts the complete pinned Newlib `fclose` source by removing exactly six lines in three cleanup clauses, retaining its Berkeley notice and dated modification. Reverse reconstruction restores the pinned Git blob byte for byte. A separate numeric forward-copy helper preserves overlap propagation and a caller-compatible inferred void convention. The two bodies cover228 original bytes, pass1,504,979 native checks with unchanged published stdio sources, and reject restored cleanup and cached-callback mutations. Supporting runtime bodies receive no duplicate award.

[Route initialization](actor_route_init.md) recovers three complete game bodies /884 bytes using published provider, bounds, plane, movement, setup and queue source. Exact complete helper function/macro spans are reused without claiming entire unrelated translation units. Its 8,122,867 native checks retain the stated finite arithmetic, simple movement and controlled callback limits. The initializer's void declaration is an inferred effects convention: the complete `001DB5F8` caller propagates incidental GPR2 after its call, so no universal original return declaration is proved.

Complete parent and independent peer review, unchanged frozen inputs, actual target layouts/dependencies and17 whole canonical-only comparisons passed. All seven functions remain reconstructed. Canonical verification reports1,406 recovered C/C++ functions covering352,444 original bytes;329 exact functions /16,604 bytes are unchanged. All500 tooling tests pass (one platform-specific symlink skip). The checkpoint46 hybrid build reproduces retail ELF SHA-256 `01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8` byte for byte, retaining unresolved original code/data. This is not a complete source build.

## Buffer and history records

[Buffer records](buffer_records.md) recover six complete game routines /356 original bytes. Backward/forward history navigation, deletion, indexed lookup, current-data lookup and copying retain signed gates, wrapping byte offsets and fresh data reloads after copy. Existing complete length source is reused unchanged. The pinned licensed generic Newlib strncpy body is native support only, with nonoverlap, valid terminated buffers and byte-path domains; its original aligned bulk paths receive no new source award.

Parent and independent peer replay preserved all266 frozen inputs and reproduced thirty complete selected/ABI links. Canonical-only comparisons confirm two complete functions /52 bytes exactly, with the other four reconstructed. The original observer covers548 fixtures /66,420 instructions, and native source passes285,633 warning-free checks plus a stale-pointer negative control. The reviewed tooling suite passes509 tests (one platform-specific symlink skip). Source totals are1,412 functions /352,800 original bytes;331 exact functions cover16,656 bytes.

Buffer manager recovery adds three complete game functions /912 original bytes, reusing whole published accessor, strstr and ctype translation units and the exact complete published length function. Generic licensed Newlib strncpy supports native validation only; the indexed-array helper and callbacks remain bounded observers with no duplicate source award. Parent and independent peer replay preserved216 frozen inputs and verified seventeen whole selected/ABI links. Native checks pass2,233,020 times over867 fixtures; only the complete16-byte flag getter matches under GCC2.9. Source totals after checkpoint48 are1,415 functions /353,712 original bytes, with332 exact functions /16,672 bytes.

Array records add nine complete game functions /868 original bytes. Native validation reuses exact complete published allocation/free macros, the full count-accessor TU and unchanged licensed qsort, with pinned generic memcpy as supporting native C only. Parent and independent peer replay preserve664 frozen inputs and reproduce forty-five whole selected/ABI links. All217 selected instructions execute across449 fixtures /27,409 instructions; native source passes464,677 checks and a cached-count negative control. Only the complete eight-byte reset matches under all three recipes. Source totals after checkpoint49 are1,424 functions /354,580 original bytes, with333 exact functions /16,680 bytes. Existing getter declarations have pointer ABI compatibility only; typed integration, complete heap behavior and whole original qsort callback equivalence remain outside proof.

UI completion, keyboard submission and drawing add three complete game functions /2,408 original bytes. Parent and independent peer reproduce16 whole dictionaries, five RAW objects and17 complete selected/ABI linked ELFs while preserving269 frozen and160 historical/qualification inputs. Genuine published buffer, Deimos, CRC, map, string and GNU conversion closures support1,053,921 warning-free native checks and three meaningful negative controls. Twenty-five actual compiler dependency closures contain49 unique inputs. The complete array getter is retained with a typed native adapter; these fixtures use an empty command array and do not execute it. All three selected functions remain reconstructed, with no supporting helper/data award. Source totals after checkpoint50 are1,427 functions /356,988 original bytes, with333 exact functions /16,680 bytes.

The string registry adds four complete startup-connected game functions /1,456 original bytes. Exact whole published heap/length spans, unchanged licensed string sources and genuine ctype/lowercase/search TUs support1,194,161 native checks. Parent and peer reproduce17 complete dictionaries, five objects and20 whole selected/ABI ELFs while preserving345 frozen and97 historical inputs. Separate canonical-only ABI supplements preserve the original limited-map producer and reproduce all eight authentic caller ELFs; all12 selected whole-function comparisons remain nonexact. An explicit qualification records that caller2AC0B0 forwards the registration result. No helper/data/source-identity award is added. Totals after checkpoint51:1,431 source functions /358,444 bytes;333 exact functions /16,680 bytes.

File operations adds ten complete engine functions /1,540 original bytes. Published registry, CRC/length, heap/map/accessor spans/macros and unchanged licensed string/ctype support are reused for1,384,882 native checks. This software was developed at Cygnus Solutions; the complete defaultclause9 notice remains in LICENSES/newlib-1.8.1.txt. Parent and peer preserve453 frozen and161 historical inputs, reproduce five objects/fifty whole selected/ABI ELFs, and independently relink all fifty through canonical defaults. The parent compares18 whole dictionaries, peer13. All30 natural selected comparisons remain nonexact. Original SDK interfaces/syscall trampolines support numeric contracts; SDK/core/DMA effects and the explicit native archive-order bridge remain controlled, with no helper/data award. Totals after checkpoint52:1,441 source functions /359,984 bytes;333 exact functions /16,680 bytes.

Archive close and lookup add two complete engine functions /184 original bytes. Native checks reuse the complete file-close and registry/CRC/length source spans, whole heap/map macro definitions and invocations, and unchanged licensed string support. This software was developed at Cygnus Solutions; the complete defaultclause9 notice remains in LICENSES/newlib-1.8.1.txt. Parent and peer each reproduce19 whole dictionaries, five objects and ten whole selected/ABI ELFs, preserving348 frozen and105 historical inputs before the archived documentation qualification. All ten fresh canonical-default relinks agree; all six selected compiler candidates remain nonexact. The500 fixtures execute223,192 original instructions and1,222,668 native checks. SDK/core effects remain controlled and no helper/data award is added. Totals after checkpoint53:1,443 source functions /360,168 bytes;333 exact functions /16,680 bytes.

Completion visitor and merge add two complete engine functions /584 original bytes. The native closure reuses complete published length/walker/adapter spans, whole heap/upper-bound/map macro invocations, complete manager append and unchanged licensed Newlib sprintf/atexit/byte-string source. This software was developed at Cygnus Solutions; the complete defaultclause9 notice remains in LICENSES/newlib-1.8.1.txt. Parent and peer each reproduce14 whole dictionaries, five target objects and ten whole selected/ABI ELFs while preserving380 authored and93 historical inputs before the sole archived doc qualification. All ten fresh canonical-default relinks agree; all six selected compiler candidates remain nonexact. Twenty actual-M closures retain74 files. Controlled nested constructor/insertion/core/vfprintf contracts and initialized ASCII/alias/one-past token domains remain explicit, with no helper/data credit. Totals after checkpoint54:1,445 source functions /360,752 bytes;333 exact functions /16,680 bytes.

Archive lifecycle adds eight complete game functions /2,344 original bytes. Whole published heap/map/CRC/length source and unchanged licensed Newlib/ctype support are reused. Parent and independent peer preserve510 RAW inputs and reproduce15+4 entire dictionaries, five objects and40 full selected/ABI ELFs. The parent independently relinks all40 through canonical defaults; all24 natural selected comparisons remain nonexact. Both18-word layouts,21 actual dependency closures/44 files,21,143 finite pointer tokens and the original instruction-cap failure are checked. Native checks pass3,227,776 times over382 fixtures /902,603 original instructions; two actual order/depth mutations fail. SDK/core/module/scheduler effects and the explicit heap-create bridge remain bounded controls, and optimized string helpers receive no source/data award. Totals after checkpoint55:1,453 source functions /363,096 bytes;333 exact functions /16,680 bytes.

Checkpoint56 adds the complete708-byte SGI pointer-vector insertion and16-byte unsigned comparator. The genuine template,18 complete primary headers/notices and generated configuration are reused without active algorithm edits; the comparator matches all16 original bytes under GCC2.9. Root and independent peer preserve1,313 RAW inputs and reproduce six target objects/four whole target ELFs/five native objects/golden/trace exactly, with12 whole evidence dictionaries subject only to disclosed output paths, test times and separately validated native PE metadata. The peer relinks all four canonically.500 fixtures execute206,144 original instructions; native checks pass2,065,912 times. GNU pointer/null-subtraction, separate initialized object/storage, native long32 versus target long64, controlled allocation and unexecuted null-handler stream limits remain explicit. No helper/data/58duplicate comparator credit. The ignored timestamp-only prose is explicitly corrected by retained erratum; actual native proof validates timestamp and checksum, without target relaxation. Totals:1,455 source functions/363,820 original bytes;334 exact/16,696 bytes.

Checkpoint57 adds complete unchanged generic newlib memchr,memset,strncat,strncmp,strncpy: five functions/1,768 original bytes. Exact LF Git blobs/default Cygnus1994/1997 clause9 are retained; optimized R5900 assembly is supporting evidence only. Both default compilers yield ten whole selected nonmatches plus ten complete ABI caller links. Parent and independent peer preserve960 RAW inputs and reproduce18 whole dictionaries with precise output/time/native-PE qualifications, twelve target objects/twenty whole target ELFs/nine native objects/trace/golden exactly.9,545 fixtures execute2,763,636 original instructions; native checks pass14,680,210 times, nine guards and two real mutations. Both17-word target ABIs prove long64/limits; native long32 GNU wrap/alias contract and readable padded disjoint objects remain qualified.441/442 instructions execute; the sole unreachable NOP is retained in full comparisons.21 actual dependency closures/33 inputs and exact manufacturer-approved local PXOR are checked. No helper/data/assembly or exact original-C/compiler identity credit. Totals:1,460 source functions/365,588 original bytes;334 exact/16,696 bytes.

Checkpoint58 adds six numeric resolver/lifetime routines and two complete genuine SGI container methods: eight functions / 2,172 original bytes, all nonmatching. Complete primary headers and notices remain unchanged; standard source provenance pins the owned C++ carrier separately from the upstream algorithms. Parent and independent peer preserve 935 RAW inputs, replay 16 complete qualified evidence dictionaries, and verify 20 natural whole selected links plus 18 whole ABI callers. Both 16-word target layouts, 15 actual dependency closures / 100 files, 493 fixtures / 148,075 original instructions and 8,094,485 native checks pass. Nine guards and two actual failing mutations pass. The 156-byte clear remains excluded because its genuine C++ candidate calls an unproved helper. GNU/native object-prefix alias contracts, initialized prior bytes, controlled engine operations, unexecuted historical warning paths and original compiler/class identity limits remain explicit. The initial replay isolation failure and unavailable old native PE remain documented; refreshed evidence never claims those unavailable bytes preserved. Totals: 1,468 source functions / 367,760 original bytes; 334 exact / 16,696 bytes.

Checkpoint59 reuses complete unchanged licensed newlib strcat and strchr: two functions / 720 original bytes, zero new exact matches. Primary LF Git blobs and default Cygnus notice are retained. Both default compilers link four selected nonmatches and four actual two-GPR ABI callers; both17-word target layouts prove long64 and consistent limits. Parent and independent peer reproduce13 entire qualified dictionaries with404 RAW inputs preserved before an archived documentation update. Six target objects/eight whole target ELFs and seven native objects, golden and trace remain strict RAW.13 actual-M closures/31 files,11670 fixtures/2649980 original instructions/17936790 native checks pass; nine guards and two real mutations pass. Entire original strcpy executes and genuine native bytewise strcpy supports comparisons without recovery credit. Native long32 versus target long64/quadword schedule and initialized padded disjoint GNU domains are explicit. Existing authored provenance is preserved; standard chosen-M metadata is supplied separately. Totals:1470 source functions/368480 original bytes;334 exact/16696 bytes.

Checkpoint60 adds three complete timer routines and one licensed SGI hashtable clear C adaptation:4 functions/1132 original bytes, zero exact. Two selected C sources and all numeric headers preserve initial bytes. Genuine SGI vector/allocator, published heap/upper-bound/comparator, unchanged Newlib exit/memmove support and the immutable scalar decoder are reused without helper credit. Timer509 fixtures/125730 original instructions/3169166 native checks, nine guards present/absent and three real wrong-capture failures pass; clear435/58155/50496, nine guards6+3absence and fixture27 wrong-order failure pass. Root/distinct peers reproduce14+3 timer and11 clear dictionaries;37 and18 strict RAW artifacts. Timer489/clear370 preserved before two archived publication document annotations; no full native PE RAW equality claim. All selected default functions are nonmatching, whole caller/layout/dependency proofs pass. Totals:1474 source functions/369612 original bytes;334 exact/16696 bytes.

Checkpoint61 adds4 C packet constructors/encoders and1 genuine HP/SGI fill_n C++ instantiation:5 functions/996 original bytes, zero exact. Complete previously published SGI/Newlib/heap/string/decoder support reused. All1273 fixtures/1214059 original instructions/5252952 native checks and two actual ordering mutations pass; all9 natural selected candidates are nonmatching. Root/distinct peer reproduce15 whole qualified dictionaries,21 ETEXECs,8 target and24 native objects. Author379 bytes preserved before one archived publication document annotation; genuine GNU32 null/alias behavior and corrected private path isolation are explicit. Totals1479 source functions/370608 original bytes;334 exact/16696 bytes.

Checkpoint62 registers3 integer timer routines/880B, migrates2 complete string routines/612B from existing reconstructed assembly rows to unchanged licensed generic C, and reuses1 genuine GNU C++ before method/36B matching every original byte. Initial production sources/headers/shared decoders remain unchanged. Timer433 fixtures/51623 original instructions/2685743 native checks; strings42217/8432793/64887529; before3872/427122/111203 pass with9 guards per batch and6 actual negative controls. Parent/distinct peers preserve328,1001(including961 defaults),122 frozen inventories before3 archived documentation annotations; qualified full native PE metadata and strict whole target/object/source comparisons retained. Source totals1485 functions/372136B;335 exact/16732B;2 migration rows add no primary entries or assembly credit. GNU native domains and provisional unrooted timer/before reachability remain explicit.

Checkpoint63 reusesfive whole licensed hash-table clear traversals/780B andtwo unchanged genuine GNU exception queries/88B. Allseven complete sources passed parent and distinct peer review; queries match all88 original bytes and clears remain reconstructed152-vs156. Clear2910 fixtures/369630 original instructions/361770 native checks; GNU1568/32928/80577; nine guards per batch andnine actual wrong-source controls pass.609 and450 RAW inventories were preserved before separately archived final documents, with strict whole target/native object evidence and read-only ImageHlp-qualified PE metadata. Source1492 functions/373004B;337 exact/16820B. Unknown clone payload/class/allocation and unrooted exception invocation remain explicit; no supporting helper/data or old-fixture award.

Checkpoint64 adds19 reviewed source functions/9252 original bytes:3 licensed MPREC copies/428B match exactly;14 unchanged genuine SGI container copies/7976B and2 numeric float timer routines/848B remain reconstructed. Parent and distinct peers reproduce1597/1732/485 immutable input inventories before two separately archived public document annotations. SGI2232 fixtures/605836 instructions/36640209 checks; timer395/60382/2450300, with explicit prefix/storage and bounded exact-normal arithmetic exclusions. Checked public compiled-symbol reuse preserves global seed/external bindings and all byte/link/provenance gates. Source1511/382256B,340 exact/17248B; no new supporting helper/data or inherited fixture award.

Checkpoint65 adds35 reviewed C functions/5260 original bytes:24 descriptor destructors/3072B and5 pooled-release callbacks/420B reuse complete existing source and remain reconstructed; six unchanged genuine GNU libio helpers/1768B match exactly with GCC2.9. Parent and distinct complete replay preserve2642/2534/1870 input inventories before three separately archived public document annotations. Descriptor2688 fixtures/662496 instructions/44143116 checks; pooled10080/190080/286569; GNU562/59291/1153190 plus1138 independent integer/packed-bit checks. Exact ordinary canonical header lookup preserves genuine source and four complete objects; six defined partial-wrapping stress fixtures remain qualified. Source1546/387516B,346 exact/19016B; no inherited test/helper/data/deferred award.

Checkpoint66 reuses the complete licensed SGI upper-bound implementation for59 existing C functions/9204B through one C++ instantiation and58 strict source-symbol copies; no new function/byte or matching credit for that migration. Genuine unchanged GNU libio Balloc/Bfree/Bcopy add3 complete matching C functions/200B, with source-local allocator bindings and full measured ABI. SGI24957 executions/24898 unique full parameters (423cases/422unique per entry),1687046 instructions/2936784 native checks; allocator412/26282/844654. All final immutable2185/2186 inputs were unchanged before two separately archived public document annotations; whole parent and distinct replays passed. Source1549/387716B,349 exact/19216B; no inherited test/helper/data/deferred award.

Checkpoint67 reuses unchanged genuine GNU libio s2b/mult/ulp/ratio/quorem, adding5 complete matching C functions/1760B. Complete original bytes, genuine source/licenses/cache/M, measured typed ABI, strict actual-VMA target links and final parent/distinct contained replays establish matching-only qualification; no new selected emulator/native/model/coverage/hardware claim. Entire195/1970 immutable inventories remain preserved (the separately annotated195 author document is archived in full). Source1554/389476B;354 exact/20976B. All old report rows, runtime catalog/symbols, source and existing binding values remain unchanged; only ten measured selected numeric/static names are added.
