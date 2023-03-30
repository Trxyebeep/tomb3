#include "../tomb3/pch.h"
#include "lasers.h"
#include "control.h"
#include "draw.h"
#include "../specific/game.h"
#include "objects.h"
#include "../specific/draweffects.h"
#include "effects.h"
#include "lara.h"
#include "box.h"

uchar LaserShades[32];

static void LaserSplitterToggle(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	long active, x, z, xStep, zStep;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (!(boxes[floor->box].overlap_index & 0x8000))
		return;

	active = TriggerActive(item);

	if (active == ((boxes[floor->box].overlap_index & 0x4000) == 0x4000))
		return;

	if (!item->pos.y_rot)
	{
		xStep = 0;
		zStep = -1024;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		xStep = -1024;
		zStep = 0;
	}
	else if (item->pos.y_rot == -0x8000)
	{
		xStep = 0;
		zStep = 1024;
	}
	else
	{
		xStep = 1024;
		zStep = 0;
	}

	x = item->pos.x_pos;
	z = item->pos.z_pos;

	while (floor->box != 2047 && boxes[floor->box].overlap_index & 0x8000)
	{
		if (active)
			boxes[floor->box].overlap_index |= 0x4000;
		else
			boxes[floor->box].overlap_index &= ~0x4000;

		x += xStep;
		z += zStep;
		floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	}
}

long LaraOnLOS(GAME_VECTOR* s, GAME_VECTOR* t)
{
	ITEM_INFO* l;
	long dx, dy, dz, x, y, z, dist, flag, lp;
	short* bounds;
	short* xextent;
	short* zextent;

	l = lara_item;
	dx = t->x - s->x;
	dy = t->y - s->y;
	dz = t->z - s->z;
	bounds = GetBoundsAccurate(l);

	if ((l->pos.y_rot + 0x2000) & 0x4000)
	{
		xextent = &bounds[4];
		zextent = &bounds[0];
	}
	else
	{
		xextent = &bounds[0];
		zextent = &bounds[4];
	}

	flag = 0;

	if (abs(dz) > abs(dx))
	{
		dist = l->pos.z_pos - s->z + zextent[0];

		for (lp = 0; lp < 2; lp++)
		{
			if ((dz ^ dist) >= 0)
			{
				y = dy * dist / dz;

				if (y > l->pos.y_pos - s->y + bounds[2] && y < l->pos.y_pos - s->y + bounds[3])
				{
					x = dx * dist / dz;

					if (x < l->pos.x_pos + xextent[0] - s->x)
						flag |= 1;
					else if (x > l->pos.x_pos + xextent[1] - s->x)
						flag |= 2;
					else
						return 1;
				}
			}

			dist = l->pos.z_pos - s->z + zextent[1];
		}

		if (flag == 3)
			return 1;
	}
	else
	{
		dist = l->pos.x_pos + xextent[0] - s->x;

		for (lp = 0; lp < 2; lp++)
		{
			if ((dx ^ dist) >= 0)
			{
				y = dy * dist / dx;

				if (y > l->pos.y_pos - s->y + bounds[2] && y < l->pos.y_pos - s->y + bounds[3])
				{
					z = dz * dist / dx;

					if (z < l->pos.z_pos - s->z + zextent[0])
						flag |= 1;
					else if (z > l->pos.z_pos - s->z + zextent[1])
						flag |= 2;
					else
						return 1;
				}
			}

			dist = l->pos.x_pos + xextent[1] - s->x;
		}

		if (flag == 3)
			return 1;
	}

	return 0;
}

void UpdateLaserShades()
{
	long r, lp;
	uchar s;

	for (lp = 0; lp < 32; lp++)
	{
		s = LaserShades[lp];
		r = GetRandomDraw();

		if (r < 1024)
			r = (r & 0xF) + 16;
		else if (r < 4096)
			r &= 7;
		else if (!(r & 0x70))
			r &= 3;
		else
			r = 0;

		if (r)
		{
			s += (uchar)r;

			if (s > 127)
				s = 127;
		}
		else if (s > 16)
			s -= s >> 3;
		else
			s = 16;

		LaserShades[lp] = s;
	}
}

void LaserControl(short item_number)
{
	LaserSplitterToggle(&items[item_number]);
}

void S_DrawLaser(ITEM_INFO* item)
{
	ITEM_INFO* target;
	GAME_VECTOR s, t;
	long x, y, z, lp, lp2;

	if (!TriggerActive(item))
		return;

	if (!item->pos.y_rot)
	{
		x = 0;
		z = 511;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		x = 511;
		z = 0;
	}
	else if (item->pos.y_rot == -0x8000)
	{
		x = 0;
		z = -511;
	}
	else
	{
		x = -511;
		z = 0;
	}

	y = 0;

	for (lp = 0; lp < item->hit_points; lp++)
	{
		s.x = item->pos.x_pos + x;
		s.y = item->pos.y_pos + y;
		s.z = item->pos.z_pos + z;
		s.room_number = item->room_number;

		t.x = item->pos.x_pos - (x << 5);
		t.y = item->pos.y_pos + y;
		t.z = item->pos.z_pos - (z << 5);

		LOS(&s, &t);

		if (LaraOnLOS(&s, &t))
		{
			if (item->object_number != SECURITY_LASER_ALARM)
			{
				if (item->object_number == SECURITY_LASER_KILLER)
					lara_item->hit_points = 0;
				else
					lara_item->hit_points -= 10;

				DoLotsOfBloodD(lara_item->pos.x_pos, item->pos.y_pos + y, lara_item->pos.z_pos, (GetRandomDraw() & 0x7F) + 128,
					short(GetRandomDraw() << 1), lara_item->room_number, 1);
			}

			if (!item->pos.y_rot || item->pos.y_rot == -0x8000)
				t.z = lara_item->pos.z_pos;
			else
				t.x = lara_item->pos.x_pos;

			for (lp2 = 0; lp2 < level_items; lp2++)
			{
				target = &items[lp2];

				if ((target->object_number == STROBE_LIGHT || target->object_number == ROBOT_SENTRY_GUN) && TriggerActive(target))
					target->really_active = 1;
			}
		}

		if (item->object_number == SECURITY_LASER_ALARM)
			S_DrawLaserBeam(&s, &t, 16, 0, 16);
		else if (item->object_number == SECURITY_LASER_DEADLY)
			S_DrawLaserBeam(&s, &t, 0, 0, 16);
		else
			S_DrawLaserBeam(&s, &t, 0, 2, 16);

		y -= 256;
	}
}
