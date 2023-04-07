#include "../tomb3/pch.h"
#include "londboss.h"
#include "items.h"
#include "../specific/game.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "effect2.h"
#include "lara.h"
#include "../3dsystem/3d_gen.h"
#include "sound.h"
#include "lot.h"
#include "control.h"
#include "effects.h"
#include "box.h"
#include "sphere.h"
#include "pickup.h"
#include "people.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "laraelec.h"
#include "setup.h"

static BITE_INFO londonboss_points[3] =
{
	{ 16, 56, 356, 10 },
	{ -28, 48, 304, 10 },
	{ -72, 48, 356, 10 }
};

static long heights[5] = { -1536, -1280, -832, -384, 0 };
static long radii[5] = { 200, 400, 500, 500, 475 };
static long dradii[5] = { 1600, 5600, 6400, 5600, 1600 };
static long dheights1[5] = { -7680, -4224, -768, 2688, 6144 };
static long dheights2[5] = { -1536, -1152, -768, -384, 0 };
static long death_radii[5];
static long death_heights[5];

SHIELD_POINTS LondonBossShield[40];
EXPLOSION_RING KBRings[3];

static void TriggerPlasmaBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle)
{
	FX_INFO* fx;
	short fx_num;

	fx_num = CreateEffect(room_number);

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		fx->speed = (GetRandomControl() & 0x1F) + 64;
		fx->pos.x_pos = pos->x;
		fx->pos.y_pos = pos->y;
		fx->pos.z_pos = pos->z;
		fx->pos.x_rot = 0x2000;
		fx->pos.y_rot = short(angle + GetRandomControl() + 0x4000);
		fx->object_number = EXTRAFX5;
		fx->fallspeed = 0;
		fx->flag1 = 1;
		fx->flag2 = type == 2;
	}
}

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

	sptr->FxObj = (uchar)fx_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

static void TriggerLaserBolt(PHD_VECTOR* pos, ITEM_INFO* item, long type, short ang)
{
	ITEM_INFO* bolt;
	short item_number;
	short angles[2];

	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	bolt = &items[item_number];
	bolt->object_number = EXTRAFX4;
	bolt->room_number = item->room_number;
	bolt->pos.x_pos = pos->x;
	bolt->pos.y_pos = pos->y;
	bolt->pos.z_pos = pos->z;
	InitialiseItem(item_number);

	if (type == 2)
	{
		bolt->pos.y_pos += item->pos.y_pos - 384;
		bolt->pos.x_rot = short(-pos->y << 5);
		bolt->pos.y_rot = short(GetRandomControl() << 1);
	}
	else
	{
		phd_GetVectorAngles(lara_item->pos.x_pos - pos->x, lara_item->pos.y_pos - pos->y - 256, lara_item->pos.z_pos - pos->z, angles);
		bolt->pos.x_rot = angles[1];
		bolt->pos.y_rot = ang;
		bolt->pos.z_rot = 0;
	}

	if (type == 1)
	{
		bolt->speed = 24;
		bolt->item_flags[0] = 31;
		bolt->item_flags[1] = 16;
	}
	else
	{
		bolt->speed = 16;
		bolt->item_flags[0] = -24;
		bolt->item_flags[1] = 4;

		if (type == 2)
			bolt->item_flags[2] = 1;
	}

	AddActiveItem(item_number);
}

static void ExplodeLondonBoss(ITEM_INFO* item)
{
	SHIELD_POINTS* p;
	long x, y, z, lp, lp2, rad, angle, r, g, b;

	if (bossdata.explode_count == 1 || bossdata.explode_count == 15 || bossdata.explode_count == 25 ||
		bossdata.explode_count == 35 || bossdata.explode_count == 45 || bossdata.explode_count == 55)
	{
		x = (GetRandomDraw() & 0x3FF) + item->pos.x_pos - 512;
		y = item->pos.y_pos - (GetRandomDraw() & 0x3FF) - 256;
		z = (GetRandomDraw() & 0x3FF) + item->pos.z_pos - 512;
		ExpRings[bossdata.ring_count].x = x;
		ExpRings[bossdata.ring_count].y = y;
		ExpRings[bossdata.ring_count].z = z;
		ExpRings[bossdata.ring_count].on = 3;
		bossdata.ring_count++;
		TriggerExplosionSparks(x, y, z, 3, -2, 2, 0);

		for (lp = 0; lp < 2; lp++)
			TriggerExplosionSparks(x, y, z, 3, -1, 2, 0);

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

	p = LondonBossShield;

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
				r = GetRandomDraw() & 0x3F;
				g = (GetRandomDraw() & 0x1F) + 224;
				b = (g >> 2) + (GetRandomDraw() & 0x3F);
				r = ((64 - bossdata.explode_count) * r) >> 6;
				g = ((64 - bossdata.explode_count) * g) >> 6;
				b = ((64 - bossdata.explode_count) * b) >> 6;
				p->rgb = (b << 16) | (g << 8) | r;
			}

			angle = (angle + 512) & 0xFFF;
		}
	}
}

