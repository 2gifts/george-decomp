# Six-face geometry classifiers

`002A4808` (228 bytes) and `002A48F0` (164 bytes) recover the complete
frame and sphere classifiers for six 28-byte faces. The parent and independent
reviewer read all 98 original instructions, the complete five-face counterparts,
shared source, headers and native harness. Entire original bodies differ only
in three frame immediates and two sphere immediates: face count five becomes six,
and the last face eligible for the crossing bit moves from index three to four.
The ordinary C templates in `geometry_classify_frame_template.h` therefore serve
both families. Numeric engine calls, captured inputs and layouts are reused.

Frame classification captures the frame pointer once. Helper result two returns
zero immediately; result zero marks a crossing only through index four, result
one retains the observed inside bit, and other full-word results continue.
Sphere classification captures radius and XYZ once and evaluates each dot product
in the original order. Both radius comparisons are strict. The sixth face can
reject but does not mark a crossing. No additional validation is invented.

Both complete bounds, return delays and excluded four-byte alignments are
verified against raw ELF bytes and disassembly. Fourteen local branches stay
inside their owning bodies. Two real entry JALs in the complete 208-byte
`0020ECA0` caller corroborate these entries. The frame's original numeric
`0029E098` call remains intact. No literal pool or table data receives an award.

All six standard compiler candidates link naturally with zero remaining
relocations and zero exact matches. The sphere GCC 2.9 candidate has the original
164-byte size but differs in nine bytes. Both routines remain reconstructed.
All 15 earlier whole comparisons, including compiled hashes, reproduce unchanged
after extracting the shared template.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness geometry_classify6`.
The independent native models pass 39,255 checks: all 4,096 combinations of
six helper results, callback arguments and early exits; 4,096 finite sphere
fixtures with exactly representable dyadic arithmetic and a double oracle; and
strict radius boundaries for every face. The existing five-face harness still
passes 94,656 checks. The host uses `-ffloat-store`; these finite fixtures and
controlled helper observations do not establish EE FCR, general special-value
arithmetic or hardware behavior. Complete target-byte comparison remains the
exact-match gate.
