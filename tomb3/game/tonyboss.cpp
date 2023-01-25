#include "../tomb3/pch.h"
#include "tonyboss.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "sphere.h"
#include "items.h"
#include "sound.h"

static long dradii[5] = { 1600, 5600, 6400, 5600, 1600 };
static long dheights1[5] = { -7680, -4224, -768, 2688, 6144 };
static long dheights2[5] = { -1536, -1152, -768, -384, 0 };
static long death_radii[5];
static long death_heights[5];

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
		angle += (GetRandomControl() & 0x1FFF) + 28672;
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
	
	if ((GetRandomControl() & 1) != 0)
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
		ExpRings[bossdata.ring_count].on = 1;
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

void inject_tonyboss(bool replace)
{
	INJECT(0x0046C460, TriggerTonyFlame, replace);
	INJECT(0x0046C640, TriggerFireBall, replace);
	INJECT(0x0046CD00, TriggerFireBallFlame, replace);
	INJECT(0x0046C1C0, ExplodeTonyBoss, replace);
}
