#include "../tomb3/pch.h"
#include "51baton.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "../specific/game.h"
#include "../3dsystem/phd_math.h"
#include "sound.h"
#include "people.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

static BITE_INFO baton_hit = { 247, 10, 11, 13 };
static BITE_INFO baton_kick = { 0, 0, 100, 6 };

void InitialiseBaton(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[MP1].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = BATON_STOP;
	item->goal_anim_state = BATON_STOP;
}

void BatonControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* candidate;
	ITEM_INFO* enemy;
	CREATURE_INFO* mp;
	CREATURE_INFO* target;
	AI_INFO info;
	AI_INFO lara_info;
	long dx, dy, dz, best, dist;
	short tilt, torso_y, torso_x, head, angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	mp = (CREATURE_INFO*)item->data;
	tilt = 0;
	torso_y = 0;
	torso_x = 0;
	head = 0;
	angle = 0;

	if (boxes[item->box_number].overlap_index & 0x4000)
	{
		DoLotsOfBloodD(item->pos.x_pos, item->pos.y_pos - (GetRandomControl() & 0xFF) - 32, item->pos.z_pos,
			(GetRandomControl() & 0x7F) + 128, short(GetRandomControl() << 1), item->room_number, 3);
		item->hit_points -= 20;
	}

	if (item->hit_points > 0)
	{
		if (item->ai_bits)
			GetAITarget(mp);
		else
		{
			mp->enemy = lara_item;
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			lara_info.distance = SQUARE(dx) + SQUARE(dz);
			best = 0x7FFFFFFF;

			for (int i = 0; i < MAX_LOT; i++)
			{
				target = &baddie_slots[i];

				if (target->item_num != NO_ITEM && target->item_num != item_number)
				{
					candidate = &items[target->item_num];

					if (candidate->object_number == LARA || candidate->object_number == BOB)
					{
						dx = candidate->pos.x_pos - item->pos.x_pos;
						dz = candidate->pos.z_pos - item->pos.z_pos;

						if (dx <= 32000 && dx >= -32000 && dz <= 32000 && dz >= -32000)
						{
							dist = SQUARE(dx) + SQUARE(dz);

							if (dist < best && dist < lara_info.distance)
							{
								mp->enemy = candidate;
								best = dist;
							}
						}
					}
				}
			}
		}

		CreatureAIInfo(item, &info);

		if (mp->enemy == lara_item)
		{
			lara_info.angle = info.angle;
			lara_info.distance = info.distance;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			lara_info.angle = short(phd_atan(dz, dx) - item->pos.y_rot);
			lara_info.distance = SQUARE(dx) + SQUARE(dz);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, mp->maximum_turn);

		enemy = mp->enemy;
		mp->enemy = lara_item;

		if (item->hit_status || (lara_info.distance < 0x100000 || TargetVisible(item, &lara_info)) && abs(lara_item->pos.y_pos - item->pos.y_pos) < 1024)
		{
			if (!mp->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		mp->enemy = enemy;

		switch (item->current_anim_state)
		{
		case BATON_WAIT:

			if (mp->alerted || item->goal_anim_state == BATON_RUN)
			{
				item->goal_anim_state = BATON_STOP;
				break;
			}

		case BATON_STOP:
			mp->flags = 0;
			mp->maximum_turn = 0;
			head = lara_info.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(mp);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == BATON_STOP)
						item->goal_anim_state = BATON_WAIT;
					else
						item->goal_anim_state = BATON_STOP;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = BATON_WALK;
			else if (mp->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = BATON_STOP;
				else
					item->goal_anim_state = BATON_RUN;
			}
			else if (mp->mood == BORED_MOOD || item->ai_bits & FOLLOW && (mp->reached_goal || lara_info.distance > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = BATON_STOP;
				else
					item->goal_anim_state = BATON_RUN;
			}
			else if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = BATON_AIM0;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = BATON_AIM1;
			else
				item->goal_anim_state = BATON_RUN;

			break;

		case BATON_WALK:
			mp->maximum_turn = 1092;
			head = lara_info.angle;
			mp->flags = 0;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = BATON_WALK;
				head = 0;
			}
			else if (mp->mood == ESCAPE_MOOD)
				item->goal_anim_state = BATON_RUN;
			else if (mp->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
				{
					item->required_anim_state = BATON_WAIT;
					item->goal_anim_state = BATON_STOP;
				}
			}
			else if (info.bite && info.distance < 0x240000 && info.x_angle < 0)
				item->goal_anim_state = BATON_KICK;
			else if (info.bite)
			{
				if (info.distance < 0x40000)
					item->goal_anim_state = BATON_STOP;
				else if (info.distance < 0x190000)
					item->goal_anim_state = BATON_AIM2;
				else
					item->goal_anim_state = BATON_RUN;
			}
			else
				item->goal_anim_state = BATON_RUN;

			break;

		case BATON_PUNCH2:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			mp->maximum_turn = 1092;

			if (enemy == lara_item)
			{
				if (mp->flags != 2 && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 100;
					lara_item->hit_status = 1;
					CreatureEffect(item, &baton_hit, DoBloodSplat);
					mp->flags = 2;
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}
			else if (mp->flags != 2 && enemy)
			{
				dx = abs(enemy->pos.x_pos - item->pos.x_pos);
				dy = abs(enemy->pos.y_pos - item->pos.y_pos);
				dz = abs(enemy->pos.z_pos - item->pos.z_pos);

				if (dx < 256 && dy <= 256 && dz < 256)
				{
					enemy->hit_points -= 6;
					enemy->hit_status = 1;
					mp->flags = 2;
					CreatureEffect(item, &baton_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}

			break;

		case BATON_AIM2:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			mp->maximum_turn = 1092;
			mp->flags = 0;

			if (info.bite && info.distance < 0x190000)
				item->goal_anim_state = BATON_PUNCH2;
			else
				item->goal_anim_state = BATON_WALK;

			break;

		case BATON_AIM1:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			mp->maximum_turn = 1092;
			mp->flags = 0;

			if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = BATON_PUNCH1;
			else
				item->goal_anim_state = BATON_STOP;

			break;

		case BATON_AIM0:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			mp->maximum_turn = 1092;
			mp->flags = 0;

			if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = BATON_PUNCH0;
			else
				item->goal_anim_state = BATON_STOP;

			break;

		case BATON_PUNCH1:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			mp->maximum_turn = 1092;

			if (enemy == lara_item)
			{
				if (!mp->flags && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 80;
					lara_item->hit_status = 1;
					CreatureEffect(item, &baton_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					mp->flags = 1;
				}
			}
			else if (!mp->flags && enemy)
			{
				dx = abs(enemy->pos.x_pos - item->pos.x_pos);
				dy = abs(enemy->pos.y_pos - item->pos.y_pos);
				dz = abs(enemy->pos.z_pos - item->pos.z_pos);

				if (dx < 256 && dy <= 256 && dz < 256)
				{
					enemy->hit_points -= 5;
					enemy->hit_status = 1;
					mp->flags = 1;
					CreatureEffect(item, &baton_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}

			if (info.ahead && info.distance > 0x100000 && info.distance < 0x190000)
				item->goal_anim_state = BATON_PUNCH2;

			break;

		case BATON_PUNCH0:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			mp->maximum_turn = 1092;

			if (enemy == lara_item)
			{
				if (!mp->flags && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 80;
					lara_item->hit_status = 1;
					CreatureEffect(item, &baton_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					mp->flags = 1;
				}
			}
			else if (!mp->flags && enemy)
			{
				dx = abs(enemy->pos.x_pos - item->pos.x_pos);
				dy = abs(enemy->pos.y_pos - item->pos.y_pos);
				dz = abs(enemy->pos.z_pos - item->pos.z_pos);

				if (dx < 256 && dy <= 256 && dz < 256)
				{
					enemy->hit_points -= 5;
					enemy->hit_status = 1;
					mp->flags = 1;
					CreatureEffect(item, &baton_hit, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}

			break;

		case BATON_RUN:

			if (info.ahead)
				head = info.angle;

			mp->maximum_turn = 1274;
			tilt = angle / 2;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = BATON_WAIT;
			else if (mp->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = BATON_STOP;
			}
			else if (item->ai_bits & FOLLOW && (mp->reached_goal || lara_info.distance > 0x400000))
				item->goal_anim_state = BATON_STOP;
			else if (mp->mood == BORED_MOOD)
				item->goal_anim_state = BATON_WALK;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = BATON_WALK;

			break;

		case BATON_KICK:

			if (info.ahead)
				torso_y = info.angle;

			mp->maximum_turn = 1092;

			if (enemy == lara_item)
			{
				if (mp->flags != 1 && item->touch_bits & 0x60 && item->frame_number > anims[item->anim_number].frame_base + 8)
				{
					lara_item->hit_points -= 150;
					lara_item->hit_status = 1;
					CreatureEffect(item, &baton_kick, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					mp->flags = 1;
				}
			}
			else if (mp->flags && enemy && item->frame_number > anims[item->anim_number].frame_base + 8)
			{
				dx = abs(enemy->pos.x_pos - item->pos.x_pos);
				dy = abs(enemy->pos.y_pos - item->pos.y_pos);
				dz = abs(enemy->pos.z_pos - item->pos.z_pos);

				if (dx < 256 && dy <= 256 && dz < 256)
				{
					enemy->hit_points -= 9;
					enemy->hit_status = 1;
					mp->flags = 1;
					CreatureEffect(item, &baton_kick, DoBloodSplat);
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}

			break;
		}
	}
	else
	{
		if (item->current_anim_state != BATON_DEATH)
		{
			item->anim_number = objects[MP1].anim_index + 26;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BATON_DEATH;
			mp->LOT.step = 256;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (item->current_anim_state >= BATON_DEATH)
	{
		mp->maximum_turn = 0;
		CreatureAnimation(item_number, angle, 0);
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			mp->maximum_turn = 0;
			item->anim_number = objects[MP1].anim_index + 30;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BATON_FALL3;
			break;

		case 2:
			mp->maximum_turn = 0;
			item->anim_number = objects[MP1].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BATON_CLIMB1;
			break;

		case 3:
			mp->maximum_turn = 0;
			item->anim_number = objects[MP1].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BATON_CLIMB2;
			break;

		case 4:
			mp->maximum_turn = 0;
			item->anim_number = objects[MP1].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BATON_CLIMB3;
			break;
		}
	}
}
