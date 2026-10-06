# Actor controller setup and update

Fourteen complete numeric routines cover **3,368 original instruction bytes**,
or **842 instructions**. They recover controller setup, update, pooled cleanup,
type-key dispatch and seven retained script callbacks. Original class names
remain unknown; the controller uses an opaque pointer and explicit observed
offsets rather than an invented complete class layout.

| Address | Bytes | Observed behavior |
| --- | ---: | --- |
| `0016C098` | 348 | Allocate and publish a Deimos node, then register seven callbacks |
| `0016C238` | 68 | Own-key predicate with base-key fallback |
| `0016C280` | 40 | Publish destruction table before the base callback |
| `0016C2F8` | 84 | Scalar argument gate and store to object `+F8` |
| `0016C350` | 84 | Same gate, store to `+F4` |
| `0016C3A8` | 84 | Same gate, store to `+F0` |
| `0016C400` | 84 | Same gate, store to `+104` |
| `0016C458` | 84 | Same gate, store to `+100` |
| `0016C4B0` | 84 | Same gate, store to `+FC` |
| `0016C508` | 84 | Same gate, store to `+108` |
| `0016C560` | 84 | Hash a retained name and register eight opaque descriptors |
| `0016C5B8` | 548 | Controller defaults, four wheel positions and pooled child setup |
| `0016C7E0` | 1,608 | Speed/curve query, steering, orientation timer and command transitions |
| `0016CED0` | 84 | Base destruction and optional pooled release |

All full instruction intervals equal the validated executable and original
assembly through their final return delay slot. Zero alignment is excluded.
Every selected entry has actual caller, stored member or registration evidence.
The three encoded incoming JALs have whole containing-body hashes. The constructor
caller `001FF5D0` was read completely; the large update caller `001F9908` was
checked at its actual argument window, without claiming a full semantic review
of that supporting caller.

Four selected members appear in bounded dispatch regions at `0042BD08` and
`0042BF48`. Their complete 88-byte and 72-byte hashes, actual table stores and
intervening register writes are recorded. The second region reuses construction
evidence. No new data award follows. The seven actual `0016C098` registrations
record callback, label and type materializations, all fourteen complete original
NUL-string hashes, count two, flags zero and fresh global node loads. The seven
scalar bodies share an authored C template, with original offsets and error
bindings retained. Signed halfword type gates do not change the destination
payload or invent a null check.

The setup caller proves four integer arguments and a retained pointer return.
The update has an independent `f12` elapsed argument; its actual caller supplies
the controller from fresh actor `+958`. Resource registration uses eight integer
register arguments. Complete supporting `0023A5D0` review establishes the opaque
descriptor count and `0x54` stride; it does not establish a full record type. The
pooled cleanup has five additional unregistered bodies with identical complete
hashes, recorded without extra function awards.

Setup preserves the wheel-source pointer captured before the clear callback,
then reloads configuration afterward. Each wheel component retains its original
read and store sequence. Child physical input is captured before its constructor;
the final child pointer reload follows the manager callback. The update likewise
captures the pose object, then reloads velocity from the physical pointer after
matrix construction. Desired/current steering fields are read after the original
soft comparison callback and survive the later comparison calls.

The contact loop uses the original `MOVN` conditional **count increment**, so all
four contacts are required to suppress the command transition. Sixteen native
contact patterns exercise that distinction. The readonly `(0, 1, 0)` vector is
captured before an adjusted actor float-result callback. Timer expiration uses
the existing EE square-root and reciprocal-square-root primitives; the latter is
the original single `RSQRT.S` operation rather than a host division substituted
into target C.

The isolated 32-bit native harness passes **1,488 checks**:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_controller.py
```

It executes all fourteen complete production bodies with authored controlled
callbacks. Cases cover registration and global replacement, all seven signed
type gates and error bindings, eight-argument registration, wheel defaults and
callback mutations, pooled release modes, speed and lateral gates, steering
directions and comparison-time mutations, signed member adjustment, orientation
threshold/timer boundaries, all contact combinations, stop thresholds, full
flags and speed-triggered command reset.

Host execution verifies these observed paths and captures. It does not prove
the engine matrix, quaternion or curve algorithms: the quaternion fixture is
identity plus translation, and the curve fixture deliberately writes its two
outputs. The production output remains uninitialized until the real helper
writes it, preserving its original output contract. Host arithmetic and the
reviewed GNU positive unordered comparison are limited models; EE FCR flags,
saturation and nonfinite bit-pattern behavior are not inferred from native tests.

All **42 genuine complete compiler/link comparisons** under GCC 3.2.3, GCC 2.9
and guarded GCC 2.9 SAVE128 resolve at original addresses with zero unresolved
relocations. None reproduces a complete original symbol. All fourteen remain
**reconstructed**, with no assembly fallback, byte masking or padding changes.

The isolated packet is `build/actor_controller/manifest.json` and `symbols.json`,
generated by `finalize_draft.py`; comparison reports come from `probe.py`.
It records 47 bounded branches, all 127 decoded stores including delays, seven
callback materializations, nine binary32 constants at fourteen genuine immediate
sites, a complete soft-double constant chain and the twelve-byte readonly vector.
Production, header, native, runner and proof-script hashes identify the frozen
inputs. The author read all selected original instructions; independently
reviewed shared Deimos, matrix, soft-double, member and pool contracts are reused
with narrow ABI limits. Independent parent and peer review passed all selected
original/source/native/proof inputs and fresh complete-link reproduction. All
fourteen routines are centrally registered as reconstructed; none is exact.
