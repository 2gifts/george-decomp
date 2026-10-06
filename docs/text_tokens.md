# Text token and name/value helpers

Two complete instruction-reviewed routines recover 1,400 original bytes.
Pinned GCC 3.2.3, GCC 2.9 and GCC 2.9 with the reviewed 128-bit callee-save
attribute all compile and naturally link both entries without remaining
relocations. Neither complete candidate equals the retail bytes, so both remain
reconstructed. The canonical GCC 2.9 recipe does not identify the original
compiler or translation unit.

| Address | Observed operation | Original bytes | GCC 3.2.3 bytes | GCC 2.9 bytes |
| --- | --- | ---: | ---: | ---: |
| `002B3190` | Delimiter-aware quoted token copy and pointer updates | 720 | 724 | 680 |
| `002B3EE0` | Name/value extraction and next-input pointer | 680 | 616 | 568 |

Only the context prefix's observed words are modeled: input base at `+08`,
length at `+10` and line counter at `+18`. The neighboring parser is not included
in this batch. Four complete call sites at `002B36D0`, `002B38D8`, `002B3C9C`
and `002B3D1C` establish the first helper's five integer-register inputs; the
name/value callers at `002B41CC` and `002B423C` use its three inputs and nullable
returned cursor. The input and output pointer cells are read once at entry,
with the output cell captured first, and stored input-first/output-last on exit.

The token helper skips leading space, tab, LF and CR while the source address
is below the wrapped unsigned sum of context base and length. A skipped LF
increments the counter using the character captured before advancing; the
upstream m2c draft incorrectly tested the following byte and was corrected from
instructions. Bounds are checked only during this leading skip. Subsequent
copying follows byte terminators and delimiters. Internal LF increments the
counter and is omitted, while CR is omitted without an increment. Counter
addition wraps at 32 bits.

Modes one and two select distinct delimiter tables; all other values select
the default table. Exactly five pointer cells are examined per outer iteration,
skipping null entries. Each nonnull delimiter calls the already recovered
`00295050` length helper, then the pointer cell is read again for the original
`00393E48` bounded comparison. The observed comparison returns zero for an equal
prefix or equal NUL and takes three integer-register arguments. Its complete
body was inspected for this ABI; no unidentified library implementation is
copied. A changed cell is compared with the length captured from its previous
pointer, and later cells remain fresh reads. An empty delimiter therefore
matches immediately with count zero.

Delimiter checks precede the quote branch. Thus a delimiter at the first byte
after an opening quote stops before quoted scanning. Once that scan starts,
it consumes the entire quoted run without intermediate delimiter calls. Opening
quotes use literal apostrophe/double-quote comparisons; closing bytes and
optionally emitted quotes use a captured three-byte lookup. A scan reaching
NUL without its closing quote still emits that closing byte when requested,
advances past the NUL, and examines the following byte. This unusual behavior
is retained; it requires readable storage beyond that NUL and can resume copying
there. A callback that changes the current byte to NUL is likewise observed
after delimiter calls, including the unquoted default store/advance path.

The name/value helper clears both output starts before reading the first input
byte. It skips leading whitespace, copies the name through the first equals or
whitespace, writes its terminator, and reloads the source byte. Missing values
return null even when a name has already been copied. The separator phase skips
equals and whitespace even after opening a quote, so leading whitespace or
equals inside that quoted value is discarded. Later quoted whitespace is kept.
An unquoted value ends at whitespace or NUL; embedded literal quotes can remain
ordinary value bytes. Empty quoted values return after their closing quote.
Unclosed values return their NUL pointer. The value terminator store precedes
the final fresh source-byte test and conditional quote advance, preserving
destructive input/output aliases.

The original quote lookup at `004476C8` is three readonly bytes in `.rodata`,
file offset `003486C8`, independently identified as the standard NUL/apostrophe/
double-quote lookup; its SHA-256 is
`f9983e1fd65410ec3b362179db67f7a566de4bd42e94b65592304cfaa31e52dc`.
The five accessed pointer words at `003FD258`, `003FD270` and `003FD288` lie in
writable `.data`. No original delimiter strings or lookup arrays are embedded
in recovered source. The tests use synthetic delimiter words and ordinary
ASCII quote constants. No public implementation identity was established for
these custom helpers, so they are newly reconstructed while reusing existing
length declarations and original numeric comparison bindings.

`tests/native/text_tokens.c` passes 22,855 checks. Its 172 instruction-derived
synthetic fixtures cover complete buffer contents, leading bounds, counter
wrap, all mode classes, quote retention, high bytes, aliases, a shared pointer
cell, delimiter holes, changed pointer cells/line counter/current source byte
around controlled calls, and old-length/new-pointer comparison. Separate
hand-written expected examples cover first-quoted delimiters, resuming beyond
an unmatched quote's NUL, empty delimiter and value strings, whitespace trimming,
missing/unclosed values, and initial output/input aliases.

`tools/trace_text_tokens.py` reads the hash-validated local executable at runtime
and permits only these two complete instruction ranges. It reuses the bounded
scalar decoder and adds the observed byte loads/stores, likely/ordinary branches,
unsigned comparisons, shifts, conditional moves and XOR immediate. Reads and
writes require initialized synthetic storage; unknown instructions/calls and
excessive execution fail. Length and comparison use controlled byte substitutes,
not an emulator of the original vectorized library. Six focused decoder tests
check signed byte extension, byte-store bounds, branch/annul targets, signed
shifts, unsigned comparisons, conditional moves, count-zero comparison and
strict scope limits.

The parent independently reviewed all 350 original instructions, both source
bodies, header layout and the scoped trace's added operations, and reran the
22,855 native checks without defects. That review identified a decoder
representation inconsistency: LB/SRA had retained negative Python integers while
the inherited arithmetic used low 32-bit words. The corrected decoder normalizes
both operations and fixture mode registers; an LB-to-DADDU-to-BEQ regression
passes, and all 172 regenerated fixtures remain byte-identical with SHA-256
`d104ae3ac61e36381a6948fbac45a01ecd98c4ee2c9a0885fca3bc01774ed493`.

All paths retain retail's valid-buffer and terminating-read requirements. There
are no output capacities or guards in these helpers. A forward output alias
can overwrite its own input terminator and continue out of bounds; such paths
are excluded from executable fixtures instead of adding a new guard. The trace
and native tests make no hardware timing, exceptional-access or stack-exposure
claim. Only synthetic inputs/outputs are tracked, without original instructions
or assets.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness text_tokens`.
Regenerate fixtures with `.venv/Scripts/python.exe tools/trace_text_tokens.py
--golden-header tests/native/text_tokens_golden.h`. Run scoped decoder tests with
`.venv/Scripts/python.exe tests/test_trace_text_tokens.py`.
