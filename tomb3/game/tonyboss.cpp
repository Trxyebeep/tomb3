#include "../tomb3/pch.h"
#include "tonyboss.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "sphere.h"
#include "items.h"
#include "sound.h"
#include "lot.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "effects.h"
#include "traps.h"
#include "box.h"
#include "pickup.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "setup.h"
#include "lara.h"

static long heights[5] = { -1536, -1280, -832, -384, 0 };
static long radii[5] = { 200, 400, 500, 500, 475 };
static long dradii[5] = { 1600, 5600, 6400, 5600, 1600 };
static long dheights1[5] = { -7680, -4224, -768, 2688, 6144 };
static long dheights2[5] = { -1536, -1152, -768, -384, 0 };
static long death_radii[5];
static long death_heights[5];

SHIELD_POINTS TonyBossShield[40];

static void TriggerTonyFlame(short item_number, long node)
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
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;

	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = (uchar)node;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

static void TriggerFireBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle, long speed)
{
	FX_INFO* fx;
	PHD_VECTOR fxpos;
	long fallspeed;
	short fxNum;

	if (!type)
	{
		fxpos.x = 0;
		fxpos.y = 0;
		fxpos.z = 0;
		GetJointAbsPosition(item, &fxpos, 10);
		angle = item->pos.y_rot;
		fallspeed = -16;
		speed = 0;
	}
	else if (type == 1)
	{
		fxpos.x = 0;
		fxpos.y = 0;
		fxpos.z = 0;
		GetJointAbsPosition(item, &fxpos, 13);
		angle = item->pos.y_rot;
		fallspeed = -16;
		speed = 0;
	}
	else if (type == 2)
	{
		fxpos.x = 0;
		fxpos.y = 0;
		fxpos.z = 0;
		GetJointAbsPosition(item, &fxpos, 13);
		speed = 160;
		fallspeed = -32 - (GetRandomControl() & 7);
	}
	else if (type == 3)
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed = 0;
		fallspeed = (GetRandomControl() & 3) + 4;
	}
	else if (type == 4)
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed += (GetRandomControl() & 3);
		angle = short(GetRandomControl() << 1);
		fallspeed = (GetRandomControl() & 3) - 2;
	}
	else if (type == 5)
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed = (GetRandomControl() & 7) + 48;
		angle += (GetRandomControl() & 0x1FFF) + 0x7000;
		fallspeed = -16 - (GetRandomControl() & 0xF);
	}
	else
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed = (GetRandomControl() & 0x1F) + 32;
		angle = short(GetRandomControl() << 1);
		fallspeed = -32 - (GetRandomControl() & 0x1F);
	}

	fxNum = CreateEffect(room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = fxpos.x;
	fx->pos.y_pos = fxpos.y;
	fx->pos.z_pos = fxpos.z;
	fx->pos.y_rot = angle;
	fx->object_number = TONYFIREBALL;
	fx->speed = (short)speed;
	fx->fallspeed = (short)fallspeed;
	fx->flag1 = (short)type;
	fx->flag2 = (GetRandomControl() & 3) + 1;

	if (type == 5)
		fx->flag2 <<= 1;
	else if (type == 2)
		fx->flag2 = 0;
}

static void TriggerFireBallFlame(short fxNum, long type, long xv, long yv, long zv)
{
	FX_INFO* fx;
	SPARKS* sptr;
	long dx, dz;

	fx = &effects[fxNum];
	dx = lara_item->pos.x_pos - fx->pos.x_pos;
	dz = lara_item->pos.z_pos - fx->pos.z_pos;

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
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = short((GetRandomControl() & 0xFF) + xv - 128);
	sptr->Yvel = (short)yv;
	sptr->Zvel = short((GetRandomControl() & 0xFF) + zv - 128);
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

	sptr->FxObj = (uchar)fxNum;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;

	if (!type || type == 1)
	{
		sptr->Gravity = (GetRandomControl() & 0x1F) + 16;
		sptr->MaxYvel = (GetRandomControl() & 0xF) + 48;
		sptr->Scalar = 2;
		sptr->Yvel *= -16;
	}
	else if (type == 4 || type == 5 || type == 6)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else if (type == 3)
	{
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -64 - (GetRandomControl() & 0x1F);
		sptr->Scalar = 2;
		sptr->Yvel <<= 4;
	}
	else if (type == 2)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		sptr->Scalar = 2;
	}
}

