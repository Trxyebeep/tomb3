#include "../tomb3/pch.h"
#include "prisoner.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "../specific/game.h"
#include "../3dsystem/phd_math.h"
#include "sound.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

static BITE_INFO bob_hit = { 10, 10, 11, 13 };

void InitialisePrisoner(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[BOB].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = BOB_STOP;
	item->goal_anim_state = BOB_STOP;
}

void PrisonerControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* bob;
	AI_INFO info;
	long dx, dz, dist, best_dist, iDist, lp;
	short tilt, angle, head, torso_x, torso_y, iAngle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	bob = (CREATURE_INFO*)item->data;
	tilt = 0;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (boxes[item->box_number].overlap_index & 0x4000)
	{
		DoLotsOfBloodD(item->pos.x_pos, item->pos.y_pos - (GetRandomControl() & 0xFF) - 32, item->pos.z_pos,
			(GetRandomControl() & 0x7F) + 128, short(GetRandomControl() << 1), item->room_number, 3);
		item->hit_points -= 20;
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != BOB_DEATH)
		{
			item->anim_number = objects[BOB].anim_index + 26;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BOB_DEATH;
			bob->LOT.step = 256;
		}
	}
	else
	{
		if (item->ai_bits && item->ai_bits != MODIFY)
			GetAITarget(bob);
		else if (bob->hurt_by_lara)
			bob->enemy = lara_item;
		else
		{
			bob->enemy = 0;
			best_dist = 0x7FFFFFFF;

			for (lp = 0; lp < MAX_LOT; lp++)
			{
				if (baddie_slots[lp].item_num == NO_ITEM || baddie_slots[lp].item_num == item_number)
					continue;

				enemy = &items[baddie_slots[lp].item_num];

				if (enemy->object_number == LARA || enemy->object_number == BOB || enemy->object_number == ROBOT_SENTRY_GUN || enemy->hit_points <= 0)
					continue;

				dx = enemy->pos.x_pos - item->pos.x_pos;
				dz = enemy->pos.z_pos - item->pos.z_pos;

				if (dx > 0x7D00 || dx < -0x7D00 || dz > 0x7D00 || dz < -0x7D00)
					continue;

				dist = SQUARE(dx) + SQUARE(dz);

				if (dist < best_dist)
				{
					bob->enemy = enemy;
					best_dist = dist;
				}
			}
		}

		if (item->ai_bits == MODIFY)
			item->hit_points = 200;

		CreatureAIInfo(item, &info);

		if (!bob->hurt_by_lara && bob->enemy == lara_item)
			bob->enemy = 0;

		if (bob->enemy == lara_item)
		{
			iDist = info.distance;
			iAngle = info.angle;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			iAngle = short(phd_atan(dz, dx) - item->pos.y_rot);
			iDist = SQUARE(dx) + SQUARE(dz);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, bob->maximum_turn);

		if (bob->hurt_by_lara)
		{
			if (!bob->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		enemy = bob->enemy;

		switch (item->current_anim_state)
		{
		case BOB_WAIT:

			if (bob->alerted || item->current_anim_state == BOB_RUN)
			{
				item->goal_anim_state = BOB_STOP;
				break;
			}

		case BOB_STOP:
			head = iAngle;
			bob->flags = 0;
			bob->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(bob);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == BOB_STOP)
						item->goal_anim_state = BOB_WAIT;
					else
						item->goal_anim_state = BOB_STOP;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = BOB_WALK;
			else if (bob->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = BOB_STOP;
				else
					item->goal_anim_state = BOB_RUN;
			}
			else if (bob->mood == BORED_MOOD || item->ai_bits & FOLLOW && (bob->reached_goal || iDist > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = BOB_STOP;
				else
					item->goal_anim_state = BOB_RUN;
			}
			else if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = BOB_AIM0;
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = BOB_AIM1;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = BOB_WALK;
			else
				item->goal_anim_state = BOB_RUN;

			break;

		case BOB_WALK:
			head = iAngle;
			bob->maximum_turn = 1274;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = BOB_WALK;
				head = 0;
			}
			else if (bob->mood == ESCAPE_MOOD)
				item->goal_anim_state = BOB_RUN;
			else if (bob->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
				{
					item->required_anim_state = BOB_WAIT;
					item->goal_anim_state = BOB_STOP;
				}
			}
			else if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = BOB_STOP;
			else if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = BOB_AIM2;
			else
				item->goal_anim_state = BOB_RUN;

			break;

		case BOB_PUNCH2:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			bob->maximum_turn = 1274;

			if (enemy == lara_item)
			{
				if (bob->flags != 2 && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 50;
					lara_item->hit_status = 1;
					CreatureEffect(item, &bob_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					bob->flags = 2;
				}
			}
			else if (bob->flags != 2 && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 256 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) <= 256 &&
					abs(enemy->pos.z_pos - item->pos.z_pos) < 256)
				{
					enemy->hit_points -= 25;
					enemy->hit_status = 1;
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					bob->flags = 2;
					CreatureEffect(item, &bob_hit, DoBloodSplat);
				}
			}

			break;

		case BOB_AIM2:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			bob->maximum_turn = 1274;
			bob->flags = 0;

			if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = BOB_PUNCH2;
			else
				item->goal_anim_state = BOB_WALK;

			break;

		case BOB_AIM1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			bob->maximum_turn = 1274;
			bob->flags = 0;

			if (info.ahead && info.distance < 0x718E4)
				item->goal_anim_state = BOB_PUNCH1;
			else
				item->goal_anim_state = BOB_STOP;

			break;

		case BOB_AIM0:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			bob->maximum_turn = 1274;
			bob->flags = 0;

			if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = BOB_PUNCH0;
			else
				item->goal_anim_state = BOB_STOP;

			break;

		case BOB_PUNCH1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			bob->maximum_turn = 1274;

			if (enemy == lara_item)
			{
				if (!bob->flags && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 40;
					lara_item->hit_status = 1;
					CreatureEffect(item, &bob_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					bob->flags = 1;
				}
			}
			else if (!bob->flags && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 256 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) <= 256 &&
					abs(enemy->pos.z_pos - item->pos.z_pos) < 256)
				{
					enemy->hit_points -= 20;
					enemy->hit_status = 1;
					bob->flags = 1;
					CreatureEffect(item, &bob_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}

			if (info.ahead && info.distance > 0x718E4 && info.distance < 0x90000)
				item->goal_anim_state = BOB_PUNCH2;

			break;

		case BOB_PUNCH0:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			bob->maximum_turn = 1274;

			if (enemy == lara_item)
			{
				if (!bob->flags && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 40;
					lara_item->hit_status = 1;
					CreatureEffect(item, &bob_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					bob->flags = 1;
				}
			}
			else if (!bob->flags && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 256 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) <= 256 &&
					abs(enemy->pos.z_pos - item->pos.z_pos) < 256)
				{
					enemy->hit_points -= 20;
					enemy->hit_status = 1;
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					bob->flags = 1;
					CreatureEffect(item, &bob_hit, DoBloodSplat);
				}
			}

			break;

		case BOB_RUN:

			if (info.ahead)
				head = info.angle;

			bob->maximum_turn = 2002;
			tilt = angle / 2;
			
			if (item->ai_bits & GUARD)
				item->goal_anim_state = BOB_STOP;
			else if (bob->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = BOB_STOP;
			}
			else if (item->ai_bits & FOLLOW && (bob->reached_goal || iDist > 0x400000))
				item->goal_anim_state = BOB_STOP;
			else if (bob->mood == BORED_MOOD)
				item->goal_anim_state = BOB_WALK;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = BOB_WALK;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (item->current_anim_state >= BOB_DEATH)
	{
		bob->maximum_turn = 0;
		CreatureAnimation(item_number, angle, 0);
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			bob->maximum_turn = 0;
			item->anim_number = objects[BOB].anim_index + 30;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BOB_FALL3;
			break;

		case 2:
			bob->maximum_turn = 0;
			item->anim_number = objects[BOB].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BOB_CLIMB1;
			break;

		case 3:
			bob->maximum_turn = 0;
			item->anim_number = objects[BOB].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BOB_CLIMB2;
			break;

		case 4:
			bob->maximum_turn = 0;
			item->anim_number = objects[BOB].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BOB_CLIMB3;
			break;
		}
	}
}
