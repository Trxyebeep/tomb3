#include "../tomb3/pch.h"
#include "smain.h"
#include "specific.h"
#include "../game/gameflow.h"
#include "../game/invfunc.h"
#include "../game/objects.h"
#include "../game/sound.h"
#include "../game/missile.h"
#include "../game/text.h"
#include "display.h"
#include "picture.h"
#include "../game/setup.h"
#include "time.h"
#include "../game/inventry.h"
#include "game.h"
#include "../game/savegame.h"
#include "winmain.h"
#include "input.h"
#include "frontend.h"
#include "../game/cinema.h"
#include "../game/demo.h"
#include "../game/lara.h"
#include "init.h"
#include "../newstuff/discord.h"
#include "../game/control.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "../newstuff/Picture2.h"
#endif
#include "../tomb3/tomb3.h"

long HiResFlag;
long title_loaded;
char exit_message[128];

bool S_LoadSettings()
{
	FILE* file;

	file = fopen("data.bin", "rb");

	if (file)
	{
		fread(&savegame.best_assault_times, sizeof(long), sizeof(savegame.best_assault_times) / sizeof(long), file);
		fread(&savegame.best_quadbike_times, sizeof(long), sizeof(savegame.best_quadbike_times) / sizeof(long), file);
		fread(&savegame.QuadbikeKeyFlag, sizeof(long), 1, file);
		fclose(file);
	}

	return T3_LoadSettings();
}

void S_SaveSettings()
{
	FILE* file;

	T3_SaveSettings();
	file = fopen("data.bin", "wb+");

	if (file)
	{
		fwrite(savegame.best_assault_times, sizeof(ulong), sizeof(savegame.best_assault_times) / sizeof(long), file);
		fwrite(savegame.best_quadbike_times, sizeof(ulong), sizeof(savegame.best_quadbike_times) / sizeof(long), file);
		fwrite(&savegame.QuadbikeKeyFlag, sizeof(ulong), 1, file);
		fclose(file);
	}
}

void CheckCheatMode()
{
	static long mode, gun, turn;
	static short as, angle;

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
			{
				if (CurrentLevel != LV_GYM)
					FinishLevelCheat = 1;
			}
			else if (as == AS_BACKJUMP)
			{
				if (CurrentLevel == LV_GYM)
				{
					Inv_AddItem(KEY_ITEM1);

					for (int i = 0; i < 50; i++)
						Inv_AddItem(FLARE_ITEM);
				}
				else
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

						if (tomb3.psx_saving)
							Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);
					}
				}

				SoundEffect(SFX_LARA_HOLSTER, 0, SFX_ALWAYS);
			}
		}
		else if (as == AS_FORWARDJUMP || as == AS_BACKJUMP)
		{
			if (CurrentLevel != LV_GYM)
			{
				ExplodingDeath(lara.item_number, -1, 1);
				lara_item->hit_points = 0;
				lara_item->flags |= IFL_INVISIBLE;
			}
		}
		
		//fallthrough

	default:
		mode = 0;
	}
}

long TitleSequence()
{
	char name[128];

	T_InitPrint();
	TempVideoAdjust(1, 1.0);
	noinput_count = 0;
	dontFadePicture = 1;

#if (DIRECT3D_VERSION >= 0x900)
	RemoveMonoScreen(0);
#endif

	if (tomb3.gold)
	{
		strcpy(name, GF_titlefilenames[1]);
		T3_GoldifyString(name);
#if (DIRECT3D_VERSION >= 0x900)
		LoadPicture(name);
#else
		LoadPicture(name, App.PictureBuffer);
#endif
	}
	else
#if (DIRECT3D_VERSION >= 0x900)
		LoadPicture(GF_titlefilenames[1]);
#else
		LoadPicture(GF_titlefilenames[1], App.PictureBuffer);
#endif

	FadePictureUp(32);

	if (!title_loaded)
	{
		if (!InitialiseLevel(0, 0))
			return EXITGAME;

		title_loaded = 1;
	}

	S_CDStop();

	if (gameflow.title_track)
		S_CDPlay(gameflow.title_track, 1);

	TIME_Init();
	Display_Inventory(INV_TITLE_MODE);
	dontFadePicture = 0;
	FadePictureDown(32);
	S_CDStop();

	if (reset_flag)
	{
		reset_flag = 0;
		return STARTDEMO;
	}

	if (Inventory_Chosen == PHOTO_OPTION)
		return STARTGAME;

	if (Inventory_Chosen == PASSPORT_OPTION)
	{
		if (!Inventory_ExtraData[0])
		{
			Inv_RemoveAllItems();
			S_LoadGame(&savegame, sizeof(SAVEGAME_INFO), Inventory_ExtraData[1]);
			return Inventory_ExtraData[1] | STARTSAVEDGAME;
		}

		if (Inventory_ExtraData[0] == 1)
		{
			InitialiseStartInfo();

			if (gameflow.play_any_level)
				return STARTGAME | (Inventory_ExtraData[1] + 1);

			return STARTGAME | LV_FIRSTLEVEL;
		}
	}

	return EXITGAME;
}

