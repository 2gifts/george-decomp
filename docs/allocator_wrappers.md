The public `memalign` entry at `0x396348` and `realloc` at `0x3971A8` are two
complete 96-byte wrappers. They capture both input arguments, load the current
reentrancy pointer separately for lock, allocation, and unlock, retain the core
result across unlock, and return that pointer. Four encoded calls establish their
entries; every selected instruction, terminal return and delay slot is verified.

These files adapt the licensed newlib 1.8.1 `malign.c` and `realloc.c` baselines
from [the pinned public GNU EE source](https://github.com/SSXModding/ps2-ee-toolchain/tree/b595ded606227e93b8c4a447446c1d2ac093827d/ee/newlib/libc/stdlib).
The upstream wrappers each contain a single core call. The adaptations add the
observed lock and unlock calls and captured return, with explicit prototypes,
dated modification notes and the complete default Cygnus notice. Their retail
source origin is unknown; these are modified behavioral reconstructions.
This software was developed at Cygnus Solutions. The complete applicable notice
is retained in [newlib-1.8.1.txt](../LICENSES/newlib-1.8.1.txt).

| Source | Raw upstream SHA256 | Adapted source SHA256 |
|---|---|---|
| `malign.c` | `2ad0ef4429acb515e78568e499d960cf6d4750f23dc8c49963cac136dd69a2f0` | `426aa19be2e9217270d24d0023fffe9b98f20ed7c4fd549e48a624c93e0bb39c` |
| `realloc.c` | `e20cca24a490cba81ef9120d92b4679288f0a90599c72ba44b8d8bd41cee05ce` | `56f59280c767fc1155f3e4c57fdfbd40dae69144e19b5b11f4a4aa05b7bd88dd` |

Both genuine compilers naturally emit complete 96-byte functions with zero
unresolved relocations. GCC 3.2.3 differs in 44 bytes per function; GCC 2.9 differs
in 61. Neither is a byte match. No code/data masking, cropping, padding, mapper,
compiler or backend changes are used. The actual five bindings are the published
reentrancy pointer, lock, unlock and two previously recovered cores; no private
alias is required. The source hash guard pins each adapted file and all 15 actual
dependencies for its selected recipe, while recording the upstream baseline
hash separately.

Both target layouts prove 32-bit `size_t` and pointers, 64-bit `long`, and the
existing authentic 748-byte reentrancy layout. Eight compiler-generated caller
functions independently establish incoming GPR4/5 and core GPR4/5/6 argument
lanes, including high-bit values. The already published newlib 1.8.1 reentrancy
header is reused with the pinned 2018 compiler's newlib 1.10 public interfaces.

The scoped trace executes all 48 original instructions in 960 authored fixtures
(23,040 instructions, maximum 24). The same fixtures pass 24,960 warning-free
native checks with both production files compiled as separate translation units.
They cover zero and high-bit arguments/results, call order, independent global
changes in all three calls, and result retention despite unlock. A cached-global
test mutation fails fixture 3. Eight guards reject unsupported operands, memory,
calls, returns, delays, missing originals and incorrect original hashes.

Allocation and OS calls are controlled observers in these tests. Real lock and
unlock bodies ignore their incoming reentrancy pointer and use original thread,
semaphore and nesting state; their complete 40 instructions were read for the
caller interface, with no source or recovery award. The tests make no heap,
alignment/overflow policy, concurrency, exception, upper-register or hardware
claim. Opaque native pointers are not dereferenced. Only four actual accessed
global words have identity evidence, with no data award or whole storage claim.

Author review covers all selected instructions, both lock bodies, complete
licensed baselines/interfaces, both small callers and the two bounded allocation
windows in the 1,920-byte large caller. The large caller's complete raw geometry
and assembly equality are checked; full semantic review is not claimed. Final
independent parent and peer review passed, including complete private replay.
Both functions are registered; canonical-only links retain the four original
comparison dictionaries and add no exact-code award. The
isolated packet and producers are under `build/reuse/allocator_wrappers*`.
