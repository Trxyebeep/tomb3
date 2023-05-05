#include "../tomb3/pch.h"
#include "triboss.h"
#include "objects.h"
#include "sphere.h"
#include "../3dsystem/phd_math.h"
#include "box.h"
#include "effect2.h"
#include "../specific/game.h"
#include "sound.h"
#include "items.h"
#include "lot.h"
#include "control.h"
#include "draw.h"
#include "laraelec.h"
#include "../specific/draweffects.h"
#include "pickup.h"
#include "setup.h"
#include "lara.h"

BITE_INFO tribeboss_hit[6] =
{
	{ 120, 68, 136, 8 },
	{ 128, -64, 136, 8 },
	{ 8, -120, 136, 8 },
	{ -128, -64, 136, 8 },
	{ -124, 64, 126, 8 },
	{ 8, 32, 400, 8 },
};

SHIELD_POINTS TribeBossShield[40];
PHD_VECTOR TrigDynamics[3];
ITEM_INFO* TribeBossItem;
char TribeBossShieldOn;
char lizard_man_active;
char shield_active;

static long lizman_summon_coords[2][4] =
{
	{ 61952, -5376, 62976, 57344 },
	{ 61952, -5376, 52736, 40960 }
};

static long radii[5] = { 200, 400, 500, 500, 475 };
static long heights[5] = { -1536, -1280, -832, -384, 0 };
static long dradii[5] = { 1600, 5600, 6400, 5600, 1600 };
static long dheights1[5] = { -7680, -4224, -768, 2688, 6144 };
static long dheights2[5] = { -1536, -1152, -768, -384, 0 };
static long death_radii[5];
static long death_heights[5];

static short FindLizardManItemNumber(short room_number)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == LIZARD_MAN && item->room_number == room_number)
			return i;
	}

	return NO_ITEM;
}

static void RotateHeadXAngle(ITEM_INFO* item)
{
	PHD_VECTOR pos;
	PHD_VECTOR lpos;
	long dx, dy, dz;
	short ang;

	lpos.x = 0;
	lpos.y = 0;
	lpos.z = 0;
	GetJointAbsPosition(lara_item, &lpos, HIPS);

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, HIPS);

	dx = abs(pos.x - lpos.x);
	dy = pos.y - lpos.y;
	dz = abs(pos.z - lpos.z);
	ang = (short)phd_atan(phd_sqrt(SQUARE(dx) + SQUARE(dz)), dy);

	if (abs(ang) < 0x2000)
		CreatureJoint(item, 2, ang);
	else
		CreatureJoint(item, 2, 0);
}

