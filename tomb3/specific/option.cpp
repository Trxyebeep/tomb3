#include "../tomb3/pch.h"
#include "option.h"
#include "../game/text.h"
#include "../game/gameflow.h"
#include "hwrender.h"
#include "dxshell.h"
#include "drawprimitive.h"
#include "specific.h"
#include "input.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/sound.h"
#include "game.h"
#include "../game/inventry.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "../game/savegame.h"
#include "../game/control.h"
#include "output.h"
#include "smain.h"
#include "../newstuff/psxsaves.h"
#include "../newstuff/map.h"
#include "../tomb3/tomb3.h"

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
static TEXTSTRING* btext[NLAYOUTKEYS];
static TEXTSTRING* ctext[NLAYOUTKEYS];
static TEXTSTRING* ctrltext[2];
static long iconfig;
static long keychange;

long SavedGames;

long GetRenderWidth()
{
	return phd_winwidth;
}

long GetRenderHeight()
{
	return phd_winheight;
}

long GetRenderWidthDownscaled()
{
	return phd_winwidth * 0x10000 / GetRenderScale(0x10000);
}

long GetRenderHeightDownscaled()
{
	return phd_winheight * 0x10000 / GetRenderScale(0x10000);
}

void do_detail_option(INVENTORY_ITEM* item)
{
	DIRECT3DINFO* dinfo;
	DISPLAYMODE* dm;
	DISPLAYMODE* cdm;
	static RES_TXT* resolutions;
	static long selected_res;
	static long selection = DOP_NOPTS - 1;
	long nSel, w, tW, oldRes;
	static char available[DOP_NOPTS];
	char gtxt[8];
	bool save;

	save = 0;
	nSel = DT_NUMT - DOP_NOPTS;
	tW = 130;
	w = GetRenderWidthDownscaled() / 2 - 115;
#if (DIRECT3D_VERSION >= 0x900)
	dinfo = &App.lpDeviceInfo->D3DInfo[App.lpDXConfig->nD3D];
#else
	dinfo = &App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].D3DInfo[App.lpDXConfig->nD3D];
