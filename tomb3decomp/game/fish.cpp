#include "../tomb3/pch.h"
#include "fish.h"
#include "gameflow.h"
#include "../specific/game.h"
#include "control.h"
#include "objects.h"
#include "effects.h"
#ifdef RANDO_STUFF
#include "../specific/smain.h"
#endif

uchar jungle_fish_ranges[1][3] =
{
	{ 8, 20, 3 }
};

uchar temple_fish_ranges[3][3] =
{
	{ 4, 4, 2 },
	{ 4, 16, 2 },
	{ 4, 28, 3 }
};

uchar quadchase_fish_ranges[8][3] =
{
	{ 4, 12, 1 },
	{ 0, 12, 2 },
	{ 8, 4, 2 },
	{ 4, 8, 1 },
	{ 4, 16, 2 },
	{ 4, 24, 1 },
	{ 12, 4, 1 },
	{ 16, 4, 1 }
};

uchar house_fish_ranges[7][3]
{
	{ 4, 4, 1 },
	{ 16, 8, 2 },
	{ 24, 8, 2 },
	{ 8, 16, 2 },
	{ 8, 12, 1 },
	{ 20, 8, 2 },
	{ 16, 8, 1 }
};

uchar shore_fish_ranges[3][3] =
{
	{ 12, 12, 6 },
	{ 12, 20, 6 },
	{ 20, 4, 8 }
};

uchar crash_fish_ranges[1][3]
{
	{ 20, 4, 6 }
};

uchar rapids_fish_ranges[2][3]
{
	{ 16, 16, 8 },
	{ 4, 8, 5 }
};

