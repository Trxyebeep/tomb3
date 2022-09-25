#include "../tomb3/pch.h"
#include "invfunc.h"
#include "../specific/frontend.h"

void InitColours()
{
	inv_colours[0] = S_COLOUR(0, 0, 0);
	inv_colours[1] = S_COLOUR(64, 64, 64);
	inv_colours[2] = S_COLOUR(255, 255, 255);
	inv_colours[3] = S_COLOUR(255, 0, 0);
	inv_colours[4] = S_COLOUR(255, 128, 0);
	inv_colours[5] = S_COLOUR(255, 255, 0);
	inv_colours[12] = S_COLOUR(0, 128, 0);
	inv_colours[13] = S_COLOUR(0, 255, 0);
	inv_colours[14] = S_COLOUR(0, 255, 255);
	inv_colours[15] = S_COLOUR(0, 0, 255);
	inv_colours[16] = S_COLOUR(255, 0, 255);
}

void inject_invfunc(bool replace)
{
	INJECT(0x00437050, InitColours, replace);
}
