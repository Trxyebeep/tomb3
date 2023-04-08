#include "../tomb3/pch.h"
#include "clawmute.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "items.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "effects.h"
#include "box.h"
#include "sound.h"
#include "people.h"
#include "lara.h"

static BITE_INFO claw_bite_left = { 19, -13, 3, 7 };
static BITE_INFO claw_bite_right = { 19, -13, 3, 4 };

static void TriggerPlasmaBallFlame(short fx_number, long type, long xv, long yv, long zv)
{
	FX_INFO* fx;
	SPARKS* sptr;
	long dx, dz;

	fx = &effects[fx_number];
	dx = lara_item->pos.x_pos - fx->pos.x_pos;
	dz = lara_item->pos.z_pos - fx->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 48;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 255;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = short(xv + (GetRandomControl() & 0xFF) - 128);
	sptr->Yvel = (short)yv;
	sptr->Zvel = short(zv + (GetRandomControl() & 0xFF) - 128);
	sptr->Friction = 5;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;

	sptr->FxObj = (uchar)fx_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;

	if (!type)
	{
		sptr->Scalar = 2;
		sptr->Xvel <<= 2;
		sptr->Yvel = (GetRandomControl() & 0x1FF) - 256;
		sptr->Zvel <<= 2;
		sptr->Friction = 85;
		sptr->dWidth >>= 1;
		sptr->dHeight >>= 1;
	}

	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
}

