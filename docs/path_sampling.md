# Path sampling helpers

Five complete connected helpers cover 864 bytes / 216 original instructions.
They have fourteen actual incoming JALs from the selected helper closure and
the published camera callbacks. Complete hashes, assembly/raw identity,
terminal return delays, preceding boundaries and zero alignment are recorded
in the isolated proof packet. All fifteen natural whole-function links under
the three established compiler recipes resolve without remaining relocations;
none matches retail bytes. Independent peer/parent frozen-packet reviews passed,
and all five are centrally registered. Canonical checkpoint verification and the hybrid build passed.

| Entry | Bytes | Observed operation |
| --- | ---: | --- |
| `00135D10` | 120 | Project a reference, initialize an exact −1 time sentinel, then sample a point |
| `00135D88` | 256 | Project and return current time divided by the endpoint span |
| `00135E88` | 260 | Scale an incoming fraction by that span and sample |
| `00135F90` | 140 | Invert the owner frame, transform a reference and forward the lower projection result |
| `00136020` | 88 | Evaluate a local point, then transform it with the fresh owner frame |

The partial path prefix establishes only fields at `+0C`, `+34`, `+38`,
`+44` and `+48`. The record prefix establishes unsigned format/stride bytes
and an unsigned halfword count. Numeric entry names and opaque owner/resource
pointers preserve uncertainty about original classes.

The two time conversions retain unsigned count-minus-one, stride multiplication
and address wrap. Format byte `10` obtains times from unsigned bytes at record
`+0F`, multiplied by the exact binary32 value 160 (`43200000`). Other formats
read a float at `+08`. The last record uses `(count - 1) * stride * 4`.
`00135D88` returns **current / (end − start)**; `00135E88` passes
**fraction * (end − start)**. Neither adds the first time or inserts a clamp,
zero-denominator guard, capacity check or null check.

The exact −1 sentinel (`BF800000`) remains fresh after projection callbacks.
The fraction wrapper writes its supplied value to `+44` and then `+48` before
reading the resource. Sampling captures its incoming output/index/position
pointers but reloads the owner after evaluation; projection reloads the resource
after the transform. These orders matter when hooks change fields or outputs
overlap later inputs.

The genuine published `002A1098` scalar rigid inverse and its existing C source
are reused, with no new award. The lower `002966F0` projection, `00297178`
evaluation and `002A1C60` VU point transform remain actual numeric external
bindings. The author read all four complete supporting originals for their
narrow contracts. `002966F0` has a final 0/1 integer return that `00135F90`
forwards; the other selected callers ignore it. `00297178` has a float return
that this caller ignores. Supporting functions receive no new source credit.

Both local point outputs reserve the complete original sixteen-byte region.
They have no fabricated initializer. The actual transform reads only XYZ and
stores Z, X, Y; its selected caller never reads the fourth scratch word. The
opaque lower interpolation callback's general write width is not inferred.

The author reviewed all selected instructions, full source/header, native
harness, scoped tracer, guards and proof scripts. The packet inventories eleven
bounded local branches, twenty-three stores including stack/delays, nine actual
selected-body JALs, five returns and seven binary32 constant materializations.
Whole incoming caller bytes are audited; this does not claim a new reconstruction
of their transitive runtime graph. Parent independently approved the original
entry scope and read all 216 selected originals/current C/header without finding
a defect. Both independent reviewers reproduced all seven complete packets,
all fifteen natural full links, the synthetic fixtures, warning-free native
checks and six strict guards. The tracked-source probe reproduced all fifteen
comparisons using only central symbol bindings.

The strict trace executes all five original bodies and the original published
inverse. **422** authored finite fixtures execute **40,656** original
instructions, at most **157** each. The native harness links the five C routines
and the real inverse in separate translation units, passing **225,346**
warning-free checks. Whole-window memory and call events cover format/count/
stride variants, count underflow with accessible preceding storage, signed
fractions, sentinel paths, retained versus fresh callback state, overlapping
outputs and aliased halfword index/float position storage. Six decoder guards
check code/memory initialization and bounds, conversion operand forms, finite
arithmetic, zero denominators, delay execution/annulment, actual JR31 returns,
unknown calls and loops. Six additional host cases preserve non-sentinel
encodings without feeding them into numerical curve/matrix calculations.

The lower curve routines and VU transform use explicit authored test contracts.
These tests establish selected caller behavior, not the complete curve algorithms,
VU accumulator precision, EE exceptional/FCR behavior, cycle timing or hardware
execution. Host pointer translation covers typed storage in the authored window;
it does not validate arbitrary reinterpretation of guest pointer bits as floats.
Full compiled byte equality remains the sole matching gate.

Synthetic input SHA256:
`46a77a985abaac3b37f3291476602ca84618680eb6db571c9ad014d6f2a6e07a`.
Golden header LF SHA256:
`44094ad685e980e4f20118ad2965b8b73cd67a99bd72b37224c25ca9f56d94f4`.

```text
python tools/trace_path_sampling.py --golden-header tests/native/path_sampling_golden.h
python tests/test_trace_path_sampling.py
python tests/native/run_path_sampling.py
```

These commands use the existing project environment and MinGW runtime. Golden
fixtures contain only authored inputs/results; no original code, tables, strings
or assets are exported.

The checkpoint passed 321 tooling tests (one platform-specific symlink skip).
Its hybrid executable retains retail SHA-256
`01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8`;
remaining original assembly contributes no additional C matching credit.
