# GNU exception handling runtime reuse

Fourteen complete functions cover 2,852 original bytes / 713 instructions.
Eight independently reproduced whole-code matches total 1,780 bytes, using
the unchanged `L_eh` portion of the already published
[libgcc2.c](../src/runtime/gcc/libgcc2.c). Canonical checkpoint verification confirms all eight whole-code matches. Ten source routines with only FDE or unused metadata references remain
deferred and receive no recovery or matching credit.

| Entry | Original source name | Bytes | Reviewed result |
| --- | --- | ---: | --- |
| `00370378` | `__default_terminate` | 16 | Complete whole-code match |
| `00370388` | `__terminate` | 36 | Whole link differs at 10 bytes |
| `00370468` | `__get_eh_info` | 40 | Writable local-data link blocked |
| `00370490` | `eh_context_initialize` | 40 | Writable local-data link blocked |
| `003704B8` | `eh_context_static` | 104 | Complete zero-storage link; code differs |
| `00370548` | `__sjthrow` | 376 | Writable local-data link blocked |
| `00370830` | `old_find_exception_handler` | 216 | Complete whole-code match |
| `00370908` | `find_exception_handler` | 324 | Complete whole-code match |
| `00370A50` | `get_reg_addr` | 136 | Complete whole-code match |
| `00370AD8` | `copy_reg` | 124 | Complete whole-code match |
| `00370B58` | `next_stack_level` | 140 | Complete whole-code match |
| `00370BE8` | `__unwinding_cleanup` | 8 | Complete whole-code match |
| `00370BF0` | `throw_helper` | 816 | Complete whole-code match |
| `00370F20` | `__throw` | 476 | Writable local-data link blocked |

The new unchanged [eh-common.h](../src/runtime/gcc/eh-common.h) has raw SHA-256
`d14e1de4b9f304e8cdc55e41bcc5289b30adb766c5932f7478acb497748ea29c`.
The existing frame and thread headers are reused. All sources and headers are
exact Git blobs from revision `b595ded606227e93b8c4a447446c1d2ac093827d` of
[SSXModding/ps2-ee-toolchain](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc).
Individual GNU notices, GPL terms and the exact historical libgcc2 linking
exception remain in their files and `LICENSES`. The separate
[provenance](../src/runtime/gcc/eh_provenance.json) records all five raw pins.

The historical Makefile explicitly requires `-fexceptions` for `L_eh`; its
SHA-256 is `98f3c814dc3f2b3a9dd1f86b614eeeba7125319ca70a17adc065b3cb63552a9e`.
The genuine cross configuration supplies `inhibit_libc`. Default GNU builtin
behavior and the same recipe with explicit `-fno-builtin` are reported separately.
Both GCC 3.2.3 attempts honestly reject the unchanged three-argument
`__builtin_eh_return` API. No API adaptation, instruction patch or flag sweep is
used. All 56 records reproduce: twenty complete GCC 2.9 links, eight writable
data blockers and 28 records from those two compiler failures.

Four genuine target-computed 31-word ABI arrays agree: pointer/int four bytes,
long/word eight, context sixteen, allocated context twenty-four, old/new region
records twelve/sixteen, descriptor twenty-four, frame state 752 with saved
registers at offset 668, and a 1,600-byte setjmp cleanup scratch buffer.
Source-only allocator/free bindings use the real engine entries already reviewed
by the project. Frame decoding, memcpy, memset and abort use existing bindings.
No supporting body receives another source award.

Forty-nine actual encoded entry references carry full containing-body raw and
assembly identity. Three additional roots have actual initialized callback
values and complete load/call or publication paths. Scanner intervals are split
at source counterparts and genuine original ends. Default termination ends in
its nonreturn abort call; `__sjthrow` ends in the actual nonlocal JR v0 with a
stack-pointer reload in its delay slot. Neither is misclassified as a normal
return. The selected inventory has 138 stores, 74 branches, 24 direct calls,
thirteen RA returns and nine other indirect transfers.

The static context retains the entire real 32-byte zero `.bss` at `0048F270`.
Context, initialized flag and top-element storage begin at offsets 0, 16 and 24.
The existing public proof rules accept the original LUI/LW pair at `003704B8`
and `003704C0`, with its preserved base across the stack adjustment. Both parent
and peer replayed the complete natural NOLOAD links: 96 compiled bytes versus
104 original, 53 differing bytes, zero unresolved relocations. This required no
mapper change and earns no exact-match credit.

The independently bounded writable initializer at `004021B0` contains eight
callback bytes, one CIE, twenty-four FDEs and the actual zero terminator:
1,528 bytes, SHA-256
`e33f3b04ec487cc52cf6f77c4d5393b2564e663a6ac0a2ebe4c28be61fa7f65b`.
The complete genuine linked candidate is 1,520 bytes and differs at 729 bytes,
including its length difference. Its two initial pointers agree. The no-builtin
whole-data trial fails real section-overlap checking. Neither initializer is
mapped, exported as a byte array, awarded credit or substituted into the hybrid.

Author and disc peer read all 713 selected original instructions, active source,
headers, scripts and supporting ABI bodies. The parent read all 445 instructions
of the eight matches, the entire active source and headers/scripts, and original
context/global windows. Independent parent reproduction agrees on twelve whole
dictionaries, 56 comparison records and twenty genuine links; all 25 frozen
inputs remained unchanged. Tracked source and headers with only central bindings
reproduce seven complete source, scope, comparison, dependency, data, compilation
and ABI dictionaries. The static mapping was separately replayed through the
public verifier rules.

Valid initialized exception chains, bounded region tables, register indices and
ABI-conforming frame pointers remain upstream requirements. Cleanup pop-before-
call order, matching priority, context reloads, complete frame/register copies and
genuine compiler nonlocal primitives are preserved. This proves source and
compiled-code identity; it includes no host unwinder test, OS/thread guarantee,
malformed-input safety or EE hardware/FCR execution claim.

The combined checkpoint passed 331 tooling tests (one platform-specific symlink
skip). Its hybrid build retains the retail executable SHA-256
`01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`.
