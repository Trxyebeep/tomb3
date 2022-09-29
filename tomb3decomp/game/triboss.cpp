#include "../tomb3/pch.h"
#include "triboss.h"
#include "objects.h"
#include "sphere.h"
#include "../3dsystem/phd_math.h"
#include "box.h"
#include "effect2.h"
#include "../specific/game.h"

static long radii[5] = { 200, 400, 500, 500, 475 };
static long heights[5] = { -1536, -1280, -832, -384, 0 };

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
		sptr->Flags = 794;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -4 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 4;
	}
	else
		sptr->Flags = 778;

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

void inject_triboss(bool replace)
{
	INJECT(0x00471FB0, FindLizardManItemNumber, replace);
	INJECT(0x00471570, InitialiseTribeBoss, replace);
	INJECT(0x00471960, RotateHeadXAngle, replace);
	INJECT(0x00471E30, TriggerSummonSmoke, replace);
}
