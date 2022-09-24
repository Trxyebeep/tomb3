#include "../tomb3/pch.h"
#include "smain.h"
#include "specific.h"
#include "../game/gameflow.h"
#include "../game/laraanim.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/sound.h"
#include "../game/missile.h"

#ifdef RANDO_STUFF
#include "init.h"

rando_info rando;
bool inject_rando = 1;

void LoadRandoInfo()
{
	FILE* file;

	file = fopen("levelinfo.dat", "rb");

	if (!file)
	{
		ShowCursor(1);
		MessageBox(0, "Unable to load rando settings. Crashes may occur.", "TR Rando", MB_OK);
		ShowCursor(0);
		return;
	}

	fread(&rando.nLevels, sizeof(uchar), 1, file);
	memset(rando.levels, 0, sizeof(rando.levels));

	if (!rando.nLevels)
	{
		ShowCursor(1);
		MessageBox(0, "Number of levels is 0.", "TR Rando", MB_OK);
		ShowCursor(0);
		return;
	}

	fread(rando.levels, sizeof(rando_level), rando.nLevels, file);
	fclose(file);
}
#else
bool inject_rando = 0;
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
		&layout[1][12],
		&layout[1][13]);

	S_CDVolume(25 * Option_Music_Volume + 5);
	S_SoundSetMasterVolume(6 * Option_SFX_Volume + 4);
	fclose(file);
	return 1;
}

void CheckCheatMode()
{
	static long mode, gun, turn;
	static short as, angle;

	if (CurrentLevel == LV_GYM || CurrentLevel == gameflow.num_levels - gameflow.num_demos - 1)
		return;

	as = lara_item->current_anim_state;

	switch (mode)
	{
	case 0:

		if (as == AS_WALK)
			mode = 1;

		break;

	case 1:
		gun = lara.gun_type == LG_PISTOLS;

		if (as != AS_WALK)
		{
			if (as == AS_STOP)
				mode = 2;
			else
				mode = 0;
		}

		break;

	case 2:

		if (as != AS_STOP)
		{
			if (as == AS_DUCK)
				mode = 3;
			else
				mode = 0;
		}

		break;

	case 3:

		if (as != AS_DUCK)
		{
			if (as == AS_STOP)
				mode = 4;
			else
				mode = 0;
		}

		break;

	case 4:

		if (as != AS_STOP)
		{
			angle = lara_item->pos.y_rot;
			turn = 0;

			if (as == AS_TURN_L)
				mode = 5;
			else if (as == AS_TURN_R)
				mode = 6;
			else
				mode = 0;
		}

		break;

	case 5:

		if (as == AS_TURN_L || as == AS_FASTTURN)
		{
			turn += short(lara_item->pos.y_rot - angle);
			angle = lara_item->pos.y_rot;
		}
		else if (turn < -0x17000)
			mode = 7;
		else
			mode = 0;

		break;

	case 6:

		if (as == AS_TURN_R || as == AS_FASTTURN)
		{
			turn += short(lara_item->pos.y_rot - angle);
			angle = lara_item->pos.y_rot;
		}
		else if (turn > 0x17000)
			mode = 7;
		else
			mode = 0;

		break;

	case 7:

		if (as != AS_STOP)
		{
			if (as == AS_COMPRESS)
				mode = 8;
			else
				mode = 0;
		}

		break;

	case 8:

		if (lara_item->fallspeed <= 0)
			break;

		if (gun)
			gun = lara.gun_type == LG_PISTOLS;

		if (gun)
		{
			if (as == AS_FORWARDJUMP)
				FinishLevelCheat = 1;
			else if (as == AS_BACKJUMP)
			{
				Inv_AddItem(M16_ITEM);
				Inv_AddItem(SHOTGUN_ITEM);
				Inv_AddItem(UZI_ITEM);
				Inv_AddItem(MAGNUM_ITEM);
				Inv_AddItem(GUN_ITEM);
				Inv_AddItem(ROCKET_GUN_ITEM);
				Inv_AddItem(GRENADE_GUN_ITEM);
				Inv_AddItem(HARPOON_ITEM);
				lara.magnums.ammo = 1000;
				lara.uzis.ammo = 1000;
				lara.shotgun.ammo = 1000;
				lara.harpoon.ammo = 1000;
				lara.rocket.ammo = 1000;
				lara.grenade.ammo = 1000;
				lara.m16.ammo = 1000;

				for (int i = 0; i < 50; i++)
				{
					Inv_AddItem(MEDI_ITEM);
					Inv_AddItem(BIGMEDI_ITEM);
					Inv_AddItem(FLARE_ITEM);
				}

				SoundEffect(SFX_LARA_HOLSTER, 0, SFX_ALWAYS);
			}
		}
		else if (as == AS_FORWARDJUMP || as == AS_BACKJUMP)
		{
			ExplodingDeath(lara.item_number, -1, 1);
			lara_item->hit_points = 0;
			lara_item->flags |= IFL_INVISIBLE;
		}
																	//no break on purpose
	default:
		mode = 0;
	}
}

void inject_smain(bool replace)
{
	INJECT(0x0048CBF0, S_LoadSettings, inject_rando ? 1 : replace);
	INJECT(0x0048C550, CheckCheatMode, replace);
}
