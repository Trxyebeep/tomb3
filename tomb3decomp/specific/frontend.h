#pragma once
#include "../global/vars.h"

void inject_frontend(bool replace);

ushort S_COLOUR(long r, long g, long b);

#define S_DrawScreenFBox	( (void(__cdecl*)(long, long, long, long, long, long, ushort*, ushort)) 0x00483770 )
