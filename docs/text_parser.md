# Connected text parser

`002B3460` reconstructs the complete 2,684-byte body, all 671 instructions and
its return delay slot. Four following alignment bytes are independently checked
zero and excluded. The original body SHA-256 is
`3c3123998612e98f1ccac894f0b77ce3ee2f4ab514a3b00b06bdcd8a1b455b93`.
It naturally links at its original address under all three standard recipes,
with no remaining relocations. GCC 3.2.3 emits 2,884 bytes; GCC 2.9 and the
reviewed callee-save attribute variant emit 2,716 bytes. None matches the full
retail bytes. The source remains reconstructed, with no assembly fallback or
byte patching. The compiler recipes do not identify retail's compiler.

The new header reuses the existing offset-asserted `0x4030` text context and
8-byte tag records. The function takes context/table pointers in two integer
registers. All thirteen direct callers were inspected: twelve application
callers consume the context code at `+14`, and the reviewed depth helper at
`002B4538` consumes count words; none consumes the parser's return register.
The source therefore declares a void return. The helper calls retain their
original addresses: reviewed token copier `002B3190`, reviewed in-place
lowercase `002B45A0`, and case-insensitive comparison `003983E8`. The two original
inline name-to-code scans share one ordinary C macro in this source, retaining
their original comparison calls and successful code reloads. They follow the
same already reviewed sentinel behavior as `002B44A8` without adding a new
engine call. No new library implementation or original data is imported.

The complete original text section contains exactly thirteen direct JALs to
this entry. Ten application calls supply fixed tables in writable `.data`;
their accessed prefixes end at `0x8000DEAD`. Every preceding record has a
non-null NUL-terminated name in original readonly storage. These prefixes are
evidence for the record ABI and termination, not asserted complete global
object extents. Full prefix and name hashes are retained in ignored local
research. Two other calls reload their table through a dynamic structure at
`0023A890` and `0023AE50`; the depth helper passes NULL. No original tables or
names are copied into this batch.

| Table address | Records through terminal sentinel | Prefix bytes |
| --- | ---: | ---: |
| `003F8970` | 4 | 32 |
| `003F8C60` | 4 | 32 |
| `003F9AE0` | 4 | 32 |
| `003F9AA8` | 4 | 32 |
| `003F9F58` | 29 | 232 |
| `003FA190` | 3 | 24 |
| `003FA270` | 6 | 48 |
| `003FA2A0` | 4 | 32 |
| `003FA650` | 8 | 64 |
| `003FA718` | 6 | 48 |

The first phase captures the source cursor, raw buffer and initial wrapped
base-plus-length bound, then clears raw byte zero. If the captured cursor is
already at or beyond that unsigned bound, it sets EOF at `+102C` and returns:
the other strings, count words, code, cursor and depth remain untouched. Later
whitespace loops read the current byte before their bound test; each LF
increments the context's line word after advancing the cursor. The current LF
is counted, rather than the following byte suggested by the initial m2c draft.
Comment scanning captures one end address, advances through `<!--...-->`, and
counts its LF bytes. It also performs the observed closing-marker lookahead
after reaching that bound. Subsequent whitespace bounds reload base and length.

An opening tag increments its local opening counter, testing the signed value
from before that increment. The first opening uses the token copier with mode
two and retained quote characters, appends `/`, `?` and `>` bytes while within
fresh bounds, skips whitespace, and copies text up to NUL, `<` or the end bound.
It then completes the raw fragment. An initial closing tag copies its first
two bytes in the observed order, invokes the same token copier for the name,
copies one following byte unconditionally and skips whitespace. Bare `/>` and
`?>` pairs also finish the fragment. A further opening/closing encounter can
finish without consuming that next tag. Ordinary unmatched input in the outer
scan does not advance: terminating parser input remains a caller precondition.
No invented progress or malformed-input guard changes that behavior.

