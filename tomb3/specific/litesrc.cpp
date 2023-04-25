#include "../tomb3/pch.h"
#include "litesrc.h"
#include "../3dsystem/phd_math.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/hwinsert.h"
#include "../game/effect2.h"
#include "../game/control.h"
#include "winmain.h"
#include "../tomb3/tomb3.h"

PHD_VECTOR LPos[3];
long LightPos[12];
long LightCol[12];
long smcr;
long smcg;
long smcb;

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

		if (abs(dx) > 0x2000 || abs(dy) > 0x2000 || abs(dz) > 0x2000)
			continue;

		dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
		falloff = dynamic->falloff >> 1;

		if (dist <= falloff)
		{
			falloff = 0x1FFF - 0x1FFF * dist / falloff;
			r += short((falloff * dynamic->r) >> 13);
			g += short((falloff * dynamic->g) >> 13);
			b += short((falloff * dynamic->b) >> 13);
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

	if (nVtx <= 0)
	{
		fade = distanceFogValue << W2V_SHIFT;

		for (; nVtx < 0; nVtx++)
		{
			shade = (0x1FFF - *objptr) >> 5;

			if (!shade)
				shade = 255;

			r = (smcr * shade) >> 8;
			g = (smcg * shade) >> 8;
			b = (smcb * shade) >> 8;

			if (r > 255)
				r = 255;

			if (g > 255)
				g = 255;

			if (b > 255)
				b = 255;

			if (buf->z > fade)
			{
				val = 2048 - ((buf->z - fade) >> 16);
				r = (val * r) / 2048;
				g = (val * g) / 2048;
				b = (val * b) / 2048;

				if (r < 0)
					r = 0;

				if (g < 0)
					g = 0;

				if (b < 0)
					b = 0;
			}

			buf->color = RGB_MAKE(r, g, b);
			buf++;
			objptr++;
		}
	}
	else
	{
		LightPos[M00] = (LPos[0].x * phd_mxptr[M00] + LPos[0].y * phd_mxptr[M10] + LPos[0].z * phd_mxptr[M20]) >> W2V_SHIFT;
		LightPos[M01] = (LPos[0].x * phd_mxptr[M01] + LPos[0].y * phd_mxptr[M11] + LPos[0].z * phd_mxptr[M21]) >> W2V_SHIFT;
		LightPos[M02] = (LPos[0].x * phd_mxptr[M02] + LPos[0].y * phd_mxptr[M12] + LPos[0].z * phd_mxptr[M22]) >> W2V_SHIFT;

		LightPos[M10] = (LPos[1].x * phd_mxptr[M00] + LPos[1].y * phd_mxptr[M10] + LPos[1].z * phd_mxptr[M20]) >> W2V_SHIFT;
		LightPos[M11] = (LPos[1].x * phd_mxptr[M01] + LPos[1].y * phd_mxptr[M11] + LPos[1].z * phd_mxptr[M21]) >> W2V_SHIFT;
		LightPos[M12] = (LPos[1].x * phd_mxptr[M02] + LPos[1].y * phd_mxptr[M12] + LPos[1].z * phd_mxptr[M22]) >> W2V_SHIFT;

		LightPos[M20] = (LPos[2].x * phd_mxptr[M00] + LPos[2].y * phd_mxptr[M10] + LPos[2].z * phd_mxptr[M20]) >> W2V_SHIFT;
		LightPos[M21] = (LPos[2].x * phd_mxptr[M01] + LPos[2].y * phd_mxptr[M11] + LPos[2].z * phd_mxptr[M21]) >> W2V_SHIFT;
		LightPos[M22] = (LPos[2].x * phd_mxptr[M02] + LPos[2].y * phd_mxptr[M12] + LPos[2].z * phd_mxptr[M22]) >> W2V_SHIFT;

		fade = distanceFogValue << W2V_SHIFT;

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

			if (buf->z > fade)
			{
				val = 2048 - ((buf->z - fade) >> 16);
				r = (val * r) / 2048;
				g = (val * g) / 2048;
				b = (val * b) / 2048;

				if (r < 0)
					r = 0;

				if (g < 0)
					g = 0;

				if (b < 0)
					b = 0;
			}

			buf->color = RGB_MAKE(r, g, b);
			buf++;
			objptr += 3;
		}
	}

	return objptr;
}

