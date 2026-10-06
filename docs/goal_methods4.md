# Drive states and connected route helpers

This batch reconstructs 23 complete functions covering 10,980 original bytes.
It follows the drive goal's state dispatch, route queue expansion, map-based
occupancy checks, steering, speed transitions and motion submission. The
40-byte map lookup `001CCBE8` matches every retail byte with GCC 3.2.3. The
other 22 functions remain **reconstructed**: all compile and individually
link under the three supported recipes, with differences recorded in the
manifest. Function names and newly observed fields retain numeric addresses;
original class names and allocation capacities remain unresolved.

The nested state table at `003F8DF8` contains five complete encoded member
pairs selected by signed goal byte `31`. Each has zero this adjustment and
selector `-1`. Three entries (`001DCD48`, `001DF290`, `001DCF50`) are recovered
here; the retained no-op `001DF288` and short setter `001DF348` are included
only as table evidence. The manifest hashes the exact 40-byte table interval,
which is contained in the previously reviewed parent dispatch table. The
dispatcher preserves signed virtual offsets, full adjustment sums and the
phase reload before invocation.

The source reuses goal, road, ring, map and adjusted virtual-call layouts.
An alternate view of the existing `0x90`-byte drive goal makes the observed
float fields and vectors explicit. Queue entries have a proven `0x18`-byte
stride and signed one-byte mode/index fields. Offset and size assertions
check both views. Resource calls use their observed integer and float argument
registers, including the four-argument resource position helper and the
three-argument matrix/vector transform.

Several details affect the behavior of these methods:

- Geometry helpers snapshot every source scalar before aliased vector stores.
  Packed selectors use a sign-extended upper nibble and a masked five-bit
  shift. The packed-resource walk reloads its index and count after the size
  callback and advances by the returned byte size plus eight.
- Route queues preserve wrapped signed byte indices and signed count
  comparisons. Neighbor sentinels, odd-mode interpolation, route output
  narrowing and complete route keys follow the original operations. Queue
  expansion reloads its budget after callbacks before subtracting a length.
- Speed and occupancy methods reload mutable owner, record and float fields
  at the original call boundaries. Radius checks preserve three separate
  virtual calls and read stack/record distances afterward. Steering retains
  distinct position, velocity and basis calls rather than caching results.
- Motion uses reciprocal **squared** length on the observed paths, separate
  length-normalization calls on others, and scalar capture before x/z/y
  stores. Orientation submission precedes the fresh speed and owner-delta
  reads that determine the final motion and progress.

The route setup retains the original ignored resource-walk result. The
target setup also retains the retail path that records a failed road lookup
and then continues using the road pointer. Valid route/resource storage and
nonempty queue inputs on paths that require them are caller contracts; the
reconstruction does not add fallback behavior.

The exact embedded binary32 values were audited against their instruction
words. Representative thresholds include `3727C5AC` (the squared-length
epsilon), `3E4CCCCD` (0.2), `3E99999A` (0.3), `3F490FDB` (pi/4), `3F733333`
(0.95), `3F7D70A4` (0.99), `3F7FBE77` (0.999), `40333333` (2.8), `404CCCCD`
(3.2), and `4CBEBC20` (the route failure distance). Their rounded decimal
spellings preserve those bits.

The shared EE minimum helper uses signed encoding order and reverses order
for two negative encodings. Its independently written value model is
corroborated by [PCSX2's pinned `fp_min` and `MIN_S` implementation](https://github.com/PCSX2/pcsx2/blob/9fffbdbd59b962d63a2259b150f419ad3773e7b4/pcsx2/FPU.cpp#L97),
commit `9fffbdbd59b962d63a2259b150f419ad3773e7b4`, file SHA-256
`204dcb290f9f56e9112622ab313ef12d2192eda5b43d24efcf2dbc8ca3fa7344`.
No PCSX2 source was imported. The shared narrow `SQRT.S` primitive also
preserves the observed instruction; complete function bodies contain no
inline assembly or original instruction arrays. These helpers do not model
FCR cause flags or establish every EE nonfinite arithmetic result.

The native harness builds recovered C directly and requires no original
executable or assets:

```powershell
.venv/Scripts/python.exe tests/native/run_goal_methods4.py
```

Its 383 passing checks cover every packed selector byte, source/destination
overlap, callback-dependent index/count/owner changes, queue budget reloads,
signed route modes, float thresholds and unordered gates, steering/stop
transitions, preserved route captures and final motion scaling. Host
arithmetic and call stubs support semantic review; the harness does not
replace complete linked target byte comparison.

Independent review passed all 23 complete instruction bodies, all original
hashes and boundaries, and the five stored member pairs. A separate fresh
compilation reproduced all 69 individual linked comparisons from the three
supported recipes, including the sole 40-byte exact match. No semantic
defects were found.
