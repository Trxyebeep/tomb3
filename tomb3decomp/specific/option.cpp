#include "../tomb3/pch.h"
#include "option.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "hwrender.h"
#include "dxshell.h"
#include "drawprimitive.h"
#include "specific.h"
#include "../game/sound.h"
#ifdef TROYESTUFF
#include "../tomb3/tomb3.h"
#include "output.h"
#include "smain.h"
#endif

static GLOBE_LEVEL GlobeLevelAngles[7] =
{
	{ -1536, -7936, 1536, GT_LSLONDON },
	{ 1024, -512, -256, GT_LSSPAC },
	{ 2560, 21248, -4096, GT_LSNEVADA },
	{ -3328, 29440, 1024, GT_LSPERU },
	{ 3072, -20992, 6400, GT_LSLONDON },
	{ -5120, -15360, -18688, GT_LSANTARC },
	{ 0, 0, 0, 0 },
};

static TEXTSTRING* dtext[DT_NUMT];
static TEXTSTRING* stext[4];

long GetRenderWidth()
{
	return phd_winwidth;
}

long GetRenderHeight()
{
	return phd_winheight;
}

#ifdef TROYESTUFF

#define LINEW	100
#define LINEH	13
#define LINEY(n) (GetRenderScale(n * LINEH))

