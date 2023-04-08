#pragma once
#include "../global/types.h"

short* ins_room_sprite(short* objptr, long num);
void S_DrawScreenSprite2d(long x, long y, long z, long scaleH, long scaleV, short sprnum, short shade, ushort flags);
void S_DrawScreenSprite(long x, long y, long z, long scaleH, long scaleV, short sprnum, long shade, ushort flags);
void S_DrawSprite(ulong flags, long x, long y, long z, short sprnum, short shade, short scale);
