#include "../tomb3/pch.h"
#include "fish.h"
#include "gameflow.h"
#include "../specific/game.h"
#include "control.h"
#include "objects.h"
#include "effects.h"
#include "lara.h"
#include "../specific/smain.h"

static uchar jungle_fish_ranges[1][3] =
{
	{ 8, 20, 3 }
};

static uchar temple_fish_ranges[3][3] =
{
	{ 4, 4, 2 },
	{ 4, 16, 2 },
	{ 4, 28, 3 }
};

static uchar quadchase_fish_ranges[8][3] =
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

static uchar house_fish_ranges[7][3]
{
	{ 4, 4, 1 },
	{ 16, 8, 2 },
	{ 24, 8, 2 },
	{ 8, 16, 2 },
	{ 8, 12, 1 },
	{ 20, 8, 2 },
	{ 16, 8, 1 }
};

static uchar shore_fish_ranges[3][3] =
{
	{ 12, 12, 6 },
	{ 12, 20, 6 },
	{ 20, 4, 8 }
};

static uchar crash_fish_ranges[1][3]
{
	{ 20, 4, 6 }
};

static uchar rapids_fish_ranges[2][3]
{
	{ 16, 16, 8 },
	{ 4, 8, 5 }
};

FISH_INFO fish[200];
LEADER_INFO lead_info[8];
short CarcassItem;
static long PirahnaHitWait;

