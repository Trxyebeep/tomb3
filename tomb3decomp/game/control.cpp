#include "../tomb3/pch.h"
#include "control.h"
#include "../specific/specific.h"
#include "../specific/smain.h"
#include "../specific/input.h"
#include "demo.h"
#include "gameflow.h"
#include "inventry.h"
#include "savegame.h"
#include "../specific/game.h"
#include "effect2.h"
#include "objects.h"
#include "box.h"
#include "laramisc.h"
#include "hair.h"
#include "sound.h"
#include "camera.h"
#include "effects.h"

long ControlPhase(long nframes, long demo_mode)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	static long framecount = 0;
	long return_val, falloff, r, g, b;
	short item_number, nex, room_number;

	if (nframes > 10)
		nframes = 10;

	for (framecount += nframes; framecount > 0; framecount -= 2)
	{
		if (cdtrack > 0)
			S_CDLoop();

		if (!gameflow.cheatmodecheck_disabled)
			CheckCheatMode();

		if (level_complete)
			return 1;

		S_UpdateInput();

		if (reset_flag)
			return 0x500;

		if (demo_mode)
		{
			if (input)
				return gameflow.on_demo_interrupt;

			GetDemoInput();

			if (input == -1)
			{
				input = 0;
				return gameflow.on_demo_end;
			}
		}
		else if (gameflow.noinput_timeout)
		{
			if (input)
				noinput_count = 0;
			else
			{
				noinput_count++;

				if (noinput_count > gameflow.noinput_time)
					return 0x400;
			}
		}

		if (lara.death_count > 300 || lara.death_count > 60 && input || overlay_flag == 2)
		{
			if (demo_mode)
				return gameflow.ondeath_demo_mode;

			if (CurrentLevel == LV_GYM)
				return 0x500;

			if (gameflow.ondeath_ingame)
				return  gameflow.ondeath_ingame;

			if (overlay_flag == 2)
			{
				overlay_flag = 1;
				return_val = Display_Inventory(5);

				if (return_val)
					return return_val;
			}
			else
				overlay_flag = 2;
		}

		if ((input & IN_OPTION || input & IN_LOAD || input & IN_SAVE || overlay_flag <= 0) && !lara.death_count && !lara.extra_anim)
		{
			if (overlay_flag > 0)
			{
				if (!gameflow.loadsave_disabled)
				{
					if (input & IN_LOAD)
						overlay_flag = -1;
					else if (input & IN_SAVE)
						overlay_flag = -2;
					else
						overlay_flag = 0;
				}
				else
					overlay_flag = 0;
			}
			else
			{
				if (overlay_flag == -1)
					return_val = Display_Inventory(4);
				else if (overlay_flag == -2)
					return_val = Display_Inventory(3);
				else
					return_val = Display_Inventory(0);

				overlay_flag = 1;

				if (return_val)
				{
					if (Inventory_ExtraData[0] != 1)
						return return_val;

					if (CurrentLevel == LV_GYM)
						return 1;

					CreateSaveGameInfo();
					S_SaveGame(&savegame, sizeof(SAVEGAME_INFO), Inventory_ExtraData[1]);
					S_SaveSettings();
				}
			}
		}

		ClearDynamics();
		item_number = next_item_active;

		while (item_number != NO_ITEM)
		{
			item = &items[item_number];
			nex = item->next_active;

			if (objects[item->object_number].control)
				objects[item->object_number].control(item_number);

			if (nex != NO_ITEM)
			{
				if (item->object_number == ROCKET || item->object_number == GRENADE)
				{
					if (!items[nex].active)
						nex = item_after_projectile;
				}
			}

			item_number = nex;
		}

		item_number = next_fx_active;

		while (item_number != NO_ITEM)
		{
			fx = &effects[item_number];
			nex = fx->next_active;

			if (objects[fx->object_number].control)
				objects[fx->object_number].control(item_number);

			item_number = nex;
		}

		room_number = lara_item->room_number;
		GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos, &room_number);

		if (KillEverythingFlag)
			KillEverything();

		if (SmokeCountL)
			SmokeCountL--;

		if (SmokeCountR)
			SmokeCountR--;

		if (SplashCount)
			SplashCount--;

		if (lara.has_fired && !(wibble & 0x7F))
		{
			AlertNearbyGuards(lara_item);
			lara.has_fired = 0;
		}

		if (lara.poisoned >= 16 && !(wibble & 0xFF))
		{
			if (lara.poisoned > 256)
				lara.poisoned = 256;

			lara_item->hit_points -= lara.poisoned >> 4;
			PoisonFlag = 16;
		}

		for (int i = 0; i < 15; i++)
			GotJointPos[i] = 0;

		lara_item->item_flags[0] = 0;
		LaraControl(0);
		HairControl(0);

		if (lara.electric)
		{
			if (lara.electric < 12)
			{
				r = (GetRandomControl() & 7) - lara.electric + 16;
				g = 32 - lara.electric;
				b = 31;
				falloff = (GetRandomControl() & 1) - 2 * lara.electric + 25;
			}
			else
			{
				r = 0;
				g = (GetRandomControl() & 7) + 8;
				b = (GetRandomControl() & 7) + 16;
				falloff = (GetRandomControl() & 3) + 8;
			}

			TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, falloff, r, g, b);
			SoundEffect(SFX_LARA_ELECTRIC_LOOP, &lara_item->pos, SFX_DEFAULT);
			SoundEffect(SFX_LARA_ELECTRIC_CRACKLES, &lara_item->pos, SFX_DEFAULT);
		}

		CalculateCamera();
		wibble = (wibble + 4) & 0xFC;

		if (CurrentLevel == LV_ANTARC || CurrentLevel == LV_MINES)
			LaraBreath(lara_item);

		UpdateSparks();
		UpdateSplashes();
		UpdateBats();
		SoundEffects();
		health_bar_timer--;

		if (CurrentLevel != LV_GYM || assault_timer_active)
			savegame.timer++;

		if (assault_penalty_display_timer)
			assault_penalty_display_timer--;

		if (QuadbikeLapTimeDisplayTimer)
			QuadbikeLapTimeDisplayTimer--;
	}

	return 0;
}

void inject_control(bool replace)
{
	INJECT(0x0041FFA0, ControlPhase, replace);
}
