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

void inject_drawbars(bool replace)
{
	INJECT(0x00475C70, S_DrawHealthBar, replace);
}
