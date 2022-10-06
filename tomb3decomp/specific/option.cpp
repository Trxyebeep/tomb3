#include "../tomb3/pch.h"
#include "option.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "hwrender.h"
#include "dxshell.h"
#include "drawprimitive.h"

long GetRenderWidth()
{
	return phd_winwidth;
}

long GetRenderHeight()
{
	return phd_winheight;
}

TEXTSTRING* dtext[DT_NUMT];

void do_detail_option(INVENTORY_ITEM* item)
{
	DIRECT3DINFO* dinfo;
	DISPLAYMODE* dm;
	DISPLAYMODE* cdm;
	static RES_TXT resolutions[30];
	static long selected_res;
	static long selection = 5;
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
			T_ChangeText(dtext[DT_OP_FILTER], GF_PCStrings[PCSTR_SPARE8]);
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
		case 0:

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

		case 1:

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

		case 2:

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

		case 3:
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

		case 4:

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

void inject_option(bool replace)
{
	INJECT(0x0048A200, GetRenderWidth, replace);
	INJECT(0x0048A1F0, GetRenderHeight, replace);
	INJECT(0x00488260, do_detail_option, replace);
}
