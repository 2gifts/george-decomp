# GNU libio Bigint helpers

Six complete functions, 1,768 bytes, reuse the entire unchanged GNU `ee/libio/floatconv.c` at public Git revision `b595ded606227e93b8c4a447446c1d2ac093827d`. Full private headers and the genuine existing generated configuration are reused. Parent and distinct final review passed; all six complete GCC2.9 functions are canonically registered as matched. The immutable author packet remains reconstructed to preserve its original evidence.

| Entry | Method | Original bytes | GCC 2.9 whole result | GCC 3.2.3 bytes / differences |
| --- | --- | ---: | --- | --- |
| 0035E9E0 | Brealloc | 116 | identical | 128 / 102 |
| 0035EA58 | multadd | 208 | identical | 196 / 173 |
| 0035F090 | lshift | 380 | identical | 340 / 325 |
| 0035F278 | diff | 400 | identical | 388 / 342 |
| 0035F4A0 | b2d | 300 | identical | 332 / 221 |
| 0035F5D0 | d2b | 364 | identical | 376 / 306 |

All twelve selected links use actual complete local STT_FUNC symbols and ordinary fixed recipes. All sixteen selected/ABI ET_EXEC functions resolve without remaining relocations or allocated noncode sections. No mapping, cropping, padding, binary patch, static visibility change, helper/data credit or deferred strtod/dtoa award is used. Original boundaries include actual JR31 and delay; four observed four-byte scanner pads remain excluded. The immutable source-free survey supplies five-executable reference inventory, 38 actual selected references, full caller geometry and explicitly bounded caller windows, without claiming whole large-caller semantics.

The real target ABI proves pointer/int/unsigned limb32, long64, short16, double64, Bigint52 and a 20-byte header; next/k/maxwds/on_stack/sign/wds/x offsets are 0/4/8/12/14/16/20. Sign and on_stack are short fields, unlike the other published newlib Bigint variant. Four complete actual soft-double ABI callers verify b2d's pointer/exponent parameters and 64-bit GPR return, and d2b's incoming GPR5 double, GPR6/7 output pointers and GPR2 pointer result. No artificial float prototype or FPU observation is introduced.

The original compile placed a genuine GCC 3.2.3 internal float.h fallback after profile includes. Canonical compile flags precede profile includes, so a single parent-approved `-idirafter` lookup adapter supplies only that genuine fallback directory. Four complete source/ABI objects and both actual source18/ABI19 prerequisite lists remain RAW identical. The initial missing-float.h failure and the observer-only relative include filename ECOFF difference are retained. Canonical ABI observers retain their original absolute compiler-input/include spellings. The primary source and all genuine headers remain unmodified.

The strict original observer executes all six complete original ranges and six genuine original support bodies at actual PCs. It delegates arithmetic to the unchanged reviewed RegistryTrace scalar decoder and keeps raw fetch equality, initialized/aligned owned memory, local pre-mutation budget, branch/delay guards and actual return checking. It generates 562 fixtures, 59,291 instructions, maximum 515; 441 of 442 selected words execute. The only unvisited selected word is unreachable internal NOP 0035F1C4. Every selected actual conditional branch has both outcomes; unconditional branches are not given fabricated alternatives.

Native code includes the entire unchanged primary, renaming only malloc/free to controlled allocation/release hooks. Every one of 2,048 arena words, return bits and callback events is compared, including untouched tail words. The 409,718-byte public golden represents deterministic initial words plus exact sparse changed-word ledgers, not output hashes. All 1,153,190 checks pass. Valid initialized capacities 8/16/32, disjoint GNU typed/tail-array storage, finite nonzero packed-bit d2b and normalized nonzero b2d are tested. Target long64/native long32 is explicit; consumed fields and bit lanes are compile-time checked. Native compilation retains two genuine warnings in unexecuted deferred _IO_dtoa; it is not described as warning-free.

An independent Python integer/packed-bit oracle supplies 1,138 checks. Six m=65,537 stress fixtures retain defined unsigned partial-product wrapping, but are excluded from an unrestricted mathematical product claim. No general FP, COP1, FCR/exception, arbitrary overlap, heap, OS or allocation-failure fidelity is claimed. Controlled allocation is nonnull; free observes capture/order and does not release arena substorage.

The initial source-free multadd prose incorrectly said fresh wds: original LW 0035EA78 captures wds before Brealloc and retains that index/count after callbacks. Fixture 71 changes the copied new wds during controlled release. A complete-primary one-expression wrong reload successfully compiles/links and fails the real arena comparison (4 instead of 9), while the unchanged source passes. Nine fail-closed guard tests pass; absence simulation has five passes and four skips. Initial oracle, import/include, result-key and proof stdout-assertion failures remain explicit historical evidence.

Source/provenance, commands and complete artifacts are under build/mprec_second. Self/peer replay uses the full actual original-at-link canonical binding snapshot, with later unrelated additions permitted only when every old binding remains unchanged. Whole target/native objects, target ET_EXECs, observations, golden and stdout remain strict RAW. Complete native PEs may vary only in their parsed timestamp/checksum fields after read-only Windows ImageHlp validation; both raw images and all differences must be retained. No binary bytes are normalized or patched.

The author reviewed all 442 selected original instructions, the full unchanged primary and active source/header/layout graph. The separate source-free parent/distinct reviews and actual-object parent gate are inherited with their precise coverage. Final distinct/parent review and whole contained replay passed:14 entire dictionaries/42 strict RAW artifact pairs/16 whole target links and2 ImageHlp-qualified native PE images. All1870 inputs were RAW equal before this separately archived publication annotation;1869 current inputs plus the complete archived document retain frozen identities. The earlier1867-input replay failure is separately bridged as1866 current inputs plus one exact archived observer driver; it is never described as all-current RAW.

The complete notices below are reproduced in supporting documentation and also retained in LICENSES/libio-floatconv-notices.txt and the unchanged source. GPL text is already published at LICENSES/GPL-2.0.txt.

```
/* 
Copyright (C) 1993, 1994 Free Software Foundation

This file is part of the GNU IO Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

As a special exception, if you link this library with files
compiled with a GNU compiler to produce an executable, this does not cause
the resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why
the executable file might be covered by the GNU General Public License. */

/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991 by AT&T.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR AT&T MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/
```
