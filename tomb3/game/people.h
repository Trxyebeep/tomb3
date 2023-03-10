#pragma once
#include "../global/vars.h"

void inject_people(bool replace);

short GunShot(long x, long y, long z, short speed, short yrot, short room_number);
short GunHit(long x, long y, long z, short speed, short yrot, short room_number);
short GunMiss(long x, long y, long z, short speed, short yrot, short room_number);
long ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* bite, short extra_rotation, long damage);
long TargetVisible(ITEM_INFO* item, AI_INFO* info);

#define Targetable	( (long(__cdecl*)(ITEM_INFO*, AI_INFO*)) 0x0045B610 )
