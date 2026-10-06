# Intrusive list helpers

Twelve routines recover700 original bytes. The two insertion helpers exactly
match complete retail code under GCC2.9 (56 bytes); the other ten remain
reconstructed. Eight are independently reconstructed in `src/game/list.c`.
Four reuse licensed AROS algorithms in the separate `src/game/list_aros.c`.

The observed header is twelve bytes: head pointer, null tail word and tail
predecessor. The head sentinel overlaps the first two words; the tail sentinel
overlaps the latter two. Nodes need only an eight-byte next/previous prefix.
This is a layout correspondence, not evidence of Papaya's original source origin.

AROS AddHead, AddTail, RemHead and Remove were adapted from the official
[AROS source at revision e8e543e6ca866e26671c8f586d545f80609ef3dd](https://github.com/aros-development-team/AROS/tree/e8e543e6ca866e26671c8f586d545f80609ef3dd/rom/exec).
The dedicated adapter file retains the AROS notices and dated modifications and
is under the AROS Public License1.1; its complete source is distributed here.
`LICENSES/AROS-Public-License-1.1.txt` preserves that license. Changes adapt the
observed structures/entry points, remove AROS library macros, retain original
field reloads, compare the explicit tail sentinel when removing the head and
clear removed links. The head guard agrees for valid lists and can differ from
AROS on malformed lists. The upstream implementation was compared against the
complete retail instruction sequence before adapting it.

The other routines initialize the header, splice source at destination head or
tail then reset source, insert beside a supplied node, obtain a successor while
excluding the null-next tail sentinel, follow predecessor links to the root,
and return the original validation diagnostic or null. The validator checks
header nullness, endpoints, backward chain consistency and correct terminal
sentinel. Cyclic/corrupt memory and invalid input guards retain original behavior;
no new bounds or loop limits are introduced. Source expressions preserve header
reloads around stores so overlapping sentinels work correctly.

| Address | Retail bytes | Candidate compiler | Candidate bytes | Exact |
| --- | ---: | --- | ---: | --- |
| `002AD9A8` | 20 | gcc323 | 20 | No |
| `002AD9C0` | 32 | gcc323 | 32 | No |
| `002AD9E0` | 28 | gcc323 | 28 | No |
| `002ADA00` | 48 | gcc323 | 48 | No |
| `002ADA30` | 88 | gcc323 | 84 | No |
| `002ADA88` | 88 | gcc29 | 88 | No |
| `002ADAE0` | 36 | gcc29 | 36 | No |
| `002ADB08` | 28 | gcc29 | 28 | Yes |
| `002ADB28` | 28 | gcc29 | 28 | Yes |
| `002ADB48` | 32 | gcc323 | 32 | No |
| `002ADB70` | 224 | gcc323 | 228 | No |
| `002ADC50` | 48 | gcc323 | 44 | No |

A focused native32-bit harness passed934 checks with no failures: empty and
populated lists, mixed head/tail insertion, insertion beside nodes, cleared
links after both removal forms, every0..3-node source/destination append/prepend
combination, successor/root walks and all seven diagnostic return cases.
Both pinned compiler profiles were compared once for this coherent batch;
no assembly changes, relocation masking or padded comparisons establish matches.
