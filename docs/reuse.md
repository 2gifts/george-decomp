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
The GNU floating-point runtime batch below adds seventeen further verified C
functions from unchanged upstream source.

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
`__errno`, `_localeconv_r`, `sinf`, and `tanf`, plus the 2,564 bytes of the GNU
floating-point runtime batch below, total 3,068 bytes from 24 verified C functions.
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

These exact runtime matches establish reusable implementations of ten specific
functions. They do not identify the compiler used for Papaya's game code, prove
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

Seventeen directly referenced runtime functions now reproduce 2,564 complete
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
| `__make_dp` | `0x003734C8` | 44 | Double |
| `dptofp` | `0x003734F8` | 84 | Double |
| `__pack_f` | `0x00373CA8` | 268 | Single |
| `__unpack_f` | `0x00373DB8` | 144 | Single |
| `__fpcmp_parts_f` | `0x00374498` | 276 | Single |
| `fptoui` | `0x00374748` | 152 | Single |
| `__make_fp` | `0x00374818` | 44 | Single |
| `fptodp` | `0x00374848` | 64 | Single |

Every entry has real original decoded direct `JAL` references. Each unchanged
source symbol is compiled with the pinned `gcc29` profile, independently sized
by its ELF `STT_FUNC` symbol, and genuinely linked at its original address with
the established runtime bindings. The complete resulting bytes, including all
relocated calls and the return delay, match the retail SHA256. No relocation
mask, instruction rewrite, or data replacement is used for awarding matches.
These runtime matches identify a compatible recipe for these functions;
they do not identify the compiler used for every game translation unit.

The preliminary disassembly heuristic merges some adjacent library functions.
For example, its 216-byte region at `0x003733F0` contains the complete 160-byte
`dptoul` followed by a separate 56-byte negate helper. Only `dptoul` is currently
registered there. `__fpcmp_parts_f` occupies 276 bytes of the heuristic's larger
region at `0x00374498`. The manifest records the independently compiled
complete function extents, decoded call entry proofs, original full hashes,
and disjoint bounds; heuristic file length is not used as a function size.

Both double arithmetic wrappers call the same file-local parts adder at
`0x00372A28`. Their per-function binding records that address and its proven
position between the exact unpack and pack calls. That helper remains
uncounted. The whole imported source also compiles other routines; their mere
presence in an object does not award recovery or matching progress. Seven
further exact symbol-sized candidates without direct call references remain
outside the manifest pending independent boundary and entry review. The
current batch needs no mapped constant sections or writable-data exception.

Root independently checked all seventeen original complete-body hashes,
terminal returns and delay extents, every recorded direct-call entry, the
unchanged source hash and complete linked comparisons. The full verifier
reproduced all seventeen matches, and the hybrid build retained the retail
executable SHA256 after their substitution.
