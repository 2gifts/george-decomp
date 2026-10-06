# Contributor workflow

1. Import the supported disc locally and run `tools/analyze.py --disassemble`.
2. Inspect candidate assembly in `build/functions/`. Review boundaries against
   callers, branch targets, padding, and the complete disassembly; automatically
   detected regions can contain data or split handwritten code.
3. Reconstruct one meaningful function in C/C++. Use address names until an
   identity is supported by evidence. Model only observed structure fields and
   assert their offsets. Preserve widths, signedness, delay-slot behavior, and
   floating-point ordering.
4. Before new work, check licensed upstream runtime/engine implementations.
   Prove an identity through exact bytes or stronger structural evidence, record
   the revision and license, and preserve notices. Similar-looking APIs do not
   prove identical code.
5. Add the function to a batch manifest in `config/functions/`, including original
   address, mapped file offset, code size without alignment padding, original
   byte hash, source path, flags, evidence, and status. Imported runtime work goes
   in `config/runtime_functions.json` and `docs/reuse.md`.
6. Run `tools/verify.py`, then `tools/build.py`. A `matched` status is a regression
   gate: any changed bytes, size, or unresolved relocations fails verification.
   Reconstructed routines can remain nonmatching. Mark a match only after the
   compiler comparison passes, and regenerate `reports/progress.json`.
7. Run the synthetic tooling tests. Check the staged file list before pushing.
   `orig/`, `build/`, and `tools/vendor/` remain local; neither game assets nor
   SDK/compiler binaries belong in commits.

For calls or globals, set `link: true` and compile with `-ffunction-sections`.
Record independently established addresses in `config/symbols/` or the
function's `link_symbols` dictionary. `tools/link_match.py` uses the pinned GNU
linker to resolve every supported relocation at the original function address.
It rejects unknown references and compiler-local constant pools without a
proven data mapping. Do not ignore relocation words to award a source match.

For a compiler-local read-only section, a `link_data` entry must prove its
original address, file offset, full size, and SHA-256. The input section and
linked output must both equal those original bytes. Writable sections,
relocation-bearing data, and pointers outside the proven range are rejected.
These data bytes do not count as recovered executable code.

Select a `compiler_profile` only with an explicit, supported flag recipe and
full-byte evidence. The verifier checks the compiler and child executable
fingerprints before compilation. Candidate versions and per-function matches
do not prove the game's original translation-unit flags; see
[compiler provenance](docs/compiler.md).

`tools/function_index.py --rebuild` indexes the existing spimdisasm output.
Use `--query DScriptMgr` or `--address 0x002CD990` to inspect direct callers,
references, and nearby retained strings. These are navigation hints; confirm
indirect calls and actual boundaries in assembly before reconstructing code.

[Analysis tools](docs/analysis_tools.md) describe the pinned `m2c` draft workflow.
Its output can omit ABI arguments or misinterpret instructions and unions.
Review original words, call signatures, and structure access order before
registering any draft as recovered source.

The denominator includes all three executable code sections, including runtime
and VU code. Preserve separate counts for game C, runtime C, and reused assembly.
An assembly or hybrid executable match is valuable build evidence but does not
claim a completed high-level decompilation.
