#pragma once
#include "../global/vars.h"

void inject_frontend(bool replace);

ushort S_COLOUR(long r, long g, long b);
void S_DrawScreenLine(long x, long y, long z, long w, long h, long c, ushort* grdptr, ushort f);
void S_DrawScreenBox(long x, long y, long z, long w, long h, long sprnum, ushort* grdptr, ushort f);

#define S_DrawScreenFBox	( (void(__cdecl*)(long, long, long, long, long, long, ushort*, ushort)) 0x00483770 )
