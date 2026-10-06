# Tree update and controller context

Eight complete routines in `src/game/tree_updates.c` recover 1,400 original
instruction bytes. Every function compiles and fully links at its original
address in both pinned compiler profiles with no unresolved relocations.
All remain reconstructed: no complete function matches its original bytes.

| Address | Observed operation | Original bytes | GCC 3.2.3 candidate bytes |
| --- | --- | ---: | ---: |
| `002AC820` | Deadline, callback timing and recursive updates | 404 | 340 |
| `002AC9B8` | Construct with saved current context | 340 | 340 |
| `002ACB10` | First controller roots through original accessor | 36 | 32 |
| `002ACB38` | Bootstrap global controller slots | 108 | 108 |
| `002ACBA8` | Destroy both controllers | 68 | 68 |
| `002ACBF0` | Update active roots | 92 | 88 |
| `002ACC50` | Select first controller and update | 176 | 172 |
| `002ACD00` | Select second controller and update | 176 | 172 |

The only target-specific assembly is one narrowly scoped Count-register read
in `george_tree_read_count`, invoked at the two observed positions. The pinned
public EE compiler backend provides no `mfc0` builtin. A volatile `mfc0` with
an output-register constraint and memory clobber expresses hardware access and
preserves the read's position relative to callback and memory operations.
No raw instruction bytes, whole-function assembly fallback or scheduling
patches are used. Deadline flags, callbacks, traversal and cycle arithmetic are
ordinary C. The manifest identifies this hardware primitive explicitly.

`002AC820` first selects its node as the active controller's current node,
decrements remaining time and records elapsed time. For remaining time less than
or equal to zero, it stores zero, sets flag `20000` and clears `10000`. Otherwise
it clears `20000`. The strict comparison keeps the original unordered branch
shape. It reads Count before loading the callback flags, invokes the captured
node/data callback when `10000` is clear or `20000` is set, then reads Count
again. The original sign extensions are followed by `subu` and a word store;
only the wrapped low 32 bits survive, expressed with unsigned subtraction.

After that callback, the routine reloads the active controller. If its current
pointer is null, recursion is skipped. Any nonnull current pointer is cleared;
it need not equal this node. The node's `40000` flag is then reloaded to decide
whether to walk children. The child head is obtained after the callback.
Each immediate successor is captured before recursion; the next link of that
successor is loaded afterwards. A child's stored cycles and the parent's stored
cycles are reloaded after the recursive call, then added modulo 32 bits. Callback
changes to the active controller, children, flags or cycle totals retain their
observed effect.

`002AC9B8` repeats the original bootstrap and constructor sequences inlined in
retail. It captures the current node, clears it, repeats bootstrap, constructs
the node, then restores the captured current node into the freshly loaded
active controller. Restoration also occurs on node allocation failure. An
allocator may replace the active controller, so the new node's insertion and
restoration can target that replacement. Initialization reuses the reviewed
tree source, including wrapped size, priority 2000 and untouched fields.

`002ACC50` and `002ACD00` bootstrap both controller slots, select the first or
second controller and walk its roots. They leave that selection active.
`002ACBF0` walks the currently active controller without bootstrapping. All
three walks capture each successor before calling the recursive updater. A
callback that removes that captured successor and clears its next pointer can
end the pass before later siblings, as in retail. No defensive rescan was added.

Shutdown destroys the first controller, reloads the second slot and destroys
that controller, then clears both slots. The active slot remains untouched.
The source preserves these unusual lifecycle effects instead of introducing
new allocation or null-controller guards.

Pinned upstream m2c supplied seven research drafts; its two unsupported CP0
reads in the updater required manual recovery. Complete original instruction
review resolved that diagnostic and the callback/global reload details.
The small roots wrapper was reconstructed directly from its complete body.
No upstream game source identity is asserted.

`tests/native/tree_updates.c` passed 610 checks with zero failures. It links
actual recovered tree/update/list sources and opts into a deterministic native
counter substitute. Cases cover all deadline/flag combinations, null callbacks,
count wrap and accumulation wrap, callback-created children, active controller
changes, recursion suppression, parent-cycle mutation, saved root successors,
bootstrap allocator mutation, root construction and failure restoration, both
context selectors and shutdown slot mutation. These establish algorithm and
order under synthetic inputs; they do not establish physical CP0 timing or EE
special-value floating behavior. Run
`.venv/Scripts/python.exe tests/native/run_utilities.py --harness tree_updates`.

The root reviewer independently checked all eight complete original instruction
bodies, including the hardware-read order, deadline flags, global context
reloads and captured-successor traversal. No defects were found.
