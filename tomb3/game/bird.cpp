#include "../tomb3/pch.h"
#include "bird.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO bird_bite = { 15, 46, 21, 6 };
static BITE_INFO crow_bite = { 2, 10, 60, 14 };

void InitialiseVulture(short item_number)
{
	ITEM_INFO* item;

	InitialiseCreature(item_number);
	item = &items[item_number];

	if (item->object_number == CROW)
	{
		item->anim_number = objects[CROW].anim_index + 14;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = BIRD_EAT;
		item->goal_anim_state = BIRD_EAT;
	}
	else
	{
		item->anim_number = objects[VULTURE].anim_index + 5;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = BIRD_STOP;
		item->goal_anim_state = BIRD_STOP;
	}
}

void VultureControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* bird;
	AI_INFO info;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	bird = (CREATURE_INFO*)item->data;
	angle = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state == BIRD_FALL)
		{
			if (item->pos.y_pos > item->floor)
			{
				item->gravity_status = 0;
				item->pos.y_pos = item->floor;
				item->fallspeed = 0;
				item->goal_anim_state = BIRD_DEATH;
			}
		}
		else if (item->current_anim_state == BIRD_DEATH)
			item->pos.y_pos = item->floor;
		else
		{
			if (item->object_number == CROW)
				item->anim_number = objects[CROW].anim_index + 1;
			else
				item->anim_number = objects[VULTURE].anim_index + 8;

			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BIRD_FALL;
			item->gravity_status = 1;
			item->speed = 0;
		}

		item->pos.x_rot = 0;
	}
	else
	{
		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 0);
		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, 546);

		switch (item->current_anim_state)
		{
		case BIRD_FLY:
			bird->flags = 0;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;

			if (bird->mood == BORED_MOOD)
				item->goal_anim_state = BIRD_STOP;
			else if (info.ahead && info.distance < 0x40000)
				item->goal_anim_state = BIRD_ATTACK;
			else
				item->goal_anim_state = BIRD_GLIDE;

			break;

		case BIRD_STOP:
		case BIRD_EAT:
			item->pos.y_pos = item->floor;

			if (bird->mood != BORED_MOOD)
				item->goal_anim_state = BIRD_FLY;

			break;

		case BIRD_GLIDE:

			if (bird->mood == BORED_MOOD)
			{
				item->required_anim_state = BIRD_STOP;
				item->goal_anim_state = BIRD_FLY;
			}
			else if (info.ahead && info.distance < 0x40000)
				item->goal_anim_state = BIRD_ATTACK;

			break;

		case BIRD_ATTACK:

			if (!bird->flags && item->touch_bits)
			{
				lara_item->hit_points -= 20;
				lara_item->hit_status = 1;

				if (item->object_number == CROW)
					CreatureEffect(item, &crow_bite, DoBloodSplat);
				else
					CreatureEffect(item, &bird_bite, DoBloodSplat);

				bird->flags = 1;
			}

			break;
		}
	}

	CreatureAnimation(item_number, angle, 0);
}
