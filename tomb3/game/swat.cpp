#include "../tomb3/pch.h"
#include "swat.h"
#include "box.h"
#include "../3dsystem/3d_gen.h"
#include "sphere.h"
#include "effect2.h"
#include "effects.h"
#include "../specific/game.h"
#include "people.h"
#include "sound.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "lara.h"

static BITE_INFO swat_gun = { 0, 300, 64, 7 };

void InitialiseSwat(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index + 12;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = SWAT_STOP;
	item->goal_anim_state = SWAT_STOP;
}

void SwatControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* swat;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO larainfo;
	long dx, dz;
	short angle, tilt, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	swat = (CREATURE_INFO*)item->data;

	if (!swat)
		return;

	angle = 0;
	tilt = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->fired_weapon)
	{
		phd_PushMatrix();
		pos.x = swat_gun.x;
		pos.y = swat_gun.y;
		pos.z = swat_gun.z;
		GetJointAbsPosition(item, &pos, swat_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 8, 192, 128, 32);
		phd_PopMatrix();
	}

	if (boxes[item->box_number].overlap_index & 0x4000)
	{
		DoLotsOfBloodD(item->pos.x_pos, item->pos.y_pos - GetRandomControl() - 32, item->pos.z_pos,
			(GetRandomControl() & 0x7F) + 128, short(GetRandomControl() << 1), item->room_number, 3);
		item->hit_points -= 20;
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != SWAT_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 19;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = SWAT_DEATH;
			swat->flags = !(GetRandomControl() & 1);
		}
		else if (swat->flags && item->frame_number > anims[item->anim_number].frame_base + 44 &&
			item->frame_number < anims[item->anim_number].frame_base + 52 && !(item->frame_number & 3))
		{
			CreatureAIInfo(item, &info);

			if (Targetable(item, &info))
			{
				if (info.angle > -0x2000 && info.angle < 0x2000)
				{
					torso_y = info.angle;
					head = info.angle;

					ShotLara(item, &info, &swat_gun, info.angle, 84);

					if (item->object_number == LON_MERCENARY1)
						SoundEffect(SFX_OIL_SMG_FIRE, &item->pos, 0x6000);
					else
						SoundEffect(SFX_SWAT_SMG_FIRE, &item->pos, 0x6000);
				}
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(swat);
		else
			swat->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (swat->enemy == lara_item)
		{
			larainfo.angle = info.angle;
			larainfo.distance = info.distance;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			larainfo.angle = short(phd_atan(dz, dx) - item->pos.y_rot);
			larainfo.distance = SQUARE(dx) + SQUARE(dz);
		}

		if (swat->enemy == lara_item)
		{
			GetCreatureMood(item, &info, 0);
			CreatureMood(item, &info, 0);
		}
		else
		{
			GetCreatureMood(item, &info, 1);
			CreatureMood(item, &info, 1);
		}

		angle = CreatureTurn(item, swat->maximum_turn);
		enemy = swat->enemy;
		swat->enemy = lara_item;

		if (item->hit_status || (larainfo.distance < 0x100000 || TargetVisible(item, &larainfo)) && abs(lara_item->pos.y_pos - item->pos.y_pos) < 2048)
		{
			if (!swat->alerted)
			{
				if (item->object_number == SWAT_GUN)
					SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);
				else
					SoundEffect(SFX_ENGLISH_HOY, &item->pos, SFX_DEFAULT);
			}

			AlertAllGuards(item_number);
		}

		swat->enemy = enemy;

		switch (item->current_anim_state)
		{
		case SWAT_STOP:
			head = larainfo.angle;
			swat->flags = 0;
			swat->maximum_turn = 0;

			if (item->anim_number == objects[item->object_number].anim_index + 17)
			{
				if (abs(info.angle) < 0x666)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 0x666;
				else
					item->pos.y_rot += 0x666;
			}

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(swat);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == SWAT_STOP)
						item->goal_anim_state = SWAT_WAIT;
					else
						item->goal_anim_state = SWAT_STOP;
				}
			}
			else if (item->ai_bits & PATROL1)
			{
				head = 0;
				item->goal_anim_state = SWAT_WALK;
			}
			else if (swat->mood == ESCAPE_MOOD)
				item->goal_anim_state = SWAT_RUN;
			else if (Targetable(item, &info))
			{
				if (info.distance >= 0x900000 && info.zone_number == info.enemy_zone)
					item->goal_anim_state = SWAT_WALK;
				else if (GetRandomControl() >= 0x4000)
					item->goal_anim_state = SWAT_AIM3;
				else
					item->goal_anim_state = SWAT_AIM1;
			}
			else if (swat->mood == BORED_MOOD || item->ai_bits & FOLLOW && (swat->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = SWAT_STOP;
			else if (info.distance > 0x400000)
				item->goal_anim_state = SWAT_RUN;
			else
				item->goal_anim_state = SWAT_WALK;

			break;

		case SWAT_WALK:
			head = larainfo.angle;
			swat->flags = 0;
			swat->maximum_turn = 1092;

			if (item->ai_bits & PATROL1)
			{
				head = 0;
				item->goal_anim_state = SWAT_WALK;
			}
			else if (swat->mood == ESCAPE_MOOD)
				item->goal_anim_state = SWAT_RUN;
			else if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (swat->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = SWAT_STOP;
			else if (Targetable(item, &info))
			{
				if (info.distance < 0x900000 || info.zone_number != info.enemy_zone)
					item->goal_anim_state = SWAT_STOP;
				else
					item->goal_anim_state = SWAT_AIM2;
			}
			else if (swat->mood == BORED_MOOD)
			{
				if (info.ahead)
					item->goal_anim_state = SWAT_STOP;
			}
			else if (info.distance > 0x400000)
				item->goal_anim_state = SWAT_RUN;

			break;

		case SWAT_RUN:

			if (info.ahead)
				head = info.angle;

			swat->maximum_turn = 1638;
			tilt = angle / 2;

			if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (swat->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = SWAT_WALK;
			else if (swat->mood != ESCAPE_MOOD)
			{
				if (Targetable(item, &info))
					item->goal_anim_state = SWAT_WALK;
				else if (swat->mood == BORED_MOOD || (swat->mood == STALK_MOOD && !(item->ai_bits & FOLLOW) && info.distance < 0x400000))
					item->goal_anim_state = SWAT_WALK;
			}

			break;

		case SWAT_WAIT:
			head = larainfo.angle;
			swat->flags = 0;
			swat->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(swat);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == SWAT_STOP)
						item->goal_anim_state = SWAT_WAIT;
					else
						item->goal_anim_state = SWAT_STOP;
				}
			}
			else if (Targetable(item, &info))
				item->goal_anim_state = SWAT_SHOOT1;
			else if (swat->mood != BORED_MOOD || !info.ahead)
				item->goal_anim_state = SWAT_STOP;

			break;

		case SWAT_SHOOT3:

			if (item->goal_anim_state != SWAT_STOP && (swat->mood == ESCAPE_MOOD || info.distance > 0x900000 || !Targetable(item, &info)))
				item->goal_anim_state = SWAT_STOP;

		case SWAT_SHOOT1:
		case SWAT_SHOOT2:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (swat->flags)
				swat->flags--;
			else
			{
				ShotLara(item, &info, &swat_gun, torso_y, 28);
				swat->flags = 5;
			}

			break;

		case SWAT_AIM1:
		case SWAT_AIM3:
			swat->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (!Targetable(item, &info))
					item->goal_anim_state = SWAT_STOP;
				else if (item->current_anim_state == SWAT_AIM1)
					item->goal_anim_state = SWAT_SHOOT1;
				else
					item->goal_anim_state = SWAT_SHOOT3;
			}

			break;

		case SWAT_AIM2:
			swat->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info))
					item->goal_anim_state = SWAT_SHOOT2;
				else
					item->goal_anim_state = SWAT_WALK;
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
