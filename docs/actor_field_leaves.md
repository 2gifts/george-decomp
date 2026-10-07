Eleven scalar actor field helpers are represented with distinct semantic C
symbols. Their offsets describe consumed storage, not whole actor classes or
original declared prototypes. Existing numerical declarations remain intact:
some callers declare updates void, and one existing mask declaration uses a
32-bit argument where other declarations use 64 bits. This package does not
claim to repair those cross-translation-unit C interfaces.

Word loads sign-extend to the lower 64 bits before masking. Predicates at
18FC88 and 18FCF0 return a Boolean; 1BA5C8 returns the raw intersection. Mask
updates capture the full result, store only its low 32 bits, and return the
captured result. Count updates use unsigned32 addition/subtraction and return
the signed interpretation of the updated word. The conventional GNU `(s32)`
conversion outside the signed range is implementation-defined; this package
measures the two's-complement target/native contract, including 0, 7FFFFFFF,
80000000 and FFFFFFFF. No signed overflow or ownership/underflow guard is added.

The bounded observer reads complete original instructions at the eleven real
addresses. Its exact word, family, memory, alignment, initialized-byte, return
target and eight-instruction budget checks precede mutation. Arithmetic is
delegated to the unchanged RegistryTrace decoder, and actual delay execution
uses unchanged Trace.run. The native harness uses a constructed typed storage
object with 32-bit scalar fields and a 64-bit field at 190, and compares every
byte plus the full lower64 returned result. This is a GNU32 initialized aligned
memory counterpart, not a claim of original class identity, upper128 state,
hardware timing/exceptions, concurrency, or portable arbitrary pointer casts.

The author proof package and later canonical acceptance receipt are separate.
Two fixed production and declaration-only ABI recipes stop for a measured
symbol/argument/return gate before linking. Whole objects, emitted dependencies,
failures and subsequent whole comparisons are retained without source tuning.
The earlier source-free 151-input packet and interface proposal stay immutable;
the parent source gate separately records the conventional conversion refinement.
