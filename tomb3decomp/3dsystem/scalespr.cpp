#include "../tomb3/pch.h"
#include "scalespr.h"
#include "3d_gen.h"

ulong TextLight[12] =
{
	0xFFFFFF,
	0x00B0B0,
	0xA0A0A0,
	0x6060FF,
	0xFF8080,
	0x4080C0,
	0x64D1B6,
	0xC0FFC0,
	0xFFFFFF,
	0xFF00FF,
	0xFF00FF,
	0xFF00FF
};

ulong TextDark[12] =
{
	0x808080,
	0x005050,
	0x181818,
	0x000018,
	0x180000,
	0x001040,
	0x1320B6,
	0xC0FFC0,
	0xFFFFFF,
	0x3F003F,
	0x3F003F,
	0x3F003F
};

short* ins_room_sprite(short* objptr, long num)
{
	PHD_VBUF* vtx;
	PHDSPRITESTRUCT* sprite;
	long zv, x1, y1, x2, y2;

	while (num)
	{
		vtx = &vbuf[*objptr];

		if (vtx->clip >= 0)
		{
			sprite = &phdspriteinfo[objptr[1]];
			zv = (long)vtx->zv / phd_persp;
			x1 = long(((sprite->x1 << 14) + vtx->xv) / zv + phd_centerx);
			y1 = long(((sprite->y1 << 14) + vtx->yv) / zv + phd_centery);
			x2 = long(((sprite->x2 << 14) + vtx->xv) / zv + phd_centerx);
			y2 = long(((sprite->y2 << 14) + vtx->yv) / zv + phd_centery);

			if (x2 >= phd_left && y2 >= phd_top && x1 < phd_right && y1 < phd_bottom)
				InsertSprite((long)vtx->zv, x1, y1, x2, y2, objptr[1], vtx->g, -1, 10, 0);
		}

		objptr += 2;
		num--;
	}

	return objptr;
}

void S_DrawScreenSprite2d(long x, long y, long z, long scaleH, long scaleV, short sprnum, short shade, ushort flags)
{
	PHDSPRITESTRUCT* sprite;
	long x1, y1, x2, y2, r, g, b, shade1, shade2;

	sprite = &phdspriteinfo[sprnum];
	y1 = y + ((scaleV * sprite->y1) >> 16);
	x1 = x + ((scaleH * sprite->x1) >> 16);
	x2 = x + ((scaleH * sprite->x2) >> 16);
	y2 = y + ((scaleV * sprite->y2) >> 16);

	if (x2 >= 0 && y2 >= 0 && x1 < phd_winwidth && y1 < phd_winheight)
	{
		r = (TextLight[shade] >> 3) & 0x1F;
		g = (TextLight[shade] >> 11) & 0x1F;
		b = (TextLight[shade] >> 19) & 0x1f;
		shade1 = r << 10 | g << 5 | b;

		r = (TextDark[shade] >> 3) & 0x1F;
		g = (TextDark[shade] >> 11) & 0x1F;
		b = (TextDark[shade] >> 19) & 0x1f;
		shade2 = r << 10 | g << 5 | b;

		if (flags == 0xFFFF)
			InsertSprite(phd_znear, x1, y1, x2, y2, sprnum, shade1, shade2, 10, 0);
		else
		{
			InsertSprite(phd_znear + 0x28000, x1, y1, x2, y2, sprnum, shade1, shade2, 10, 0);
			InsertSprite(phd_znear + 0x3C000, x1 + 1, y1 + 1, x2 + 1, y2 + 1, sprnum, 0, 0, 10, 0);
		}
	}
}

void S_DrawScreenSprite(long x, long y, long z, long scaleH, long scaleV, short sprnum, short shade, ushort flags)
{
	PHDSPRITESTRUCT* sprite;
	long x1, x2, y1, y2;

	sprite = &phdspriteinfo[sprnum];
	x1 = x + ((scaleH * (sprite->x1 >> 3)) >> 16);
	x2 = x + ((scaleH * (sprite->x2 >> 3)) >> 16);
	y1 = y + ((scaleV * (sprite->y1 >> 3)) >> 16);
	y2 = y + ((scaleV * (sprite->y2 >> 3)) >> 16);

	if (x2 >= 0 && y2 >= 0 && x1 < phd_winwidth && y1 < phd_winheight)
		InsertSprite(z << 3, x1, y1, x2, y2, sprnum, shade, -1, 10, 0);
}

