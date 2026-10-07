# Licensed generic memory and string routines

Five whole unchanged newlib C files reconstruct the bounded ordinary-memory
behavior of these complete original functions. Their ten default target
compilations naturally link, but none matches the entire original function.
The exact original C source and compiler are not identified.

| Function | Original address | Original bytes | GCC 2.9 candidate bytes | GCC 3.2.3 candidate bytes |
|---|---|---:|---:|---:|
| `memchr` | `00393378` | 236 | 276 | 268 |
| `memset` | `003936A0` | 184 | 252 | 220 |
| `strncat` | `00393C90` | 436 | 332 | 240 |
| `strncmp` | `00393E48` | 456 | 264 | 332 |
| `strncpy` | `00394010` | 456 | 276 | 248 |

The original total is 1,768 bytes / 442 instructions. `003936CC` is an
interior `PCPYH` instruction, not the entry of `memset`. Four zero padding
bytes following the actual `strncat` end are excluded. Registered length
helper `00295050` and the separately scoped `strcat`/`strchr` are excluded.
No supporting library code, data, or historic assembly probe earns additional
source or matching credit.

## Source and license

The complete files in `src/runtime/{memchr,memset,strncat,strncmp,strncpy}.c`
are exact LF Git blobs from
[SSXModding/ps2-ee-toolchain](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string),
revision `b595ded606227e93b8c4a447446c1d2ac093827d`. All five files retain
their entire comments, documented prototypes, byte paths and optimized long
paths. None has a per-file copyright override. Distribution license
`LICENSES/newlib-1.8.1.txt`, clause 9, supplies the Cygnus Solutions
1994/1997 notice and permissions. This software was developed at Cygnus
Solutions.

Complete pinned R5900 assembly sources were separately reviewed as optimized
algorithm/access evidence. They retain their distinct Jeff Johnston/Cygnus
1999 notice and are not imported as reconstructed C. Twenty existing failed
assembly match probes are preserved, rather than rerun as a flag search.
Generic C correspondence over the stated domain does not prove the retail
compiler consumed these exact generic files.

The immutable source-free proposal contains 466 actual encoded references
across all five executable sections, 373 whole containing-interval identities,
all allocated-pointer/address-construction scans, 384 original geometries,
and exact source/header/license blobs. Bounded argument/result windows were
reviewed for all references; no claim is made to have manually interpreted
every large caller or established every path from startup.

## Actual compiler and memory contracts

Both genuine target ABI objects establish signed plain `char`, four-byte
pointers and `size_t`, eight-byte `long`, and consistent `LONG_MIN`,
`LONG_MAX`, `ULONG_MAX` and long alignment. All ten complete forwarding
functions retain GPR 4, 5 and 6 and the return register while actually calling
the canonical selected address. Target recipes use the existing defaults:
`-O2 -G0 -mfp64 -fno-builtin -fno-common -ffunction-sections`.
GCC 2.9 additionally needs the already published whole `src/runtime/include`
limits header. The original missing-header diagnostic is retained; no source
or header repair and no `-mlong64` change was needed. Twelve successful actual
target `-M` closures and nine native closures pin 33 distinct inputs.

Native tests compile all five entire optimized sources as separate TUs under
the genuine 32-bit GNU compiler, whose `long` is four bytes. The explicit
native contract is `-fno-strict-aliasing -fwrapv`, not universal ISO C. In
particular, optimized `strncpy` performs signed subtraction in `DETECTNULL`;
155 native fixtures explicitly require wrapping arithmetic and 9,390 avoid
that subtraction overflow. The latter classification does not make arbitrary
character arrays valid ISO-C long aliasing objects. The target has 136
overflow-word fixtures; replay of both entire naturally linked `strncpy`
candidates demonstrates the measured 64-bit `DADDU` wrapping behavior under
those compilers, not a general language guarantee.

Inputs are initialized, padded, ordinary readable memory with representable
pointer/count arithmetic. Mutating string functions use disjoint source and
destination objects with sufficient capacity. Readable padding is required
for aligned block scans past a NUL; retail `strncmp` may load the next left
16-byte block before checking whether fewer than 16 requested bytes remain.
Retail `strncpy` has a conservative initial detector that also flags some
nonzero bytes and then falls back to its byte path; it still detects every
NUL. `strncat` scans its destination even for a zero append count. Full buffer
canaries and zero-count behavior are checked. No equivalence is asserted for
uninitialized storage, overlap, invalid objects, page faults, volatile/MMIO,
concurrent observation, timing, hardware flags or allocator behavior.