static void ExplodeTonyBoss(ITEM_INFO* item)
{
	SHIELD_POINTS* p;
	long x, y, z, lp, lp2, rad, angle, r, g, b;

	if (item->hit_points <= 0 && (bossdata.explode_count == 1 || bossdata.explode_count == 15 || bossdata.explode_count == 25 ||
		bossdata.explode_count == 35 || bossdata.explode_count == 45 || bossdata.explode_count == 55))
	{
		x = (GetRandomDraw() & 0x3FF) + item->pos.x_pos - 512;
		y = item->pos.y_pos - (GetRandomDraw() & 0x3FF) - 256;
		z = (GetRandomDraw() & 0x3FF) + item->pos.z_pos - 512;
		ExpRings[bossdata.ring_count].x = x;
		ExpRings[bossdata.ring_count].y = y;
		ExpRings[bossdata.ring_count].z = z;
		ExpRings[bossdata.ring_count].on = 2;
		bossdata.ring_count++;

		TriggerExplosionSparks(x, y, z, 3, -2, 0, 0);

		for (lp = 0; lp < 2; lp++)
			TriggerExplosionSparks(x, y, z, 3, -1, 0, 0);

		SoundEffect(SFX_BLAST_CIRCLE, &item->pos, 0x800000 | SFX_SETPITCH);
	}

	for (lp = 0; lp < 5; lp++)
	{
		if (bossdata.explode_count < 128)
		{
			death_radii[lp] = (dradii[lp] >> 4) + ((bossdata.explode_count * dradii[lp]) >> 7);
			death_heights[lp] = dheights2[lp] + ((bossdata.explode_count * (dheights1[lp] - dheights2[lp])) >> 7);
		}
	}

	if (bossdata.explode_count > 64)
		return;

	p = TonyBossShield;

	for (lp = 0; lp < 5; lp++)
	{
		y = death_heights[lp];
		rad = death_radii[lp];
		angle = (wibble & 0x3F) << 3;

		for (lp2 = 0; lp2 < 8; lp2++, p++)
		{
			p->x = short((rad * rcossin_tbl[angle << 1]) >> 11);
			p->y = (short)y;
			p->z = short((rad * rcossin_tbl[(angle << 1) + 1]) >> 11);

			if (!lp || lp == 16 || bossdata.explode_count >= 64)
				p->rgb = 0;
			else
			{
				r = (GetRandomDraw() & 0x1F) + 224;
				g = (r >> 2) + (GetRandomDraw() & 0x3F);
				b = GetRandomDraw() & 0x3F;

				if (item->hit_points <= 0)
				{
					r = ((64 - bossdata.explode_count) * r) >> 6;
					g = ((64 - bossdata.explode_count) * g) >> 6;
					b = ((64 - bossdata.explode_count) * b) >> 6;
				}
				else
				{
					r = ((128 - bossdata.explode_count) * r) >> 7;
					g = ((128 - bossdata.explode_count) * g) >> 7;
					b = ((128 - bossdata.explode_count) * b) >> 7;
				}

				p->rgb = (b << 16) | (g << 8) | r;
			}

			angle = (angle + 512) & 0xFFF;
		}
	}
}

static void TonyBossDie(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->hit_points = DONT_TARGET;
	item->collidable = 0;
	KillItem(item_number);
	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE;
}

