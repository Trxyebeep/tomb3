#include "../tomb3/pch.h"
#include "autogun.h"
#include "objects.h"
#include "../specific/game.h"
#include "../3dsystem/3d_gen.h"
#include "sphere.h"
#include "effect2.h"
#include "box.h"
#include "missile.h"
#include "lot.h"
#include "items.h"
#include "control.h"
#include "people.h"
#include "sound.h"

static BITE_INFO autogun_left = { 110, -30, -530, 2 };
static BITE_INFO autogun_right = { -110, -30, -530, 2 };

void InitialiseAutogun(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[ROBOT_SENTRY_GUN].anim_index + 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = AUTOGUN_STILL;
	item->goal_anim_state = AUTOGUN_STILL;
	item->item_flags[0] = 0;
}

void AutogunControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* gun;
	AI_INFO info;
	PHD_VECTOR pos;
	short tilt, diff;

	item = &items[item_number];

	if (item->fired_weapon > 1)
	{
		item->fired_weapon--;

		if (GetRandomControl() & 1)
		{
			objects[item->object_number].bite_offset = AUTOGUN_LEFT_BITE;

			phd_PushMatrix();
			pos.x = autogun_left.x;
			pos.y = autogun_left.y;
			pos.z = autogun_left.z;
			GetJointAbsPosition(item, &pos, autogun_left.mesh_num);
		}
		else
		{
			objects[item->object_number].bite_offset = AUTOGUN_RIGHT_BITE;

			phd_PushMatrix();
			pos.x = autogun_right.x;
			pos.y = autogun_right.y;
			pos.z = autogun_right.z;
			GetJointAbsPosition(item, &pos, autogun_right.mesh_num);
		}

		TriggerDynamic(pos.x, pos.y, pos.z, 2 * item->fired_weapon + 8, 192, 128, 32);
		phd_PopMatrix();
	}

	if (!CreatureActive(item_number))
		return;

	gun = (CREATURE_INFO*)item->data;

	if (!gun)
		return;

	if (item->hit_status)
		item->really_active = 1;

	if (item->hit_points <= 0)
	{
		ExplodingDeath(item_number, -1, 0);
		DisableBaddieAI(item_number);
		KillItem(item_number);
		item->flags |= IFL_INVISIBLE;
		item->status = ITEM_DEACTIVATED;
	}

	if (!item->really_active)
		return;

	CreatureAIInfo(item, &info);
	tilt = -info.x_angle;

	switch (item->current_anim_state)
	{
	case AUTOGUN_FIRE:

		if (!Targetable(item, &info))
			item->goal_anim_state = AUTOGUN_STILL;
		else if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->item_flags[0] = 1;

			if (objects[item->object_number].bite_offset == AUTOGUN_RIGHT_BITE)
				ShotLara(item, &info, &autogun_left, gun->joint_rotation[0], 10);
			else
				ShotLara(item, &info, &autogun_right, gun->joint_rotation[0], 10);

			item->fired_weapon = 10;
			SoundEffect(SFX_LARA_UZI_STOP, &item->pos, SFX_DEFAULT);
		}

		break;

	case AUTOGUN_STILL:

		if (Targetable(item, &info) && !item->item_flags[0])
			item->goal_anim_state = AUTOGUN_FIRE;
		else if (item->item_flags[0])
		{
			if (item->ai_bits == MODIFY)
			{
				item->item_flags[0] = 1;
				item->goal_anim_state = AUTOGUN_FIRE;
			}
			else
			{
				item->really_active = 0;
				item->item_flags[0] = 0;
			}
		}

		break;
	}

	diff = info.angle - gun->joint_rotation[0];

	if (diff > 1820)
		diff = 1820;
	else if (diff < -1820)
		diff = -1820;

	gun->joint_rotation[0] += diff;
	CreatureJoint(item, 1, tilt);
	AnimateItem(item);

	if (info.angle > 0x4000)
	{
		item->pos.y_rot += 0x8000;

		if (info.angle > 0 || info.angle < 0)
			gun->joint_rotation[0] += 0x8000;
	}
	else if (info.angle < -0x4000)
	{
		item->pos.y_rot += 0x8000;

		if (info.angle > 0 || info.angle < 0)
			gun->joint_rotation[0] += 0x8000;
	}
}