void do_detail_option(INVENTORY_ITEM* item)
{
	DIRECT3DINFO* dinfo;
	DISPLAYMODE* dm;
	DISPLAYMODE* cdm;
	static RES_TXT resolutions[30];
	static long selected_res;
	static long selection = DOP_NOPTS - 1;
	long nSel, w, tW, oldRes;
	static char available[DOP_NOPTS];
	char gtxt[8];
	bool save;

	save = 0;
	nSel = DT_NUMT - DOP_NOPTS;
	tW = GetRenderScale(LINEW);
	w = GetRenderWidth() / 2 - tW;
	dinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];

	if (!dtext[DT_GAMMA])
	{
		cdm = &dinfo->DisplayMode[App.DXConfigPtr->nVMode];

		for (int i = 0; i < dinfo->nDisplayMode; i++)
		{
			dm = &dinfo->DisplayMode[i];

			if (dm->bpp == cdm->bpp)
			{
				sprintf(resolutions[i].res, "%dx%d", dm->w, dm->h);

				if (dm->w == cdm->w && dm->h == cdm->h)
					selected_res = i;
			}
		}

		//option names
		dtext[DT_EMPTY] = T_Print(0, -LINEY(5), 0, " ");
		dtext[DT_VIDEOTITLE] = T_Print(0, -LINEY(5) + GetRenderScale(2), 0, GF_PCStrings[PCSTR_VIDEOTITLE]);
		dtext[DT_RESOLUTION] = T_Print(w, -LINEY(4), 0, GF_PCStrings[PCSTR_RESOLUTION]);
		dtext[DT_ZBUFFER] = T_Print(w, -LINEY(3), 0, GF_PCStrings[PCSTR_ZBUFFER]);
		dtext[DT_FILTER] = T_Print(w, -LINEY(2), 0, GF_PCStrings[PCSTR_FILTERING]);
		dtext[DT_DITHER] = T_Print(w, -LINEY(1), 0, GF_PCStrings[PCSTR_DITHER]);
		dtext[DT_TRUEALPHA] = T_Print(w, LINEY(0), 0, GF_PCStrings[PCSTR_TRUEALPHA]);
		dtext[DT_GAMMA] = T_Print(w, LINEY(1), 0, GF_PCStrings[PCSTR_SKY]);
		dtext[DT_FOOTPRINTS] = T_Print(w, LINEY(2), 0, "Footprints");
		dtext[DT_SHADOW] = T_Print(w, LINEY(3), 0, "Shadow mode");
		dtext[DT_BARS] = T_Print(w, LINEY(4), 0, "Bar mode");
		dtext[DT_PICKUP] = T_Print(w, LINEY(5), 0, "Pickup display");
		
		T_AddBackground(dtext[DT_EMPTY], short(tW << 1), (short)GetRenderScale((DOP_NOPTS + 1) * LINEH), 0, 0, 48, 0, 0, 0);
		T_AddOutline(dtext[DT_EMPTY], 1, 15, 0, 0);
		T_CentreH(dtext[DT_EMPTY], 1);
		T_CentreV(dtext[DT_EMPTY], 1);

		T_AddBackground(dtext[DT_VIDEOTITLE], short(tW << 1) - 4, 0, 0, 0, 48, 0, 0, 0);
		T_AddOutline(dtext[DT_VIDEOTITLE], 1, 4, 0, 0);
		T_CentreH(dtext[DT_VIDEOTITLE], 1);
		T_CentreV(dtext[DT_VIDEOTITLE], 1);

		T_CentreV(dtext[DT_RESOLUTION], 1);
		T_CentreV(dtext[DT_ZBUFFER], 1);
		T_CentreV(dtext[DT_FILTER], 1);
		T_CentreV(dtext[DT_DITHER], 1);
		T_CentreV(dtext[DT_TRUEALPHA], 1);
		T_CentreV(dtext[DT_GAMMA], 1);
		T_CentreV(dtext[DT_FOOTPRINTS], 1);
		T_CentreV(dtext[DT_SHADOW], 1);
		T_CentreV(dtext[DT_BARS], 1);
		T_CentreV(dtext[DT_PICKUP], 1);

		//actual options
		dtext[DT_OP_RESOLUTION] = T_Print(w + tW, -LINEY(4), 0, resolutions[selected_res].res);

		if (App.lpZBuffer)
			dtext[DT_OP_ZBUFFER] = T_Print(w + tW, -LINEY(3), 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_ZBUFFER] = T_Print(w + tW, -LINEY(3), 0, GF_PCStrings[PCSTR_OFF]);

		if (HWConfig.nFilter == 2)
			dtext[DT_OP_FILTER] = T_Print(w + tW, -LINEY(2), 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_FILTER] = T_Print(w + tW, -LINEY(2), 0, GF_PCStrings[PCSTR_OFF]);

		if (HWConfig.Dither)
			dtext[DT_OP_DITHER] = T_Print(w + tW, -LINEY(1), 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_DITHER] = T_Print(w + tW, -LINEY(1), 0, GF_PCStrings[PCSTR_OFF]);

		if (HWConfig.TrueAlpha)
			dtext[DT_OP_TRUEALPHA] = T_Print(w + tW, LINEY(0), 0, GF_PCStrings[PCSTR_OFF]);
		else
			dtext[DT_OP_TRUEALPHA] = T_Print(w + tW, LINEY(0), 0, GF_PCStrings[PCSTR_ON]);

		sprintf(gtxt, "%d", (ulong)GammaOption);
		dtext[DT_OP_GAMMA] = T_Print(w + tW, LINEY(1), 0, gtxt);

		if (tomb3.footprints)
			dtext[DT_OP_FOOTPRINTS] = T_Print(w + tW, LINEY(2), 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_FOOTPRINTS] = T_Print(w + tW, LINEY(2), 0, GF_PCStrings[PCSTR_OFF]);

		if (tomb3.shadow_mode == SHADOW_ORIGINAL)
			dtext[DT_OP_SHADOW] = T_Print(w + tW, LINEY(3), 0, "Original");
		else
			dtext[DT_OP_SHADOW] = T_Print(w + tW, LINEY(3), 0, "PSX");

		if (tomb3.bar_mode == BAR_ORIGINAL)
			dtext[DT_OP_BARS] = T_Print(w + tW, LINEY(4), 0, "Original");
		else
			dtext[DT_OP_BARS] = T_Print(w + tW, LINEY(4), 0, "PSX");

		if (tomb3.pickup_display)
			dtext[DT_OP_PICKUP] = T_Print(w + tW, LINEY(5), 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_PICKUP] = T_Print(w + tW, LINEY(5), 0, GF_PCStrings[PCSTR_OFF]);

		T_CentreV(dtext[DT_OP_RESOLUTION], 1);
		T_CentreV(dtext[DT_OP_ZBUFFER], 1);
		T_CentreV(dtext[DT_OP_FILTER], 1);
		T_CentreV(dtext[DT_OP_DITHER], 1);
		T_CentreV(dtext[DT_OP_TRUEALPHA], 1);
		T_CentreV(dtext[DT_OP_GAMMA], 1);
		T_CentreV(dtext[DT_OP_FOOTPRINTS], 1);
		T_CentreV(dtext[DT_OP_SHADOW], 1);
		T_CentreV(dtext[DT_OP_BARS], 1);
		T_CentreV(dtext[DT_OP_PICKUP], 1);

		for (int i = 0; i < DOP_NOPTS; i++)
			available[i] = 1;

		if (dinfo->bHardware)
		{
			T_ChangeText(dtext[DT_OP_TRUEALPHA], GF_PCStrings[PCSTR_SPARE8]);
			available[DOP_TRUEALPHA] = 0;
		}
		else
		{
			T_ChangeText(dtext[DT_OP_DITHER], GF_PCStrings[PCSTR_SPARE8]);
			T_ChangeText(dtext[DOP_ZBUFFER], GF_PCStrings[PCSTR_SPARE8]);
			available[DOP_DITHER] = 0;
			available[DOP_ZBUFFER] = 0;

			if (App.DXConfigPtr->MMX)
			{
				T_ChangeText(dtext[DT_OP_TRUEALPHA], GF_PCStrings[PCSTR_SPARE8]);
				available[DOP_TRUEALPHA] = 0;
			}
			else
			{
				T_ChangeText(dtext[DT_OP_FILTER], GF_PCStrings[PCSTR_SPARE8]);
				available[DOP_FILTER] = 0;
			}
		}

		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
	}

	if (inputDB & IN_LEFT && selection == DOP_RESOLUTION)
	{
		oldRes = selected_res;

		if (selected_res > 0)
			selected_res--;

		if (oldRes != selected_res)
		{
			if (!DXSwitchVideoMode(selected_res, oldRes, 0))
				selected_res = oldRes;

			for (int i = 0; i < nSel; i++)
			{
				T_RemovePrint(dtext[i]);
				dtext[i] = 0;
			}

			for (int i = nSel; i < DT_NUMT; i++)
				T_RemovePrint(dtext[i]);

			save = 1;
		}
	}

	if (inputDB & IN_RIGHT && selection == DOP_RESOLUTION)
	{
		oldRes = selected_res;

		if (selected_res < dinfo->nDisplayMode - 1)
			selected_res++;

		if (oldRes != selected_res)
		{
			if (!DXSwitchVideoMode(selected_res, oldRes, 0))
				selected_res = oldRes;

			for (int i = 0; i < nSel; i++)
			{
				T_RemovePrint(dtext[i]);
				dtext[i] = 0;
			}

			for (int i = nSel; i < DT_NUMT; i++)
				T_RemovePrint(dtext[i]);

			save = 1;
		}
	}

	if (inputDB & (IN_LEFT | IN_RIGHT))
	{
		switch (selection)
		{
		case DOP_GAMMA:

			if (inputDB & IN_RIGHT)
				GammaOption++;

			if (inputDB & IN_LEFT)
				GammaOption--;

			if (GammaOption > 10)
				GammaOption = 10;

			if (GammaOption < 1)
				GammaOption = 1;

			HWR_InitState();
			T_RemovePrint(dtext[DT_OP_GAMMA]);
			sprintf(gtxt, "%d", (ulong)GammaOption);
			dtext[DT_OP_GAMMA] = T_Print(w + tW, LINEY(1), 0, gtxt);
			T_CentreV(dtext[DT_OP_GAMMA], 1);
			break;

		case DOP_TRUEALPHA:

			if (HWConfig.TrueAlpha)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				HWConfig.TrueAlpha = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				HWConfig.TrueAlpha = 1;
			}

			CloseDrawPrimitive();
			InitDrawPrimitive(App.lpD3DDevice, App.lpBackBuffer, dinfo->bHardware);
			HWR_InitState();
			break;

		case DOP_DITHER:

			if (HWConfig.Dither)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				HWConfig.Dither = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				HWConfig.Dither = 1;
			}

			HWR_InitState();
			break;

		case DOP_FILTER:
			if (HWConfig.nFilter == 2)
			{
				HWConfig.nFilter = 1;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
			}
			else
			{
				HWConfig.nFilter = 2;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
			}

			HWR_InitState();
			break;

		case DOP_ZBUFFER:

			if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
			{
				if (App.lpZBuffer)
				{
					App.DXConfigPtr->bZBuffer = 0;
					DXSwitchVideoMode(selected_res, selected_res, 0);
					T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
					App.lpZBuffer = 0;
				}
				else
				{
					App.DXConfigPtr->bZBuffer = 1;

					if (DXSwitchVideoMode(selected_res, selected_res, 1))
						T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
					else
					{
						App.DXConfigPtr->bZBuffer = 0;
						App.lpZBuffer = 0;
					}
				}

				HWR_InitState();
			}

			break;

		case DOP_FOOTPRINTS:

			if (tomb3.footprints)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				tomb3.footprints = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				tomb3.footprints = 1;
			}

			break;

		case DOP_SHADOW:

			if (tomb3.shadow_mode < NSHADOW_MODES - 1)
				tomb3.shadow_mode++;
			else
				tomb3.shadow_mode = SHADOW_ORIGINAL;

			if (tomb3.shadow_mode == SHADOW_ORIGINAL)
				T_ChangeText(dtext[selection + nSel], (char*)"Original");
			else
				T_ChangeText(dtext[selection + nSel], (char*)"PSX");

			break;

		case DOP_BARS:

			if (tomb3.bar_mode < NBAR_MODES - 1)
				tomb3.bar_mode++;
			else
				tomb3.bar_mode = BAR_ORIGINAL;

			if (tomb3.bar_mode == BAR_ORIGINAL)
				T_ChangeText(dtext[selection + nSel], (char*)"Original");
			else
				T_ChangeText(dtext[selection + nSel], (char*)"PSX");

			break;

		case DOP_PICKUP:

			if (tomb3.pickup_display)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				tomb3.pickup_display = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				tomb3.pickup_display = 1;
			}

			break;
		}

		save = 1;
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
	}

	if (inputDB & IN_BACK && selection > 0)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection--;

		while (!available[selection] && selection > 0) selection--;

		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
	}

	if (inputDB & IN_FORWARD && selection <= DOP_NOPTS - 2)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection++;

		while (!available[selection] && selection <= DOP_NOPTS - 1) selection++;

		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
	}

	if (inputDB & (IN_SELECT | IN_DESELECT))
	{
		for (int i = 0; i < nSel; i++)
		{
			T_RemovePrint(dtext[i]);
			dtext[i] = 0;
		}

		for (int i = nSel; i < DT_NUMT; i++)
			T_RemovePrint(dtext[i]);
	}

	if (save)
		S_SaveSettings();	//save everything if needed
}
#else
void do_detail_option(INVENTORY_ITEM* item)
{
	DIRECT3DINFO* dinfo;
	DISPLAYMODE* dm;
	DISPLAYMODE* cdm;
	static RES_TXT resolutions[30];
	static long selected_res;
	static long selection = DOP_NOPTS - 1;
	long nSel, w, oldRes;
	static char available[DOP_NOPTS];
	char gtxt[8];

	nSel = DT_NUMT - DOP_NOPTS;
	w = GetRenderWidth() / 2 - 115;
	dinfo = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D];

	if (!dtext[DT_GAMMA])
	{
		cdm = &dinfo->DisplayMode[App.DXConfigPtr->nVMode];

		for (int i = 0; i < dinfo->nDisplayMode; i++)
		{
			dm = &dinfo->DisplayMode[i];

			if (dm->bpp == cdm->bpp)
			{
				sprintf(resolutions[i].res, "%dx%d", dm->w, dm->h);

				if (dm->w == cdm->w && dm->h == cdm->h)
					selected_res = i;
			}
		}

		dtext[DT_GAMMA] = T_Print(w, 55, 0, GF_PCStrings[PCSTR_SKY]);
		dtext[DT_TRUEALPHA] = T_Print(w, 35, 0, GF_PCStrings[PCSTR_TRUEALPHA]);
		dtext[DT_DITHER] = T_Print(w, 15, 0, GF_PCStrings[PCSTR_DITHER]);
		dtext[DT_FILTER] = T_Print(w, -5, 0, GF_PCStrings[PCSTR_FILTERING]);
		dtext[DT_ZBUFFER] = T_Print(w, -25, 0, GF_PCStrings[PCSTR_ZBUFFER]);
		dtext[DT_RESOLUTION] = T_Print(w, -45, 0, GF_PCStrings[PCSTR_RESOLUTION]);
		dtext[DT_EMPTY] = T_Print(0, -72, 0, " ");
		dtext[DT_VIDEOTITLE] = T_Print(0, -70, 0, GF_PCStrings[PCSTR_VIDEOTITLE]);
		T_AddBackground(dtext[DT_EMPTY], 240, 150, 0, 0, 48, 0, 0, 0);
		T_AddOutline(dtext[DT_EMPTY], 1, 15, 0, 0);

		T_AddBackground(dtext[DT_VIDEOTITLE], 236, 0, 0, 0, 48, 0, 0, 0);
		T_AddOutline(dtext[DT_VIDEOTITLE], 1, 4, 0, 0);

		T_CentreV(dtext[DT_GAMMA], 1);
		T_CentreV(dtext[DT_TRUEALPHA], 1);
		T_CentreV(dtext[DT_DITHER], 1);
		T_CentreV(dtext[DT_FILTER], 1);
		T_CentreV(dtext[DT_ZBUFFER], 1);
		T_CentreV(dtext[DT_RESOLUTION], 1);

		T_CentreH(dtext[DT_EMPTY], 1);
		T_CentreV(dtext[DT_EMPTY], 1);

		T_CentreH(dtext[DT_VIDEOTITLE], 1);
		T_CentreV(dtext[DT_VIDEOTITLE], 1);

		dtext[DT_OP_RESOLUTION] = T_Print(w + 130, -45, 0, resolutions[selected_res].res);

		if (HWConfig.TrueAlpha)
			dtext[DT_OP_TRUEALPHA] = T_Print(w + 130, 35, 0, GF_PCStrings[PCSTR_OFF]);
		else
			dtext[DT_OP_TRUEALPHA] = T_Print(w + 130, 35, 0, GF_PCStrings[PCSTR_ON]);

		if (HWConfig.Dither)
			dtext[DT_OP_DITHER] = T_Print(w + 130, 15, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_DITHER] = T_Print(w + 130, 15, 0, GF_PCStrings[PCSTR_OFF]);

		if (HWConfig.nFilter == 2)
			dtext[DT_OP_FILTER] = T_Print(w + 130, -5, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_FILTER] = T_Print(w + 130, -5, 0, GF_PCStrings[PCSTR_OFF]);

		if (App.lpZBuffer)
			dtext[DT_OP_ZBUFFER] = T_Print(w + 130, -25, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_ZBUFFER] = T_Print(w + 130, -25, 0, GF_PCStrings[PCSTR_OFF]);

		for (int i = 0; i < DOP_NOPTS; i++)
			available[i] = 1;

		if (dinfo->bHardware)
		{
			T_ChangeText(dtext[DT_OP_TRUEALPHA], GF_PCStrings[PCSTR_SPARE8]);
			available[DOP_TRUEALPHA] = 0;
		}
		else
		{
			T_ChangeText(dtext[DT_OP_DITHER], GF_PCStrings[PCSTR_SPARE8]);
			T_ChangeText(dtext[DOP_ZBUFFER], GF_PCStrings[PCSTR_SPARE8]);
			available[DOP_DITHER] = 0;
			available[DOP_ZBUFFER] = 0;

			if (App.DXConfigPtr->MMX)
			{
				T_ChangeText(dtext[DT_OP_TRUEALPHA], GF_PCStrings[PCSTR_SPARE8]);
				available[DOP_TRUEALPHA] = 0;
			}
			else
			{
				T_ChangeText(dtext[DT_OP_FILTER], GF_PCStrings[PCSTR_SPARE8]);
				available[DOP_FILTER] = 0;
			}
		}

		sprintf(gtxt, "%d", (ulong)GammaOption);
		dtext[DT_OP_GAMMA] = T_Print(w + 130, 55, 0, gtxt);
		T_CentreV(dtext[DT_OP_GAMMA], 1);

		T_CentreV(dtext[DT_OP_TRUEALPHA], 1);
		T_CentreV(dtext[DT_OP_DITHER], 1);
		T_CentreV(dtext[DT_OP_FILTER], 1);
		T_CentreV(dtext[DT_OP_ZBUFFER], 1);
		T_CentreV(dtext[DT_OP_RESOLUTION], 1);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
	}

	if (inputDB & IN_LEFT && selection == DOP_RESOLUTION)
	{
		oldRes = selected_res;

		if (selected_res > 0)
			selected_res--;

		if (oldRes != selected_res)
		{
			if (!DXSwitchVideoMode(selected_res, oldRes, 0))
				selected_res = oldRes;

			for (int i = 0; i < nSel; i++)
			{
				T_RemovePrint(dtext[i]);
				dtext[i] = 0;
			}

			for (int i = nSel; i < DT_NUMT; i++)
				T_RemovePrint(dtext[i]);
		}
	}

	if (inputDB & IN_RIGHT && selection == DOP_RESOLUTION)
	{
		oldRes = selected_res;

		if (selected_res < dinfo->nDisplayMode - 1)
			selected_res++;

		if (oldRes != selected_res)
		{
			if (!DXSwitchVideoMode(selected_res, oldRes, 0))
				selected_res = oldRes;

			for (int i = 0; i < nSel; i++)
			{
				T_RemovePrint(dtext[i]);
				dtext[i] = 0;
			}

			for (int i = nSel; i < DT_NUMT; i++)
				T_RemovePrint(dtext[i]);
		}
	}

	if (inputDB & (IN_LEFT | IN_RIGHT))
	{
		switch (selection)
		{
		case DOP_GAMMA:

			if (inputDB & IN_RIGHT)
				GammaOption++;

			if (inputDB & IN_LEFT)
				GammaOption--;

			if (GammaOption > 10)
				GammaOption = 10;

			if (GammaOption < 1)
				GammaOption = 1;

			HWR_InitState();
			T_RemovePrint(dtext[DT_OP_GAMMA]);
			sprintf(gtxt, "%d", (ulong)GammaOption);
			dtext[DT_OP_GAMMA] = T_Print(w + 130, 55, 0, gtxt);
			T_CentreV(dtext[DT_OP_GAMMA], 1);
			break;

		case DOP_TRUEALPHA:

			if (HWConfig.TrueAlpha)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				HWConfig.TrueAlpha = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				HWConfig.TrueAlpha = 1;
			}

			CloseDrawPrimitive();
			InitDrawPrimitive(App.lpD3DDevice, App.lpBackBuffer, dinfo->bHardware);
			HWR_InitState();
			break;

		case DOP_DITHER:

			if (HWConfig.Dither)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				HWConfig.Dither = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				HWConfig.Dither = 1;
			}

			HWR_InitState();
			break;

		case DOP_FILTER:
			if (HWConfig.nFilter == 2)
			{
				HWConfig.nFilter = 1;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
			}
			else
			{
				HWConfig.nFilter = 2;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
			}

			HWR_InitState();
			break;

		case DOP_ZBUFFER:

			if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].bHardware)
			{
				if (App.lpZBuffer)
				{
					App.DXConfigPtr->bZBuffer = 0;
					DXSwitchVideoMode(selected_res, selected_res, 0);
					T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
					App.lpZBuffer = 0;
				}
				else
				{
					App.DXConfigPtr->bZBuffer = 1;

					if (DXSwitchVideoMode(selected_res, selected_res, 1))
						T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
					else
					{
						App.DXConfigPtr->bZBuffer = 0;
						App.lpZBuffer = 0;
					}
				}

				HWR_InitState();
			}

			break;
		}

		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
	}

	if (inputDB & IN_BACK && selection > 0)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection--;

		while (!available[selection] && selection > 0) selection--;

		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
	}

	if (inputDB & IN_FORWARD && selection <= DOP_NOPTS - 2)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection++;

		while (!available[selection] && selection <= DOP_NOPTS - 1) selection++;

		T_AddOutline(dtext[selection + nSel], 1, 4, 0, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, 0, 0);
	}

	if (inputDB & (IN_SELECT | IN_DESELECT))
	{
		for (int i = 0; i < nSel; i++)
		{
			T_RemovePrint(dtext[i]);
			dtext[i] = 0;
		}

		for (int i = nSel; i < DT_NUMT; i++)
			T_RemovePrint(dtext[i]);
	}
}
#endif

