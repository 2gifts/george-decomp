# Perspective geometry

`func_002A3640` reconstructs one complete 1,628-byte routine. Its five planes
form a perspective pyramid from a frame, depth, width and height. These names
describe observed behavior; the original stripped executable supplies no names.
The only caller, the camera routine at `002B7AF0`, supplies perspective extents
and does not consume a return value in f0.

The source captures all three scaled frame axes before writing output. It copies
the apex translation component by component, then reloads translation for each
remaining vertex. Each vertex's three values are computed before its stores.
This preserves the observed behavior when a frame overlaps the output. The four
side planes use apex-relative cross products and apex dot products; the base
uses vertex 3 as its origin. All five cross-product windings, component store
orders and left-associated dot products follow the complete retail body.

The existing recovered vector normalizer at `002A3538` is reused for every
plane. Collapsed extents retain its `(1,0,0)` zero-length fallback. Independent
review covered all 407 original instructions, the normalizer and both calls
from the camera. Four trailing zero alignment bytes are excluded from coverage.

All three established compiler recipes compile and fully link this source at
its original address without unresolved relocations. The canonical GCC 3.2.3
candidate has 1,160 bytes and differs from retail. The routine is reconstructed;
no byte-matching credit is awarded.

The native harness passes 783 checks over plane lengths, outward orientation,
interior bounds, collapsed extents, untouched trailing memory and 13 fixtures
with separate or overlapping frame/output storage. It executes the actual
recovered geometry and vector source, with controlled substitutes for unused
trigonometric callees.

`tools/trace_geometry.py` generates those fixtures from the locally supplied,
hash-validated executable. It executes only this routine and its normalizer,
rejects unknown memory and unsupported instructions, handles delay slots and
branch-likely annulment, and rounds every arithmetic result to host binary32.
Each finite fixture executes 547 instructions including delays. The checked-in
header contains only synthetic input and output words. It reproduces exactly
from the trace and contains no original instruction arrays or assets.

This limited reference excludes EE special-value behavior, hardware precision
differences, FCR flags and timing. Finite agreement and native checks support
the recovered operation order and alias behavior; they do not establish a full
EE emulator or a matching executable.

Run the native check:

```powershell
.venv/Scripts/python.exe tests/native/run_utilities.py --harness geometry
```

Regenerate a report and a temporary header for comparison:

```powershell
.venv/Scripts/python.exe tools/trace_geometry.py --golden-header build/geometry_golden.regenerated.h
```
