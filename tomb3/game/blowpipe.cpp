#include "../tomb3/pch.h"
#include "blowpipe.h"
#include "box.h"
#include "people.h"
#include "../specific/game.h"
#include "sound.h"
#include "effects.h"
#include "items.h"
#include "objects.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "effect2.h"
#include "control.h"
#include "lara.h"

static BITE_INFO blow_biff_hit = { 0, 0, -200, 13 };
static BITE_INFO blow_shoot_hit = { 8, 40, -248, 13 };

static void BlowpipeDart(ITEM_INFO* item)	//NEW FUNCTION, NOT IN ORIGINAL CODE!
{
	ITEM_INFO* dart;
	PHD_VECTOR pos;
	PHD_VECTOR pos1;
	long lp;
	short dart_num;
	short angles[2];

	dart_num = CreateItem();

	if (dart_num == NO_ITEM)
		return;

	dart = &items[dart_num];
	dart->object_number = DARTS;
	dart->room_number = item->room_number;

	pos.x = blow_shoot_hit.x;
	pos.y = blow_shoot_hit.y;
	pos.z = blow_shoot_hit.z;
	GetJointAbsPosition(item, &pos, blow_shoot_hit.mesh_num);

	pos1.x = blow_shoot_hit.x;
	pos1.y = blow_shoot_hit.y;
	pos1.z = blow_shoot_hit.z << 1;
	GetJointAbsPosition(item, &pos1, blow_shoot_hit.mesh_num);

	phd_GetVectorAngles(pos1.x - pos.x, pos1.y - pos.y, pos1.z - pos.z, angles);
	dart->pos.x_pos = pos.x;
	dart->pos.y_pos = pos.y;
	dart->pos.z_pos = pos.z;
	InitialiseItem(dart_num);
	dart->pos.x_rot = angles[1];
	dart->pos.y_rot = angles[0];
	dart->speed = 256;
	AddActiveItem(dart_num);
	dart->status = ITEM_ACTIVE;

	pos.x = blow_shoot_hit.x;
	pos.y = blow_shoot_hit.y;
	pos.z = blow_shoot_hit.z + 96;
	GetJointAbsPosition(item, &pos, blow_shoot_hit.mesh_num);

	for (lp = 0; lp < 2; lp++)
		TriggerDartSmoke(pos.x, pos.y, pos.z, 0, 0, 1);
}

void BlowpipeControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* blow;
	AI_INFO info;
	short angle, tilt, torso_x, torso_y, head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	blow = (CREATURE_INFO*)item->data;
	angle = 0;
	tilt = 0;
	torso_x = 0;
	torso_y = 0;
	head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != BLOW_DEATH)
		{
			if (item->current_anim_state == 1 || item->current_anim_state == 4)
				item->anim_number = objects[item->object_number].anim_index + 21;
			else
				item->anim_number = objects[item->object_number].anim_index + 20;

			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = BLOW_DEATH;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(blow);

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, info.zone_number == info.enemy_zone);

		if (item->hit_status && lara.poisoned >= 256 && blow->mood == BORED_MOOD)
			blow->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, blow->mood == BORED_MOOD ? 364 : blow->maximum_turn);

		if (info.ahead)
		{
			head = info.angle >> 1;
			torso_y = info.angle >> 1;
		}

		if (item->hit_status || (blow->enemy == lara_item && (info.distance < 1024 || TargetVisible(item, &info)) && (abs(lara_item->pos.y_pos - item->pos.y_pos) < 2048)))
			AlertAllGuards(item_number);

		switch (item->current_anim_state)
		{
		case BLOW_WAIT1:

			if (info.ahead)
			{
				torso_x = info.x_angle >> 1;
				torso_y = info.angle;
			}

			blow->flags &= 0xFFF;
			blow->maximum_turn = 364;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(blow);
				torso_x = 0;
				torso_y = 0;
				blow->maximum_turn = 0;

				if (!(GetRandomControl() & 0xFF))
					item->goal_anim_state = BLOW_WAIT2;
			}
			else if (blow->mood == ESCAPE_MOOD)
			{
				if (lara.target == item || !info.ahead || item->hit_status)
					item->goal_anim_state = BLOW_RUN;
				else
					item->goal_anim_state = BLOW_WAIT1;
			}
			else if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = BLOW_WAIT2;
			else if (info.bite && info.distance < 0x400000)
				item->goal_anim_state = BLOW_WALK;
			else if (Targetable(item, &info) && info.distance < 0x4000000)
				item->goal_anim_state = BLOW_ATTACK1;
			else if (blow->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 512)
					item->goal_anim_state = BLOW_WALK;
			}
			else
				item->goal_anim_state = BLOW_RUN;

			break;

		case BLOW_WALK:
			blow->maximum_turn = 1638;

			if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = BLOW_WAIT2;
			else if (info.bite && info.distance < 0x400000)
				item->goal_anim_state = BLOW_WALK;
			else if (Targetable(item, &info) && info.distance < 0x4000000)
				item->goal_anim_state = BLOW_WAIT1;
			else if (blow->mood == ESCAPE_MOOD)
				item->goal_anim_state = BLOW_RUN;
			else if (blow->mood == BORED_MOOD)
			{
				if (GetRandomControl() > 512)
					item->goal_anim_state = BLOW_WALK;
				else if (GetRandomControl() > 512)
					item->goal_anim_state = BLOW_WAIT2;
				else
					item->goal_anim_state = BLOW_WAIT1;
			}
			else if (info.distance > 0x400000)
				item->goal_anim_state = BLOW_RUN;

			break;

		case BLOW_RUN:
			blow->flags &= 0xFFF;
			blow->maximum_turn = 1092;
			tilt = angle >> 2;

			if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = BLOW_WAIT2;
			else if (Targetable(item, &info) && info.distance < 0x4000000)
				item->goal_anim_state = BLOW_WAIT1;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = BLOW_WAIT2;
			else if (blow->mood == ESCAPE_MOOD && lara.target != item && info.ahead)
				item->goal_anim_state = BLOW_WAIT2;
			else if (blow->mood == BORED_MOOD)
				item->goal_anim_state = BLOW_WAIT1;

			break;

		case BLOW_ATTACK1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			blow->maximum_turn = 0;

			if (abs(info.angle) < 364)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 364;
			else
				item->pos.y_rot += 364;

			if (item->frame_number == anims[item->anim_number].frame_base + 15)
				BlowpipeDart(item);

			item->goal_anim_state = BLOW_WAIT1;
			break;

		case BLOW_ATTACK3:
			enemy = blow->enemy;

			if (enemy == lara_item)
			{
				if (!(blow->flags & 0xF000) && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 100;
					lara_item->hit_status = 1;
					blow->flags |= 0x1000;
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					CreatureEffect(item, &blow_biff_hit, DoBloodSplat);
				}
			}
			else if (!(blow->flags & 0xF000) && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 512 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) < 512 &&
					abs(enemy->pos.z_pos - item->pos.z_pos))
				{
					enemy->hit_points -= 5;
					enemy->hit_status = 1;
					blow->flags |= 0x1000;
					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}

			break;

		case BLOW_AIM3:

			if (!info.bite || info.distance > 0x40000)
				item->goal_anim_state = BLOW_WAIT2;
			else
				item->goal_anim_state = BLOW_ATTACK3;

			break;

		case BLOW_WAIT2:
			blow->flags &= 0xFFF;
			blow->maximum_turn = 364;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(blow);
				torso_x = 0;
				torso_y = 0;
				blow->maximum_turn = 0;

				if (!(GetRandomControl() & 0xFF))
					item->goal_anim_state = BLOW_WAIT1;
			}
			else if (blow->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = BLOW_WAIT1;
				else
					item->goal_anim_state = BLOW_RUN;
			}
			else if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = BLOW_ATTACK3;
			else if (info.bite && info.distance < 0x400000)
				item->goal_anim_state = BLOW_WALK;
			else if (Targetable(item, &info) && info.distance < 0x4000000)
				item->goal_anim_state = BLOW_WAIT1;
			else if (blow->mood == BORED_MOOD && GetRandomControl() < 512)
				item->goal_anim_state = BLOW_WALK;
			else
				item->goal_anim_state = BLOW_RUN;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head - torso_y);
	CreatureJoint(item, 3, 0);
	CreatureAnimation(item_number, angle, 0);
}