void S_CalculateLight(long x, long y, long z, short room_number, ITEM_LIGHT* il)
{
	ROOM_INFO* r;
	LIGHT_INFO* sl;
	LIGHT_INFO* light;
	LIGHT_INFO* l;
	DYNAMIC* dl;
	DYNAMIC* dl2;
	LIGHT_INFO sunlight;
	ITEM_LIGHT dummy;
	PHD_VECTOR ls;
	PHD_VECTOR lc;
	long brightest, distance, shade;
	ushort ambience;
	uchar sun, aShift, cShift;

	sl = 0;
	dl2 = 0;
	l = 0;

	if (!il)
	{
		il = &dummy;
		il->init = 0;
	}

	for (int i = 0; i < indices_count; i++)
	{
		LightPos[i] = 0;
		LightCol[i] = 0;
	}

	for (int i = 0; i < 3; i++)
	{
		LPos[i].x = 0;
		LPos[i].y = 0;
		LPos[i].z = 0;
	}

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
	{
		aShift = 6;
		cShift = 3;
	}
	else
#endif
	{
		aShift = 5;
		cShift = 4;
	}

	r = &room[room_number];
	sun = 0;
	brightest = -1;
	ambience = ((0x1FFF - r->ambient) >> aShift) + 1;

	for (int i = 0; i < r->num_lights; i++)
	{
		light = &r->light[i];

		if (light->type)
		{
			sun = 1;
			sl = light;
			continue;
		}

		lc.x = (light->x - x) >> 2;
		lc.y = (light->y - y) >> 2;
		lc.z = (light->z - z) >> 2;
		distance = phd_sqrt(SQUARE(lc.x) + SQUARE(lc.y) + SQUARE(lc.z));

		if (distance <= light->spot.falloff)
		{
			shade = light->spot.intensity - light->spot.intensity * distance / light->spot.falloff;
			ambience += ushort(shade >> 7);

			if (shade > brightest)
			{
				brightest = shade;
				l = light;
				ls.x = lc.x;
				ls.y = lc.y;
				ls.z = lc.z;
			}
		}
	}

	if (!sun && il->init & 1)
	{
		sunlight.sun.nx = (short)il->sun.x;
		sunlight.sun.ny = (short)il->sun.y;
		sunlight.sun.nz = (short)il->sun.z;
		sunlight.r = (0x1FFF - r->ambient) >> aShift;
		sunlight.g = (0x1FFF - r->ambient) >> aShift;
		sunlight.b = (0x1FFF - r->ambient) >> aShift;
		sl = &sunlight;
		sun = 1;
	}

	if (sun)
	{
		if (il->init & 1)
		{
			il->sun.x += (sl->sun.nx - il->sun.x) >> 3;
			il->sun.y += (sl->sun.ny - il->sun.y) >> 3;
			il->sun.z += (sl->sun.nz - il->sun.z) >> 3;
			il->sunr += (sl->r - il->sunr) >> 3;
			il->sung += (sl->g - il->sung) >> 3;
			il->sunb += (sl->b - il->sunb) >> 3;
		}
		else
		{
			il->sun.x = sl->sun.nx;
			il->sun.y = sl->sun.ny;
			il->sun.z = sl->sun.nz;
			il->sunr = sl->r;
			il->sung = sl->g;
			il->sunb = sl->b;
			il->init |= 1;
		}

		LPos[0].x = il->sun.x;
		LPos[0].y = il->sun.y;
		LPos[0].z = il->sun.z;
		LightCol[M00] = il->sunr << cShift;
		LightCol[M10] = il->sung << cShift;
		LightCol[M20] = il->sunb << cShift;
	}

	if (brightest == -1 && il->init & 2)
	{
		brightest = 0x1FFF;
		ls.x = il->bulb.x;
		ls.y = il->bulb.y;
		ls.z = il->bulb.z;
		sunlight.r = (0x1FFF - r->ambient) >> aShift;
		sunlight.g = (0x1FFF - r->ambient) >> aShift;
		sunlight.b = (0x1FFF - r->ambient) >> aShift;
		l = &sunlight;
	}

	if (brightest != -1)
	{
		phd_NormaliseVector(ls.x, ls.y, ls.z, (long*)&ls);

		if (il->init & 2)
		{
			il->bulb.x += (ls.x - il->bulb.x) >> 3;
			il->bulb.y += (ls.y - il->bulb.y) >> 3;
			il->bulb.z += (ls.z - il->bulb.z) >> 3;
			il->bulbr += uchar((((brightest * l->r) >> 13) - il->bulbr) >> 3);
			il->bulbg += uchar((((brightest * l->g) >> 13) - il->bulbg) >> 3);
			il->bulbb += uchar((((brightest * l->b) >> 13) - il->bulbb) >> 3);
		}
		else
		{
			il->bulb.x = ls.x;
			il->bulb.y = ls.y;
			il->bulb.z = ls.z;
			il->bulbr = uchar((brightest * l->r) >> 13);
			il->bulbg = uchar((brightest * l->g) >> 13);
			il->bulbb = uchar((brightest * l->b) >> 13);
			il->init |= 2;
		}

		LPos[1].x = il->bulb.x >> 2;
		LPos[1].y = il->bulb.y >> 2;
		LPos[1].z = il->bulb.z >> 2;
		LightCol[M01] = il->bulbr << cShift;
		LightCol[M11] = il->bulbg << cShift;
		LightCol[M21] = il->bulbb << cShift;
	}

	brightest = -1;

	for (int i = 0; i < number_dynamics; i++)
	{
		dl = &dynamics[i];

		lc.x = dl->x - x;
		lc.y = dl->y - y;
		lc.z = dl->z - z;

		if (abs(lc.x) > 0x2000 || abs(lc.y) > 0x2000 || abs(lc.z) > 0x2000)
			continue;

		distance = phd_sqrt(SQUARE(lc.x) + SQUARE(lc.y) + SQUARE(lc.z));

		if (distance <= dl->falloff >> 1)
		{
			shade = 0x1FFF - 0x1FFF * distance / (dl->falloff >> 1);
			ambience += ushort(shade >> 8);

			if (shade > brightest)
			{
				brightest = shade;
				dl2 = dl;
				ls.x = lc.x;
				ls.y = lc.y;
				ls.z = lc.z;
			}
		}
	}

	if (brightest != -1)
	{
		phd_NormaliseVector(ls.x, ls.y, ls.z, (long*)&ls);

		if (il->init & 4)
		{
			il->dynamic.x += (ls.x - il->dynamic.x) >> 1;
			il->dynamic.y += (ls.y - il->dynamic.y) >> 1;
			il->dynamic.z += (ls.z - il->dynamic.z) >> 1;
			il->dynamicr += uchar((((brightest * dl2->r) >> 13) - il->dynamicr) >> 1);
			il->dynamicg += uchar((((brightest * dl2->g) >> 13) - il->dynamicg) >> 1);
			il->dynamicb += uchar((((brightest * dl2->b) >> 13) - il->dynamicb) >> 1);
		}
		else
		{
			il->dynamic.x = ls.x;
			il->dynamic.y = ls.y;
			il->dynamic.z = ls.z;
			il->dynamicr = uchar((brightest * dl2->r) >> 13);
			il->dynamicg = uchar((brightest * dl2->g) >> 13);
			il->dynamicb = uchar((brightest * dl2->b) >> 13);
		}

		LPos[2].x = il->dynamic.x >> 2;
		LPos[2].y = il->dynamic.y >> 2;
		LPos[2].z = il->dynamic.z >> 2;
		LightCol[M02] = il->dynamicr << cShift;
		LightCol[M12] = il->dynamicg << cShift;
		LightCol[M22] = il->dynamicb << cShift;
	}

	if (ambience > 255)
		ambience = 255;

	if (il->init & 8)
		il->ambient += (ambience - il->ambient) >> 3;
	else
	{
		il->ambient = (uchar)ambience;
		il->init |= 8;
	}

	smcr = il->ambient;
	smcg = il->ambient;
	smcb = il->ambient;

	for (int i = 0; i < 3; i++)
	{
		LPos[i].x <<= 2;
		LPos[i].y <<= 2;
		LPos[i].z <<= 2;
		x = (LPos[i].x * w2v_matrix[M00] + LPos[i].y * w2v_matrix[M01] + LPos[i].z * w2v_matrix[M02]) >> W2V_SHIFT;
		y = (LPos[i].x * w2v_matrix[M10] + LPos[i].y * w2v_matrix[M11] + LPos[i].z * w2v_matrix[M12]) >> W2V_SHIFT;
		z = (LPos[i].x * w2v_matrix[M20] + LPos[i].y * w2v_matrix[M21] + LPos[i].z * w2v_matrix[M22]) >> W2V_SHIFT;
		LPos[i].x = x;
		LPos[i].y = y;
		LPos[i].z = z;
	}
}
