#include "../tomb3/pch.h"
#include "frontend.h"
#include "specific.h"
#include "../3dsystem/3d_gen.h"
#include "display.h"
#include "input.h"
#include "time.h"
#include "fmv.h"
#ifdef TROYESTUFF
#include "output.h"
#include "../3dsystem/hwinsert.h"
#include "../tomb3/tomb3.h"
#endif

ushort S_COLOUR(long r, long g, long b)
{
	return SWR_FindNearestPaletteEntry(game_palette, r, g, b, 0);
}

void S_DrawScreenLine(long x, long y, long z, long w, long h, long c, GOURAUD_FILL* grdptr, ushort f)
{
	InsertLine(x, y, x + w, y + h, phd_znear + 8 * z, (char)c, c);
}

void S_DrawScreenBox(long x, long y, long z, long w, long h, long sprnum, GOURAUD_FILL* grdptr, ushort f)
{
	long sx, sy;

	sx = x + w;
	sy = y + h;
	S_DrawScreenLine(x,			y - 1,	z, w + 1,	0,		15, 0, f);
	S_DrawScreenLine(x + 1,		y,		z, w - 1,	0,		31, 0, f);
	S_DrawScreenLine(sx,		y + 1,	z, 0,		h - 1,	15, 0, f);
	S_DrawScreenLine(sx + 1,	y,		z, 0,		h + 1,	31, 0, f);
	S_DrawScreenLine(x - 1,		y - 1,	z, 0,		h + 1,	15, 0, f);
	S_DrawScreenLine(x,			y,		z, 0,		h - 1,	31, 0, f);
	S_DrawScreenLine(x,			sy,		z, w - 1,	0,		15, 0, f);
	S_DrawScreenLine(x - 1,		sy + 1, z, w + 1,	0,		31, 0, f);
}

void S_DrawScreenFBox(long x, long y, long z, long w, long h, long c, GOURAUD_FILL* grdptr, ushort f)
{
	long adder;
#ifdef TROYESTUFF
	long bx[3];
	long by[3];

	if (tomb3.psx_boxes)
	{
		adder = GetRenderScale(1);
		x += adder;
		y += adder;
	}
	else
		adder = 2;
#else
	adder = 1;
#endif

	w += adder;
	h += adder;

#ifdef TROYESTUFF
	if (tomb3.psx_boxes && grdptr)
	{
		bx[0] = x;
		by[0] = y;
		bx[2] = x + w;
		by[2] = y + h;
		bx[1] = (bx[0] + bx[2]) / 2;
		by[1] = (by[0] + by[2]) / 2;
		HWI_InsertGourQuad_Sorted(bx[0], by[0], bx[1], by[1], phd_znear + 0x50000,
			grdptr->clr[0][0], grdptr->clr[0][1],
			grdptr->clr[0][2], grdptr->clr[0][3]);
		HWI_InsertGourQuad_Sorted(bx[1], by[0], bx[2], by[1], phd_znear + 0x50000,
			grdptr->clr[1][0], grdptr->clr[1][1],
			grdptr->clr[1][2], grdptr->clr[1][3]);
		HWI_InsertGourQuad_Sorted(bx[1], by[1], bx[2], by[2], phd_znear + 0x50000,
			grdptr->clr[2][0], grdptr->clr[2][1],
			grdptr->clr[2][2], grdptr->clr[2][3]);
		HWI_InsertGourQuad_Sorted(bx[0], by[1], bx[1], by[2], phd_znear + 0x50000,
			grdptr->clr[3][0], grdptr->clr[3][1],
			grdptr->clr[3][2], grdptr->clr[3][3]);

		return;
	}
#endif

	InsertTransQuad(phd_winxmin + x, phd_winymin + y, w, h, phd_znear + 0x50000);
}

void S_FinishInventory()
{
	if (Inventory_Mode != INV_TITLE_MODE)
		TempVideoRemove();
}

void S_Wait(long nFrames, long skip)
{
	long s;

	if (skip)
	{
		while (nFrames > 0)
		{
			if (!input)
				break;

			S_UpdateInput();

			do { s = Sync(); } while (!s);

			nFrames -= s;
		}
	}

	while (nFrames > 0)
	{
		S_UpdateInput();

		if (skip && input)
			break;

		do { s = Sync(); } while (!s);

		nFrames -= s;
	}
}

long S_PlayFMV(char* name)
{
	return FMV_Play(name);
}

long S_IntroFMV(char* name1, char* name2)
{
	return FMV_PlayIntro(name1, name2);
}

void inject_frontend(bool replace)
{
	INJECT(0x004835E0, S_COLOUR, replace);
	INJECT(0x00483610, S_DrawScreenLine, replace);
	INJECT(0x00483650, S_DrawScreenBox, replace);
	INJECT(0x00483770, S_DrawScreenFBox, replace);
	INJECT(0x004837B0, S_FinishInventory, replace);
	INJECT(0x004837D0, S_Wait, replace);
	INJECT(0x00483830, S_PlayFMV, replace);
	INJECT(0x00483840, S_IntroFMV, replace);
}
