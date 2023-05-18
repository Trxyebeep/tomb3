#include "../tomb3/pch.h"
#include "scalespr.h"
#include "3d_gen.h"
#include "hwinsert.h"
#include "../specific/file.h"
#include "../specific/winmain.h"
#include "../specific/output.h"
#include "../tomb3/tomb3.h"

ulong TextLight[12] =
{
	0xFFFFFF,
	0xB0B000,
	0xA0A0A0,
	0xFF6060,
	0x8080FF,
	0xC08040,
	0xB6D164,
	0xC0FFC0,
	0xFFFFFF,
	0xFF00FF,
	0xFF00FF,
	0xFF00FF
};

ulong TextDark[12] =
{
	0x808080,
	0x505000,
	0x181818,
	0x180000,
	0x000018,
	0x401000,
	0xB62013,
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
			x1 = long(((sprite->x1 << W2V_SHIFT) + vtx->xv) / zv + phd_centerx);
			y1 = long(((sprite->y1 << W2V_SHIFT) + vtx->yv) / zv + phd_centery);
			x2 = long(((sprite->x2 << W2V_SHIFT) + vtx->xv) / zv + phd_centerx);
			y2 = long(((sprite->y2 << W2V_SHIFT) + vtx->yv) / zv + phd_centery);

			if (x2 >= phd_left && y2 >= phd_top && x1 < phd_right && y1 < phd_bottom)
				InsertSprite((long)vtx->zv, x1, y1, x2, y2, objptr[1], vtx->color, -1, DT_POLY_WGT, 0);
		}

		objptr += 2;
		num--;
	}

	return objptr;
}

static void SetPSXTextColor()
{
	TextLight[0] = 0x808080;
	TextLight[1] = 0xB0B000;
	TextLight[2] = 0xA0A0A0;
	TextLight[3] = 0xFF6060;
	TextLight[4] = 0x8080FF;
	TextLight[5] = 0xC08040;
	TextLight[6] = 0xC0C060;
	TextLight[7] = 0x00C000;
	TextLight[8] = 0x00C000;
	TextLight[9] = 0x800000;
	TextLight[10] = 0x808000;
	TextLight[11] = 0x804040;

	TextDark[0] = 0x808080;
	TextDark[1] = 0x505000;
	TextDark[2] = 0x181818;
	TextDark[3] = 0x180000;
	TextDark[4] = 0x000018;
	TextDark[5] = 0x401000;
	TextDark[6] = 0x101010;
	TextDark[7] = 0x004000;
	TextDark[8] = 0x004000;
	TextDark[9] = 0x400000;
	TextDark[10] = 0x404000;
	TextDark[11] = 0x401010;
}

void S_DrawScreenSprite2d(long x, long y, long z, long scaleH, long scaleV, short sprnum, short shade, ushort flags)
{
	PHDSPRITESTRUCT* sprite;
	long x1, y1, x2, y2, c1, c2, p;
	static bool set = 0;

	if (!set)
	{
		if (tomb3.psx_text_colors)
			SetPSXTextColor();

		set = 1;
	}

	sprite = &phdspriteinfo[sprnum];
	x1 = x + ((scaleH * sprite->x1) >> 16);
	y1 = y + ((scaleV * sprite->y1) >> 16);
	x2 = x + ((scaleH * sprite->x2) >> 16);
	y2 = y + ((scaleV * sprite->y2) >> 16);

	if (x2 >= 0 && y2 >= 0 && x1 < phd_winwidth && y1 < phd_winheight)
	{
		c1 = TextLight[shade];
		c2 = TextDark[shade];

		if (flags == 0xFFFF)
			InsertSprite(phd_znear, x1, y1, x2, y2, sprnum, c1, c2, DT_POLY_WGT, 0);
		else
		{
			p = GetRenderScale(1);
			InsertSprite(phd_znear + (10 << W2V_SHIFT), x1, y1, x2, y2, sprnum, c1, c2, DT_POLY_WGT, 2);
			InsertSprite(phd_znear + (15 << W2V_SHIFT), x1 + p, y1 + p, x2 + p, y2 + p, sprnum, 0, 0, DT_POLY_WGT, 2);
		}
	}
}

void S_DrawScreenSprite(long x, long y, long z, long scaleH, long scaleV, short sprnum, long shade, ushort flags)
{
	PHDSPRITESTRUCT* sprite;
	long x1, x2, y1, y2;

	sprite = &phdspriteinfo[sprnum];
	x1 = x + ((scaleH * (sprite->x1 >> 3)) >> 16);
	x2 = x + ((scaleH * (sprite->x2 >> 3)) >> 16);
	y1 = y + ((scaleV * (sprite->y1 >> 3)) >> 16);
	y2 = y + ((scaleV * (sprite->y2 >> 3)) >> 16);

	if (x2 >= 0 && y2 >= 0 && x1 < phd_winwidth && y1 < phd_winheight)
		InsertSprite(z << 3, x1, y1, x2, y2, sprnum, shade, -1, DT_POLY_WGT, 0);
}

void S_DrawSprite(ulong flags, long x, long y, long z, short sprnum, short shade, short scale)
{
	PHDSPRITESTRUCT* sprite;
	long xv, yv, zv, zop, x1, y1, x2, y2, r, g, b, c;

	if (flags & SPR_ABS)
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

	if (flags & SPR_SCALE)
	{
		x1 = (scale * x1) << 6;
		y1 = (scale * y1) << 6;
		x2 = (scale * x2) << 6;
		y2 = (scale * y2) << 6;
	}
	else
	{
		x1 <<= W2V_SHIFT;
		y1 <<= W2V_SHIFT;
		x2 <<= W2V_SHIFT;
		y2 <<= W2V_SHIFT;
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

	if (flags & SPR_SHADE)
	{
		if (zv > distanceFogValue << W2V_SHIFT)
		{
			shade += short((zv >> W2V_SHIFT) - distanceFogValue);

			if (shade > 8191)
				return;
		}
	}
	else
		shade = 4096;

	if (flags & 0xFFFFFF)
	{
		c = flags & 0xFFFFFF;
		r = RGB_GETBLUE(c);
		g = RGB_GETGREEN(c);
		b = RGB_GETRED(c);
		c = RGB_MAKE(r, g, b);
		InsertSprite(zv, x1, y1, x2, y2, sprnum, c, -1, DT_POLY_WGTA, 0);
	}
	else
	{
		r = ((shade >> 10) & 0x1F) << 3;
		g = ((shade >> 5) & 0x1F) << 3;
		b = (shade & 0x1F) << 3;
		c = RGB_MAKE(r, g, b);
		InsertSprite(zv, x1, y1, x2, y2, sprnum, c, -1, DT_POLY_WGT, 0);
	}
}