void ControlTonyFireBall(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR oldPos;
	long dx, dy, dz, h, c, lp, lp2, type, r, g, b;
	short room_number;
	uchar falloffs[7];

	falloffs[0] = 16;
	falloffs[1] = 0;
	falloffs[2] = 14;
	falloffs[3] = 9;
	falloffs[4] = 7;
	falloffs[5] = 7;
	falloffs[6] = 7;
	fx = &effects[fx_number];
	oldPos.x = fx->pos.x_pos;
	oldPos.y = fx->pos.y_pos;
	oldPos.z = fx->pos.z_pos;

	if (!fx->flag1 || fx->flag1 == 1)
	{
		fx->fallspeed += (fx->fallspeed >> 3) + 1;

		if (fx->fallspeed < -4096)
			fx->fallspeed = -4096;

		fx->pos.y_pos += fx->fallspeed;

		if (wibble & 4)
			TriggerFireBallFlame(fx_number, fx->flag1, 0, 0, 0);
	}
	else if (fx->flag1 == 3)
	{
		fx->fallspeed += 2;
		fx->pos.y_pos += fx->fallspeed;

		if (wibble & 4)
			TriggerFireBallFlame(fx_number, 3, 0, 0, 0);
	}
	else
	{
		if (fx->flag1 != 2)
		{
			if (fx->speed > 48)
				fx->speed--;
		}

		fx->fallspeed += fx->flag2;

		if (fx->fallspeed > 512)
			fx->fallspeed = 512;

		fx->pos.x_pos += fx->speed * phd_sin(fx->pos.y_rot) >> W2V_SHIFT;
		fx->pos.y_pos += fx->fallspeed >> 1;
		fx->pos.z_pos += fx->speed * phd_cos(fx->pos.y_rot) >> W2V_SHIFT;
		dx = (oldPos.x - fx->pos.x_pos) << 3;
		dy = (oldPos.y - fx->pos.y_pos) << 3;
		dz = (oldPos.z - fx->pos.z_pos) << 3;

		if (wibble & 4)
			TriggerFireBallFlame(fx_number, fx->flag1, dx, dy, dz);
	}

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos < c)
	{
		if (!fx->flag1 || fx->flag1 == 1 || fx->flag1 == 2 || fx->flag1 == 3)
		{
			TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 3, -2, 0, fx->room_number);

			if (!fx->flag1 || fx->flag1 == 1)
			{
				for (lp = 0; lp < 2; lp++)
					TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 3, -1, 0, fx->room_number);
			}

			pos = oldPos;

			if (fx->flag1 == 2)
				lp2 = 7;
			else
				lp2 = 3;

			if (fx->flag1 == 2)
				type = 5;
			else if (fx->flag1 == 3)
				type = 6;
			else
				type = 4;

			for (lp = 0; lp < lp2; lp++)
				TriggerFireBall(0, type, &pos, fx->room_number, fx->pos.y_rot, (lp << 2) + 32);

			if (!fx->flag1 || fx->flag1 == 1)
			{
				room_number = lara_item->room_number;
				floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
				pos.x = (GetRandomControl() & 0x3FF) + lara_item->pos.x_pos - 512;
				pos.y = GetCeiling(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos) + 256;
				pos.z = (GetRandomControl() & 0x3FF) + lara_item->pos.z_pos - 512;
				TriggerExplosionSparks(pos.x, pos.y, pos.z, 3, -2, 0, room_number);
				TriggerFireBall(0, 3, &pos, room_number, 0, 0);
			}
		}

		KillEffect(fx_number);
		return;
	}

	if (room[room_number].flags & ROOM_UNDERWATER)
	{
		KillEffect(fx_number);
		return;
	}

	if (!lara.burn && ItemNearLara(&fx->pos, 200))
	{
		lara_item->hit_status = 1;
		KillEffect(fx_number);
		lara_item->hit_points -= 200;
		LaraBurn();
		return;
	}

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, lara_item->room_number);

	if (falloffs[fx->flag1])
	{
		type = GetRandomControl();
		r = 255 - ((type >> 4) & 0x1F);
		g = 192 - ((type >> 6) & 0x1F);
		b = type & 0x3F;
		TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, falloffs[fx->flag1], r, g, b);
	}
}

