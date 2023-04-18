#include "../tomb3/pch.h"
#include "51civvy.h"
#include "effect2.h"
#include "../specific/game.h"
#include "control.h"
#include "gameflow.h"
#include "../specific/smain.h"
#include "box.h"
#include "objects.h"
#include "effects.h"
#include "../3dsystem/phd_math.h"
#include "people.h"
#include "sound.h"
#include "lara.h"

static BITE_INFO civvy_hit = { 0, 0, 0, 13 };

static void TriggerFenceSparks(long x, long y, long z, long kill)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = (GetRandomControl() & 0x3F) + 192;
	sptr->sB = sptr->sG;
	sptr->dR = 0;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = sptr->dB;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 16;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = ((GetRandomControl() & 0xFF) << 1) - 256;
	sptr->Yvel = short((GetRandomControl() & 0xF) - (kill << 5) - 8);
	sptr->Zvel = ((GetRandomControl() & 0xFF) << 1) - 256;
	sptr->Friction = 4;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 2;
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 0xF) + 16;

	sptr->Width = 1;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->sWidth;
	sptr->Height = (GetRandomControl() & 7) + 6;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->sHeight;
}

void ControlElectricFence(short item_number)
{
	ITEM_INFO* item;
	long dx, dz, x, z, ex, ey, ez, xa, za, xs, zs, num, num2, oex, oez;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x5000 || dx > 0x5000 || dz < -0x5000 || dz > 0x5000)
		return;

	if (!item->pos.y_rot)
	{
		x = item->pos.x_pos + 512;
		z = item->pos.z_pos + 512;
		ex = x - 992;
		ez = z;

		if (CurrentLevel == LV_OFFICE)
		{
			xa = GetRandomControl() & 0x3FF;
			ez += xa * -(GetRandomControl() & 1);
		}

		xa = 1024;
		za = 0;
		xs = 1056;
		zs = 128;
	}
	else if (item->pos.y_rot == -0x4000)
	{
		x = item->pos.x_pos - 512;
		z = item->pos.z_pos + 512;
		ex = x;
		ez = z - 992;

		if (CurrentLevel == LV_OFFICE)
		{
			xa = GetRandomControl() & 0x3FF;
			ex += xa * -(GetRandomControl() & 1);
		}

		xa = 0;
		za = 1024;
		xs = 128;
		zs = 1056;
	}
	else if (item->pos.y_rot == -0x8000)
	{
		x = item->pos.x_pos - 512;
		z = item->pos.z_pos - 512;
		ex = x - 992;
		ez = z;

		if (CurrentLevel == LV_OFFICE)
		{
			xa = GetRandomControl() & 0x3FF;
			ez += xa * -(GetRandomControl() & 1);
		}

		xa = 1024;
		za = 0;
		xs = 1056;
		zs = 128;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		x = item->pos.x_pos + 512;
		z = item->pos.z_pos - 512;
		ex = x;
		ez = z - 992;

		if (CurrentLevel == LV_OFFICE)
		{
			xa = GetRandomControl() & 0x3FF;
			ex += xa * -(GetRandomControl() & 1);
		}

		xa = 0;
		za = 1024;
		xs = 128;
		zs = 1056;
	}
	else
	{
		x = 0;
		z = 0;
		ex = 0;
		ez = 0;
		xa = 0;
		za = 0;
		xs = 0;
		zs = 0;
	}

	if (!(GetRandomControl() & 0x1F))
	{
		if (xa)
			ex += GetRandomControl() & xa;
		else
			ez += GetRandomControl() & za;

		if (CurrentLevel == LV_OFFICE)
			ey = item->pos.y_pos - (GetRandomControl() & 0x1F);
		else
			ey = item->pos.y_pos - (GetRandomControl() & 0x7FF) - (GetRandomControl() & 0x3FF);

		num = (GetRandomControl() & 3) + 3;

		for (int i = 0; i < num; i++)
		{
			TriggerFenceSparks(ex, ey, ez, 0);

			if (xa)
				ex += (xa & GetRandomControl() & 7) - 4;
			else
				ez += (za & GetRandomControl() & 7) - 4;

			ey += (GetRandomControl() & 7) - 4;
		}
	}

	if (lara.electric || CurrentLevel == LV_OFFICE ||
		lara_item->pos.x_pos < x - xs || lara_item->pos.x_pos > x + xs ||
		lara_item->pos.z_pos < z - zs || lara_item->pos.z_pos > z + zs ||
		lara_item->pos.y_pos > item->pos.y_pos + 32 || lara_item->pos.y_pos < item->pos.y_pos - 3072)
		return;

	oex = ex;
	oez = ez;
	num = (GetRandomControl() & 0xF) + 3;

	for (int i = 0; i < num; i++)
	{
		if (xa)
			ex = (GetRandomControl() & 0x1FF) + lara_item->pos.x_pos - 256;
		else
			ez = (GetRandomControl() & 0x1FF) + lara_item->pos.z_pos - 256;

		ey = lara_item->pos.y_pos - GetRandomControl() % 768;
		num2 = (GetRandomControl() & 3) + 6;

		for (int j = 0; j < num2; j++)
		{
			TriggerFenceSparks(ex, ey, ez, 1);

			if (xa)
				ex += (xa & GetRandomControl() & 7) - 4;
			else
				ez += (za & GetRandomControl() & 7) - 4;

			ey += (GetRandomControl() & 7) - 4;
		}

		ex = oex;
		ez = oez;
	}

	lara.electric = 1;
	lara_item->hit_points = 0;
}

