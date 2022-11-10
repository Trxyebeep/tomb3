#include "../tomb3/pch.h"
#include "sphere.h"

void InitInterpolate2(long frac, long rate)
{
	IM_rate = rate;
	IM_frac = frac;
	IMptr = &IMstack[384];
	memcpy(&IMstack[384], phd_mxptr, 48);
}

void inject_sphere(bool replace)
{
	INJECT(0x00468580, InitInterpolate2, replace);
}
