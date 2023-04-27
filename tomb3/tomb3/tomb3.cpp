#include "../tomb3/pch.h"
#include "tomb3.h"
#include "../newstuff/registry.h"
#include "../specific/specific.h"
#include "../specific/option.h"
#include "../specific/input.h"
#include "../specific/winmain.h"
#include "../specific/hwrender.h"
#include "../game/inventry.h"

TOMB3_OPTIONS tomb3;
TOMB3_SAVE tomb3_save;
ulong tomb3_save_size;

ulong water_color[24] =
{
	//home
	0xFFCCFF80,

	//India
	0xFFCCFF80,
	0xFFCCFF80,
	0xFFCCFF80,
	0xFFCCFF80,

	//South Pacific
	0xFF80FFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFF80E0FF,		//puna has no water

	//London
	0xFFFFFFFF,
	0xFFCCFF80,
	0xFFCCFF80,
	0xFFCCFF80,

	//Nevada
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,

	//Antarctica
	0xFF80FFFF,
	0xFFCCFFCC,
	0xFF80E0FF,
	0xFF80E0FF,		//cavern has no water

	//Hallows
	0xFFB2E6E6,

	//
	0xFF80E0FF,
	0xFF80E0FF,
	0xFF80E0FF
};

static void T3_InitSettings()
{
	Option_Music_Volume = 7;
	Option_SFX_Volume = 10;
	App.Windowed = 0;

	tomb3.footprints = 1;
	tomb3.pickup_display = 1;
	tomb3.improved_rain = 1;
	tomb3.improved_lasers = 1;
	tomb3.uwdust = 1;
	tomb3.flexible_crawl = 1;
	tomb3.duck_roll = 1;
	tomb3.flexible_sprint = 1;
	tomb3.slide_to_run = 1;
	tomb3.kayak_mist = 1;
	tomb3.dozy = 0;
	tomb3.disable_gamma = 1;
	tomb3.disable_ckey = 0;
	tomb3.crawl_tilt = 1;
	tomb3.improved_poison_bar = 1;
	tomb3.custom_water_color = 1;
	tomb3.psx_text_colors = 0;
	tomb3.upv_wake = 1;
	tomb3.psx_fov = 0;
	tomb3.psx_boxes = 0;
	tomb3.psx_mono = 0;
	tomb3.psx_saving = 0;
	tomb3.psx_crystal_sfx = 0;
	tomb3.blue_crystal_light = 0;
	tomb3.improved_electricity = 1;
	tomb3.psx_contrast = 0;
	tomb3.shadow_mode = SHADOW_PSX;
	tomb3.bar_mode = BAR_PSX;
	tomb3.sophia_rings = SRINGS_PSX;
	tomb3.bar_pos = BPOS_ORIGINAL;
	tomb3.ammo_counter = ACTR_PC;
	tomb3.GUI_Scale = 1.0F;
	tomb3.INV_Scale = 0.5F;
	tomb3.unwater_music_mute = 0.8F;
	tomb3.inv_music_mute = 0.8F;
}

void T3_SaveSettings()
{
	OpenRegistry(SUB_KEY);

#if (DIRECT3D_VERSION < 0x900)
	if (App.lpDeviceInfo->nDDInfo)
		REG_WriteLong((char*)"DD", App.lpDXConfig->nDD);
#endif

#if (DIRECT3D_VERSION >= 0x900)
	if (App.lpDeviceInfo->nD3DInfo)
#else
	if (App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].nD3DInfo)
