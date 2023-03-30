#include "../tomb3/pch.h"
#include "lizman.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/phd_math.h"
#include "box.h"
#include "people.h"
#include "effects.h"
#include "triboss.h"
#include "control.h"
#include "lara.h"

static BITE_INFO lizman_bite_hit = { 0, -120, 120, 10 };
static BITE_INFO lizman_swipe_hit = { 0, 0, 0, 5 };
static BITE_INFO lizman_gas = { 0, -64, 56, 9 };

static void TriggerLizmanGas(long x, long y, long z, long xv, long yv, long zv, long FxObj)
{
	SPARKS* sptr;
	long size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = (GetRandomControl() & 0x3F) + 128;
	sptr->sB = 32;
	sptr->dR = 0;
	sptr->dG = (GetRandomControl() & 0xF) + 32;
	sptr->dB = 0;

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

static short TriggerLizmanGasThrower(ITEM_INFO* item, BITE_INFO* bite, short speed)
{
	FX_INFO* fx;
	PHD_VECTOR pos;
	PHD_VECTOR pos1;
	long s, r, xv, yv, zv, lp;
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
	pos1.y = bite->y << 3;
	pos1.z = bite->z << 2;
	GetJointAbsPosition(item, &pos1, bite->mesh_num);

	phd_GetVectorAngles(pos1.x - pos.x, pos1.y - pos.y, pos1.z - pos.z, angles);

	fx->pos.x_pos = pos.x;
	fx->pos.y_pos = pos.y;
	fx->pos.z_pos = pos.z;
	fx->room_number = item->room_number;
	fx->pos.x_rot = angles[1];
	fx->pos.z_rot = 0;
	fx->pos.y_rot = angles[0];
	fx->speed = speed << 2;
	fx->object_number = DRAGON_FIRE;
	fx->counter = 20;
	fx->flag1 = 1;
	TriggerLizmanGas(0, 0, 0, 0, 0, 0, fxNum);

	for (lp = 0; lp < 2; lp++)
	{
		s = GetRandomControl() % (speed << 2) + 32;
		r = (s * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
		xv = (r * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
		yv = -((s * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
		zv = (r * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
		TriggerLizmanGas(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -1);
	}

	r = ((speed << 1) * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	xv = (r * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	yv = -(((speed << 1) * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	zv = (r * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
	TriggerLizmanGas(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -2);

	return fxNum;
}

void LizManControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* lizman;
	AI_INFO info;
	short tilt, angle, neck, f;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	lizman = (CREATURE_INFO*)item->data;
	tilt = 0;
	angle = 0;
	neck = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != LIZMAN_DEATH)
		{
			item->anim_number = objects[LIZARD_MAN].anim_index + 26;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LIZMAN_DEATH;
		}
		else if (lizard_man_active && item->frame_number - anims[item->anim_number].frame_base == 50)
		{
			CreatureDie(item_number, 1);
			lizard_man_active = 0;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (boxes[lizman->enemy->box_number].overlap_index & 0x8000)
			lizman->mood = ATTACK_MOOD;

		angle = CreatureTurn(item, lizman->maximum_turn);

		switch (item->current_anim_state)
		{
		case LIZMAN_STOP:
			lizman->flags = 0;

			if (info.ahead)
				neck = info.angle;

			lizman->maximum_turn = 0;

			if (lizman->mood == ESCAPE_MOOD)
				item->goal_anim_state = LIZMAN_RUN;
			else if (lizman->mood == BORED_MOOD)
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (GetRandomControl() < 0x4000)
					item->goal_anim_state = LIZMAN_WALK;
				else
					item->goal_anim_state = LIZMAN_WAIT;
			}
			else if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = LIZMAN_AIM1;
			else if (Targetable(item, &info) && info.distance < 0x640000 && (lara.poisoned < 256 || boxes[lizman->enemy->box_number].overlap_index & 0x8000))
				item->goal_anim_state = LIZMAN_AIM0;
			else
				item->goal_anim_state = LIZMAN_RUN;

			break;

		case LIZMAN_WALK:

			if (info.ahead)
				neck = info.angle;

			if (item->anim_number == objects[LIZARD_MAN].anim_index + 23 || item->anim_number == objects[LIZARD_MAN].anim_index + 31)
				lizman->maximum_turn = 0;
			else
				lizman->maximum_turn = 1820;

			if (lizman->mood == ESCAPE_MOOD)
				item->goal_anim_state = LIZMAN_RUN;
			else if (lizman->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
				{
					item->required_anim_state = LIZMAN_WAIT;
					item->goal_anim_state = LIZMAN_STOP;
				}
			}
			else if (info.bite && info.distance < 0x90000)
				item->goal_anim_state = LIZMAN_STOP;
			else if (info.bite && info.distance < 0x240000)
				item->goal_anim_state = LIZMAN_AIM2;
			else if (Targetable(item, &info) && info.distance < 0x640000 && (lara.poisoned < 256 || boxes[lizman->enemy->box_number].overlap_index & 0x8000))
				item->goal_anim_state = LIZMAN_STOP;
			else if (info.distance > 0x400000)
				item->goal_anim_state = LIZMAN_RUN;

			break;

		case LIZMAN_PUNCH2:

			if (info.ahead)
				neck = info.angle;

			if (lizman->flags != 2 && item->touch_bits & 0xC00)
			{
				lara_item->hit_points -= 100;
				lara_item->hit_status = 1;
				CreatureEffect(item, &lizman_bite_hit, DoBloodSplat);
				lizman->flags = 2;
			}

			break;

		case LIZMAN_AIM2:

			if (info.ahead)
				neck = info.angle;

			lizman->maximum_turn = 1820;
			lizman->flags = 0;

			if (info.bite && info.distance < 0x240000)
				item->goal_anim_state = LIZMAN_PUNCH2;
			else
				item->goal_anim_state = LIZMAN_WALK;

			break;

		case LIZMAN_WAIT:

			if (info.ahead)
				neck = info.angle;

			lizman->maximum_turn = 0;

			if (lizman->mood != BORED_MOOD)
				item->goal_anim_state = LIZMAN_STOP;
			else if (GetRandomControl() < 256)
			{
				item->required_anim_state = LIZMAN_WALK;
				item->goal_anim_state = LIZMAN_STOP;
			}

			break;

		case LIZMAN_AIM1:

			if (info.ahead)
				neck = info.angle;

			lizman->maximum_turn = 1820;
			lizman->flags = 0;

			if (info.ahead && info.distance < 0x90000)
				item->goal_anim_state = LIZMAN_PUNCH1;
			else
				item->goal_anim_state = LIZMAN_STOP;
			
			break;

		case LIZMAN_AIM0:

			if (info.ahead)
				neck = info.angle;

			lizman->maximum_turn = 0;

			if (abs(info.angle) < 728)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 728;
			else
				item->pos.y_rot += 728;

			if (info.bite && info.distance < 0x640000 && (lara.poisoned < 256 || boxes[lizman->enemy->box_number].overlap_index & 0x8000))
				item->goal_anim_state = LIZMAN_PUNCH0;
			else
				item->goal_anim_state = LIZMAN_STOP;

			break;

		case LIZMAN_PUNCH1:
			if (info.ahead)
				neck = info.angle;

			if (!lizman->flags && item->touch_bits & 0x20)
			{
				lara_item->hit_points -= 120;
				lara_item->hit_status = 1;
				CreatureEffect(item, &lizman_swipe_hit, DoBloodSplat);
				lizman->flags = 1;
			}

			if (info.distance < 0x240000)
				item->goal_anim_state = LIZMAN_PUNCH2;

			break;

		case LIZMAN_PUNCH0:

			if (info.ahead)
				neck = info.angle;

			if (abs(info.angle) < 728)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 728;
			else
				item->pos.y_rot += 728;

			if (item->frame_number >= anims[item->anim_number].frame_base + 7 && item->frame_number <= anims[item->anim_number].frame_base + 28)
			{
				if (lizman->flags < 24)
					lizman->flags += 2;

				if (lizman->flags < 24)
					f = lizman->flags;
				else
					f = (GetRandomControl() & 0xF) + 8;

				TriggerLizmanGasThrower(item, &lizman_gas, f);
			}

			if (item->frame_number > anims[item->anim_number].frame_base + 28)
				lizman->flags = 0;

			break;

		case LIZMAN_RUN:

			if (info.ahead)
				neck = info.angle;

			lizman->maximum_turn = 728;
			tilt = angle / 2;

			if (lizman->mood != ESCAPE_MOOD)
			{
				if (lizman->mood == BORED_MOOD)
					item->goal_anim_state = LIZMAN_WALK;
				else if (info.bite && info.distance < 0x90000)
					item->goal_anim_state = LIZMAN_STOP;
				else if (Targetable(item, &info) && info.distance < 0x640000 && (lara.poisoned < 256 || boxes[lizman->enemy->box_number].overlap_index & 0x8000))
					item->goal_anim_state = LIZMAN_STOP;
				else if (info.ahead && info.distance < 0x400000)
					item->goal_anim_state = LIZMAN_WALK;
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, 0);
	CreatureJoint(item, 1, neck);

	if (item->current_anim_state >= LIZMAN_DEATH)
		CreatureAnimation(item_number, angle, 0);
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			lizman->maximum_turn = 0;
			item->anim_number = objects[LIZARD_MAN].anim_index + 30;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LIZMAN_FALL3;
			break;

		case 2:
			lizman->maximum_turn = 0;
			item->anim_number = objects[LIZARD_MAN].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LIZMAN_CLIMB1;
			break;

		case 3:
			lizman->maximum_turn = 0;
			item->anim_number = objects[LIZARD_MAN].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LIZMAN_CLIMB2;
			break;

		case 4:
			lizman->maximum_turn = 0;
			item->anim_number = objects[LIZARD_MAN].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LIZMAN_CLIMB3;
			break;
		}
	}
}
