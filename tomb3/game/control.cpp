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
#include "../3dsystem/phd_math.h"
#include "../specific/winmain.h"
#include "traps.h"
#include "fish.h"
#include "items.h"
#include "lot.h"
#include "pickup.h"
#include "draw.h"
#include "moveblok.h"
#include "setup.h"
#include "../specific/file.h"
#include "lara.h"
#include "health.h"
#include "../newstuff/pausemenu.h"
#include "../newstuff/discord.h"

ITEM_INFO* items;
ANIM_STRUCT* anims;
ROOM_INFO* room;
short** meshes;
long* bones;
long level_items;
short number_rooms;

short* trigger_index;
long tiltxoff;
long tiltyoff;
long OnObject;
long height_type;
uchar CurrentAtmosphere;
uchar IsAtmospherePlaying;

long CurrentLevel;
long level_complete;
long chunky_flag;
long GnGameMode;
long reset_flag;
long noinput_count;
long overlay_flag = 1;
short FXType;
short item_after_projectile;
short cdtrack = -1;
char PoisonFlag;

long flipmap[10];
long flip_status;
long flipeffect = -1;
long fliptimer;
char cd_flags[128];
uchar HeavyTriggered;
static long number_los_rooms;
static long los_rooms[20];

long ControlPhase(long nframes, long demo_mode)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	static long framecount = 0;
	long return_val, falloff, r, g, b;
	short item_number, nex, room_number;

	if (nframes > TICKS_PER_FRAME * 5)
		nframes = TICKS_PER_FRAME * 5;

	for (framecount += nframes; framecount > 0; framecount -= TICKS_PER_FRAME)
	{
		RPC_Update();

		if (!gameflow.cheatmodecheck_disabled)
			CheckCheatMode();

		if (level_complete)
			return 1;

		S_UpdateInput();

		if (reset_flag)
			return EXIT_TO_TITLE;

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
					return STARTDEMO;
			}
		}

		if (lara.death_count > 300 || lara.death_count > 60 && input || overlay_flag == 2)
		{
			if (demo_mode)
				return gameflow.ondeath_demo_mode;

			if (CurrentLevel == LV_GYM)
				return EXIT_TO_TITLE;

			if (gameflow.ondeath_ingame)
				return gameflow.ondeath_ingame;

			if (overlay_flag == 2)
			{
				overlay_flag = 1;
				return_val = Display_Inventory(INV_DEATH_MODE);

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
					return_val = Display_Inventory(INV_LOAD_MODE);
				else if (overlay_flag == -2)
					return_val = Display_Inventory(INV_SAVE_MODE);
				else
					return_val = Display_Inventory(INV_GAME_MODE);

				overlay_flag = 1;

				if (return_val)
				{
					if (Inventory_ExtraData[0] != 1)
						return return_val;

					if (CurrentLevel == LV_GYM)
						return STARTGAME | LV_FIRSTLEVEL;

					CreateSaveGameInfo();
					S_SaveGame(&savegame, sizeof(SAVEGAME_INFO), Inventory_ExtraData[1]);
					S_SaveSettings();
				}
			}
		}

		if (input & IN_PAUSE && !lara.death_count && !lara.extra_anim)
		{
			if (S_Pause())
				return 1;
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
				b = 255;
				falloff = (GetRandomControl() & 1) - 2 * lara.electric + 25;
				r <<= 3;
				g <<= 3;
			}
			else
			{
				r = 0;
				g = (GetRandomControl() & 0x3F) + 64;
				b = (GetRandomControl() & 0x3F) + 128;
				falloff = (GetRandomControl() & 3) + 8;
			}

			TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, falloff, r, g, b);
			SoundEffect(SFX_LARA_ELECTRIC_LOOP, &lara_item->pos, SFX_DEFAULT);
			SoundEffect(SFX_LARA_ELECTRIC_CRACKLES, &lara_item->pos, SFX_DEFAULT);
		}

		CalculateCamera();
		wibble = (wibble + 4) & 0xFC;

		if (GF_Cold)
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

