#include "../tomb3/pch.h"
#include "display.h"
#include "../3dsystem/3d_gen.h"

static long fade_value = 0x100000;
static long fade_limit = 0x100000;
static long fade_adder = 0x8000;

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

void TempVideoAdjust(long a, double sizer)
{
	VidSizeLocked = 1;

	if (sizer != screen_sizer)
	{
		screen_sizer = sizer;
		setup_screen_size();
	}
}

void TempVideoRemove()
{
	VidSizeLocked = 0;

	if (screen_sizer != game_sizer)
	{
		screen_sizer = game_sizer;
		setup_screen_size();
	}
}

void S_FadeInInventory(long fade)
{
//	if (Inventory_Mode != INV_TITLE_MODE)
		//empty function call here

	if (fade)
	{
		fade_value = 0x100000;
		fade_limit = 0x180000;
		fade_adder = 0x8000;
	}
}

void S_FadeOutInventory(long fade)
{
	if (fade)
	{
		fade_value = 0x180000;
		fade_limit = 0x100000;
		fade_adder = -0x8000;
	}
}

void inject_display(bool replace)
{
	INJECT(0x00475800, setup_screen_size, replace);
	INJECT(0x00475720, IncreaseScreenSize, replace);
	INJECT(0x00475790, DecreaseScreenSize, replace);
	INJECT(0x00475910, TempVideoAdjust, replace);
	INJECT(0x00475950, TempVideoRemove, replace);
	INJECT(0x00475990, S_FadeInInventory, replace);
	INJECT(0x004759D0, S_FadeOutInventory, replace);
}