#endif

	if (!dtext[DT_GAMMA])
	{
		cdm = &dinfo->DisplayMode[App.lpDXConfig->nVMode];
		resolutions = (RES_TXT*)malloc(sizeof(RES_TXT) * dinfo->nDisplayMode);

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
		dtext[DT_EMPTY] = T_Print(0, -72, 0, " ");
		dtext[DT_VIDEOTITLE] = T_Print(0, -70, 0, GF_PCStrings[PCSTR_VIDEOTITLE]);
		dtext[DT_RESOLUTION] = T_Print(w, -45, 0, GF_PCStrings[PCSTR_RESOLUTION]);
		dtext[DT_ZBUFFER] = T_Print(w, -25, 0, GF_PCStrings[PCSTR_ZBUFFER]);
		dtext[DT_FILTER] = T_Print(w, -5, 0, GF_PCStrings[PCSTR_FILTERING]);
		dtext[DT_DITHER] = T_Print(w, 15, 0, GF_PCStrings[PCSTR_DITHER]);
		dtext[DT_GAMMA] = T_Print(w, 35, 0, GF_PCStrings[PCSTR_SKY]);
		
		T_AddBackground(dtext[DT_EMPTY], 240, 130, 0, 0, 48, 0, &req_bgnd_gour1, 0);
		T_AddOutline(dtext[DT_EMPTY], 1, 15, &req_bgnd_gour2, 0);
		T_CentreH(dtext[DT_EMPTY], 1);
		T_CentreV(dtext[DT_EMPTY], 1);

		T_AddBackground(dtext[DT_VIDEOTITLE], 236, 0, 0, 0, 48, 0, &req_main_gour1, 0);
		T_AddOutline(dtext[DT_VIDEOTITLE], 1, 4, &req_main_gour2, 0);
		T_CentreH(dtext[DT_VIDEOTITLE], 1);
		T_CentreV(dtext[DT_VIDEOTITLE], 1);

		T_CentreV(dtext[DT_RESOLUTION], 1);
		T_CentreV(dtext[DT_ZBUFFER], 1);
		T_CentreV(dtext[DT_FILTER], 1);
		T_CentreV(dtext[DT_DITHER], 1);
		T_CentreV(dtext[DT_GAMMA], 1);

		//actual options
		dtext[DT_OP_RESOLUTION] = T_Print(w + tW, -45, 0, resolutions[selected_res].res);

		if (App.lpDXConfig->bZBuffer)
			dtext[DT_OP_ZBUFFER] = T_Print(w + tW, -25, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_ZBUFFER] = T_Print(w + tW, -25, 0, GF_PCStrings[PCSTR_OFF]);

#if (DIRECT3D_VERSION >= 0x900)
		if (HWConfig.nFilter == D3DTEXF_LINEAR)
#else
		if (HWConfig.nFilter == D3DFILTER_LINEAR)
#endif
			dtext[DT_OP_FILTER] = T_Print(w + tW, -5, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_FILTER] = T_Print(w + tW, -5, 0, GF_PCStrings[PCSTR_OFF]);

		if (HWConfig.bDither)
			dtext[DT_OP_DITHER] = T_Print(w + tW, 15, 0, GF_PCStrings[PCSTR_ON]);
		else
			dtext[DT_OP_DITHER] = T_Print(w + tW, 15, 0, GF_PCStrings[PCSTR_OFF]);

		sprintf(gtxt, "%d", (ulong)GammaOption);
		dtext[DT_OP_GAMMA] = T_Print(w + tW, 35, 0, gtxt);

		T_CentreV(dtext[DT_OP_RESOLUTION], 1);
		T_CentreV(dtext[DT_OP_ZBUFFER], 1);
		T_CentreV(dtext[DT_OP_FILTER], 1);
		T_CentreV(dtext[DT_OP_DITHER], 1);
		T_CentreV(dtext[DT_OP_GAMMA], 1);

		for (int i = 0; i < DOP_NOPTS; i++)
			available[i] = 1;

		if (tomb3.disable_gamma)
		{
			T_ChangeText(dtext[DT_OP_GAMMA], GF_PCStrings[PCSTR_SPARE8]);
			available[DOP_GAMMA] = 0;
			GammaOption = 2.5F;
		}

#if (DIRECT3D_VERSION >= 0x900)
		T_ChangeText(dtext[DT_OP_DITHER], GF_PCStrings[PCSTR_SPARE8]);
		available[DOP_DITHER] = 0;
		HWConfig.bDither = 0;
#endif

		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		free(resolutions);
	}

	if (selection == DOP_RESOLUTION)
	{
		oldRes = selected_res;

		if (inputDB & IN_LEFT)
		{
			if (selected_res > 0)
				selected_res--;
		}
		else if (inputDB & IN_RIGHT)
		{
			if (selected_res < dinfo->nDisplayMode - 1)
				selected_res++;
		}

		if (oldRes != selected_res)
		{
#if (DIRECT3D_VERSION >= 0x900)
			if (!DXSwitchVideoMode(selected_res, oldRes))
#else
			if (!DXSwitchVideoMode(selected_res, oldRes, 0))
#endif
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
			dtext[DT_OP_GAMMA] = T_Print(w + tW, 35, 0, gtxt);
			T_CentreV(dtext[DT_OP_GAMMA], 1);
			break;

#if (DIRECT3D_VERSION < 0x900)
		case DOP_DITHER:

			if (HWConfig.bDither)
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				HWConfig.bDither = 0;
			}
			else
			{
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				HWConfig.bDither = 1;
			}

			HWR_InitState();
			break;
#endif

		case DOP_FILTER:
#if (DIRECT3D_VERSION >= 0x900)
			if (HWConfig.nFilter == D3DTEXF_LINEAR)
			{
				HWConfig.nFilter = D3DTEXF_POINT;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
			}
			else
			{
				HWConfig.nFilter = D3DTEXF_LINEAR;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
			}
#else
			if (HWConfig.nFilter == D3DFILTER_LINEAR)
			{
				HWConfig.nFilter = D3DFILTER_NEAREST;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
			}
			else
			{
				HWConfig.nFilter = D3DFILTER_LINEAR;
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
			}
#endif

			HWR_InitState();
			break;

#if (DIRECT3D_VERSION >= 0x900)
		case DOP_ZBUFFER:
			DXToggleZbuffer();

			if (App.lpDXConfig->bZBuffer)
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
			else
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);

			break;
