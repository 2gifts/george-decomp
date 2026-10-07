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

/* The compatible cross-normal/two-equation solve and line-plane formula are
 * adapted from Coin3D SbPlane::intersect at the pinned source documented in
 * docs/plane_intersection.md. Original game thresholds, ties, coordinate
 * divisions, endpoint representation and alias-visible stores are retained.
 * The complete upstream copyright/license notice precedes this comment. */
#include "george/plane_intersection.h"

typedef unsigned long long PlaneBits64;
typedef char plane_bits64_width[(sizeof(PlaneBits64) == 8) ? 1 : -1];
extern void *func_003936A0(void *, s32, u32);
extern PlaneBits64 func_00374848(float);
extern s32 func_00373250(PlaneBits64, PlaneBits64);
extern PlaneBits64 func_00372CC0(PlaneBits64, PlaneBits64);

u32 func_0029F370(const GeorgeMathVec4 *first, const GeorgeMathVec4 *second,
                  GeorgeMathVec3 *point, GeorgeMathVec3 *endpoint)
{
    GeorgeMathVec3 position, direction;
    float magnitude_x, magnitude_y, magnitude_z, first_w, second_w;
    float x, y, z, end_x, end_y, end_z;
    s32 axis;
    func_003936A0(&position, 0, 12);
    func_003936A0(&direction, 0, 12);
    direction.x = first->y * second->z - first->z * second->y;
    direction.z = first->x * second->y - first->y * second->x;
    direction.y = first->z * second->x - first->x * second->z;
    magnitude_x = direction.x;
    if (!(0.0f <= magnitude_x)) magnitude_x = -magnitude_x;
    magnitude_y = direction.y;
    if (!(0.0f <= magnitude_y)) magnitude_y = -magnitude_y;
    magnitude_z = direction.z;
    if (!(0.0f <= magnitude_z)) magnitude_z = -magnitude_z;
    if ((magnitude_x + magnitude_y) + magnitude_z < 0.0001f) return 0;
    if (magnitude_y < magnitude_x)
        axis = magnitude_z < magnitude_x ? 1 : 3;
    else
        axis = magnitude_z < magnitude_y ? 2 : 3;
    first_w = -first->w;
    second_w = -second->w;
    if (axis == 1) {
        position.x = 0.0f;
        position.z = (first_w * second->y - second_w * first->y) / direction.x;
        position.y = (second_w * first->z - first_w * second->z) / direction.x;
    } else if (axis == 2) {
        position.y = 0.0f;
        position.z = (second_w * first->x - first_w * second->x) / direction.y;
        position.x = (first_w * second->z - second_w * first->z) / direction.y;
    } else {
        position.z = 0.0f;
        position.y = (first_w * second->x - second_w * first->x) / direction.z;
        position.x = (second_w * first->y - first_w * second->y) / direction.z;
    }
    x = position.x; y = position.y; z = position.z;
    end_x = x + direction.x;
    end_y = y + direction.y;
    end_z = z + direction.z;
    point->z = z;
    point->x = x;
    point->y = y;
    endpoint->x = end_x;
    endpoint->z = end_z;
    endpoint->y = end_y;
    return 1;
}

u32 func_0029F630(const GeorgeMathVec3 *first, const GeorgeMathVec3 *second,
                  const GeorgeMathVec4 *plane, GeorgeMathVec3 *output)
{
    GeorgeMathVec3 difference;
    PlaneBits64 absolute;
    float denominator, numerator, fraction, x, y, z;
    func_003936A0(&difference, 0, 12);
    difference.x = second->x - first->x;
    difference.y = second->y - first->y;
    difference.z = second->z - first->z;
    denominator = (plane->x * difference.x + plane->y * difference.y) +
                  plane->z * difference.z;
    numerator = plane->w - ((plane->x * first->x + plane->y * first->y) +
                            plane->z * first->z);
    absolute = func_00374848(denominator);
    if (func_00373250(absolute, 0) < 0) absolute = func_00372CC0(0, absolute);
    if (func_00373250(absolute, 0x3F1A36E2E0000000ULL) < 0) return 0;
    fraction = numerator / denominator;
    x = first->x;
    output->x = x;
    y = first->y;
    output->y = y;
    z = first->z;
    difference.x *= fraction;
    difference.y *= fraction;
    difference.z *= fraction;
    output->x = x + difference.x;
    output->z = z + difference.z;
    output->y = y + difference.y;
    return 1;
}
