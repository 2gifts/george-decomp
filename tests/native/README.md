# Native semantic checks

Property management: `run_utilities.py --harness property_management` runs 61,865 native32 checks against 91 authored complete-original scalar fixtures (7,626 instructions), including actual constructor/list/length helpers and the original misaligned copy branch. Allocator/free/VU output contracts, retained dead storage and explicit helper hooks preserve the documented model limits. Seven scoped decoder guards pass; no original code/table arrays are tracked. Production management and ownership sources compile as separate translation units.

`property_updates` exercises the five callback/deferred-destruction/cache routines
with the existing ownership source as a separate translation unit. The 272
authored original-instruction fixtures compare every memory word and callback
event: 186,429 native checks, including recursive time capture, sibling removal,
callback replacement and all cache modes. Preserved VU output is modeled only
for exact identity-quaternion inputs; no hardware/heap fidelity is claimed.
Run `tests/native/run_utilities.py --harness property_updates`; its neutral
executable name avoids the Windows installer filename heuristic. Seven
scoped guards run via `tests/test_trace_property_updates.py`.

`property_lifecycle` checks six complete ownership/init callers with 64,188
native word/call/event checks and 93 authored scoped fixtures. The harness and
production source compile as separate translation units, reusing reviewed list C.
The preserved VU call has a fixed caller-visible identity-store substitute only;
authored post-call hooks and retained free storage test scalar observations, not
heap/VU hardware fidelity. Two complete wrappers have unresolved reachability.

The `property_pack` utility harness passes 54,214 bit-exact checks against
101 bounded integer original-instruction fixtures, including real nested
next-record execution, allocator/copy mutations, head and captured-successor
reloads, wrapped-size failure paths, output-size aliases and retained partial
node ownership. Run `--harness property_pack`. The actual recovered CRC and
next-record C are reused unchanged through native call-recording adapters; six
scoped memory/call/return/delay guards pass. See `docs/property_pack.md` for
callback, capacity and runtime-model limits.

The `property_records` utility harness passes 21,951 checks against 163
finite controlled-call original-instruction fixtures: all descriptor cases,
first-match and zero-count paths, shifted payload/self-descriptor aliases,
callback offset/flags/step mutations, packed colors and the alpha +16 gap,
next-record flags/steps and reused byte-order passthrough. Run
`--harness property_records`; the runner applies `-ffloat-store`. Seven scoped
decoder/table/callback guards pass. See `docs/property_records.md` for memory
preconditions, finite host limits and the complete code/data match gate.

The `camera_basis` utility harness passes 88,568 checks against 473 finite
controlled-call original-instruction fixtures, including all switch modes,
vertical threshold neighbors, saved-local callback mutations, shifted matrix
output/source aliases, fresh mode reloads and retained first length. Run with
`--harness camera_basis`; the runner applies `-ffloat-store`. Six scoped decoder
guards check full64 shifts/moves, signed branches and the complete readonly
switch-table identity. See `docs/camera_basis.md` for model and hardware limits.

The `camera_transform` utility harness passes 13,178 checks against 176 finite
controlled-call original-instruction fixtures: allocation failure, sparse
initialization callbacks, captured projection operands and fresh aspect,
overlapping matrix/source outputs and 20 shifted translation aliases. Run
`--harness camera_transform`; the scoped `-ffloat-store` flag applies. Five
scoped decoder guards pass. See `docs/camera_transform.md` for controlled-call
and hardware limits.