static void LondonBossDie(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->hit_points = DONT_TARGET;
	item->collidable = 0;
	KillItem(item_number);
	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE;
}

long KnockBackCollision(EXPLOSION_RING* ring)
{
	long dx, dz, dist;
	short ang, dy;

	dx = lara_item->pos.x_pos - ring->x;
	dz = lara_item->pos.z_pos - ring->z;

	if (dx > 16000 || dx < -16000 || dz > 16000 || dz < -16000)
		dist = 0x7FFF;
	else
		dist = SQUARE(dx) + SQUARE(dz);

	if (dist >= SQUARE(ring->radius))
		return 0;

	lara_item->hit_points -= 200;
	lara_item->hit_status = 1;
	ang = (short)phd_atan(dz, dx);
	dy = lara_item->pos.y_rot - ang;

	if (abs(dy) >= 0x4000)
	{
		lara_item->pos.y_rot = ang + 0x8000;
		lara_item->speed = -75;
	}
	else
	{
		lara_item->pos.y_rot = ang;
		lara_item->speed = 75;
	}

	lara_item->gravity_status = 1;
	lara_item->fallspeed = -50;
	lara_item->pos.x_rot = 0;
	lara_item->pos.z_rot = 0;
	lara_item->anim_number = ANIM_FALLDOWN;
	lara_item->frame_number = anims[ANIM_FALLDOWN].frame_base;
	lara_item->current_anim_state = AS_FORWARDJUMP;
	lara_item->goal_anim_state = AS_FORWARDJUMP;
	TriggerExplosionSparks(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 3, -2, 2, lara_item->room_number);

	for (int i = 0; i < 3; i++)
		TriggerPlasmaBall(lara_item, 2, (PHD_VECTOR*)&lara_item->pos, lara_item->room_number, short(GetRandomControl() << 1));

	return 1;
}

