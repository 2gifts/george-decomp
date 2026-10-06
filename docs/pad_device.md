# Pad-device connection and packet wrapper

Four complete routines recover 1,820 original instruction bytes. The nullable
delta getter at `002B2ED8` matches all 72 bytes after original-address linking
under both pinned compiler profiles. The other three remain reconstructed.

| Address | Observed operation | Original bytes | Canonical candidate bytes |
| --- | --- | ---: | ---: |
| `002B2800` | Connection state machine and packet decoder | 1,376 | 1,360, unresolved switch data |
| `002B2D60` | Signed motor gate and SDK sender | 68 | 68, differing code |
| `002B2DA8` | Initialize wrapper and open SDK port | 304 | 296 |
| `002B2ED8` | Read four nullable axis deltas | 72 | 72, exact |

The main reader uses a natural C switch. Both compiler profiles produce local
readonly code-address tables that the conservative linker cannot map; it is
registered as `reconstructed`, `link:false`. Its relocation offsets and full
object-byte difference remain visible. No opcode is patched, masked or omitted
to award a match. The three other complete functions link with no unresolved
relocations. Official pinned m2c drafts assisted recovery, including a main
draft supplied with independently validated internal labels and the derived
table in ignored research files. The tracked implementation was typed and
reviewed against complete instructions.

The original switch table occupies 78 words at `00447530`, file offset
`00348530` in `.rodata`, with SHA-256
`0c947dda92650b8efe1c2dd22551a487e973e4511436c840c4fdf1ce0bbae436`.
Its complete grouped destinations are recorded in the manifest:

| State | Original target | Observed transition |
| --- | --- | --- |
| 0 | `002B28AC` | Query primary and optional positive extended mode |
| 40 | `002B2934` | Query extended mode, then enter common main-mode path |
| 41 | `002B295C` | Set main mode with integer inputs 1 and 3 |
| 42 | `002B2978` | Two request queries; final zero establishes kind 2 |
| 70 | `002B29C0` | Actuator information/alignment and request query |
| 71 | `002B2A10` | Two request queries and fresh-state increment |
| 72 | `002B2A50` | Pressure request selects state 76 or 99 |
| 76 | `002B2A80` | Pressure information, then conditional increment |
| 77 | `002B2AA8` | Two request queries; final zero establishes kind 3 |
| All other table indices | `002B2AF0` | Shared default continuation |

States outside unsigned 0 through 77 also take that default. Query results keep
the original signed and exact comparisons: an extended mode replaces the
primary only when at least one, while several SDK return gates accept any
nonzero value. Request calls are deliberately repeated instead of sharing one
result. State and port/slot fields are reloaded after callbacks. State increments,
decrements and the update counter retain wrapped 32-bit arithmetic.

The shared record is refined to the observed 384-byte layout: a 256-byte
SDK-owned DMA prefix, button counters, port/slot, connection and kind words,
four signed axes, four signed 16-bit deltas, twelve pressure bytes, and two
six-byte motor/actuator arrays. Offset/size assertions preserve the previous
`pad_input` ABI. Initialization clears only the observed fields; kind, two
padding bytes and the suffix remain untouched. Motor/actuator initialization
interleaves ascending stores; pressure clears in reverse order. One-time setup
returns the captured SDK initialization result, or zero when setup was already
done. The original port and slot arguments survive callbacks changing record
fields. The open callback runs before the final word and button-counter stores.

The reader accepts SDK states 2 and 6 for packets. Other SDK states return zero,
copy the current button word when requested, zero requested scalar axes and
leave pressure outputs untouched. For positive kind it captures previous buttons
before the SDK read; read failure returns immediately without writing outputs.
Packets decode sixteen active-low buttons and update accumulated/toggled masks.
Kind at least two converts raw byte axes around 128, captures their signed
deltas, then updates the stored axes. Nullable outputs follow in order. Kind 3
with packet marker zero also emits pressure and signed deltas through an
interleaved scalar loop; shifted destinations can affect later old-pressure
reads. The final packet-type and kind reads remain after those outputs.

Retail does **not initialize the local packet** when SDK state is 2 or 6 but
kind is nonpositive and skips the SDK read. The final packet-type read is then
indeterminate; output aliases that change kind can expose further packet reads.
The C reconstruction preserves this first-use path without inventing zero bytes.
Deterministic native fixtures avoid those indeterminate packet reads. Initialization
also preserves kind, as required by the original stores rather than a fabricated
default.

The motor sender gates on full signed kind at least two. It derives the first
motor byte from full signed positivity, narrows the second to its low byte, and
leaves four remaining bytes intact before the SDK call. The delta getter reloads
each source just before its nullable output store; shifted aliases propagate.

Twelve SDK-facing implementations retain their original numeric addresses and
observed argument shapes. Their source and library identity are not imported
or counted by this engine-wrapper batch. `pad_input` reuses this header and its
existing original-address calls.

`tests/native/pad_device.c` passes 4,047 checks. It covers all state cases and
default values, signed SDK return gates, repeated queries, callback changes,
wrapped state increments, initialization/open ordering, untouched bytes,
buttons/axes/pressure decoding, read failures, shifted pressure/delta/getter
aliases, and motor sign/narrowing. It compiles the actual recovered C with
controlled SDK substitutes and embeds no original bytes. The connected pad-input
harness still passes 610,739 checks, and its target candidate code is unchanged
after the shared header refinement.

Run `.venv/Scripts/python.exe tests/native/run_utilities.py --harness pad_device`.

An independent review compared all four complete original instruction bodies
and the shared record header. It found no defects in callback reloads, signed
SDK return gates, axis/pressure aliases, initialization field preservation or
the documented indeterminate local-packet paths.
