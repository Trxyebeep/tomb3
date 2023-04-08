#include "../tomb3/pch.h"
#include "punk.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "box.h"
#include "sphere.h"
#include "../3dsystem/phd_math.h"
#include "people.h"
#include "gameflow.h"
#include "sound.h"
#include "effects.h"
#include "traps.h"
#include "../specific/smain.h"
#include "control.h"
#include "lara.h"

static BITE_INFO punk_hit = { 16, 48, 320, 13 };

static void TriggerPunkFlame(short item_number)
{
	SPARKS* sptr;
	long dx, dz;
	uchar size;

	dx = lara_item->pos.x_pos - items[item_number].pos.x_pos;
	dz = lara_item->pos.z_pos - items[item_number].pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;

	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	
	if (GetRandomControl() & 1)
	{
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
	{
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}

	sptr->NodeNumber = 2;
	sptr->FxObj = (uchar)item_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	size = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = size;
	sptr->Width = size;
	sptr->sHeight = size;
	sptr->Height = size;
	size >>= 2;
	sptr->dWidth = size;
	sptr->dHeight = size;
}

void InitialisePunk(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[PUNK1].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void PunkControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* punk;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO lara_info;
	long rnd, dx, dz, r, g, b;
	short tilt, angle, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	punk = (CREATURE_INFO*)item->data;
	tilt = 0;
	angle = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->item_flags[2])
	{
		TriggerPunkFlame(item_number);
		rnd = GetRandomControl();
		pos.x = (rnd & 0xF) + punk_hit.x - 8;
		pos.y = ((rnd >> 4) & 0xF) + punk_hit.y - 8;
		pos.z = ((rnd >> 8) & 0xF) + punk_hit.z - 8;
		GetJointAbsPosition(item, &pos, punk_hit.mesh_num);
		r = 255 - ((rnd >> 4) & 0x1F);
		g = 192 - ((rnd >> 6) & 0x1F);
		b = rnd & 0x3F;
		TriggerDynamic(pos.x, pos.y, pos.z, 13, r, g, b);
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != 11)
		{
			item->anim_number = objects[PUNK1].anim_index + 26;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 11;
			punk->LOT.step = 256;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(punk);
		else
			punk->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (punk->enemy == lara_item)
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

		if (!punk->alerted && punk->enemy == lara_item)
			punk->enemy = 0;

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, punk->maximum_turn);
		enemy = punk->enemy;
		punk->enemy = lara_item;

		if (item->hit_status || (lara_info.distance < 0x100000 || TargetVisible(item, &lara_info)) &&
			abs(lara_item->pos.y_pos - item->pos.y_pos) < 1280 && CurrentLevel != LV_TOWER && !(item->ai_bits & FOLLOW))
		{
			if (!punk->alerted)
				SoundEffect(SFX_ENGLISH_HOY, &item->pos, SFX_DEFAULT);

			AlertAllGuards(item_number);
		}

		punk->enemy = enemy;

		switch (item->current_anim_state)
		{
		case 5:

			if (punk->alerted || item->goal_anim_state == 10)
			{
				item->goal_anim_state = 1;
				break;
			}

		case 1:
			punk->flags = 0;
			punk->maximum_turn = 0;
			head = lara_info.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(punk);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == 1)
						item->goal_anim_state = 5;
					else
						item->goal_anim_state = 1;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = 2;
			else if (punk->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = 1;
			}
			else if (punk->mood == BORED_MOOD || item->ai_bits & FOLLOW && (punk->reached_goal || lara_info.distance > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = 1;
				else
					item->goal_anim_state = 10;
			}
			else if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = 7;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = 6;
			else
				item->goal_anim_state = 10;

			break;

		case 2:
			punk->maximum_turn = 910;
			head = lara_info.angle;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = 2;
				head = 0;
			}
			else if (punk->mood == ESCAPE_MOOD)
				item->goal_anim_state = 10;
			else if (punk->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
				{
					item->required_anim_state = 5;
					item->goal_anim_state = 1;
				}
			}
			else if (info.bite && info.distance < 0x40000)
				item->goal_anim_state = 1;
			else if (info.bite && info.distance < 0x190000)
				item->goal_anim_state = 4;
			else
				item->goal_anim_state = 10;

			break;

		case 3:
			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			punk->maximum_turn = 910;

			if (punk->flags != 2 && item->touch_bits & 0x2400)
			{
				lara_item->hit_points -= 100;
				lara_item->hit_status = 1;
				CreatureEffect(item, &punk_hit, DoBloodSplat);
				SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);

				if (item->item_flags[2] == 1)
					LaraBurn();
				else if (item->item_flags[2])
					item->item_flags[2]--;

				punk->flags = 2;
			}

			break;

		case 4:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			punk->maximum_turn = 910;
			punk->flags = 0;

			if (info.bite && info.distance < 0x190000)
				item->goal_anim_state = 3;
			else
				item->goal_anim_state = 2;

			break;

		case 6:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			punk->maximum_turn = 910;
			punk->flags = 0;

			if (!info.ahead || info.distance >= 0x100000)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 8;

			break;

		case 7:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			punk->maximum_turn = 910;
			punk->flags = 0;

			if (!info.bite || info.distance >= 0x40000)
				item->goal_anim_state = 1;
			else
				item->goal_anim_state = 9;

			break;

		case 8:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			punk->maximum_turn = 910;

			if (!punk->flags && item->touch_bits & 0x2400)
			{
				lara_item->hit_points -= 80;
				lara_item->hit_status = 1;
				CreatureEffect(item, &punk_hit, DoBloodSplat);
				SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);

				if (item->item_flags[2] == 1)
					LaraBurn();
				else if (item->item_flags[2])
					item->item_flags[2]--;

				punk->flags = 1;
			}

			if (info.ahead && info.distance > 0x100000 && info.distance < 0x190000)
				item->goal_anim_state = 3;

			break;

		case 9:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
			}

			punk->maximum_turn = 910;

			if (!punk->flags && item->touch_bits & 0x2400)
			{
				lara_item->hit_points -= 80;
				lara_item->hit_status = 1;
				CreatureEffect(item, &punk_hit, DoBloodSplat);
				SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);

				if (item->item_flags[2] == 1)
					LaraBurn();
				else if (item->item_flags[2])
					item->item_flags[2]--;

				punk->flags = 1;
			}

			break;

		case 10:

			if (info.ahead)
				head = info.angle;

			punk->maximum_turn = 1092;
			tilt = angle / 2;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = 5;
			else if (punk->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = 1;
			}
			else if (item->ai_bits & FOLLOW && (punk->reached_goal || lara_info.distance > 0x400000))
				item->goal_anim_state = 1;
			else if (punk->mood == BORED_MOOD)
				item->goal_anim_state = 2;
			else if (info.ahead && info.distance < 0x100000)
				item->goal_anim_state = 2;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (item->current_anim_state >= 11)
	{
		punk->maximum_turn = 0;
		CreatureAnimation(item_number, angle, 0);
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 260))
		{
		case -4:
			punk->maximum_turn = 0;
			item->anim_number = objects[PUNK1].anim_index + 30;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 15;
			break;

		case 2:
			punk->maximum_turn = 0;
			item->anim_number = objects[PUNK1].anim_index + 28;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 13;
			break;

		case 3:
			punk->maximum_turn = 0;
			item->anim_number = objects[PUNK1].anim_index + 29;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 14;
			break;

		case 4:
			punk->maximum_turn = 0;
			item->anim_number = objects[PUNK1].anim_index + 27;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 12;
			break;
		}
	}
}
