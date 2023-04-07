#include "../tomb3/pch.h"
#include "sealmute.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/phd_math.h"
#include "box.h"
#include "people.h"
#include "lot.h"
#include "control.h"
#include "lara.h"
#include "effects.h"

static BITE_INFO seal_gas = { 0, 48, 140, 10 };

static void TriggerSealmuteGas(long x, long y, long z, long xv, long yv, long zv, long FxObj)
{
	SPARKS* sptr;
	long size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) + 128;
	sptr->sG = (GetRandomControl() & 0x3F) + 128;
	sptr->sB = 32;
	sptr->dR = (GetRandomControl() & 0xF) + 32;
	sptr->dG = (GetRandomControl() & 0xF) + 32;
	sptr->dB = 0;

	if (xv || yv || zv)
	{
		sptr->ColFadeSpeed = 6;
		sptr->FadeToBlack = 2;
		sptr->Life = (GetRandomControl() & 1) + 16;
	}
	else
	{
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 16;
		sptr->Life = (GetRandomControl() & 3) + 28;
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
		if (FxObj < 0)
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (FxObj < 0)
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;

	sptr->MaxYvel = 0;
	sptr->FxObj = (uchar)FxObj;
	sptr->Gravity = 0;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	size = (GetRandomControl() & 0x1F) + 48;

	if (xv || yv || zv)
	{
		sptr->Width = uchar(size >> 5);
		sptr->sWidth = sptr->Width;
		sptr->dWidth = uchar(size >> 1);
		sptr->Height = sptr->Width;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->dWidth;

		if (FxObj == -2)
			sptr->Scalar = 2;
		else
			sptr->Scalar = 3;
	}
	else
	{
		sptr->Scalar = 4;
		sptr->Width = uchar(size >> 4);
		sptr->sWidth = sptr->Width;
		sptr->dWidth = uchar(size >> 1);
		sptr->Height = sptr->Width;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->dWidth;
	}
}