static void ExplodeTribeBoss(ITEM_INFO* item)
{
	SHIELD_POINTS* p;
	long x, y, z, rad, angle, r, g, b, m;

	TribeBossShieldOn = 0;

	if (bossdata.explode_count == 1 || bossdata.explode_count == 15 || bossdata.explode_count == 25 ||
		bossdata.explode_count == 35 || bossdata.explode_count == 45 || bossdata.explode_count == 55)
	{
		x = (GetRandomDraw() & 0x3FF) + item->pos.x_pos - 512;
		y = item->pos.y_pos - (GetRandomDraw() & 0x3FF) - 256;
		z = (GetRandomDraw() & 0x3FF) + item->pos.z_pos - 512;
		ExpRings[bossdata.ring_count].x = x;
		ExpRings[bossdata.ring_count].y = y;
		ExpRings[bossdata.ring_count].z = z;
		ExpRings[bossdata.ring_count].on = 4;
		bossdata.ring_count++;
		TriggerExplosionSparks(x, y, z, 3, -2, 2, 0);

		for (int i = 0; i < 2; i++)
			TriggerExplosionSparks(x, y, z, 3, -1, 2, 0);

		SoundEffect(SFX_BLAST_CIRCLE, &item->pos, 0x800000 | SFX_SETPITCH);
	}

	for (int i = 0; i < 5; i++)
	{
		if (bossdata.explode_count < 128)
		{
			death_radii[i] = (dradii[i] >> 4) + ((bossdata.explode_count * dradii[i]) >> 7);
			death_heights[i] = dheights2[i] + ((bossdata.explode_count * (dheights1[i] - dheights2[i])) >> 7);
		}
	}

	p = TribeBossShield;

	for (int i = 0; i < 5; i++)
	{
		y = death_heights[i];
		rad = death_radii[i];
		angle = (wibble & 0x3F) << 3;

		for (int j = 0; j < 8; j++, p++)
		{
			p->x = short((rad * rcossin_tbl[angle << 1]) >> 11);
			p->y = (short)y;
			p->z = short((rad * rcossin_tbl[(angle << 1) + 1]) >> 11);

			if (!i || i == 16 || bossdata.explode_count >= 64)
				p->rgb = 0;
			else
			{
				m = 64 - bossdata.explode_count;
				r = (m * (GetRandomDraw() & 0x1F)) >> 6;
				b = (GetRandomDraw() & 0x3F) + 224;
				g = (m * ((b >> 2) + (GetRandomDraw() & 0x3F))) >> 6;
				b = (m * b) >> 6;
				p->rgb = (b << 16) | (g << 8) | r;	//bgr
			}

			angle = (angle + 512) & 0xFFF;
		}
	}
}

static void TribeBossDie(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->collidable = 0;
	item->hit_points = DONT_TARGET;
	KillItem(item_number);
	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE;
}

void InitialiseTribeBoss(short item_number)
{
	ITEM_INFO* item;
	SHIELD_POINTS* p;
	long r, y, angle;

	item = &items[item_number];
	bossdata.LizmanItem = FindLizardManItemNumber(item->room_number);
	bossdata.LizmanRoom = items[bossdata.LizmanItem].room_number;

	for (int i = 0; i < 3; i++)
		TrigDynamics[i].x = 0;

	bossdata.dropped_icon = 0;
	bossdata.dead = 0;
	bossdata.ring_count = 0;
	bossdata.explode_count = 0;
	bossdata.attack_head_count = 0;
	bossdata.death_count = 0;
	bossdata.attack_flag = 0;
	bossdata.attack_count = 0;
	shield_active = 0;
	TribeBossShieldOn = 0;
	lizard_man_active = 0;

	p = TribeBossShield;

	for (int i = 0; i < 5; i++)
	{
		y = heights[i];
		r = radii[i];
		angle = 0;

		for (int j = 0; j < 8; j++, p++, angle += 1024)
		{
			p->x = short((r * rcossin_tbl[angle]) >> 11);
			p->y = (short)y;
			p->z = short((r * rcossin_tbl[angle + 1]) >> 11);
			p->rgb = 0;
		}
	}
}

void TriggerSummonSmoke(long x, long y, long z)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 16;
	sptr->sG = 64;
	sptr->sB = 0;
	sptr->dR = 8;
	sptr->dG = 32;
	sptr->dB = 0;
	sptr->FadeToBlack = 64;
	sptr->ColFadeSpeed = (GetRandomControl() & 7) + 16;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 0xF) + 96;
	sptr->sLife = sptr->Life;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x7F) + x - 64;
	sptr->y = y - (GetRandomControl() & 0x1F);
	sptr->z = (GetRandomControl() & 0x7F) + z - 64;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 0;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -4 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 4;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -8 - (GetRandomControl() & 7);
	sptr->MaxYvel = -4 - (GetRandomControl() & 7);
	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->sWidth = sptr->dWidth >> 1;
	sptr->Width = sptr->dWidth >> 1;
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->sHeight = sptr->dHeight >> 1;
	sptr->Height = sptr->dHeight >> 1;
}

