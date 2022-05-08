#include "../tomb3/pch.h"
#include "litesrc.h"
#include "../3dsystem/phd_math.h"

void S_CalculateStaticLight(short adder)
{
	smcr = (adder & 0x1F) << 3;
	smcg = (adder & 0x3E0) >> 2;
	smcb = (adder & 0x7C00) >> 7;
}

void S_CalculateStaticMeshLight(long x, long y, long z, short shade, short shadeB, ROOM_INFO* ri)
{
	DYNAMIC* dynamic;
	long dx, dy, dz, dist, falloff;
	short r, g, b;

	r = 0;
	g = 0;
	b = 0;

	for (int i = 0; i < number_dynamics; i++)
	{
		dynamic = &dynamics[i];
		dx = x - dynamic->x;
		dy = y - dynamic->y;
		dz = z - dynamic->z;

		if (ABS(dx) > 0x2000 || ABS(dy) > 0x2000 || ABS(dz) > 0x2000)
			continue;

		dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
		falloff = dynamic->falloff >> 1;

		if (dist <= falloff)
		{
			falloff = 0x1FFF - 0x1FFF * dist / falloff;
			r += short((falloff * dynamic->r) >> 10);
			g += short((falloff * dynamic->g) >> 10);
			b += short((falloff * dynamic->b) >> 10);
		}
	}

	if (r + g + b)
	{
		r += (shade & 0x1F) << 3;
		g += (shade & 0x3E0) >> 2;
		b += (shade & 0x7C00) >> 7;

		if (r > 255)
			r = 255;

		if (g > 255)
			g = 255;

		if (b > 255)
			b = 255;

		smcr = r;
		smcg = g;
		smcb = b;
	}
	else
		S_CalculateStaticLight(shade);
}

void inject_litesrc(bool replace)
{
	INJECT(0x00487550, S_CalculateStaticLight, replace);
	INJECT(0x00487590, S_CalculateStaticMeshLight, replace);
}
