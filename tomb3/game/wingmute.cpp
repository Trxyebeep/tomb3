#include "../tomb3/pch.h"
#include "wingmute.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"
#include "effect2.h"
#include "effects.h"
#include "sphere.h"
#include "control.h"
#include "lara.h"

static BITE_INFO wing_bite = { 0, 0, 0, 12 };

static void TriggerWingMuteParticles(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long dx, dz;

	item = &items[item_number];
	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sG = (GetRandomControl() & 0x3F) + 32;
	sptr->sB = sptr->sG >> 1;
	sptr->sR = sptr->sG >> 2;
	sptr->dG = (GetRandomControl() & 0x1F) + 224;
	sptr->dB = sptr->dG >> 1;
	sptr->dR = sptr->dG >> 2;
	sptr->Life = 8;
	sptr->sLife = 8;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 2;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = (GetRandomControl() & 0xF) - 8;
	sptr->z = (GetRandomControl() & 0x7F) - 64;
	sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Yvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_DEF | SF_SCALE;
	sptr->Friction = 34;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = 1;
	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Width = (GetRandomControl() & 3) + 3;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 1;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 1;
}

void InitialiseWingmute(short item_number)
{
	ITEM_INFO* item;

	InitialiseCreature(item_number);
	item = &items[item_number];
	item->anim_number = objects[MUTANT1].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = WING_WAIT;
	item->goal_anim_state = WING_WAIT;
	item->item_flags[1] = GetRandomControl() & 0x7F;
}

void WingmuteControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* fly;
	AI_INFO info;
	PHD_VECTOR pos;
	long c;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	fly = (CREATURE_INFO*)item->data;
	angle = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state == WING_FALL)
		{
			if (item->pos.y_pos > item->floor)
			{
				item->pos.y_pos = item->floor;
				item->fallspeed = 0;
				item->gravity_status = 0;
				item->goal_anim_state = WING_DEATH;
			}

			item->pos.x_rot = 0;
		}
		else if (item->current_anim_state == WING_DEATH)
		{
			item->pos.x_rot = 0;
			item->pos.y_pos = item->floor;
		}
		else
		{
			item->anim_number = objects[MUTANT1].anim_index + 5;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = WING_FALL;
			item->gravity_status = 1;
			item->speed = 0;
			item->pos.x_rot = 0;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, fly->maximum_turn);

		switch (item->current_anim_state)
		{
		case WING_HOVER:
			fly->flags = 0;
			fly->maximum_turn = 546;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (item->hit_status || GetRandomControl() < 384 || item->ai_bits == MODIFY)
				item->goal_anim_state = WING_MOVE;
			else if (fly->mood != BORED_MOOD && GetRandomControl() >= 128 || item->hit_status || item->ai_bits == MODIFY)
			{
				if (info.ahead && info.distance < 0x40000)
					item->goal_anim_state = WING_ATTACK;
			}
			else
				item->goal_anim_state = WING_LAND;

			break;

		case WING_LAND:
			item->pos.y_pos += 51;

			if (item->pos.y_pos > item->floor)
				item->pos.y_pos = item->floor;

			break;

		case WING_WAIT:
			item->pos.y_pos = item->floor;
			fly->maximum_turn = 182;

			if (item->hit_status || info.distance < 0x900000 || fly->hurt_by_lara || item->ai_bits == MODIFY)
				item->goal_anim_state = WING_TAKEOFF;

			break;

		case WING_ATTACK:
			fly->maximum_turn = 546;

			if (info.ahead && info.distance < 0x40000)
				item->goal_anim_state = WING_ATTACK;
			else if (info.distance < 0x40000)
				item->goal_anim_state = WING_HOVER;
			else
			{
				item->goal_anim_state = WING_HOVER;
				item->required_anim_state = WING_MOVE;
			}

			if (!fly->flags && item->touch_bits & 0x1000)
			{
				lara_item->hit_points -= 50;
				lara_item->hit_status = 1;
				CreatureEffect(item, &wing_bite, DoBloodSplat);
				fly->flags = 1;
			}

			break;

		case WING_MOVE:
			fly->flags = 0;
			fly->maximum_turn = 546;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (fly->mood != BORED_MOOD && GetRandomControl() >= 128 || fly->hurt_by_lara || item->ai_bits == MODIFY)
			{
				if (info.ahead && info.distance < 0x40000)
					item->goal_anim_state = WING_ATTACK;
			}
			else
				item->goal_anim_state = WING_HOVER;

			break;
		}
	}

	c = abs(rcossin_tbl[item->item_flags[1] << 7] >> 7);

	if (c > 31)
		c = 31;

	c <<= 3;
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, 10);
	TriggerDynamic(pos.x, pos.y, pos.z, 10, 0, c, 0);
	item->item_flags[1] = (item->item_flags[1] + 1) & 0x3F;
	TriggerWingMuteParticles(item_number);
	TriggerWingMuteParticles(item_number);
	CreatureAnimation(item_number, angle, 0);
}
