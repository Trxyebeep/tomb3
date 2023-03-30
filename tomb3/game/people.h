#pragma once
#include "../global/types.h"

short GunShot(long x, long y, long z, short speed, short yrot, short room_number);
short GunHit(long x, long y, long z, short speed, short yrot, short room_number);
short GunMiss(long x, long y, long z, short speed, short yrot, short room_number);
long ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* bite, short extra_rotation, long damage);
long TargetVisible(ITEM_INFO* item, AI_INFO* info);
long Targetable(ITEM_INFO* item, AI_INFO* info);
