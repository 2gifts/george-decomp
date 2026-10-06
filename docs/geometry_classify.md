# Geometry classification

Five complete routines recover 1,608 original bytes, including all 402
instructions and terminal delay slots. They reuse the reviewed 28-byte face,
24-byte bounds, 64-byte frame and vector layouts. Existing numeric bindings
provide full 64-bit soft conversion, comparison, subtraction and addition.

| Address | Behavior | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002A3CA0` | Five-face bounds classification | 344 | 332 | 324 |
| `002A4520` | Six-face bounds classification | 344 | 332 | 324 |
| `002A4678` | Five-face frame classification | 228 | 192 | 196 |
| `002A4760` | Five-face sphere classification | 164 | 168 | 164 |
| `0029E098` | Frame against one face | 528 | 520 | 512 |

All fifteen standard candidates genuinely link at their original addresses
with no remaining relocations. None matches every original byte. In particular,
the same-size GCC 2.9 sphere candidate differs in nine bytes and remains
reconstructed. The two routines with preserved wide-register local state use
the established optional save attribute in their selected recipe. No compiler
tuning grid, assembly substitution or comparison mask is used.

The two complete bounds routines differ in precisely two original words:
offset `0x14` changes the last crossing-eligible index from three to four,
and offset `0x134` changes the loop count from five to six. One private
ordinary C macro recovers both algorithms without adding a helper call.
Each face chooses both support corners component by component using
`0 <= normal`; unordered comparisons take the original alternate branch.
The minimum projection strictly beyond the face distance culls immediately.
The maximum projection strictly below it is inside; otherwise the crossing
bit is set only for eligible indices. The final face can cull but cannot set
that bit. The result is zero for an early cull, two if any eligible face
crosses, and one otherwise. No additional input validation is invented.

The sphere routine captures radius and all three center components before
its five-face loop. A projection minus distance strictly greater than radius
culls. A value strictly greater than negative radius can set the crossing bit
through index three; equality at negative radius is inside. Equality at positive
radius does not cull. These strict boundary choices are retained.

The single-face frame helper captures all three projected axis extents and
the signed position distance before its first soft conversion. It takes the
absolute value of each extent through the original conversion/comparison/
conditional-subtraction sequence, adds them in order, and converts the sum
back to a float. It then takes the captured distance's absolute value and
compares it with a fresh conversion of the radius. A distance within that
extent returns zero; a strictly separated negative side returns one and a
positive side returns two. Input mutations by any controlled arithmetic call
cannot alter the already captured extents or distance. The frame loop maps
the helper's positive-side result to an immediate cull, its crossing result
to bit two only through index three, and its negative-side result to bit one.

The source uses an eight-byte integer bit representation for the established
EE soft-call ABI. Native tests substitute finite host binary64 operations;
they preserve the upper word. Existing layout assertions and the new
representation-size assertion retain the observed pointer, vector and scalar
contracts. Callers must provide valid complete face arrays and storage.
Original class names and unused fields are not inferred.

Run the asset-free harness with:

```powershell
.venv/Scripts/python.exe tests/native/run_utilities.py --harness geometry_classify
```

It passes 94,656 checks using independent finite eight-corner bounds and sphere
support models, randomized frame/face cases, per-plane crossing eligibility,
strict contact boundaries and complete soft-call ordering. A controlled first
conversion mutates both frame and face inputs to verify that all projected
values and signed distance were captured beforehand. These are checks rather
than 94,656 distinct scenarios. The build uses `-ffloat-store` and the existing
physical-view alias option. Finite host arithmetic does not establish EE
exceptional values, FCR flags, instruction timing or complete engine behavior.

The parent and an independent agent each reviewed all five complete original
bodies, source, private macro, header and native harness. Both checked full
hashes, terminal delays, excluded zero alignment, bounded branches and all
nineteen actual direct entry JALs; zero JALs were found for the two bounds
entries, so no caller role is invented for them. The frame/sphere caller
argument and result roles were reviewed. The peer independently reproduced
all fifteen natural link comparison dictionaries and the 94,656 native checks.
The manifest records the full identities, references, direct callees,
two-word reuse proof and all profile comparisons. Registration is approved;
all five routines remain reconstructed.
