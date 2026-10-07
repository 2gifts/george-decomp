# Checkpoint 75: quaternion conversion and resource pointers

Five reviewed C functions cover 860 original bytes. The 48-byte resource predicate and 40-byte resource initializer match exactly; the 144-byte resource relocation, 148-byte resource selector and 480-byte matrix-to-quaternion function remain reconstructed. Alignment padding, tables and called helpers receive no matching credit.

The project now has 1,657 reviewed C/C++ functions covering 392,116 bytes. Of these, 446 functions match all 23,920 original bytes. The full source build remains incomplete. The hybrid executable reproduces the original executable, including retained original code and data.

The [quaternion](matrix_to_quaternion.md) and [resource](resource_pointer.md) family notes are immutable descriptions captured before final acceptance. Their pending-review wording is historical. The registered function manifests and this checkpoint describe the accepted results.

## Reuse and validation

Quaternion conversion adapts Coin3D's complete BSD-licensed `SbRotation::setValue` matrix algorithm, with its notice in [LICENSES](../LICENSES/coin3d-matrix-to-quaternion.txt). It also reuses the unchanged project EE square-root helper and existing scalar instruction decoders. Retail-specific transpose, strict tie selection, writable lookup table, zero-root division and live overlapping stores remain explicit. This adaptation does not establish Coin3D ancestry for the retail game.

The resource routines reuse the unchanged project scalar types, field conventions and instruction decoders. Their tests preserve fresh pointer loads, count subtraction before pointer publication, complete 64-bit selector mode and sign-extended changed results. No previously verified library algorithm was found for these four bodies.

Each family retains four production/ABI target compilations and their actual ordered dependency queries, complete objects, real relocations and original-address links. Both quaternion compiler profiles remain nonmatching. The two resource exact matches use GCC 2.9; the initializer also matches GCC 3.2.3 without duplicate credit. Parent and distinct reviewers each completed one unchanged contained replay, using existing target objects and native executables without rebuilding them.

Quaternion evidence covers 168 unique complete memory arenas, all 120 selected instruction addresses and 12,978 original instructions. Independent rational checks cover 88 mathematically exact cases; the 48 primary-zero cases are a subset of those 88. The 120 nominal cases comprise 80 inexact cases and 40 zero-sign gaps. Native checks total 12,504, including 24 additional proper rotations. Five actual incorrect-source controls fail. Resource evidence covers 320 unique complete arenas, all 95 selected instruction addresses and 11,880 original instructions; native checks total 1,622,660, and six actual incorrect-source controls fail.

## Preserved qualifications

The frozen quaternion packet incorrectly describes the first transpose-sign control failure as the final W component. In fixture 8, the W store aliases a later matrix load. The final destination word 17 is `q[0]`, whose correct bits are `0x3D800000` (+1/16). Reversing the earlier W skew would propagate -1/16 (`0xBD800000`) to that later component. Those wrong bits are an independent prediction; the saved native failure identifies fixture 8 and word 17 without capturing that value. The original packet and failed readers remain intact, with this additive correction.

Resource native objects and the authentic MinGW limits header establish a 32-bit `unsigned long` counter contract. That batch did not emit a separate `sizeof(long)` measurement; no such measurement is claimed. Its pointer and selector-result widths were measured explicitly. The quaternion batch preserves its initial link-capture and reader failures, including two selected links without separate raw output pipes. Full objects, scripts and linked executables remain available privately.

The tested domain is initialized, aligned ordinary GNU scalar storage and the observed lower 64-bit register effects. Original classes and universal prototypes, upper 128-bit lanes, hardware floating-point identity, timing, concurrency and full gameplay remain open. Original executable data, complete instruction arrays, disc contents and private manufacturer documentation are excluded from GitHub.
