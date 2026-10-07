/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

/* Adapted 2026-10-07 from the complete Coin3D
 * SbRotation::setValue(const SbMatrix&) at
 * da9c1330c618cff65598b97e881bb896d9ac84ad.
 * The retail counterpart transposes the indexing signs, uses literal one,
 * reads its writable index table, skips a zero-root reciprocal, and omits the
 * homogeneous m33 scale. Its publication/reload order is preserved below.
 * This is algorithm reuse, not unchanged source identity or ancestry.
 */
#include "george/matrix_to_quaternion.h"
#include "george/ee_math.h"

void george_matrix_to_quaternion(GeorgeMathVec4 *output,
                                 const GeorgeRotationMatrix *matrix)
{
    const float *m = matrix->element;
    /* Indexed XYZW is the measured GNU contiguous-float view of this prefix.
     * Later m loads stay fresh when the initialized output overlaps it.
     */
    float *q = (float *)output;
    float first = m[0];
    float second = m[5];
    float third = m[10];
    float trace = (first + second) + third;

    if (trace > 0.0f) {
        float root = george_ee_square_root(trace + 1.0f);
        float half_root = root * 0.5f;
        float scale = 0.5f / root;

        /* The reciprocal precedes W, then each pair is loaded after the
         * preceding store. There is no whole-matrix snapshot.
         */
        q[3] = half_root;
        first = m[6];
        second = m[9];
        q[0] = (second - first) * scale;
        first = m[8];
        second = m[2];
        q[1] = (second - first) * scale;
        first = m[1];
        second = m[4];
        q[2] = (second - first) * scale;
    } else {
        s32 i = 0;
        s32 j;
        s32 k;
        float diagonal;
        float root;
        float scale;

        if (first < second) i = 1;
        if (m[5 * i] < third) i = 2;

        diagonal = m[5 * i];
        j = D_003FC940[i];
        k = D_003FC940[j];
        first = m[5 * j];
        second = m[5 * k];
        root = george_ee_square_root((diagonal - (first + second)) + 1.0f);

        /* Indices are captured before the first output publication. The
         * original retains root zero instead of executing a division.
         */
        q[i] = root * 0.5f;
        scale = root;
        if (root != 0.0f) scale = 0.5f / root;

        first = m[4 * k + j];
        second = m[4 * j + k];
        q[3] = (first - second) * scale;
        first = m[4 * i + j];
        second = m[4 * j + i];
        q[j] = (second + first) * scale;
        first = m[4 * k + i];
        second = m[4 * i + k];
        q[k] = (first + second) * scale;
    }
}
