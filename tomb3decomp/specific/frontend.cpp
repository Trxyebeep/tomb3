#include "../tomb3/pch.h"
#include "frontend.h"
#include "specific.h"
#include "../3dsystem/3d_gen.h"
#include "display.h"
#include "input.h"
#include "time.h"

ushort S_COLOUR(long r, long g, long b)
{
	return SWR_FindNearestPaletteEntry(game_palette, r, g, b, 0);
}

void S_DrawScreenLine(long x, long y, long z, long w, long h, long c, ushort* grdptr, ushort f)
{
	InsertLine(x, y, x + w, y + h, phd_znear + 8 * z, (char)c, c);
}

void S_DrawScreenBox(long x, long y, long z, long w, long h, long sprnum, ushort* grdptr, ushort f)
{
	S_DrawScreenLine(x, y - 1, z, w + 1, 0, 15, 0, f);
	S_DrawScreenLine(x + 1, y, z, w - 1, 0, 31, 0, f);
	S_DrawScreenLine(x + w, y + 1, z, 0, h - 1, 15, 0, f);
	S_DrawScreenLine(x + w + 1, y, z, 0, h + 1, 31, 0, f);
	S_DrawScreenLine(x - 1, y - 1, z, 0, h + 1, 15, 0, f);
	S_DrawScreenLine(x, y, z, 0, h - 1, 31, 0, f);
	S_DrawScreenLine(x, h + y, z, w - 1, 0, 15, 0, f);
	S_DrawScreenLine(x - 1, h + y + 1, z, w + 1, 0, 31, 0, f);
}

void S_DrawScreenFBox(long x, long y, long z, long w, long h, long c, ushort* grdptr, ushort f)
{
	InsertTransQuad(phd_winxmin + x, phd_winymin + y, w + 1, h + 1, phd_znear + 0x50000);
}

void S_FinishInventory()
{
	if (Inventory_Mode != 1)
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

void inject_frontend(bool replace)
{
	INJECT(0x004835E0, S_COLOUR, replace);
	INJECT(0x00483610, S_DrawScreenLine, replace);
	INJECT(0x00483650, S_DrawScreenBox, replace);
	INJECT(0x00483770, S_DrawScreenFBox, replace);
	INJECT(0x004837B0, S_FinishInventory, replace);
	INJECT(0x004837D0, S_Wait, replace);
}