long GameMain()
{
	long s, lp, level;

	HiResFlag = 0;
	screen_sizer = 1.0F;
	game_sizer = 1.0F;

	if (!S_InitialiseSystem())
		return 0;

	RPC_Init();

	if (tomb3.gold)
	{
		if (!GF_LoadScriptFile("datag\\trtla.dat"))	//seems some CDs come with trtla.dat, others with tombPC.dat, others with both!
		{
			if (!GF_LoadScriptFile("datag\\tombPC.dat"))
				S_ExitSystem("GameMain: could not load script file");
		}
	}
	else
	{
		if (!GF_LoadScriptFile("data\\tombPC.dat"))
			S_ExitSystem("GameMain: could not load script file");
	}

	SOUND_Init();
	InitialiseStartInfo();
	S_FrontEndCheck(&savegame, sizeof(SAVEGAME_INFO));
	HiResFlag = -1;
	malloc_buffer = (char*)GlobalAlloc(GMEM_FIXED, MALLOC_SIZE);

	if (!malloc_buffer)
	{
		lstrcpy(exit_message, "GameMain: could not allocate malloc_buffer");
		return 0;
	}

	HiResFlag = 0;
	TempVideoAdjust(1, 1.0);
	S_UpdateInput();

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.gold)
		LoadPicture("pixg\\legal.bmp");
	else
		LoadPicture("pix\\legal.bmp");
#else
	if (tomb3.gold)
		LoadPicture("pixg\\legal.bmp", App.PictureBuffer);
	else
		LoadPicture("pix\\legal.bmp", App.PictureBuffer);
#endif

	FadePictureUp(32);
	S_Wait(150 * TICKS_PER_FRAME, 1);
	ForceFadeDown(1);
	FadePictureDown(32);
	s = GF_DoFrontEndSequence();

	if (GtWindowClosed)
		return 1;

	if (s == 1)
	{
		lstrcpy(exit_message, "GameMain: failed in GF_DoFrontEndSequence()");
		return 0;
	}

	s = gameflow.firstOption;
	title_loaded = 0;
	lp = 1;
	
	while (lp)
	{
		level = s & 0xFF;
		s &= ~0xFF;

		switch (s)
		{
		case STARTGAME:

			if (gameflow.singlelevel < 0)
			{
				if (level > gameflow.num_levels)
				{
					wsprintf(exit_message, "GameMain: STARTGAME with invalid level number (%d)", level);
					return 0;
				}

				s = GF_DoLevelSequence(level, 1);
			}
			else
				s = GF_DoLevelSequence(gameflow.singlelevel, 1);

			break;

		case STARTSAVEDGAME:
			S_LoadGame(&savegame, sizeof(SAVEGAME_INFO), level);

			if (savegame.current_level > gameflow.num_levels)
			{
				wsprintf(exit_message, "GameMain: STARTSAVEDGAME with invalid level number (%d)", savegame.current_level);
				return 0;
			}

			s = GF_DoLevelSequence(savegame.current_level, 2);
			break;

		case STARTCINE:
			StartCinematic(level);
			s = EXIT_TO_TITLE;
			break;

		case STARTDEMO:
			s = DoDemoSequence(-1);
			break;

		case EXIT_TO_TITLE:
		case EXIT_TO_OPTION:

			if (gameflow.title_disabled)
			{
				s = gameflow.title_replace;

				if (gameflow.title_replace < 0 || gameflow.title_replace == EXIT_TO_TITLE)
				{
					lstrcpy(exit_message, "GameMain Failed: Title disabled & no replacement");
					return 0;
				}
			}
			else
			{
				s = TitleSequence();
				GF_StartGame = 1;
			}

			break;

		case LEVELCOMPLETE:
			s = LevelCompleteSequence();
			break;

		case EXITGAME:
		default:
			lp = 0;
			break;
		}
	}

	if (nLoadedPictures)
	{
		ForceFadeDown(1);
		FadePictureDown(32);
	}

	S_SaveSettings();
	ShutdownGame();
	return 1;
}
