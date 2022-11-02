#pragma once
#include "../global/vars.h"

void inject_quadbike(bool replace);

void QuadBikeDraw(ITEM_INFO* item);

#define QuadBikeControl	( (long(__cdecl*)()) 0x0045EE20 )
