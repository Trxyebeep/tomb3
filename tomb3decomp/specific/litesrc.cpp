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

short* calc_vertice_light(short* objptr, short* objptr1)
{
	PHD_VBUF* buf;
	float fx, fy, fz, v0, v1, v2;
	long fade, val, shade, r, g, b, x, y, z;
	short nVtx;

	buf = vbuf;
	nVtx = *objptr++;

	if (nVtx < 0)
	{
		fade = distanceFogValue << 14;

		for (; nVtx < 0; nVtx++)
		{
			shade = (0x1FFF - *objptr) >> 5;

			if (!shade)
				shade = 255;

			r = (smcr * shade) >> 11;
			g = (smcg * shade) >> 11;
			b = (smcb * shade) >> 11;

			if (r > 31)
				r = 31;

			if (g > 31)
				g = 31;

			if (b > 31)
				b = 31;

			if (buf->z > fade)
			{
				val = 2048 - ((buf->z - fade) >> 16);
				r = (val * 8 * r) >> 14;
				g = (val * 8 * g) >> 14;
				b = (val * 8 * b) >> 14;

				if (r < 0)
					r = 0;

				if (g < 0)
					g = 0;

				if (b < 0)
					b = 0;
			}

			buf->g = short(r << 10 | g << 5 | b);
			buf++;
			objptr++;
		}
	}
	else
	{
		LightPos[M00] = (LPos[0].x * phd_mxptr[M00] + LPos[0].y * phd_mxptr[M10] + LPos[0].z * phd_mxptr[M20]) >> 14;
		LightPos[M01] = (LPos[0].x * phd_mxptr[M01] + LPos[0].y * phd_mxptr[M11] + LPos[0].z * phd_mxptr[M21]) >> 14;
		LightPos[M02] = (LPos[0].x * phd_mxptr[M02] + LPos[0].y * phd_mxptr[M12] + LPos[0].z * phd_mxptr[M22]) >> 14;

		LightPos[M10] = (LPos[1].x * phd_mxptr[M00] + LPos[1].y * phd_mxptr[M10] + LPos[1].z * phd_mxptr[M20]) >> 14;
		LightPos[M11] = (LPos[1].x * phd_mxptr[M01] + LPos[1].y * phd_mxptr[M11] + LPos[1].z * phd_mxptr[M21]) >> 14;
		LightPos[M12] = (LPos[1].x * phd_mxptr[M02] + LPos[1].y * phd_mxptr[M12] + LPos[1].z * phd_mxptr[M22]) >> 14;

		LightPos[M20] = (LPos[2].x * phd_mxptr[M00] + LPos[2].y * phd_mxptr[M10] + LPos[2].z * phd_mxptr[M20]) >> 14;
		LightPos[M21] = (LPos[2].x * phd_mxptr[M01] + LPos[2].y * phd_mxptr[M11] + LPos[2].z * phd_mxptr[M21]) >> 14;
		LightPos[M22] = (LPos[2].x * phd_mxptr[M02] + LPos[2].y * phd_mxptr[M12] + LPos[2].z * phd_mxptr[M22]) >> 14;

		fade = distanceFogValue << 14;

		for (; nVtx > 0; nVtx--)
		{
			v0 = (float)objptr[0];
			v1 = (float)objptr[1];
			v2 = (float)objptr[2];
			fx = v0 * (float)LightPos[M00] + v1 * (float)LightPos[M01] + v2 * (float)LightPos[M02];
			fy = v0 * (float)LightPos[M10] + v1 * (float)LightPos[M11] + v2 * (float)LightPos[M12];
			fz = v0 * (float)LightPos[M20] + v1 * (float)LightPos[M21] + v2 * (float)LightPos[M22];
			x = (long)fx;
			y = (long)fy;
			z = (long)fz;
			x >>= 16;
			y >>= 16;
			z >>= 16;

			if (x < 0)
				x = 0;

			if (y < 0)
				y = 0;

			if (z < 0)
				z = 0;

			r = smcr + ((LightCol[M00] * x + LightCol[M01] * y + LightCol[M02] * z) >> 16);
			g = smcg + ((LightCol[M10] * x + LightCol[M11] * y + LightCol[M12] * z) >> 16);
			b = smcb + ((LightCol[M20] * x + LightCol[M21] * y + LightCol[M22] * z) >> 16);

			if (r > 255)
				r = 255;

			if (g > 255)
				g = 255;

			if (b > 255)
				b = 255;

			if (r < 0)
				r = 0;

			if (g < 0)
				g = 0;

			if (b < 0)
				b = 0;

			r >>= 3;
			g >>= 3;
			b >>= 3;

			if (buf->z > fade)
			{
				val = 2048 - ((buf->z - fade) >> 16);
				r = (val * 8 * r) >> 14;
				g = (val * 8 * g) >> 14;
				b = (val * 8 * b) >> 14;

				if (r < 0)
					r = 0;

				if (g < 0)
					g = 0;

				if (b < 0)
					b = 0;
			}

			buf->g = short(r << 10 | g << 5 | b);
			buf++;
			objptr += 3;
		}
	}

	return objptr;
}

void inject_litesrc(bool replace)
{
	INJECT(0x00487550, S_CalculateStaticLight, replace);
	INJECT(0x00487590, S_CalculateStaticMeshLight, replace);
	INJECT(0x004866E0, calc_vertice_light, replace);
}
