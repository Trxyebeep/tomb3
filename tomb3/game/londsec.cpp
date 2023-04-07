#include "../tomb3/pch.h"
#include "londsec.h"
#include "box.h"
#include "../3dsystem/3d_gen.h"
#include "sphere.h"
#include "effect2.h"
#include "../specific/game.h"
#include "people.h"
#include "sound.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "objects.h"
#include "lara.h"

static BITE_INFO londsec_gun = { 0, 160, 40, 13 };

void LondSecControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* sec;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO larainfo;
	long x, y, z, mood, h;
	short angle, tilt, head, torso_x, torso_y, room_number, near_cover, rnd;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	sec = (CREATURE_INFO*)item->data;
	angle = 0;
	tilt = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->fired_weapon)
	{
		phd_PushMatrix();
		pos.x = londsec_gun.x;
		pos.y = londsec_gun.y;
		pos.z = londsec_gun.z;
		GetJointAbsPosition(item, &pos, londsec_gun.mesh_num);
		TriggerDynamic(pos.x, pos.y, pos.z, (item->fired_weapon << 1) + 4, 192, 128, 32);
		phd_PopMatrix();
	}

	if (item->hit_points <= 0)
	{
		item->hit_points = 0;

		if (item->current_anim_state != LONDSEC_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 14;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LONDSEC_DEATH;
		}
		else if (GetRandomControl() & 1)
		{
			if (item->frame_number == anims[item->anim_number].frame_base + 3 || item->frame_number == anims[item->anim_number].frame_base + 28)
			{
				CreatureAIInfo(item, &info);

				if (Targetable(item, &info))
				{
					if (info.angle > -0x2000 && info.angle < 0x2000)
					{
						torso_y = info.angle;
						head = info.angle;
						ShotLara(item, &info, &londsec_gun, info.angle, 64);
						SoundEffect(SFX_SECURITY_GUARD_FIRE, &item->pos, 0x6000);
					}
				}
			}
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(sec);
		else
			sec->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (sec->enemy == lara_item)
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

		mood = sec->enemy != lara_item;
		GetCreatureMood(item, &info, mood);
		CreatureMood(item, &info, mood);
		angle = CreatureTurn(item, sec->maximum_turn);

		x = item->pos.x_pos + (WALL_SIZE * phd_sin(item->pos.y_rot + larainfo.angle) >> W2V_SHIFT);
		y = item->pos.y_pos;
		z = item->pos.z_pos + (WALL_SIZE * phd_cos(item->pos.y_rot + larainfo.angle) >> W2V_SHIFT);

		room_number = item->room_number;
		floor = GetFloor(x, y, z, &room_number);
		h = GetHeight(floor, x, y, z);
		y = item->pos.y_pos;
		near_cover = y > h + 384 && y < h + 1152 && larainfo.distance > 0x100000;

		enemy = sec->enemy;
		sec->enemy = lara_item;

		if (item->hit_status || (larainfo.distance < 0x100000 || TargetVisible(item, &larainfo)) && abs(lara_item->pos.y_pos - item->pos.y_pos) < 2048)
		{
			if (!sec->alerted)
				SoundEffect(SFX_ENGLISH_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		sec->enemy = enemy;

		switch (item->current_anim_state)
		{
		case LONDSEC_WAIT:
			head = larainfo.angle;

			if (item->anim_number == objects[item->object_number].anim_index + 17 ||
				item->anim_number == objects[item->object_number].anim_index + 27 ||
				item->anim_number == objects[item->object_number].anim_index + 28)
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
				head = AIGuard(sec);
				item->goal_anim_state = LONDSEC_WAIT;
			}
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = LONDSEC_WALK;
				head = 0;
			}
			else if (near_cover && (lara.target == item || item->hit_status))
				item->goal_anim_state = LONDSEC_DUCK;
			else if (item->required_anim_state == LONDSEC_DUCK)
				item->goal_anim_state = LONDSEC_DUCK;
			else if (sec->mood == ESCAPE_MOOD)
				item->goal_anim_state = LONDSEC_RUN;
			else if (Targetable(item, &info))
			{
				if (info.distance > 0x400000)
					item->goal_anim_state = LONDSEC_WALK;
				else
				{
					rnd = (short)GetRandomControl();

					if (rnd < 0x2000)
						item->goal_anim_state = LONDSEC_SHOOT1;
					else if (rnd < 0x4000)
						item->goal_anim_state = LONDSEC_SHOOT2;
					else
						item->goal_anim_state = LONDSEC_AIM3;
				}
			}
			else if (sec->mood == BORED_MOOD || item->ai_bits & FOLLOW && (sec->reached_goal || larainfo.distance > 0x400000))
			{
				if (info.ahead)
					item->goal_anim_state = LONDSEC_WAIT;
				else
					item->goal_anim_state = LONDSEC_WALK;
			}
			else
				item->goal_anim_state = LONDSEC_RUN;

			break;

		case LONDSEC_WALK:
			head = larainfo.angle;
			sec->maximum_turn = 910;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = LONDSEC_WALK;
				head = 0;
			}
			else if (near_cover && (lara.target == item || item->hit_status))
			{
				item->required_anim_state = LONDSEC_DUCK;
				item->goal_anim_state = LONDSEC_WAIT;
			}
			else if (sec->mood == ESCAPE_MOOD)
				item->goal_anim_state = LONDSEC_RUN;
			else if (Targetable(item, &info))
			{
				if (info.distance > 0x400000 && info.zone_number == info.enemy_zone)
					item->goal_anim_state = LONDSEC_AIM4;
				else
					item->goal_anim_state = LONDSEC_WAIT;
			}
			else if (sec->mood == BORED_MOOD)
			{
				if (info.ahead)
					item->goal_anim_state = LONDSEC_WALK;
				else
					item->goal_anim_state = LONDSEC_WAIT;
			}
			else
				item->goal_anim_state = LONDSEC_RUN;

			break;

		case LONDSEC_RUN:

			if (info.ahead)
				head = info.angle;

			sec->maximum_turn = 1820;
			tilt = angle >> 1;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = LONDSEC_WAIT;
			else if (near_cover && (lara.target == item || item->hit_status))
			{
				item->required_anim_state = LONDSEC_DUCK;
				item->goal_anim_state = LONDSEC_WAIT;
			}
			else if (sec->mood != ESCAPE_MOOD)
			{
				if (Targetable(item, &info) || item->ai_bits & FOLLOW && (sec->reached_goal || larainfo.distance > 0x400000))
					item->goal_anim_state = LONDSEC_WAIT;
				else if (sec->mood == BORED_MOOD)
					item->goal_anim_state = LONDSEC_WALK;
			}

			break;

		case LONDSEC_AIM1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->anim_number == objects[SECURITY_GUARD].anim_index + 12 ||
				(item->anim_number == objects[SECURITY_GUARD].anim_index + 1 && item->frame_number == anims[item->anim_number].frame_base + 10))
			{
				if (!ShotLara(item, &info, &londsec_gun, torso_y, 32))
					item->required_anim_state = LONDSEC_WAIT;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = LONDSEC_DUCK;
				item->goal_anim_state = LONDSEC_WAIT;
			}

			break;

		case LONDSEC_SHOOT1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->required_anim_state == LONDSEC_WAIT)
				item->goal_anim_state = LONDSEC_WAIT;

			break;

		case LONDSEC_SHOOT2:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->frame_number == anims[item->anim_number].frame_base)
			{
				if (!ShotLara(item, &info, &londsec_gun, torso_y, 32))
					item->goal_anim_state = LONDSEC_WAIT;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = LONDSEC_DUCK;
				item->goal_anim_state = LONDSEC_WAIT;
			}

			break;

		case LONDSEC_SHOOT3A:
		case LONDSEC_SHOOT3B:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->frame_number == anims[item->anim_number].frame_base || item->frame_number == anims[item->anim_number].frame_base + 11)
			{
				if (!ShotLara(item, &info, &londsec_gun, torso_y, 32))
					item->goal_anim_state = LONDSEC_WAIT;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = LONDSEC_DUCK;
				item->goal_anim_state = LONDSEC_WAIT;
			}

			break;

		case LONDSEC_SHOOT4A:
		case LONDSEC_SHOOT4B:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->required_anim_state == LONDSEC_WALK)
				item->goal_anim_state = LONDSEC_WALK;

			if (item->frame_number == anims[item->anim_number].frame_base + 16 && !ShotLara(item, &info, &londsec_gun, torso_y, 32))
				item->goal_anim_state = LONDSEC_WALK;

			if (info.distance < 0x400000)
				item->goal_anim_state = LONDSEC_WALK;

			break;

		case LONDSEC_AIM4:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (item->anim_number == objects[SECURITY_GUARD].anim_index + 18 && item->frame_number == anims[item->anim_number].frame_base + 17 ||
				item->anim_number == objects[SECURITY_GUARD].anim_index + 19 && item->frame_number == anims[item->anim_number].frame_base + 6)
			{
				if (!ShotLara(item, &info, &londsec_gun, torso_y, 32))
					item->required_anim_state = LONDSEC_WALK;
			}
			else if (item->hit_status && !(GetRandomControl() & 3) && near_cover)
			{
				item->required_anim_state = LONDSEC_DUCK;
				item->goal_anim_state = LONDSEC_WAIT;
			}

			if (info.distance < 0x400000)
				item->required_anim_state = LONDSEC_WALK;

			break;

		case LONDSEC_DUCKED:

			if (info.ahead)
				head = info.angle;

			sec->maximum_turn = 0;

			if (Targetable(item, &info))
				item->goal_anim_state = LONDSEC_DUCKAIM;
			else if (item->hit_status || !near_cover || info.ahead && !(GetRandomControl() & 0x1F))
				item->goal_anim_state = LONDSEC_STAND;
			else
				item->goal_anim_state = LONDSEC_DUCKWALK;

			break;

		case LONDSEC_DUCKAIM:
			sec->maximum_turn = 182;

			if (info.ahead)
				torso_y = info.angle;

			if (Targetable(item, &info))
				item->goal_anim_state = LONDSEC_DUCKSHOT;
			else
				item->goal_anim_state = LONDSEC_DUCKED;

			break;

		case LONDSEC_DUCKSHOT:

			if (info.ahead)
				torso_y = info.angle;

			if (item->frame_number == anims[item->anim_number].frame_base)
			{
				if (!ShotLara(item, &info, &londsec_gun, torso_y, 32) || !(GetRandomControl() & 7))
					item->goal_anim_state = LONDSEC_DUCKED;
			}

			break;

		case LONDSEC_DUCKWALK:

			if (info.ahead)
				head = info.angle;

			sec->maximum_turn = 910;

			if (Targetable(item, &info) || item->hit_status || !near_cover || info.ahead && !(GetRandomControl() & 0x1F))
				item->goal_anim_state = LONDSEC_DUCKED;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
