# Deimos wordcode interpreter

`func_002CBC48` is the complete 3,312-byte interpreter at `0x002CBC48`.
The observed entry is the script path of the recovered `func_002CCC58` call
dispatcher. It receives a word-addressed code stream and an absolute result
slot in the shared value array, or `-1` to discard the result. No upstream
implementation identity was established. The source reuses the project's
reviewed pool, value, dictionary, table, iterator, and call helpers.

The original opcode table at `0x00448970` contains 36 pointers. Equality and
inequality use separate seven-entry tag tables at `0x00448A00` and
`0x00448A20`. All three complete tables were read from the validated executable,
checked against read-only ELF geometry, and checked for aligned destinations
inside the complete interpreter body. Their full hashes and exact target
addresses are recorded in `config/functions/deimos_interpreter.json`.

| Opcode | Observed operation |
| --- | --- |
| 0 | Continue |
| 1 | Clear tag and subtype, retain payload |
| 2 | Signed 16-bit integer literal, tag 1 |
| 3 | Next-word floating literal, tag 2 |
| 4 | Inline zero-terminated string, tag 3; advance by padded word length |
| 5 | Next-word raw literal, tag 6 |
| 6 | Copy all eight bytes of a value |
| 7–9 | Global lookup, assignment, and assignment from another global |
| 10–13 | Floating addition, subtraction, multiplication, and division |
| 14 | Convert float/string operands to text and concatenate |
| 15 | Logical negation |
| 16–19 | Equality, inequality, less-than, and greater-than |
| 20–21 | Logical conjunction and disjunction |
| 22 | Create table, tag 4 |
| 23–26 | Table lookup/assignment with immediate or value-derived key |
| 27–28 | Create/advance table iterator |
| 29–31 | Absolute word-index jump, jump if true, jump if false |
| 32 | Nested call; next word supplies result destination |
| 33–34 | Return cleared value or copy a complete value to the absolute result |
| 35 | Set the current frame's annotation word |

The low instruction byte selects the opcode. Individual cases preserve the
original operand extraction: packed input indices use unsigned bytes, while
extended result indices and branch targets retain their signed 8-, 16-, or
24-bit extensions. Call argument count uses the unsigned high byte. Program
and value addresses wrap at 32 bits. Unknown opcodes continue execution.
Stream bounds and valid operand records remain the original caller invariants.

Several details affect behavior. Only tag 1 reads payload bits for truth; other
nonzero signed tags are true. String inequality stores the complete `strcmp`
return word, rather than normalizing it to one. Comparison callbacks can change
the global value window, so destinations are reloaded afterwards. Value-derived
table operations keep the original table-slot address across key conversion,
reload that slot's payload after the callback, and use the current window for
later operands and output.

Numeric text conversion preserves the complete 64-bit software-double ABI.
It compares the absolute `fmodf(value, 1.0f)` result against the double encoding
of float `0.01`, then formats with the original `%d` or `%f` string. Source
scalars are reloaded after callbacks, and concatenation reloads the allocated
node's buffer between copy and append.

The two 64-byte formatting scratch areas live across the instruction loop.
Iterator creation reuses the first area: it writes tag 7 and the node pointer,
copies all eight bytes, and leaves subtype bytes untouched. Earlier formatting
can therefore determine the copied subtype. Before any formatting, retail reads
uninitialized stack bytes for this subtype; the reconstruction does not assign
a deterministic first-use value or introduce a zero initialization.
The harness exercises the defined reuse path
reuse with `12.500000`, checking the retained `'.'`/`'5'` bytes. It also covers
every opcode, NaN comparisons, aliased output, callback window/payload changes,
signed extended destinations, complete soft-double formatting arguments,
unsigned call inputs, and returns. Root independently reviewed all 36 original
opcode bodies, formatting, reloads, and the three jump tables without finding
a defect. The committed harness runs with:

```powershell
.venv/Scripts/python.exe tests/native/run_utilities.py --harness deimos_interpreter
```

All 418 native 32-bit checks passed. Integer formatting includes the largest
representable float below signed 32-bit overflow, the exact signed minimum,
and both signed zeros. The target C cast retains the original `trunc.w.s`
operation. Out-of-range conversion and native NaN/comparison tests do not
establish the EE hardware's special-value behavior; native tests use the host
IEEE model.

Both pinned compilers compile the complete C switch. Canonical GCC 3.2.3 emits
2,744 bytes; GCC 2.9 with the reviewed save attribute emits 2,828 bytes. Both
differ from the original and reference compiler-local switch data containing
code-pointer relocations. The current strict linker does not support that
mapping policy. The manifest records `reconstructed`, `link: false`, the complete
unresolved relocation offsets and candidate comparison. No matching code bytes
are claimed. The original full switch is retained for future matching work.
