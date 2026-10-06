# Audited scalar accessors

This batch adds **129 byte-matched C functions**, covering **1,032 original
bytes**. These are complete eight-byte scalar accessors, reconstructed with
one shared C template. Their small size is explicit: a large function count
here does not imply a large portion of gameplay has been recovered.

Each body consists of `jr ra` and exactly one load from `a0` in its return
delay slot. The source preserves the observed load width, sign, signed byte
offset and integer or float return register. Opaque input pointers and address
names avoid asserting original class names, semantic field names or word
values' pointer interpretations. The signed byte/halfword reads return signed
32-bit values; unsigned reads return unsigned 32-bit values; float reads return
the loaded scalar. Each supported profile's generated instructions establish
the target ABI behavior directly.

The initial shape scan found 540 leads. Shape alone was insufficient. The
reproducible audit accepts 130 leads using stronger entry evidence: 88 actual
direct-call targets, plus 42 targets in contiguous zero-adjustment/function
pair arrays whose bounded table addresses are materialized and stored by
original code. One accepted function was already registered elsewhere and
was omitted from this batch. The remaining 410 leads are deferred. Unknown
descriptor records, isolated function pointers and unreferenced shapes are
not registered as decompiled functions.

For every selected address, the manifest records exact original-body SHA-256,
load type and signed offset, entry references and full function boundaries.
Direct edges are validated from encoded original `jal`/`j` instructions.
Dispatch evidence records its data section, zero adjusted-this pair, bounded
contiguous pair region/hash and original `lui`/`addiu`/`sw` instruction addresses
that materialize and store the table pointer. The inventory checks intervening
register writes and permits only a control transfer's single delay slot.
These observations identify referenced entry points without assigning class
identities or claiming an entire enclosing table has been decompiled.

Reproduce the inventory after local original analysis:

```powershell
.venv/Scripts/python.exe tools/accessor_inventory.py
```

This writes only the ignored `build/accessor_inventory.json`. It validates the
supported ELF revision and original generated geometry; it neither creates
source nor upgrades matching status.

Each of the 129 C functions was compiled and linked independently with the
pinned GCC 3.2.3 profile. Every linked ELF function symbol has exactly eight
bytes, equal SHA-256 and no unresolved relocation. Alignment padding is
excluded from the comparison. The source template does not contain inline
assembly or original byte arrays, and each template invocation retains its
own complete comparison gate. Independent review rechecked all 129 original
bodies, recorded references, bounded pair-region hashes and 124 distinct
table-pointer materializations. A separately compiled object and 129 fresh
individual links reproduced all 1,032 matched bytes without unresolved
relocations. No source or entry-evidence defect was found.

The inventory rejects a materialization beginning in a preceding control
transfer's delay slot, and treats `sc`/`scd` results as register overwrites.
Ten synthetic scanner checks cover clobbers and delay-slot boundaries without
requiring original game files:

```powershell
.venv/Scripts/python.exe -m unittest discover -s tests -p test_accessor_inventory.py -v
```