#endif
	{
		REG_WriteLong((char*)"D3D", App.lpDXConfig->nD3D);
#if (DIRECT3D_VERSION < 0x900)
		REG_WriteLong((char*)"D3DTF", App.lpDXConfig->D3DTF);
#endif
	}

	if (App.lpDeviceInfo->nDSInfo)
		REG_WriteLong((char*)"DS", App.lpDXConfig->nDS);

	REG_WriteLong((char*)"VM", App.lpDXConfig->nVMode);
	REG_WriteLong((char*)"zbuffer", App.lpDXConfig->bZBuffer);
	REG_WriteLong((char*)"dither", App.lpDXConfig->Dither);
	REG_WriteLong((char*)"filter", App.lpDXConfig->Filter);
	REG_WriteLong((char*)"sound", App.lpDXConfig->sound);
	REG_WriteLong((char*)"SFXVolume", Option_SFX_Volume);
	REG_WriteLong((char*)"MusicVolume", Option_Music_Volume);
	REG_WriteBool((char*)"Window", App.Windowed);
	REG_WriteLong((char*)"WindowX", App.rScreen.left);
	REG_WriteLong((char*)"WindowY", App.rScreen.top);
	REG_WriteFloat((char*)"Gamma", GammaOption);
	REG_WriteBlock((char*)"keyLayout", &layout[1][0], sizeof(layout) / 2);

	//new settings :)
	REG_WriteBool((char*)"footprints", tomb3.footprints);
	REG_WriteBool((char*)"pickup_display", tomb3.pickup_display);
	REG_WriteBool((char*)"improved_rain", tomb3.improved_rain);
	REG_WriteBool((char*)"improved_lasers", tomb3.improved_lasers);
	REG_WriteBool((char*)"uwdust", tomb3.uwdust);
	REG_WriteBool((char*)"flexible_crawl", tomb3.flexible_crawl);
	REG_WriteBool((char*)"duck_roll", tomb3.duck_roll);
	REG_WriteBool((char*)"flexible_sprint", tomb3.flexible_sprint);
	REG_WriteBool((char*)"slide_to_run", tomb3.slide_to_run);
	REG_WriteBool((char*)"kayak_mist", tomb3.kayak_mist);
	REG_WriteBool((char*)"dozy", tomb3.dozy);
	REG_WriteBool((char*)"disable_gamma", tomb3.disable_gamma);
	REG_WriteBool((char*)"disable_ckey", tomb3.disable_ckey);
	REG_WriteBool((char*)"crawl_tilt", tomb3.crawl_tilt);
	REG_WriteBool((char*)"improved_poison_bar", tomb3.improved_poison_bar);
	REG_WriteBool((char*)"custom_water_color", tomb3.custom_water_color);
	REG_WriteBool((char*)"psx_text_colors", tomb3.psx_text_colors);
	REG_WriteBool((char*)"upv_wake", tomb3.upv_wake);
	REG_WriteBool((char*)"psx_fov", tomb3.psx_fov);
	REG_WriteBool((char*)"psx_boxes", tomb3.psx_boxes);
	REG_WriteBool((char*)"psx_mono", tomb3.psx_mono);

	if (!tomb3.gold)
		REG_WriteBool((char*)"psx_saving", tomb3.psx_saving);

	REG_WriteBool((char*)"psx_crystal_sfx", tomb3.psx_crystal_sfx);
	REG_WriteBool((char*)"blue_crystal_light", tomb3.blue_crystal_light);
	REG_WriteBool((char*)"improved_electricity", tomb3.improved_electricity);
	REG_WriteBool((char*)"psx_contrast", tomb3.psx_contrast);
	REG_WriteLong((char*)"shadow_mode", tomb3.shadow_mode);
	REG_WriteLong((char*)"bar_mode", tomb3.bar_mode);
	REG_WriteLong((char*)"sophia_rings", tomb3.sophia_rings);
	REG_WriteLong((char*)"bar_pos", tomb3.bar_pos);
	REG_WriteLong((char*)"ammo_counter", tomb3.ammo_counter);
	REG_WriteFloat((char*)"GUI_Scale", tomb3.GUI_Scale);
	REG_WriteFloat((char*)"INV_Scale", tomb3.INV_Scale);
	REG_WriteFloat((char*)"unwater_music_mute", tomb3.unwater_music_mute);
	REG_WriteFloat((char*)"inv_music_mute", tomb3.inv_music_mute);

	CloseRegistry();
}