void AnimateItem(ITEM_INFO* item)
{
	ANIM_STRUCT* anim;
	short* command;
	long speed;
	ushort type, num;

	anim = &anims[item->anim_number];
	item->touch_bits = 0;
	item->hit_status = 0;
	item->frame_number++;

	if (anim->number_changes > 0 && GetChange(item, anim))
	{
		anim = &anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;

		if (item->required_anim_state == item->current_anim_state)
			item->required_anim_state = 0;
	}

	if (item->frame_number > anim->frame_end)
	{
		if (anim->number_commands > 0)
		{
			command = &commands[anim->command_index];

			for (int i = 0; i < anim->number_commands; i++)
			{
				switch (*command++)
				{
				case COMMAND_MOVE_ORIGIN:
					TranslateItem(item, *command, command[1], command[2]);
					command += 3;
					break;

				case COMMAND_JUMP_VELOCITY:
					item->fallspeed = *command++;
					item->speed = *command++;
					item->gravity_status = 1;
					break;

				case COMMAND_DEACTIVATE:

					if (objects[item->object_number].intelligent)
						item->after_death = 1;
					else
						item->after_death = 64;

					item->status = ITEM_DEACTIVATED;
					break;

				case COMMAND_SOUND_FX:
				case COMMAND_EFFECT:
					command += 2;
					break;
				}
			}
		}

		item->anim_number = anim->jump_anim_num;
		item->frame_number = anim->jump_frame_num;
		anim = &anims[item->anim_number];

		if (item->current_anim_state != anim->current_anim_state)
		{
			item->current_anim_state = anim->current_anim_state;
			item->goal_anim_state = anim->current_anim_state;
		}

		if (item->required_anim_state == item->current_anim_state)
			item->required_anim_state = 0;
	}

	if (anim->number_commands > 0)
	{
		command = &commands[anim->command_index];

		for (int i = 0; i < anim->number_commands; i++)
		{
			switch (*command++)
			{
			case COMMAND_MOVE_ORIGIN:
				command += 3;
				break;

			case COMMAND_JUMP_VELOCITY:
				command += 2;
				break;

			case COMMAND_SOUND_FX:

				if (item->frame_number == *command)
				{
					num = command[1] & 0x3FFF;
					type = command[1] & 0xC000;

					if (objects[item->object_number].water_creature)
						SoundEffect(num, &item->pos, SFX_WATER);
					else if (item->room_number == NO_ROOM)
					{
						item->pos.x_pos = lara_item->pos.x_pos;
						item->pos.y_pos = lara_item->pos.y_pos - 762;
						item->pos.z_pos = lara_item->pos.z_pos;

						if (item->object_number == HARPOON_GUN)
							SoundEffect(num, &item->pos, SFX_ALWAYS);
						else
							SoundEffect(num, &item->pos, SFX_DEFAULT);
					}
					else if (room[item->room_number].flags & ROOM_UNDERWATER)
					{
						if (type == SFX_LANDANDWATER || type == SFX_WATERONLY)
							SoundEffect(num, &item->pos, SFX_DEFAULT);
					}
					else if (type == SFX_LANDANDWATER || type == SFX_LANDONLY)
						SoundEffect(num, &item->pos, SFX_DEFAULT);
				}

				command += 2;
				break;

			case COMMAND_EFFECT:

				if (item->frame_number == *command)
				{
					FXType = command[1] & 0xC000;
					num = command[1] & 0x3FFF;
					effect_routines[num](item);
				}
				
				command += 2;
				break;
			}
		}
	}

	if (item->gravity_status)
	{
		item->fallspeed += item->fallspeed >= 128 ? 1 : 6;
		item->pos.y_pos += item->fallspeed;
	}
	else
	{
		speed = anim->velocity;

		if (anim->acceleration)
			speed += anim->acceleration * (item->frame_number - anim->frame_base);

		item->speed = short(speed >> 16);
	}

	item->pos.x_pos += (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.z_pos += (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
}

long GetChange(ITEM_INFO* item, ANIM_STRUCT* anim)
{
	CHANGE_STRUCT* change;
	RANGE_STRUCT* range;

	if (item->current_anim_state == item->goal_anim_state)
		return 0;

	change = &changes[anim->change_index];

	for (int i = 0; i < anim->number_changes; i++, change++)
	{
		if (change->goal_anim_state == item->goal_anim_state && change->number_ranges > 0)
		{
			range = &ranges[change->range_index];

			for (int j = 0; j < change->number_ranges; j++, range++)
			{
				if (item->frame_number >= range->start_frame && item->frame_number <= range->end_frame)
				{
					item->anim_number = range->link_anim_num;
					item->frame_number = range->link_frame_num;
					return 1;
				}
			}
		}
	}

	return 0;
}

void TranslateItem(ITEM_INFO* item, long x, long y, long z)
{
	long c, s;

	c = phd_cos(item->pos.y_rot);
	s = phd_sin(item->pos.y_rot);
	item->pos.x_pos += (z * s + x * c) >> W2V_SHIFT;
	item->pos.y_pos += y;
	item->pos.z_pos += (z * c - x * s) >> W2V_SHIFT;
}

FLOOR_INFO* GetFloor(long x, long y, long z, short* room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	long x_floor, y_floor;
	short data;

	r = &room[*room_number];

	do
	{
		x_floor = (z - r->z) >> WALL_SHIFT;
		y_floor = (x - r->x) >> WALL_SHIFT;

		if (x_floor <= 0)
		{
			x_floor = 0;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (x_floor >= r->x_size - 1)
		{
			x_floor = r->x_size - 1;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->y_size)
			y_floor = r->y_size - 1;

		floor = &r->floor[x_floor + y_floor * r->x_size];
		data = GetDoor(floor);

		if (data != NO_ROOM)
		{
			*room_number = data;
			r = &room[data];
		}

	} while (data != NO_ROOM);

	if (y >= GetMaximumFloor(floor, x, z))
	{
		do
		{
			if (floor->pit_room == NO_ROOM)
				return floor;

			if(CheckNoColFloorTriangle(floor, x, z) == 1)
				break;

			*room_number = floor->pit_room;
			r = &room[floor->pit_room];
			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];

		} while (y >= GetMaximumFloor(floor, x, z));
	}
	else if (y < GetMinimumCeiling(floor, x, z))
	{
		do
		{
			if (floor->sky_room == NO_ROOM)
				return floor;

			if (CheckNoColCeilingTriangle(floor, x, z) == 1)
				break;

			*room_number = floor->sky_room;
			r = &room[floor->sky_room];
			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];

		} while (y < GetMinimumCeiling(floor, x, z));
	}

	return floor;
}

long GetWaterHeight(long x, long y, long z, short room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	long x_floor, y_floor;
	short data;

	r = &room[room_number];

	do
	{
		x_floor = (z - r->z) >> WALL_SHIFT;
		y_floor = (x - r->x) >> WALL_SHIFT;

		if (x_floor <= 0)
		{
			x_floor = 0;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (x_floor >= r->x_size - 1)
		{
			x_floor = r->x_size - 1;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->y_size)
			y_floor = r->y_size - 1;

		floor = &r->floor[x_floor + y_floor * r->x_size];
		data = GetDoor(floor);

		if (data != NO_ROOM)
		{
			room_number = data;
			r = &room[data];
		}

	} while (data != NO_ROOM);

	if (r->flags & (ROOM_UNDERWATER | ROOM_SWAMP))
	{
		while (floor->sky_room != NO_ROOM)
		{
			if (CheckNoColCeilingTriangle(floor, x, z) == 1)
				break;

			r = &room[floor->sky_room];

			if (!(r->flags & (ROOM_UNDERWATER | ROOM_SWAMP)))
				break;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}

		return GetMinimumCeiling(floor, x, z);
	}
	else
	{
		while (floor->pit_room != NO_ROOM)
		{
			if (CheckNoColFloorTriangle(floor, x, z) == 1)
				break;

			r = &room[floor->pit_room];

			if (r->flags & (ROOM_UNDERWATER | ROOM_SWAMP))
				return GetMaximumFloor(floor, x, z);

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}

	return NO_HEIGHT;
}

long GetHeight(FLOOR_INFO* floor, long x, long y, long z)
{
	ROOM_INFO* r;
	ITEM_INFO* item;
	short* data;
	long h, h2, xoff, yoff, ch1, ch2;
	short type, trigger, tilts, t0, t1, t2, t3, dx, dz, hadj;

	OnObject = 0;
	height_type = WALL;

	while (floor->pit_room != NO_ROOM)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
	}

	h = floor->floor << 8;

	if (h == NO_HEIGHT)
		return h;

	if (GF_NoFloor && GF_NoFloor == h)
		h = 0x4000;

	h2 = h;
	trigger_index = 0;

	if (!floor->index)
		return h;

	tiltxoff = 0;
	tiltyoff = 0;
	data = &floor_data[floor->index];

	do
	{
		type = *data++;

		switch (type & 0x1F)
		{
		case DOOR_TYPE:
		case ROOF_TYPE:
		case SPLIT3:
		case SPLIT4:
		case NOCOLC1T:
		case NOCOLC1B:
		case NOCOLC2T:
		case NOCOLC2B:
			data++;
			break;

		case TILT_TYPE:
			xoff = *data >> 8;
			yoff = (char)*data;
			tiltxoff = xoff;
			tiltyoff = yoff;

			if (!chunky_flag || (abs(xoff) <= 2 && abs(yoff) <= 2))
			{
				if (abs(xoff) > 2 || abs(yoff) > 2)
					height_type = BIG_SLOPE;
				else
					height_type = SMALL_SLOPE;

				if (xoff < 0)
					h -= xoff * (z & WALL_MASK) >> 2;
				else
					h += xoff * ((-1 - (ushort)z) & WALL_MASK) >> 2;

				if (yoff < 0)
					h -= (yoff * (x & WALL_MASK)) >> 2;
				else
					h += (yoff * ((-1 - (ushort)x) & WALL_MASK)) >> 2;
			}

			data++;
			break;

		case TRIGGER_TYPE:

			if (!trigger_index)
				trigger_index = data - 1;

			data++;

			do
			{
				trigger = *data++;

				if (((trigger & 0x3C00) != (TO_OBJECT << 10)))
				{
					if (((trigger & 0x3C00) == (TO_CAMERA << 10)))
						trigger = *data++;

					continue;
				}

				item = &items[trigger & 0x3FF];

				if (objects[item->object_number].floor)
					objects[item->object_number].floor(item, x, y, z, &h);

			} while (!(trigger & 0x8000));

			break;

		case LAVA_TYPE:
			trigger_index = data - 1;
			break;

		case CLIMB_TYPE:
		case MONKEY_TYPE:
		case MINEL_TYPE:
		case MINER_TYPE:

			if (!trigger_index)
				trigger_index = data - 1;

			break;

		case SPLIT1:
		case SPLIT2:
		case NOCOLF1T:
		case NOCOLF1B:
		case NOCOLF2T:
		case NOCOLF2B:
			tilts = *data;
			t0 = tilts & 15;
			t1 = (tilts >> 4) & 15;
			t2 = (tilts >> 8) & 15;
			t3 = (tilts >> 12) & 15;
			dx = x & WALL_MASK;
			dz = z & WALL_MASK;
			xoff = 0;
			yoff = 0;
			height_type = SPLIT_TRI;

			if ((type & 0x1F) != SPLIT1 && (type & 0x1F) != NOCOLF1T && (type & 0x1F) != NOCOLF1B)
			{
				if (dx <= dz)
				{
					hadj = (type >> 10) & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					h += hadj << 8;
					xoff = t2 - t1;
					yoff = t3 - t2;
				}
				else
				{
					hadj = (type >> 5) & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					h += hadj << 8;
					xoff = t3 - t0;
					yoff = t0 - t1;
				}
			}
			else
			{
				if (dx <= (WALL_SIZE - dz))
				{
					hadj = (type >> 10) & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					h += hadj << 8;
					xoff = t2 - t1;
					yoff = t0 - t1;
				}
				else
				{
					hadj = (type >> 5) & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					h += hadj << 8;
					xoff = t3 - t0;
					yoff = t3 - t2;
				}
			}

			tiltxoff = xoff;
			tiltyoff = yoff;

			if (chunky_flag)
			{
				hadj = (hadj >> 10) & 0x1F;

				if (hadj & 0x10)
					hadj |= 0xFFF0;

				ch1 = h2;
				ch2 = ch1;
				ch1 += hadj << 8;

				hadj = (hadj >> 5) & 0x1F;

				if (hadj & 0x10)
					hadj |= 0xFFF0;

				ch2 += hadj << 8;

				if (ch1 < ch2)
					h = ch1;
				else
					h = ch2;
			}
			else
			{
				if (abs(xoff) > 2 || abs(yoff) > 2)
					height_type = DIAGONAL;
				else if (height_type != SPLIT_TRI)
					height_type = SMALL_SLOPE;

				if (xoff < 0)
					h -= xoff * (z & WALL_MASK) >> 2;
				else
					h += xoff * ((-1 - (ushort)z) & WALL_MASK) >> 2;

				if (yoff < 0)
					h -= yoff * (x & WALL_MASK) >> 2;
				else
					h += yoff * ((-1 - (ushort)x) & WALL_MASK) >> 2;
			}

			data++;
			break;

		default:
			S_ExitSystem("GetHeight(): Unknown type");
		}

	} while (!(type & 0x8000));

	return h;
}

void RefreshCamera(short type, short* data)
{
	short trigger, value, target_ok;

	target_ok = 2;

	do
	{
		trigger = *data++;
		value = trigger & 0x3FF;

		if (((trigger >> 10) & 0xF) == TO_CAMERA)
		{
			data++;

			if (value == camera.last)
			{
				camera.number = trigger & 0x3FF;

				if (camera.timer >= 0 && (camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA))
				{
					camera.type = FIXED_CAMERA;
					target_ok = 1;
					continue;
				}

				camera.timer = -1;
			}

			target_ok = 0;
		}
		else if (((trigger >> 10) & 0xF) == TO_TARGET && camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA)
			camera.item = &items[value];

	} while (!(trigger & 0x8000));

	if (camera.item && (!target_ok || (target_ok == 2 && camera.item->looked_at && camera.item != camera.last_item)))
		camera.item = 0;

	if (camera.number == -1 && camera.timer > 0)
		camera.timer = -1;
}

void TestTriggers(short* data, long heavy)
{
	ITEM_INFO* item;
	ITEM_INFO* camera_item;
	long quad, switch_off, neweffect, flip, flip_available;
	short type, flags, value, trigger, camera_flags, camera_timer;
	char timer;

	switch_off = 0;
	flip = -1;
	flip_available = 0;
	neweffect = -1;
	HeavyTriggered = 0;

	if (!heavy)
	{
		lara.CanMonkeySwing = 0;
		lara.MineL = 0;
		lara.MineR = 0;
		lara.climb_status = 0;
	}

	if (!data)
		return;

	if ((*data & 0x1F) == LAVA_TYPE)
	{
		if (!heavy && (lara_item->pos.y_pos == lara_item->floor || lara.water_status))
			LavaBurn(lara_item);

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == CLIMB_TYPE)
	{
		if (!heavy)
		{
			quad = ushort(lara_item->pos.y_rot + 8192) >> 14;

			if ((1 << (quad + 8)) & *data)
				lara.climb_status = 1;
		}

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == MONKEY_TYPE)
	{
		if (!heavy)
			lara.CanMonkeySwing = 1;

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == MINEL_TYPE)
	{
		if (!heavy)
			lara.MineL = 1;

		if (*data & 0x8000)
			return;

		data++;
	}

	if ((*data & 0x1F) == MINER_TYPE)
	{
		if (!heavy)
			lara.MineR = 1;

		if (*data & 0x8000)
			return;

		data++;
	}

	type = (*data++ >> 8) & 0x3F;
	flags = *data++;
	timer = flags & 0xFF;

	if (camera.type != HEAVY_CAMERA)
		RefreshCamera(type, data);

	if (heavy)
	{
		if (type != HEAVY)
			return;
	}
	else
	{
		switch (type)
		{
		case PAD:
		case ANTIPAD:
			LaraOnPad = 1;

			if (lara_item->pos.y_pos != lara_item->floor || lara_item->item_flags[0])
				return;

			if (CurrentLevel == LV_GYM)
			{
				if (lara_item->item_flags[1] == 1)
					return;

				lara_item->item_flags[1] = 1;
			}

			break;

		case SWITCH:
			value = *data++ & 0x3FF;

			if (flags & IFL_INVISIBLE)
				items[value].item_flags[0] = 1;

			if (!SwitchTrigger(value, timer))
				return;

			switch_off = items[value].current_anim_state == 1;
			break;

		case KEY:
			value = *data++ & 0x3FF;

			if (KeyTrigger(value))
				break;

			return;

		case PICKUP:
			value = *data++ & 0x3FF;

			if (PickupTrigger(value))
				break;

			return;

		case COMBAT:

			if (lara.gun_status == LG_READY)
				break;

			//fallthrough

		case HEAVY:
		case DUMMY:
			return;
		}
	}

	camera_item = 0;

	do
	{
		trigger = *data++;
		value = trigger & 0x3FF;

		switch ((trigger & 0x3FFF) >> 10)
		{
		case TO_OBJECT:
			item = &items[value];

			if (((type == ANTIPAD || type == ANTITRIGGER) && item->flags & IFL_ANTITRIGGER_ONESHOT) ||
				(type == SWITCH && item->flags & IFL_SWITCH_ONESHOT) ||
				(type != SWITCH && type != ANTIPAD && type != ANTITRIGGER && item->flags & IFL_INVISIBLE))
				break;

			if (type != ANTIPAD && type != ANTITRIGGER)
			{
				if ((item->object_number == DART_EMITTER || item->object_number == HOMING_DART_EMITTER) && item->active)
					break;

				if (item->object_number == TROPICAL_FISH || item->object_number == PIRAHNAS)
				{
					item->hit_points = timer & 7;
					SetupShoal(item->hit_points);
					timer = 0;
				}
				else if (item->object_number >= SECURITY_LASER_ALARM && item->object_number <= SECURITY_LASER_KILLER)
				{
					timer &= 7;

					if (!timer)
						timer = 1;

					item->hit_points = timer;
					timer = 0;
				}
				else if (item->object_number == CEILING_SPIKES || item->object_number == PENDULUM || item->object_number == FIREHEAD)
				{
					item->item_flags[0] = timer;

					if (item->object_number == FIREHEAD)
						item->hit_points = timer;

					timer = 0;
				}
			}

			item->timer = timer;

			if (timer != 1)
				item->timer *= 30;

			if (type == SWITCH)
			{
				item->flags ^= (flags & IFL_CODEBITS);

				if (flags & IFL_INVISIBLE)
					item->flags |= IFL_SWITCH_ONESHOT;
			}
			else if (type == ANTIPAD || type == ANTITRIGGER)
			{
				if (item->object_number == TROPICAL_FISH || item->object_number == PIRAHNAS)
					lead_info[item->hit_points].on = 0;

				if (item->object_number == EARTHQUAKE)
				{
					item->item_flags[0] = 0;
					item->item_flags[1] = 100;
				}

				item->flags &= ~(IFL_CODEBITS | IFL_REVERSE);

				if (flags & IFL_INVISIBLE)
					item->flags |= IFL_ANTITRIGGER_ONESHOT;
			}
			else if (flags & IFL_CODEBITS)
				item->flags |= flags & IFL_CODEBITS;

			if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
				break;

			if (flags & IFL_INVISIBLE)
				item->flags |= IFL_INVISIBLE;

			if (!item->active)
			{
				if (objects[item->object_number].intelligent)
				{
					if (item->status == ITEM_INACTIVE)
					{
						item->touch_bits = 0;
						item->status = ITEM_ACTIVE;
						AddActiveItem(value);

						if (item->object_number != TARGETS)
							EnableBaddieAI(value, 1);
					}
					else if (item->status == ITEM_INVISIBLE)
					{
						item->touch_bits = 0;

						if (EnableBaddieAI(value, 0))
							item->status = ITEM_ACTIVE;
						else
							item->status = ITEM_INVISIBLE;

						AddActiveItem(value);
					}
				}
				else
				{
					item->touch_bits = 0;
					AddActiveItem(value);
					item->status = ITEM_ACTIVE;
					HeavyTriggered = (uchar)heavy;
				}
			}

			break;

		case TO_CAMERA:
			trigger = *data++;
			camera_flags = trigger;
			camera_timer = trigger & 0xFF;

			if (camera.fixed[value].flags & IFL_INVISIBLE)
				break;

			camera.number = value;

			if ((camera.type == LOOK_CAMERA || camera.type == COMBAT_CAMERA) || type == COMBAT || (type == SWITCH && timer && switch_off))
				break;

			if (camera.number != camera.last || type == SWITCH)
			{
				camera.timer = camera_timer * 30;

				if (camera_flags & IFL_INVISIBLE)
					camera.fixed[camera.number].flags |= IFL_INVISIBLE;

				camera.speed = ((camera_flags & IFL_CODEBITS) >> 6) + 1;

				if (heavy)
					camera.type = HEAVY_CAMERA;
				else
					camera.type = FIXED_CAMERA;
			}

			break;

		case TO_SINK:
			lara.current_active = value + 1;
			break;

		case TO_FLIPMAP:
			flip_available = 1;

			if (flipmap[value] & IFL_INVISIBLE)
				break;

			if (type == SWITCH)
				flipmap[value] ^= flags & IFL_CODEBITS;
			else if (flags & IFL_CODEBITS)
				flipmap[value] |= flags & IFL_CODEBITS;

			if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS)
			{
				if (flags & IFL_INVISIBLE)
					flipmap[value] |= IFL_INVISIBLE;

				if (!flip_status)
					flip = 1;
			}
			else if (flip_status)
				flip = 1;

			break;

		case TO_FLIPON:
			flip_available = 1;

			if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS && !flip_status)
				flip = 1;

			break;

		case TO_FLIPOFF:
			flip_available = 1;

			if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS && flip_status)
				flip = 1;

			break;

		case TO_TARGET:
			camera_item = &items[value];
			break;

		case TO_FINISH:
			level_complete = 1;
			break;

		case TO_CD:
			TriggerCDTrack(value, flags, type);
			break;

		case TO_FLIPEFFECT:
			neweffect = value;
			break;

		case TO_SECRET:

			if (!(savegame.secrets & 1 << value))
			{
				S_CDPlay(122, 0);
				savegame.secrets |= 1 << value;
			}

			break;

		case TO_BODYBAG:
			ClearBodyBag();
			break;
		}

	} while (!(trigger & 0x8000));

	if (camera_item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA))
		camera.item = camera_item;

	if (flip != -1)
		FlipMap();

	if (neweffect != -1 && (flip || !flip_available))
	{
		flipeffect = neweffect;
		fliptimer = 0;
	}
}

