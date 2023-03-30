#include "../tomb3/pch.h"
#include "tiger.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"
#include "effects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO tiger_bite = { 19, -13, 3, 26 };

void TigerControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* tiger;
	AI_INFO info;
	short angle, head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	tiger = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != TIGER_DEATH)
		{
			item->anim_number = objects[TIGER].anim_index + 11;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = TIGER_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		GetCreatureMood(item, &info, 1);

		if (tiger->alerted && info.zone_number != info.enemy_zone)
			tiger->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, tiger->maximum_turn);

		switch (item->current_anim_state)
		{
		case TIGER_STOP:
			tiger->maximum_turn = 0;
			tiger->flags = 0;

			if (tiger->mood == ESCAPE_MOOD)
			{
				if (lara.target == item || !info.ahead)
					item->goal_anim_state = TIGER_RUN;
				else
					item->goal_anim_state = TIGER_STOP;
			}
			else if (tiger->mood == BORED_MOOD)
			{
				if ((short)GetRandomControl() < 96)
					item->goal_anim_state = TIGER_ROAR;

				item->goal_anim_state = TIGER_WALK;
			}
			else if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = TIGER_ATTACK1;
			else if (info.bite && info.distance < 0x100000)
			{
				tiger->maximum_turn = 546;
				item->goal_anim_state = TIGER_ATTACK3;
			}
			else if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (tiger->mood != ATTACK_MOOD && GetRandomControl() < 96)
				item->goal_anim_state = TIGER_ROAR;
			else
				item->goal_anim_state = TIGER_RUN;

			break;

		case TIGER_WALK:
			tiger->maximum_turn = 546;

			if (tiger->mood == ESCAPE_MOOD || tiger->mood == ATTACK_MOOD)
				item->goal_anim_state = TIGER_RUN;
			else if (GetRandomControl() < 96)
			{
				item->goal_anim_state = TIGER_STOP;
				item->required_anim_state = TIGER_ROAR;
			}

			break;

		case TIGER_RUN:
			tiger->maximum_turn = 1092;

			if (tiger->mood == BORED_MOOD || tiger->flags && info.ahead)
				item->goal_anim_state = TIGER_STOP;
			else if (info.bite && info.distance < 0x240000)
			{
				if (lara_item->speed)
					item->goal_anim_state = TIGER_ATTACK2;
				else
					item->goal_anim_state = TIGER_STOP;
			}
			else if (tiger->mood != ATTACK_MOOD && GetRandomControl() < 96)
			{
				item->goal_anim_state = TIGER_STOP;
				item->required_anim_state = TIGER_ROAR;
			}
			else if (tiger->mood == ESCAPE_MOOD && lara.target != item && info.ahead)
				item->goal_anim_state = TIGER_STOP;

			tiger->flags = 0;
			break;

		case TIGER_ATTACK1:
		case TIGER_ATTACK2:
		case TIGER_ATTACK3:

			if (!tiger->flags && (item->touch_bits & 0x7FDC000) != 0)
			{
				lara_item->hit_status = 1;
				lara_item->hit_points -= 90;
				CreatureEffect(item, &tiger_bite, DoBloodSplat);
				tiger->flags = 1;
			}

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, head);
	CreatureAnimation(item_number, angle, 0);
}
