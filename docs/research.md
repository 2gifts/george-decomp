# Initial executable analysis

Target: USA PS2 **SLUS_216.68**, ELF32 little-endian MIPS R5900. Entry point:
`0x00100008`. ELF flags: `0x20924001`. Global pointer: `0x0045fb70`.
No `.symtab` or `.mdebug` section survives in this executable.

| Section | File offset | Address | Size | Current handling |
| --- | --- | --- | ---: | --- |
| `.text` | `0x001000` | `0x00100000` | 2,949,800 | Byte-exact generated assembly baseline; partial C replacement |
| `.rentext` | `0x2d1300` | `0x003d0300` | 10,032 | Retained from original |
| `.vutext` | `0x2d3a80` | `0x003d2a80` | 123,880 | Retained from original |
| `.rendata` | `0x2f1e80` | `0x003f0e80` | 4,792 | Retained from original |
| `.data` | `0x2f3180` | `0x003f2180` | 98,240 | Retained from original |
| `.descriptors` | `0x30b180` | `0x0040a180` | 89,632 | Retained from original; format research needed |
| `.rodata` | `0x321000` | `0x00420000` | 227,936 | Retained from original |
| `.gcc_except_table` | `0x358a80` | `0x00457a80` | 228 | Retained from original |
| `.sdata` | `0x358b80` | `0x00457b80` | 88 | Retained from original |
| `.bss` | NOBITS | `0x00457c00` | 264,348 | Original metadata retained |

The disc also contains Sony IOP modules and PS2-specific data containers. The
current importer deliberately extracts only boot files. There are 565 files in
the ISO9660 inventory. Full inventory and printable strings stay in local
`orig/` and `build/` analysis outputs.

Papaya and Havok client identifiers survive in strings. Deimos table references,
`DScriptMgr` names, and `.ds`/`.ddf` filenames provide scripting leads. These are
evidence of interfaces, not recovered source or proof of a shared engine.

The EEGCC setting used for disassembly is an analysis profile. Some C leaves
match open homebrew GCC 3.2.3 using `-mfp64` and `-fno-reorder-blocks`; this is
insufficient to identify the retail compiler. Provisional parameter/return types
and partial structure layouts are supported by observed instructions, but need
caller analysis before being promoted to original class definitions.

## Next work

- Identify compiler generation and optimization settings using representative
  game and middleware functions, retaining exact tool fingerprints.
- Finish matching the current serializers and timer routines; then extend
  verification to linked functions with calls and global references.
- Map startup and the main loop, using string cross-references and callers to
  recover meaningful subsystem names and object layouts.
- Expand runtime fingerprinting beyond relocation-free objects. Compare
  relocation-normalized candidates to propose identities, then require a real
  linked byte comparison before marking matches.
- Separate game, physics, scripting, SDK, and runtime regions through evidence;
  establish per-subsystem progress after their boundaries are reviewed.
- Split `.rentext`, VU programs, descriptors, data, and exception tables into
  reproducible objects. Recover the linker layout and move from the current
  original-metadata hybrid build to a complete linked executable.
- Validate executable replacement in an emulator using the user's local game
  files, and document tested boot/gameplay behavior.

Primary sources for the reused runtime and toolchain are recorded in
[reuse.md](reuse.md). No other project's game source has been copied without a
proven identity and an applicable license.
