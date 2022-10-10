#include "../tomb3/pch.h"
#include "tomb3.h"
#include "registry.h"
#include "../specific/specific.h"
#include "../specific/option.h"

TOMB3_OPTIONS tomb3;

static void T3_InitSettings()
{
	Option_Music_Volume = 7;
	Option_SFX_Volume = 10;
	S_CDVolume(25 * Option_Music_Volume + 5);
	S_SoundSetMasterVolume(6 * Option_SFX_Volume + 4);

	tomb3.footprints = 1;
	tomb3.shadow_mode = SHADOW_PSX;
	tomb3.bar_mode = BAR_PSX;
	tomb3.pickup_display = 1;
}

void T3_SaveSettings()
{
	ulong gamma;

	OpenRegistry(SUB_KEY);

	if (App.DeviceInfoPtr->nDDInfo)
		REG_WriteLong((char*)"DD", App.DXConfigPtr->nDD);

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].nD3DInfo)
	{
		REG_WriteLong((char*)"D3D", App.DXConfigPtr->nD3D);
		REG_WriteLong((char*)"D3DTF", App.DXConfigPtr->D3DTF);
	}

	if (App.DeviceInfoPtr->nDSInfo)
		REG_WriteLong((char*)"DS", App.DXConfigPtr->DS);

	if (App.DeviceInfoPtr->nDIInfo)
		REG_WriteLong((char*)"DI", App.DXConfigPtr->DI);

	REG_WriteLong((char*)"VM", App.DXConfigPtr->nVMode);
	REG_WriteLong((char*)"zbuffer", App.DXConfigPtr->bZBuffer);
	REG_WriteLong((char*)"dither", App.DXConfigPtr->Dither);
	REG_WriteLong((char*)"filter", App.DXConfigPtr->Filter);
	REG_WriteLong((char*)"AGP", App.DXConfigPtr->AGP);
	REG_WriteLong((char*)"sound", App.DXConfigPtr->sound);
	REG_WriteLong((char*)"joystick", App.DXConfigPtr->Joystick);
	REG_WriteLong((char*)"MMX", App.DXConfigPtr->MMX);
	REG_WriteLong((char*)"SFXVolume", Option_SFX_Volume);
	REG_WriteLong((char*)"MusicVolume", Option_Music_Volume);

	gamma = (ulong)GammaOption;
	REG_WriteLong((char*)"Gamma", gamma);
	REG_WriteBlock((char*)"keyLayout", &layout[1][0], sizeof(layout) / 2);

	//new settings :)
	REG_WriteBool((char*)"footprints", tomb3.footprints);
	REG_WriteLong((char*)"shadow_mode", tomb3.shadow_mode);
	REG_WriteLong((char*)"bar_mode", tomb3.bar_mode);
	REG_WriteBool((char*)"pickup_display", tomb3.pickup_display);

	CloseRegistry();
}

bool T3_LoadSettings()
{
	ulong gamma;

	if (!OpenRegistry(SUB_KEY))
	{
		T3_InitSettings();
		return 0;
	}

	if (App.DeviceInfoPtr->nDDInfo)
		REG_ReadLong((char*)"DD", (ulong&)App.DXConfigPtr->nDD, 0);

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].nD3DInfo)
	{
		REG_ReadLong((char*)"D3D", (ulong&)App.DXConfigPtr->nD3D, 0);
		REG_ReadLong((char*)"D3DTF", (ulong&)App.DXConfigPtr->D3DTF, 0);
	}

	if (App.DeviceInfoPtr->nDSInfo)
		REG_ReadLong((char*)"DS", (ulong&)App.DXConfigPtr->DS, 0);

	if (App.DeviceInfoPtr->nDIInfo)
		REG_ReadLong((char*)"DS", (ulong&)App.DXConfigPtr->DI, 0);

	REG_ReadLong((char*)"VM", (ulong&)App.DXConfigPtr->nVMode, 0);
	REG_ReadLong((char*)"zbuffer", (ulong&)App.DXConfigPtr->bZBuffer, 0);
	REG_ReadLong((char*)"dither", (ulong&)App.DXConfigPtr->Dither, 0);//
	REG_ReadLong((char*)"filter", (ulong&)App.DXConfigPtr->Filter, 0);
	REG_ReadLong((char*)"AGP", (ulong&)App.DXConfigPtr->AGP, 0);
	REG_ReadLong((char*)"sound", (ulong&)App.DXConfigPtr->sound, 0);
	REG_ReadLong((char*)"joystick", (ulong&)App.DXConfigPtr->Joystick, 0);
	REG_ReadLong((char*)"MMX", (ulong&)App.DXConfigPtr->MMX, 0);
	REG_ReadLong((char*)"SFXVolume", (ulong&)Option_SFX_Volume, 0);
	REG_ReadLong((char*)"MusicVolume", (ulong&)Option_Music_Volume, 0);
	REG_ReadLong((char*)"Gamma", gamma, 0);
	GammaOption = (float)gamma;

	REG_ReadBlock((char*)"keyLayout", &layout[1][0], sizeof(layout) / 2, 0);
	DefaultConflict();	//fix having to open the control options to set conflicts;

	REG_ReadBool((char*)"footprints", tomb3.footprints, 1);
	REG_ReadLong((char*)"shadow_mode", (ulong&)tomb3.shadow_mode, SHADOW_PSX);
	REG_ReadLong((char*)"bar_mode", (ulong&)tomb3.bar_mode, BAR_PSX);
	REG_ReadBool((char*)"pickup_display", tomb3.pickup_display, 1);

	S_CDVolume(25 * Option_Music_Volume + 5);
	S_SoundSetMasterVolume(6 * Option_SFX_Volume + 4);
	CloseRegistry();
	return 1;
}
