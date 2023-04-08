#include "../tomb3/pch.h"
#include "oilsmg.h"
#include "box.h"
#include "objects.h"
#include "../3dsystem/3d_gen.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/game.h"
#include "people.h"
#include "sound.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "lara.h"

static BITE_INFO oilsmg_gun = { 0, 400, 64, 7 };

void InitialiseOilSMG(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[WHITE_SOLDIER].anim_index + 12;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = OILSMG_STOP;
	item->goal_anim_state = OILSMG_STOP;
	
}

void OilSMGControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* smg;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO larainfo;
	long x, z, mood;
	short angle, tilt, head, torso_x, torso_y, frame, base;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	smg = (CREATURE_INFO*)item->data;
	angle = 0;
	tilt = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->fired_weapon)
	{
		phd_PushMatrix();
		pos.x = oilsmg_gun.x;
		pos.y = oilsmg_gun.y;
		pos.z = oilsmg_gun.z;
		GetJointAbsPosition(item, &pos, oilsmg_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 8, 192, 128, 32);
		phd_PopMatrix();
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != OILSMG_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 19;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = OILSMG_DEATH;
			smg->flags = !(GetRandomControl() & 3);
		}
		else if (smg->flags)
		{
			frame = item->frame_number;
			base = anims[item->anim_number].frame_base;

			if (frame > base + 3 && frame < base + 31 && !(frame & 3))
			{
				CreatureAIInfo(item, &info);
				head = info.angle;
				torso_y = info.angle;
				ShotLara(item, &info, &oilsmg_gun, 0, 0);
				SoundEffect(SFX_OIL_SMG_FIRE, &item->pos, 0x6000);
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(smg);
		else
			smg->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (smg->enemy == lara_item)
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

		mood = smg->enemy != lara_item;
		GetCreatureMood(item, &info, mood);
		CreatureMood(item, &info, mood);
		angle = CreatureTurn(item, smg->maximum_turn);

		enemy = smg->enemy;
		smg->enemy = lara_item;

		if ((larainfo.distance < 0x100000 || item->hit_status || TargetVisible(item, &larainfo)) && !(item->ai_bits & FOLLOW))
		{
			if (!smg->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		smg->enemy = enemy;

		switch (item->current_anim_state)
		{
		case OILSMG_STOP:
			head = larainfo.angle;
			smg->flags = 0;
			smg->maximum_turn = 0;

			if (item->anim_number == objects[item->object_number].anim_index + 17)
			{
				if (abs(info.angle) < 1820)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1820;
				else
					item->pos.y_rot += 1820;
			}

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(smg);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == OILSMG_STOP)
						item->goal_anim_state = OILSMG_WAIT;
					else
						item->goal_anim_state = OILSMG_STOP;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = OILSMG_WALK;
			else if (smg->mood == ESCAPE_MOOD)
				item->goal_anim_state = OILSMG_RUN;
			else if (Targetable(item, &info))
			{
				if (info.distance >= 0x900000 && info.zone_number == info.enemy_zone)
					item->goal_anim_state = OILSMG_WALK;
				else if (GetRandomControl() < 0x4000)
					item->goal_anim_state = OILSMG_AIM1;
				else
					item->goal_anim_state = OILSMG_AIM3;
			}
			else if (smg->mood == BORED_MOOD || item->ai_bits & FOLLOW && (smg->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = OILSMG_STOP;
			else if (smg->mood != BORED_MOOD && info.distance > 0x400000)
				item->goal_anim_state = OILSMG_RUN;
			else
				item->goal_anim_state = OILSMG_WALK;

			break;

		case OILSMG_WALK:
			smg->flags = 0;
			smg->maximum_turn = 910;
			head = larainfo.angle;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = OILSMG_WALK;
				head = 0;
			}
			else if (smg->mood == ESCAPE_MOOD)
				item->goal_anim_state = OILSMG_RUN;
			else if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (smg->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = OILSMG_STOP;
			else if (Targetable(item, &info))
			{
				if (info.distance < 0x900000 || info.zone_number != info.enemy_zone)
					item->goal_anim_state = OILSMG_STOP;
				else
					item->goal_anim_state = OILSMG_AIM2;
			}
			else if (smg->mood == BORED_MOOD)
			{
				if (info.ahead)
					item->goal_anim_state = OILSMG_STOP;
			}
			else if (info.distance > 0x400000)
				item->goal_anim_state = OILSMG_RUN;

			break;

		case OILSMG_RUN:

			if (info.ahead)
				head = info.angle;

			smg->maximum_turn = 1820;
			tilt = angle >> 1;

			if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (smg->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = OILSMG_WALK;
			else if (smg->mood != ESCAPE_MOOD)
			{
				if (Targetable(item, &info))
					item->goal_anim_state = OILSMG_WALK;
				else if (smg->mood == BORED_MOOD || (smg->mood == STALK_MOOD && !(item->ai_bits & FOLLOW) && info.distance < 0x400000))
					item->goal_anim_state = OILSMG_WALK;
			}

			break;

		case OILSMG_WAIT:
			head = larainfo.angle;
			smg->flags = 0;
			smg->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(smg);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == OILSMG_STOP)
						item->goal_anim_state = OILSMG_WAIT;
					else
						item->goal_anim_state = OILSMG_STOP;
				}
			}
			else if (Targetable(item, &info))
				item->goal_anim_state = OILSMG_SHOOT1;
			else if (smg->mood != BORED_MOOD || !info.ahead)
				item->goal_anim_state = OILSMG_STOP;

			break;

		case OILSMG_SHOOT3:

			if (item->goal_anim_state != OILSMG_STOP && (smg->mood == ESCAPE_MOOD || info.distance > 0x900000 || !Targetable(item, &info)))
				item->goal_anim_state = OILSMG_STOP;

		case OILSMG_SHOOT1:
		case OILSMG_SHOOT2:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (smg->flags)
				smg->flags--;
			else
			{
				ShotLara(item, &info, &oilsmg_gun, torso_y, 28);
				smg->flags = 5;
			}

			break;

		case OILSMG_AIM1:
		case OILSMG_AIM3:
			smg->flags = 0;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;

				if (Targetable(item, &info))
					item->goal_anim_state = item->current_anim_state == OILSMG_AIM1 ? OILSMG_SHOOT1 : OILSMG_SHOOT3;
				else
					item->goal_anim_state = OILSMG_STOP;
			}

			break;

		case OILSMG_AIM2:
			smg->flags = 0;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;

				if (Targetable(item, &info))
					item->goal_anim_state = OILSMG_SHOOT2;
				else
					item->goal_anim_state = OILSMG_WALK;
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
