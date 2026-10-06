# Compiler profiles

The project uses two candidate GNU compilers. The game's original compiler has
not been identified. `config/compiler_profiles.json` records their executable
hashes, versions, process DLL paths, fixed command prefixes and source provenance.
The default remains `gcc323`; individual functions select `gcc29` only after an
independent exact comparison with the original executable.

| Profile | Candidate | Setup |
| --- | --- | --- |
| `gcc323` | PS2DEV GCC 3.2.3, official 2018-10-19 homebrew release | `python tools/bootstrap_toolchain.py` |
| `gcc29` | Public GNU GCC `2.9-ee-991111`, built for Windows | `python tools/bootstrap_legacy_compiler.py` |

The [reviewed GNU compiler release](https://github.com/2gifts/george-decomp/releases/tag/gcc29-windows-v1) supplies the pinned binary ZIP and complete corresponding source.
Run `python tools/bootstrap_legacy_compiler.py` after installing the default
PS2DEV tools. The bootstrap verifies the archive and all four GNU executable
hashes. Local copies can be installed with `--binary-archive <path>`.

```powershell
python tools/bootstrap_legacy_compiler.py --verify-only
```

The optional compiler ZIP contains the locally built GNU driver, C and C++
backends, preprocessor, and GCC intrinsic headers. The existing pinned PS2DEV
archive supplies newlib headers and the GNU assembler separately. Modern
decompals binutils perform inspection and exact linking. The ZIP contains no
game executable, game assets, SDK implementation, samples, or Havok source.

## Public source and host repairs

The source is [SSXModding's public GNU EE toolchain
repository](https://github.com/SSXModding/ps2-ee-toolchain) at commit
`b595ded606227e93b8c4a447446c1d2ac093827d`. GCC and Bison retain their GNU GPL
notices; linked libiberty retains its GNU LGPL notices. The corresponding source
package contains GNU `ee/gcc`, `ee/libiberty`, `ee/bison`, their shared GNU
configure files, all relevant upstream license texts, and the build recipe.
GNU compiler sources, rather than a proprietary compiler SDK, are compiled.

`tools/patches/gcc29-windows-host.patch` makes three host portability repairs:

- GCC `prefix.c` modifies a mutable path copy rather than a const-qualified pointer.
- Bison uses a relative temporary filename on native Windows.
- libiberty quotes Windows child-process arguments, including spaces and backslashes.

The patch does not modify target instructions, register allocation, scheduling,
calling conventions, or the R5900 backend. The `-B` prefixes select child
programs using paths relative to the repository root; this also avoids the
historical driver's handling of spaces in the child program's first argument.
`-mgas -mno-mips-tfile` selects GNU assembly and disables a missing historical
postpass. Explicit source-directory include paths accommodate the old
preprocessor. These fixed options are recorded in the profile rather than
silently added to source-specific flags.

## Rebuilding on Windows

Run this from the repository root with Python 3, Git, and Windows `tar.exe`:

```powershell
python tools/bootstrap_toolchain.py
python tools/bootstrap_legacy_compiler.py --build --jobs 4
```

The recipe verifies the pinned official PS2DEV archive, extracts its GNU
MinGW/MSYS host tools inside the ignored vendor directory, checks out the pinned
public GNU source, checks the host patch hash, and builds only libiberty, Bison,
and the GCC frontends/driver. It temporarily mounts the workspace as `R:` because
old GNU make cannot use paths containing spaces, and restores the MSYS mount
file afterward. An unrelated existing `R:` drive is rejected. No machine-wide
tool installation is required. Logs and the generated shell recipe are retained
under `build/reuse/gcc29`.

The reference recipe was tested against the existing source build on Windows.
A fresh source build can have different PE executable timestamps and host debug
paths. Its hashes are recorded in `build/reuse/gcc29/local-build-receipt.json`;
they must be reviewed before changing a public compiler profile. The reviewed
binary ZIP provides stable executable fingerprints for reproducible matching.

For manual reconstruction from the corresponding source ZIP, extract its
`ps2-ee-toolchain` directory into `tools/vendor`, apply no additional target
patches, and run the included `recipe/build-compiler.sh` inside the temporary
`R:`/MSYS mount described above. The sources already include the three disclosed
host repairs. The ZIP also includes the Python recipe and the unmodified patch
for auditing.

## Evidence and limits

An initial comparison of 75 registered C functions compiled 74 with the older
candidate: 12 full-byte matches, including two new matches totaling 164 bytes.
Two functions that match with GCC 3.2.3 differ under GCC 2.9, so per-function
profiles are necessary. The remaining failure was an unbound `sqrtf` call.
These observations do not establish one original compiler for the game.

Matches compare the complete compiled function bytes after proven relocations
are linked. Instruction bytes are never masked or rewritten to award a match.
The retail `fmodf` wrapper uses 64-bit callee saves; many game functions use
128-bit saves. Neither compiler's version string alone proves either ABI.

## Linking proven read-only data

`tools/link_match.py` links one function at its original address. A function's
optional `link_data` manifest maps exact compiler section names to their original
`address`, `file_offset`, `size`, and `original_sha256`. The verifier checks the
complete original bytes and read-only ELF geometry before supplying those bytes
and their hash to the linker. Only sections directly referenced by the selected
function can be retained; unused mappings are rejected.

The default requires the complete input section to equal the original bytes and
contain no data relocations. The explicit
`"relocation_policy": "self_r_mips_32"` opt-in permits a nonempty table of
`R_MIPS_32` pointers into that same complete read-only section. Each relocation,
symbol, offset, and signed addend must be bounded and unambiguous. GNU `ld`
resolves the original object without manual byte patches. The complete linked
data must then equal the original proof, and the verifier independently compares
the returned data again. Code matches still require every function byte and no
remaining relocations; mapped data contributes no recovered function bytes.

This policy supports the authentic `_localeconv_r` object's self-contained
`.rodata` at `0x00456C30`. Writable data, mixed relocation types, external or
cross-section data pointers, and unused mappings remain unsupported. A mapping
cannot bypass differing literal bytes or differing resolved pointer values.