The `geometry_bounds` utility harness passes 8,738 checks including 134 finite
original-instruction synthetic fixtures, destructive affine inverse aliases,
inclusive/unordered comparisons, signed count gates, wrapped allocation/address
math, closest-output aliases and captured dimensions across mutable callbacks.
Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness geometry_bounds`.

Run the 32-bit goal state-update harness from the repository root:

```powershell
.venv/Scripts/python.exe tests/native/run_goal_methods2.py
```

The default native compiler is MinGW GCC supplied by the candidate PS2 toolchain
bundle. Pass `--compiler PATH` to use another native GCC with 32-bit support.
The harness compiles recovered C directly and uses callback stubs with
instruction-derived observable expectations. It requires no ISO, executable,
original bytes, or generated assembly. The 127 checks cover member dispatch,
timers and overlapping storage, callback mutations, route fallback ordering,
idle call arguments and signed arithmetic, stack vector outputs, and complete
software-double values.

Native tests support semantic review. Matching still requires the separate
target compiler/linker and complete comparison against the original code.

Run the heap, list, fixed-slot pool, interpolation, interpreter and tree harnesses with:

```powershell
.venv/Scripts/python.exe tests/native/run_utilities.py
```

Use `--harness NAME` for a single batch; omit it to run them all.
The pool checks cover free-index order, borrowed/owned storage, freeze/restore,
callback changes, wrap and endpoint behavior. The interpolation checks cover
endpoints, intervals, overlapping output and untouched output when no interval
is found. Native float tests use the host IEEE model; they do not independently
establish the EE hardware's treatment of nonfinite encodings.

The heap and list harnesses cover coalescing/allocation/callback behavior and
intrusive sentinel/link operations. The interpreter covers every opcode and
callback mutations; tree checks cover sorted membership, recursive destruction
and captured successor behavior. These tests compile the actual recovered
sources, including the licensed AROS adapter for list operations.

The tree update harness uses an explicit native Count-register substitute for
its 610 algorithm checks, including deadlines, callback gates, context changes,
captured successors and wrapped cycle arithmetic. It does not model physical
CP0 timing. Run it with `--harness tree_updates`; the runner uses `tree_walk.exe`
to avoid Windows treating an executable name containing `update` as an installer.

The input-state harness covers controller edges/history, consumed axis flags,
shifted scalar aliases, disabled updates, allocation and field preservation,
repeat endpoints and all 256 key-byte encodings. Run it with
`--harness input_state`; its 2,475 checks use synthetic input values.

The vector-math harness passes 77 checks for vector/quaternion arithmetic,
thresholds, zero/antipodal fallbacks, callback captures and shifted aliases.
Run it with `--harness vector_math`. Its square-root fallback tests host
arithmetic, without claiming full EE floating-point or FCR behavior.

The pad-input harness checks startup aliases, all 65,536 raw button patterns,
stick deadzones, output aliases, signed pressure, connection gates, rumble and
callback-dependent state reloads. Run it with `--harness pad_input`; its 610,739
checks include callee argument/sequence assertions. Pad and angle callees are
controlled substitutes, and native square root is a finite host arithmetic model.

The pad-device harness passes 4,047 checks for connection-state transitions,
packet/button/axis/pressure decoding, repeated SDK queries, field preservation,
callbacks and shifted outputs. Run it with `--harness pad_device`. Its SDK calls
are controlled substitutes; original paths reading an uninitialized local
packet are documented and excluded from deterministic fixtures.

The geometry harness passes 783 checks for perspective-plane construction,
zero extents and shifted frame/output aliases. Run it with `--harness geometry`.
Its 13 synthetic golden fixtures reproduce from `tools/trace_geometry.py`, a
bounded finite host-IEEE instruction reference that reads the local validated
ELF. See `docs/geometry.md` for regeneration and its EE precision/FCR limits.

Run `tests/native/run_goal_methods4.py` for the drive and traffic batch's 383
checks. They cover callback mutations, signed route bytes, queue budget reloads,
geometry aliases and motion calls. See `docs/goal_methods4.md` for the recovered
state contracts and remaining platform limitations.

The rotation harness passes 2,530 checks for quaternion arithmetic, shifted
aliases, callback mutations, trig/store ordering and retail fallback behavior.
Run it with `--harness rotation`. Its 74 finite synthetic alias fixtures
reproduce from `tools/trace_rotation.py`; see `docs/rotation.md` for the bounded
reference and EE precision/FCR limits.

The scalar matrix harness passes 3,245 checks for callback captures, shifted
input/output aliases, live position reloads and axis builders. Run it with
`--harness matrix_scalar`. Sixty synthetic instruction-derived fixtures check
all initialized words; regeneration and limits are in `docs/matrix_scalar.md`.

Run `tests/native/run_goal_methods5.py` for the route/reference batch's 169
checks, including persistent scoring flags, signed route modes, NaN timers,
wide member adjustments and callback mutations. See `docs/goal_methods5.md`.

The byte-order harness passes 146,760 checks for exhaustive halfwords, raw word
encodings, finite float bit conversion, fixed counts, untouched storage, shifted
forward overlaps and wrapped products. Run it with `--harness byte_order`.
Float argument/return and float-store tests avoid nonfinite encodings because
the native x87 ABI can quiet signaling NaNs. See `docs/byte_order.md`.

The cache-transfer harness passes 12,540 checks for key hits/holes, every cursor
slot, strict unsigned overlap endpoints, zero lengths, wrapped rounding and
callback-mutated globals. Run it with `--harness cache_transfer`. Its controlled
copy substitute records the original numeric destination and argument sequence
without emulating or dereferencing physical memory. See `docs/cache_transfer.md`.

The text-token harness passes 22,855 checks for quote/delimiter behavior, leading
bounds, high bytes, counter wrap, complete-buffer aliases, shared pointer cells
and callback-mutated delimiter pointers/source/counter. Run it with
`--harness text_tokens`. Its 172 synthetic byte fixtures regenerate through
`tools/trace_text_tokens.py`; controlled length/compare substitutes do not model
the original vectorized library. See `docs/text_tokens.md` for buffer limits.

The text-lookup harness passes 66,408 checks for sparse initialization, untouched
context bytes, sentinel/code reloads and table aliases, valid signed ctype indices,
ordered duplicate/fallback value scans, callback mutation and output aliases.
Run it with `--harness text_lookup`. It links the already reviewed token helper
and unchanged licensed newlib character table; compare/copy use controlled ABI
substitutes. See `docs/text_lookup.md` for capacities and signed-index limits.

The typed-text harness passes 454 checks for all lookup precedences, sequential
truth comparisons and output mutations, vector scanner pointer ABI/ignored count,
optional lowercase/copy order, initial output aliases, wrapped depth accumulation
and fresh EOF/count gates. Run it with `--harness text_values`. The scanner
substitute writes every lane; unwritten lanes from real partial conversion remain
outside defined C semantics. See `docs/text_values.md` for limits and reuse proof.

The inverse-angle harness passes 3,106 checks, including 1,552 finite synthetic
fixtures regenerated from the original instructions, signed zero and independent
mathematical comparisons. Run it with `--harness engine_angles`. Its six decoder
guards check operand order, aliases, function scope and bounded execution. The
native build stores intermediate values as binary32; it models finite host
arithmetic without establishing the EE's full SQRT/RSQRT precision or FCR behavior.
See `docs/engine_angles.md` for fixture regeneration and limits.

## Identical game algorithm bodies

`run_utilities.py --harness algorithm_duplicates` compiles all 61 recovered
entries using shared ordinary C definitions. It passes 2,375,547 checks against
an independent sorted-range oracle, callback mutations, map lookup/visitor
effects and gated action reload/overlap cases. The 58 search entries share one
algorithm; repeated checks do not imply 58 distinct gameplay behaviors.

The complete text-parser harness passes 4,005,840 checks across 240 synthetic
instruction-derived fixtures, comparing every context/input byte and controlled
call count. Run it with `--harness text_parser`. It covers comments, tags,
slash-prefixed closing names, pointer-order-dependent scratch whitespace bounds,
four input aliases, repeated parsing and callback mutations. The new scoped
decoder has six guard tests and regenerates fixtures from the locally validated
ELF; no original instructions or assets are included. See `docs/text_parser.md`
for capacity, lookahead, termination and low-word model limits.

The direct numeric-lookup harness passes 159,630 checks for all twelve
integer/float/halfword variants, real host partial conversion writes, ignored
scanner counts, lookup precedence, pointer order, output aliases and callback
mutations. Run it with `--harness text_numbers`. See `docs/text_numbers.md` for
initial-store differences, buffer preconditions and host scanner limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_states3.py` for 1,621
asset-free checks of the complete phase/callback, effect, matrix and attachment
paths in `actor_states3.c`. Full signed phase boundaries, wrapping counters,
branch-likely phase102 gating, callback reloads and unaligned position aliasing
are checked. The host callee/arithmetic models do not claim EE FCR equivalence.

