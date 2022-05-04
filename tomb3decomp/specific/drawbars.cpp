#include "../tomb3/pch.h"
#include "drawbars.h"
#include "hwrender.h"
#include "../3dsystem/3d_gen.h"

void S_DrawHealthBar(long percent)
{
	long oldAlpha, z, w;

	oldAlpha = GlobalAlpha;
	GlobalAlpha = 0xFF000000;
	HWR_EnableZBuffer(1, 1);
	z = phd_znear + 50;
	bBlueEffect = 0;
	w = BarsWidth * percent / 100;

	//black box
	InsertLine(6, 7, BarsWidth + 9, 7, z, 0, 0);
	InsertLine(6, 8, BarsWidth + 9, 8, z, 0, 0);
	InsertLine(6, 9, BarsWidth + 9, 9, z, 0, 0);
	InsertLine(6, 10, BarsWidth + 9, 10, z, 0, 0);
	InsertLine(6, 11, BarsWidth + 9, 11, z, 0, 0);
	InsertLine(6, 12, BarsWidth + 9, 12, z, 0, 0);
	InsertLine(6, 13, BarsWidth + 9, 13, z, 0, 0);

	//gray frame
	z = phd_znear + 40;
	InsertLine(6, 14, BarsWidth + 11, 14, z, 0x7F7F7F, 0x7F7F7F);
	InsertLine(BarsWidth + 10, 6, BarsWidth + 10, 14, z, 0x7F7F7F, 0x7F7F7F);

	//white frame
	z = phd_znear + 30;
	InsertLine(6, 6, BarsWidth + 10, 6, z, 0xFFFFFF, 0xFFFFFF);
	InsertLine(6, 14, 6, 6, z, 0xFFFFFF, 0xFFFFFF);

	if (w)
	{
		w += 8;
		z = phd_znear + 20;

		if (lara.poisoned)
		{
			InsertLine(8, 9, w, 9, z, 0xFFFF00, 0xFFFF00);
			InsertLine(8, 8, w, 8, z, 0x4F4F00, 0x4F4F00);
			InsertLine(8, 10, w, 10, z, 0x7F7F00, 0x7F7F00);
			InsertLine(8, 11, w, 11, z, 0x7F7F00, 0x7F7F00);
			InsertLine(8, 12, w, 12, z, 0x4F4F00, 0x4F4F00);
		}
		else
		{
			InsertLine(8, 9, w, 9, z, 0xFF0000, 0xFF0000);
			InsertLine(8, 8, w, 8, z, 0x4F0000, 0x4F0000);
			InsertLine(8, 10, w, 10, z, 0x7F0000, 0x7F0000);
			InsertLine(8, 11, w, 11, z, 0x7F0000, 0x7F0000);
			InsertLine(8, 12, w, 12, z, 0x4F0000, 0x4F0000);
		}
	}

	GlobalAlpha = oldAlpha;
}

void S_DrawDashBar(long percent)
{
	long oldAlpha, w, x, z;

	oldAlpha = GlobalAlpha;
	GlobalAlpha = 0xFF000000;
	HWR_EnableZBuffer(1, 1);
	bBlueEffect = 0;
	w = BarsWidth * percent / 100;
	x = DumpWidth - BarsWidth - 10;
	z = phd_znear + 50;

	//black box
	InsertLine(x - 2, 19, x + BarsWidth + 1, 19, z, 0, 0);
	InsertLine(x - 2, 20, x + BarsWidth + 1, 20, z, 0, 0);
	InsertLine(x - 2, 21, x + BarsWidth + 1, 21, z, 0, 0);
	InsertLine(x - 2, 22, x + BarsWidth + 1, 22, z, 0, 0);
	InsertLine(x - 2, 23, x + BarsWidth + 1, 23, z, 0, 0);
	InsertLine(x - 2, 24, x + BarsWidth + 1, 24, z, 0, 0);
	InsertLine(x - 2, 25, x + BarsWidth + 1, 25, z, 0, 0);

	//gray frame
	z = phd_znear + 40;
	InsertLine(x - 2, 26, x + BarsWidth + 3, 26, z, 0x7F7F7F, 0x7F7F7F);
	InsertLine(x + BarsWidth + 2, 18, x + BarsWidth + 2, 26, z, 0x7F7F7F, 0x7F7F7F);

	//white frame
	z = phd_znear + 30;
	InsertLine(x - 2, 18, x + BarsWidth + 2, 18, z, 0xFFFFFF, 0xFFFFFF);
	InsertLine(x - 2, 26, x - 2, 18, z, 0xFFFFFF, 0xFFFFFF);

	if (w)
	{
		z = phd_znear + 20;
		InsertLine(x, 21, w + x, 21, z, 0xFF00, 0xFF00);
		InsertLine(x, 20, w + x, 20, z, 0x4F00, 0x4F00);
		InsertLine(x, 22, w + x, 22, z, 0x7F00, 0x7F00);
		InsertLine(x, 23, w + x, 23, z, 0x7F00, 0x7F00);
		InsertLine(x, 24, w + x, 24, z, 0x4F00, 0x4F00);
	}

	GlobalAlpha = oldAlpha;
}

