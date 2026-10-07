Two full string routines are reconstructed from unchanged licensed Newlib C:
`strcat` at `0x00393758` (304 bytes) and `strchr` at `0x00393888`
(416 bytes). Their genuine GCC 2.9 and GCC 3.2.3 default candidates link completely
but do not match the retail bytes. They remain reconstructed.

The entire generic C files come from the pinned
[PS2 EE toolchain tree](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/string).
The default Cygnus Solutions notice in `LICENSES/newlib-1.8.1.txt` applies:
this software was developed at Cygnus Solutions. The old toolchain's optimized
R5900 assembly differs from the primary tree and is retained as read-only
algorithm evidence. Neither source nor compiler identity is proved by reuse.

The author read all 180 selected original instructions, both complete generic C
and R5900 assembly files, and the complete 70-instruction original `strcpy`
supporting body. The source-free audit proves 86 actual executable references
and complete geometry/hashes for 49 containing callers; large caller semantics
are qualified to bounded ABI windows. Both target compilers measure 64-bit
`long`, 32-bit pointers/integers and the same 17-word layout. All four complete
ABI callers preserve GPR4/GPR5, call the actual canonical function, and return
GPR2 without a floating-point argument conversion.

The strict observer executes the real selected instructions and complete
original `strcpy`, reusing the published runtime-string decoder unchanged.
It validates the one actual copy-call site, delay controls, return targets,
the complete 128-bit saved-register lanes and a bounded initialized stack.
11,670 fixtures execute all 180 selected and 70 supporting instructions:
2,649,980 instructions in total. Independent ordinary-byte specifications check
the returned pointer and every byte of three owned 512-byte objects.

Native tests compile both full selected sources separately with optimized
defaults, together with the entire pinned generic `strcpy` using its genuine
`PREFER_SIZE_OVER_SPEED` branch. The helper receives no new recovery credit.
The 32-bit native ABI has 32-bit `long`, so block widths and access order differ
from the retail EE instructions. Native GNU alias/alignment assumptions are
explicit; one authentic assignment-condition warning from the unchanged helper
is retained. 17,936,790 checks pass. Deliberate return-pointer and low-byte search
defects fail the same original-derived golden. Nine guard methods pass; three
original-dependent methods skip when the local game executable is absent.

Reproduce the local checks from the repository root with Python:

```text
python tools/trace_runtime_concat_search.py --golden-header tests/native/runtime_concat_search_golden.h
python tests/native/run_runtime_concat_search.py
python -m unittest discover -s tests -p test_trace_runtime_concat_search.py -v
```

The native runner accepts `--output` and checks every compile/link output stays
inside that directory. Original execution requires the user's local executable.
Public tests never require copyrighted game bytes. Fixture data is synthetic.

These comparisons cover initialized, terminated, padded, disjoint ordinary
objects with sufficient destination storage and representable low-32-bit
addresses. Wide retail reads can reach 15 bytes beyond a terminator; those bytes
are initialized in the fixtures. No production capacity/null/overlap guards
were invented. Faults, MMIO, volatile or concurrent observations, EE timing,
universal ISO-C type validity and exact original C identity are outside the
claim. Native PE timestamp/checksum variation, when present during replay, is
observed separately from strict whole target object and ELF equality.

The first compiler attempt stopped at a missing canonical `strcat` alias before
linking its ABI caller; the complete partial objects/logs remain in history.
The first native harness accidentally included target stdio headers and failed
to resolve `_impure_ptr`. Only the harness's host-header domain was corrected;
the complete failed commands and runner are preserved. Both genuine imported
production files remain unchanged. Parent and independent final peer reviews passed, followed by canonical registration. Both preserve all404 RAW inputs before this separately archived publication-status update. Thirteen complete dictionaries reproduce with precisely disclosed private output/link paths, measured test duration and parsed full native PE timestamp/checksum fields. Six target objects/eight whole target ELFs, seven native objects, trace and golden fixtures agree exactly RAW. The parent supplies standard source-provenance metadata from the measured chosen-recipe closure in a separate package; the authored import provenance and candidate draft remain unchanged. Both functions remain reconstructed. This batch grants no helper, data, assembly, exact-match or original source/compiler identity credit. Whole native PE raw equality is not asserted.