The direct-content harness passes 231,402 checks for all seven scanner pointer
ABIs, real host partial/empty writes and propagated counts, aliases/callbacks,
short-circuit truth comparisons, copy-return forwarding and complete sparse
rewind storage preservation. Run it with `--harness text_content`. See
`docs/text_content.md` for original unused return-type uncertainty and runtime
preconditions.

The input-driven camera harness passes 13,122 checks, including 144 finite
controlled-call original-instruction fixtures, held-mask priority, captured
transform/fresh input callbacks, retained local vectors, sparse constructor and
mode effects, one-step angle/timestep boundaries and shifted transform/input
aliases. Run it with `--harness camera_motion`; the runner uses `-ffloat-store`.
Its scoped decoder has eight guard tests. See `docs/camera_motion.md` for
fixture hashes/regeneration, numeric call bindings and EE/host model limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_states4.py` for 2,438
asset-free checks of all 20 state20-24 methods and the direction selector.
The harness covers callbacks that change soft-angle inputs, retained vector
outputs, phase fallthrough, duration/store aliases, shifted matrix output and
full signed virtual adjustments. Parent and research independently reproduced
this result. See `docs/actor_states4.md` for host/EE model limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_states5.py` for 1,692
asset-free checks of all 19 state25-29 methods. The harness covers retained query
outputs, request/control reloads, signed phase/timer/threshold gates, bit38,
wrapped duration callbacks, output/data aliases and full signed virtual
adjustment. Parent and research independently reproduced this result. See
`docs/actor_states5.md` for host/EE arithmetic model limits.

