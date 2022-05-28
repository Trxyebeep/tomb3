#pragma once
#include "../global/vars.h"

void inject_collide(bool replace);

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll);

#define GetCollisionInfo	( (void(__cdecl*)(COLL_INFO*, long, long, long, short, long)) 0x0041D500 )
#define LaraBaddieCollision	( (void(__cdecl*)(ITEM_INFO*, COLL_INFO*)) 0x0041E8D0 )
#define UpdateLaraRoom	( (void(__cdecl*)(ITEM_INFO*, long)) 0x0041E6D0 )
#define ObjectCollision	( (void(__cdecl*)(short, ITEM_INFO*, COLL_INFO*)) 0x0041EBD0 )
#define TestBoundsCollide	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*, long)) 0x0041F0E0 )
