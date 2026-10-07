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

/* Adapted from the pinned Coin3D SbTri3f::sqrDistance region algorithm.
 * Original game inputs already supply displacements; scalar grouping and the
 * soft-double absolute operation follow the complete original instructions.
 * See docs/segment_distance.md for the source pin, changes and proof limits. */
#include "george/segment_distance.h"

typedef unsigned long long SegmentBits64;
typedef char segment_bits64_size[(sizeof(SegmentBits64) == 8) ? 1 : -1];
extern SegmentBits64 func_00374848(float);
extern s32 func_00373250(SegmentBits64, SegmentBits64);
extern SegmentBits64 func_00372CC0(SegmentBits64, SegmentBits64);
extern float func_003734F8(SegmentBits64);

#if defined(__GNUC__) && __GNUC__ >= 3
#define SEGMENT_INLINE static __inline__ __attribute__((always_inline))
#elif defined(__GNUC__)
#define SEGMENT_INLINE static __inline__
#else
#define SEGMENT_INLINE static
#endif
SEGMENT_INLINE float segment_absolute(float value)
{
    SegmentBits64 bits = func_00374848(value);
    if (func_00373250(bits, 0) < 0) bits = func_00372CC0(0, bits);
    return func_003734F8(bits);
}

float func_0029D5D0(const GeorgeMathVec3 *origin0,
                    const GeorgeMathVec3 *direction0,
                    const GeorgeMathVec3 *origin1,
                    const GeorgeMathVec3 *direction1,
                    float *pfSegP0, float *pfSegP1)
{
  float dx = origin0->x - origin1->x;
  float dy = origin0->y - origin1->y;
  float dz = origin0->z - origin1->z;
  float fA00 = (direction0->x * direction0->x + direction0->y * direction0->y) + direction0->z * direction0->z;
  float fA01 = -((direction0->x * direction1->x + direction0->y * direction1->y) + direction0->z * direction1->z);
  float fA11 = (direction1->x * direction1->x + direction1->y * direction1->y) + direction1->z * direction1->z;
  float fB0 = (dx * direction0->x + dy * direction0->y) + dz * direction0->z;
  float fC = (dx * dx + dy * dy) + dz * dz;
  float fDet = segment_absolute(fA00 * fA11 - fA01 * fA01);
  float fB1, fS, fT, fSqrDist, fTmp;
  if (fDet >= 0.000001f) {
    /* line segments are not parallel */
    fB1 = -((dx * direction1->x + dy * direction1->y) + dz * direction1->z);
    fS = fA01*fB1-fA11*fB0;
    fT = fA01*fB0-fA00*fB1;

    if (fS >= 0.0f) {
      if (fS <= fDet) {
        if (fT >= 0.0f) {
          if (fT <= fDet) {  /* region 0 (interior) */
            /* minimum at two interior points of 3D lines */
            float fInvDet = 1.0f/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+(fB0 + fB0)) +
              fT*(fA01*fS+fA11*fT+(fB1 + fB1))+fC;
          }
          else {  /* region 3 (side) */
            fT = 1.0f;
            fTmp = fA01+fB0;
            if (fTmp >= 0.0f) {
              fS = 0.0f;
              fSqrDist = fA11+(fB1 + fB1)+fC;
            }
            else if (-fTmp >= fA00) {
              fS = 1.0f;
              fSqrDist = fA00+fA11+fC+((fB1 + fTmp) + (fB1 + fTmp));
            }
            else {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+(fB1 + fB1)+fC;
            }
          }
        }
        else {  /* region 7 (side) */
          fT = 0.0f;
          if (fB0 >= 0.0f) {
            fS = 0.0f;
            fSqrDist = fC;
          }
          else if (-fB0 >= fA00) {
            fS = 1.0f;
            fSqrDist = fA00+(fB0 + fB0)+fC;
          }
          else {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
      }
      else {
        if (fT >= 0.0f) {
          if (fT <= fDet) {  /* region 1 (side) */
            fS = 1.0f;
            fTmp = fA01+fB1;
            if (fTmp >= 0.0f) {
              fT = 0.0f;
              fSqrDist = fA00+(fB0 + fB0)+fC;
            }
            else if (-fTmp >= fA11) {
              fT = 1.0f;
              fSqrDist = fA00+fA11+fC+((fB0 + fTmp) + (fB0 + fTmp));
            }
            else {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+(fB0 + fB0)+fC;
            }
          }
          else {  /* region 2 (corner) */
            fTmp = fA01+fB0;
            if (-fTmp <= fA00) {
              fT = 1.0f;
              if (fTmp >= 0.0f) {
                fS = 0.0f;
                fSqrDist = fA11+(fB1 + fB1)+fC;
              }
              else {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+(fB1 + fB1)+fC;
              }
            }
            else {
              fS = 1.0f;
              fTmp = fA01+fB1;
              if (fTmp >= 0.0f) {
                fT = 0.0f;
                fSqrDist = fA00+(fB0 + fB0)+fC;
              }
              else if (-fTmp >= fA11) {
                fT = 1.0f;
                fSqrDist = fA00+fA11+fC+((fB0 + fTmp) + (fB0 + fTmp));
              }
              else {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+(fB0 + fB0)+fC;
              }
            }
          }
        }
        else {  /* region 8 (corner) */
          if (-fB0 < fA00) {
            fT = 0.0f;
            if (fB0 >= 0.0f) {
              fS = 0.0f;
              fSqrDist = fC;
            }
            else {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
          else {
            fS = 1.0f;
            fTmp = fA01+fB1;
            if (fTmp >= 0.0f) {
              fT = 0.0f;
              fSqrDist = fA00+(fB0 + fB0)+fC;
            }
            else if (-fTmp >= fA11) {
              fT = 1.0f;
              fSqrDist = fA00+fA11+fC+((fB0 + fTmp) + (fB0 + fTmp));
            }
            else {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+(fB0 + fB0)+fC;
            }
          }
        }
      }
    }
    else {
      if (fT >= 0.0f) {
        if (fT <= fDet) {  /* region 5 (side) */
          fS = 0.0f;
          if (fB1 >= 0.0f) {
            fT = 0.0f;
            fSqrDist = fC;
          }
          else if (-fB1 >= fA11) {
            fT = 1.0f;
            fSqrDist = fA11+(fB1 + fB1)+fC;
          }
          else {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
        else {  /* region 4 (corner) */
          fTmp = fA01+fB0;
          if (fTmp < 0.0f) {
            fT = 1.0f;
            if (-fTmp >= fA00) {
              fS = 1.0f;
              fSqrDist = fA00+fA11+fC+((fB1 + fTmp) + (fB1 + fTmp));
            }
            else {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+(fB1 + fB1)+fC;
            }
          }
          else {
            fS = 0.0f;
            if (fB1 >= 0.0f) {
              fT = 0.0f;
              fSqrDist = fC;
            }
            else if (-fB1 >= fA11) {
              fT = 1.0f;
              fSqrDist = fA11+(fB1 + fB1)+fC;
            }
            else {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
        }
      }
      else {   /* region 6 (corner) */
        if (fB0 < 0.0f) {
          fT = 0.0f;
          if (-fB0 >= fA00) {
            fS = 1.0f;
            fSqrDist = fA00+(fB0 + fB0)+fC;
          }
          else {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else {
          fS = 0.0f;
          if (fB1 >= 0.0f) {
            fT = 0.0f;
            fSqrDist = fC;
          }
          else if (-fB1 >= fA11) {
            fT = 1.0f;
            fSqrDist = fA11+(fB1 + fB1)+fC;
          }
          else {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
    }
  }
  else {
    /* line segments are parallel */
    if (fA01 > 0.0f) {
      /* direction vectors form an obtuse angle */
      if (fB0 >= 0.0f) {
        fS = 0.0f;
        fT = 0.0f;
        fSqrDist = fC;
      }
      else if (-fB0 <= fA00) {
        fS = -fB0/fA00;
        fT = 0.0f;
        fSqrDist = fB0*fS+fC;
      }
      else {
        fB1 = -((dx * direction1->x + dy * direction1->y) + dz * direction1->z);
        fS = 1.0f;
        fTmp = fA00+fB0;
        if (-fTmp >= fA01) {
          fT = 1.0f;
          fSqrDist = fA00+fA11+fC+(((fA01 + fB0) + fB1) + ((fA01 + fB0) + fB1));
        }
        else {
          fT = -fTmp/fA01;
          fSqrDist = fA00+(fB0 + fB0)+fC+fT*(fA11*fT+((fA01 + fB1) + (fA01 + fB1)));
        }
      }
    }
    else {
      /* direction vectors form an acute angle */
      if (-fB0 >= fA00) {
        fS = 1.0f;
        fT = 0.0f;
        fSqrDist = fA00+(fB0 + fB0)+fC;
      }
      else if (fB0 <= 0.0f) {
        fS = -fB0/fA00;
        fT = 0.0f;
        fSqrDist = fB0*fS+fC;
      }
      else {
        fB1 = -((dx * direction1->x + dy * direction1->y) + dz * direction1->z);
        fS = 0.0f;
        if (fB0 >= -fA01) {
          fT = 1.0f;
          fSqrDist = fA11+(fB1 + fB1)+fC;
        }
        else {
          fT = -fB0/fA01;
          fSqrDist = fC+fT*((fB1 + fB1)+fA11*fT);
        }
      }
    }
  }

  if (pfSegP0) *pfSegP0 = fS;

  if (pfSegP1) *pfSegP1 = fT;
  return segment_absolute(fSqrDist);
}

#undef SEGMENT_INLINE