void do_levelselect_option(INVENTORY_ITEM* item)
{
	static TEXTSTRING* AdventureText;
	static TEXTSTRING* LeftArrow;
	static TEXTSTRING* RightArrow;
	static long GlobeButton;
	long w;
	short ang, axes, goin, nAvailable;
	char left_arrow[2];
	char right_arrow[2];

	strcpy(left_arrow, "\x11");
	strcpy(right_arrow, "\x12");

	T_RemovePrint(AdventureText);
	AdventureText = 0;

	T_RemovePrint(LeftArrow);
	LeftArrow = 0;

	T_RemovePrint(RightArrow);
	RightArrow = 0;

	axes = 0;
	ang = GlobeLevelAngles[GlobeLevel].xrot - GlobeXRot;

	if (ang >= 128 || ang <= -128)
		GlobeXRot += ang >> 3;
	else
	{
		GlobeXRot = GlobeLevelAngles[GlobeLevel].xrot;
		axes = 1;
	}

	ang = GlobeLevelAngles[GlobeLevel].yrot - GlobeYRot;

	if (ang >= 128 || ang <= -128)
		GlobeYRot += ang >> 3;
	else
	{
		GlobeYRot = GlobeLevelAngles[GlobeLevel].yrot;
		axes++;
	}

	ang = GlobeLevelAngles[GlobeLevel].zrot - GlobeZRot;

	if (ang >= 128 || ang <= -128)
		GlobeZRot += ang >> 3;
	else
	{
		GlobeZRot = GlobeLevelAngles[GlobeLevel].zrot;
		axes++;
	}

	if (axes == 3 && GlobeLevel != 6)
	{
		AdventureText = T_Print(0, -16, 5, GF_GameStrings[GlobeLevelAngles[GlobeLevel].txt]);
		T_CentreH(AdventureText, 1);
		T_BottomAlign(AdventureText, 1);

		if (input & IN_LEFT)
		{
			do
			{
				if (GlobeLevel)
					GlobeLevel--;
				else
					GlobeLevel = 5;
			}
			while (GlobeLevelComplete[GlobeLevel]);
		}
		else if (input & IN_RIGHT)
		{
			do
			{
				if (GlobeLevel == 5)
					GlobeLevel = 0;
				else
					GlobeLevel++;
			}
			while (GlobeLevelComplete[GlobeLevel]);
		}
	}

	if (GlobeLevel == 6)
		item->drawn_meshes = 3969;

	goin = 0;

	if (axes == 3 && GlobeLevel == 6)
	{
		if (GlobeLevel == 6)
		{
			GlobeLevel = 0;
			while (GlobeLevelComplete[GlobeLevel]) GlobeLevel++;
			inputDB |= GlobeButton;
			item->drawn_meshes = 3969;
			return;
		}

		goin = 1;
	}

	if (GlobeLevel != 6 || goin)
	{
		nAvailable = 0;

		for (int i = 0; i < 6; i++)
		{
			if (!GlobeLevelComplete[i])
				nAvailable++;
		}

		if (axes == 3 && nAvailable > 1)
		{
			w = App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode[App.DXConfigPtr->nVMode].w;
			w -= w >> 1;

			LeftArrow = T_Print(w - 120, -16, 2, left_arrow);
			T_BottomAlign(LeftArrow, 1);

			RightArrow = T_Print(w + 120, -16, 2, right_arrow);
			T_BottomAlign(RightArrow, 1);
		}

		item->drawn_meshes = 0xFFF;

		if (GlobeLevelComplete[3] == 255)
			item->drawn_meshes = 0xFF7;

		if (GlobeLevelComplete[5] == 255)
			item->drawn_meshes &= ~0x40;
	}

	inputDB &= ~IN_DESELECT;

	if ((inputDB & (IN_SELECT | IN_DESELECT)) && GlobeLevel != 6)
	{
		if (inputDB & IN_SELECT)
			NextAdventure = GlobeLevel;

		GlobeLevel = 6;
		GlobeButton = inputDB & (IN_SELECT | IN_DESELECT);
		inputDB &= ~(IN_SELECT | IN_DESELECT);
	}

	if ((inputDB & (IN_SELECT | IN_DESELECT)) && GlobeLevel == 6)
		inputDB &= ~(IN_SELECT | IN_DESELECT);
}