long TriggerActive(ITEM_INFO* item)
{
	long reverse;

	reverse = (item->flags & IFL_REVERSE) ? 0 : 1;

	if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
		return !reverse;

	if (!item->timer)
		return reverse;

	if (item->timer == -1)
		return !reverse;

	item->timer--;

	if (!item->timer)
		item->timer = -1;

	return reverse;
}

long GetCeiling(FLOOR_INFO* floor, long x, long y, long z)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* f;
	long xoff, yoff, height, h1, h2, ch1, ch2;
	short* data;
	short type, ftype, trigger, dx, dz, t0, t1, t2, t3, hadj, ended;

	f = floor;

	while (f->sky_room != NO_ROOM)
	{
		if (CheckNoColCeilingTriangle(floor, x, z) == 1)
			break;

		r = &room[f->sky_room];
		xoff = (z - r->z) >> WALL_SHIFT;
		yoff = (x - r->x) >> WALL_SHIFT;
		f = &r->floor[xoff + r->x_size * yoff];
	}

	height = f->ceiling << 8;

	if (height == NO_HEIGHT)
		return NO_HEIGHT;

	if (f->index)
	{
		data = &floor_data[f->index];
		type = *data++;
		ftype = type & 0x1F;
		ended = 0;

		if (ftype == TILT_TYPE || ftype == SPLIT1 || ftype == SPLIT2 || ftype == NOCOLF1T || ftype == NOCOLF1B || ftype == NOCOLF2T || ftype == NOCOLF2B)
		{
			data++;

			if (type & 0x8000)
				ended = 1;

			type = *data++;
		}

		ftype = type & 0x1F;

		if (!ended)
		{
			h1 = 0;
			h2 = 0;

			if (ftype == ROOF_TYPE)
			{
				h1 = *data >> 8;
				h2 = *(char*)data;
			}
			else if (ftype == SPLIT3 || ftype == SPLIT4 || ftype == NOCOLC1T || ftype == NOCOLC1B || ftype == NOCOLC2T || ftype == NOCOLC2B)
			{
				dx = x & WALL_MASK;
				dz = z & WALL_MASK;
				t0 = -(*data & 0xF);
				t1 = -(*data >> 4 & 0xF);
				t2 = -(*data >> 8 & 0xF);
				t3 = -(*data >> 12 & 0xF);

				if (ftype == SPLIT3 || ftype == NOCOLC1T || ftype == NOCOLC1B)
				{
					if (dx <= WALL_SIZE - dz)
					{
						hadj = type >> 10 & 0x1F;

						if (hadj & 0x10)
							hadj |= 0xFFF0;

						height += hadj << 8;
						h1 = t2 - t1;
						h2 = t3 - t2;
					}
					else
					{
						hadj = type >> 5 & 0x1F;

						if (hadj & 0x10)
							hadj |= 0xFFF0;

						height += hadj << 8;
						h1 = t3 - t0;
						h2 = t0 - t1;
					}
				}
				else if (dx <= dz)
				{
					hadj = type >> 10 & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
					h1 = t2 - t1;
					h2 = t0 - t1;
				}
				else
				{
					hadj = type >> 5 & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					height += hadj << 8;
					h1 = t3 - t0;
					h2 = t3 - t2;
				}

				if (chunky_flag)
				{
					hadj = hadj >> 10 & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					ch2 = f->ceiling << 8;
					ch1 = ch2 + (hadj << 8);

					hadj = hadj >> 5 & 0x1F;

					if (hadj & 0x10)
						hadj |= 0xFFF0;

					ch2 += hadj << 8;

					if (ch1 > ch2)
						height = ch1;
					else
						height = ch2;
				}
			}

			if (!chunky_flag)
			{
				if (h1 < 0)
					height += (z & WALL_MASK) * h1 >> 2;
				else
					height -= (-1 - z & WALL_MASK) * h1 >> 2;

				if (h2 < 0)
					height += (-1 - x & WALL_MASK) * h2 >> 2;
				else
					height -= (x & WALL_MASK) * h2 >> 2;
			}
		}
	}

	while (floor->pit_room != NO_ROOM)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		xoff = (z - r->z) >> WALL_SHIFT;
		yoff = (x - r->x) >> WALL_SHIFT;
		floor = &r->floor[xoff + r->x_size * yoff];
	}

	if (floor->index)
	{
		data = &floor_data[floor->index];

		do
		{
			type = *data++;

			switch (type & 0x1F)
			{
			case DOOR_TYPE:
			case TILT_TYPE:
			case ROOF_TYPE:
			case SPLIT1:
			case SPLIT2:
			case SPLIT3:
			case SPLIT4:
			case NOCOLF1T:
			case NOCOLF1B:
			case NOCOLF2T:
			case NOCOLF2B:
			case NOCOLC1T:
			case NOCOLC1B:
			case NOCOLC2T:
			case NOCOLC2B:
				data++;
				break;

			case TRIGGER_TYPE:
				data++;

				do
				{
					trigger = *data++;

					if ((trigger & 0x3C00) != (TO_OBJECT << 10))
					{
						if ((trigger & 0x3C00) == (TO_CAMERA << 10))
							trigger = *data++;
					}
					else
					{
						item = &items[trigger & 0x3FF];

						if (objects[item->object_number].ceiling && !(item->flags & 0x8000))
							objects[item->object_number].ceiling(item, x, y, z, &height);
					}

				} while (!(trigger & 0x8000));

				break;

			case LAVA_TYPE:
			case CLIMB_TYPE:
			case MONKEY_TYPE:
			case MINEL_TYPE:
			case MINER_TYPE:
				break;

			default:
				S_ExitSystem("GetCeiling(): Unknown type");
				break;
			}
		} while (!(type & 0x8000));
	}

	return height;
}

