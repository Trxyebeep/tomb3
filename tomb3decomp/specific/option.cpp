#include "../tomb3/pch.h"
#include "option.h"

long GetRenderWidth()
{
	return phd_winwidth;
}

long GetRenderHeight()
{
	return phd_winheight;
}

void inject_option(bool replace)
{
	INJECT(0x0048A200, GetRenderWidth, replace);
	INJECT(0x0048A1F0, GetRenderHeight, replace);
}