#else
		case DOP_ZBUFFER:

			if (App.ZBuffer)
			{
				App.lpDXConfig->bZBuffer = 0;
				DXSwitchVideoMode(selected_res, selected_res, 0);
				T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_OFF]);
				App.ZBuffer = 0;
			}
			else
			{
				App.lpDXConfig->bZBuffer = 1;

				if (DXSwitchVideoMode(selected_res, selected_res, 1))
					T_ChangeText(dtext[selection + nSel], GF_PCStrings[PCSTR_ON]);
				else
				{
					App.lpDXConfig->bZBuffer = 0;
					App.ZBuffer = 0;
				}
			}

			HWR_InitState();
			break;
#endif
		}

		save = 1;
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
	}

	if (inputDB & IN_BACK && selection > 0)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection--;

		while (!available[selection])
		{
			selection--;

			if (selection <= 0)
			{
				while (!available[selection] && selection <= DOP_NOPTS - 1) selection++;
				break;
			}
		}

		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
	}

	if (inputDB & IN_FORWARD && selection <= DOP_NOPTS - 2)
	{
		T_RemoveOutline(dtext[selection + nSel]);
		T_RemoveBackground(dtext[selection + nSel]);
		selection++;

		while (!available[selection] && selection <= DOP_NOPTS - 1) selection++;

		T_AddOutline(dtext[selection + nSel], 1, 4, &req_sel_gour2, 0);
		T_AddBackground(dtext[selection + nSel], (short)T_GetTextWidth(dtext[selection + nSel]), 0, 0, 0, 48, 0, &req_sel_gour1, 1);
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
		S_SaveSettings();
}

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
			w = GetRenderWidthDownscaled();
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
		T_AddBackground(stext[0], 168, 0, 0, 0, 8, 0, &req_sel_gour1, 1);
		T_AddOutline(stext[0], 1, 4, &req_sel_gour2, 0);

		if (Option_SFX_Volume > 10)
			Option_SFX_Volume = 10;

		wsprintf(buf, "} %2d", Option_SFX_Volume);
		stext[1] = T_Print(0, 25, 0, buf);

		stext[2] = T_Print(0, -32, 0, " ");
		T_AddBackground(stext[2], 180, 85, 0, 0, 48, 0, &req_sel_gour1, 1);
		T_AddOutline(stext[2], 1, 15, &req_sel_gour2, 0);

		stext[3] = T_Print(0, -30, 0, GF_PCStrings[PCSTR_SETVOLUME]);
		T_AddBackground(stext[3], 176, 0, 0, 0, 8, 0, &req_main_gour1, 0);
		T_AddOutline(stext[3], 1, 15, &req_main_gour2, 0);

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
		T_AddBackground(stext[item_data], 168, 0, 0, 0, 8, 0, &req_sel_gour1, 1);
		T_AddOutline(stext[item_data], 1, 4, &req_sel_gour2, 0);
	}

	if (inputDB & IN_BACK && item_data < 1)
	{
		T_RemoveOutline(stext[item_data]);
		T_RemoveBackground(stext[item_data]);
		item_data++;
		T_AddBackground(stext[item_data], 168, 0, 0, 0, 8, 0, &req_sel_gour1, 1);
		T_AddOutline(stext[item_data], 1, 4, &req_sel_gour2, 0);
	}

	goin = 0;

	if (item_data)
	{
		if (input & IN_LEFT && Option_SFX_Volume > 0)
		{
			idelay = 1;
			idcount = 5 * TICKS_PER_FRAME;
			Option_SFX_Volume--;
			goin = 1;
		}
		else if (input & IN_RIGHT && Option_SFX_Volume < 10)
		{
			idelay = 1;
			idcount = 5 * TICKS_PER_FRAME;
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
			{
				S_CDMute();
				S_CDVolume(0);
			}

			SoundEffect(SFX_MENU_PASSPORT, 0, SFX_ALWAYS);
		}
	}

	if (goin)
		S_SaveSettings();

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