void do_pickup_option(INVENTORY_ITEM* item)
{
	if (inputDB & (IN_SELECT | IN_DESELECT))
	{
		item->anim_direction = 1;
		item->goal_frame = item->frames_total - 1;
	}
}

void do_sound_option(INVENTORY_ITEM* item)
{
	long goin;
	char buf[20];

	if (!stext[0])
	{
		if (Option_Music_Volume > 10)
			Option_Music_Volume = 10;

		wsprintf(buf, "| %2d", Option_Music_Volume);
		stext[0] = T_Print(0, 0, 0, buf);
		T_AddBackground(stext[0], 168, 0, 0, 0, 8, 0, 0, 0);
		T_AddOutline(stext[0], 1, 4, 0, 0);

		if (Option_SFX_Volume > 10)
			Option_SFX_Volume = 10;

		wsprintf(buf, "} %2d", Option_SFX_Volume);
		stext[1] = T_Print(0, 25, 0, buf);

		stext[2] = T_Print(0, -32, 0, " ");
		T_AddBackground(stext[2], 180, 85, 0, 0, 48, 0, 0, 0);
		T_AddOutline(stext[2], 1, 15, 0, 0);

		stext[3] = T_Print(0, -30, 0, GF_PCStrings[PCSTR_SETVOLUME]);
		T_AddBackground(stext[3], 176, 0, 0, 0, 8, 0, 0, 0);
		T_AddOutline(stext[3], 1, 15, 0, 0);

		for (int i = 0; i < 4; i++)
		{
			T_CentreH(stext[i], 1);
			T_CentreV(stext[i], 1);
		}
	}

	if (inputDB & IN_FORWARD && item_data > 0)
	{
		T_RemoveOutline(stext[item_data]);
		T_RemoveBackground(stext[item_data]);
		item_data--;
		T_AddBackground(stext[item_data], 168, 0, 0, 0, 8, 0, 0, 0);
		T_AddOutline(stext[item_data], 1, 4, 0, 0);
	}

	if (inputDB & IN_BACK && item_data < 1)
	{
		T_RemoveOutline(stext[item_data]);
		T_RemoveBackground(stext[item_data]);
		item_data++;
		T_AddBackground(stext[item_data], 168, 0, 0, 0, 8, 0, 0, 0);
		T_AddOutline(stext[item_data], 1, 4, 0, 0);
	}

	goin = 0;

	if (item_data)
	{
		if (input & IN_LEFT && Option_SFX_Volume > 0)
		{
			idelay = 1;
			idcount = 10;
			Option_SFX_Volume--;
			goin = 1;
		}
		else if (input & IN_RIGHT && Option_SFX_Volume < 10)
		{
			idelay = 1;
			idcount = 10;
			Option_SFX_Volume++;
			goin = 1;
		}

		if (goin)
		{
			wsprintf(buf, "} %2d", Option_SFX_Volume);
			T_ChangeText(stext[1], buf);

			if (Option_SFX_Volume)
				S_SoundSetMasterVolume(6 * Option_SFX_Volume + 4);
			else
				S_SoundSetMasterVolume(0);

			SoundEffect(SFX_MENU_PASSPORT, 0, SFX_ALWAYS);
		}
	}
	else
	{
		if (input & IN_LEFT && Option_Music_Volume > 0)
		{
			idelay = 1;
			idcount = 10;
			Option_Music_Volume--;
			goin = 1;
		}
		else if (input & IN_RIGHT && Option_Music_Volume < 10)
		{
			idelay = 1;
			idcount = 10;
			Option_Music_Volume++;
			goin = 1;
		}

		if (goin)
		{
			wsprintf(buf, "| %2d", Option_Music_Volume);
			T_ChangeText(stext[0], buf);

			if (Option_Music_Volume)
				S_CDVolume(25 * Option_Music_Volume + 5);
			else
				S_CDVolume(0);

			SoundEffect(SFX_MENU_PASSPORT, 0, SFX_ALWAYS);
		}
	}

#ifdef TROYESTUFF
	if (goin)
		S_SaveSettings();	//save if needed
#endif

	if (inputDB & (IN_SELECT | IN_DESELECT))
	{
		T_RemovePrint(stext[0]);
		stext[0] = 0;

		T_RemovePrint(stext[1]);
		stext[1] = 0;

		T_RemovePrint(stext[2]);
		stext[2] = 0;

		T_RemovePrint(stext[3]);
		stext[3] = 0;
	}
}

