The recovered close routine at `0x003941D8` spans 180 bytes. It retains the pinned Newlib `fclose` NULL, initialization and inactive-stream gates, output flush, signed close-result test and final flags clear. The executable omits the three clauses that free the main, ungetc and line buffers. The new source records that modification with a dated comment and retains the original Berkeley notice. Removing the comment and reinserting the exact six deleted lines restores the complete pinned Git blob byte for byte. This is a modified licensed adaptation, not an unchanged-source identity claim. The project acknowledgement is in [the Newlib notice](../LICENSES/newlib-1.8.1.txt).

The 48-byte helper at `0x003947D8` copies freshly read bytes forward. Its source preserves zero count and overlapping-copy propagation. Both observed callers in `fread` discard its incidental return-register contents. The numeric C function uses a caller-compatible void convention; no destination-pointer return or generic `memcpy` identity has been established.

The original scan proves four encoded calls across all five executable sections, validates their complete containing functions, and excludes alignment padding from both selected intervals. The author read all 57 selected instructions, all three complete callers and the complete initialization, flush, read and standard-stream initialization helpers for the relevant argument, field and callback ordering. Both close callers retain and propagate the signed close result through their returns. The initial source-free note describing those results as ignored is imprecise and remains preserved as historical evidence; no production change follows. The two fread copy results are actually discarded. Supporting routines receive no new recovery award.

| Genuine compiler profile | Close bytes / different bytes | Copy bytes / different bytes |
| --- | --- | --- |
| GCC 3.2.3 | 200 / 143 | 60 / 54 |
| GCC 2.9 | 176 / 146 | 52 / 19 |

All four whole function comparisons link without unresolved relocations using the actual published initializer, flush and shared-context bindings. Neither function matches. Both compiler probes confirm the 15-word target layout and four complete call observers: one GPR argument to close, three to copy, without floating arguments. Target `long` and `fpos_t` are eight bytes and the context size is 752; the 32-bit native observer has four-byte `long`/`fpos_t` and context size 748. The tested FILE size and field offsets agree. Seek is never executed, so the harness makes no claim about that different return ABI.

The strict original decoder supplies 731 initialized fixtures and executes 62,660 original instructions, with a maximum of 240 per fixture and all 57 selected instructions covered. It executes the real complete flush, initializer, standard-stream initializer and read instruction bodies. The native harness executes the selected C plus the unchanged published flush, `findfp`, `fwalk` and `fread` files as seven separate translation units. The full `fwalk` file is compiled but its walk/cleanup paths are outside the fixtures. There is no source extraction or substituted flush/init/read implementation.

The native-only `stdio_observe_fread_copy` bridge has the authentic pointer-return `memcpy` interface, invokes the selected void helper and returns NULL. Unchanged `fread` discards that value at both call sites. This bridge has no source or return-value award and does not alter the published target recipe. Write and close hooks supply controlled results and mutations only. Refill, allocator, read, standard-write and seek hooks fail if called. Native checks compare the complete synthetic arena, callback sequence, result and shared-context value: 1,504,979 checks pass. Only the 87 explicitly typed pointer cells are translated; each FILE's packed flags/descriptor word remains a scalar even when its bits resemble a guest address.

Nine decoder and original regressions pass, including strict delayed-control encoding, initialized memory, signed short loads, callback freshness and independent sequential byte-copy invariants. Without the local original, five synthetic tests pass and four original-dependent tests skip. Two compiled negative controls fail as intended: the exact restored upstream cleanup calls the forbidden free hook, and a deliberately cached close callback produces the wrong event after flush. Thirteen genuine `-M` closures record 51 distinct dependency files. The unmodified K&R initializer requires `-Wno-missing-parameter-type`; unchanged `fread` requires `-Wno-sign-compare` for its signed/unsigned comparison. Those narrowly recorded suppressions do not change the source or tested behavior.

This observer covers bounded initialized low-word state, not EE upper register bits, traps, host filesystems, allocation, synchronization or malformed-stream contracts. Positive copy inputs require their readable/writable byte domains; the C introduces no synthetic capacity, alignment or NULL guard. Supporting refill and division paths remain outside this model. Final parent and peer review passed; the isolated records remain reconstructed.

With a locally supplied matching original, reproduce the public checks from the repository root:

```powershell
.venv/Scripts/python.exe tools/trace_stdio_close.py --output build/reuse/stdio_close/trace.json --golden-header tests/native/stdio_close_golden.h
.venv/Scripts/python.exe tests/native/run_stdio_close.py
.venv/Scripts/python.exe -m unittest discover -s tests -p test_trace_stdio_close.py
```

Upstream evidence is the pinned [Newlib close source](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdio/fclose.c) and [COPYING.NEWLIB](https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/COPYING.NEWLIB). The independent byte-copy source contains no upstream implementation copy.