static void FlashConflicts()
{
	short c;

	for (int i = 0; i < NLAYOUTKEYS; i++)
	{
		c = layout[iconfig][i];
		T_FlashText(btext[i], 0, 0);

		for (int j = 0; j < NLAYOUTKEYS; j++)
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

	for (int i = 0; i < NLAYOUTKEYS; i++)
	{
		c = layout[0][i];
		conflict[i] = 0;

		for (int j = 0; j < NLAYOUTKEYS; j++)
		{
			if (c == layout[1][j])
			{
				conflict[i] = 1;
				break;
			}
		}
	}
}

#define CONTROL_NLINES		8
#define CONTROL_Y_BOX		-70
#define CONTROL_Y_TITLE		(CONTROL_Y_BOX + 4)
#define CONTROL_STARTY		(CONTROL_Y_BOX + 30)
#define CONTROL_SPACE		16
#define CONTROL_WIDTH_HIGH	420
#define CONTROL_HEIGHT_HIGH	(CONTROL_SPACE * CONTROL_NLINES + 45)
#define CONTROL_WIDTH_LOW	308
#define CONTROL_HEIGHT_LOW	(CONTROL_SPACE * CONTROL_NLINES + 35)

static void S_ShowControls()
{
	long mid, n, x, y, s;

	mid = GetRenderWidthDownscaled() / 2;
	s = CONTROL_SPACE;

	if (!btext[0])
	{
		n = 0;
		x = mid < 320 ? mid - 150 : mid - 200;	//left column key binds
		y = CONTROL_STARTY;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][0]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][1]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][2]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][3]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][4]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][5]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][6]]);
		y += s;

		btext[14] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][14]]);
		y += s;

		x = mid < 320 ? mid - 20 : mid + 10;	//right column key binds
		y = CONTROL_STARTY;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][7]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][8]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][9]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][10]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][11]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][12]]);
		y += s;

		btext[n++] = T_Print(x, y, 0, KeyboardButtons[layout[iconfig][13]]);
		y += s;

		for (int i = 0; i < NLAYOUTKEYS ; i++)
			T_CentreV(btext[i], 1);

		keychange = 0;
	}

	if (!ctext[0])
	{
		n = 0;
		x = mid < 320 ? mid - 78 : mid - 120;	//left column key names
		y = CONTROL_STARTY;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_RUN]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_BACK]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_LEFT]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_RIGHT]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_STEPLEFT1]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_STEPRIGHT1]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_WALK]);
		y += s;

		ctext[14] = T_Print(x, y, 0, "Pause");
		y += s;

		x = mid < 320 ? mid + 55 : mid + 90;	//right column key names
		y = CONTROL_STARTY;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_JUMP]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_ACTION]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_DRAWWEAPON1]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_USEFLARE]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_LOOK]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_ROLL]);
		y += s;

		ctext[n++] = T_Print(x, y, 0, GF_GameStrings[GT_INVENTORY]);
		y += s;

		for (int i = 0; i < NLAYOUTKEYS; i++)
			T_CentreV(ctext[i], 1);
	}

	ctrltext[1] = T_Print(0, CONTROL_Y_BOX, 0, " ");
	T_CentreV(ctrltext[1], 1);
	T_CentreH(ctrltext[1], 1);
	T_AddOutline(ctrltext[1], 1, 15, &req_bgnd_gour2, 0);

	if (mid < 320)
		T_AddBackground(ctrltext[1], CONTROL_WIDTH_LOW, CONTROL_HEIGHT_LOW, 0, 0, 48, 0, &req_bgnd_gour1, 0);
	else
		T_AddBackground(ctrltext[1], CONTROL_WIDTH_HIGH, CONTROL_HEIGHT_HIGH, 0, 0, 48, 0, &req_bgnd_gour1, 0);
}

static void S_ChangeCtrlText()
{
	char buf[40];

	if (iconfig)
	{
		wsprintf(buf, GF_PCStrings[PCSTR_USERKEYS], iconfig);
		T_ChangeText(ctrltext[0], buf);
	}
	else
		T_ChangeText(ctrltext[0], GF_PCStrings[PCSTR_DEFAULTKEYS]);

	for (int i = 0; i < NLAYOUTKEYS; i++)
	{
		if (KeyboardButtons[layout[iconfig][i]])
			T_ChangeText(btext[i], (char*)KeyboardButtons[layout[iconfig][i]]);
		else
			T_ChangeText(btext[i], (char*)"BAD");
	}
}

static void S_RemoveCtrlText()
{
	for (int i = 0; i < NLAYOUTKEYS; i++)
	{
		if (ctext[i])
			T_RemovePrint(ctext[i]);

		ctext[i] = 0;

		if (btext[i])
			T_RemovePrint(btext[i]);

		btext[i] = 0;
	}
}

