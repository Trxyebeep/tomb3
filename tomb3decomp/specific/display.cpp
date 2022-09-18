#include "../tomb3/pch.h"
#include "display.h"
#include "../3dsystem/3d_gen.h"

void setup_screen_size()
{
	DISPLAYMODE* dm;
	long w, h, sw, sh;

	dm = App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode;
	w = dm[App.DXConfigPtr->nVMode].w;
	h = dm[App.DXConfigPtr->nVMode].h;
	sw = long(w * screen_sizer);
	sh = long(h * screen_sizer);

	if (sw > w)
		sw = w;

	if (sh > h)
		sh = h;

	phd_InitWindow((w - sw) / 2, (h - sh) / 2, sw, sh, 20, farz, 80, w, h);
	DumpX = short((w - sw) / 2);
	DumpY = short((h - sh) / 2);
	DumpWidth = (short)sw;
	DumpHeight = (short)sh;
	GtFullScreenClearNeeded = 1;
	BarsWidth = sw / 5;

	if (BarsWidth > 100)
		BarsWidth = 100;
}

void IncreaseScreenSize()
{
	if (screen_sizer != 1)
	{
		screen_sizer += 0.08F;

		if (screen_sizer > 1)
			screen_sizer = 1;

		game_sizer = screen_sizer;
		setup_screen_size();
	}
}

void DecreaseScreenSize()
{
	if (screen_sizer != 0.44F)
	{
		screen_sizer -= 0.08F;

		if (screen_sizer < 0.44F)
			screen_sizer = 0.44F;

		game_sizer = screen_sizer;
		setup_screen_size();
	}
}

void inject_display(bool replace)
{
	INJECT(0x00475800, setup_screen_size, replace);
	INJECT(0x00475720, IncreaseScreenSize, replace);
	INJECT(0x00475790, DecreaseScreenSize, replace);
}