The geometry classification harness passes 94,656 checks against independent
finite corner/support models, strict face boundaries, crossing eligibility,
frame projections and full soft-call ordering with input-changing callbacks.
Run it with `--harness geometry_classify`. Parent and independent peer reviewed
all five complete routines and reproduced the native checks and fifteen links.
See `docs/geometry_classify.md` for the two-body reuse proof and host/EE limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_states6.py` for 964
asset-free checks of all 18 state30-33 methods and interaction record helpers.
The harness covers retained vectors/basis captures, repeated angular samples,
request/record cleanup reloads, signed phase/timer gates and output/data aliases.
Parent and research independently reproduced this result. See
`docs/actor_states6.md` for host/EE arithmetic and caller precondition limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_states7.py` for 2,266
asset-free checks of all 20 state34-38 methods and duration callbacks. The
harness covers full animation identifiers/mask toggles, fresh matrix/object
reloads, state34's two independent transitions, soft64 wrapped-angle crossings,
duration/store aliases, and cleanup fields changed by callbacks. Parent and
research independently reproduced the corrected six-store cleanup and all
native checks. See `docs/actor_states7.md` for review roles and host/EE limits.

The scalar rigid inverse harness passes 32,769 exact word checks for 512 finite
original-instruction fixtures across every four-byte input/output overlap shift
from -60 through +60 and disjoint output. Run it with --harness matrix_rigid.
Three scoped guards cover code/memory, operands and return/delay transfers. See
docs/matrix_rigid.md for finite host arithmetic and caller storage limits.

Run .venv/Scripts/python.exe tests/native/run_actor_states8.py for 1,341
asset-free checks of all 17 state39-41 members and connected callbacks/controls.
Independent reviews preserve lifecycle stores, fresh predicates, signed phase
gates, vector aliases and ballistic snapshots. See docs/actor_states8.md.

Run --harness geometry_classify6 for 39,255 finite dyadic and helper-call checks
of both complete six-face classifiers. Shared ordinary C templates preserve all
15 prior five-face comparison hashes and 94,656 existing native checks. See
docs/geometry_classify6.md for strict gates and host/EE arithmetic limits.

