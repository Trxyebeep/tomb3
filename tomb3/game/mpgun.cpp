#include "../tomb3/pch.h"
#include "mpgun.h"
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
#include "lot.h"
#include "lara.h"

static BITE_INFO mpgun_gun = { 0, 160, 40, 13 };

void MPGunControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* candidate;
	ITEM_INFO* enemy;
	CREATURE_INFO* mp;
	CREATURE_INFO* target;
	FLOOR_INFO* floor;
	AI_INFO info;
	AI_INFO lara_info;
	PHD_VECTOR pos;
	long dx, dz, x, y, z, h, near_cover;
	short tilt, torso_y, torso_x, head, angle, room_number;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	mp = (CREATURE_INFO*)item->data;
	tilt = 0;
	torso_y = 0;
	torso_x = 0;
	head = 0;
	angle = 0;

	if (item->fired_weapon)
	{
		phd_PushMatrix();
		pos.x = mpgun_gun.x;
		pos.y = mpgun_gun.y;
		pos.z = mpgun_gun.z;
		GetJointAbsPosition(item, &pos, mpgun_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, 2 * item->fired_weapon + 4, 192, 128, 32);
		phd_PopMatrix();
	}

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

						if (SQUARE(dx) + SQUARE(dz) < lara_info.distance)
							mp->enemy = candidate;
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

		if (mp->enemy == lara_item)
		{
			GetCreatureMood(item, &info, 0);
			CreatureMood(item, &info, 0);
		}
		else
		{
			GetCreatureMood(item, &info, 1);
			CreatureMood(item, &info, 1);
		}

		angle = CreatureTurn(item, mp->maximum_turn);

		room_number = item->room_number;
		x = item->pos.x_pos + (WALL_SIZE * phd_sin(item->pos.y_rot + lara_info.angle) >> W2V_SHIFT);
		y = item->pos.y_pos;
		z = item->pos.z_pos + (WALL_SIZE * phd_cos(item->pos.y_rot + lara_info.angle) >> W2V_SHIFT);
		floor = GetFloor(x, y, z, &room_number);
		h = GetHeight(floor, x, y, z);
		near_cover = y > h + 384 && y < h + 1152 && lara_info.distance > 0x100000;

		enemy = mp->enemy;
		mp->enemy = lara_item;

		if (lara_info.distance < 0x100000 || item->hit_status || TargetVisible(item, &lara_info))
		{
			if (!mp->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		mp->enemy = enemy;

		switch (item->current_anim_state)
		{
		case MPGUN_WAIT:
			head = lara_info.angle;
			mp->maximum_turn = 0;

			if (item->anim_number == objects[item->object_number].anim_index + 17 ||
				item->anim_number == objects[item->object_number].anim_index + 27 ||
				item->anim_number == objects[item->object_number].anim_index + 28)
			{
				if (abs(info.angle) < 1820)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					info.angle -= 1820;
				else
					info.angle += 1820;
			}

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(mp);
				item->goal_anim_state = MPGUN_WAIT;
			}
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = MPGUN_WALK;
				head = 0;
			}
			else if (near_cover && (lara.target == item || item->hit_status))
				item->goal_anim_state = MPGUN_DUCK;
			else if (item->required_anim_state == MPGUN_DUCK)
				item->goal_anim_state = MPGUN_DUCK;
			else if (mp->mood == ESCAPE_MOOD)
				item->goal_anim_state = MPGUN_RUN;
			else if (Targetable(item, &info))
			{
				dx = GetRandomControl();

				if (dx < 0x2000)
					item->goal_anim_state = MPGUN_SHOOT1;
				else if (dx < 0x4000)
					item->goal_anim_state = MPGUN_SHOOT2;
				else
					item->goal_anim_state = MPGUN_AIM3;
			}
			else if (mp->mood != BORED_MOOD && (!(item->ai_bits & FOLLOW) || (!mp->reached_goal && lara_info.distance <= 0x400000)))
				item->goal_anim_state = MPGUN_RUN;
			else if (info.ahead)
				item->goal_anim_state = MPGUN_WAIT;
			else
				item->goal_anim_state = MPGUN_WALK;

			break;

		case MPGUN_WALK:
			head = lara_info.angle;
			mp->maximum_turn = 1092;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = MPGUN_WALK;
				head = 0;
			}
			else if (near_cover && (lara.target == item || item->hit_status))
			{
				item->required_anim_state = MPGUN_DUCK;
				item->goal_anim_state = MPGUN_WAIT;
			}
			else if (mp->mood == ESCAPE_MOOD)
				item->goal_anim_state = MPGUN_RUN;
			else if (Targetable(item, &info))
			{
				if (info.distance > 0x240000 && info.zone_number == info.enemy_zone)
					item->goal_anim_state = MPGUN_AIM4;
				else
					item->goal_anim_state = MPGUN_WAIT;
			}
			else if (mp->mood != BORED_MOOD)
				item->goal_anim_state = MPGUN_RUN;
			else if (info.ahead)
				item->goal_anim_state = MPGUN_WALK;
			else
				item->goal_anim_state = MPGUN_WAIT;

			break;

		case MPGUN_RUN:

			if (info.ahead)
				head = info.angle;

			mp->maximum_turn = 1820;
			tilt = angle / 2;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = MPGUN_WAIT;
			else if (near_cover && (lara.target == item || item->hit_status))
			{
				item->required_anim_state = MPGUN_DUCK;
				item->goal_anim_state = MPGUN_WAIT;
			}
			else if (mp->mood != ESCAPE_MOOD)
			{
				if (Targetable(item, &info) || item->ai_bits & FOLLOW && (mp->reached_goal || lara_info.distance > 0x400000))
					item->goal_anim_state = MPGUN_WAIT;
				else if (mp->mood == BORED_MOOD)
					item->goal_anim_state = MPGUN_WALK;
			}

			break;

		case MPGUN_AIM1:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (item->anim_number == objects[MP2].anim_index + 12 ||
				(item->anim_number == objects[MP2].anim_index + 1 && item->frame_number == anims[item->anim_number].frame_base + 10))
			{
				if (!ShotLara(item, &info, &mpgun_gun, torso_y, 32))
					item->required_anim_state = MPGUN_WAIT;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = MPGUN_DUCK;
				item->goal_anim_state = MPGUN_WAIT;
			}

			break;

		case MPGUN_SHOOT1:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (item->required_anim_state == MPGUN_WAIT)
				item->goal_anim_state = MPGUN_WAIT;

			break;

		case MPGUN_SHOOT2:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (item->frame_number == anims[item->anim_number].frame_base)
				item->goal_anim_state = MPGUN_WAIT;
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = MPGUN_DUCK;
				item->goal_anim_state = MPGUN_WAIT;
			}

			break;

		case MPGUN_SHOOT3A:
		case MPGUN_SHOOT3B:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (item->frame_number == anims[item->anim_number].frame_base || item->frame_number == anims[item->anim_number].frame_base + 11)
			{
				if (!ShotLara(item, &info, &mpgun_gun, torso_y, 32))
					item->goal_anim_state = MPGUN_WAIT;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = MPGUN_DUCK;
				item->goal_anim_state = MPGUN_WAIT;
			}

			break;

		case MPGUN_SHOOT4A:
		case MPGUN_SHOOT4B:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (item->required_anim_state == MPGUN_WALK)
				item->goal_anim_state = MPGUN_WALK;

			if (item->frame_number == anims[item->anim_number].frame_base + 16 && !ShotLara(item, &info, &mpgun_gun, torso_y, 32))
				item->goal_anim_state = MPGUN_WALK;

			if (info.distance < 0x240000)
				item->goal_anim_state = MPGUN_WALK;

			break;

		case MPGUN_AIM4:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			if (item->anim_number == objects[MP2].anim_index + 18 && item->frame_number == anims[item->anim_number].frame_base + 17
				|| item->anim_number == objects[MP2].anim_index + 19 && item->frame_number == anims[item->anim_number].frame_base + 6)
			{
				if (!ShotLara(item, &info, &mpgun_gun, torso_y, 32))
					item->required_anim_state = MPGUN_WALK;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = MPGUN_DUCK;
				item->goal_anim_state = MPGUN_WAIT;
			}

			if (info.distance < 0x240000)
				item->required_anim_state = MPGUN_WALK;

			break;

		case MPGUN_DUCKED:
			if (info.ahead)
				head = info.angle;

			mp->maximum_turn = 0;

			if (Targetable(item, &info))
				item->goal_anim_state = MPGUN_DUCKAIM;
			else if (item->hit_status || !near_cover || info.ahead && !(GetRandomControl() & 0x1F))
				item->goal_anim_state = MPGUN_STAND;
			else
				item->goal_anim_state = MPGUN_DUCKWALK;

			break;

		case MPGUN_DUCKAIM:
			mp->maximum_turn = 182;

			if (info.ahead)
				torso_y = info.angle;
			if (Targetable(item, &info))
				item->goal_anim_state = MPGUN_DUCKSHOT;
			else
				item->goal_anim_state = MPGUN_DUCKED;

			break;

		case MPGUN_DUCKSHOT:

			if (info.ahead)
				torso_y = info.angle;

			if (item->frame_number == anims[item->anim_number].frame_base && (!ShotLara(item, &info, &mpgun_gun, torso_y, 32) || !(GetRandomControl() & 7)))
				item->goal_anim_state = MPGUN_DUCKED;

			break;

		case MPGUN_DUCKWALK:

			if (info.ahead)
				head = info.angle;

			mp->maximum_turn = 1092;

			if (Targetable(item, &info) || item->hit_status || !near_cover || info.ahead && !(GetRandomControl() & 0x1F))
				item->goal_anim_state = MPGUN_DUCKED;

			break;
		}
	}
	else
	{
		item->hit_points = 0;

		if (item->current_anim_state != MPGUN_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 14;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MPGUN_DEATH;
		}
		else if (!(GetRandomControl() & 3) && item->frame_number == anims[item->anim_number].frame_base + 1)
		{
			CreatureAIInfo(item, &info);

			if (Targetable(item, &info))
			{
				if (info.angle > -0x2000 && info.angle < 0x2000)
				{
					torso_y = info.angle;
					head = info.angle;
					ShotLara(item, &info, &mpgun_gun, info.angle, 32);
					SoundEffect(SFX_OIL_SMG_FIRE, &item->pos, 0x6000);
				}
			}
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