void TriggerLizardMan()
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	short room_number;

	item = &items[bossdata.LizmanItem];
	item->object_number = LIZARD_MAN;
	item->room_number = bossdata.LizmanRoom;
	item->pos.x_pos = lizman_summon_coords[bossdata.attack_type - 1][0];
	item->pos.y_pos = lizman_summon_coords[bossdata.attack_type - 1][1];
	item->pos.z_pos = lizman_summon_coords[bossdata.attack_type - 1][2];
	item->anim_number = objects[LIZARD_MAN].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = anims[item->anim_number].current_anim_state;
	item->goal_anim_state = anims[item->anim_number].current_anim_state;
	item->required_anim_state = 0;
	item->pos.x_rot = 0;

	if (bossdata.attack_type == 1)
		item->pos.y_rot = -0x8000;
	else
		item->pos.y_rot = 0;

	item->pos.z_rot = 0;
	item->timer = 0;
	item->flags = 0;
	item->data = 0;
	item->mesh_bits = -1;
	item->hit_points = objects[LIZARD_MAN].hit_points;
	item->active = 0;
	item->status = ITEM_ACTIVE;
	item->collidable = 1;
	AddActiveItem(bossdata.LizmanItem);
	EnableBaddieAI(bossdata.LizmanItem, 1);

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(bossdata.LizmanItem, room_number);

	lizard_man_active = 1;
	RemoveDrawnItem(bossdata.LizmanItem);

	r = &room[item->room_number];
	item->next_item = r->item_number;
	r->item_number = bossdata.LizmanItem;
}

void TriggerElectricSparks(GAME_VECTOR* pos, long shield)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - pos->x;
	dz = lara_item->pos.z_pos - pos->z;

	if (dx < -0x5000 || dx > 0x5000 || dz < -0x5000 || dz > 0x5000)
		return;

	TrigDynamics[1].x = pos->x;
	TrigDynamics[1].y = pos->y;
	TrigDynamics[1].z = pos->z;
	
	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;

	if (shield)
	{
		sptr->dR = 255;
		sptr->dG = (GetRandomControl() & 0x7F) + 64;
		sptr->dB = 0;
	}
	else if (bossdata.attack_type)
	{
		sptr->dR = 0;
		sptr->dB = (GetRandomControl() & 0x7F) + 64;
		sptr->dG = (sptr->dB >> 1) + 128;
	}
	else
	{
		sptr->dR = 0;
		sptr->dG = (GetRandomControl() & 0x7F) + 64;
		sptr->dB = (sptr->dG >> 1) + 128;
	}

	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 8;
	sptr->Life = 16;
	sptr->sLife = 16;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + pos->x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + pos->y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + pos->z - 16;
	sptr->Xvel = 4 * (GetRandomControl() & 0x1FF) - 1024;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
	sptr->Zvel = 4 * (GetRandomControl() & 0x1FF) - 1024;

	if (shield)
	{
		sptr->Xvel >>= 1;
		sptr->Yvel >>= 1;
		sptr->Zvel >>= 1;
	}

	sptr->Friction = 4;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 3;
	sptr->Width = (GetRandomControl() & 1) + 1;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomControl() & 3) + 4;
	sptr->Height = (GetRandomControl() & 1) + 1;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomControl() & 3) + 4;
	sptr->Gravity = 15;
	sptr->MaxYvel = 0;
}

