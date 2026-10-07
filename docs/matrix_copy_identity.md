The copy routine captures all 64 input bytes before publishing any output byte,
including aligned shifted overlap. Unsigned-char access preserves arbitrary
object representations, including NaN payloads and signed zero, without float
arithmetic or overlapping memcpy.

The identity routine reuses the unchanged complete matrix_unit method and aligned
MATRIX typedef from PS2SDK commit 120aaba7d4df42e251840fc3e46b4b57308307a8.
The new extracted translation unit retains Naomi Peori's full 2005 notice and
prominent modified-work attribution. It is distributed under Academic Free
License version 2.0; the complete unmodified terms are in
LICENSES/PS2SDK-AFL-2.0.txt. Use or distribution of this licensed source indicates
acceptance of those terms, and recipients must retain its attribution notices.
The upstream whole C/header/license/README hashes and exact method span appear
in the private provenance packet. The new translation unit is an extraction,
not an unchanged import of the complete SDK source file.

Identity equivalence is final memory on ordinary initialized nonvolatile aligned
binary32 float storage. SDK memset/diagonal writes differ from the original's
row3/0/1/2 quad-store order; intermediate observers, atomicity, timing, exceptions
and micro concurrency are excluded. The original derives literal zero/one rows
from VF00; scratch lanes remain unknown until the actual masks define them.
Neither routine claims incidental return-register preservation or original
class/SDK ancestry. Existing float* and GeorgeRotationMatrix* identity declarations
remain unchanged and their ISO-C conflict is explicitly qualified.

The bounded observer executes actual original PCs, validates initialized aligned
storage before mutation, and compares full arenas to an independent initial-byte
snapshot or literal identity words. Copy's unaligned LQ/SQ address flooring and
identity's SQC2 alignment exception are outside the source fixture domain.
Production source, helper closure, whole natural target artifacts, measured ABI,
native commands and real wrong-source controls are captured by the author packet
in build/matrix_copy_identity. Final acceptance and publication are recorded
separately in the canonical function manifest and parent-review receipts.
