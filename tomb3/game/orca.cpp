#include "../tomb3/pch.h"
#include "orca.h"
#include "box.h"
#include "sphere.h"
#include "control.h"
#include "../specific/game.h"
#include "effect2.h"
#include "lara.h"

void OrcaControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* orca;
	AI_INFO info;
	PHD_VECTOR pos;
	long h;
	short angle, room_number;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	orca = (CREATURE_INFO*)item->data;
	item->hit_points = DONT_TARGET;
	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 1);

	if (!(room[lara_item->room_number].flags & ROOM_UNDERWATER) && lara.skidoo == NO_ITEM)
		orca->mood = BORED_MOOD;

	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, orca->maximum_turn);

	switch (item->current_anim_state)
	{
	case ORCA_SLOW:
		orca->flags = 0;
		orca->maximum_turn = 364;

		if (orca->mood == BORED_MOOD)
		{
			if (GetRandomControl() & 0xFF)
				item->goal_anim_state = ORCA_SLOW;
			else
				item->goal_anim_state = ORCA_JUMP;
		}
		else if (info.ahead && info.distance < 0x90000)
			item->goal_anim_state = ORCA_SLOW_BUTT;
		else if (orca->mood == ESCAPE_MOOD)
			item->goal_anim_state = ORCA_FAST;
		else if (info.distance > 0x240000)
		{
			if (info.angle >= 0x5000 || info.angle <= -0x5000)
				item->goal_anim_state = ORCA_ROLL180;
			else if (GetRandomControl() & 0x3F)
				item->goal_anim_state = ORCA_FAST;
			else
				item->goal_anim_state = ORCA_BREACH;
		}

		break;

	case ORCA_FAST:
		orca->flags = 0;
		orca->maximum_turn = 364;

		if (orca->mood == BORED_MOOD)
		{
			if (GetRandomControl() & 0xFF)
				item->goal_anim_state = ORCA_SLOW;
			else
				item->goal_anim_state = ORCA_JUMP;
		}
		else if (orca->mood != ESCAPE_MOOD)
		{
			if (info.ahead && info.distance < 0x240000 && info.zone_number == info.enemy_zone)
				item->goal_anim_state = ORCA_SLOW;
			else if (info.distance > 0x240000 && !(GetRandomControl() & 0x7F))
				item->goal_anim_state = ORCA_JUMP;
			else if (info.distance > 0x240000 && !info.ahead)
				item->goal_anim_state = ORCA_SLOW;
		}

		break;

	case ORCA_ROLL180:
		orca->maximum_turn = 0;

		if (item->frame_number == anims[item->anim_number].frame_base + 59)
		{
			item->pos.x_rot = -item->pos.x_rot;
			item->pos.y_rot += 0x8000;
		}

		break;
	}

	CreatureAnimation(item_number, angle, 0);
	CreatureUnderwater(item, 204);

	if (wibble & 4)
	{
		pos.x = -32;
		pos.y = 16;
		pos.z = -300;
		GetJointAbsPosition(item, &pos, 5);
		room_number = item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_number);
		h = GetWaterHeight(pos.x, pos.y, pos.z, room_number);

		if (h != NO_HEIGHT && pos.y < h)
			SetupRipple(pos.x, h, pos.z, -2 - (GetRandomControl() & 1), 0)->init = 0;
	}
}
