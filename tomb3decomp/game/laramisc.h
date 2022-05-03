#pragma once
#include "../global/vars.h"

extern void (*extra_control_routines[13])(ITEM_INFO* item, COLL_INFO* coll);
extern void (*lara_control_routines[89])(ITEM_INFO* item, COLL_INFO* coll);
extern void (*lara_collision_routines[89])(ITEM_INFO* item, COLL_INFO* coll);

#define AnimateLara	( (void(__cdecl*)(ITEM_INFO*)) 0x0044D2A0 )