short GetDoor(FLOOR_INFO* floor)
{
	short* data;
	short type, ftype;

	if (!floor->index)
		return NO_ROOM;

	data = &floor_data[floor->index];
	type = *data++;
	ftype = type & 0x1F;

	if (ftype == TILT_TYPE || ftype == SPLIT1 || ftype == SPLIT2 || ftype == NOCOLF1B || ftype == NOCOLF1T || ftype == NOCOLF2B || ftype == NOCOLF2T)
	{
		if (type & 0x8000)
			return NO_ROOM;

		data++;
		type = *data++;
	}

	ftype = type & 0x1F;

	if (ftype == ROOF_TYPE || ftype == SPLIT3 || ftype == SPLIT4 || ftype == NOCOLC1B || ftype == NOCOLC1T || ftype == NOCOLC2B || ftype == NOCOLC2T)
	{
		if (type & 0x8000)
			return NO_ROOM;

		data++;
		type = *data++;
	}

	ftype = type & 0x1F;

	if (ftype == DOOR_TYPE)
		return *data;

	return NO_ROOM;
}

long LOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	long los1, los2;

	target->room_number = start->room_number;

	if (abs(target->z - start->z) > abs(target->x - start->x))
	{
		los1 = xLOS(start, target);
		los2 = zLOS(start, target);
	}
	else
	{
		los1 = zLOS(start, target);
		los2 = xLOS(start, target);
	}

	if (los2)
	{
		GetFloor(target->x, target->y, target->z, &target->room_number);

		if (ClipTarget(start, target) && los1 == 1 && los2 == 1)
			return 1;
	}

	return 0;
}

long zLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	FLOOR_INFO* floor;
	long dx, dy, dz, x, y, z;
	short room_number, last_room;

	dz = target->z - start->z;

	if (!dz)
		return 1;

	dx = ((target->x - start->x) << WALL_SHIFT) / dz;
	dy = ((target->y - start->y) << WALL_SHIFT) / dz;
	number_los_rooms = 1;
	los_rooms[0] = start->room_number;
	room_number = start->room_number;
	last_room = start->room_number;

	if (dz < 0)
	{
		z = start->z & ~WALL_MASK;
		x = start->x + ((z - start->z) * dx >> WALL_SHIFT);
		y = start->y + ((z - start->z) * dy >> WALL_SHIFT);

		while (z > target->z)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			floor = GetFloor(x, y, z - 1, &room_number);

			if (y > GetHeight(floor, x, y, z - 1) || y < GetCeiling(floor, x, y, z - 1))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			z -= WALL_SIZE;
			x -= dx;
			y -= dy;
		}
	}
	else
	{
		z = start->z | WALL_MASK;
		x = start->x + ((z - start->z) * dx >> WALL_SHIFT);
		y = start->y + ((z - start->z) * dy >> WALL_SHIFT);

		while (z < target->z)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			floor = GetFloor(x, y, z + 1, &room_number);

			if (y > GetHeight(floor, x, y, z + 1) || y < GetCeiling(floor, x, y, z + 1))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			z += WALL_SIZE;
			x += dx;
			y += dy;
		}
	}

	target->room_number = room_number;
	return 1;
}

long xLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	FLOOR_INFO* floor;
	long dx, dy, dz, x, y, z;
	short room_number, last_room;

	dx = target->x - start->x;

	if (!dx)
		return 1;

	dy = ((target->y - start->y) << WALL_SHIFT) / dx;
	dz = ((target->z - start->z) << WALL_SHIFT) / dx;
	number_los_rooms = 1;
	los_rooms[0] = start->room_number;
	room_number = start->room_number;
	last_room = start->room_number;

	if (dx < 0)
	{
		x = start->x & ~WALL_MASK;
		y = start->y + ((x - start->x) * dy >> WALL_SHIFT);
		z = start->z + ((x - start->x) * dz >> WALL_SHIFT);

		while (x > target->x)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			floor = GetFloor(x - 1, y, z, &room_number);

			if (y > GetHeight(floor, x - 1, y, z) || y < GetCeiling(floor, x - 1, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			x -= WALL_SIZE;
			y -= dy;
			z -= dz;
		}
	}
	else
	{
		x = start->x | WALL_MASK;
		y = start->y + ((x - start->x) * dy >> WALL_SHIFT);
		z = start->z + ((x - start->x) * dz >> WALL_SHIFT);

		while (x < target->x)
		{
			floor = GetFloor(x, y, z, &room_number);

			if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = room_number;
				return -1;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			floor = GetFloor(x + 1, y, z, &room_number);

			if (y > GetHeight(floor, x + 1, y, z) || y < GetCeiling(floor, x + 1, y, z))
			{
				target->x = x;
				target->y = y;
				target->z = z;
				target->room_number = last_room;
				return 0;
			}

			if (room_number != last_room)
			{
				last_room = room_number;
				los_rooms[number_los_rooms] = room_number;
				number_los_rooms++;
			}

			x += WALL_SIZE;
			y += dy;
			z += dz;
		}
	}

	target->room_number = room_number;
	return 1;
}

long ClipTarget(GAME_VECTOR* start, GAME_VECTOR* target)
{
	GAME_VECTOR src;
	long dx, dy, dz;
	short room_no;

	room_no = target->room_number;

	if (target->y > GetHeight(GetFloor(target->x, target->y, target->z, &room_no), target->x, target->y, target->z))
	{
		src.x = start->x + (7 * (target->x - start->x) >> 3);
		src.y = start->y + (7 * (target->y - start->y) >> 3);
		src.z = start->z + (7 * (target->z - start->z) >> 3);

		for (int i = 3; i > 0; i--)
		{
			dx = src.x + (i * (target->x - src.x) >> 2);
			dy = src.y + (i * (target->y - src.y) >> 2);
			dz = src.z + (i * (target->z - src.z) >> 2);

			if (dy < GetHeight(GetFloor(dx, dy, dz, &room_no), dx, dy, dz))
				break;
		}

		target->x = dx;
		target->y = dy;
		target->z = dz;
		target->room_number = room_no;
		return 0;
	}

	room_no = target->room_number;

	if (target->y < GetCeiling(GetFloor(target->x, target->y, target->z, &room_no), target->x, target->y, target->z))
	{
		src.x = start->x + (7 * (target->x - start->x) >> 3);
		src.y = start->y + (7 * (target->y - start->y) >> 3);
		src.z = start->z + (7 * (target->z - start->z) >> 3);

		for (int i = 3; i > 0; i--)
		{
			dx = src.x + (i * (target->x - src.x) >> 2);
			dy = src.y + (i * (target->y - src.y) >> 2);
			dz = src.z + (i * (target->z - src.z) >> 2);

			if (dy > GetCeiling(GetFloor(dx, dy, dz, &room_no), dx, dy, dz))
				break;
		}

		target->x = dx;
		target->y = dy;
		target->z = dz;
		target->room_number = room_no;
		return 0;
	}

	return 1;
}

