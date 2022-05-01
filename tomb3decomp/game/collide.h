#pragma once
#include "../global/vars.h"

void inject_collide(bool replace);

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);

#define GetCollisionInfo	( (void(__cdecl*)(COLL_INFO*, long, long, long, short, long)) 0x0041D500 )
