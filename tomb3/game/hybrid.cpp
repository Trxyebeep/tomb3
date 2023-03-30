#include "../tomb3/pch.h"
#include "hybrid.h"
#include "box.h"
#include "../3dsystem/phd_math.h"
#include "people.h"
#include "effects.h"
#include "objects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO hybrid_bite_left = { 19, -13, 3, 7 };
static BITE_INFO hybrid_bite_right = { 19, -13, 3, 14 };

void HybridControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* hybrid;
	AI_INFO info;
	AI_INFO larainfo;
	long x, z;
	short angle, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	hybrid = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != HYBRID_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 18;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = HYBRID_DEATH;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(hybrid);

		CreatureAIInfo(item, &info);

		if (hybrid->enemy == lara_item)
		{
			larainfo.angle = info.angle;
			larainfo.distance = info.distance;
		}
		else
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			larainfo.angle = short(phd_atan(z, x) - item->pos.y_rot);
			larainfo.distance = SQUARE(x) + SQUARE(z);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, hybrid->maximum_turn);

		enemy = lara_item;
		hybrid->enemy = lara_item;

		if (larainfo.distance < 0x100000 || item->hit_status || TargetVisible(item, &larainfo))
			AlertAllGuards(item_number);

		hybrid->enemy = enemy;

		switch (item->current_anim_state)
		{
		case HYBRID_STOP:
			hybrid->maximum_turn = 0;
			hybrid->flags = 0;
			head = info.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(hybrid);
				item->goal_anim_state = HYBRID_STOP;
			}
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = HYBRID_WALK;
				head = 0;
			}
			else if (hybrid->mood == ESCAPE_MOOD)
			{
				if (lara.target == item || !info.ahead)
					item->goal_anim_state = HYBRID_RUN;
				else
					item->goal_anim_state = HYBRID_STOP;
			}
			else if (info.angle < 0x2000 && info.angle > -0x2000 && info.distance > 0x100000)
				item->goal_anim_state = HYBRID_JUMP_STR;
			else if (info.bite && info.distance < 0x100000)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
				
				if (info.angle < 0)
					item->goal_anim_state = HYBRID_SLASH_LEFT;
				else
					item->goal_anim_state = HYBRID_KICK;
			}
			else if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (info.distance < 0x400000)
				item->goal_anim_state = HYBRID_WALK;
			else
				item->goal_anim_state = HYBRID_RUN;

			break;

		case HYBRID_WALK:
			hybrid->maximum_turn = 546;

			if (info.ahead)
				head = info.angle;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = HYBRID_WALK;
				head = 0;
			}
			else if (info.bite && info.distance < 0x1C6E39)
			{
				hybrid->maximum_turn = 546;
				item->goal_anim_state = HYBRID_WALK_ATAK;
			}
			else if (hybrid->mood == ESCAPE_MOOD || hybrid->mood == ATTACK_MOOD)
				item->goal_anim_state = HYBRID_RUN;

			break;

		case HYBRID_RUN:
			hybrid->maximum_turn = 1092;

			if (info.ahead)
				head = info.angle;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = HYBRID_STOP;
			else if (hybrid->mood == BORED_MOOD || hybrid->mood == ESCAPE_MOOD && lara.target != item && info.ahead)
				item->goal_anim_state = HYBRID_STOP;
			else if (hybrid->flags && info.ahead)
				item->goal_anim_state = HYBRID_STOP;
			else if (info.bite && info.distance < 0x400000)
			{
				if (lara_item->speed)
					item->goal_anim_state = HYBRID_RUN_ATAK;
				else
					item->goal_anim_state = HYBRID_STOP;
			}
			else if (info.distance < 0x400000)
				item->goal_anim_state = HYBRID_WALK;

			hybrid->flags = 0;
			break;

		case HYBRID_JUMP_STR:
			hybrid->maximum_turn = 546;
			break;

		case HYBRID_JUMP_MID:
		case HYBRID_JUMP_END:
			hybrid->maximum_turn = 0;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->touch_bits & 0x80)
			{
				lara_item->hit_points -= 20;
				lara_item->hit_status = 1;
				CreatureEffect(item, &hybrid_bite_left, DoBloodSplat);
			}

			break;

		case HYBRID_SLASH_LEFT:
		case HYBRID_RUN_ATAK:
		case HYBRID_WALK_ATAK:
			hybrid->maximum_turn = 546;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (!hybrid->flags && item->touch_bits & 0x80)
			{
				lara_item->hit_points -= 100;
				lara_item->hit_status = 1;
				CreatureEffect(item, &hybrid_bite_left, DoBloodSplat);
				hybrid->flags = 1;
			}

			if (!info.bite || info.distance >= 0x100000)
				item->goal_anim_state = HYBRID_STOP;

			if (item->frame_number == anims[item->anim_number].frame_end)
				hybrid->flags = 0;

			break;

		case HYBRID_KICK:
			hybrid->maximum_turn = 546;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (!hybrid->flags && item->touch_bits & 0x800)
			{
				lara_item->hit_points -= 80;
				lara_item->hit_status = 1;
				CreatureEffect(item, &hybrid_bite_right, DoBloodSplat);
				hybrid->flags = 1;
			}

			if (!info.bite || info.distance >= 0x100000)
				item->goal_anim_state = HYBRID_STOP;

			if (item->frame_number == anims[item->anim_number].frame_end)
				hybrid->flags = 0;

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_x);
	CreatureJoint(item, 1, torso_y);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
