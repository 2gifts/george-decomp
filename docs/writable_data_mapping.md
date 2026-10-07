# Whole initialized writable data linking

The optional `link_writable_data` declaration lets a function link an entire
compiler-local initialized word object at its proven original address. It
supports `.data` and `.data.*` only. The object remains writable PROGBITS
with flags3 in the real linked ELF, including a zero initializer.

Each declaration supplies the whole item's file offset, size, address and
SHA-256; the authentic containing original writable section and its entire
SHA-256; the compiler-local object symbol; and actual original code pointer
bindings. The existing narrow LUI/ADDIU pointer grammar is reused unchanged.
The compiler object must cover its complete input section with one local
object symbol, compatible word alignment, identical whole bytes and no
initializer relocations. Subobjects, aliases, shared sections, unsupported
flags and overlap with any other mapped storage or selected code are rejected.

The backend records `mapped_writable_data` separately from readonly and
NOBITS mappings. GNU ld must retain the complete writable bytes, geometry
and symbol. Verification checks the actual output; the hybrid builder checks
it again against the canonical declaration before substituting code. The
hybrid retains original data. Without the option, existing report fields,
method description, planner results and rejection messages stay unchanged.

Parent and independent review passed98 focused tests:83 existing and15 new.
Synthetic target links cover a complete24-byte object, a four-byte zero
initializer and combined readonly/writable storage. Negative cases exercise
whole-byte mutations, extents, symbols, flags, initializer relocations,
pointer/addend bounds, overlap, missing reports and actual linked-ELF tampering.
Six entire legacy default outputs remained byte-identical.

Earlier proof inventories remain immutable. Exact complete predecessor
tools are retained in private version archives and in Git history at
`f41340fd102a2bf250ff3ff9de8d3d36a6c53b21`. Historical replay actually executes
those versions, logging their real archive paths and whole hashes; canonical
filenames supply path context only. Both replay-parent preservation reads
and producer-child imports/reads use verified archived bytes. The full prior
Query3 replay reproduces ten dictionaries,38 RAW artifact pairs and12 whole
target ELFs. Changed live tools are not described as unchanged historical
inputs, and no frozen index or hash is substituted or regenerated.

This tooling addition registers no game/library candidate or data mapping
and adds no source function, matching byte, helper or data credit. Runtime
initialization, pointer-bearing initializers, shared literal pools and broader
hardware behavior remain outside this capability.