void S_DrawAirBar(long percent)
{
	long oldAlpha, w, x, z;

	oldAlpha = GlobalAlpha;
	GlobalAlpha = 0xFF000000;
	HWR_EnableZBuffer(1, 1);
	bBlueEffect = 0;
	w = BarsWidth * percent / 100;
	x = DumpWidth - BarsWidth - 10;
	z = phd_znear + 50;

	//black box
	InsertLine(x - 2, 7, x + BarsWidth + 1, 7, z, 0, 0);
	InsertLine(x - 2, 8, x + BarsWidth + 1, 8, z, 0, 0);
	InsertLine(x - 2, 9, x + BarsWidth + 1, 9, z, 0, 0);
	InsertLine(x - 2, 10, x + BarsWidth + 1, 10, z, 0, 0);
	InsertLine(x - 2, 11, x + BarsWidth + 1, 11, z, 0, 0);
	InsertLine(x - 2, 12, x + BarsWidth + 1, 12, z, 0, 0);
	InsertLine(x - 2, 13, x + BarsWidth + 1, 13, z, 0, 0);

	//gray frame
	z = phd_znear + 40;
	InsertLine(x - 2, 14, x + BarsWidth + 3, 14, z, 0x7F7F7F, 0x7F7F7F);
	InsertLine(x + BarsWidth + 2, 6, x + BarsWidth + 2, 14, z, 0x7F7F7F, 0x7F7F7F);

	//white frame
	z = phd_znear + 30;
	InsertLine(x - 2, 6, x + BarsWidth + 2, 6, z, 0xFFFFFF, 0xFFFFFF);
	InsertLine(x - 2, 14, x - 2, 6, z, 0xFFFFFF, 0xFFFFFF);

	if (percent > 0)
	{
		z = phd_znear + 20;
		InsertLine(x, 9, w + x, 9, z, 0xFF, 0xFF);
		InsertLine(x, 8, w + x, 8, z, 0x4F, 0x4F);
		InsertLine(x, 10, w + x, 10, z, 0x7F, 0x7F);
		InsertLine(x, 11, w + x, 11, z, 0x7F, 0x7F);
		InsertLine(x, 12, w + x, 12, z, 0x4F, 0x4F);
	}

	GlobalAlpha = oldAlpha;
}

void S_DrawColdBar(long percent)
{
	long oldAlpha, w, z;

	oldAlpha = GlobalAlpha;
	GlobalAlpha = 0xFF000000;
	HWR_EnableZBuffer(1, 1);
	bBlueEffect = 0;
	w = BarsWidth * percent / 100;
	z = phd_znear + 50;

	//black box
	InsertLine(6, 19, BarsWidth + 9, 19, z, 0, 0);
	InsertLine(6, 20, BarsWidth + 9, 20, z, 0, 0);
	InsertLine(6, 21, BarsWidth + 9, 21, z, 0, 0);
	InsertLine(6, 22, BarsWidth + 9, 22, z, 0, 0);
	InsertLine(6, 23, BarsWidth + 9, 23, z, 0, 0);
	InsertLine(6, 24, BarsWidth + 9, 24, z, 0, 0);
	InsertLine(6, 25, BarsWidth + 9, 25, z, 0, 0);

	//gray frame
	z = phd_znear + 40;
	InsertLine(6, 26, BarsWidth + 11, 26, z, 0x7F7F7F, 0x7F7F7F);
	InsertLine(BarsWidth + 10, 18, BarsWidth + 10, 26, z, 0x7F7F7F, 0x7F7F7F);

	//white frame
	z = phd_znear + 30;
	InsertLine(6, 18, BarsWidth + 10, 18, z, 0xFFFFFF, 0xFFFFFF);
	InsertLine(6, 26, 6, 18, z, 0xFFFFFF, 0xFFFFFF);

	if (w)
	{
		w += 8;
		z = phd_znear + 20;
		InsertLine(8, 21, w, 21, z, 0xFFFF, 0xFFFF);
		InsertLine(8, 20, w, 20, z, 0x4F4F, 0x4F4F);
		InsertLine(8, 22, w, 22, z, 0x7F7F, 0x7F7F);
		InsertLine(8, 23, w, 23, z, 0x7F7F, 0x7F7F);
		InsertLine(8, 24, w, 24, z, 0x4F4F, 0x4F4F);
	}

	GlobalAlpha = oldAlpha;
}

void inject_drawbars(bool replace)
{
	INJECT(0x00475C70, S_DrawHealthBar, replace);
	INJECT(0x00475A00, S_DrawDashBar, replace);
	INJECT(0x00475F60, S_DrawAirBar, replace);
	INJECT(0x004761C0, S_DrawColdBar, replace);
}
