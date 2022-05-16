#include "../tomb3/pch.h"
#include "scalespr.h"
#include "3d_gen.h"

ulong TextLight[8] =
{
	0xFFFFFF,
	0x00B0B0,
	0xA0A0A0,
	0x6060FF,
	0xFF8080,
	0x4080C0,
	0x64D1B6,
	0xC0FFC0
};

ulong TextDark[8] =
{
	0x808080,
	0x005050,
	0x181818,
	0x000018,
	0x180000,
	0x001040,
	0x1320B6,
	0xC0FFC0
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

void inject_scalespr(bool replace)
{
	INJECT(0x0040D060, ins_room_sprite, replace);
	INJECT(0x0040D1C0, S_DrawScreenSprite2d, replace);
}
