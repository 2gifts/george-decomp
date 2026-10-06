# Resource ownership methods

Four complete routines reconstruct 452 original bytes and 113 instructions.
They reuse the observed resource prefix and signed virtual-word representation
from the resource geometry batch. Field names describe offsets rather than
asserting original class names.

| Entry | Bytes | Observed behavior |
| --- | ---: | --- |
| `0020EF60` | 104 | Activate a flagged child or increment its count, then notify the fresh holder |
| `0020EFC8` | 60 | Release through the original wrapper, then notify the fresh holder |
| `0020F008` | 104 | Detach a pending child when required, then invoke its fresh virtual release |
| `0020F070` | 184 | Publish the original table, conditionally release/notify, then invoke base release |

`20EF60` requires a valid child. Its `0x80` flag selects the original activation
call; otherwise the unsigned 16-bit count increments with wraparound. The holder
pointer is loaded afterward, so a callback can replace it. A nonzero holder byte
at offset 2 selects notification. `20EFC8` retains a genuine call to `20F008` and
then reloads the holder and notification byte.

The repeated release sequence captures a child and flags at each caller's
original positions. Flags `4` and absence of `0x20` select the detach call. It
then reloads the child, its table pointer and the signed 16-bit adjustment in the
pair at offset `0x10`. The virtual target receives the adjusted child and word
zero. Replacement of the child during detach affects that call. The shared
ordinary C inline body expands naturally in all three compiler recipes; there
is no invented helper binding.

`20F070` publishes the table pointer before reading flags. Bit `0x80` skips
child release and holder notification. Otherwise it captures the child, runs
the repeated release sequence when nonnull, reloads flags for the `0x20` gate,
and reloads the holder before checking its byte. The original mode argument is
retained across calls and supplied to the numeric base-release entry. No
incidental callee return is promoted to a source result.

The complete observed 16-byte readonly prefix at `0x0043A408` and the authentic
LUI/lower address pair are verified. Its second virtual pair has zero adjustment
and target `20F070`; the earlier resource lookup uses that actual pair with word
three. Two further aligned readonly pairs point to `20EFC8` and `20EF60` at
`0x0043A42C` and `0x0043A434`. Their eight-byte pair geometry, zero adjustment and
hashes are verified, while the complete enclosing table and runtime dispatch
remain unproven. The sole encoded incoming JAL is `20EFC8` calling `20F008`,
inside the complete 60-byte caller. These distinct levels of entry evidence are
retained in the metadata.

All four raw/disassembly intervals, terminal JR31 instructions and delay slots,
local branches, nonstack stores and numeric calls are checked. The four
bytes after `20EFC8` belong to alignment rather than its body. Twelve
complete compiler links have zero unresolved relocations and zero byte matches;
the routines remain reconstructed. Root author and independent peer reviewed
all complete original bodies, source, header, model and metadata, independently
reproducing all twelve linked comparisons and the native/guard checks. The
ignored author packet is in `.local/resource_lifecycle`; peer evidence is in
`build/reuse/resource_lifecycle_review`. Registration is approved.

The scoped instruction model reuses the reviewed scalar decoder and exact
byte/halfword stores, adding signed halfword loads for virtual adjustments.
It permits only the four complete bodies, bounded initialized authored memory
and named controlled calls. Actual JR31 returns must reach the selected stop;
taken and untaken ordinary delays are executed and checked, and likely
annulment is preserved. Seven guards cover these limits, alignment, widths,
readonly exclusion and reserved operands.

The 175 fixtures execute 4,834 original instructions, at most 44 per fixture.
Native production and harness compile as separate translation units and pass
94,071 checks of all buffer words, calls and events. Cases cover flag gates,
child absence in guarded routines, count wraparound, negative/positive member
adjustments, holder notification gates and child/flag/holder mutation during
controlled calls. The native table and virtual target are authored substitutes;
original code and table arrays are not exported. The input digest is
`f2ff2710ef1c6abbadf29bee1e0174d165846ae3f8d7bf6d9f33c935a66bcba4`.

Regenerate synthetic fixtures with:

```text
.venv/Scripts/python.exe tools/trace_resource_lifecycle.py --golden-header tests/native/resource_lifecycle_golden.h
```

Inputs must satisfy the original resource, child, table and holder contracts.
Controlled engine effects test the caller's captures and reloads; they do not
implement the engine, allocator, VU state, hardware exceptions or malformed
input behavior. Complete compiled-code comparison remains the exact-match gate.
