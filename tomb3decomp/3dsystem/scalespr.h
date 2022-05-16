#pragma once
#include "../global/vars.h"

void inject_scalespr(bool replace);

short* ins_room_sprite(short* objptr, long num);
void S_DrawScreenSprite2d(long x, long y, long z, long scaleH, long scaleV, short sprnum, short shade, ushort flags);
void S_DrawScreenSprite(long x, long y, long z, long scaleH, long scaleV, short sprnum, short shade, ushort flags);