void ControlLondBossPlasmaBall(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long oldY, speed, h, c, r, g, b;
	short room_number;
	uchar falloffs[2];

	falloffs[0] = 13;
	falloffs[1] = 7;
	fx = &effects[fx_number];
	fx->fallspeed++;
	oldY = fx->pos.y_pos;

	if (fx->speed > 8)
		fx->speed -= 2;

	if (fx->pos.x_rot > -15360)
		fx->pos.x_rot -= 256;

	speed = (fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.x_pos += (speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += fx->fallspeed - ((fx->speed * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	fx->pos.z_pos += (speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

	if (!(wibble & 0xF))
		TriggerPlasmaBallFlame(fx_number, 0, 0, abs(oldY - fx->pos.y_pos) << 3, 0);

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos < c || room[room_number].flags & ROOM_UNDERWATER)
	{
		KillEffect(fx_number);
		return;
	}

	if (!fx->flag2 && ItemNearLara(&fx->pos, 200))
	{
		lara_item->hit_points -= 25;
		lara_item->hit_status = 1;
		KillEffect(fx_number);
		return;
	}

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, room_number);

	c = GetRandomControl();
	r = c & 0x3F;
	g = 255 - ((c >> 4) & 0x1F);
	b = 192 - ((c >> 6) & 0x1F);
	TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, falloffs[fx->flag1], r, g, b);
}

void ControlLaserBolts(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	PHD_VECTOR oldPos;
	long speed, hit, c, extras, lp, dx, dy, dz, dist, g, b, f;
	short oldRoom, room_number;

	item = &items[item_number];
	oldPos.x = item->pos.x_pos;
	oldPos.y = item->pos.y_pos;
	oldPos.z = item->pos.z_pos;
	oldRoom = item->room_number;

	speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	item->pos.x_pos += (speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.y_pos -= (item->speed * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	item->pos.z_pos += (speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

	if (item->speed < 384)
		item->speed += (item->speed >> 3) + 2;

	if (item->item_flags[2] && item->speed > 192)
	{
		item->item_flags[3]++;

		if (item->item_flags[3] >= 16)
		{
			KillItem(item_number);
			return;
		}
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (!item->item_flags[2])
	{
		hit = ItemNearLara(&item->pos, 400);
		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (hit || item->pos.y_pos >= item->floor || item->pos.y_pos <= c)
		{
			SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
			extras = (item->item_flags[0] >= 0) + 2;
			TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, extras, -2, 2, item->room_number);

			for (lp = 0; lp < extras; lp++)
				TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 2, -1, 2, item->room_number);

			extras++;

			for (lp = 0; lp < extras; lp++)
				TriggerPlasmaBall(item, 1, &oldPos, oldRoom, item->pos.y_rot);

			if (hit)
			{
				lara_item->hit_points += -30 - ((item->item_flags[0] >= 0) << 9);
				lara_item->hit_status = 1;
			}
			else
			{
				dx = lara_item->pos.x_pos - item->pos.x_pos;
				dy = lara_item->pos.y_pos - item->pos.y_pos - 256;
				dz = lara_item->pos.z_pos - item->pos.z_pos;
				dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

				if (dist < 1024)
				{
					lara_item->hit_points -= short((1024 - dist) >> (6 - 2 * (item->item_flags[0] >= 0)));
					lara_item->hit_status = 1;
				}
			}

			KillItem(item_number);
			return;
		}
	}

	g = 255 - (GetRandomControl() & 0x3F);
	b = g >> 1;

	if (item->item_flags[0] < 0)
	{
		if (item->item_flags[2])
		{
			f = 16 - item->item_flags[3];
			g = (f * g) >> 4;
			b = (f * b) >> 4;
		}

		f = -item->item_flags[0];

		if (f > 10)
		{
			item->item_flags[1] += 2;
			item->item_flags[0]++;
		}
	}
	else
	{
		f = item->item_flags[0];

		if (f > 16)
		{
			item->item_flags[1] += 4;
			item->item_flags[0]--;
		}
	}

	TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, f, 0, g, b);
}

void InitialiseLondonBoss(short item_number)
{
	SHIELD_POINTS* p;
	long y, rad, angle;

	bossdata.dropped_icon = 0;
	bossdata.dead = 0;
	bossdata.ring_count = 0;
	bossdata.explode_count = 0;
	p = LondonBossShield;

	for (int i = 0; i < 5; i++)
	{
		y = heights[i];
		rad = radii[i];
		angle = 0;

		for (int j = 0; j < 8; j++)
		{
			p->x = short((rad * rcossin_tbl[angle << 1]) >> 11);
			p->y = (short)y;
			p->z = short((rad * rcossin_tbl[(angle << 1) + 1]) >> 11);
			p->rgb = 0;
			angle += 512;
			p++;
		}
	}
}

void LondonBossControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* sophia;
	AI_INFO info;
	AI_INFO lara_info;
	PHD_VECTOR points[3];
	PHD_VECTOR pos;
	long f, r, g, b, lp, x, y, z, d;
	short angle, tilt, head, torso_x, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	sophia = (CREATURE_INFO*)item->data;
	angle = 0;
	tilt = 0;
	head = 0;
	torso_x = 0;
	torso_y = 0;

	if (item->item_flags[2])
	{
		if (item->item_flags[2] == 1)
			item->hit_points = 0;

		if (item->item_flags[2] < 12)
		{
			f = (GetRandomControl() & 1) - (item->item_flags[2] << 1) + 25;
			r = (GetRandomControl() & 0x3F) - (item->item_flags[2] << 3) + 128;
			g = 256 - (item->item_flags[2] << 3);
			b = 255;
		}
		else
		{
			f = (GetRandomControl() & 3) + 8;
			r = 0;
			g = (GetRandomControl() & 0x3F) + 64;
			b = (GetRandomControl() & 0x3F) + 128;
		}

		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, f, r, g, b);
	}

	for (lp = 0; lp < 3; lp++)
	{
		points[lp].x = londonboss_points[lp].x;
		points[lp].y = londonboss_points[lp].y;
		points[lp].z = londonboss_points[lp].z;
		GetJointAbsPosition(item, &points[lp], londonboss_points[lp].mesh_num);
	}

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != LONDONBOSS_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 17;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LONDONBOSS_DEATH;
		}

		if (anims[item->anim_number].frame_end - item->frame_number == 1)
		{
			item->mesh_bits = 0;
			item->frame_number = anims[item->anim_number].frame_end - 1;

			if (!bossdata.explode_count)
			{
				bossdata.ring_count = 0;

				for (lp = 0; lp < 6; lp++)
				{
					ExpRings[lp].on = 0;
					ExpRings[lp].life = 32;
					ExpRings[lp].radius = 512;
					ExpRings[lp].speed = short(128 + (lp << 5));
					ExpRings[lp].xrot = ((GetRandomControl() & 0x1FF) - 256) << 4;
					ExpRings[lp].zrot = ((GetRandomControl() & 0x1FF) - 256) << 4;
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
				LondonBossDie(item_number);
				bossdata.dead = 1;
			}
			else
				ExplodeLondonBoss(item);

			return;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(sophia);

		CreatureAIInfo(item, &info);

		if (sophia->enemy == lara_item)
		{
			lara_info.angle = info.angle;
			lara_info.x_angle = info.x_angle;
			lara_info.distance = info.distance;
		}
		else
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			y = item->pos.y_pos - lara_item->pos.y_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			lara_info.angle = short(phd_atan(z, x) - item->pos.y_rot);
			lara_info.distance = SQUARE(x) + SQUARE(z);

			if (abs(x) <= abs(z))
				z = z + (x >> 1);
			else
				z = x + (z >> 1);

			lara_info.x_angle = (short)phd_atan(z, y);
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, sophia->maximum_turn);

		enemy = sophia->enemy;
		sophia->enemy = lara_item;

		if (item->hit_status || lara_info.distance < 0x100000 || TargetVisible(item, &lara_info) || lara_item->pos.y_pos < item->pos.y_pos)
			AlertAllGuards(item_number);

		sophia->enemy = enemy;

		if (lara_item->pos.y_pos < item->pos.y_pos)
			sophia->hurt_by_lara = 1;

		if (item->timer > 0)
			item->timer--;

		item->hit_points = 300;

		switch (item->current_anim_state)
		{
		case LONDONBOSS_LAUGH:

			if (abs(lara_info.angle) < 728)
				item->pos.y_rot += lara_info.angle;
			else if (lara_info.angle >= 0)
				item->pos.y_rot += 728;
			else
				item->pos.y_rot -= 728;

			if (sophia->alerted)
			{
				item->goal_anim_state = LONDONBOSS_STAND;
				break;
			}

		case LONDONBOSS_STAND:
			sophia->flags = 0;
			sophia->maximum_turn = 0;

			if (sophia->reached_goal)
			{
				sophia->reached_goal = 0;
				item->ai_bits |= AMBUSH;
				item->item_flags[3] += 0x2000;
			}

			head = lara_info.angle;

			if (item->ai_bits & GUARD)
			{
				if ((lara_info.angle < -0x3000 || lara_info.angle > 0x3000) && item->pos.y_pos > -0x2E00)
				{
					item->goal_anim_state = LONDONBOSS_WALK;
					sophia->maximum_turn = 728;
				}
			}
			else if ((item->pos.y_pos <= -0x2E00 || item->pos.y_pos < lara_item->pos.y_pos) &&
				!(GetRandomControl() & 0xF) && !bossdata.charged && item->timer)
				item->goal_anim_state = LONDONBOSS_LAUGH;
			else if (sophia->reached_goal || lara_item->pos.y_pos > item->pos.y_pos || item->pos.y_pos <= -0x2E00)
			{
				if (bossdata.charged)
					item->goal_anim_state = LONDONBOSS_BIGZAP;
				else if (item->timer)
					item->goal_anim_state = LONDONBOSS_LILZAP;
				else
					item->goal_anim_state = LONDONBOSS_SUMMON;
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = LONDONBOSS_WALK;
			else if (sophia->mood == ESCAPE_MOOD || item->pos.y_pos > lara_item->pos.y_pos)
				item->goal_anim_state = LONDONBOSS_RUN;
			else if (sophia->mood == BORED_MOOD || (item->ai_bits & FOLLOW && lara_info.distance > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = LONDONBOSS_STAND;
				else
					item->goal_anim_state = LONDONBOSS_RUN;
			}
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = LONDONBOSS_WALK;
			else
				item->goal_anim_state = LONDONBOSS_RUN;

			break;

		case LONDONBOSS_WALK:
			head = lara_info.angle;
			sophia->flags = 0;
			sophia->maximum_turn = 728;

			if (item->ai_bits & GUARD || (sophia->reached_goal && !(item->ai_bits & FOLLOW)))
				item->goal_anim_state = LONDONBOSS_STAND;
			else if (item->ai_bits & PATROL1)
			{
				item->goal_anim_state = LONDONBOSS_WALK;
				head = 0;
			}
			else if (sophia->mood == ESCAPE_MOOD)
				item->goal_anim_state = LONDONBOSS_RUN;
			else if (sophia->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
				{
					item->required_anim_state = LONDONBOSS_LAUGH;
					item->goal_anim_state = LONDONBOSS_STAND;
				}
			}
			else if (info.distance > 0x100000)
				item->goal_anim_state = LONDONBOSS_RUN;

			break;

		case LONDONBOSS_RUN:

			if (info.ahead)
				head = info.angle;

			sophia->maximum_turn = 1274;
			tilt = angle >> 1;

			if (item->ai_bits & GUARD || (sophia->reached_goal && !(item->ai_bits & FOLLOW)))
				item->goal_anim_state = LONDONBOSS_STAND;
			else if (sophia->mood != ESCAPE_MOOD)
			{
				if (item->ai_bits & FOLLOW && (sophia->reached_goal || lara_info.distance > 0x400000))
					item->goal_anim_state = LONDONBOSS_STAND;
				else if (sophia->mood == BORED_MOOD)
					item->goal_anim_state = LONDONBOSS_WALK;
				else if (info.ahead && info.distance < 0x100000)
					item->goal_anim_state = LONDONBOSS_WALK;
			}

			break;

		case LONDONBOSS_SUMMON:
			head = lara_info.angle;

			if (sophia->reached_goal)
			{
				sophia->reached_goal = 0;
				item->ai_bits = AMBUSH;
				item->item_flags[3] += 0x2000;
			}

			if (item->anim_number == objects[item->object_number].anim_index + 1)
			{
				if (item->frame_number == anims[item->anim_number].frame_base)
				{
					bossdata.hp_counter = item->hit_points;
					item->timer = 600;
				}
				else if (item->hit_status && item->goal_anim_state != LONDONBOSS_STAND)
				{
					StopSoundEffect(SFX_LONDON_BOSS_SUMMON);
					SoundEffect(SFX_LONDON_BOSS_TAKE_HIT, &item->pos, SFX_DEFAULT);
					SoundEffect(SFX_LONDON_BOSS_SUMMON_NOT, &item->pos, SFX_DEFAULT);
					item->goal_anim_state = LONDONBOSS_STAND;
				}
			}
			else if (item->anim_number == objects[item->object_number].anim_index + 2 && item->frame_number == anims[item->anim_number].frame_end)
				bossdata.charged = 1;

			if (abs(lara_info.angle) < 728)
				item->pos.y_rot += lara_info.angle;
			else if (lara_info.angle >= 0)
				item->pos.y_rot += 728;
			else
				item->pos.y_rot -= 728;

			if (!(wibble & 7))
			{
				pos.x = item->pos.x_pos;
				pos.y = (GetRandomControl() & 0x1FF) - 256;
				pos.z = item->pos.z_pos;
				TriggerLaserBolt(&pos, item, 2, 0);

				for (lp = 0; lp < 6; lp++)
				{
					if (!ExpRings[lp].on)
					{
						r = GetRandomControl() & 0x3FF;
						ExpRings[lp].on = 3;
						ExpRings[lp].life = 64;
						ExpRings[lp].speed = (GetRandomControl() & 0xF) + 16;
						ExpRings[lp].x = item->pos.x_pos;
						ExpRings[lp].y = item->pos.y_pos - r + 128;
						ExpRings[lp].z = item->pos.z_pos;
						ExpRings[lp].xrot = 16 * ((GetRandomControl() & 0x1FF) - 256);
						ExpRings[lp].zrot = 16 * ((GetRandomControl() & 0x1FF) - 256);
						ExpRings[lp].radius = short(2048 - abs(r - 512));
						break;
					}
				}
			}

			sophia->maximum_turn = 0;
			break;

		case LONDONBOSS_BIGZAP:

			if (sophia->reached_goal)
			{
				sophia->reached_goal = 0;
				item->ai_bits = AMBUSH;
				item->item_flags[3] += 0x2000;
			}

			bossdata.charged = 0;

			if (abs(lara_info.angle) < 728)
				item->pos.y_rot += lara_info.angle;
			else if (lara_info.angle >= 0)
				item->pos.y_rot += 728;
			else
				item->pos.y_rot -= 728;

			sophia->maximum_turn = 0;
			torso_x = lara_info.x_angle;
			torso_y = lara_info.angle;

			if (item->frame_number == anims[item->anim_number].frame_base + 36)
			{
				TriggerLaserBolt(&points[0], item, 0, item->pos.y_rot + 512);
				TriggerLaserBolt(&points[1], item, 1, item->pos.y_rot);
				TriggerLaserBolt(&points[2], item, 0, item->pos.y_rot - 512);
			}

			break;

		case LONDONBOSS_LILZAP:

			if (sophia->reached_goal)
			{
				sophia->reached_goal = 0;
				item->ai_bits = AMBUSH;
				item->item_flags[3] += 0x2000;
			}

			if (abs(lara_info.angle) < 728)
				item->pos.y_rot += lara_info.angle;
			else if (lara_info.angle >= 0)
				item->pos.y_rot += 728;
			else
				item->pos.y_rot -= 728;

			sophia->maximum_turn = 0;
			torso_x = lara_info.x_angle;
			torso_y = lara_info.angle;

			if (item->frame_number == anims[item->anim_number].frame_base + 14)
			{
				TriggerLaserBolt(&points[0], item, 0, item->pos.y_rot + 512);
				TriggerLaserBolt(&points[2], item, 0, item->pos.y_rot - 512);
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head);

	if (item->current_anim_state >= LONDONBOSS_VAULT2 && item->current_anim_state <= LONDONBOSS_GODOWN || item->current_anim_state == LONDONBOSS_DEATH)
	{
		sophia->maximum_turn = 0;
		CreatureAnimation(item_number, angle, 0);
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 96))
		{
		case -4:
			sophia->maximum_turn = 0;
			item->anim_number = objects[LON_BOSS].anim_index + 21;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LONDONBOSS_GODOWN;
			break;

		case 2:
			sophia->maximum_turn = 0;
			item->anim_number = objects[LON_BOSS].anim_index + 9;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LONDONBOSS_VAULT2;
			break;

		case 3:
			sophia->maximum_turn = 0;
			item->anim_number = objects[LON_BOSS].anim_index + 18;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LONDONBOSS_VAULT3;
			break;

		case 4:
			sophia->maximum_turn = 0;
			item->anim_number = objects[LON_BOSS].anim_index + 15;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = LONDONBOSS_VAULT4;
			break;
		}
	}

	g = (GetRandomControl() & 7) + abs(rcossin_tbl[item->item_flags[1] << 7] >> 7);

	if (g > 31)
		g = 31;

	g <<= 3;
	TriggerDynamic(points[1].x, points[1].y, points[1].z, 10, 0, g >> 1, g >> 2);
	item->item_flags[1] = (item->item_flags[1] + 1) & 0x3F;

	if (item->hit_points > 0 && item->item_flags[0] != 2 && lara_item->hit_points > 0)
	{
		x = lara_item->pos.x_pos - item->pos.x_pos;
		y = lara_item->pos.y_pos - item->pos.y_pos - 256;
		z = lara_item->pos.z_pos - item->pos.z_pos;

		if (x > 8000 || x < -8000 || y > 8000 || y < -8000 || z > 8000 || z < -8000)
			d = 4095;
		else
			d = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));

		if (d < 2816)
		{
			item->item_flags[0] = 2;

			for (lp = 0; lp < 3; lp++)
			{
				KBRings[lp].on = 1;
				KBRings[lp].life = 32;
				KBRings[lp].speed = ((lp == 1) + 1) << 4;
				KBRings[lp].x = item->pos.x_pos;
				KBRings[lp].y = item->pos.y_pos - 512 + (lp << 7);
				KBRings[lp].z = item->pos.z_pos;
				KBRings[lp].xrot = 0;
				KBRings[lp].zrot = 0;
				KBRings[lp].radius = ((lp == 1) + 2) << 8;
			}
		}
	}
	else if (!KBRings[0].on && !KBRings[1].on && !KBRings[2].on)
		item->item_flags[0] = 0;
}

void S_DrawLondonBoss(ITEM_INFO* item)
{
	DrawAnimatingItem(item);

	if (bossdata.explode_count)
	{
		DrawLondonBossShield(item);
		DrawExplosionRings();
	}
	else
	{
		DrawSummonRings();
		DrawKnockBackRings();
	}

	if (item->hit_points <= 0 && !bossdata.explode_count)
	{
		UpdateElectricityPoints();
		LaraElectricDeath(0, item);
		LaraElectricDeath(1, item);
	}
}
