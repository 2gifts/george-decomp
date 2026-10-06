# George of the Jungle decompilation

A matching decompilation project for **George of the Jungle and the Search for
the Secret**, USA PlayStation 2 release **SLUS_216.68**.

The first milestone is a reproducible partial reconstruction: the entire main
CPU `.text` section reassembles byte for byte, and a small set of recovered C
functions compiles to the original instructions. This is an early research
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
| Recovered game C | 13 functions reviewed; 8 match (292 bytes) |
| Reused upstream C | 1 match (28 bytes: `fabsf`) |
| Reused upstream assembly | 3 functions match (572 bytes) |
| Full source build | Incomplete |

The C matching total is **320 / 3,083,712 code bytes (0.010377%)**, including
game and runtime code. The denominator includes `.text`, `.rentext`, and
`.vutext`; middleware and VU code are still unresolved. Assembly reproduction
and original data retained in the hybrid build do **not** count as C progress.
<!-- progress:end -->

## Build on Windows

Use Python 3.12 or newer and run these commands from the repository root:

```powershell
python -m venv .venv
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe tools/bootstrap_toolchain.py
.venv\Scripts\python.exe tools/bootstrap_toolchain.py --verify-only
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
The original compiler version is still unidentified. The open homebrew GCC
3.2.3 candidate can reproduce some functions, which does not establish that it
compiled the game.

[Runtime reuse notes](docs/reuse.md) document the exact upstream source,
licenses, and independent byte comparisons. [Research notes](docs/research.md)
record the executable layout and next work. Imported runtime files retain their
licenses; the project's tooling license applies only to original tooling.

## Contributing

Start with the [contributor workflow](CONTRIBUTING.md). Keep proven names separate
from inferred ones, check signatures and structure offsets against instruction
behavior, and record source provenance before copying code. A plausible function
or an automated decompiler listing remains reconstructed until a real compiler
comparison proves a match. No-op padding and generated assembly cannot inflate
the C matching total.
