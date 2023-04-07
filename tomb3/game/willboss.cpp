#include "../tomb3/pch.h"
#include "willboss.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "effects.h"
#include "sphere.h"
#include "traps.h"
#include "sound.h"
#include "box.h"
#include "invfunc.h"
#include "lot.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "lara.h"
#include "setup.h"

static BITE_INFO willboss_bite_left = { 19, -13, 3, 20 };
static BITE_INFO willboss_bite_right = { 19, -13, 3, 23 };

static long dradii[5] = { 1600, 5600, 6400, 5600, 1600 };
static long dheights1[5] = { -7680, -4224, -768, 2688, 6144 };
static long dheights2[5] = { -1536, -1152, -768, -384, 0 };
static long death_radii[5];
static long death_heights[5];

static long closest_ai_path = -1;
static long lara_ai_path = -1;
static long lara_junction = -1;
static long junction_index[4];
static PHD_3DPOS ai_path[16];
static PHD_3DPOS ai_junction[4];

SHIELD_POINTS WillBossShield[40];

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
	sptr->sG = 255;
	sptr->sB = (GetRandomControl() & 0x1F) + 48;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 192;
	sptr->dB = (GetRandomControl() & 0x3F) + 128;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->Friction = 85;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = short(2 * (xv + (GetRandomControl() & 0xFF)) - 256);
	sptr->Yvel = (GetRandomControl() & 0x1FF) - 256;
	sptr->Zvel = short(2 * (zv + (GetRandomControl() & 0xFF)) - 256);
	
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
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	if (type < 0)
	{
		if (type >= -2)
			sptr->Scalar = 2;
		else
			sptr->Scalar = 4;

		sptr->Width = (GetRandomControl() & 0xF) + 16;
		sptr->Friction = 5;
		sptr->Xvel = short((GetRandomControl() & 0xFF) + xv - 128);
		sptr->Yvel = (short)yv;
		sptr->Zvel = short((GetRandomControl() & 0xFF) + zv - 128);
	}
	else
	{
		sptr->Scalar = 3;
		sptr->Width = (uchar)type;
	}

	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 3;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 3;
}

static void TriggerPlasmaBall(PHD_VECTOR* pos, short room_number, short angle, short type)
{
	FX_INFO* fx;
	short fxNum;

	fxNum = CreateEffect(room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = pos->x;
	fx->pos.y_pos = pos->y;
	fx->pos.z_pos = pos->z;
	fx->pos.x_rot = 0;
	fx->pos.y_rot = angle;
	fx->object_number = EXTRAFX2;

	if (type != -16)
		fx->speed = (GetRandomControl() & 0x1F) + 16;
	else
		fx->speed = 0;

	fx->fallspeed = -16 * type;
	fx->flag1 = type;
}

static void TriggerPlasma(short item_number, long node, long size)
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
	sptr->sG = 255;
	sptr->sB = (GetRandomControl() & 0x1F) + 48;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 192;
	sptr->dB = (GetRandomControl() & 0x3F) + 128;
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
	sptr->Yvel = (GetRandomControl() & 7) + 8;
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

	sptr->Gravity = (GetRandomControl() & 7) + 8;
	sptr->NodeNumber = (uchar)node;
	sptr->MaxYvel = (GetRandomControl() & 7) + 16;
	sptr->FxObj = (uchar)item_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	size += GetRandomControl() & 0xF;
	sptr->Width = (uchar)size;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

static void ExplodeWillBoss(ITEM_INFO* item)
{
	SHIELD_POINTS* p;
	PHD_VECTOR pos;
	long x, y, z, lp, lp2, rad, angle, r, g, b, m;

	if (bossdata.explode_count == 1 || bossdata.explode_count == 15 || bossdata.explode_count == 25 ||
		bossdata.explode_count == 35 || bossdata.explode_count == 45 || bossdata.explode_count == 55)
	{
		x = (GetRandomDraw() & 0x3FF) + item->pos.x_pos - 512;
		y = item->pos.y_pos - (GetRandomDraw() & 0x3FF) - 256;
		z = (GetRandomDraw() & 0x3FF) + item->pos.z_pos - 512;
		ExpRings[bossdata.ring_count].x = x;
		ExpRings[bossdata.ring_count].y = y;
		ExpRings[bossdata.ring_count].z = z;
		ExpRings[bossdata.ring_count].on = 1;
		bossdata.ring_count++;

		for (lp = 0; lp < 24; lp += 3)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, lp);
			TriggerPlasmaBall(&pos, item->room_number, short(GetRandomControl() << 1), 4);
		}

		TriggerExplosionSparks(x, y, z, 3, -2, 2, 0);
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

	p = WillBossShield;

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
				m = 64 - bossdata.explode_count;

				r = GetRandomDraw() & 0x3F;
				g = (GetRandomDraw() & 0x1F) + 224;
				b = (g >> 1) + (GetRandomDraw() & 0x3F);

				r = (m * r) >> 6;
				g = (m * g) >> 6;
				b = (m * b) >> 6;

				p->rgb = (b << 16) | (g << 8) | r;
			}

			angle = (angle + 512) & 0xFFF;
		}
	}
}

