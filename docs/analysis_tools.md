# Reviewed m2c candidate workflow

The project uses the existing [m2c decompiler](https://github.com/matt-kempster/m2c),
formerly `mips_to_c`, to produce local research drafts. Its GNU assembly input
and `mipsee-gcc-c` target suit the current spimdisasm output. The upstream target
describes an EE little-endian EABI64 register convention; that target name does
not establish this executable's original compiler or certify every R5900
extension. See the [upstream usage and target documentation](https://github.com/matt-kempster/m2c/blob/708d2d2cb2698f091a92492b328f73b24209f72d/README.md).

The pinned upstream revision is
`708d2d2cb2698f091a92492b328f73b24209f72d`. Upstream declares
[GPL-3.0-only](https://github.com/matt-kempster/m2c/blob/708d2d2cb2698f091a92492b328f73b24209f72d/pyproject.toml);
its [LICENSE](https://github.com/matt-kempster/m2c/blob/708d2d2cb2698f091a92492b328f73b24209f72d/LICENSE)
is preserved in the ignored checkout. No m2c implementation code is copied into
the game sources or wrapper.

## Installation and pin check

From the project root, prepare the ignored checkout with these separate commands:

```powershell
git init tools/vendor/m2c-708d2d2
git -C tools/vendor/m2c-708d2d2 remote add origin https://github.com/matt-kempster/m2c.git
git -C tools/vendor/m2c-708d2d2 fetch --depth 1 origin 708d2d2cb2698f091a92492b328f73b24209f72d
git -C tools/vendor/m2c-708d2d2 checkout --detach 708d2d2cb2698f091a92492b328f73b24209f72d
```

The wrapper checks a SHA-256 digest covering `m2c.py`, the Python files under
`m2c/` and `m2c_pycparser/`, `LICENSE`, and `pyproject.toml`. It sorts POSIX
relative paths, normalizes CRLF to LF, and hashes each UTF-8 path, a zero byte,
file contents, and another zero byte. The expected digest of 48 files is
`ea6f716da03b47479cfc41eea9a57a2f521b61950c73f2b04dd52af1a13271eb`.
The LF-normalized license SHA-256 is
`8ceb4b9ee5adedde47b31e975c1d90c73ad27b6b165a1dcd80c7c545eb65b903`.

The checkout runs directly with the existing project virtual environment.
Ordinary C generation imports only the standard library and m2c's bundled parser;
Graphviz is optional for visualization and is not invoked here. No new project
requirements were needed for the tested workflow.

## Batch and typed-context use

The wrapper selects explicit names or a bounded address range. Range limits use
function start addresses, and at most `--limit` functions are processed. The
default size filter is 64–1024 bytes and the default limit is 25, avoiding batches
of trivial leaves and keeping unsupported routines manageable.

```powershell
.venv/Scripts/python.exe tools/decompile_candidates.py --function func_002CF298 --function func_002D05E0 --output build/m2c/callbacks
.venv/Scripts/python.exe tools/decompile_candidates.py --start 0x002D1000 --end 0x002D5000 --minimum-size 64 --maximum-size 1024 --limit 25 --output build/m2c/script_gameplay
```

After configuring the compiler's PATH as in the normal build, a shared context
can be preprocessed with the project's EE GCC:

```powershell
tools/vendor/ps2dev-20181019/MinGW/msys/1.0/local/ps2dev/ee/bin/ee-gcc.exe -E -P -I include src/game/deimos_callbacks.c -o build/m2c/deimos_context.c
.venv/Scripts/python.exe tools/decompile_candidates.py --function func_002D05E0 --context build/m2c/deimos_context.c --union-field GeorgeDeimosPayload:scalar --output build/m2c/fmod
```

The `--context` option may be repeated. Upstream's `--union-field` option is passed
through so the payload union can be interpreted deliberately. **Inspect all union
accesses:** in the initial typed DFMod draft, m2c selected `.bits` and passed that
integer value directly to the floating-point helper. It emitted no error for
this semantic mistake. Selecting `.scalar` corrected the observed draft. Mixed
pointer and scalar routines still require independent type review.

Each function gets `input.s`, `candidate.c`, and `stderr.txt`. The batch report
records the upstream pin, original executable and function hashes, input/output
hashes, context hashes, body/inventory sizes, normalization records and diagnostic
status. All products are restricted to ignored `build/` or `.local/` within the
workspace, including resolved output paths. No generated game bytes or drafts
belong in Git.

## Review gates and observed limitations

The wrapper validates the executable revision, executable-section geometry, and
every original word recorded in the selected disassembler input. It distinguishes
the disassembler's body boundary from CSV alignment padding and requires any
excluded padding to be zero. These are candidate boundaries until reviewed.
Checking raw word annotations does not by itself prove that the assembly text
assembles to those words; the existing assembly roundtrip and function verifier
provide that separate check.

There are two narrow input normalizations: the annotated word `04 00 00 46`
(`0x46000004`) printed as `c1 0x4` is rendered as `sqrt.s $f0, $f0`. This is the
same instruction encoding. The word `64 00 00 46` (`0x46000064`) printed as
`.word` is rendered as `trunc.w.s $f1, $f0`, as confirmed by the pinned R5900
objdump on the original-address assembly roundtrip and the target compiler's
integer-cast output. The differing EE truncation encoding is also discussed in
the [official GCC R5900 support thread](https://gcc.gnu.org/pipermail/gcc-patches/2013-January/356447.html).
The wrapper neither discards unsupported instructions
nor modifies upstream m2c. Other unsupported operations remain in the draft as
errors; nonzero exit status, stderr, `M2C_ERROR`, and the `second half of f64`
placeholder mark a diagnostic draft.
A zero exit status does not establish semantic correctness.

Independent smoke review covered strlen (`00295050`), streaming CRC (`0029C5F0`),
Deimos hash lookup (`002CD990`), DVDist (`002CF298`), and DFMod (`002D05E0`). The
control flow and lookup sequences were recognizable. The CRC draft still inferred
a signed length and an unknown table type; those need correction from the original
instructions. The untyped callbacks omitted the unused first parameter, which a
shared prototype restored. DVDist initially reported the unsupported square-root
alias; the recorded canonicalization restored that operation. These examples
demonstrate both the tool's usefulness and the need for review.

Before adopting a draft, confirm function boundaries, prototypes, signedness,
pointer and field sizes, delay-slot effects, callbacks and global reloads, all
otherwise-unused calls, and floating-point evaluation order against the original.
Replace unknown-field and error macros with reviewed types and actual behavior.
Then compile the resulting ordinary C and run the existing exact, relocation-
resolved comparison through `tools/link_match.py` and `tools/verify.py`. Generated
drafts, assembly fallbacks, and merely compiling C do not add recovered or matched
progress. The wrapper always records zero additions to both counts and never
writes function manifests.