long ObjectOnLOS(GAME_VECTOR* start, GAME_VECTOR* target)
{
	ROOM_INFO* r;
	ITEM_INFO* item;
	short* bounds;
	short* xextent;
	short* zextent;
	long dx, dy, dz, dist, pass, x, y, z;
	short item_number, objnum;

	dx = target->x - start->x;
	dy = target->y - start->y;
	dz = target->z - start->z;

	for (int i = 0; i < number_los_rooms; i++)
	{
		r = &room[los_rooms[i]];

		for (item_number = r->item_number; item_number != NO_ITEM; item_number = item->next_item)
		{
			item = &items[item_number];

			if (item->status == ITEM_DEACTIVATED)
				continue;

			objnum = item->object_number;

			if (objnum != SMASH_WINDOW && objnum != SMASH_OBJECT1 && objnum != SMASH_OBJECT2 &&
				objnum != SMASH_OBJECT3 && objnum != CARCASS && objnum != EXTRAFX6)
				continue;

			bounds = GetBoundsAccurate(item);

			if ((item->pos.y_rot + 0x2000) & 0x4000)
			{
				xextent = &bounds[4];
				zextent = bounds;
			}
			else
			{
				xextent = bounds;
				zextent = &bounds[4];
			}

			pass = 0;

			if (abs(dz) > abs(dx))
			{
				dist = item->pos.z_pos - start->z + zextent[0];

				for (int j = 0; j < 2; j++)
				{
					if (!((dz ^ dist) & 0x80000000))
					{
						y = dy * dist / dz;

						if (y > item->pos.y_pos - start->y + bounds[2] && y < item->pos.y_pos - start->y + bounds[3])
						{
							x = dx * dist / dz;

							if (x < item->pos.x_pos + xextent[0] - start->x)
								pass |= 1;
							else if (x > item->pos.x_pos + xextent[1] - start->x)
								pass |= 2;
							else
								return item_number;
						}
					}

					dist = item->pos.z_pos - start->z + zextent[1];
				}

				if (pass == 3)
					return item_number;
			}
			else
			{
				dist = item->pos.x_pos + xextent[0] - start->x;

				for (int j = 0; j < 2; j++)
				{
					if (!((dx ^ dist) & 0x80000000))
					{
						y = dy * dist / dx;

						if (y > item->pos.y_pos - start->y + bounds[2] && y < item->pos.y_pos - start->y + bounds[3])
						{
							z = dz * dist / dx;

							if (z < item->pos.z_pos - start->z + zextent[0])
								pass |= 1;
							else if (z > item->pos.z_pos - start->z + zextent[1])
								pass |= 2;
							else
								return item_number;
						}
					}

					dist = item->pos.x_pos + xextent[1] - start->x;
				}

				if (pass == 3)
					return item_number;
			}
		}
	}

	return NO_ITEM;
}

void FlipMap()
{
	ROOM_INFO* r;
	ROOM_INFO* flipped;
	ROOM_INFO temp;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (r->flipped_room < 0)
			continue;

		flipped = &room[r->flipped_room];

		RemoveRoomFlipItems(r);
		memcpy(&temp, r, sizeof(ROOM_INFO));
		memcpy(r, flipped, sizeof(ROOM_INFO));
		memcpy(flipped, &temp, sizeof(ROOM_INFO));
		r->flipped_room = flipped->flipped_room;
		r->item_number = flipped->item_number;
		r->fx_number = flipped->fx_number;
		flipped->flipped_room = -1;
		AddRoomFlipItems(r);
	}

	flip_status = !flip_status;
}

void RemoveRoomFlipItems(ROOM_INFO* r)
{
	ITEM_INFO* item;
	short item_number;

	for (item_number = r->item_number; item_number != NO_ITEM; item_number = item->next_item)
	{
		item = &items[item_number];

		if (objects[item->object_number].control == MovableBlock)
			AlterFloorHeight(item, WALL_SIZE);
		else if (item->flags & IFL_INVISIBLE && objects[item->object_number].intelligent && item->hit_points <= 0)
		{
			RemoveDrawnItem(item_number);
			item->flags |= IFL_CLEARBODY;
		}
	}
}

void AddRoomFlipItems(ROOM_INFO* r)
{
	ITEM_INFO* item;
	short item_number;

	for (item_number = r->item_number; item_number != NO_ITEM; item_number = item->next_item)
	{
		item = &items[item_number];

		if (objects[item->object_number].control == MovableBlock)
			AlterFloorHeight(item, -WALL_SIZE);
	}
}

void TriggerCDTrack(short value, short flags, short type)
{
	if (value > 1 && value < 128)
		TriggerNormalCDTrack(value, flags, type);
}

void TriggerNormalCDTrack(short value, short flags, short type)
{
	long code;

	if (value >= 26 && value <= 36 || value >= 73 && value <= 78 || value == 117)
	{
		if (CurrentAtmosphere != value)
		{
			CurrentAtmosphere = (char)value;

			if (IsAtmospherePlaying)
				S_CDPlay(value, 1);

			printf("Got new atmosphere %d\n", value);
		}

		return;
	}

	if (value != 2 || CurrentLevel != LV_GYM)
	{
		code = (flags >> 8) & 0x3F;		//(IFL_CODEBITS | IFL_INVISIBLE) = 0x3F00, then >> 8 = 0x3F

		if ((cd_flags[value] & code) == code)
			return;

		cd_flags[value] |= code;
	}

	S_CDPlay(value, 0);
	IsAtmospherePlaying = 0;
}

long CheckNoColFloorTriangle(FLOOR_INFO* floor, long x, long z)
{
	short type;

	if (!floor->index)
		return 0;

	type = floor_data[floor->index] & 0x1F;

	if (type != NOCOLF1T && type != NOCOLF1B && type != NOCOLF2T && type != NOCOLF2B)
		return 0;

	x &= WALL_MASK;
	z &= WALL_MASK;

	if (type == NOCOLF1T && x <= WALL_SIZE - z)
		return -1;

	if (type == NOCOLF1B && x > WALL_SIZE - z)
		return -1;

	if (type == NOCOLF2T && x <= z)
		return -1;

	if (type == NOCOLF2B && x > z)
		return -1;

	return 1;
}