Run --harness property_hierarchy for 100,057 exact word/event checks against
153 authored graphs executing 15,132 original instructions. The runner compiles
production property_hierarchy.c as a separate translation unit. The native
substring adapter includes the unchanged tracked newlib source. Six guards
check original body/call/return/memory scopes and reused opcode restrictions.
See docs/property_hierarchy.md for captured successors and capacity overshoot.


The resource_lifecycle harness compiles the production source separately and
checks 175 authored original-instruction fixtures (94,071 checks). Seven scoped
decoder guards reject unsupported memory, calls, operands, returns and delays.
Engine/virtual effects are controlled caller observations; see
[resource ownership notes](../../docs/resource_lifecycle.md).

Run `.venv/Scripts/python.exe tests/native/run_actor_script.py` for 598
asset-free checks of all 23 registered actor VM callbacks. The harness covers
signed typed gates, sparse result publication, full flags, scalar clamps and
callback/argument aliases. Research and parent separately reproduced this
result and reviewed all complete originals. See `docs/actor_script.md` for
registration evidence, caller contracts and host/EE arithmetic limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_actions.py` for 1,091
asset-free checks of all 15 actor angle/effect/pose/health/request methods.
The harness covers full dispatch groups and flags, callback/argument aliases,
five effect branches, duration reloads, ray outputs and live collision float
arguments. Peer and parent independently reproduced this result and reviewed
all complete originals. See `docs/actor_actions.md` for ABI and host/EE limits.

Run `.venv/Scripts/python.exe tests/native/run_actor_substates.py` for 851
asset-free checks of all eleven registered actor substate/lifecycle methods.
The harness covers complete dispatch and phase groups, callback captures and
reloads, shifted vector aliases, signed mask narrowing, registry insertion,
linked successors and reference-count wrapping. Peer and parent independently
reproduced the corrected Z-before-W alias regression and all checks. See
`docs/actor_substates.md` for entry/ABI evidence and host/EE limits.


Resource groups: run `python tests/native/run_utilities.py --harness resource_groups`.
The 220 authored fixtures compare full buffers/events/calls/results with strict
original-code traces and execute all five connected helper C bodies. 173,269 checks
and eight decoder guards pass. Only proved pointer fields are translated; aligned
storage preserves low 16 pointer aliases. Original engine/heap/EE timing is outside
the controlled model. Complete scope and contracts: `docs/resource_groups.md`.


Resource bounds: `python tests/native/run_resource_bounds.py` executes 239
authored fixtures and 179,846 checks with separate production bounds/group TUs.
The checked bounds-table pair invokes real FCC0; engine projection supplies
only a 16-byte point and 48-byte frame prefix. Ten strict trace guards pass.
Pointer fields/count aliases retain their original widths; complete contracts
and unresolved incoming-entry evidence are in `docs/resource_bounds.md`.

Arena buffers: `python tests/native/run_utilities.py --harness arena_buffers`
executes 452 authored fixtures and 238,300 checks with separate production C.
Eight decoder guards pass. Numeric wrap, signed padding, output aliases and
post-zero mutations preserve captured bases and published cursor fields.
Native storage preserves low 16 address bits and its complete authored extent
remains below bit 31; raw numeric pointers are never dereferenced. See
`docs/arena_buffers.md` for reachability, ABI and modeled-call limits.

Actor core: `python tests/native/run_actor_core.py` executes all five complete
production bodies and 2,296 authored checks, warning-free. Callback mutations,
member adjustments, shifted aliases, retained scratch, projection/emission ABI
and soft-angle operand order are covered. Host arithmetic and engine callbacks
are limited test doubles. See `docs/actor_core.md` for the complete contracts.

Resource base: `python tests/native/run_utilities.py --harness resource_base`
executes 290 authored fixtures and 795,118 checks with reused map/pool C and
nested subscription/removal. Eight strict decoder guards pass. Every case
compares all 2,560 words, global pointers and callback events; physical native
storage must fit the original low28 mask. See `docs/resource_base.md`.

Arena extensions: `python tests/native/run_utilities.py --harness arena_extensions`
executes 628 authored fixtures and 337,948 checks with separate production C.
Eight strict decoder guards pass. Whole buffers/descriptors, four output aliases,
zero-call events and numeric wrap are checked; all five incoming entries remain
unresolved. See `docs/arena_extensions.md` for scope and model limits.

Actor construction: `python tests/native/run_actor_construction.py` executes
all five complete production functions and passes 6,146 checks. Supporting
collision, curve, matrix and engine callbacks are controlled observations;
host arithmetic does not establish EE/VU/FCR behavior. See `docs/actor_construction.md`.

Arena ownership: `python tests/native/run_utilities.py --harness arena_ownership`
executes both new source files and the published arena initializer/no-op.
It passes 246,561 checks across 886 finite authored fixtures, including complete
record/global/descriptor words and captured allocation/free events. Nine decoder
guards pass; incoming release reachability remains unresolved. See `docs/arena_ownership.md`.

Actor controller: `python tests/native/run_actor_controller.py` executes all
fourteen complete production bodies and passes 1,488 warning-free checks.
Callback/global replacement, wheel inputs, contact count, steering and timer
transitions retain scoped host/matrix/curve contracts. See `docs/actor_controller.md`.

Pose controller: `python tests/native/run_actor_pose_controller.py` executes
all six complete production bodies and passes 4,978 warning-free checks.
Sequential cloning, constructor callbacks, signed adjusted cleanup, unclamped
duration ratios and transform-pointer aliases are covered. See `docs/actor_pose_controller.md`.

Resource manager: `python tests/native/run_utilities.py --harness resource_manager`
executes four production bodies and reused pool source across 561 authored
fixtures, passing 2,447,135 checks. Nine decoder guards and 49,081 original
instructions cover switch/provider/allocation/accounting paths and full buffer
aliases. Controlled filesystem/queue/heap/map contracts remain limited. See
`docs/resource_manager.md`.

Rail-camera state: `python tests/native/run_actor_camera_state.py` executes all
fifteen production bodies on defined paths, with reused vector normalization,
and passes 3,625 warning-free checks. The original indeterminate-stack jitter
branch is explicitly unexecuted; no numerical or equal-stack-placement claim.
See `docs/actor_camera_state.md`.

Resource registry: `python tests/native/run_utilities.py --harness resource_registry`
executes thirteen production functions and the published accessor across
414 authored fixtures, passing 424,710 checks. Nine decoder guards and
24,495 original instructions cover pooled mutations, aliases, sentinel walks
and indexed/linear lookup. See `docs/resource_registry.md`.

`run_path_sampling.py` checks five connected path helpers together with the
existing production matrix inverse: 225,346 warning-free assertions against
422 authored fixtures / 40,656 original instructions, plus six decoder guards.
Lower curve projection/evaluation and the VU transform use explicit finite
contracts; this validates caller capture, aliases and ordering. See
[the path sampling evidence](../../docs/path_sampling.md) for the numerical limits.

`run_path_curves.py` executes the seven curve routines and real published vector
helpers in separate production translation units:397158 warning-free checks.
The strict original tracer covers755 authored fixtures /113140 instructions
and six guards. [Curve evidence](../../docs/path_curves.md) describes controlled
callback/query/trig/remainder contracts and finite low32 memory limits.

The shared bounded decoder rejects control-transfer encodings in executed delay
slots before interpreting them, including untaken ordinary branches. Annulled
likely delays are neither fetched nor executed. Four synthetic regression
methods cover the classifier and five published decoder runners; refreshed
original curve/path fixtures retain their earlier outputs and instruction counts.

`run_path_callbacks.py` executes fifteen production callbacks and the actual published quaternion helper in separate translation units:210,928 warning-free checks across1,536 authored fixtures /133,933 original instructions and eight guards. Controlled projection/trig callbacks preserve explicit finite observation limits. See [callback evidence](../../docs/path_callbacks.md).

`run_curve_query.py` passes78,724 warning-free checks across1,083 fixtures /57,815 original instructions and nine guards. Independent axis distances and strict query boundaries supplement original execution. See [query evidence](../../docs/curve_query.md).

`run_plane_geometry.py` passes165,110 warning-free checks across1,123 fixtures /89,426 original instructions and six guards, using actual published normalization in a separate production translation unit. Independent line, winding and sphere/bounds invariants supplement instruction-derived results. See [plane evidence](../../docs/plane_geometry.md).

The three runners model finite scalar binary32 operations with their documented compiler arithmetic options and host square root. They do not establish EE exceptional/FCR/timing behavior. The unchanged GNU C++ runtime has target object/ABI and complete-link equality evidence, without an authored native RTTI/EH execution claim.

The remaining five curve callbacks pass 208,890 checks over 1,512 original fixtures;
`run_path_callbacks2.py` executes their production source and the existing
quaternion source in separate translation units. The complete segment-distance
adaptation passes 391,307 checks over 2,747 original fixtures with explicit finite
soft-runtime observer contracts. Both preserve alias-sensitive memory and actual
call observations; SSE scalar binary32 excludes x87 excess precision. Their
[callback](../../docs/path_callbacks2.md) and [distance](../../docs/segment_distance.md)
notes state the finite-domain and EE hardware limits. The integrated reviewed
tooling suite now contains 500 tests (one platform-specific symlink skip).

`run_segment_intersection.py` passes389,118 warning-free checks across3,043 fixtures /698,525 original instructions and nine guards, using real published normalization. `run_record_collision.py` passes1,157,227 checks across718 fixtures /170,738 original instructions and eight guards, using real production inverse, normalization/scale and licensed segment distance; original transform helpers and finite native observer contracts retain the documented limits. `run_vector_transform.py` executes the two production transform bodies in a separate translation unit and passes136,999 checks across2,060 fixtures /61,800 original instructions and six local guards. Five synthetic vector methods also pass without the private executable; the actual-original alias method then skips. The [intersection](../../docs/segment_intersection.md), [record](../../docs/record_collision.md) and [transform](../../docs/vector_transform.md) notes describe numeric and hardware limits. Unchanged GNU signed division has actual target ABI, complete code/data and source proof; it has no new native arithmetic claim.

`run_plane_intersection.py` passes149,517 warning-free checks across1,317 fixtures /134,392 original instructions and nine guards. `run_spatial_queries.py` passes799,611 checks across1,074 fixtures /285,810 original instructions and eight guards, executing the real published bounds and point-transform sources. [Plane](../../docs/plane_intersection.md) and [spatial](../../docs/spatial_queries.md) notes state finite arithmetic, alias ordering, scratch initialization and stack/capacity limits. Unchanged GNU lifecycle and newlib random sources have complete target ABI/source/link evidence, without a new native OS/startup/random-quality execution claim.

`run_actor_collision.py` executes the recursive collision source with eight previously published helper translation units: 594,275 warning-free checks across 213 fixtures / 237,931 original instructions, plus nine guards. Full32 route-key regressions reject the archived initial draft; the original uninitialized query-mode path remains explicitly excluded from defined native output. `run_geometry_frustum.py` executes the constructor and real normalizer in separate translation units: 112,158 warning-free checks across 728 fixtures / 429,299 original instructions and nine guards. [Collision](../../docs/actor_collision.md) and [constructor](../../docs/geometry_frustum.md) notes distinguish complete manual review from partial dynamic coverage and finite helper/hardware limits. The unchanged memory wrappers have target source/ABI/link evidence without an added native runtime execution claim.

`run_road_queries.py` executes three selected functions and real published road/provider/bounds/registry/vector and licensed GNU/newlib runtime sources: 6,132,301 warning-free checks across427 original fixtures. `run_actor_route.py` executes three consumers with exact byte-extracted published goal helper functions and real road source: 2,182,795 checks across96 fixtures. `run_route_setup.py` validates both production initializers and all1,536 arena words across1,224 fixtures: 1,880,064 checks. `run_allocator_wrappers.py` validates the two explicitly modified licensed newlib wrappers against960 original fixtures: 24,960 checks. The [road](../../docs/road_queries.md), [actor](../../docs/actor_route.md), [setup](../../docs/route_setup.md) and [wrapper](../../docs/allocator_wrappers.md) notes state controlled callbacks, alias ordering, raw pointer and finite arithmetic limits. No additional exact-code credit is claimed.

The allocator lock runner passes80,640 warning-free checks over5,760 original fixtures. The stdio close runner executes selected source with unchanged published flush, initialization and read files:1,504,979 checks over731 fixtures, with two compiled negative controls. The actor route initialization runner executes three selected bodies, whole route_setup.c and six exact raw helper function/macro translation units:8,122,867 checks over984 fixtures. The lock, stdio and route initialization notes preserve callback, pointer, inferred-return and finite-domain limits. All seven bodies remain reconstructed.

The buffer-record runner executes six selected routines, unchanged complete published length source and pinned licensed generic Newlib strncpy support. It passes285,633 warning-free checks over548 original fixtures, comparing the512-word arena, ordered helper arguments and getter results; nine guards and a stale-data-pointer negative control also pass. Known pointer-cell representation translation, unaligned/small-count copy domains and inferred void-return limits are documented in [the buffer notes](../../docs/buffer_records.md). Two complete selected functions match52 original bytes.

The buffer-manager runner executes three selected routines and unchanged published length, count accessors, strstr and ctype source, with licensed generic Newlib strncpy as native support. It passes2,233,020 warning-free checks across867 fixtures /165,053 original instructions, plus nine guards. Explicit unknown indexed-array and callback contracts, initialized pointer-cell translations and nonoverlapping byte-copy domains remain documented in the buffer-manager notes. One complete16-byte function matches; the other two remain reconstructed.

The array-record runner executes nine selected functions with exact published heap-wrapper macros, the whole count-accessor TU, unchanged licensed qsort and pinned generic memcpy support. It passes464,677 checks across449 fixtures /27,409 original instructions, with every selected instruction visited, nine guards and a cached-count negative control. Sorting covers signed32 keys, stride4 and counts0..12; allocator cores and original sort are controlled contracts. One complete eight-byte function matches under all three compiler recipes; supporting helpers earn no additional credit.

The UI runner checks308 authored fixtures /215,706 original instructions and1,053,921 native assertions using unchanged published source and licensed support. It preserves overlapping scratch, full signed64 formatter lanes, both next-node conventions and fresh secondary/global loads. Eight guard methods and three negative variants cover decoder/memory strictness and source-sensitive behavior. Original renderer/parser/completion effects remain explicit controls, EE conversion claims remain bounded, and the linked array getter is not executed by the empty command-array fixtures. Nine natural whole-function comparisons remain nonexact.

The registry runner executes487 initialized authored fixtures /256,632 original instructions and1,194,161 native checks, including complete selected coverage, callback-sensitive capacity capture and a meaningful cached-capacity negative control. Genuine heap/length macros/spans and licensed string/ctype support run in separate translation units; allocation cores/free/diagnostics remain controlled contracts. Nine guards pass; absence gives five passes/four skips. The finite pointer ledger is qualified across the actual fixtures, and ASCII/initialized-memory/scratch limitations remain explicit. Twelve whole selected compiler comparisons remain nonexact; eight authentic ABI callers also reproduce using canonical-only bindings.

The file-operations runner executes522 initialized original fixtures /184,978 instructions and1,384,882 native checks. Complete selected source, published registry/CRC/length and exact heap/map/accessor macro spans run in separate TUs with licensed generic string support. SDK/cache/DMA/core effects and a bounded archive-order bridge are explicit controls; memcpy is compiled but never reached, and separate tolower TU execution is unclaimed. Nine guards pass, absent-original gives six passes/three skips, and a fresh post-size slot failure-gate omission fails fixture440. All arena/slot/global/text/result/event values are compared with independently audited finite typed pointer translations. Thirty selected compiler links and twenty genuine ABI callers preserve complete artifacts using canonical bindings; all selected links remain nonexact.