The raw fragment is terminated, the source cursor is stored, and flag bit zero
optionally lowercases raw text. EOF is checked against freshly loaded base and
length after that call. The parser then clears only the first bytes of name,
attributes and content, resets the code and per-fragment open/close/special
words, and parses its raw fragment. Opening names skip qualifying `<` bytes and
whitespace, copy up to whitespace, `>` or `/`, then invoke the token copier for
attributes and content. Its scratch cursor is retained across each call. The
special word is set for `<?` or `<!`; it is not substituted for ordinary tag
counting. Standalone closing names deliberately include their leading `/`:
the branch-likely delay slot at `002B3DF8` is annulled on this path, so the copy
starts at the original cursor plus one. The copy is a do/while ending before
`>`. Table success reloads the current record code after comparison, and
failure advances before reading the next record. The final depth calculation
uses wrapped word subtraction/addition: previous depth minus closes plus opens.
Comparison-side changes to those words and to the saved source cursor remain
visible in the source.

Retail has no independent raw/name/attribute/content capacity checks. Valid
caller storage must accommodate all copied bytes and terminators in each
observed 4-KiB buffer, including the extra raw/closing-byte stores. Bounded
scans still read the current byte before checking bounds, and several paths
read one through three lookahead bytes. The caller's declared input length is
not a universal access limit for the token copier or the scratch-phase name
loops. NUL-terminated token strings, readable lookahead, valid terminated tag
tables and writable destination storage remain preconditions; oversized or
nonterminating malformed strings are not repaired. Optional lowercase retains
the reviewed signed character-table index precondition: ASCII and byte `FF`
are supported by the proven table, while `80` through `FE` access before it.

The token copier compares scratch cursors against the original input window
even when invoked on internal context buffers. The relative addresses can
therefore change leading-whitespace skipping. Tests preserve the decoder's
input-below-context arrangement explicitly in one native storage struct and
also exercise input within all four context buffers. This models the observed
address predicates without asserting a universal heap/disc allocation order.

`tools/trace_text_parser.py` reads the hash-validated original instructions at
runtime. It executes the complete parser and both original recovered helper
bodies through the existing scoped integer/byte decoder, adding only BGTZ,
ANDI and wrapped SUBU. Every initialized memory read/write and code range is
bounded; unknown operations/calls fail. One 120,000-instruction budget includes
all helper instructions and delay slots. Controlled length, bounded comparison
and ASCII case-insensitive comparison calls permit explicit callback mutations;
these are substitutes, not proofs of library vector/hardware behavior. No
original instruction arrays, parser assets or pointer tables are tracked.

The generated native fixtures contain synthetic inputs and sparse expected
context-byte changes. All 240 cases execute 272,391 original instructions in
total, with a maximum of 3,062 per case. The native 32-bit harness compares all
`0x4030` context bytes, its full 256-byte input buffer and each controlled call
count: 4,005,840 checks pass. Cases cover quote-preserving attributes, text,
self-closing/closing/special tags, comment chains and bounded unterminated
comments, line/depth wrap, sentinels, `/tag` lookup, early EOF with old state,
five repeated calls, four input-storage aliases, callback code/next-record/
depth/source-cursor changes and optional lowercase including byte `FF`.
Six focused unit tests cover the newly required decoder operations, signed
branch interpretation, wrapping, invalid encodings, total budget and rejection
of unknown code/memory/calls. These checks model scalar low-word behavior and
initialized synthetic storage; they do not assert a complete R5900 emulator,
hardware exceptions, timing or invalid-memory behavior.

The official pinned m2c draft was used only as ignored research evidence.
Manual instruction review corrected its line-counting, pre-increment predicate,
two-byte copies, standalone closing name and final cursor loop interpretations.

Parent independently reviewed all 671 original instructions, complete source
and header, scoped decoder, native harness and documentation, finding no defect.
It independently regenerated all 240 fixtures byte-identically, reran all
4,005,840 native checks and six decoder guards, and verified all thirteen direct
JAL references, ten complete writable table prefixes and 62 readonly name hashes.
Tracked metadata records the actual JAL addresses and full prefix geometry/hash
proof without copying original table/name bytes. The synthetic golden header
SHA-256 is `aa895d8be5d59f74fc7a0699f18dfbb3efdb460d2d26a7b66a605533022fa440`.
