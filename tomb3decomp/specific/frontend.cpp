#include "../tomb3/pch.h"
#include "frontend.h"
#include "specific.h"

ushort S_COLOUR(long r, long g, long b)
{
	return SWR_FindNearestPaletteEntry(game_palette, r, g, b, 0);
}

void inject_frontend(bool replace)
{
	INJECT(0x004835E0, S_COLOUR, replace);
}