bool T3_LoadSettings()
{
	if (!OpenRegistry(SUB_KEY))
	{
		T3_InitSettings();
		return 0;
	}

#if (DIRECT3D_VERSION < 0x900)
	if (App.lpDeviceInfo->nDDInfo)
		REG_ReadLong((char*)"DD", (ulong&)App.lpDXConfig->nDD, 0);
#endif

#if (DIRECT3D_VERSION >= 0x900)
	if (App.lpDeviceInfo->nD3DInfo)
#else
	if (App.lpDeviceInfo->DDInfo[App.lpDXConfig->nDD].nD3DInfo)
#endif
	{
		REG_ReadLong((char*)"D3D", (ulong&)App.lpDXConfig->nD3D, 0);
#if (DIRECT3D_VERSION < 0x900)
		REG_ReadLong((char*)"D3DTF", (ulong&)App.lpDXConfig->D3DTF, 0);
#endif
	}

	if (App.lpDeviceInfo->nDSInfo)
		REG_ReadLong((char*)"DS", (ulong&)App.lpDXConfig->nDS, 0);

	REG_ReadLong((char*)"VM", (ulong&)App.lpDXConfig->nVMode, 0);
	REG_ReadLong((char*)"zbuffer", (ulong&)App.lpDXConfig->bZBuffer, 0);
	REG_ReadLong((char*)"dither", (ulong&)App.lpDXConfig->Dither, 0);
	REG_ReadLong((char*)"filter", (ulong&)App.lpDXConfig->Filter, 0);
	REG_ReadLong((char*)"sound", (ulong&)App.lpDXConfig->sound, 0);
	REG_ReadLong((char*)"SFXVolume", (ulong&)Option_SFX_Volume, 0);
	REG_ReadLong((char*)"MusicVolume", (ulong&)Option_Music_Volume, 0);
	REG_ReadBool((char*)"Window", App.Windowed, 0);
	REG_ReadLong((char*)"WindowX", (ulong&)App.rScreen.left, 0);
	REG_ReadLong((char*)"WindowY", (ulong&)App.rScreen.top, 0);
	REG_ReadFloat((char*)"Gamma", GammaOption, 0);

	REG_ReadBlock((char*)"keyLayout", &layout[1][0], sizeof(layout) / 2, 0);
	DefaultConflict();	//fix having to open the control options to set conflicts;

	REG_ReadBool((char*)"footprints", tomb3.footprints, 1);
	REG_ReadBool((char*)"pickup_display", tomb3.pickup_display, 1);
	REG_ReadBool((char*)"improved_rain", tomb3.improved_rain, 1);
	REG_ReadBool((char*)"improved_lasers", tomb3.improved_lasers, 1);
	REG_ReadBool((char*)"uwdust", tomb3.uwdust, 1);
	REG_ReadBool((char*)"flexible_crawl", tomb3.flexible_crawl, 1);
	REG_ReadBool((char*)"duck_roll", tomb3.duck_roll, 1);
	REG_ReadBool((char*)"flexible_sprint", tomb3.flexible_sprint, 1);
	REG_ReadBool((char*)"slide_to_run", tomb3.slide_to_run, 1);
	REG_ReadBool((char*)"kayak_mist", tomb3.kayak_mist, 1);
	REG_ReadBool((char*)"dozy", tomb3.dozy, 0);
	REG_ReadBool((char*)"disable_gamma", tomb3.disable_gamma, 1);
	REG_ReadBool((char*)"disable_ckey", tomb3.disable_ckey, 0);
	REG_ReadBool((char*)"crawl_tilt", tomb3.crawl_tilt, 1);
	REG_ReadBool((char*)"improved_poison_bar", tomb3.improved_poison_bar, 1);
	REG_ReadBool((char*)"custom_water_color", tomb3.custom_water_color, 1);
	REG_ReadBool((char*)"psx_text_colors", tomb3.psx_text_colors, 0);
	REG_ReadBool((char*)"upv_wake", tomb3.upv_wake, 1);
	REG_ReadBool((char*)"psx_fov", tomb3.psx_fov, 0);
	REG_ReadBool((char*)"psx_boxes", tomb3.psx_boxes, 0);
	REG_ReadBool((char*)"psx_mono", tomb3.psx_mono, 0);

	if (tomb3.gold)
		tomb3.psx_saving = 0;
	else
		REG_ReadBool((char*)"psx_saving", tomb3.psx_saving, 0);

	REG_ReadBool((char*)"psx_crystal_sfx", tomb3.psx_crystal_sfx, 0);
	REG_ReadBool((char*)"blue_crystal_light", tomb3.blue_crystal_light, 0);
	REG_ReadBool((char*)"improved_electricity", tomb3.improved_electricity, 1);
	REG_ReadBool((char*)"psx_contrast", tomb3.psx_contrast, 0);
	REG_ReadLong((char*)"shadow_mode", (ulong&)tomb3.shadow_mode, SHADOW_PSX);
	REG_ReadLong((char*)"bar_mode", (ulong&)tomb3.bar_mode, BAR_PSX);
	REG_ReadLong((char*)"sophia_rings", (ulong&)tomb3.sophia_rings, SRINGS_PSX);
	REG_ReadLong((char*)"bar_pos", (ulong&)tomb3.bar_pos, BPOS_ORIGINAL);
	REG_ReadLong((char*)"ammo_counter", (ulong&)tomb3.ammo_counter, ACTR_PC);
	REG_ReadFloat((char*)"GUI_Scale", tomb3.GUI_Scale, 1.0F);
	REG_ReadFloat((char*)"INV_Scale", tomb3.INV_Scale, 1.0F);
	REG_ReadFloat((char*)"unwater_music_mute", tomb3.unwater_music_mute, 0.8F);
	REG_ReadFloat((char*)"inv_music_mute", tomb3.inv_music_mute, 0.8F);

	CloseRegistry();
	return 1;
}

void T3_GoldifyString(char* string)
{
	char str[128];
	char buf[4];

	buf[0] = string[0];
	buf[1] = string[1];
	buf[2] = string[2];

	if (strstr(buf, "pix"))	//pix
	{
		strcpy(&str[4], &string[3]);	//the rest of the string
		str[0] = 'p';
		str[1] = 'i';
		str[2] = 'x';
		str[3] = 'g';
		strcpy(string, str);	//becomes pixg
		return;
	}

	buf[3] = string[3];

	if (strstr(buf, "data"))	//data
	{
		strcpy(&str[5], &string[4]);
		str[0] = 'd';
		str[1] = 'a';
		str[2] = 't';
		str[3] = 'a';
		str[4] = 'g';
		strcpy(string, str);	//becomes datag
		return;
	}
}
