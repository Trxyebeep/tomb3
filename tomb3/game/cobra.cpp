#include "../tomb3/pch.h"
#include "cobra.h"
#include "box.h"
#include "gameflow.h"
#include "effects.h"
#include "objects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO cobra_hit = { 0, 0, 0, 13 };

void InitialiseCobra(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base + 45;
	item->current_anim_state = 3;
	item->goal_anim_state = 3;
	item->item_flags[2] = item->hit_points;
	item->hit_points = DONT_TARGET;
}

void CobraControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* snake;
	AI_INFO info;
	static long forget_radius, alert_radius, attack_radius;
	short angle;

	if (!CreatureActive(item_number))
		return;

	if (CurrentLevel < LV_ROOFTOPS)
	{
		forget_radius = 0x900000;
		alert_radius = 0x240000;
		attack_radius = 0x100000;
	}
	else
	{
		forget_radius = 0x640000;
		alert_radius = 0x190000;
		attack_radius = 0x718E4;
	}

	item = &items[item_number];
	snake = (CREATURE_INFO*)item->data;
	angle = 0;

	if (!snake)
		return;

	if (item->hit_points <= 0 && item->hit_points != DONT_TARGET)
	{
		if (item->current_anim_state != 4)
		{
			item->anim_number = objects[item->object_number].anim_index + 4;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 4;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);
		info.angle += 3072;
		snake->target.x = lara_item->pos.x_pos;
		snake->target.z = lara_item->pos.z_pos;
		angle = CreatureTurn(item, snake->maximum_turn);

		if (abs(info.angle) < 1820)
			item->pos.y_rot += info.angle;
		else if (info.angle < 0)
			item->pos.y_rot -= 1820;
		else
			item->pos.y_rot += 1820;

		switch (item->current_anim_state)
		{
		case 0:
			item->hit_points = item->item_flags[2];
			break;

		case 1:
			snake->flags = 0;

			if (info.distance > forget_radius)
				item->goal_anim_state = 3;
			else if (lara_item->hit_points > 0 && (info.ahead && info.distance < attack_radius || item->hit_status || lara_item->speed > 15))
				item->goal_anim_state = 2;

			break;

		case 2:

			if (snake->flags != 1 && (item->touch_bits & 0x2000) != 0)
			{
				snake->flags = 1;
				lara_item->hit_points -= 80;
				lara_item->hit_status = 1;
				lara.poisoned = 256;
				CreatureEffect(item, &cobra_hit, DoBloodSplat);
			}

			break;

		case 3:
			snake->flags = 0;

			if (item->hit_points != DONT_TARGET)
			{
				item->item_flags[2] = item->hit_points;
				item->hit_points = DONT_TARGET;
			}

			if (info.distance < alert_radius && lara_item->hit_points > 0)
			{
				item->goal_anim_state = 0;
				item->hit_points = item->item_flags[2];
			}

			break;
		}
	}

	CreatureAnimation(item_number, angle, 0);
}
