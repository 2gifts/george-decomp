# Reusing complete duplicate bodies

Thirty-two original functions are complete byte duplicates of already reviewed,
matched project C. Shared typed macros in `include/george/function_templates.h`
reuse those bodies in the original entries and `src/game/duplicate_helpers.c`.
All thirty-two entries independently compile and link at their own original
addresses, reproducing 896 bytes exactly. They add source coverage for repeated
helpers; they do not represent thirty-two newly discovered algorithms.

The reused bodies set/clear bit zero at offset18, update a positive scalar while
allowing negative overshoot, query bit zero at offset1C, or forward a pointer to
the already recovered free routine. Types describe observed numeric prefixes;
the identical code does not establish a common original class name.

Each manifest records its template source, complete original hash, size, target
address, compiler profile and reviewed entry evidence. Forwarders additionally
have their entire seven-instruction standalone stack frame, fixed direct call,
return and stack restoration decoded. Alignment words are excluded. Original
template entries were recompiled after the shared-macro refactor and their
existing exact-match regression gates still pass. No assembly fallback,
relocation masking or function-range overlap earns credit.

`tools/find_duplicates.py` conservatively inventories further candidates in
ignored `build/duplicate_candidates.json`. It validates the original executable,
code-section geometry, complete template fingerprint, consecutive disassembler
instruction addresses and exact endlabel. It reports heuristic entries and
known callers but adds no source or progress automatically. Signatures and
entries require review, followed by real complete compile/link comparisons.