void do_control_option(INVENTORY_ITEM* item)
{
	static long sel;
	short c;

	if (!ctrltext[0])
	{
		if (iconfig)
			ctrltext[0] = T_Print(0, CONTROL_Y_TITLE, 0, GF_PCStrings[PCSTR_USERKEYS]);
		else
			ctrltext[0] = T_Print(0, CONTROL_Y_TITLE, 0, GF_PCStrings[PCSTR_DEFAULTKEYS]);

		ctrltext[0]->zpos = 16;
		T_CentreH(ctrltext[0], 1);
		T_CentreV(ctrltext[0], 1);
		S_ShowControls();
		keychange = -1;
		T_AddBackground(ctrltext[0], 0, 0, 0, 0, 48, 0, &req_sel_gour1, 1);
		T_AddOutline(ctrltext[0], 1, 15, &req_sel_gour2, 0);
	}

	switch (sel)
	{
	case 0:

		if (inputDB & (IN_RIGHT | IN_LEFT))
		{
			if (keychange == -1)
			{
				iconfig = !iconfig;
				S_ChangeCtrlText();
				FlashConflicts();
			}
			else
			{
				ctext[keychange]->zpos = 16;
				T_RemoveBackground(ctext[keychange]);
				T_RemoveOutline(ctext[keychange]);

				if (keychange <= 6)
					keychange += 7;
				else if (keychange == NLAYOUTKEYS)
					keychange = 7;
				else
					keychange -= 7;

				ctext[keychange]->zpos = 16;
				T_AddBackground(ctext[keychange], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
				T_AddOutline(ctext[keychange], 1, 15, &req_sel_gour2, 0);
			}
		}
		else if (inputDB & IN_DESELECT || (inputDB & IN_SELECT && keychange == -1))
		{
			T_RemovePrint(ctrltext[0]);
			ctrltext[0] = 0;

			T_RemovePrint(ctrltext[1]);
			ctrltext[1] = 0;

			S_RemoveCtrlText();
			DefaultConflict();
			return;
		}

		if (iconfig)
		{
			if (inputDB & IN_SELECT)
			{
				sel = 1;

				ctext[keychange]->zpos = 16;
				T_RemoveBackground(ctext[keychange]);
				T_RemoveOutline(ctext[keychange]);

				btext[keychange]->zpos = 16;
				T_AddBackground(btext[keychange], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
				T_AddOutline(btext[keychange], 1, 15, &req_sel_gour2, 0);
			}
			else if (inputDB & IN_FORWARD)
			{
				if (keychange == -1)
				{
					T_RemoveBackground(ctrltext[0]);
					T_RemoveOutline(ctrltext[0]);
				}
				else
				{
					ctext[keychange]->zpos = 16;
					T_RemoveBackground(ctext[keychange]);
					T_RemoveOutline(ctext[keychange]);
				}

				keychange--;

				if (keychange < -1)
					keychange = NLAYOUTKEYS - 1;

				if (keychange == -1)
				{
					T_AddBackground(ctrltext[0], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
					T_AddOutline(ctrltext[0], 1, 15, &req_sel_gour2, 0);
				}
				else
				{
					ctext[keychange]->zpos = 16;
					T_AddBackground(ctext[keychange], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
					T_AddOutline(ctext[keychange], 1, 15, &req_sel_gour2, 0);
				}
			}
			else if (inputDB & IN_BACK)
			{
				if (keychange == -1)
				{
					T_RemoveBackground(ctrltext[0]);
					T_RemoveOutline(ctrltext[0]);
				}
				else
				{
					ctext[keychange]->zpos = 16;
					T_RemoveBackground(ctext[keychange]);
					T_RemoveOutline(ctext[keychange]);
				}

				keychange++;

				if (keychange > NLAYOUTKEYS - 1)
					keychange = -1;

				if (keychange == -1)
				{
					T_AddBackground(ctrltext[0], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
					T_AddOutline(ctrltext[0], 1, 15, &req_sel_gour2, 0);
				}
				else
				{
					ctext[keychange]->zpos = 16;
					T_AddBackground(ctext[keychange], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
					T_AddOutline(ctext[keychange], 1, 15, &req_sel_gour2, 0);
				}
			}
		}

		break;

	case 1:

		if (!(inputDB & IN_SELECT))
			sel = 2;

		break;

	case 2:

		for (c = 0; c < 256; c++)
		{
			if (key_pressed(c))
				break;
		}

		if (c == 256)
			c = 0;

		if (!c || !KeyboardButtons[c] || c == DIK_RETURN || c == DIK_LEFT || c == DIK_RIGHT || c == DIK_UP || c == DIK_DOWN)
			break;

		if (c != DIK_ESCAPE)
		{
			layout[iconfig][keychange] = c;
			T_ChangeText(btext[keychange], (char*)KeyboardButtons[c]);
		}

		btext[keychange]->zpos = 16;
		T_RemoveBackground(btext[keychange]);
		T_RemoveOutline(btext[keychange]);

		ctext[keychange]->zpos = 16;
		T_AddBackground(ctext[keychange], 0, 0, 0, 0, 0, 0, &req_sel_gour1, 1);
		T_AddOutline(ctext[keychange], 1, 15, &req_sel_gour2, 0);

		sel = 3;
		FlashConflicts();
		break;

	case 3:

		if (!key_pressed(layout[iconfig][keychange]))
		{
			sel = 0;

			if (layout[iconfig][keychange] == DIK_LCONTROL)
				layout[iconfig][keychange] = DIK_RCONTROL;

			if (layout[iconfig][keychange] == DIK_LSHIFT)
				layout[iconfig][keychange] = DIK_RSHIFT;

			if (layout[iconfig][keychange] == DIK_LMENU)
				layout[iconfig][keychange] = DIK_RMENU;

			FlashConflicts();
		}

		if (!sel)
			S_SaveSettings();

		break;
	}

	input = 0;
	inputDB = 0;
}

void do_compass_option(INVENTORY_ITEM* item)
{
	long s;
	char buf[32];

	s = savegame.timer / 30;

	sprintf(buf, "%02d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);

	if (CurrentLevel != LV_GYM)
		ShowStatsText(buf, 1);
	else
		ShowGymStatsText(buf, 1);

	if (inputDB & (IN_SELECT | IN_DESELECT))
	{
		if (inputDB & IN_SELECT && gameflow.map_enabled)
		{
			do_map_option();
			input = 0;
			inputDB = 0;
		}
		else
		{
			item->anim_direction = 1;
			item->goal_frame = item->frames_total - 1;
		}
	}

	SoundEffect(SFX_MENU_STOPWATCH, 0, SFX_ALWAYS);
}

#define PASSPORT_LINE_COUNT	10
#define PASSPORT_Y_BOX		-32
#define PASSPORT_Y_TITLE	-16

void SetPassportRequesterSize(REQUEST_INFO* req)
{
	float scale;
	long nLines, adjust;

	scale = (float)GetRenderHeight() / (float)GetRenderScale(480);
	adjust = (scale > 1.0F) ? 5 : 0;
	nLines = long((PASSPORT_LINE_COUNT + adjust) * scale - adjust);

	if (nLines < 5)
		nLines = 5;
	else if (nLines > 16)
		nLines = 16;

	SetPCRequesterSize(req, nLines, PASSPORT_Y_BOX);
}

void do_passport_option(INVENTORY_ITEM* item)
{
	static long mode;
	long select;
	short page;

	T_RemovePrint(Inv_itemText[0]);
	Inv_itemText[0] = 0;

	if ((item->goal_frame - item->open_frame) % 5)
		page = -1;
	else
		page = (item->goal_frame - item->open_frame) / 5;

	if (Inventory_Mode == INV_LOAD_MODE || Inventory_Mode == INV_SAVE_MODE || gameflow.loadsave_disabled)
		inputDB &= ~(IN_LEFT | IN_RIGHT);

	if (!page)
	{
		if (CurrentLevel == LV_GYM && Inventory_Mode != INV_TITLE_MODE || gameflow.loadsave_disabled)
			inputDB = IN_RIGHT;
		else if (mode == 1)
		{
			SetPassportRequesterSize(&Load_Game_Requester);
			select = Display_Requester(&Load_Game_Requester, 1, 1);

			if (select)
			{
				if (select > 1)
					Inventory_ExtraData[1] = select - 1;

				mode = 0;
			}
			else if (inputDB & IN_RIGHT)
			{
				Remove_Requester(&Load_Game_Requester);
				mode = 0;
			}
			else
			{
				input = 0;
				inputDB = 0;
			}
		}
		else if (!mode)
		{
			if (!SavedGames || Inventory_Mode == INV_SAVE_MODE)
				inputDB = IN_RIGHT;
			else
			{
				if (!passport_text1)
				{
					passport_text1 = T_Print(0, PASSPORT_Y_TITLE, 5, GF_GameStrings[GT_LOADGAME]);
					T_BottomAlign(passport_text1, 1);
					T_CentreH(passport_text1, 1);
				}

				T_RemovePrint(Inv_itemText[2]);
				Inv_itemText[2] = 0;

				T_RemovePrint(Inv_tagText);
				Inv_tagText = 0;

				T_RemovePrint(Inv_ringText);
				Inv_ringText = 0;

				T_RemovePrint(Inv_itemText[0]);
				Inv_itemText[0] = 0;

				GetSavedGamesList(&Load_Game_Requester);
				SetRequesterHeading(&Load_Game_Requester, GF_GameStrings[GT_LOADGAME], 0, 0, 0);
				mode = 1;
				input = 0;
				inputDB = 0;
			}
		}
	}
	else if (page == 1)
	{
		if (tomb3.psx_saving && Inventory_Mode != INV_TITLE_MODE)
		{
			if (item->anim_direction == -1 && SavedGames)
				inputDB = IN_LEFT;
			else
				inputDB = IN_RIGHT;
		}
		else if (CurrentLevel == LV_GYM && Inventory_Mode != INV_TITLE_MODE || gameflow.loadsave_disabled)
			inputDB = IN_RIGHT;
		else if (mode == 1 || mode == 2)
		{
			if (mode == 1)
			{
				SetPassportRequesterSize(&Load_Game_Requester);
				select = Display_Requester(&Load_Game_Requester, 1, 1);
			}
			else
			{
				SetPassportRequesterSize(&Level_Select_Requester);
				select = Display_Requester(&Level_Select_Requester, 1, 1);
			}

			if (select)
			{
				if (select > 0)
					Inventory_ExtraData[1] = select - 1;

				mode = 0;
			}
			else if (inputDB & (IN_LEFT | IN_RIGHT))
			{
				if (mode == 1)
					Remove_Requester(&Load_Game_Requester);
				else
					Remove_Requester(&Level_Select_Requester);

				mode = 0;
			}
			else
			{
				input = 0;
				inputDB = 0;
			}
		}
		else if (!mode)
		{
			if (Inventory_Mode == INV_DEATH_MODE)
				inputDB = item->anim_direction != -1 ? IN_RIGHT : IN_RIGHT;
			else
			{
				if (!passport_text1)
				{
					if (Inventory_Mode == INV_TITLE_MODE || CurrentLevel == LV_GYM)
						passport_text1 = T_Print(0, PASSPORT_Y_TITLE, 5, GF_GameStrings[GT_STARTGAME]);
					else
						passport_text1 = T_Print(0, PASSPORT_Y_TITLE, 5, GF_GameStrings[GT_SAVEGAME]);

					T_BottomAlign(passport_text1, 1);
					T_CentreH(passport_text1, 1);
				}

				if (Inventory_Mode != INV_TITLE_MODE && CurrentLevel != LV_GYM)
				{
					T_RemovePrint(Inv_itemText[2]);
					Inv_itemText[2] = 0;

					T_RemovePrint(Inv_tagText);
					Inv_tagText = 0;

					T_RemovePrint(Inv_ringText);
					Inv_ringText = 0;

					T_RemovePrint(Inv_itemText[0]);
					Inv_itemText[0] = 0;

					GetSavedGamesList(&Load_Game_Requester);
					SetRequesterHeading(&Load_Game_Requester, GF_GameStrings[GT_SAVEGAME], 0, 0, 0);
					mode = 1;
					input = 0;
					inputDB = 0;
				}
				else if (gameflow.play_any_level)
				{
					T_RemovePrint(Inv_itemText[2]);
					Inv_itemText[2] = 0;

					T_RemovePrint(Inv_tagText);
					Inv_tagText = 0;

					T_RemovePrint(Inv_itemText[0]);
					Inv_itemText[0] = 0;

					Init_Requester(&Level_Select_Requester);
					GetValidLevelsList(&Level_Select_Requester);
					SetRequesterHeading(&Level_Select_Requester, GF_GameStrings[GT_SELECTLEVEL], 0, 0, 0);
					mode = 2;
					input = 0;
					inputDB = 0;
				}
				else if (inputDB & IN_SELECT)
					Inventory_ExtraData[1] = LV_FIRSTLEVEL;
			}
		}
	}
	else if (page == 2)
	{
		if (!passport_text1)
		{
			if (Inventory_Mode == INV_TITLE_MODE)
				passport_text1 = T_Print(0, PASSPORT_Y_TITLE, 5, GF_GameStrings[GT_EXITGAME]);
			else if (gameflow.demoversion)
				passport_text1 = T_Print(0, PASSPORT_Y_TITLE, 5, GF_GameStrings[GT_EXITDEMO]);
			else
				passport_text1 = T_Print(0, PASSPORT_Y_TITLE, 5, GF_GameStrings[GT_EXIT2TITLE]);

			T_BottomAlign(passport_text1, 1);
			T_CentreH(passport_text1, 1);
		}
	}

	if (inputDB & IN_LEFT)
	{
		if (Inventory_Mode != INV_DEATH_MODE || SavedGames)
		{
			item->goal_frame -= 5;
			item->anim_direction = -1;

			if (SavedGames)
			{
				if (item->goal_frame < item->open_frame)
					item->goal_frame = item->open_frame;
				else
				{
					SoundEffect(SFX_MENU_PASSPORT, 0, SFX_ALWAYS);
					T_RemovePrint(passport_text1);
					passport_text1 = 0;
				}
			}
			else
			{
				if (item->goal_frame < item->open_frame + 5)
					item->goal_frame = item->open_frame + 5;
				else
				{
					T_RemovePrint(passport_text1);
					passport_text1 = 0;
				}
			}

			input = 0;
			inputDB = 0;
		}
	}

	if (inputDB & IN_RIGHT)
	{
		item->goal_frame += 5;
		item->anim_direction = 1;

		if (item->goal_frame <= item->frames_total - 6)
		{
			SoundEffect(SFX_MENU_PASSPORT, 0, SFX_ALWAYS);
			T_RemovePrint(passport_text1);
			passport_text1 = 0;
		}
		else
			item->goal_frame = item->frames_total - 6;

		input = 0;
		inputDB = 0;
	}

	if (inputDB & IN_DESELECT)
	{
		if (Inventory_Mode == INV_DEATH_MODE)
		{
			input = 0;
			inputDB = 0;
		}
		else
		{
			if (page == 2)
			{
				item->goal_frame = item->frames_total - 1;
				item->anim_direction = 1;
			}
			else
			{
				item->goal_frame = 0;
				item->anim_direction = -1;
			}

			T_RemovePrint(passport_text1);
			passport_text1 = 0;
		}
	}

	if (inputDB & IN_SELECT)
	{
		Inventory_ExtraData[0] = page;

		if (page == 2)
		{
			item->anim_direction = 1;
			item->goal_frame = item->frames_total - 1;
		}
		else
		{
			item->goal_frame = 0;
			item->anim_direction = -1;
		}

		T_RemovePrint(passport_text1);
		passport_text1 = 0;
	}
}

void do_inventory_options(INVENTORY_ITEM* item)
{
	switch (item->object_number)
	{
	case PASSPORT_OPTION:
		return do_passport_option(item);

	case MAP_OPTION:
		return do_compass_option(item);

	case DETAIL_OPTION:
		return do_detail_option(item);

	case SOUND_OPTION:
		return do_sound_option(item);

	case CONTROL_OPTION:
		return do_control_option(item);

	case GAMMA_OPTION:
		return do_levelselect_option(item);

	case GUN_OPTION:
	case SHOTGUN_OPTION:
	case MAGNUM_OPTION:
	case UZI_OPTION:
	case HARPOON_OPTION:
	case M16_OPTION:
	case ROCKET_OPTION:
	case GRENADE_OPTION:
	case MEDI_OPTION:
	case BIGMEDI_OPTION:
	case PUZZLE_OPTION1:
	case PUZZLE_OPTION2:
	case PUZZLE_OPTION3:
	case PUZZLE_OPTION4:
	case KEY_OPTION1:
	case KEY_OPTION2:
	case KEY_OPTION3:
	case KEY_OPTION4:
		inputDB |= IN_SELECT;

	case GUN_AMMO_OPTION:
	case SG_AMMO_OPTION:
	case MAG_AMMO_OPTION:
	case UZI_AMMO_OPTION:
	case HARPOON_AMMO_OPTION:
	case M16_AMMO_OPTION:
	case ROCKET_AMMO_OPTION:
		return;

	case PICKUP_OPTION1:
	case PICKUP_OPTION2:
		return do_pickup_option(item);

	case SAVEGAME_CRYSTAL_OPTION:

		if (tomb3.psx_saving)
			return do_crystal_option(item);

	default:

		if (inputDB & (IN_SELECT | IN_DESELECT))
		{
			item->goal_frame = 0;
			item->anim_direction = -1;
		}

		break;
	}
}
