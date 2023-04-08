#include "../tomb3/pch.h"
#include "flamer.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/phd_math.h"
#include "box.h"
#include "gameflow.h"
#include "people.h"
#include "sound.h"
#include "../specific/smain.h"
#include "lot.h"
#include "control.h"
#include "lara.h"
#include "effects.h"

static BITE_INFO flamer_gun = { 0, 340, 64, 7 };

static void TriggerPilotFlame(short item_number)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - items[item_number].pos.x_pos;
	dz = lara_item->pos.z_pos - items[item_number].pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 0x80;
	sptr->dB = 32;
	sptr->FadeToBlack = 4;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) - 16;
	sptr->y = (GetRandomControl() & 0x1F) - 16;
	sptr->z = (GetRandomControl() & 0x1F) - 16;
	sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Yvel = -(GetRandomControl() & 3);
	sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = 0;
	sptr->Friction = 4;
	sptr->Gravity = -2 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 0;
	sptr->dWidth = (GetRandomControl() & 7) + 32;
	sptr->sWidth = sptr->dWidth >> 1;
	sptr->Width = sptr->sWidth;
	sptr->sHeight = sptr->sWidth;
	sptr->Height = sptr->sWidth;
	sptr->dHeight = sptr->dWidth;
}

static void TriggerFlamethrowerFlame(long x, long y, long z, long xv, long yv, long zv, long fxnum)
{
	SPARKS* sptr;
	uchar size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 0x80;
	sptr->dB = 32;

	if (xv || yv || zv)
	{
		sptr->ColFadeSpeed = 6;
		sptr->FadeToBlack = 2;
		sptr->Life = (GetRandomControl() & 1) + 12;
	}
	else
	{
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 16;
		sptr->Life = (GetRandomControl() & 3) + 20;
	}

	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = short((GetRandomControl() & 0xF) + xv - 16);
	sptr->Yvel = (short)yv;
	sptr->Zvel = short((GetRandomControl() & 0xF) + zv - 16);
	sptr->Friction = 0;

	if (GetRandomControl() & 1)
	{
		if (fxnum < 0)
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (fxnum < 0)
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;

	sptr->MaxYvel = 0;
	sptr->FxObj = (uchar)fxnum;
	sptr->Gravity = 0;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	size = (GetRandomControl() & 0x1F) + 64;

	if (xv || yv || zv)
	{
		sptr->Width = size >> 5;
		sptr->sWidth = size >> 5;
		sptr->Height = size >> 5;
		sptr->sHeight = size >> 5;

		if (fxnum == -2)
			sptr->Scalar = 2;
		else
			sptr->Scalar = 3;

		sptr->dWidth = size >> 1;
		sptr->dHeight = size >> 1;
	}
	else
	{
		sptr->Scalar = 4;
		sptr->Width = size >> 4;
		sptr->sWidth = size >> 4;
		sptr->Height = size >> 4;
		sptr->sHeight = size >> 4;
		sptr->dWidth = size >> 1;
		sptr->dHeight = size >> 1;
	}
}

static short TriggerFlameThrower(ITEM_INFO* item, BITE_INFO* bite, short speed)
{
	FX_INFO* fx;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	long xv, yv, zv, vel, spd;
	short angles[2];
	short fx_number;

	fx_number = CreateEffect(item->room_number);

	if (fx_number != NO_ITEM)
	{
		fx = &effects[fx_number];
		pos1.x = bite->x;
		pos1.y = bite->y;
		pos1.z = bite->z;
		GetJointAbsPosition(item, &pos1, bite->mesh_num);
		pos2.x = bite->x;
		pos2.y = bite->y << 1;
		pos2.z = bite->z;
		GetJointAbsPosition(item, &pos2, bite->mesh_num);
		phd_GetVectorAngles(pos2.x - pos1.x, pos2.y - pos1.y, pos2.z - pos1.z, angles);
		fx->pos.x_pos = pos1.x;
		fx->pos.y_pos = pos1.y;
		fx->pos.z_pos = pos1.z;
		fx->room_number = item->room_number;
		fx->pos.x_rot = angles[1];
		fx->pos.y_rot = angles[0];
		fx->pos.z_rot = 0;
		fx->speed = speed << 2;
		fx->object_number = DRAGON_FIRE;
		fx->counter = 20;
		fx->flag1 = 0;
		TriggerFlamethrowerFlame(0, 0, 0, 0, 0, 0, fx_number);

		for (int i = 0; i < 2; i++)
		{
			spd = (GetRandomControl() % (speed << 2)) + 32;
			vel = (spd * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
			zv = (vel * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
			yv = -((spd * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
			xv = (vel * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
			TriggerFlamethrowerFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -1);
		}

		vel = ((speed << 1) * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
		zv = (vel * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
		yv = -(((speed << 1) * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
		xv = (vel * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
		TriggerFlamethrowerFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -2);
	}

	return fx_number;
}

void FlamerControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* flamer;
	CREATURE_INFO* target;
	AI_INFO info;
	AI_INFO lara_info;
	PHD_VECTOR pos;
	long dx, dz, dist, best_dist, r, g, b;
	short rnd, torso_y, torso_x, angle, head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	flamer = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	pos.x = flamer_gun.x;
	pos.y = flamer_gun.y;
	pos.z = flamer_gun.z;
	GetJointAbsPosition(item, &pos, flamer_gun.mesh_num);
	rnd = (short)GetRandomControl();

	if (item->current_anim_state == 6 || item->current_anim_state == 11)
	{
		r = 255 - ((rnd >> 4) & 0x1F);
		g = 192 - ((rnd >> 6) & 0x1F);
		b = rnd & 0x3F;
		TriggerDynamic(pos.x, pos.y, pos.z, (rnd & 3) + 10, r, g, b);
	}
	else
	{
		r = 192 - ((rnd >> 4) & 0x1F);
		g = 128 - ((rnd >> 6) & 0x1F);
		b = rnd & 0x1F;
		TriggerDynamic(pos.x, pos.y, pos.z, (rnd & 3) + 6, r, g, b);
		TriggerPilotFlame(item_number);
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 7)
		{
			item->anim_number = objects[item->object_number].anim_index + 19;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 7;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(flamer);
		else if (flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER)
			flamer->enemy = lara_item;
		else
		{
			flamer->enemy = 0;
			best_dist = 0x7FFFFFFF;

			for (int i = 0; i < MAX_LOT; i++)
			{
				target = &baddie_slots[i];

				if (target->item_num == NO_ITEM || target->item_num == item_number)
					continue;

				enemy = &items[target->item_num];

				if (enemy->object_number == LARA ||
					enemy->object_number == WHITE_SOLDIER ||
					enemy->object_number == FLAMETHROWER_BLOKE ||
					enemy->hit_points <= 0)
					continue;

				dx = enemy->pos.x_pos - item->pos.x_pos;
				dz = enemy->pos.z_pos - item->pos.z_pos;
				dist = SQUARE(dx) + SQUARE(dz);

				if (dist < best_dist)
				{
					flamer->enemy = enemy;
					best_dist = dist;
				}
			}
		}

		CreatureAIInfo(item, &info);

		if (flamer->enemy == lara_item)
		{
			lara_info.angle = info.angle;
			lara_info.distance = info.distance;

			if (!flamer->hurt_by_lara && CurrentLevel != LV_CHAMBER)
				flamer->enemy = 0;
		}
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			info.x_angle -= 2048;
			lara_info.angle = short(phd_atan(dz, dx) - item->pos.y_rot);
			lara_info.distance = SQUARE(dx) + SQUARE(dz);
		}

		GetCreatureMood(item, &info, 0);
		CreatureMood(item, &info, 0);
		angle = CreatureTurn(item, flamer->maximum_turn);
		enemy = flamer->enemy;

		if (item->hit_status || (lara_info.distance < 0x100000 || TargetVisible(item, &lara_info)) && CurrentLevel == LV_CHAMBER)
		{
			if (!flamer->alerted)
				SoundEffect(SFX_AMERCAN_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		switch (item->current_anim_state)
		{
		case 1:
			head = lara_info.angle;
			flamer->flags = 0;
			flamer->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(flamer);

				if (!(GetRandomControl() & 0xFF))
					item->goal_anim_state = 4;
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (flamer->mood == ESCAPE_MOOD)
				item->goal_anim_state = 2;
			else if (Targetable(item, &info) && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER))
			{
				if (info.distance >= 0x1000000)
					item->goal_anim_state = 2;
				else
					item->goal_anim_state = 10;
			}
			else if (flamer->mood == BORED_MOOD && info.ahead && !(GetRandomControl() & 0xFF))
				item->goal_anim_state = 4;
			else if (flamer->mood == ATTACK_MOOD || !(GetRandomControl() & 0xFF))
				item->goal_anim_state = 2;

			break;

		case 2:
			head = lara_info.angle;
			flamer->flags = 0;
			flamer->maximum_turn = 910;

			if (item->ai_bits & GUARD)
			{
				item->anim_number = objects[item->object_number].anim_index + 12;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = 1;
				item->goal_anim_state = 1;
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (flamer->mood == ESCAPE_MOOD)
				item->goal_anim_state = 2;
			else if (Targetable(item, &info) && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER))
			{
				if (info.distance >= 0x1000000)
					item->goal_anim_state = 9;
				else
					item->goal_anim_state = 1;
			}
			else if (flamer->mood == BORED_MOOD && info.ahead)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 2;

			break;

		case 4:
			head = lara_info.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(flamer);

				if (!(GetRandomControl() & 0xFF))
					item->goal_anim_state = 1;
			}
			else if (Targetable(item, &info) && info.distance < 0x1000000 && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER) ||
				flamer->mood != BORED_MOOD)
				item->goal_anim_state = 1;

			break;

		case 6:

			if (flamer->flags < 40)
				flamer->flags += (flamer->flags >> 2) + 1;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info) && info.distance < 0x1000000 && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER))
					item->goal_anim_state = 6;
				else
					item->goal_anim_state = 2;
			}
			else
				item->goal_anim_state = 2;

			if (flamer->flags < 40)
				TriggerFlameThrower(item, &flamer_gun, flamer->flags);
			else
			{
				TriggerFlameThrower(item, &flamer_gun, (GetRandomControl() & 0x1F) + 12);

				if (enemy && enemy->object_number == BURNT_MUTANT)
					enemy->item_flags[0]++;
			}

			SoundEffect(SFX_FLAME_THROWER_LOOP, &item->pos, SFX_DEFAULT);
			break;

		case 9:
			flamer->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info) && info.distance < 0x1000000 && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER))
					item->goal_anim_state = 6;
				else
					item->goal_anim_state = 2;
			}

			break;

		case 10:
			flamer->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info) && info.distance < 0x1000000 && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER))
					item->goal_anim_state = 11;
				else
					item->goal_anim_state = 1;
			}

			break;

		case 11:

			if (flamer->flags < 40)
				flamer->flags += (flamer->flags >> 2) + 1;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;

				if (Targetable(item, &info) && info.distance < 0x1000000 && (enemy != lara_item || flamer->hurt_by_lara || CurrentLevel == LV_CHAMBER))
					item->goal_anim_state = 11;
				else
					item->goal_anim_state = 1;
			}
			else
				item->goal_anim_state = 1;

			if (flamer->flags < 40)
				TriggerFlameThrower(item, &flamer_gun, flamer->flags);
			else
			{
				TriggerFlameThrower(item, &flamer_gun, (GetRandomControl() & 0x1F) + 12);

				if (enemy && enemy->object_number == BURNT_MUTANT)
					enemy->item_flags[0]++;
			}

			SoundEffect(SFX_FLAME_THROWER_LOOP, &item->pos, SFX_DEFAULT);
			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