void InitialiseTonyBoss(short item_number)
{
	ITEM_INFO* item;
	SHIELD_POINTS* p;
	long y, rad, angle;

	item = &items[item_number];
	bossdata.dead = 0;
	bossdata.dropped_icon = 0;
	bossdata.ring_count = 0;
	bossdata.explode_count = 0;
	item->item_flags[3] = 0;

	p = TonyBossShield;

	for (int i = 0; i < 5; i++)
	{
		y = heights[i];
		rad = radii[i];
		angle = 0;

		for (int j = 0; j < 8; j++, p++)
		{
			p->x = short((rad * rcossin_tbl[angle << 1]) >> 11);
			p->y = (short)y;
			p->z = short((rad * rcossin_tbl[(angle << 1) + 1]) >> 11);
			p->rgb = 0;
			angle += 512;
		}
	}
}

void TonyBossControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* tony;
	AI_INFO info;
	PHD_VECTOR pos;
	long x, z, f, r, g, b;
	short angle, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	tony = (CREATURE_INFO*)item->data;
	angle = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != TONYBOSS_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 6;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = TONYBOSS_DEATH;
		}

		if (item->frame_number - anims[item->anim_number].frame_base > 110)
		{
			item->mesh_bits = 0;
			item->frame_number = anims[item->anim_number].frame_base + 110;

			if (!bossdata.explode_count)
			{
				bossdata.ring_count = 0;

				for (int i = 0; i < 6; i++)
				{
					ExpRings[i].on = 0;
					ExpRings[i].life = 32;
					ExpRings[i].radius = 512;
					ExpRings[i].speed = 128 + (i << 5);
					ExpRings[i].xrot = ((GetRandomControl() & 0x1FF) - 256) & 0xFFF;
					ExpRings[i].zrot = ((GetRandomControl() & 0x1FF) - 256) & 0xFFF;
				}

				if (!bossdata.dropped_icon)
				{
					BossDropIcon(item_number);
					bossdata.dropped_icon = 1;
				}
			}

			if (bossdata.explode_count < 256)
				bossdata.explode_count++;

			if (bossdata.explode_count > 128 && bossdata.ring_count == 6 && !ExpRings[5].life)
			{
				TonyBossDie(item_number);
				bossdata.dead = 1;
			}
			else
				ExplodeTonyBoss(item);

			return;
		}
	}
	else
	{
		if (item->item_flags[3] != 2)
			item->hit_points = 100;

		CreatureAIInfo(item, &info);

		if (item->item_flags[3])
		{
			tony->target.x = lara_item->pos.x_pos;
			tony->target.z = lara_item->pos.z_pos;
			angle = CreatureTurn(item, tony->maximum_turn);
		}
		else
		{
			x = item->pos.x_pos - lara_item->pos.x_pos;
			z = item->pos.z_pos - lara_item->pos.z_pos;

			if (SQUARE(x) + SQUARE(z) < 0x1900000)
				item->item_flags[3] = 1;

			angle = 0;
		}

		switch (item->current_anim_state)
		{
		case TONYBOSS_WAIT:
			tony->maximum_turn = 0;

			if (item->goal_anim_state != TONYBOSS_RISE && item->item_flags[3])
				item->goal_anim_state = TONYBOSS_RISE;

			break;

		case TONYBOSS_RISE:

			if (item->frame_number - anims[item->anim_number].frame_base <= 16)
				tony->maximum_turn = 0;
			else
				tony->maximum_turn = 364;

			break;

		case TONYBOSS_FLOAT:
			torso_y = info.angle;
			torso_x = info.x_angle;
			tony->maximum_turn = 364;

			if (!bossdata.explode_count)
			{
				if (item->goal_anim_state != TONYBOSS_BIGBOOM && item->item_flags[3] != 2)
				{
					item->goal_anim_state = TONYBOSS_BIGBOOM;
					tony->maximum_turn = 0;
				}

				if (item->goal_anim_state != TONYBOSS_ROCKZAPP && item->item_flags[3] == 2 && !(wibble & 0xFF) && !item->item_flags[0])
				{
					item->goal_anim_state = TONYBOSS_ROCKZAPP;
					item->item_flags[0] = 1;
				}

				if (item->goal_anim_state != TONYBOSS_ZAPP && item->goal_anim_state != TONYBOSS_ROCKZAPP &&
					item->item_flags[3] == 2 && !(wibble & 0xFF) && item->item_flags[0] == 1)
				{
					item->goal_anim_state = TONYBOSS_ZAPP;
					item->item_flags[0] = 0;
				}
			}

			break;

		case TONYBOSS_ZAPP:
			torso_y = info.angle;
			torso_x = info.x_angle;
			tony->maximum_turn = 182;

			if (item->frame_number - anims[item->anim_number].frame_base == 28)
				TriggerFireBall(item, 2, 0, item->room_number, item->pos.y_rot, 0);

			break;

		case TONYBOSS_ROCKZAPP:
			torso_y = info.angle;
			torso_x = info.x_angle;
			tony->maximum_turn = 0;

			if (item->frame_number - anims[item->anim_number].frame_base == 40)
			{
				TriggerFireBall(item, 0, 0, item->room_number, 0, 0);
				TriggerFireBall(item, 1, 0, item->room_number, 0, 0);
			}

			break;

		case TONYBOSS_BIGBOOM:
			tony->maximum_turn = 0;

			if (item->frame_number - anims[item->anim_number].frame_base == 56)
			{
				item->item_flags[3] = 2;
				bossdata.explode_count = 1;
			}

			break;
		}
	}

	if (item->current_anim_state == TONYBOSS_ROCKZAPP || item->current_anim_state == TONYBOSS_ZAPP || item->current_anim_state == TONYBOSS_BIGBOOM)
	{
		f = item->frame_number - anims[item->anim_number].frame_base;

		if (f > 16)
		{
			f = anims[item->anim_number].frame_end - item->frame_number;

			if (f > 16)
				f = 16;
		}

		b = GetRandomControl();
		r = (f * (255 - ((b >> 4) & 0x1F))) >> 4;
		g = (f * (192 - ((b >> 6) & 0x1F))) >> 4;
		b = (f * (b & 0x3F)) >> 4;

		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 10);
		TriggerDynamic(pos.x, pos.y, pos.z, 12, r, g, b);
		TriggerTonyFlame(item_number, 5);

		if (item->current_anim_state == TONYBOSS_ROCKZAPP || item->current_anim_state == TONYBOSS_BIGBOOM)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, 13);
			TriggerDynamic(pos.x, pos.y, pos.z, 12, r, g, b);
			TriggerTonyFlame(item_number, 4);
		}
	}

	if (bossdata.explode_count && item->hit_points > 0)
	{
		ExplodeTonyBoss(item);
		bossdata.explode_count++;

		if (bossdata.explode_count == 32)
			FlipMap();

		if (bossdata.explode_count > 64)
		{
			bossdata.ring_count = 0;
			bossdata.explode_count = 0;
		}
	}

	CreatureJoint(item, 0, torso_y >> 1);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, torso_y >> 1);
	CreatureAnimation(item_number, angle, 0);
}

void S_DrawTonyBoss(ITEM_INFO* item)
{
	DrawAnimatingItem(item);

	if (bossdata.explode_count)
	{
		if (item->hit_points <= 0)
			DrawExplosionRings();

		if (bossdata.explode_count && bossdata.explode_count <= 64)
			DrawTonyBossShield(item);
	}
}