void SetupShoal(long shoal_number)
{
	LEADER_INFO* leader;

	leader = &lead_info[shoal_number];

#ifdef RANDO_STUFF
	if (rando.levels[RANDOLEVEL].original_id == LV_JUNGLE)
#else
	if (CurrentLevel == LV_JUNGLE)
#endif
	{
		leader->Xrange = (jungle_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = jungle_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (jungle_fish_ranges[shoal_number][1] + 2) << 8;
	}
#ifdef RANDO_STUFF
	else if (rando.levels[RANDOLEVEL].original_id == LV_TEMPLE)
#else
	else if (CurrentLevel == LV_TEMPLE)
#endif
	{
		leader->Xrange = (temple_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = temple_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (temple_fish_ranges[shoal_number][1] + 2) << 8;
	}
#ifdef RANDO_STUFF
	else if (rando.levels[RANDOLEVEL].original_id == LV_QUADBIKE)
#else
	else if (CurrentLevel == LV_QUADBIKE)
#endif
	{
		leader->Xrange = (quadchase_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = quadchase_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (quadchase_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_GYM)
	{
		leader->Xrange = (house_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = house_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (house_fish_ranges[shoal_number][1] + 2) << 8;
	}
#ifdef RANDO_STUFF
	else if (rando.levels[RANDOLEVEL].original_id == LV_SHORE)
#else
	else if (CurrentLevel == LV_SHORE)
#endif
	{
		leader->Xrange = (shore_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = shore_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (shore_fish_ranges[shoal_number][1] + 2) << 8;
	}
#ifdef RANDO_STUFF
	else if (rando.levels[RANDOLEVEL].original_id == LV_CRASH)
#else
	else if (CurrentLevel == LV_CRASH)
#endif
	{
		leader->Xrange = (crash_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = (crash_fish_ranges[shoal_number][2]) << 8;
		leader->Zrange = (crash_fish_ranges[shoal_number][1] + 2) << 8;
	}
#ifdef RANDO_STUFF
	else if (rando.levels[RANDOLEVEL].original_id == LV_RAPIDS)
#else
	else if (CurrentLevel == LV_RAPIDS)
#endif
	{
		leader->Xrange = (rapids_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = rapids_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (rapids_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else
	{
		leader->Xrange = 256;
		leader->Zrange = 256;
		leader->Yrange = 256;
	}
}

void SetupFish(long leader, ITEM_INFO* item)
{
	LEADER_INFO* pLeader;
	FISH_INFO* pFish;
	short x, y, z;

	pLeader = &lead_info[leader];
	pFish = &fish[leader];
	x = pLeader->Xrange;
	y = pLeader->Yrange;
	z = pLeader->Zrange;
	pFish->x = 0;
	pFish->y = 0;
	pFish->z = 0;
	pFish->angle = 0;
	pFish->speed = (GetRandomControl() & 0x3F) + 8;
	pFish->swim = GetRandomControl() & 0x3F;

	for (int i = 0; i < 24; i++)
	{
		pFish = &fish[(leader * 24) + 8 + i];
		pFish->x = GetRandomControl() % (x << 1) - x;
		pFish->y = GetRandomControl() % y;
		pFish->z = GetRandomControl() % (z << 1) - z;
		pFish->desty = GetRandomControl() % y;
		pFish->angle = GetRandomControl() & 0xFFF;
		pFish->speed = (GetRandomControl() & 0x1F) + 32;
		pFish->swim = GetRandomControl() & 0x3F;
	}

	pLeader->on = 1;
	pLeader->angle = 0;
	pLeader->speed = (GetRandomControl() & 0x7F) + 32;
	pLeader->angle_time = 0;
	pLeader->speed_time = 0;
}

static long FishNearLara(PHD_3DPOS* pos, long dist, ITEM_INFO* item)
{
	long dx, dy, dz;

	dx = pos->x_pos - item->pos.x_pos;
	dy = ABS(pos->y_pos - item->pos.y_pos);
	dz = pos->z_pos - item->pos.z_pos;

	if (dx < -dist || dx > dist || dz < -dist || dz > dist || dy < -3072 || dy > 3072 || SQUARE(dz) + SQUARE(dx) > SQUARE(dist) || dy > dist)
		return 0;

	return 1;
}

void ControlFish(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	LEADER_INFO* pLeader;
	FISH_INFO* pFish;
	PHD_3DPOS pos;
	long leader, pirahna_attack, diff, x, z, xRange, zRange, angadd, fx, fz, angle, dx, dz;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	leader = item->hit_points;

	pLeader = &lead_info[leader];

	if (!pLeader->on)
		SetupFish(leader, item);

	enemy = lara_item;

	if (item->object_number == PIRAHNAS)
	{

#ifdef RANDO_STUFF
		if (rando.levels[RANDOLEVEL].original_id == LV_CRASH)
#else
		if (CurrentLevel == LV_CRASH)
#endif
		{
			if (CarcassItem == NO_ITEM)
				pirahna_attack = lara_item->room_number == item->room_number;
			else
				pirahna_attack = 2;
		}
		else
			pirahna_attack = lara_item->room_number == item->room_number;
	}
	else
		pirahna_attack = 0;

	if (PirahnaHitWait)
		PirahnaHitWait--;

	pFish = &fish[leader];

	if (pirahna_attack)
	{
		if (pirahna_attack != 1)
			enemy = &items[CarcassItem];

		pFish->angle = (-((long)mGetAngle(pFish->x + item->pos.x_pos, pFish->z + item->pos.z_pos, enemy->pos.x_pos, enemy->pos.z_pos) + 0x4000) >> 4) & 0xFFF;
		pLeader->angle = pFish->angle;
		pLeader->speed = (GetRandomControl() & 63) + 192;
	}

	diff = pFish->angle - pLeader->angle;

	if (diff > 2048)
		diff -= 4096;
	else if (diff < -2048)
		diff += 4096;

	if (diff > 128)
	{
		pFish->angadd -= 4;

		if (pFish->angadd < -120)
			pFish->angadd = -120;
	}
	else if (diff < -128)
	{
		pFish->angadd += 4;

		if (pFish->angadd > 120)
			pFish->angadd = 120;
	}
	else
	{
		pFish->angadd -= pFish->angadd >> 2;

		if (ABS(pFish->angadd) < 4)
			pFish->angadd = 0;
	}

	pFish->angle += pFish->angadd;

	if (diff > 1024)
		pFish->angle += pFish->angadd >> 2;

	pFish->angle &= 4095;

	diff = pFish->speed - pLeader->speed;

	if (diff < -4)
	{
		diff = pFish->speed + (GetRandomControl() & 3) + 1;

		if (diff < 0)
			diff = 0;

		pFish->speed = (uchar)diff;

	}
	else if (diff > 4)
	{
		diff = pFish->speed - (GetRandomControl() & 3) - 1;

		if (diff > 255)
			diff = 255;

		pFish->speed = (uchar)diff;
	}

	pFish->swim = (pFish->swim + (pFish->speed >> 4)) & 0x3F;
	x = pFish->x - ((pFish->speed * rcossin_tbl[pFish->angle << 1]) >> 13);
	z = pFish->z + ((pFish->speed * rcossin_tbl[(pFish->angle << 1) + 1]) >> 13);

	if (!pirahna_attack)
	{
		xRange = pLeader->Xrange;
		zRange = pLeader->Zrange;

		if (z < -zRange)
		{
			z = -zRange;

			if (pFish->angle < 2048)
				pLeader->angle = pFish->angle - (GetRandomControl() & 0x7F) + 128;
			else
				pLeader->angle = pFish->angle + (GetRandomControl() & 0x7F) + 128;

			pLeader->angle_time = (GetRandomControl() & 0xF) + 8;
			pLeader->speed_time = 0;
		}
		else if (z > zRange)
		{
			z = zRange
				;
			if (pFish->angle > 3072)
				pLeader->angle = pFish->angle - (GetRandomControl() & 0x7F) + 128;
			else
				pLeader->angle = pFish->angle + (GetRandomControl() & 0x7F) + 128;

			pLeader->angle_time = (GetRandomControl() & 15) + 8;
			pLeader->speed_time = 0;
		}

		if (x < -xRange)
		{
			x = -xRange;

			if (pFish->angle < 1024)
				pLeader->angle = pFish->angle - ((GetRandomControl() & 127) + 128);
			else
				pLeader->angle = pFish->angle + ((GetRandomControl() & 127) + 128);

			pLeader->angle_time = (GetRandomControl() & 15) + 8;
			pLeader->speed_time = 0;
		}
		else if (x > xRange)
		{
			x = xRange;

			if (pFish->angle < 3072)
				pLeader->angle = pFish->angle - ((GetRandomControl() & 127) + 128);
			else
				pLeader->angle = pFish->angle + ((GetRandomControl() & 127) + 128);

			pLeader->angle_time = (GetRandomControl() & 15) + 8;
			pLeader->speed_time = 0;
		}

		if (!(GetRandomControl() & 15))
			pLeader->angle_time = 0;

		if (pLeader->angle_time)
			pLeader->angle_time--;
		else
		{
			pLeader->angle_time = (GetRandomControl() & 15) + 8;
			angadd = (GetRandomControl() & 0x3F) - 24;

			if (!(GetRandomControl() & 3))
				pLeader->angle += (short)angadd << 5;
			else
				pLeader->angle += (short)angadd;

			pLeader->angle &= 0xFFF;
		}

		if (pLeader->speed_time)
			pLeader->speed_time--;
		else
		{
			pLeader->speed_time = (GetRandomControl() & 0x1F) + 32;

			if (!(GetRandomControl() & 7))
				pLeader->speed = (GetRandomControl() & 0x7F) + 128;
			else if (!(GetRandomControl() & 3))
				pLeader->speed += (GetRandomControl() & 0x7F) + 32;
			else if (pLeader->speed > 140)
				pLeader->speed -= (GetRandomControl() & 0x1F) + 48;
			else
			{
				pLeader->speed_time = (GetRandomControl() & 3) + 4;
				pLeader->speed += (GetRandomControl() & 0x1F) - 15;
			}
		}
	}

	pFish->x = (short)x;
	pFish->z = (short)z;
	fx = x;
	fz = z;

	for (int i = 0; i < 24; i++)
	{
		pFish = &fish[(leader * 24) + 8 + i];

		if (item->object_number == PIRAHNAS)
		{
			pos.x_pos = item->pos.x_pos + pFish->x;
			pos.y_pos = item->pos.y_pos + pFish->y;
			pos.z_pos = item->pos.z_pos + pFish->z;

			if (FishNearLara(&pos, 256, pirahna_attack < 2 ? lara_item : enemy))
			{
				if (!PirahnaHitWait)
				{
					DoBloodSplat(item->pos.x_pos + pFish->x, item->pos.y_pos + pFish->y, item->pos.z_pos + pFish->z, 0, 0,
						pirahna_attack < 2 ? lara_item->room_number : enemy->room_number);
					PirahnaHitWait = 8;
				}

				if (pirahna_attack != 2)
					lara_item->hit_points -= 4;
			}
		}

		angle = (-((long)mGetAngle(pFish->x, pFish->z, fx, fz) + 0x4000) >> 4) & 0xFFF;
		dx = pFish->x - fx + ((24 - i) << 7);
		dz = pFish->z - fz - ((24 - i) << 7);
		dx *= dx;
		dz *= dz;
		diff = pFish->angle - angle;

		if (diff > 2048)
			diff -= 4096;
		else if (diff < -2048)
			diff += 4096;

		if (diff > 128)
		{
			pFish->angadd -= 4;

			if (pFish->angadd < -92 - (i >> 1))
				pFish->angadd = -92 - (i >> 1);
		}
		else	if (diff < -128)
		{
			pFish->angadd += 4;

			if (pFish->angadd > 92 + (i >> 1))
				pFish->angadd = 92 + (i >> 1);
		}
		else
		{
			pFish->angadd -= pFish->angadd >> 2;

			if (ABS(pFish->angadd) < 4)
				pFish->angadd = 0;
		}

		pFish->angle += pFish->angadd;

		if (diff > 1024)
			pFish->angle += pFish->angadd >> 2;

		pFish->angle &= 0xFFF;

		if (dx + dz < SQUARE(i << 7) + 0x100000)
		{
			if (pFish->speed > 32 + (i << 1))
				pFish->speed -= pFish->speed >> 5;
		}
		else
		{
			if (pFish->speed < 160 + (i >> 1))
				pFish->speed += uchar((GetRandomControl() & 3) + 1 + (i >> 1));

			if (pFish->speed > 160 + (i >> 1) - (i << 2))
				pFish->speed = 160 + (i >> 1) - (i << 2);
		}

		if (GetRandomControl() & 1)
			pFish->speed -= GetRandomControl() & 1;
		else
			pFish->speed += GetRandomControl() & 1;

		if (pFish->speed < 32)
			pFish->speed = 32;
		else if (pFish->speed > 200)
			pFish->speed = 200;

		pFish->swim = (pFish->swim + (pFish->speed >> 4) + (pFish->speed >> 5)) & 0x3F;

		x = pFish->x - ((pFish->speed * rcossin_tbl[pFish->angle << 1]) >> 13);
		z = pFish->z + ((pFish->speed * rcossin_tbl[(pFish->angle << 1) + 1]) >> 13);

		if (z < -32000)
			z = -32000;
		else if (z > 32000)
			z = 32000;
		if (x < -32000)
			x = -32000;
		else if (x > 32000)
			x = 32000;

		pFish->x = (short)x;
		pFish->z = (short)z;

		if (!pirahna_attack)
		{
			if (ABS(pFish->y - pFish->desty) < 16)
				pFish->desty = GetRandomControl() % lead_info[leader].Yrange;
		}
		else
		{
			diff = enemy->pos.y_pos - item->pos.y_pos;

			if (ABS(pFish->y - pFish->desty) < 16)
				pFish->desty = short(diff + (GetRandomControl() & 0xFF));
		}

		pFish->y += (pFish->desty - pFish->y) >> 4;
	}
}

void inject_fish(bool replace)
{
	INJECT(0x00430050, SetupShoal, replace);
	INJECT(0x004302B0, SetupFish, replace);
	INJECT(0x00430D30, FishNearLara, replace);
	INJECT(0x004303E0, ControlFish, replace);
}
