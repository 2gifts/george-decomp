#ifndef GEORGE_GOAL_DESTRUCTOR_WRAPPERS_H
#define GEORGE_GOAL_DESTRUCTOR_WRAPPERS_H

#include "george/types.h"

/* Effects-only forwarding convention for the observed two pointer/word lanes.
 * Valid storage uses the existing GeorgeGoalBase prefix at the helper call.
 * No original class, universal return prototype, ownership or lifetime is
 * inferred. An incidental helper result is outside this void convention. */
void george_goal_base_destructor_forward(void *storage, u32 flags);

#endif
