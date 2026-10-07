#ifndef GEORGE_SPATIAL_QUERIES_H
#define GEORGE_SPATIAL_QUERIES_H

#include "george/goal_methods4.h"
#include "george/geometry_bounds.h"

/* Numeric entry names retain existing caller declarations. The argument called
 * context by older callers addresses XYZ floats; the selected body proves no
 * larger context layout. These views supplement the existing partial types. */
extern GeorgeGoalMapOwner *D_003F8C28;
GeorgeGoalRoad *func_001CC960(const GeorgeMathVec3 *point) GEORGE_SAVE128;
GeorgeGoalRoad *func_001CCA28(u32 word);
GeorgeGoalRouteResult *func_001CF340(void *context) GEORGE_SAVE128;
s32 func_001CF588(void *context, u32 word) GEORGE_SAVE128;

/* The original polygon routines use 64 bytes of uninitialized caller scratch.
 * Zero vertices still read its first Y. In the multi-record entry an earlier
 * record can have initialized that Y; scratch is never cleared between records.
 * More than five vertices overlap saved
 * registers. No asset capacity or positive-count contract has been established.
 * Ordinary C retains the uninitialized local and introduces no capacity guard.
 * Numerical testing covers safe counts 1..5 and explicitly proved later-zero
 * reads initialized by an earlier record, together with
 * the finite normal/zero value model of the reused point-transform source. */

#endif
