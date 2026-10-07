# George of the Jungle decompilation

A matching decompilation project for **George of the Jungle and the Search for
the Secret**, USA PlayStation 2 release **SLUS_216.68**.

The first milestone is a reproducible partial reconstruction: the entire main
CPU `.text` section reassembles byte for byte, and selected reviewed C and C++
functions compile to the original instructions. This is an early research
project, not a complete source build or a PC port.

## Progress

The machine-readable [progress report](reports/progress.json) is regenerated
by compiling the checked-in source and comparing each function with the
supported retail executable. Exact byte equality, exact code size, and no
unresolved relocations are required. Alignment padding is excluded.

<!-- progress:start -->
| Milestone | Result |
| --- | --- |
| Disc identification and extraction | 565 files inventoried; boot files extracted |
| Main CPU assembly baseline | 2,949,800 / 2,949,800 bytes identical |
| Candidate function regions | 14,560 detected; boundaries need review |
| Recovered game C/C++ | 1227 functions reviewed; 253 match (5,488 bytes) |
| Reused upstream C/C++ | 80 match (11,192 bytes: `fabsf`, `atoi`, `matherr`, `__errno`, `_localeconv_r`, `sinf`, `tanf`, `__pack_d`, `__unpack_d`, `dpadd`, `dpsub`, `__fpcmp_parts_d`, `dpcmp`, `litodp`, `dptoli`, `dptoul`, `__make_dp`, `dptofp`, `__pack_f`, `__unpack_f`, `__fpcmp_parts_f`, `fptoui`, `__make_fp`, `fptodp`, `__negdf2`, `fpadd`, `fpsub`, `fpcmp`, `sitofp`, `fptosi`, `__negsf2`, `__muldi3`, `__fixunsdfdi`, `__fixdfdi`, `__fixunssfdi`, `__floatdidf`, `cosf`, `eofread_sscanf`, `lflush`, `strtodf`, `_Bfree`, `_hi0bits`, `_lo0bits`, `__mcmp`, `__sclose`, `_cleanup`, `atof`, `decode_uleb128`, `decode_sleb128`, `fde_merge`, `end_fde_sort`, `count_fdes`, `add_fdes`, `frame_init`, `__frame_state_for`, `fde_split`, `__default_terminate`, `old_find_exception_handler`, `find_exception_handler`, `get_reg_addr`, `copy_reg`, `next_stack_level`, `__unwinding_cleanup`, `throw_helper`, `__eq__C9type_infoRC9type_info`, `dcast__C16__user_type_infoRC9type_infoiPvPC9type_infoT3`, `dcast__C14__si_type_infoRC9type_infoiPvPC9type_infoT3`, `dcast__C17__class_type_infoRC9type_infoiPvPC9type_infoT3`, `__dynamic_cast`, `__start_cp_handler`, `__eh_alloc`, `__cplus_type_matcher`, `__cp_pop_exception`, `__uncatch_exception`, `what__C9exception`, `exit`, `srand`, `rand`, `bcopy`, `index`) |
| C source | 1429 functions reviewed; 322 match (15,292 bytes) |
| C++ source | 24 functions reviewed; 11 match (1,388 bytes) |
| Reused upstream assembly | 3 functions match (572 bytes) |
| Full source build | Incomplete |

The C/C++ matching total is **16,680 / 3,083,712 code bytes (0.540907%)**, including
game and runtime code. The denominator includes `.text`, `.rentext`, and
`.vutext`; middleware and VU code are still unresolved. Assembly reproduction
and original data retained in the hybrid build do **not** count as C/C++ progress.
<!-- progress:end -->

[Archive lifecycle](docs/file_archive_lifecycle.md) now covers recursive directory loading, startup and map lifetime. Its original retry conditions and callback-sensitive field order are retained.

## Build on Windows

Use Python 3.12 or newer and run these commands from the repository root:

```powershell
python -m venv .venv
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe tools/bootstrap_toolchain.py
.venv\Scripts\python.exe tools/bootstrap_toolchain.py --verify-only
.venv\Scripts\python.exe tools/bootstrap_legacy_compiler.py
.venv\Scripts\python.exe tools/probe_disc.py "PATH_TO_YOUR_USA_ISO" --expect-iso-sha256 659d323cdf461c320a1006281db0e9be7622c1ff96b99c09da0275ebc41027d6 --expect-boot-sha256 01c035b7fb0d6a91ae0e5afa75203c3ece967196fadf651d94ef9cc1586fa4e8
.venv\Scripts\python.exe tools/analyze.py --disassemble
.venv\Scripts\python.exe tools/build.py
.venv\Scripts\python.exe tools/verify.py --publish-report
.venv\Scripts\python.exe -m unittest discover -s tests -v
```

`tools/build.py` creates **`build/SLUS_216.68`**, byte-identical to the original
ELF. Its main CPU code comes from generated assembly and verified compiled
source substitutions. The original ELF metadata, data, `.rentext`, and VU
programs are retained. The result has not been tested in an emulator; no ISO
or game executable is distributed here.