void S_DrawSprite(ulong flags, long x, long y, long z, short sprnum, short shade, short scale)
{
	PHDSPRITESTRUCT* sprite;
	long xv, yv, zv, zop, x1, y1, x2, y2;
	short r, g, b;

	if (flags & 0x1000000)
	{
		x -= w2v_matrix[M03];
		y -= w2v_matrix[M13];
		z -= w2v_matrix[M23];

		if (x < -phd_viewdist || x > phd_viewdist || y < -phd_viewdist || y > phd_viewdist || z < -phd_viewdist || z > phd_viewdist)
			return;

		zv = x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22];

		if (zv < phd_znear || zv >= phd_zfar)
			return;

		xv = x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02];
		yv = x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12];
	}
	else if (x | y | z)
	{
		zv = x * phd_mxptr[M20] + y * phd_mxptr[M21] + z * phd_mxptr[M22] + phd_mxptr[M23];

		if (zv < phd_znear || zv > phd_zfar)
			return;

		xv = x * phd_mxptr[M00] + y * phd_mxptr[M01] + z * phd_mxptr[M02] + phd_mxptr[M03];
		yv = x * phd_mxptr[M10] + y * phd_mxptr[M11] + z * phd_mxptr[M12] + phd_mxptr[M13];
	}
	else
	{
		zv = phd_mxptr[M23];

		if (zv < phd_znear || zv > phd_zfar)
			return;

		xv = phd_mxptr[M03];
		yv = phd_mxptr[M13];
	}

	zop = zv / phd_persp;
	sprite = &phdspriteinfo[sprnum];
	x1 = sprite->x1;
	y1 = sprite->y1;
	x2 = sprite->x2;
	y2 = sprite->y2;

	if (flags & 0x4000000)
	{
		x1 = (scale * x1) << 6;
		y1 = (scale * y1) << 6;
		x2 = (scale * x2) << 6;
		y2 = (scale * y2) << 6;
	}
	else
	{
		x1 <<= 14;
		y1 <<= 14;
		x2 <<= 14;
		y2 <<= 14;
	}

	x1 = phd_centerx + (x1 + xv) / zop;

	if (x1 >= phd_winwidth)
		return;

	y1 = phd_centery + (y1 + yv) / zop;

	if (y1 >= phd_winheight)
		return;

	x2 = phd_centerx + (x2 + xv) / zop;

	if (x2 < 0)
		return;

	y2 = phd_centery + (y2 + yv) / zop;

	if (y2 < 0)
		return;

	if (flags & 0x8000000)
	{
		if (zv > distanceFogValue << 14)
		{
			shade += short((zv >> 14) - distanceFogValue);

			if (shade > 8191)
				return;
		}
	}
	else
		shade = 4096;

	if (flags & 0xFFFFFF)
	{
		shade = short(flags & 0xFFFFFF);
		r = (shade & 0xFF) >> 3;
		g = ((shade >> 8) & 0xFF) >> 3;
		b = ((shade >> 16) & 0xFF) >> 3;
		shade = r << 10 | g << 5 | b;
		InsertSprite(zv, x1, y1, x2, y2, sprnum, shade, -1, 14, 0);
	}
	else
		InsertSprite(zv, x1, y1, x2, y2, sprnum, shade, -1, 10, 0);
}

void inject_scalespr(bool replace)
{
	INJECT(0x0040D060, ins_room_sprite, replace);
	INJECT(0x0040D1C0, S_DrawScreenSprite2d, replace);
	INJECT(0x0040D320, S_DrawScreenSprite, replace);
	INJECT(0x0040CD20, S_DrawSprite, replace);
}
