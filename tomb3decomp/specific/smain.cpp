#include "../tomb3/pch.h"
#include "smain.h"
#include "specific.h"

#ifdef RANDO_STUFF
rando_info rando;

void LoadRandoInfo()
{
	FILE* file;

	file = fopen("levelinfo.dat", "rb");

	if (file)
	{
		fread(&rando, sizeof(rando_info), 1, file);
		fclose(file);
		return;
	}

	MessageBox(0, "Unable to load rando settings. Crashes may occur.", "TR Rando", MB_OK);
}
#endif

bool S_LoadSettings()
{
	FILE* file;
	char s[80];

#ifdef RANDO_STUFF
	LoadRandoInfo();
#endif

	file = fopen("data.bin", "rb");

	if (file)
	{
		fread(&savegame.best_assault_times, sizeof(long), sizeof(savegame.best_assault_times) / sizeof(long), file);
		fread(&savegame.best_quadbike_times, sizeof(long), sizeof(savegame.best_quadbike_times) / sizeof(long), file);
		fread(&savegame.QuadbikeKeyFlag, sizeof(long), 1, file);
		fclose(file);
	}

	file = fopen("config.txt", "r");

	if (!file)
	{
		Option_Music_Volume = 7;
		Option_SFX_Volume = 10;
		S_CDVolume(25 * Option_Music_Volume + 5);
		S_SoundSetMasterVolume(6 * Option_SFX_Volume + 4);
		return 0;
	}

	if (App.DeviceInfoPtr->nDDInfo)
	{
		fscanf(file, "%s", &s);
		fscanf(file, "%d\n", &App.DXConfigPtr->nDD);
	}

	if (App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].nD3DInfo)
	{
		fscanf(file, "%s", &s);
		fscanf(file, "%d\n", &App.DXConfigPtr->nD3D);
		fscanf(file, "%s", &s);
		fscanf(file, "%d\n", &App.DXConfigPtr->D3DTF);
	}

	if (App.DeviceInfoPtr->nDSInfo)
	{
		fscanf(file, "%s", &s);
		fscanf(file, "%d\n", &App.DXConfigPtr->DS);
	}

	if (App.DeviceInfoPtr->nDIInfo)
	{
		fscanf(file, "%s", &s);
		fscanf(file, "%d\n", &App.DXConfigPtr->DI);
	}

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->nVMode);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->bZBuffer);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->Dither);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->Filter);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->AGP);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->sound);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->Joystick);

	fscanf(file, "%s", &s);
	fscanf(file, "%d\n", &App.DXConfigPtr->MMX);

	fscanf(file, "%s", &s);
	fscanf(file, "%hd\n", &Option_SFX_Volume);

	fscanf(file, "%s", &s);
	fscanf(file, "%hd\n", &Option_Music_Volume);

	fscanf(file, "%s", &s);
	fscanf(file, "%f\n", &GammaOption);

	fscanf(file, "%s", &s);
	fscanf(file, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd\n",
		&layout[0][0],
		&layout[0][1],
		&layout[0][2],
		&layout[0][3],
		&layout[0][4],
		&layout[0][5],
		&layout[0][6],
		&layout[0][7],
		&layout[0][8],
		&layout[0][9],
		&layout[0][10],
		&layout[0][11],
		&layout[0][12],
		&layout[0][13]);

	fscanf(file, "%s", &s);
	fscanf(file, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd\n",
		&layout[0][14],
		&layout[1][0],
		&layout[1][1],
		&layout[1][2],
		&layout[1][3],
		&layout[1][4],
		&layout[1][5],
		&layout[1][6],
		&layout[1][7],
		&layout[1][8],
		&layout[1][9],
		&layout[1][10],
		&layout[1][11],
		&layout[1][12]);

	S_CDVolume(25 * Option_Music_Volume + 5);
	S_SoundSetMasterVolume(6 * Option_SFX_Volume + 4);
	fclose(file);
	return 1;
}

void inject_smain(bool replace)
{
	INJECT(0x0048CBF0, S_LoadSettings, replace);
}
