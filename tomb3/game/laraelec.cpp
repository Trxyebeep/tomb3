#include "../tomb3/pch.h"
#include "laraelec.h"
#include "../specific/game.h"

void UpdateElectricityPoints()
{
	short* points;
	long rnd;
	short x, y, z, xv, yv, zv;

	points = &electricity_points[0][0];

	for (int i = 0; i < 32; i++)
	{
		rnd = GetRandomDraw();
		x = points[0];
		y = points[1];
		z = points[2];
		xv = points[3];
		yv = points[4];
		zv = points[5];

		if ((x > 256 || x < -256) && (y > 256 || y < -256) && (z > 256 || z < -256) ||
			x > 384 || x < -128 || y > 384 || y < -128 || z > 384 || z < -128)
		{
			x = 0;
			y = 0;
			z = 0;
			xv = 0;
			yv = 0;
			zv = 0;
		}

		if (xv)
		{
			if (xv >= 0)
				xv += 2;
			else
				xv -= 2;
		}
		else if (rnd & 1)
			xv = -1 - (GetRandomDraw() & 3);
		else
			xv = (GetRandomDraw() & 3) + 1;

		if (yv)
		{
			if (yv >= 0)
				yv += 2;
			else
				yv -= 2;
		}
		else if (rnd & 2)
			yv = -1 - (GetRandomDraw() & 3);
		else
			yv = (GetRandomDraw() & 3) + 1;

		if (zv)
		{
			if (zv >= 0)
				zv++;
			else
				zv--;
		}
		else if (rnd & 4)
			zv = -1 - (GetRandomDraw() & 3);
		else
			zv = (GetRandomDraw() & 3) + 1;

		x += xv;
		y += yv;
		z += zv;

		*points++ = x;
		*points++ = y;
		*points++ = z;
		*points++ = xv;
		*points++ = yv;
		*points++ = zv;
	}
}

void inject_laraelec(bool replace)
{
	INJECT(0x00449A30, UpdateElectricityPoints, replace);
}
