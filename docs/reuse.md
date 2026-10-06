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

The seven verified sources checked into this repository were independently compiled or
assembled, and their function bytes compared with the retail executable again:

| Symbol | Retail address | Bytes | Reused source | Source form | Verification |
| --- | --- | ---: | --- | --- | --- |
| `memcmp` | `0x00393464` | 148 | `src/runtime/memcmp.S` | Upstream assembly | Exact |
| `memcpy` | `0x003934f8` | 172 | `src/runtime/memcpy.S` | Upstream assembly | Exact |
| `memmove` | `0x003935a4` | 252 | `src/runtime/memmove.S` | Upstream assembly | Exact |
| `fabsf` | `0x0037dd58` | 28 | `src/runtime/fabsf.c` | Upstream C | Exact |
| `atoi` | `0x00396320` | 40 | `src/runtime/atoi.c` | Upstream C | Exact after linking `strtol` |
| `matherr` | `0x0037db30` | 36 | `src/runtime/matherr.c` | Upstream C | Exact under GCC 2.9 after linking `dpcmp` |
| `__errno` | `0x00393368` | 12 | `src/runtime/errno.c` | Upstream C | Exact after linking `_impure_ptr` |

The 572 bytes of reused assembly must remain separate from high-level C
decompilation progress. The 116 bytes of compiled `fabsf`, `atoi`, `matherr`, and
`__errno` C are high-level source.
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

These exact runtime matches establish reusable implementations of seven specific
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