void InitialiseCivvy(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[CIVVIE].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = CIVVY_STOP;
	item->goal_anim_state = CIVVY_STOP;
}

void CivvyControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* civvy;
	AI_INFO info;
	AI_INFO larainfo;
	long x, z;
	short tilt, angle, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	civvy = (CREATURE_INFO*)item->data;
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
		if (item->current_anim_state != CIVVY_DEATH)
		{
			item->anim_number = objects[CIVVIE].anim_index + 26;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CIVVY_DEATH;
			civvy->LOT.step = 256;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(civvy);
		else
			civvy->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (civvy->enemy == lara_item)
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

		if (civvy->enemy == lara_item && info.distance > 0x900000 && info.enemy_facing < 0x3000 && info.enemy_facing > -0x3000)
			civvy->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, civvy->maximum_turn);

		enemy = civvy->enemy;
		civvy->enemy = lara_item;

		if ((larainfo.distance < 0x100000 || item->hit_status || TargetVisible(item, &larainfo)) && !(item->ai_bits & FOLLOW))
		{
			if (!civvy->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		civvy->enemy = enemy;

		switch (item->current_anim_state)
		{
		case CIVVY_WAIT:

			if (civvy->alerted || item->goal_anim_state == CIVVY_RUN)
			{
				item->goal_anim_state = CIVVY_STOP;
				break;
			}

		case CIVVY_STOP:
			civvy->flags = 0;
			civvy->maximum_turn = 0;
			head = larainfo.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(civvy);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == CIVVY_STOP)
						item->goal_anim_state = CIVVY_WAIT;
					else
						item->goal_anim_state = CIVVY_STOP;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = CIVVY_WALK;
			else if (civvy->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = CIVVY_STOP;
				else
					item->goal_anim_state = CIVVY_RUN;
			}
			else if (civvy->mood == BORED_MOOD || item->ai_bits & FOLLOW && (civvy->reached_goal || larainfo.distance > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = CIVVY_STOP;
				else
					item->goal_anim_state = CIVVY_RUN;
			}
			else if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = CIVVY_AIM0;
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = CIVVY_AIM1;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = CIVVY_WALK;
			else
				item->goal_anim_state = CIVVY_RUN;

			break;

		case CIVVY_WALK:
			head = larainfo.angle;
			civvy->maximum_turn = 910;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = CIVVY_WALK;
				head = 0;
			}
			else if (civvy->mood == ESCAPE_MOOD)
				item->goal_anim_state = CIVVY_RUN;
			else if (civvy->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
				{
					item->required_anim_state = CIVVY_WAIT;
					item->goal_anim_state = CIVVY_STOP;
				}
			}
			else if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = CIVVY_STOP;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = CIVVY_AIM2;
			else
				item->goal_anim_state = CIVVY_RUN;

			break;

		case CIVVY_PUNCH2:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			civvy->maximum_turn = 910;

			if (civvy->flags != 2 && item->touch_bits & 0x2400)
			{
				lara_item->hit_points -= 50;
				lara_item->hit_status = 1;
				CreatureEffect(item, &civvy_hit, DoBloodSplat);
				SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				civvy->flags = 2;
			}

			break;

		case CIVVY_AIM2:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			civvy->maximum_turn = 910;
			civvy->flags = 0;

			if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = CIVVY_PUNCH2;
			else
				item->goal_anim_state = CIVVY_WALK;

			break;

		case CIVVY_AIM1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			civvy->maximum_turn = 910;
			civvy->flags = 0;

			if (info.ahead && info.distance < 0x718E4)
				item->goal_anim_state = CIVVY_PUNCH1;
			else
				item->goal_anim_state = CIVVY_STOP;

			break;

		case CIVVY_AIM0:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			civvy->maximum_turn = 910;
			civvy->flags = 0;

			if (info.bite && info.distance < 0x1C639)
				item->goal_anim_state = CIVVY_PUNCH0;
			else
				item->goal_anim_state = CIVVY_STOP;
			
			break;

		case CIVVY_PUNCH1:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			civvy->maximum_turn = 910;

			if (!civvy->flags && item->touch_bits & 0x2400)
			{
				lara_item->hit_points -= 40;
				lara_item->hit_status = 1;
				CreatureEffect(item, &civvy_hit, DoBloodSplat);
				SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				civvy->flags = 1;
			}

			if (info.ahead && info.distance > 0x718E4 && info.distance < 0x100000)
				item->goal_anim_state = CIVVY_PUNCH2;

			break;

		case CIVVY_PUNCH0:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			civvy->maximum_turn = 910;

			if (!civvy->flags && item->touch_bits & 0x2400)
			{
				lara_item->hit_points -= 40;
				lara_item->hit_status = 1;
				CreatureEffect(item, &civvy_hit, DoBloodSplat);
				SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				civvy->flags = 1;
			}

			break;

		case CIVVY_RUN:

			if (info.ahead)
				head = info.angle;

			civvy->maximum_turn = 1092;
			tilt = angle >> 1;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = CIVVY_WAIT;
			else if (civvy->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = CIVVY_STOP;
			}
			else if (item->ai_bits & FOLLOW && (civvy->reached_goal || larainfo.distance > 0x400000))
				item->goal_anim_state = CIVVY_STOP;
			else if (civvy->mood == BORED_MOOD)
				item->goal_anim_state = CIVVY_WALK;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = CIVVY_WALK;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (item->current_anim_state >= CIVVY_DEATH)
	{
		civvy->maximum_turn = 0;
		CreatureAnimation(item_number, angle, 0);
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			civvy->maximum_turn = 0;
			item->anim_number = objects[65].anim_index + 30;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CIVVY_FALL3;
			break;

		case 2:
			civvy->maximum_turn = 0;
			item->anim_number = objects[65].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CIVVY_CLIMB1;
			break;

		case 3:
			civvy->maximum_turn = 0;
			item->anim_number = objects[65].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CIVVY_CLIMB2;
			break;

		case 4:
			civvy->maximum_turn = 0;
			item->anim_number = objects[CIVVIE].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CIVVY_CLIMB3;
			break;
		}
	}
}