The importer uses only the standard library, streams the image, and imports
`SYSTEM.CNF` plus its named boot executable. It validates disc bounds, paths,
revision fingerprints, and output locations. All extracted game data, generated
assembly, compiled objects, and downloaded tool binaries live in ignored
directories. You supply your own disc image.

## Reuse and research

The workflow uses [spimdisasm](https://github.com/Decompollaborate/spimdisasm),
[Rabbitizer](https://github.com/Decompollaborate/rabbitizer), pyelftools,
[decompals PS2 binutils](https://github.com/decompals/binutils-mips-ps2-decompals),
and the openly published [PS2DEV toolchain](https://github.com/ps2dev/ps2toolchain).
Tool downloads, flags, and hashes are pinned in
[the toolchain manifest](tools/toolchain_manifest.json).

The retail executable is stripped. Its strings identify Papaya, Havok, and
Deimos/`DScriptMgr`; they do not establish a reusable Lua or RenderWare engine.
The original compiler version is still unidentified. Two pinned candidate GNU
compiler profiles reproduce individual functions. [Compiler setup and
provenance](docs/compiler.md) document the public source, host repairs, and
per-function selection. Exact matches do not establish the game's original
translation-unit compiler or flags.

[Runtime reuse notes](docs/reuse.md) document the exact upstream source,
licenses, and independent byte comparisons. [Research notes](docs/research.md)
record the executable layout and next work. Imported runtime files retain their
licenses; the project's tooling license applies only to original tooling.

[Actor construction and physics](docs/actor_construction.md) document five
recovered lifecycle and control routines. [Render records and arena ownership](docs/arena_ownership.md)
document allocation inputs, partial record fields, callback-visible writes and
the exact startup wrapper. These batches retain their native validation limits.

[Controller setup and update](docs/actor_controller.md) and
[pose-controller construction and interpolation](docs/actor_pose_controller.md)
recover camera points, wheel configuration, command transitions and script
callbacks. [Rail-camera state](docs/actor_camera_state.md) adds timed blending
and typed path commands. [Path sampling](docs/path_sampling.md) adds projection,
span conversion and point evaluation wrappers. [Curve selection and evaluation](docs/path_curves.md)
adds endpoint searches and format dispatch. [Curve callbacks](docs/path_callbacks.md),
[remaining curve callbacks](docs/path_callbacks2.md),
[distance/ray queries](docs/curve_query.md), [plane geometry](docs/plane_geometry.md)
[segment distance](docs/segment_distance.md),
[segment/triangle intersections](docs/segment_intersection.md),
[record collisions](docs/record_collision.md) and
[vector transforms](docs/vector_transform.md), [plane intersections](docs/plane_intersection.md)
and [provider/polygon queries](docs/spatial_queries.md) recover interpolation, strict boundary tests and alias-sensitive output order. [GNU frame runtime](docs/frame_runtime.md)
reuses unchanged licensed frame sorting and state decoding source;
[exception handling](docs/exception_runtime.md) adds context, handler search and unwind helpers.
[GNU C++ runtime](docs/cxx_runtime.md) adds unchanged RTTI and exception source,
with eleven complete exact matches. [Signed division](docs/gnu_signed_division.md)
adds another complete routine from the unchanged GNU source.
[GNU startup and exit](docs/gnu_lifecycle.md) reuse four more entries, including
the exact exit routine. [Newlib random state](docs/random_runtime.md) reuses
two unchanged functions matching all 64 original bytes.
[Recursive collision nodes](docs/actor_collision.md) and
[six-face construction](docs/geometry_frustum.md) add three complete game bodies
with unchanged published geometry, allocator and query helpers.
[Newlib memory wrappers](docs/memory_wrappers.md) add unchanged `bcopy`/`index`
source matching another 64 original bytes.
[Road-cell and neighboring-road queries](docs/road_queries.md),
[actor route consumers](docs/actor_route.md) and [route setup stores](docs/route_setup.md)
add eight connected game bodies with existing helper source.
[Allocator wrappers](docs/allocator_wrappers.md) adapt two licensed newlib
functions with the observed lock and fresh reentrancy-pointer behavior.
[Recursive allocator hooks](docs/allocator_locks.md),
[stream close and byte copy](docs/stdio_close.md), and
[actor route initialization](docs/actor_route_init.md) add seven reviewed
functions, preserving callback freshness and alias-sensitive store order.
[Resource resolution and accounting](docs/resource_manager.md)
and the [resource registry](docs/resource_registry.md) cover providers,
allocation, queued requests, pooled map mutation and shared counters.

## Contributing

Start with the [contributor workflow](CONTRIBUTING.md). Keep proven names separate
from inferred ones, check signatures and structure offsets against instruction
behavior, and record source provenance before copying code. A plausible function
or an automated decompiler listing remains reconstructed until a real compiler
comparison proves a match. No-op padding and generated assembly cannot inflate
the C/C++ matching total.