## Verification and observer limits

The bounded original decoder executes 9,545 synthetic fixtures comprising
2,763,636 actual original instructions, with a maximum of 2,566 per call.
It visits 441 of the 442 selected instructions. The sole unexecuted word is
the unreachable NOP at `00393FD4`, after the unconditional branch at
`00393FCC` and its `00393FD0` delay instruction; complete function comparison
still includes that word. Independent byte algorithms cross-check each
original result and all 1,536 owned buffer bytes. The native production TUs
pass 14,680,210 checks against the resulting synthetic fixtures.

Nine focused guards check owned/initialized memory, alignment, code/control
bounds, real JR31 returns, delay encodings, the instruction budget, arithmetic
classification and optimized lookahead. Without the exact private ELF, seven
run and two explicitly skip. Two copied-source mutations fail the native
golden: a wrong `memset` return and nonzero `strncpy` padding. All public
sources remain unchanged. A fixture-only `INT_MIN` spelling refinement to
`(-2147483647-1)` preserves every value; the prior header and actual warning
transcript are retained.

The observer inherits the published lifecycle/registry decoder unchanged.
Its sole local instruction extension is the manufacturer-reviewed 128-bit
`PXOR`, with full operand snapshots, register-zero enforcement, reserved
encoding rejection and the existing 250,000-instruction cap. The exact
approved method and inherited spans are pinned. Manufacturer instruction
semantics are not a native EE hardware execution proof. No original bytes
are copied into the public golden header.

Genuine upstream warnings from `memchr` and `memset`, plus target diagnostics,
remain visible in the actual command records. A successful whole unchanged
source is not required to compile without warnings. Historic missing-limits,
successful dependency-command warnings, initial native diagnostics and a
Windows Make-path parser failure are separately retained with candid labels.

Author review covers all 442 selected original instructions, all five whole
generic C sources and R5900 assembly files, the two entire target `strncpy`
candidates, scoped decoder/native/guard code and producers. Parent and independent peer review passed, followed by canonical registration. Both preserve all960 RAW inputs before this separately archived publication-status update. Eighteen complete dictionaries reproduce with narrowly disclosed private output/link paths, measured test times and separately checked native PE header fields. Twelve target objects/twenty whole target ELFs, nine native objects, synthetic traces and goldens agree exactly RAW. The parent supplies standard source/dependency provenance metadata from the measured chosen-recipe closure while retaining the unchanged author draft. All five functions remain reconstructed, with zero exact matches. Whole native PE raw equality is not asserted; full images/checksums/differences are retained without patching or a target exception.

## Reproduction

With the validated private ELF, pinned compiler packages and dependencies:

```powershell
.venv/Scripts/python.exe build/runtime_strings/probe.py
.venv/Scripts/python.exe tools/trace_runtime_strings.py --golden-header tests/native/runtime_strings_golden.h --output build/runtime_strings/trace.json
.venv/Scripts/python.exe tests/native/run_runtime_strings.py
.venv/Scripts/python.exe build/runtime_strings/target_wrap.py
.venv/Scripts/python.exe build/runtime_strings/negative.py
.venv/Scripts/python.exe build/runtime_strings/dependencies_native.py
.venv/Scripts/python.exe build/runtime_strings/guards.py
.venv/Scripts/python.exe build/runtime_strings/proof.py
.venv/Scripts/python.exe build/runtime_strings/metadata.py
```

The ignored author packet records whole object/ET_EXEC hashes, every actual
command and dependency closure, source blobs, bounded traces and histories.
Private replay must preserve the canonical read-only target `abi.c` and host
`native_abi.c` compiler-input filenames, redirect every output prefix, and
check all source-free raw baselines before and after each producer. It may
normalize exact output prefixes, genuine random `link-*` directories, measured
test durations, and only explicitly inspected native PE clock/checksum fields;
target objects and entire ET_EXEC files are always compared raw.
