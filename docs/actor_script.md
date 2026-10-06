# Actor native script callbacks

This isolated batch reconstructs 23 complete numerical entries spanning 2,764
original text bytes. Every entry has an actual native registration in
`00179240`: the original materializes its callback, command string, signature,
argument count and zero flags before calling `002CE6B8`. The complete registrar
body confirms that it hashes the strings and retains the native pointer.
The ignored manifest records each original instruction pair, call, string
address and terminating-zero hash. These strings provide entry evidence;
they do not receive an additional data or matching-code award.

| Entries | Retained command strings |
| --- | --- |
| `192E70`, `192ED0`, `192F40`, `192FA0` | name, position, gender, angle |
| `193010`, `193070`, `1930F8`, `1931B0` | health, gethealth, addhealth, isdead |
| `193218`, `193298`, `193348`, `1933A0` | usealternatesounds, setshockwaveeffect, sethintcomponent, cancelhintcomponent |
| `1933D0`, `193470`, `1934D0`, `193558` | vehicle, faction, dontrender, freezepose |
| `1935D8`, `193608`, `193638` | sheatheweapon, unsheatheweapon, setgrapplerange |
| `1936C0`, `193778`, `193830`, `1938D8` | setgrappletargethitcb, setgrappletargetreachedcb, setnodie, inzone |

Names remain numerical. In particular, the retained `gethealth` registration
points to a scalar setter; the observed instructions determine its behavior.
Each callback looks up key `0x69F0BC67` through the existing registry contract.
All 23 native callbacks have a void `(s32 argc, s32 destination)` interface;
the first input is unused by these bodies. CPU results from sheathe helpers
are ignored and do not produce a VM result.

The callbacks reuse the reviewed eight-byte Deimos value representation,
pool-node retention/release, vector serialization and actor/control methods.
Destination arithmetic wraps the unsigned index shifted by three, with only
`-1` suppressing publication. Existing bounded-storage and nonnull-reference
contracts remain explicit; no additional range or null behavior is invented.
Word results store tag, payload and subtype in that order. Scalar and boolean
results store payload first. Empty results clear only subtype and tag, leaving
the payload intact. Missing actor queries usually leave output untouched;
the vehicle query and setter families retain their distinct empty-result paths.

Scalar arguments require signed tag 2, booleans tag 1, callback nodes tag 5 and
keys tag 6 only where the original checks them. Hint and pose callbacks consume
raw arguments without such gates. Error paths invoke the bound original
diagnostic and preserve each body's output behavior. Angle conversion uses
exact binary32 `0x42652EE0`. Add-health stores the sum before its comparison and
clamp, then writes the clamped value. The ordered zero comparison sends NaNs
to zero; the upper clamp uses the shared, independently expressed EE minimum
value model. Pose freezing modifies the full 64-bit flag word before reading
the current argument array.

The callback setters capture the old node, optionally release and clear it,
then reload the global argument pointer. They publish the replacement before
retaining it, including a null replacement. They do not recheck the tag after
release. The hint setter cancels first and then reloads both arguments. The
shockwave setter caches its key before cleanup, publishes the constructor
result and invokes the captured reference's signed-adjusted virtual pair.
The zone query preserves the complete 32-bit engine result in a boolean-tagged
slot, including noncanonical true values, and still invokes the engine when
destination is `-1`.

Complete source-family macros cover scalar setters, callback setters and
observed result publications. They share only proven whole behavior, while
individual gates, captures, stores and callback reloads stay explicit. There
are no new private helper address bindings, assembly function bodies, imported
proprietary source or generated-byte patches.

Run the asset-free checks with:

```powershell
.venv/Scripts/python.exe tests/native/run_actor_script.py
```

The frozen harness passes 598 checks across all 23 bodies. It covers absent
lookup and destination `-1`, signed tag rejection, output/payload preservation,
full state and flag values, exact angle bits, NaNs and signed zero, callback
changes to argument pointers and stored nodes, publication before null retain,
raw hint/pose arguments and captured shockwave keys. Alias cases include a
node-clear store overwriting the next argument payload and a scalar setter
whose result header overlaps actor storage. Host arithmetic and engine mocks
omit EE FCR effects and do not certify general nonfinite instruction behavior.

The author and research independently read all 23 complete original bodies
(691 instructions) against C/header, including every decoded nonstack store
and terminal delay. Research independently reproduced all 23 original
hashes/boundaries, 71 local branches, 51 outgoing callee JALs, all 23 genuine
registration blocks, 69 preserved materialization pairs, 46 complete string
hashes and argument counts, whole registrar bytes, numerical bindings and
three binary32 constants. Its native run passed all 598 checks. Its fresh
69 complete compiler/link comparisons under the three supported recipes
reproduce the packet with zero unresolved relocations and zero exact matches.
No mandatory source defect was found. Every routine remains reconstructed;
the parent also independently read all 691 original instructions and the
entire C/header/native packet, reproduced the full registration/string proofs,
all 69 genuine linked comparisons and all 598 native checks. The reviewed
manifest and separate numeric bindings are registered in
`config/functions/actor_script.json` and `config/symbols/actor_script.json`.
Central-only binding reproduction preserves all three recipes and their whole
comparison results. All 23 routines remain reconstructed. No canonical verifier
was run by this author.
