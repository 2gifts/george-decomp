# Script-referenced goal constructors

This batch reconstructs 26 constructors reached by the 25 callbacks documented
in [script_gameplay.md](script_gameplay.md), together with their shared base
initializer and scalar/timer initializer. The 28 routines contain 2,352 original
instruction bytes. Each routine is address-named because the original class and
method names have not been established. A script command establishes its caller
binding, rather than proving that every retained engine command is used by this
particular game.

The common base stores the supplied owner address at +0, clears the intrusive
queue links at +4 and +8, and stores a dispatch-table address at +0C. The source
reuses the existing `GeorgeGameplayGoal`, `GeorgeGameplayActor`, math-vector,
and Deimos pool-node definitions. Additional types express observed fields and
opaque gaps. The constructors initialize individual fields; they do not clear
the opaque parts of an allocation.

| Constructor | Script caller binding | Table at +0C | Additional observed work |
| --- | --- | --- | --- |
| `001D9920` | `act_attackcomponent` | `00436818` | Scalar +14; status bytes +18=2, +19=0 |
| `001D9A80` | `act_conjure` | `004368A0` | Same scalar/status pattern |
| `001DA5E8` | `gen_lookaround` | `00436900` | Three independent float inputs at +1C/+20/+24 |
| `001DB930` | `act_facefront` | `004369A0` | Base plus table replacement |
| `001DC5A0` | `gen_restartplan` | `00436A70` | Halfword +10; +12 records full input > 0 |
| `001DC650` | `gen_setmood` | `00436AF8` | Integer +14; zero word +10 |
| `001DC6F8` | `gen_wait` | `00436B48` | Scalar +10; zero word +14 |
| `001DC7A8` | `mov_setposhome` | `00436B98` | Base plus table replacement |
| `001DF028` | `mov_drivesection` | `00436BF8` | Clear selected vehicle state; copy owner +4; allocate five 0x18-byte elements |
| `001DFD38` | `mov_walkintersection` | `00436C48` | Embedded timer +1C; multiplier 0.75; byte +14=FF |
| `001E04C8` | `mov_walkroad` | `00436C98` | Embedded timer +14; multiplier 0.75 |
| `001E0C38` | `mov_idle` | `00436CE8` | Four integer words, one narrowed byte, and selected default state |
| `001E0EC8` | `act_spinattack` | `00436DC0` | Shared scalar/status pattern |
| `001E1078` | `act_getstunned` | `00436E48` | Shared scalar/status pattern |
| `001E1DF0` | `gen_reevaluategoals` | `00437118` | Full actor at +10, distinct supplied owner in base |
| `001E1E78` | `mov_interact` | `00437168` | Script word +14; zero word +10 |
| `001E1F48` | `gen_scriptcall` | `004371B8` | Script word +10; retain its Deimos pool node |
| `001E5988` | `gen_playvo` | `00437340` | Two script words at +10/+14 |
| `001E5A58` | `mov_setpos`, userdata path | `00437390` | Object +10; increment separate object byte +5 |
| `001E5AA8` | `mov_setpos`, vector path | `00437390` | Zero +10; sequential float vector copy to +14/+18/+1C |
| `001E6828` | `mov_setposinteract` | `004374C8` | Script word +14; +10 remains untouched |
| `001E84D0` | `mov_entervehicle` | `004375A0` | Object-byte increment, three narrowed settings, timer, owner flag |
| `001E87C0` | `mov_exitvehicle` | `00437628` | Base owner=actor+34; full actor +10; script word +14 |
| `001E8950` | `mov_stopvehicle` | `00437678` | Base plus table replacement |
| `001E8D98` | `car_honkhorn` | `00437788` | Base owner=actor+34; full actor +10 |
| `001E8E40` | `gen_ignoreavoidance` | `004377D8` | Script word +10 |
| `0020D2A0` | Shared base initializer | `0043A2B0` | Owner and intrusive links |

These dispatch-table addresses were recovered from the original `lui/addiu`
pairs. The original data at each address contains nearby method-entry addresses
with intervening zero words, consistent with the retained compiler's C++ virtual
dispatch layout. Only the pointer assignments are reconstructed here. The
table data is external original data and is excluded from C progress.

`001DC538` initializes a 0x28-byte embedded structure: six zero words, a float
bound of exactly `1.0e9f` (bits `4E6E6B28`), the caller's float at +1C, and two
unit-valued floats at +20/+24. The walk constructors reset three of those zero
words in the original order and change the +20 float to 0.75. Enter-vehicle uses
0.5 as the scalar input, then reloads its owner pointer and sets owner halfword
+24 bit 0x40. The separate object byte +5 is incremented modulo 256; its class
and lifetime meaning are still unproved.

The source preserves the access order relevant to aliasing and callbacks. The
vector copy reads and writes each component individually. Restart-plan tests
the full signed 32-bit argument before recording its positivity, even when
narrowing that argument to +10 changes its sign. Drive-section reloads the
owner after the base call and reads its +4 word before container allocation.
Script-call stores its fields before invoking the independently reconstructed
Deimos retain function. No additional null checks were inserted into paths
where the original routine dereferences a pointer.

Official m2c revision `708d2d2` supplied ignored drafts, which were reviewed
against the complete instruction bodies. Context-free drafts omitted owner
parameters carried through the base call and misrepresented independent float
arguments as stale integer-register values. The checked-in prototypes use
caller and callee evidence. Shared macros represent only repeated, reviewed
constructor bodies.

The original compiler remains unidentified. Each manifest entry records the
original instruction length, file offset, SHA256, dispatch-table binding, and
linked verification profile. A reconstructed routine becomes a match only
after its entire linked code and symbol size reproduce the original bytes.
All 28 routines compile and link with both the GCC 3.2.3 and GCC 2.9 candidate
profiles, with every relocation resolved. Neither profile exactly matches these
new routines. Both emit `sd/ld` for saved integer registers where the original
constructors use `sq/lq`; GCC 2.9 reduces many remaining scheduling differences
but does not establish the original ABI or compiler identity. A separate probe
with the supported `register_precision` attribute and `GEORGE_SAVE128` source
annotation reproduces the save widths. Save-slot order and instruction
scheduling still differ, so that probe also produces no new matches. The
annotation expands to nothing in the default compilation recipe and contains
no inline assembly or backend modifications.
