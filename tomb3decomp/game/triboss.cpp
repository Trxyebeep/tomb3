#include "../tomb3/pch.h"
#include "triboss.h"
#include "objects.h"
#include "sphere.h"
#include "../3dsystem/phd_math.h"
#include "box.h"

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

void inject_triboss(bool replace)
{
	INJECT(0x00471FB0, FindLizardManItemNumber, replace);
	INJECT(0x00471570, InitialiseTribeBoss, replace);
	INJECT(0x00471960, RotateHeadXAngle, replace);
}