static void TriggerPlasmaBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle)
{
	FX_INFO* fx;
	PHD_VECTOR p;
	short angles[2];
	short fxNum, speed;

	if (type)
	{
		p.x = pos->x;
		p.y = pos->y;
		p.z = pos->z;
		angles[0] = short(GetRandomControl() << 1);
		angles[1] = 0x2000;
		speed = (GetRandomControl() & 0xF) + 16;
	}
	else
	{
		p.x = -32;
		p.y = -16;
		p.z = -192;
		GetJointAbsPosition(item, &p, 13);
		phd_GetVectorAngles(lara_item->pos.x_pos - p.x, lara_item->pos.y_pos - p.y - 256, lara_item->pos.z_pos - p.z, angles);
		angles[0] = item->pos.y_rot;
		speed = (GetRandomControl() & 0xF) + 16;
	}

	fxNum = CreateEffect(room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = p.x;
	fx->pos.y_pos = p.y;
	fx->pos.z_pos = p.z;
	fx->pos.x_rot = angles[1];
	fx->pos.y_rot = angles[0];
	fx->object_number = EXTRAFX1;
	fx->speed = speed;
	fx->fallspeed = 0;
	fx->flag1 = (short)type;
}

static void TriggerPlasma(short item_number)
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
	sptr->sR = 48;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 255;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->Friction = 3;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Yvel = (GetRandomControl() & 0xF) + 16;
	sptr->Zvel = (GetRandomControl() & 0x1F) - 16;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;

	sptr->Gravity = (GetRandomControl() & 0x1F) + 16;
	sptr->NodeNumber = 6;
	sptr->MaxYvel = (GetRandomControl() & 7) + 16;
	sptr->FxObj = (uchar)item_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

void ControlClawmutePlasmaBall(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR oldPos;
	long speed, h, c, r, g, b, lp;
	short room_number;
	uchar falloffs[2];

	falloffs[0] = 13;
	falloffs[1] = 7;
	fx = &effects[fx_number];
	oldPos.x = fx->pos.x_pos;
	oldPos.y = fx->pos.y_pos;
	oldPos.z = fx->pos.z_pos;

	if (fx->speed < 384 && !fx->flag1)
		fx->speed += (fx->speed >> 3) + 4;

	if (fx->flag1)
	{
		fx->fallspeed++;

		if (fx->fallspeed > 8)
			fx->speed -= 2;

		if (fx->pos.x_rot > -0x3C00)
			fx->pos.x_rot -= 0x100;
	}

	speed = (fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.x_pos += (speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += fx->fallspeed - ((fx->speed * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	fx->pos.z_pos += (speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

	if (wibble & 4)
	{
		if (fx->flag1)
			TriggerPlasmaBallFlame(fx_number, fx->flag1, 0, abs(oldPos.y - fx->pos.y_pos) << 3, 0);
		else
			TriggerPlasmaBallFlame(fx_number, 0, 0, 0, 0);
	}

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos < c)
	{
		if (!fx->flag1)
		{
			pos.x = oldPos.x;
			pos.y = oldPos.y;
			pos.z = oldPos.z;
			h = (GetRandomControl() & 3) + 5;

			for (lp = 0; lp < h; lp++)
				TriggerPlasmaBall(0, 1, &pos, fx->room_number, fx->pos.y_rot);
		}

		KillEffect(fx_number);
	}
	else if (room[room_number].flags & ROOM_UNDERWATER)
		KillEffect(fx_number);
	else if (ItemNearLara(&fx->pos, 200) && !fx->flag1)
	{
		pos.x = fx->pos.x_pos;
		pos.y = fx->pos.y_pos;
		pos.z = fx->pos.z_pos;
		h = (GetRandomControl() & 1) + 3;

		for (lp = 0; lp < h; lp++)
			TriggerPlasmaBall(0, 1, &pos, fx->room_number, fx->pos.y_rot);

		lara_item->hit_points -= 200;
		lara_item->hit_status = 1;
		KillEffect(fx_number);
	}
	else
	{
		if (fx->room_number != room_number)
			EffectNewRoom(fx_number, lara_item->room_number);

		if (falloffs[fx->flag1])
		{
			c = GetRandomControl();
			r = c & 0x3F;
			g = 192 - ((c >> 6) & 0x1F);
			b = 255 - ((c >> 4) & 0x1F);
			TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, falloffs[fx->flag1], r, g, b);
		}
	}
}

void ClawmuteControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* claw;
	PHD_VECTOR pos;
	AI_INFO info;
	AI_INFO larainfo;
	long x, z, r, g, b, lp;
	short angle, torso_x, torso_y, head;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	claw = (CREATURE_INFO*)item->data;
	angle = 0;
	torso_x = 0;
	torso_y = 0;
	head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != CLAW_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 20;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = CLAW_DEATH;
		}

		if (item->frame_number == anims[item->anim_number].frame_end - 1)
		{
			CreatureDie(item_number, 1);
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 2, 0);

			for (lp = 0; lp < 2; lp++)
				TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 2, 0);

			SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
			return;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(claw);

		CreatureAIInfo(item, &info);

		if (claw->enemy == lara_item)
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

		if (info.zone_number == info.enemy_zone)
		{
			GetCreatureMood(item, &info, 1);
			CreatureMood(item, &info, 1);
		}
		else
		{
			GetCreatureMood(item, &info, 0);
			CreatureMood(item, &info, 0);
		}

		angle = CreatureTurn(item, claw->maximum_turn);

		enemy = claw->enemy;
		claw->enemy = lara_item;

		if (larainfo.distance < 0x100000 || item->hit_status || TargetVisible(item, &larainfo))
			AlertAllGuards(item_number);

		claw->enemy = enemy;

		switch (item->current_anim_state)
		{
		case CLAW_STOP:
			claw->maximum_turn = 0;
			claw->flags = 0;
			head = info.angle;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(claw);
				item->goal_anim_state = CLAW_STOP;
			}
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = CLAW_WALK;
				head = 0;
			}
			else if (claw->mood == ESCAPE_MOOD)
				item->goal_anim_state = CLAW_RUN;
			else if (info.bite && info.distance < 0x100000)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;

				if (info.angle < 0)
					item->goal_anim_state = CLAW_SLASH_LEFT;
				else
					item->goal_anim_state = CLAW_SLASH_RIGHT;
			}
			else if (info.bite && info.distance < 0x1C6E39)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
				item->goal_anim_state = CLAW_CLAW_ATAK;
			}
			else if (Targetable(item, &info) && (info.distance > 0x900000 && !item->item_flags[0]) || info.zone_number != info.enemy_zone)
				item->goal_anim_state = CLAW_FIRE_ATAK;
			else if (claw->mood == BORED_MOOD)
			{
				GetRandomControl();
				item->goal_anim_state = CLAW_WALK;
			}
			else if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else
				item->goal_anim_state = CLAW_RUN;

			break;

		case CLAW_WALK:
			claw->maximum_turn = 546;

			if (info.ahead)
				head = info.angle;

			if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = CLAW_WALK;
				head = 0;
			}
			else if (info.bite && info.distance < 0x1C6E39)
			{
				claw->maximum_turn = 546;

				if (info.angle < 0)
					item->goal_anim_state = CLAW_WALK_ATAK1;
				else
					item->goal_anim_state = CLAW_WALK_ATAK2;
			}
			else if (Targetable(item, &info) && (info.distance > 0x900000 && !item->item_flags[0]) || info.zone_number != info.enemy_zone)
			{
				claw->maximum_turn = 546;
				item->goal_anim_state = CLAW_STOP;
			}
			else if (claw->mood == ESCAPE_MOOD || claw->mood == ATTACK_MOOD)
				item->goal_anim_state = CLAW_RUN;

			break;

		case CLAW_RUN:
			claw->maximum_turn = 728;

			if (info.ahead)
				head = info.angle;

			if (item->ai_bits & GUARD || claw->mood == BORED_MOOD || claw->flags && info.ahead)
				item->goal_anim_state = CLAW_STOP;
			else if (info.bite && info.distance < 0x400000)
			{
				if (lara_item->speed)
					item->goal_anim_state = CLAW_RUN_ATAK;
				else
					item->goal_anim_state = CLAW_STOP;
			}
			else if (Targetable(item, &info) && (info.distance > 0x900000 && !item->item_flags[0]) || info.zone_number != info.enemy_zone)
			{
				claw->maximum_turn = 546;
				item->goal_anim_state = CLAW_STOP;
			}

			claw->flags = 0;
			break;

		case CLAW_RUN_ATAK:
		case CLAW_WALK_ATAK1:
		case CLAW_WALK_ATAK2:
		case CLAW_SLASH_LEFT:
		case CLAW_SLASH_RIGHT:
		case CLAW_CLAW_ATAK:

			if (info.ahead)
			{
				torso_x = info.x_angle;
				torso_y = info.angle;
			}

			if (!claw->flags && item->touch_bits & 0x90)
			{
				lara_item->hit_points -= 100;
				lara_item->hit_status = 1;
				CreatureEffect(item, &claw_bite_left, DoBloodSplat);
				CreatureEffect(item, &claw_bite_right, DoBloodSplat);
				claw->flags = 1;
			}

			item->item_flags[0] = 0;

			break;

		case CLAW_FIRE_ATAK:

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

			if (item->frame_number == anims[item->anim_number].frame_base && !(GetRandomControl() & 3))
				item->item_flags[0] = 1;

			if (item->frame_number - anims[item->anim_number].frame_base < 28)
				TriggerPlasma(item_number);
			else if (item->frame_number - anims[item->anim_number].frame_base == 28)
				TriggerPlasmaBall(item, 0, 0, item->room_number, item->pos.y_rot);

			lp = item->frame_number - anims[item->anim_number].frame_base;

			if (lp > 16)
			{
				lp = anims[item->anim_number].frame_base - item->frame_number + 44;

				if (lp > 16)
					lp = 16;
			}

			if (lp > 0)
			{
				b = GetRandomControl();
				r = (lp * (b & 0x3F)) >> 4;
				g = (lp * (192 - ((b >> 6) & 0x1F))) >> 4;
				b = (lp * (255 - ((b >> 4) & 0x1F))) >> 4;
				pos.x = -32;
				pos.y = -16;
				pos.z = -192;
				GetJointAbsPosition(item, &pos, 13);
				TriggerDynamic(pos.x, pos.y, pos.z, 13, r, g, b);
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