long CheckNoColCeilingTriangle(FLOOR_INFO* floor, long x, long z)
{
	short* data;
	short type;

	if (!floor->index)
		return 0;

	data = &floor_data[floor->index];
	type = *data & 0x1F;

	if (type == TILT_TYPE || type == SPLIT1 || type == SPLIT2 || type == NOCOLF1T || type == NOCOLF1B || type == NOCOLF2T || type == NOCOLF2B)
	{
		if (*data & 0x8000)
			return 0;

		type = data[2] & 0x1F;
	}

	if (type != NOCOLC1T && type != NOCOLC1B && type != NOCOLC2T && type != NOCOLC2B)
		return 0;

	x &= WALL_MASK;
	z &= WALL_MASK;

	if (type == NOCOLC1T && x <= WALL_SIZE - z)
		return -1;

	if (type == NOCOLC1B && x > WALL_SIZE - z)
		return -1;

	if (type == NOCOLC2T && x <= z)
		return -1;

	if (type == NOCOLC2B && x > z)
		return -1;

	return 1;
}

long IsRoomOutside(long x, long y, long z)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	uchar* p;
	long h, c, offset;
	short rn;

	offset = (ushort)OutsideRoomOffsets[27 * (x >> 12) + (z >> 12)];

	if (offset == -1)
		return -2;

	p = (uchar*)&OutsideRoomTable[offset];

	while (*p != NO_ROOM)
	{
		rn = *p;
		r = &room[rn];

		if (y > r->maxceiling && y < r->minfloor &&
			(z > r->z + WALL_SIZE && z < (r->x_size << WALL_SHIFT) + r->z - WALL_SIZE) &&
			(x > r->x + WALL_SIZE && x < (r->y_size << WALL_SHIFT) + r->x - WALL_SIZE))
		{
			floor = GetFloor(x, y, z, &rn);
			h = GetHeight(floor, x, y, z);

			if (h == NO_HEIGHT || y > h)
				return -2;

			c = GetCeiling(floor, x, y, z);

			if (y < c)
				return -2;

			if (!(r->flags & (ROOM_UNDERWATER | ROOM_NOT_INSIDE)))
				return -3;

			IsRoomOutsideNo = *p;
			return 1;
		}

		p++;
	}

	return -2;
}

long GetMaximumFloor(FLOOR_INFO* floor, long x, long z)
{
	long height, h1, h2;
	short* data;
	short type, ftype, dx, dz, t0, t1, t2, t3, hadj;

	height = floor->floor << 8;

	if (height == NO_HEIGHT || !floor->index)
		return height;

	data = &floor_data[floor->index];
	type = *data++;
	ftype = type & 0x1F;
	h1 = 0;
	h2 = 0;

	if (ftype == TILT_TYPE)
	{
		h1 = *data >> 8;
		h2 = *(char*)data;
	}
	else if (ftype == SPLIT1 || ftype == SPLIT2 || ftype == NOCOLF1T || ftype == NOCOLF1B || ftype == NOCOLF2T || ftype == NOCOLF2B)
	{
		dx = x & WALL_MASK;
		dz = z & WALL_MASK;
		t0 = *data & 0xF;
		t1 = *data >> 4 & 0xF;
		t2 = *data >> 8 & 0xF;
		t3 = *data >> 12 & 0xF;

		if (ftype == SPLIT1 || ftype == NOCOLF1T || ftype == NOCOLF1B)
		{
			if (dx <= WALL_SIZE - dz)
			{
				hadj = type >> 10 & 0x1F;
				h1 = t2 - t1;
				h2 = t0 - t1;
			}
			else
			{
				hadj = type >> 5 & 0x1F;
				h1 = t3 - t0;
				h2 = t3 - t2;
			}
		}
		else
		{
			if (dx <= dz)
			{
				hadj = type >> 10 & 0x1F;
				h1 = t2 - t1;
				h2 = t3 - t2;
			}
			else
			{
				hadj = type >> 5 & 0x1F;
				h1 = t3 - t0;
				h2 = t0 - t1;
			}
		}

		if (hadj & 0x10)
			hadj |= 0xFFF0;

		height += hadj << 8;
	}

	height += 256 * (abs(h1) + abs(h2));
	return height;
}

long GetMinimumCeiling(FLOOR_INFO* floor, long x, long z)
{
	long height, h1, h2;
	short* data;
	short type, ftype, dx, dz, t0, t1, t2, t3, hadj, ended;

	height = floor->ceiling << 8;

	if (height == NO_HEIGHT || !floor->index)
		return height;

	data = &floor_data[floor->index];
	type = *data++;
	ftype = type & 0x1F;
	ended = 0;

	if (ftype == TILT_TYPE || ftype == SPLIT1 || ftype == SPLIT2 || ftype == NOCOLF1T || ftype == NOCOLF1B || ftype == NOCOLF2T || ftype == NOCOLF2B)
	{
		data++;

		if (type & 0x8000)
			ended = 1;

		type = *data++;
	}

	if (ended)
		return height;

	ftype = type & 0x1F;
	h1 = 0;
	h2 = 0;

	if (ftype == ROOF_TYPE)
	{
		h1 = *data >> 8;
		h2 = *(char*)data;
	}
	else if (ftype == SPLIT3 || ftype == SPLIT4 || ftype == NOCOLC1T || ftype == NOCOLC1B || ftype == NOCOLC2T || ftype == NOCOLC2B)
	{
		dx = x & WALL_MASK;
		dz = z & WALL_MASK;
		t0 = -(*data & 0xF);
		t1 = -(*data >> 4 & 0xF);
		t2 = -(*data >> 8 & 0xF);
		t3 = -(*data >> 12 & 0xF);

		if (ftype == SPLIT3 || ftype == NOCOLC1T || ftype == NOCOLC1B)
		{
			if (dx <= WALL_SIZE - dz)
			{
				hadj = type >> 10 & 0x1F;
				h1 = t2 - t1;
				h2 = t3 - t2;
			}
			else
			{
				hadj = type >> 5 & 0x1F;
				h1 = t3 - t0;
				h2 = t0 - t1;
			}
		}
		else
		{
			if (dx <= dz)
			{
				hadj = type >> 10 & 0x1F;
				h1 = t2 - t1;
				h2 = t0 - t1;
			}
			else
			{
				hadj = type >> 5 & 0x1F;
				h1 = t3 - t0;
				h2 = t3 - t2;
			}
		}

		if (hadj & 0x10)
			hadj |= 0xFFF0;

		height += hadj << 8;
	}

	height -= 256 * (abs(h1) + abs(h2));
	return height;
}
