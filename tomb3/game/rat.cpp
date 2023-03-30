#include "../tomb3/pch.h"
#include "rat.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "../specific/game.h"
#include "control.h"
#include "lara.h"

static BITE_INFO mouse_bite = { 0, 0, 57, 2 };

void MouseControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* rat;
	AI_INFO info;
	long rnd;
	short angle, head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	rat = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != MOUSE_DEATH)
		{
			item->anim_number = objects[SMALL_RAT].anim_index + 9;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MOUSE_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		GetCreatureMood(item, &info, 0);
		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, 1092);

		switch (item->current_anim_state)
		{
		case MOUSE_RUN:
			rat->maximum_turn = 1092;

			if (rat->mood == BORED_MOOD || rat->mood == STALK_MOOD)
			{
				rnd = (short)GetRandomControl();

				if (rnd < 0x500)
				{
					item->required_anim_state = MOUSE_WAIT1;
					item->goal_anim_state = MOUSE_STOP;
				}
				else if (rnd < 0xA00)
					item->goal_anim_state = MOUSE_STOP;
			}
			else if (info.ahead && info.distance < 0x1C639)
				item->goal_anim_state = MOUSE_STOP;

			break;

		case MOUSE_STOP:
			rat->maximum_turn = 0;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;

			break;

		case MOUSE_WAIT1:

			if (GetRandomControl() < 0x500)
				item->goal_anim_state = MOUSE_STOP;

			break;

		case MOUSE_WAIT2:

			if (rat->mood == BORED_MOOD || rat->mood == STALK_MOOD)
			{
				rnd = (short)GetRandomControl();

				if (rnd < 0x500)
					item->required_anim_state = MOUSE_WAIT1;
				else if (rnd > 0xA00)
					item->required_anim_state = MOUSE_RUN;
			}
			else if (info.distance < 0x1C639)
				item->required_anim_state = MOUSE_ATTACK;
			else
				item->required_anim_state = MOUSE_RUN;

			if (item->required_anim_state)
				item->goal_anim_state = MOUSE_STOP;

			break;

		case MOUSE_ATTACK:

			if (item->required_anim_state == MOUSE_EMPTY && item->touch_bits & 0x7F)
			{
				CreatureEffect(item, &mouse_bite, DoBloodSplat);
				lara_item->hit_points -= 20;
				lara_item->hit_status = 1;
				item->required_anim_state = MOUSE_STOP;
			}

			break;
		}
	}

	CreatureJoint(item, 0, head);
	CreatureAnimation(item_number, angle, 0);
}
