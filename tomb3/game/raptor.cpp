#include "../tomb3/pch.h"
#include "raptor.h"
#include "lot.h"
#include "../specific/game.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO raptor_bite = { 0, 66, 318, 22 };

void RaptorControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	ITEM_INFO* candidate;
	CREATURE_INFO* raptor;
	AI_INFO info;
	long x, y, z, dist, best_dist, lp;
	short tilt, angle, neck, head;

	item = &items[item_number];
	enemy = 0;
	best_dist = 0x7FFFFFFF;

	if (item->status == ITEM_INVISIBLE)
	{
		if (!EnableBaddieAI(item_number, 0))
			return;

		item->status = ITEM_ACTIVE;
	}

	raptor = (CREATURE_INFO*)item->data;
	tilt = 0;
	angle = 0;
	neck = 0;
	head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != RAPTOR_DEATH)
		{
			if (GetRandomControl() > 0x4000)
				item->anim_number = objects[item->object_number].anim_index + 9;
			else
				item->anim_number = objects[item->object_number].anim_index + 10;

			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = RAPTOR_DEATH;
		}
	}
	else
	{
		if (!raptor->enemy || !(GetRandomControl() & 0x7F))
		{
			for (lp = 0; lp < MAX_LOT; lp++)
			{
				if (baddie_slots[lp].item_num == NO_ITEM || baddie_slots[lp].item_num == item_number)
					continue;

				candidate = &items[baddie_slots[lp].item_num];
				x = (candidate->pos.x_pos - item->pos.x_pos) >> 6;
				y = (candidate->pos.y_pos - item->pos.y_pos) >> 6;
				z = (candidate->pos.z_pos - item->pos.z_pos) >> 6;
				dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

				if (dist < best_dist && item->hit_points > 0)
				{
					enemy = candidate;
					best_dist = dist;
				}
			}

			if (enemy && (enemy->object_number != RAPTOR || GetRandomControl() < 0x400 && best_dist < 0x400000))
				raptor->enemy = enemy;

			//Favor Lara if she's closer
			x = (lara_item->pos.x_pos - item->pos.x_pos) >> 6;
			y = (lara_item->pos.y_pos - item->pos.y_pos) >> 6;
			z = (lara_item->pos.z_pos - item->pos.z_pos) >> 6;
			dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

			if (dist < best_dist)
				raptor->enemy = lara_item;
		}

		if (item->ai_bits)
			GetAITarget(raptor);

		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (raptor->mood == BORED_MOOD)
			raptor->maximum_turn >>= 1;

		angle = CreatureTurn(item, raptor->maximum_turn);
		neck = -6 * angle;

		switch (item->current_anim_state)
		{
		case RAPTOR_STOP:
			raptor->flags &= ~1;
			raptor->maximum_turn = 0;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (raptor->flags & 2)
			{
				raptor->flags &= ~2;
				item->goal_anim_state = RAPTOR_WARNING;
			}
			else if (item->touch_bits & 0xFF7C00 || info.distance < 0x538D1 && info.bite)
				item->goal_anim_state = RAPTOR_ATTACK3;
			else if (info.bite && info.distance < 0x240000)
				item->goal_anim_state = RAPTOR_ATTACK1;
			else if (raptor->mood == ESCAPE_MOOD && lara.target != item && info.ahead && !item->hit_status)
				item->goal_anim_state = RAPTOR_STOP;
			else if (raptor->mood == BORED_MOOD)
				item->goal_anim_state = RAPTOR_WALK;
			else
				item->goal_anim_state = RAPTOR_RUN;

			break;

		case RAPTOR_WALK:
			raptor->flags &= ~1;
			raptor->maximum_turn = 364;

			if (raptor->mood != BORED_MOOD)
				item->goal_anim_state = RAPTOR_STOP;
			else if (info.ahead && GetRandomControl() < 128)
			{
				item->goal_anim_state = RAPTOR_STOP;
				item->required_anim_state = RAPTOR_WARNING;
				raptor->flags &= ~2;
			}

			break;

		case RAPTOR_RUN:
			raptor->flags &= ~1;
			raptor->maximum_turn = 728;
			tilt = angle;

			if (item->touch_bits & 0xFF7C00)
				item->goal_anim_state = RAPTOR_STOP;
			else if (raptor->flags & 2)
			{
				item->goal_anim_state = RAPTOR_STOP;
				item->required_anim_state = RAPTOR_WARNING;
				raptor->flags &= ~2;
			}
			else if (info.bite && info.distance < 0x240000)
			{
				if (item->goal_anim_state == RAPTOR_RUN)
					item->goal_anim_state = GetRandomControl() < 0x2000 ? RAPTOR_STOP : RAPTOR_ATTACK2;
			}
			else if (info.ahead && raptor->mood != ESCAPE_MOOD && GetRandomControl() < 128 && raptor->enemy->object_number != ANIMATING6)
			{
				item->goal_anim_state = RAPTOR_STOP;
				item->required_anim_state = RAPTOR_WARNING;
			}
			else if (raptor->mood == BORED_MOOD || raptor->mood == ESCAPE_MOOD && lara.target != item && info.ahead)
				item->goal_anim_state = RAPTOR_STOP;

			break;

		case RAPTOR_ATTACK1:
		case RAPTOR_ATTACK3:
			raptor->maximum_turn = 364;
			tilt = angle;
			enemy = raptor->enemy;

			if (enemy == lara_item)
			{
				if (!(raptor->flags & 1) && item->touch_bits & 0xFF7C00)
				{
					raptor->flags |= 1;
					CreatureEffect(item, &raptor_bite, DoBloodSplat);

					if (lara_item->hit_points <= 0)
						raptor->flags |= 2;

					lara_item->hit_points -= 100;
					lara_item->hit_status = 1;
					item->required_anim_state = RAPTOR_STOP;
				}
			}
			else if (!(raptor->flags & 1) && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 512 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) < 512 &&
					abs(enemy->pos.z_pos - item->pos.z_pos) < 512)
				{
					enemy->hit_points -= 25;
					enemy->hit_status = 1;

					if (enemy->hit_status <= 0)
						raptor->flags |= 2;

					raptor->flags |= 1;
					CreatureEffect(item, &raptor_bite, DoBloodSplat);
				}
			}

			break;

		case RAPTOR_ATTACK2:
			raptor->maximum_turn = 364;
			tilt = angle;
			enemy = raptor->enemy;

			if (enemy == lara_item)
			{
				if (!(raptor->flags & 1) && item->touch_bits & 0xFF7C00)
				{
					raptor->flags |= 1;
					CreatureEffect(item, &raptor_bite, DoBloodSplat);
					lara_item->hit_points -= 100;
					lara_item->hit_status = 1;

					if (lara_item->hit_points <= 0)
						raptor->flags |= 2;

					item->required_anim_state = RAPTOR_RUN;
				}
			}
			else if (!(raptor->flags & 1) && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 512 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) < 512 &&
					abs(enemy->pos.z_pos - item->pos.z_pos) < 512)
				{
					enemy->hit_points -= 25;
					enemy->hit_status = 1;

					if (enemy->hit_status <= 0)
						raptor->flags |= 2;

					raptor->flags |= 1;
					CreatureEffect(item, &raptor_bite, DoBloodSplat);
				}
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, head >> 1);
	CreatureJoint(item, 1, head >> 1);
	CreatureJoint(item, 2, neck);
	CreatureJoint(item, 3, neck);
	CreatureAnimation(item_number, angle, tilt);
}