#define iconfig	VAR_(0x006A0130, long)
#define btext	ARRAY_(0x006A0170, TEXTSTRING*, [14])
#define ctext	ARRAY_(0x006A0138, TEXTSTRING*, [14])

static void FlashConflicts()
{
	short c;

	for (int i = 0; i < 14; i++)
	{
		c = layout[iconfig][i];
		T_FlashText(btext[i], 0, 0);

		for (int j = 0; j < 14; j++)
		{
			if (i != j && c == layout[iconfig][j])
			{
				T_FlashText(btext[i], 1, 20);
				break;
			}
		}
	}
}

void DefaultConflict()
{
	short c;

	for (int i = 0; i < 14; i++)
	{
		c = layout[0][i];
		conflict[i] = 0;

		for (int j = 0; j < 14; j++)
		{
			if (c == layout[1][j])
			{
				conflict[i] = 1;
				break;
			}
		}
	}
}

void inject_option(bool replace)
{
	INJECT(0x0048A200, GetRenderWidth, replace);
	INJECT(0x0048A1F0, GetRenderHeight, replace);
	INJECT(0x00488260, do_detail_option, replace);
	INJECT(0x00487870, do_levelselect_option, replace);
	INJECT(0x00487720, do_pickup_option, replace);
	INJECT(0x00488F30, do_sound_option, replace);
	INJECT(0x00489490, FlashConflicts, replace);
	INJECT(0x00489510, DefaultConflict, replace);
}
