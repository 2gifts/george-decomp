# Timer registry and elapsed values

Three complete numeric routines reconstruct 976 original bytes. Each shares the same lazy rate/descriptor registration sequence; the rate allocation's first word remains unwritten. Names and layouts describe observed fields, without establishing an original timer class or duration units.

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `002BD3C8` | 300 | Register when needed, read Count once, store the low word. |
| `002BD618` | 340 | Read Count, then load the old timer; optionally replace the timer and return unsigned wrapped subtraction. |
| `002BD928` | 336 | Capture the rate pointer, call elapsed, then divide by the captured object's freshly read divider. |

The repeated ordinary C macro preserves allocation and descriptor publication order, range captures before descriptor stores, the conditional append and its fresh end reload, and exit callback registration. It reuses the reviewed allocator, upper bound, unsigned comparator, genuine SGI vector method, and Newlib exit registration. The three source/header initial copies remain unchanged through author verification. Nine complete natural candidate links under GCC 2.9, GCC 2.9 with the supported SAVE128 attribute, and GCC 3.2.3 resolve without relocations; none matches retail bytes. No optimization search or instruction substitute was used.

The target probes measure pointer/u32/s32/long widths, the three partial layouts and genuine exit-registration fields under both compilers. Actual callers forward one or two GPR arguments and preserve the returned low word for elapsed/scaled. Start uses an effects-only convention inferred from reviewed callers; this does not prove an original declaration or erase its incidental original result.

The strict observer executes the selected bodies and complete supporting original intervals. Its only local instruction addition accepts exact `40024800` at the two reviewed Count read sites; the shared integer decoder stays unchanged. The [manufacturer instruction manual, page 318](https://raw.githubusercontent.com/DarrenRainey/PS2-Programming-Docs/b13fb347ceebd3f71a0ce978adb15b60c64ed6ec/EE_Core_Instruction_Set_Manual.pdf) specifies sign extension of a COP0 low word into the modeled low 64-bit GPR value. The [manufacturer user manual, pages 62 and 49](https://raw.githubusercontent.com/DarrenRainey/PS2-Programming-Docs/b13fb347ceebd3f71a0ce978adb15b60c64ed6ec/EE_Core_Users_Manual.pdf) identifies register 9 and the privilege condition. These are read-only manufacturer documents hosted by a third-party mirror, with pinned full-file/page hashes. The fixtures deliberately supply Count values; hardware timing, increment frequency, interrupts, privilege traps and unobserved upper lanes are outside this test.

There are 509 initialized fixtures and 125,730 original instructions. Coverage includes all selected instructions except three constant-divisor `BREAK` delay words and the zero-divider `BREAK`; native division fixtures require a nonzero divisor. The original words remain part of every complete comparison. Cases exercise unsigned wrap, signed reset inputs, lazy registration, SGI cached/refilled allocation, full/spare ranges, exit-list allocation and controlled failure, mutated timer/rate/global values, timer/divider aliasing and an append destination that aliases the range end field.

The native runner passes 3,169,166 checks against original observations. It executes actual published heap/upper-bound/comparator spans, the entire authentic constructed SGI vector implementation, unchanged licensed Newlib `atexit`, and the bytewise size-over-speed branch of pinned generic `memmove`. Exact extraction spans, whole sources, actual dependencies and generated translation units are recorded. The C/C++ prefix views and static-member aliases use measured GNU/native storage conventions; they are not a universal C++ layout/lifetime claim. The special end-field alias case takes the C append path and never invokes C++ insertion. Historical C++ header warnings are retained. Inactive stream linkage hooks fail fast; no stream object layout is inferred.

Count, the underlying core allocator/release, allocation-failure reporting, and the pop callback are controlled support seams. The deliberate reporting observer returns only on the exit-list failure fixture; no actual abort/OS behavior is reproduced. Registered exit callbacks are inspected but never executed. The prior source-free pending-pop record is preserved as historical evidence. No additional helper, table, allocator or data recovery is awarded. Generic native bytewise copying validates these finite objects and output/event observations without claiming the optimized retail copy schedule or fault/volatile behavior.

Three genuinely compiled incorrect C variants fail fixtures 90, 406 and 500: loading the new timer as the old value, reloading the rate global after elapsed, and advancing the pre-store range end. Nine synthetic decoder guards pass with the original ELF present or absent. Early native/link failures, the missing appended pointer-cell classification and the corrected alias invariant are preserved separately; no production fix or valid-fixture exclusion resulted. Typed pointer translation is restricted to actual descriptor, vector, free-list, exit-registration and global pointer fields; scalar words remain unchanged.

Run public checks with Python and the local legally supplied original/toolchains:

```powershell
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_timer_registry.py
.venv/Scripts/python.exe tools/trace_timer_registry.py
.venv/Scripts/python.exe tests/native/run_timer_registry.py --output build/native/timer_registry
```

Author, distinct independent peer and parent reviews passed, followed by canonical registration. Fourteen entire main dictionaries and three native auxiliary dictionaries reproduce with37 strict RAW artifact pairs and four full native PE timestamp/checksum observations. All489 inputs were unchanged before this archived publication-status annotation; the current freeze has488 RAW-equal inputs plus this explicitly archived document. The single path-length-dependent initial heap.o Make continuation is qualified by its exact source-specific prefix pair, complete measured file lists and retained RAW output. Sources/headers, all prior failures and controls remain unchanged. Parent supplies a separate measured chosen-recipe provenance package for the authored source. All three routines remain reconstructed, with zero new exact/helper/data credit.
