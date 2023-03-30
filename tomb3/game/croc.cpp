#include "../tomb3/pch.h"
#include "croc.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO croc_bite = { 5, -21, 467, 9 };

void CrocControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* croc;
	AI_INFO info;
	short head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	croc = (CREATURE_INFO*)item->data;
	head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != CROC_DEATH)
		{
			item->anim_number = objects[CROCODILE].anim_index + 4;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CROC_DEATH;
			item->hit_points = DONT_TARGET;
		}

		CreatureFloat(item_number);
		return;
	}

	CreatureAIInfo(item, &info);

	if (info.ahead)
		head = info.angle;

	GetCreatureMood(item, &info, 1);

	if (!(room[lara_item->room_number].flags & ROOM_UNDERWATER) && lara.skidoo == NO_ITEM)
		croc->mood = BORED_MOOD;

	CreatureMood(item, &info, 1);
	CreatureTurn(item, 546);

	switch (item->current_anim_state)
	{
	case CROC_SWIM:

		if (info.bite && item->touch_bits)
			item->goal_anim_state = CROC_ATTACK;

		break;

	case CROC_ATTACK:

		if (item->frame_number == anims[item->anim_number].frame_base)
			item->required_anim_state = 0;

		if (info.bite && item->touch_bits)
		{
			if (!item->required_anim_state)
			{
				CreatureEffect(item, &croc_bite, DoBloodSplat);
				lara_item->hit_points -= 100;
				lara_item->hit_status = 1;
				item->required_anim_state = 1;
			}
		}
		else
			item->goal_anim_state = CROC_SWIM;

		break;
	}

	CreatureJoint(item, 0, head);
	CreatureUnderwater(item, 256);
	CreatureAnimation(item_number, 0, 0);
}
