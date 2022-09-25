#include "../tomb3/pch.h"
#include "specific.h"

uchar SWR_FindNearestPaletteEntry(uchar* p, long r, long g, long b, bool ignoreSystemPalette)
{
	double best, dr, dg, db, d;
	long start, end, c;

	best = 10000000000.0;
	c = 0;					//originally uninitialized

	if (ignoreSystemPalette)
	{
		start = 10;
		end = 246;
		p += 30;
	}
	else
	{
		start = 0;
		end = 256;
	}

	for (; start < end; start++)
	{
		dr = double(r - *p++);
		dg = double(g - *p++);
		db = double(b - *p++);
		d = SQUARE(dr) + SQUARE(dg) + SQUARE(db);

		if (d < best)
		{
			best = d;
			c = start;
		}
	}

	return (uchar)c;
}

void inject_specific(bool replace)
{
	INJECT(0x0048D500, SWR_FindNearestPaletteEntry, replace);
}