void SetupShoal(long shoal_number)
{
	LEADER_INFO* leader;

	leader = &lead_info[shoal_number];

	if (CurrentLevel == LV_JUNGLE)
	{
		leader->Xrange = (jungle_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = jungle_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (jungle_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_TEMPLE)
	{
		leader->Xrange = (temple_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = temple_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (temple_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_QUADBIKE)
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
	else if (CurrentLevel == LV_SHORE)
	{
		leader->Xrange = (shore_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = shore_fish_ranges[shoal_number][2] << 8;
		leader->Zrange = (shore_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_CRASH)
	{
		leader->Xrange = (crash_fish_ranges[shoal_number][0] + 2) << 8;
		leader->Yrange = (crash_fish_ranges[shoal_number][2]) << 8;
		leader->Zrange = (crash_fish_ranges[shoal_number][1] + 2) << 8;
	}
	else if (CurrentLevel == LV_RAPIDS)
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
	dy = abs(pos->y_pos - item->pos.y_pos);
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
	long leader, pirahna_attack, diff, x, z, dx, dz;

	item = &items[item_number];
	enemy = item;

	if (TriggerActive(item))
	{
		leader = item->hit_points;
		pLeader = &lead_info[leader];

		if (!pLeader->on)
			SetupFish(leader, item);

		if (item->object_number == PIRAHNAS)
		{
			if (CurrentLevel != LV_CRASH)
				pirahna_attack = lara_item->room_number == item->room_number;
			else if (CarcassItem != NO_ITEM)
				pirahna_attack = 2;
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
			else
				enemy = lara_item;

			pFish->angle = ((-(long)mGetAngle(pFish->x + item->pos.x_pos, pFish->z + item->pos.z_pos, enemy->pos.x_pos, enemy->pos.z_pos) - 16384) >> 4) & 0xFFF;
			pLeader->angle = pFish->angle;
			pLeader->speed = (GetRandomControl() & 0x3F) - 64;
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

			if (abs(pFish->angadd) < 4)
				pFish->angadd = 0;
		}

		pFish->angle += pFish->angadd;

		if (diff > 1024)
			pFish->angle += pFish->angadd >> 2;

		pFish->angle &= 0xFFF;
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
		x = pFish->x - ((pFish->speed * rcossin_tbl[2 * pFish->angle]) >> 13);
		z = pFish->z + ((pFish->speed * rcossin_tbl[2 * pFish->angle + 1]) >> 13);

		if (!pirahna_attack)
		{
			if (z < -pLeader->Zrange)
			{
				z = -pLeader->Zrange;

				if (pFish->angle < 2048)
					pLeader->angle = pFish->angle - (GetRandomControl() & 0x7F) - 128;
				else
					pLeader->angle = pFish->angle + (GetRandomControl() & 0x7F) + 128;

				pLeader->angle_time = (GetRandomControl() & 0xF) + 8;
				pLeader->speed_time = 0;
			}
			else if (z > pLeader->Zrange)
			{
				z = pLeader->Zrange;

				if (pFish->angle > 3072)
					pLeader->angle = pFish->angle - (GetRandomControl() & 0x7F) - 128;
				else
					pLeader->angle = pFish->angle + (GetRandomControl() & 0x7F) + 128;

				pLeader->angle_time = (GetRandomControl() & 0xF) + 8;
				pLeader->speed_time = 0;
			}

			if (x < -pLeader->Xrange)
			{
				x = -pLeader->Xrange;

				if (pFish->angle < 1024)
					pLeader->angle = pFish->angle - (GetRandomControl() & 0x7F) - 128;
				else
					pLeader->angle = pFish->angle + (GetRandomControl() & 0x7F) + 128;

				pLeader->angle_time = (GetRandomControl() & 0xF) + 8;
				pLeader->speed_time = 0;
			}
			else if (x > pLeader->Xrange)
			{
				x = pLeader->Xrange;

				if (pFish->angle < 3072)
					pLeader->angle = pFish->angle - (GetRandomControl() & 0x7F) - 128;
				else
					pLeader->angle = pFish->angle + (GetRandomControl() & 0x7F) + 128;

				pLeader->angle_time = (GetRandomControl() & 0xF) + 8;
				pLeader->speed_time = 0;
			}

			if (!(GetRandomControl() & 0xF))
				pLeader->angle_time = 0;

			if (pLeader->angle_time)
				pLeader->angle_time--;
			else
			{
				pLeader->angle_time = (GetRandomControl() & 0xF) + 8;
				diff = (GetRandomControl() & 0x3F) - 24;

				if (!(GetRandomControl() & 3))
					diff <<= 5;

				pLeader->angle = (pLeader->angle + diff) & 0xFFF;
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
					pLeader->speed += 208 - (GetRandomControl() & 0x1F);
				else
				{
					pLeader->speed_time = (GetRandomControl() & 3) + 4;
					pLeader->speed += (GetRandomControl() & 0x1F) - 15;
				}
			}
		}

		pFish->x = (short)x;
		pFish->z = (short)z;

		for (int i = 0; i < 24; i++)
		{
			pFish = &fish[24 * leader + i + 8];

			if (item->object_number == PIRAHNAS)
			{
				pos.x_pos = item->pos.x_pos + pFish->x;
				pos.y_pos = item->pos.y_pos + pFish->y;
				pos.z_pos = item->pos.z_pos + pFish->z;

				if (FishNearLara(&pos, 256, pirahna_attack >= 2 ? enemy : lara_item))
				{
					if (!PirahnaHitWait)
					{
						DoBloodSplat(item->pos.x_pos + pFish->x, item->pos.y_pos + pFish->y, item->pos.z_pos + pFish->z, 0, 0, pirahna_attack < 2 ? lara_item->room_number : enemy->room_number);
						PirahnaHitWait = 8;
					}

					if (pirahna_attack != 2)
						lara_item->hit_points -= 4;
				}
			}

			dx = SQUARE(pFish->x - x - 128 * i + 3072);
			dz = SQUARE(pFish->z - z + 128 * i - 3072);
			diff = pFish->angle - (((-(long)mGetAngle(pFish->x, pFish->z, x, z) - 16384) >> 4) & 0xFFF);

			if (diff > 2048)
				diff -= 4096;
			else if (diff < -2048)
				diff += 4096;

			if (diff > 128)
			{
				pFish->angadd -= 4;

				if (pFish->angadd < -(i >> 1) - 92)
					pFish->angadd = -(i >> 1) - 92;
			}
			else if (diff < -128)
			{
				pFish->angadd += 4;

				if (pFish->angadd > (i >> 1) + 92)
					pFish->angadd = (i >> 1) + 92;
			}
			else
			{
				pFish->angadd -= pFish->angadd >> 2;

				if (abs(pFish->angadd) < 4)
					pFish->angadd = 0;
			}

			pFish->angle += pFish->angadd;

			if (diff > 1024)
				pFish->angle += pFish->angadd >> 2;

			pFish->angle &= 0xFFF;

			if (dx + dz < 16384 * SQUARE(i) + 1048576)
			{
				if (pFish->speed > 2 * i + 32)
					pFish->speed -= pFish->speed >> 5;
			}
			else
			{
				if (pFish->speed < (i >> 1) + 160)
					pFish->speed += uchar((i >> 1) + (GetRandomControl() & 3) + 1);

				if (pFish->speed > (i >> 1) - 4 * i + 160)
					pFish->speed = (i >> 1) - 4 * i - 96;
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
			dx = pFish->x - ((pFish->speed * rcossin_tbl[2 * pFish->angle]) >> 13);
			dz = pFish->z + ((pFish->speed * rcossin_tbl[2 * pFish->angle + 1]) >> 13);

			if (dz < -32000)
				dz = -32000;
			else if (dz > 32000)
				dz = 32000;

			if (dx < -32000)
				dx = -32000;
			else if (dx > 32000)
				dx = 32000;

			pFish->x = (short)dx;
			pFish->z = (short)dz;

			if (!pirahna_attack)
			{
				if (abs(pFish->y - pFish->desty) < 16)
					pFish->desty = GetRandomControl() % pLeader->Yrange;
			}
			else if (abs(pFish->y - pFish->desty) < 16)
				pFish->desty = short(enemy->pos.y_pos - item->pos.y_pos + (GetRandomControl() & 0xFF));

			pFish->y += (pFish->desty - pFish->y) >> 4;
		}
	}
}
