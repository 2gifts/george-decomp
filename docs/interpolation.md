# Component interpolation

`002ADC80` is a complete 264-byte helper. Its interleaved records contain one
key followed by the requested number of floating components. The function
copies first/last components with the existing runtime `memcpy` entry when the
key meets those endpoints. Otherwise it scans adjacent records, calculates
`t = (key - left_key) / (right_key - left_key)`, and writes each component as
`left * (1 - t) + right * t`, preserving both multiplies and their order.

The source retains the original comparison graph. An unordered key can pass
through without any output stores; an unordered intermediate key can instead
produce an unordered blend in a later interval. Each iteration loads both
component values before its output store. Inputs are not copied in advance, so
output overlapping a later component can affect that later input. Nonpositive
component count suppresses blend stores; endpoint copy sizes and address
products retain their low 32 bits. Record validity and counts retain the
original caller contract.

Root and an independent reviewer checked the full instruction sequence, argument
registers, branches, loads/stores and final return delay slot. A checked-in
32-bit native harness passes 30 checks covering endpoints, multiple intervals,
zero components, signed zero, overlapping output and unordered inputs. These
float checks use the host IEEE model; they do not independently prove EE special
value behavior. The C operations preserve the original target comparisons and
arithmetic order.

The pinned GCC 3.2.3 candidate emits 264 bytes with different instruction ordering;
GCC 2.9 emits 240 bytes. Every external reference links to its proven original
address. The helper remains reconstructed, with no matching progress awarded.
