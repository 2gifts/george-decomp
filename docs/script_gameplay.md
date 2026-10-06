# Script gameplay callbacks

This batch recovers 25 callbacks and their intrusive queue helper, totaling
3,960 original instruction bytes. They are reviewed C reconstructions. No byte
match is claimed for this batch with the candidate EE GCC 3.2.3 compiler.

`func_001D2B60` registers each callback through `func_002CE540`, passing a retained
command string, argument count, callback address and context. Nearby diagnostics
name `AiScriptActions` and `AiBackupMgr`. Those strings establish the command
bindings below; they do not establish that every retained command is exercised
by George of the Jungle. Vehicle and AI commands may belong to shared engine
code. Addresses remain the exported C names until original symbol evidence is
available.

| Callback | Retained command | Arguments | Allocation | Constructor |
| --- | --- | ---: | ---: | --- |
| `001D3A68` | `mov_entervehicle` | 4 | `6C` | `001E84D0` |
| `001D3B20` | `mov_exitvehicle` | 1 | `18` | `001E87C0` |
| `001D3BB8` | `mov_interact` | 1 | `18` | `001E1E78` |
| `001D44D0` | `mov_setpos` | 1 | `20` | `001E5A58` / `001E5AA8` |
| `001D48D0` | `mov_setposinteract` | 1 | `24` | `001E6828` |
| `001D4968` | `mov_walkintersection` | 1 | `74` | `001DFD38` |
| `001D4A00` | `mov_walkroad` | 1 | `58` | `001E04C8` |
| `001D4E48` | `act_conjure` | 1 | `1C` | `001D9A80` |
| `001D52D8` | `act_spinattack` | 1 | `1C` | `001E0EC8` |
| `001D5378` | `act_getstunned` | 1 | `1C` | `001E1078` |
| `001D57F0` | `act_attackcomponent` | 1 | `1C` | `001D9920` |
| `001D5CD0` | `gen_ignoreavoidance` | 1 | `18` | `001E8E40` |
| `001D5D68` | `gen_lookaround` | 3 | `28` | `001DA5E8` |
| `001D5E28` | `gen_restartplan` | variable | `14` | `001DC5A0` |
| `001D5ED8` | `gen_setmood` | 1 | `18` | `001DC650` |
| `001D5F88` | `gen_scriptcall` | 1 | `14` | `001E1F48` |
| `001D6020` | `gen_playvo` | 2 | `18` | `001E5988` |
| `001D60C0` | `gen_wait` | 1 | `18` | `001DC6F8` |
| `001D6648` | `mov_stopvehicle` | 0 | `10` | `001E8950` |
| `001D66D0` | `mov_idle` | 0 | `A0` | `001E0C38` |
| `001D6948` | `mov_setposhome` | 0 | `10` | `001DC7A8` |
| `001D6A30` | `mov_drivesection` | 0 | `90` | `001DF028` |
| `001D6AB8` | `act_facefront` | 0 | `10` | `001DB930` |
| `001D6B40` | `gen_reevaluategoals` | 0 | `14` | `001E1DF0` |
| `001D6BC8` | `car_honkhorn` | 1 | `14` | `001E8D98` |

All addresses and allocation sizes in the table are hexadecimal. Constructor
bodies remain external. Each manifest entry records the registration call and
string address, original file offset, exact body size, SHA-256 and symbol
bindings.

## Observed protocol

Callbacks receive argument count in `$4` and a destination slot in `$5`. Even
callbacks that ignore the count need both C parameters. Arguments come from
the current `GeorgeDeimosValue` array at `D_00474F48`; each value is eight bytes,
with its payload at offset four. Raw word arguments use `payload.bits`; float
arguments use `payload.scalar`. Float parameters use the independent EE EABI
register sequence `$f12`, `$f13`, `$f14`.

Most callbacks allocate through `func_002AD700` using allocator storage at
`0045C680`, conditionally call a constructor, and submit its result to
`func_001D89D8` at the current actor's offset `188`. The actor global at
`003F8AF8` is reloaded after external calls. Allocation failure retains the
observed null submission. For destination `-1`, callbacks skip result writes;
otherwise they clear subtype then tag and leave the payload intact.

The distinction between reading arguments before and after allocation matters.
The four `act_*` scalar callbacks snapshot their float first; the walk and wait
callbacks read it after allocation. Enter-vehicle snapshots its first raw word
but reloads the argument array for the remaining words. Look-around snapshots
all three floats before allocation.

Set-position dispatches signed tag 6 to a userdata constructor. Tag 4 decodes a
three-float vector into local stack storage before allocation and passes that
storage to a separate constructor. Other tags submit null. Set-mood skips
allocation and queue submission while actor word `148` has bit `400` set, but
still clears an optional result. Restart-plan uses zero when argument count is
nonpositive; otherwise it converts its first float to signed 32-bit. Its cast
and set-mood's cast compile to the same `trunc.w.s` / `mfc1` operation present in
retail. Portable host behavior for nonfinite or out-of-range inputs is not
claimed.

The 40-byte queue helper writes the new tail and head for an empty queue. For a
nonempty queue it writes the old tail's `+4` link, reloads the queue tail, writes
the new node's `+8` link, and then replaces the tail. The reload preserves the
observed behavior for overlapping layouts. The header defines only those
observed fields and checks their offsets for the target compiler.

## Review and matching

Pinned upstream [m2c](https://github.com/matt-kempster/m2c/tree/708d2d2cb2698f091a92492b328f73b24209f72d)
provided ignored research drafts using its `mipsee-gcc-c` target. Drafts were
checked against the original instructions before registration. Manual review
restored the unused first callback parameter, replaced unresolved stack-vector
storage, corrected odd-register conversion placeholders, and removed stale
integer arguments inferred by m2c. Inspection of `001DFD38`, `001E04C8` and
`001D89D8` confirms that those inferred arguments are not consumed.

Repeated source macros represent the same observed callback protocol; they do
not replace missing function bodies. All 26 original ranges are separate and
exclude alignment padding. `tools/verify.py` compiles the source and resolves
external references at reviewed absolute addresses before comparison. The
candidate compiler emits 64-bit callee-save operations where retail uses
128-bit saves, and the queue helper's branch layout also differs. These remain
reconstructions until the complete compiled instruction bytes and size agree.
