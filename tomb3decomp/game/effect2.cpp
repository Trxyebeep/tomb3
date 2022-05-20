#include "../tomb3/pch.h"
#include "effect2.h"

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	if (number_dynamics != 32)
	{
		dl = &dynamics[number_dynamics];
		dl->x = x;
		dl->y = y;
		dl->z = z;
		dl->falloff = ushort(falloff << 8);
		dl->on = 1;
		dl->r = (uchar)r;
		dl->g = (uchar)g;
		dl->b = (uchar)b;
		number_dynamics++;
	}
}

void inject_effect2(bool replace)
{
	INJECT(0x0042DE00, TriggerDynamic, replace);
}