static void WillBossDie(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->hit_points = DONT_TARGET;
	item->collidable = 0;
	KillItem(item_number);
	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE;
}

void ControlWillbossPlasmaBall(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR oldPos;
	long speed, h, c, r, g, b;
	short room_number;
	short ang[2];
	char falloffs[5];

	falloffs[0] = 13;
	falloffs[1] = 7;
	falloffs[2] = 7;
	falloffs[3] = 7;
	falloffs[4] = 7;
	fx = &effects[fx_number];
	oldPos.x = fx->pos.x_pos;
	oldPos.y = fx->pos.y_pos;
	oldPos.z = fx->pos.z_pos;

	if (fx->flag1)
	{
		fx->fallspeed += (fx->flag1 != 1) + 1;

		if (!(wibble & 0xC))
		{
			if (fx->speed)
				fx->speed--;

			TriggerPlasmaBallFlame(fx_number, -1 - fx->flag1, 0, -(GetRandomControl() & 0x1F), 0);
		}
	}
	else
	{
		phd_GetVectorAngles(lara_item->pos.x_pos - oldPos.x, lara_item->pos.y_pos - oldPos.y - 256, lara_item->pos.z_pos - oldPos.z, ang);
		fx->pos.x_rot = ang[1];
		fx->pos.y_rot = ang[0];

		if (fx->speed < 512)
			fx->speed += (fx->speed >> 4) + 4;

		if (wibble & 4)
			TriggerPlasmaBallFlame(fx_number, fx->speed >> 1, 0, 0, 0);
	}

	speed = (fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.x_pos += (speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += fx->fallspeed - ((fx->speed * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	fx->pos.z_pos += (speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

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
			h = (GetRandomControl() & 3) + 2;

			for (int i = 0; i < h; i++)
				TriggerPlasmaBall(&pos, fx->room_number, fx->pos.y_rot + (GetRandomControl() & 0x3FFF) + 0x6000, 1);
		}

		KillEffect(fx_number);
	}
	else if (ItemNearLara(&fx->pos, 200) && !fx->flag1)
	{
		for (int i = 14; i >= 0; i -= 2)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(lara_item, &pos, i);
			TriggerPlasmaBall(&pos, fx->room_number, short(GetRandomControl() << 1), 1);
		}

		LaraBurn();
		lara_item->hit_points = -1;
		lara.BurnGreen = 1;
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
			g = 255 - ((c >> 4) & 0x1F);
			b = 192 - ((c >> 6) & 0x1F);
			TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, falloffs[fx->flag1], r, g, b);
		}
	}
}

void InitialiseWillBoss(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[1] = 0;
	closest_ai_path = -1;
	bossdata.explode_count = 0;
	bossdata.dropped_icon = 0;
	bossdata.dead = 0;
}

void WillBossControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* ai;
	CREATURE_INFO* willy;
	AI_INFO info;
	PHD_VECTOR pos;
	long lp, lp2, nJunction, nPath, dist, best_dist, best_dist2, x, z, index, fire, f, r, g, b;
	static long direction = 1;
	static long desired_direction = 1;
	static long puzzle_complete;
	short lara_alive, angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	willy = (CREATURE_INFO*)item->data;
	lara_alive = lara_item->hit_points > 0;

	if (closest_ai_path == -1)
	{
		nJunction = 0;
		nPath = 0;

		for (lp = room[item->room_number].item_number; lp != NO_ITEM; lp = ai->next_item)
		{
			ai = &items[lp];

			if (ai->object_number == AI_X1 && nPath < 16)
				ai_path[nPath++] = ai->pos;
			else if (ai->object_number == AI_X2 && nJunction < 4)
				ai_junction[nJunction++] = ai->pos;
		}

		closest_ai_path = -1;
		best_dist = 0x7FFFFFFF;

		for (lp = 0; lp < 16; lp++)
		{
			x = (ai_path[lp].x_pos - item->pos.x_pos) >> 6;
			z = (ai_path[lp].z_pos - item->pos.z_pos) >> 6;
			dist = SQUARE(x) + SQUARE(z);

			if (dist < best_dist)
			{
				closest_ai_path = lp;
				best_dist = dist;
			}
		}

		lara_ai_path = -1;
		best_dist = 0x7FFFFFFF;

		for (lp = 0; lp < 16; lp++)
		{
			x = (ai_path[lp].x_pos - lara_item->pos.x_pos) >> 6;
			z = (ai_path[lp].z_pos - lara_item->pos.z_pos) >> 6;
			dist = SQUARE(x) + SQUARE(z);

			if (dist < best_dist)
			{
				lara_ai_path = lp;
				best_dist = dist;
			}
		}

		for (lp2 = 0; lp2 < 4; lp2++)
		{
			index = -1;
			best_dist = 0x7FFFFFFF;

			for (lp = 0; lp < 16; lp++)
			{
				x = abs((ai_path[lp].x_pos - ai_junction[lp2].x_pos) >> 6);
				z = abs((ai_path[lp].z_pos - ai_junction[lp2].z_pos) >> 6);
				dist = x + (z >> 1);

				if (dist < best_dist)
				{
					index = lp;
					best_dist = dist;
				}
			}

			junction_index[lp2] = index;
		}
	}

	lp2 = closest_ai_path;
	best_dist = 0x7FFFFFFF;

	for (lp = lp2 - 1; lp < lp2 + 2; lp++)
	{
		if (lp < 0)
			nPath = lp + 16;
		else if (lp > 15)
			nPath = lp - 16;
		else
			nPath = lp;

		x = (ai_path[nPath].x_pos - item->pos.x_pos) >> 6;
		z = (ai_path[nPath].z_pos - item->pos.z_pos) >> 6;
		dist = SQUARE(x) + SQUARE(z);

		if (dist < best_dist)
		{
			closest_ai_path = nPath;
			best_dist = dist;
		}
	}

	lp2 = lara_ai_path;
	best_dist = 0x7FFFFFFF;

	for (lp = lp2 - 1; lp < lp2 + 2; lp++)
	{
		if (lp < 0)
			nPath = lp + 16;
		else if (lp > 15)
			nPath = lp - 16;
		else
			nPath = lp;

		x = (ai_path[nPath].x_pos - lara_item->pos.x_pos) >> 6;
		z = (ai_path[nPath].z_pos - lara_item->pos.z_pos) >> 6;
		dist = SQUARE(x) + SQUARE(z);

		if (dist < best_dist)
		{
			lara_ai_path = nPath;
			best_dist = dist;
		}
	}

	best_dist2 = 0x7FFFFFFF;

	for (lp = 0; lp < 4; lp++)
	{
		x = (ai_junction[lp].x_pos - lara_item->pos.x_pos) >> 6;
		z = (ai_junction[lp].z_pos - lara_item->pos.z_pos) >> 6;
		dist = SQUARE(x) + SQUARE(z);

		if (dist < best_dist2)
		{
			lara_junction = lp;
			best_dist2 = dist;
		}
	}

	fire = best_dist2 < best_dist || item->pos.y_pos > lara_item->pos.y_pos + 2048;
	x = ai_junction[lara_junction].x_pos - item->pos.x_pos;
	z = ai_junction[lara_junction].z_pos - item->pos.z_pos;
	dist = SQUARE(x) + SQUARE(z);

	if (item->hit_points <= 0)
	{
		puzzle_complete = Inv_RequestItem(ICON_PICKUP1_ITEM);
		puzzle_complete += Inv_RequestItem(ICON_PICKUP2_ITEM);
		puzzle_complete += Inv_RequestItem(ICON_PICKUP3_ITEM);
		puzzle_complete += Inv_RequestItem(ICON_PICKUP4_ITEM);

		if (puzzle_complete == 4 && item->item_flags[1])
		{
			if (item->current_anim_state != WILLBOSS_STUNNED)
			{
				item->anim_number = objects[item->object_number].anim_index + 7;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = WILLBOSS_STUNNED;
			}
			else if (item->frame_number >= anims[item->anim_number].frame_end - 2)
			{
				item->mesh_bits = 0;
				item->frame_number = anims[item->anim_number].frame_end - 2;

				if (!bossdata.explode_count)
				{
					bossdata.ring_count = 0;

					for (lp = 0; lp < 6; lp++)
					{
						ExpRings[lp].on = 0;
						ExpRings[lp].life = 32;
						ExpRings[lp].radius = 512;
						ExpRings[lp].speed = short((lp + 4) << 5);
						ExpRings[lp].xrot = ((GetRandomControl() & 0x1FF) - 256) & 0xFFF;
						ExpRings[lp].zrot = ((GetRandomControl() & 0x1FF) - 256) & 0xFFF;
					}
				}

				if (bossdata.explode_count < 256)
					bossdata.explode_count++;

				if (bossdata.explode_count <= 128 || bossdata.ring_count != 6 || ExpRings[5].life)
					ExplodeWillBoss(item);
				else
				{
					WillBossDie(item_number);
					bossdata.dead = 1;
				}

				return;
			}
		}
		else
		{
			willy->maximum_turn = 0;

			switch (item->current_anim_state)
			{
			case WILLBOSS_STOP:
				item->goal_anim_state = WILLBOSS_STUNNED;
				break;

			case WILLBOSS_STUNNED:
				bossdata.death_count = 280;
				break;

			case WILLBOSS_KNOCKOUT:
				bossdata.death_count--;

				if (bossdata.death_count < 0)
					item->goal_anim_state = WILLBOSS_GETUP;

				break;

			case WILLBOSS_GETUP:
				item->hit_points = 200;

				if (puzzle_complete == 4)
					item->item_flags[1] = 1;

				willy->maximum_turn = 364;
				break;

			default:
				item->goal_anim_state = WILLBOSS_STOP;
				break;
			}
		}
	}
	else
	{
		CreatureAIInfo(item, &info);

		if (item->touch_bits)
			lara_item->hit_points -= 10;

		index = lara_ai_path - closest_ai_path;

		if (direction == -1 && (index < 0 && index > -6 || index > 10))
			desired_direction = 1;
		else if (direction == 1 && (index > 0 && index < 6 || index < -10))
			desired_direction = -1;

		willy->target.x = ai_path[closest_ai_path].x_pos + (WALL_SIZE * direction * phd_sin(ai_path[closest_ai_path].y_rot) >> W2V_SHIFT);
		willy->target.z = ai_path[closest_ai_path].z_pos + (WALL_SIZE * direction * phd_cos(ai_path[closest_ai_path].y_rot) >> W2V_SHIFT);

		switch (item->current_anim_state)
		{
		case WILLBOSS_STOP:
			willy->maximum_turn = 0;
			willy->flags = 0;

			if (direction != desired_direction)
				item->goal_anim_state = WILLBOSS_180;
			else if (fire && info.ahead && dist < 0x1000000 && lara_item->hit_points > 0)
				item->goal_anim_state = WILLBOSS_SHOOT;
			else if (!info.bite || info.distance >= 0x400000)
				item->goal_anim_state = WILLBOSS_WALK;
			else
				item->goal_anim_state = WILLBOSS_LUNGE;

			break;

		case WILLBOSS_WALK:
			willy->maximum_turn = 910;
			willy->flags = 0;

			if (direction != desired_direction)
				item->goal_anim_state = WILLBOSS_STOP;
			else if (fire && info.ahead && dist < 0x1000000)
				item->goal_anim_state = WILLBOSS_STOP;
			else if (info.bite && info.distance < 0x240000)
			{
				if ((GetRandomControl() & 3) == 1)
					item->goal_anim_state = WILLBOSS_STOP;
				else if (item->frame_number >= anims[item->anim_number].frame_base + 30)
					item->goal_anim_state = WILLBOSS_WALKATAK1;
				else
					item->goal_anim_state = WILLBOSS_WALKATAK2;
			}

			break;

		case WILLBOSS_LUNGE:
			willy->target.x = lara_item->pos.x_pos;
			willy->target.z = lara_item->pos.z_pos;
			willy->maximum_turn = 364;

			if (!willy->flags && item->touch_bits & 0x900000)
			{
				lara_item->hit_points -= 440;
				lara_item->hit_status = 1;
				CreatureEffect(item, &willboss_bite_left, DoBloodSplat);
				CreatureEffect(item, &willboss_bite_right, DoBloodSplat);
				willy->flags = 1;
			}

			break;

		case WILLBOSS_BIGKILL:

			switch (item->frame_number - anims[item->anim_number].frame_base)
			{
			case 0:
			case 43:
			case 95:
			case 105:
				CreatureEffect(item, &willboss_bite_left, DoBloodSplat);
				break;

			case 61:
			case 91:
			case 101:
				CreatureEffect(item, &willboss_bite_right, DoBloodSplat);
				break;
			}

			break;

		case WILLBOSS_WALKATAK1:
		case WILLBOSS_WALKATAK2:

			if (!willy->flags && (item->touch_bits & 0x900000) != 0)
			{
				lara_item->hit_points -= 220;
				lara_item->hit_status = 1;
				CreatureEffect(item, &willboss_bite_left, DoBloodSplat);
				CreatureEffect(item, &willboss_bite_right, DoBloodSplat);
				willy->flags = 1;
			}

			if (fire && info.bite && dist < 0x1000000)
				item->goal_anim_state = WILLBOSS_WALK;
			else if (info.bite && info.distance < 0x240000)
			{
				if (item->current_anim_state == WILLBOSS_WALKATAK1)
					item->goal_anim_state = WILLBOSS_WALKATAK2;
				else
					item->goal_anim_state = WILLBOSS_WALKATAK1;
			}
			else
				item->goal_anim_state = WILLBOSS_WALK;

			break;

		case WILLBOSS_180:
			willy->maximum_turn = 0;
			willy->flags = 0;

			if (item->frame_number == anims[item->anim_number].frame_base + 51)
			{
				item->pos.y_rot += 0x8000;
				direction = -direction;
			}

			break;

		case WILLBOSS_SHOOT:
			willy->target.x = lara_item->pos.x_pos;
			willy->target.z = lara_item->pos.z_pos;
			willy->maximum_turn = 364;

			if (item->frame_number - anims[item->anim_number].frame_base == 40 && lara_item->hit_points > 0)
			{
				pos.x = -64;
				pos.y = 410;
				pos.z = 0;
				GetJointAbsPosition(item, &pos, 20);
				TriggerPlasmaBall(&pos, item->room_number, item->pos.y_rot - 4096, 0);

				pos.x = 64;
				pos.y = 410;
				pos.z = 0;
				GetJointAbsPosition(item, &pos, 23);
				TriggerPlasmaBall(&pos, item->room_number, item->pos.y_rot + 4096, 0);
			}

			f = item->frame_number - anims[item->anim_number].frame_base;

			if (f > 16)
			{
				f = anims[item->anim_number].frame_end - item->frame_number;

				if (f > 16)
					f = 16;
			}

			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(item, &pos, 17);

			b = GetRandomControl();
			r = (f * (b & 0x3F)) >> 4;
			g = (f * (255 - ((b >> 4) & 0x1F))) >> 4;
			b = (f * (192 - ((b >> 6) & 0x1F))) >> 4;

			TriggerDynamic(pos.x, pos.y, pos.z, 12, r, g, b);
			TriggerPlasma(item_number, 7, f << 2);
			TriggerPlasma(item_number, 8, f << 2);
			break;
		}

		if (lara_alive && lara_item->hit_points <= 0)
		{
			CreatureKill(item, 6, WILLBOSS_BIGKILL, EXTRA_YETIKILL);
			willy->maximum_turn = 0;
			return;
		}
	}

	angle = CreatureTurn(item, willy->maximum_turn);
	CreatureAnimation(item_number, angle, 0);
}

void S_DrawWillBoss(ITEM_INFO* item)
{
	DrawAnimatingItem(item);

	if (bossdata.explode_count)
	{
		DrawExplosionRings();

		if (bossdata.explode_count)
		{
			if (bossdata.explode_count <= 64)
				DrawWillBossShield(item);
		}
	}
}