void FindClosestShieldPoint(long x, long y, long z, ITEM_INFO* item)
{
	SHIELD_POINTS* p;
	GAME_VECTOR pos;
	long affected[5];
	long point, dist, bestdist, dx, dy, dz, c, n;
	long r, g, b;

	affected[0] = 0;
	affected[1] = -1;
	affected[2] = 1;
	affected[3] = -8;
	affected[4] = 8;

	bestdist = 0x7FFFFFFF;
	point = 0;

	for (int i = 0; i < 40; i++)
	{
		p = &TribeBossShield[i];

		if (i >= 16 && i <= 23)
		{
			dx = p->x + item->pos.x_pos - x;
			dy = p->y + item->pos.y_pos - y;
			dz = p->z + item->pos.z_pos - z;
			dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

			if (dist < bestdist)
			{
				bestdist = dist;
				point = i;
			}
		}
	}

	switch (lara.gun_type)
	{
	case LG_PISTOLS:
	case LG_UZIS:
		c = 144;
		break;

	case LG_MAGNUMS:
	case LG_HARPOON:
		c = 200;
		break;

	case LG_SHOTGUN:
	case LG_M16:
		c = 192;
		break;

	case LG_ROCKET:
	case LG_GRENADE:
		c = 224;
		break;

	default:
		c = 0;
		break;
	}

	for (int i = 0; i < 5; i++)
	{
		n = point + affected[i];

		if ((n & 7) == 7 && affected[i] == -1)
			n += 8;

		if (!(n & 7) && affected[i] == 1)
			n -= 8;

		p = &TribeBossShield[n];

		r = p->rgb & 0xFF;
		g = (p->rgb >> 8) & 0xFF;
		b = (p->rgb >> 16) & 0xFF;

		if (i)
		{
			if (c < 200)
			{
				r += c >> 3;

				if (r > c >> 1)
					r = c >> 1;
			}
			else
				r = c >> 1;
		}
		else if (c < 200)
		{
			r += c >> 2;

			if (r > c)
				r = c;
		}
		else
			r = c;

		if (i)
		{
			if (c < 200)
			{
				g += c >> 3;

				if (g > c >> 1)
					g = c >> 1;
			}
			else
				g = c >> 1;
		}
		else if (c < 200)
		{
			g += c >> 2;

			if (g > c)
				g = c;
		}
		else
			g = c;

		if (i)
		{
			if (c < 200)
			{
				b += c >> 3;

				if (b > c >> 1)
					b = c >> 1;
			}
			else
				b = c >> 1;
		}
		else if (c < 200)
		{
			b += c >> 2;

			if (b > c)
				b = c;
		}
		else
			b = c;

		p->rsub = (GetRandomControl() & 7) + 8;
		p->gsub = (GetRandomControl() & 7) + 8;
		p->bsub = (GetRandomControl() & 7) + 8;

		if (lara.gun_type == LG_ROCKET || lara.gun_type == LG_GRENADE)
		{
			p->rsub >>= 1;
			p->gsub >>= 1;
			p->bsub >>= 1;
		}

		p->rgb = (b << 16) | (g << 8) | r;
	}

	pos.x = x;
	pos.y = y;
	pos.z = z;

	for (int i = 0; i < 7; i++)
		TriggerElectricSparks(&pos, 1);
}

void S_DrawTribeBoss(ITEM_INFO* item)
{
	DrawAnimatingItem(item);
	UpdateElectricityPoints();
	TrigDynamics[0].x = 0;
	TrigDynamics[1].x = 0;
	TrigDynamics[2].x = 0;

	if (bossdata.explode_count)
		DrawExplosionRings();
	else
	{
		TriggerTribeBossHeadElectricity(item, 0);
		TriggerTribeBossHeadElectricity(item, 1);
	}

	DrawTribeBossShield(item);
}

void TribeBossControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* triboss;
	EXPLOSION_RING* ring;
	AI_INFO info;
	PHD_VECTOR pos;
	long x, y, z, f, r, g, b, x1, z1, x2, z2;
	ushort ytest;
	short angle, head, yrot;
	static char turned;

	for (int i = 0; i < 3; i++)
	{
		if (!TrigDynamics[i].x)
			continue;

		if (!i)
		{
			x = TrigDynamics[0].x;
			y = TrigDynamics[0].y;
			z = TrigDynamics[0].z;
			f = (GetRandomControl() & 3) + 8;
			r = 0;
			g = (GetRandomControl() & 0x3F) + 64;
			b = (GetRandomControl() & 0x3F) + 128;
		}
		else if (i == 1)
		{
			r = 0;

			if (bossdata.attack_type)
			{
				g = (GetRandomControl() & 0x3F) + 128;
				b = (GetRandomControl() & 0x3F) + 64;
			}
			else
			{
				g = (GetRandomControl() & 0x3F) + 64;
				b = (GetRandomControl() & 0x3F) + 128;
			}

			f = (GetRandomControl() & 7) + 8;
			x = TrigDynamics[1].x;
			y = TrigDynamics[1].y;
			z = TrigDynamics[1].z;
		}
		else
		{
			if (!bossdata.attack_count)
				continue;

			f = (128 - bossdata.attack_count) >> 1;

			if (f > 31)
				f = 31;

			if (f <= 0)
				continue;

			r = f << 1;

			if (bossdata.attack_type)
			{
				g = (GetRandomControl() & 0x3F) + 192;
				b = (GetRandomControl() & 0x3F) + 128;
			}
			else
			{
				g = (GetRandomControl() & 0x3F) + 128;
				b = (GetRandomControl() & 0x3F) + 192;
			}
			
			x = TrigDynamics[2].x;
			y = TrigDynamics[2].y;
			z = TrigDynamics[2].z;
		}

		TriggerDynamic(x, y, z, f, r, g, b);
	}

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	triboss = (CREATURE_INFO*)item->data;
	TribeBossItem = item;
	angle = 0;
	head = 0;
	yrot = 0x7FFF;	//new line.. originally uninitialized!!! (high value to avoid jeopardizing the tests at the end~) (hmmm)

	if (item->hit_points > 0)
	{
		CreatureAIInfo(&items[item_number], &info);

		if (item->hit_status)
			SoundEffect(SFX_TRIBOSS_TAKE_HIT, &item->pos, SFX_DEFAULT);

		yrot = item->pos.y_rot;

		if (!bossdata.attack_flag)
		{
			x = item->pos.x_pos - lara_item->pos.x_pos;
			z = item->pos.z_pos - lara_item->pos.z_pos;

			if (SQUARE(x) + SQUARE(z) < 0x400000)
				bossdata.attack_flag = 1;
		}

		triboss->target.x = lara_item->pos.x_pos;
		triboss->target.z = lara_item->pos.z_pos;

		if (!lizard_man_active || item->current_anim_state)
			angle = CreatureTurn(item, triboss->maximum_turn);
		else
		{
			ytest = item->pos.y_rot;

			if (abs(0xC000 - ytest) > 182)
				item->pos.y_rot += (0xC000 - ytest) >> 3;
			else
				item->pos.y_rot = -0x4000;
		}

		RotateHeadXAngle(item);

		if (info.ahead)
			head = info.angle;

		switch (item->current_anim_state)
		{
		case 0:
			bossdata.attack_count = 0;

			if (item->goal_anim_state != 1 && item->goal_anim_state != 2)
				TribeBossShieldOn = 1;

			if (lizard_man_active)
				CreatureJoint(item, 1, head);
			else
				CreatureJoint(item, 1, 0);

			if (!bossdata.attack_flag || lizard_man_active)
				triboss->maximum_turn = 0;
			else
				triboss->maximum_turn = 546;

			if (item->goal_anim_state != 1 && info.angle > -128 && info.angle < 128 && lara_item->hit_points > 0 &&
				bossdata.attack_head_count < 4 && !lizard_man_active && !shield_active)
			{
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				GetJointAbsPosition(lara_item, &pos, 0);
				bossdata.BeamTarget = pos;
				item->goal_anim_state = 1;
				triboss->maximum_turn = 0;
				TribeBossShieldOn = 0;
				bossdata.attack_head_count++;
				break;
			}

			if (item->goal_anim_state == 2 || bossdata.attack_head_count < 4 || lara_item->hit_points <= 0)
				break;

			triboss->maximum_turn = 0;

			if (!bossdata.attack_type)
			{
				x1 = lara_item->pos.x_pos - lizman_summon_coords[0][0];
				z1 = lara_item->pos.z_pos - lizman_summon_coords[0][2];
				x2 = lara_item->pos.x_pos - lizman_summon_coords[1][0];
				z2 = lara_item->pos.z_pos - lizman_summon_coords[1][2];

				if (SQUARE(x1) + SQUARE(z1) > SQUARE(x2) + SQUARE(z2))
					bossdata.attack_type = 1;
				else
					bossdata.attack_type = 2;
			}

			ytest = item->pos.y_rot;
			y = lizman_summon_coords[bossdata.attack_type - 1][3];

			if (abs(y - ytest) >= 182)
				item->pos.y_rot += short((y - ytest) >> 4);
			else
			{
				item->pos.y_rot = (short)y;

				if (!shield_active)
				{
					item->goal_anim_state = 2;
					bossdata.BeamTarget.x = lizman_summon_coords[bossdata.attack_type - 1][0];
					bossdata.BeamTarget.y = lizman_summon_coords[bossdata.attack_type - 1][1];
					bossdata.BeamTarget.z = lizman_summon_coords[bossdata.attack_type - 1][2];
					triboss->maximum_turn = 0;
					bossdata.attack_head_count = 0;
					TribeBossShieldOn = 0;
				}
			}

			break;

		case 1:
			triboss->maximum_turn = 0;
			bossdata.attack_count += 3;
			bossdata.attack_type = 0;
			CreatureJoint(item, 1, 0);
			break;

		case 2:
			triboss->maximum_turn = 0;

			if (bossdata.attack_count < 64)
				bossdata.attack_count += 2;
			else
				bossdata.attack_count += 3;

			CreatureJoint(item, 1, 0);
			break;
		}
	}
	else
	{
		if (item->current_anim_state != 3)
		{
			item->anim_number = objects[item->object_number].anim_index + 3;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = 3;
			bossdata.death_count = 1;
		}

		if (item->frame_number - anims[item->anim_number].frame_base > 119)
		{
			item->mesh_bits = 0;
			item->frame_number = anims[item->anim_number].frame_base + 120;
			bossdata.death_count = -1;

			if (!bossdata.explode_count)
			{
				bossdata.ring_count = 0;
				SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);

				for (int i = 0; i < 6; i++)
				{
					ring = &ExpRings[i];
					ring->on = 0;
					ring->life = 32;
					ring->radius = 512;
					ring->speed = (i << 5) + 128;
					ring->xrot = ((GetRandomControl() & 0x1FF) - 256) & 0xFFF;
					ring->zrot = ((GetRandomControl() & 0x1FF) - 256) & 0xFFF;
				}

				if (!bossdata.dropped_icon)
				{
					BossDropIcon(item_number);
					bossdata.dropped_icon = 1;
				}
			}

			if (bossdata.explode_count < 256)
				bossdata.explode_count++;

			if (bossdata.explode_count <= 128 || bossdata.ring_count != 6 || ExpRings[5].life)
				ExplodeTribeBoss(item);
			else
			{
				TribeBossDie(item_number);
				bossdata.dead = 1;
			}

			return;
		}

		item->pos.z_rot = GetRandomControl() % bossdata.death_count - (bossdata.death_count >> 1);

		if (bossdata.death_count < 2048)
			bossdata.death_count += 32;
	}

	if (bossdata.attack_count && !bossdata.attack_type && bossdata.attack_count < 64)
		tribeboss_hit[5].z = 4 * bossdata.attack_count + 136;

	triboss->joint_rotation[0] += 1274;
	CreatureAnimation(item_number, angle, 0);

	if (yrot != item->pos.y_rot && !turned)
	{
		turned = 1;
		SoundEffect(SFX_TRIBOSS_TURN_CHAIR, &item->pos, 0x800000 | SFX_SETPITCH);
		return;
	}

	if (yrot == item->pos.y_rot)
		turned = 0;
}
