#include "../tomb3/pch.h"
#include "armysmg.h"
#include "box.h"
#include "objects.h"
#include "../3dsystem/3d_gen.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/game.h"
#include "../3dsystem/phd_math.h"
#include "sound.h"
#include "people.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

static BITE_INFO army_gun = { 0, 300, 64, 7 };

void InitialiseArmySMG(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[STHPAC_MERCENARY].anim_index + 12;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = ARMY_STOP;
	item->goal_anim_state = ARMY_STOP;
}

void ArmySMGControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* army;
	PHD_VECTOR pos;
	AI_INFO info;
	long dist, best_dist, x, z, iDist, lp;
	short tilt, angle, head, torso_x, torso_y, iAngle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	army = (CREATURE_INFO*)item->data;

	if (!army)
		return;

	tilt = 0;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->fired_weapon)
	{
		phd_PushMatrix();
		pos.x = army_gun.x;
		pos.y = army_gun.y;
		pos.z = army_gun.z;
		GetJointAbsPosition(item, &pos, army_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 8, 192, 128, 32);
		phd_PopMatrix();
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != ARMY_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 19;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = ARMY_DEATH;
			army->flags = !(GetRandomControl() & 3);
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(army);
		else if (army->hurt_by_lara)
			army->enemy = lara_item;
		else
		{
			army->enemy = 0;
			best_dist = 0x7FFFFFFF;

			for (lp = 0; lp < MAX_LOT; lp++)
			{
				if (baddie_slots[lp].item_num == NO_ITEM || baddie_slots[lp].item_num == item_number)
					continue;

				enemy = &items[baddie_slots[lp].item_num];

				if (enemy->object_number == LARA || enemy->object_number == STHPAC_MERCENARY || enemy == lara_item)
					continue;

				x = enemy->pos.x_pos - item->pos.x_pos;
				z = enemy->pos.z_pos - item->pos.z_pos;
				dist = SQUARE(x) + SQUARE(z);

				if (dist < best_dist)
				{
					army->enemy = enemy;
					best_dist = dist;
				}
			}
		}

		CreatureAIInfo(item, &info);

		if (army->enemy == lara_item)
		{
			iDist = info.distance;
			iAngle = info.angle;
		}
		else
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			iAngle = short(phd_atan(z, x) - item->pos.y_rot);
			iDist = SQUARE(x) + SQUARE(z);
		}

		if (!army->hurt_by_lara && army->enemy == lara_item)
			army->enemy = 0;

		GetCreatureMood(item, &info, army->enemy != lara_item);
		CreatureMood(item, &info, army->enemy != lara_item);
		angle = CreatureTurn(item, army->maximum_turn);

		if (item->hit_status)
		{
			if (!army->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		switch (item->current_anim_state)
		{
		case ARMY_STOP:
			army->flags = 0;
			army->maximum_turn = 0;
			head = iAngle;

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
				head = AIGuard(army);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == ARMY_STOP)
						item->goal_anim_state = ARMY_WAIT;
					else
						item->goal_anim_state = ARMY_STOP;
				}
			}
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = ARMY_WALK;
				head = 0;
			}
			else if (army->mood == ESCAPE_MOOD)
				item->goal_anim_state = ARMY_RUN;
			else if (Targetable(item, &info))
			{
				if (info.distance >= 0x900000 && info.zone_number == info.enemy_zone)
					item->goal_anim_state = ARMY_WALK;
				else if (GetRandomControl() < 0x4000)
					item->goal_anim_state = ARMY_AIM1;
				else
					item->goal_anim_state = ARMY_AIM3;
			}
			else if ((!army->alerted && army->mood == BORED_MOOD) || (item->ai_bits & FOLLOW && (army->reached_goal || iDist > 0x400000)))
				item->goal_anim_state = ARMY_STOP;
			else if (army->mood != BORED_MOOD && info.distance > 0x400000)
				item->goal_anim_state = ARMY_RUN;
			else
				item->goal_anim_state = ARMY_WALK;

			break;

		case ARMY_WALK:
			head = iAngle;
			army->flags = 0;
			army->maximum_turn = 910;

			if (item->ai_bits & PATROL1)
				item->goal_anim_state = ARMY_WALK;
			else if (army->mood == ESCAPE_MOOD)
				item->goal_anim_state = ARMY_RUN;
			else if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (army->reached_goal || iDist > 0x400000))
				item->goal_anim_state = ARMY_STOP;
			else if (Targetable(item, &info))
			{
				if (info.distance < 0x900000 || info.zone_number != info.enemy_zone)
					item->goal_anim_state = ARMY_STOP;
				else
					item->goal_anim_state = ARMY_AIM2;
			}
			else if (army->mood == BORED_MOOD)
			{
				if (info.ahead)
					item->goal_anim_state = ARMY_STOP;
			}
			else if (info.distance > 0x400000)
				item->goal_anim_state = ARMY_RUN;

			break;

		case ARMY_RUN:

			if (info.ahead)
				head = info.angle;

			army->maximum_turn = 1820;
			tilt = angle >> 1;

			if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (army->reached_goal || iDist > 0x400000))
				item->goal_anim_state = ARMY_WALK;
			else if (army->mood != ESCAPE_MOOD)
			{
				if (Targetable(item, &info))
					item->goal_anim_state = ARMY_WALK;
				else if (army->mood == BORED_MOOD || army->mood == STALK_MOOD && !(item->ai_bits & FOLLOW) && info.distance < 0x400000)
					item->goal_anim_state = ARMY_WALK;
			}

			break;

		case ARMY_WAIT:
			head = iAngle;
			army->flags = 0;
			army->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(army);

				if (!(GetRandomControl() & 0xFF))
					item->goal_anim_state = ARMY_STOP;
			}
			else if (Targetable(item, &info))
				item->goal_anim_state = ARMY_SHOOT1;
			else if (army->mood != BORED_MOOD || !info.ahead)
					item->goal_anim_state = ARMY_STOP;

			break;

		case ARMY_SHOOT3:

			if (item->goal_anim_state != 1 && (army->mood == ESCAPE_MOOD || info.distance > 0x900000 || !Targetable(item, &info)))
				item->goal_anim_state = 1;

		case ARMY_SHOOT1:
		case ARMY_SHOOT2:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (army->flags)
				army->flags--;
			else
			{
				ShotLara(item, &info, &army_gun, torso_y, 28);
				army->flags = 5;
			}

			break;

		case ARMY_AIM1:
		case ARMY_AIM3:
			army->flags = 0;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
				
				if (Targetable(item, &info))
					item->goal_anim_state = item->current_anim_state == ARMY_AIM1 ? ARMY_SHOOT1 : ARMY_SHOOT3;
				else
					item->goal_anim_state = ARMY_STOP;
			}

			break;

		case ARMY_AIM2:
			army->flags = 0;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;

				if (Targetable(item, &info))
					item->goal_anim_state = ARMY_SHOOT2;
				else
					item->goal_anim_state = ARMY_WALK;
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