static short TriggerSealmuteGasThrower(ITEM_INFO* item, BITE_INFO* bite, short speed)
{
	FX_INFO* fx;
	PHD_VECTOR pos;
	PHD_VECTOR pos1;
	long lp, s, r, xv, yv, zv;
	short fxNum;
	short angles[2];

	fxNum = CreateEffect(item->room_number);

	if (fxNum == NO_ITEM)
		return NO_ITEM;

	fx = &effects[fxNum];

	pos.x = bite->x;
	pos.y = bite->y;
	pos.z = bite->z;
	GetJointAbsPosition(item, &pos, bite->mesh_num);

	pos1.x = bite->x;
	pos1.y = bite->y << 1;
	pos1.z = bite->z << 3;
	GetJointAbsPosition(item, &pos1, bite->mesh_num);

	phd_GetVectorAngles(pos1.x - pos.x, pos1.y - pos.y, pos1.z - pos.z, angles);

	fx->pos.x_pos = pos.x;
	fx->pos.y_pos = pos.y;
	fx->pos.z_pos = pos.z;
	fx->pos.x_rot = angles[1];
	fx->pos.y_rot = angles[0];
	fx->pos.z_rot = 0;
	fx->room_number = item->room_number;
	fx->speed = speed << 2;
	fx->object_number = DRAGON_FIRE;
	fx->counter = 20;
	fx->flag1 = 1;

	TriggerSealmuteGas(0, 0, 0, 0, 0, 0, fxNum);

	for (lp = 0; lp < 2; lp++)
	{
		s = GetRandomControl() % (speed << 2) + 32;
		r = (s * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
		xv = (r * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
		yv = -((s * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
		zv = (r * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
		TriggerSealmuteGas(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -1);
	}

	r = ((speed << 1) * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	xv = (r * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	yv = -(((speed << 1) * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	zv = (r * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
	TriggerSealmuteGas(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -2);

	return fxNum;
}

void SealmuteControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* seal;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO larainfo;
	ulong f;
	long c, r, g, b, dx, dz, lp;
	short angle, head, torso_x, torso_y, speed;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	seal = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->item_flags[0] > 80)
		item->hit_points = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != SEAL_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 5;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = SEAL_DEATH;
			seal->flags = 0;
		}
		else if (item->item_flags[0] > 80)
		{
			for (lp = 9; lp < 17; lp++)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetJointAbsPosition(item, &pos, lp);
				TriggerFireFlame(pos.x, pos.y, pos.z, -1, 255);
			}

			c = item->frame_number - anims[item->anim_number].frame_base;

			if (c > 16)
			{
				c = anims[item->anim_number].frame_end - item->frame_number;

				if (c > 16)
					c = 16;
			}

			b = GetRandomControl();
			r = (c * (255 - ((b >> 4) & 0x1F))) >> 4;
			g = (c * (192 - ((b >> 6) & 0x3F))) >> 4;
			b = (c * (b & 0x3F)) >> 4;
			TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 12, r, g, b);
		}
		else if (item->frame_number >= anims[item->anim_number].frame_base + 1 && item->frame_number <= anims[item->anim_number].frame_end - 8)
		{
			f = item->frame_number - anims[item->anim_number].frame_base + 1;

			if (f > 24)
			{
				f = anims[item->anim_number].frame_end - item->frame_number - 8;

				if (f <= 0)
					f = 1;

				if (f > 24)
					f = (GetRandomControl() & 0xF) + 8;
			}

			TriggerSealmuteGasThrower(item, &seal_gas, (short)f);
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(seal);
		else
		{
			seal->enemy = lara_item;
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			larainfo.distance = SQUARE(dx) + SQUARE(dz);

			for (lp = 0; lp < MAX_LOT; lp++)
			{
				if (baddie_slots[lp].item_num == NO_ITEM || baddie_slots[lp].item_num == item_number)
					continue;

				enemy = &items[baddie_slots[lp].item_num];
				
				if ((enemy->object_number == LARA || enemy->object_number == FLAMETHROWER_BLOKE) && enemy->hit_points > 0)
				{
					dx = enemy->pos.x_pos - item->pos.x_pos;
					dz = enemy->pos.z_pos - item->pos.z_pos;

					if (SQUARE(dx) + SQUARE(dz) < larainfo.distance)
						seal->enemy = enemy;
				}
			}
		}

		CreatureAIInfo(item, &info);

		if (seal->enemy == lara_item)
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

		if (info.zone_number == info.enemy_zone)
		{
			GetCreatureMood(item, &info, 1);

			if (seal->enemy == lara_item && lara.poisoned >= 256)
				seal->mood = ESCAPE_MOOD;

			CreatureMood(item, &info, 1);
		}
		else
		{
			GetCreatureMood(item, &info, 0);

			if (seal->enemy == lara_item && lara.poisoned >= 256)
				seal->mood = ESCAPE_MOOD;

			CreatureMood(item, &info, 0);
		}

		angle = CreatureTurn(item, seal->maximum_turn);
		enemy = seal->enemy;
		seal->enemy = lara_item;

		if (larainfo.distance < 0x100000 || item->hit_status || TargetVisible(item, &larainfo))
			AlertAllGuards(item_number);

		seal->enemy = enemy;

		switch (item->current_anim_state)
		{
		case SEAL_STOP:
			seal->maximum_turn = 0;
			seal->flags = 0;
			head = info.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(seal);
				item->goal_anim_state = SEAL_STOP;
			}
			else if (item->ai_bits & PATROL1)
			{
				head = 0;
				item->goal_anim_state = SEAL_WALK;
			}
			else if (seal->mood == ESCAPE_MOOD)
				item->goal_anim_state = SEAL_WALK;
			else if (Targetable(item, &info) && info.distance < 0x400000)
				item->goal_anim_state = SEAL_BURP;
			else if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else
				item->goal_anim_state = SEAL_WALK;

			break;

		case SEAL_WALK:
			seal->maximum_turn = 546;

			if (info.ahead)
				head = info.angle;

			if (item->ai_bits & PATROL1)
			{
				head = 0;
				item->goal_anim_state = SEAL_WALK;
			}
			else if (Targetable(item, &info) && info.distance < 0x400000)
				item->goal_anim_state = SEAL_STOP;

			break;

		case SEAL_BURP:

			if (abs(info.angle) < 546)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 546;
			else
				item->pos.y_rot += 546;

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle >> 1;
			}

			if (item->frame_number >= anims[item->anim_number].frame_base + 35 && item->frame_number <= anims[item->anim_number].frame_base + 58)
			{
				if (seal->flags < 24)
					seal->flags += 3;

				if (seal->flags < 24)
					speed = seal->flags;
				else
					speed = (GetRandomControl() & 0xF) + 8;

				TriggerSealmuteGasThrower(item, &seal_gas, speed);

				if (seal->enemy != lara_item)
					seal->enemy->hit_status = 1;
			}

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_x);
	CreatureJoint(item, 1, torso_y);
	CreatureJoint(item, 2, head);
	CreatureAnimation(item_number, angle, 0);
}
