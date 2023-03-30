#include "../tomb3/pch.h"
#include "moveblok.h"
#include "control.h"
#include "collide.h"
#include "draw.h"
#include "items.h"
#include "effects.h"
#include "sound.h"
#include "box.h"
#include "lara.h"
#include "laramisc.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/input.h"

static short MovingBlockBounds[12] = { -300, 300, 0, 0, -692, -512, -1820, 1820, -5460, 5460, -1820, 1820 };

void ClearMovableBlockSplitters(long x, long y, long z, short room_number)
{
	FLOOR_INFO* floor;
	short room_num, height;

	floor = GetFloor(x, y, z, &room_number);
	boxes[floor->box].overlap_index &= ~0x4000;
	height = boxes[floor->box].height;
	room_num = room_number;
	floor = GetFloor(x + WALL_SIZE, y, z, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x + WALL_SIZE, y, z, room_number);
	}

	room_number = room_num;
	floor = GetFloor(x - WALL_SIZE, y, z, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x - WALL_SIZE, y, z, room_number);
	}

	room_number = room_num;
	floor = GetFloor(x, y, z + WALL_SIZE, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x, y, z + WALL_SIZE, room_number);
	}

	room_number = room_num;
	floor = GetFloor(x, y, z - WALL_SIZE, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x, y, z - WALL_SIZE, room_number);
	}
}

void AlterFloorHeight(ITEM_INFO* item, long height)
{
	FLOOR_INFO* floor;
	FLOOR_INFO* ceiling;
	short room_num;

	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	ceiling = GetFloor(item->pos.x_pos, item->pos.y_pos + height - WALL_SIZE, item->pos.z_pos, &room_num);

	if (floor->floor == -127)
		floor->floor = ceiling->ceiling + char(height >> 8);
	else
	{
		floor->floor += char(height >> 8);

		if (floor->floor == ceiling->ceiling)
			floor->floor = -127;
	}

	if (boxes[floor->box].overlap_index & 0x8000)
	{
		if (height >= 0)
			boxes[floor->box].overlap_index &= ~0x4000;
		else
			boxes[floor->box].overlap_index |= 0x4000;
	}
}

static long TestBlockMovable(ITEM_INFO* item, long blockhite)
{
	FLOOR_INFO* floor;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (floor->floor == -127 || floor->floor << 8 == item->pos.y_pos - blockhite)
		return 1;

	return 0;
}

static long TestBlockPush(ITEM_INFO* item, long blockhite, ushort quadrant)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	COLL_INFO coll;
	long x, y, z;
	short room_number;

	if (!TestBlockMovable(item, blockhite))
		return 0;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	room_number = item->room_number;

	switch (quadrant)
	{
	case NORTH:
		z += WALL_SIZE;
		break;

	case EAST:
		x += WALL_SIZE;
		break;

	case SOUTH:
		z -= WALL_SIZE;
		break;

	case WEST:
		x -= WALL_SIZE;
		break;
	}

	floor = GetFloor(x, y, z, &room_number);
	r = &room[room_number];

	if (r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper)
		return 0;

	coll.quadrant = quadrant;
	coll.radius = 500;

	if (CollideStaticObjects(&coll, x, y, z, room_number, 1000))
		return 0;

	if (floor->floor << 8 != y)
		return 0;

	GetHeight(floor, x, y, z);

	if (height_type != WALL)
		return 0;

	y -= blockhite - 100;
	floor = GetFloor(x, y, z, &room_number);

	if (GetCeiling(floor, x, y, z) > y)
		return 0;

	item->item_flags[0] = lara_item->pos.y_rot;
	return 1;
}

static long TestBlockPull(ITEM_INFO* item, long blockhite, ushort quadrant)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	COLL_INFO coll;
	long x, y, y2, z, xAdd, zAdd;
	short room_number;

	if (!TestBlockMovable(item, blockhite))
		return 0;

	xAdd = 0;
	zAdd = 0;

	switch (quadrant)
	{
	case NORTH:
		zAdd = -WALL_SIZE;
		break;

	case EAST:
		xAdd = -WALL_SIZE;
		break;

	case SOUTH:
		zAdd = WALL_SIZE;
		break;

	case WEST:
		xAdd = WALL_SIZE;
		break;
	}

	x = item->pos.x_pos + xAdd;
	y = item->pos.y_pos;
	z = item->pos.z_pos + zAdd;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	r = &room[room_number];

	if (r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper)
		return 0;

	coll.quadrant = quadrant;
	coll.radius = 500;

	if (CollideStaticObjects(&coll, x, y, z, room_number, 1000))
		return 0;

	if (floor->floor << 8 != y)
		return 0;

	y2 = y - blockhite;
	floor = GetFloor(x, y2, z, &room_number);

	if (floor->ceiling << 8 > y2)
		return 0;

	x += xAdd;
	z += zAdd;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);

	if (floor->floor << 8 != y)
		return 0;

	y2 = y - 762;
	floor = GetFloor(x, y2, z, &room_number);

	if (floor->ceiling << 8 > y2)
		return 0;

	x = lara_item->pos.x_pos + xAdd;
	y = lara_item->pos.y_pos;
	z = lara_item->pos.z_pos + zAdd;
	room_number = lara_item->room_number;
	GetFloor(x, y, z, &room_number);

	coll.quadrant = (quadrant - 2) & 3;
	coll.radius = 100;

	if (CollideStaticObjects(&coll, x, y, z, room_number, 762))
		return 0;

	item->item_flags[0] = lara_item->pos.y_rot + 0x8000;
	return 1;
}

void DrawUnclippedItem(ITEM_INFO* item)
{
	long t, b, l, r;

	t = phd_top;
	l = phd_left;
	b = phd_bottom;
	r = phd_right;
	phd_top = 0;
	phd_left = 0;
	phd_bottom = phd_winymax;
	phd_right = phd_winxmax;
	DrawAnimatingItem(item);
	phd_top = t;
	phd_left = l;
	phd_bottom = b;
	phd_right = r;
}

void SetupCleanerFromSavegame(ITEM_INFO* item, long block)
{
	ROOM_INFO* r;
	long x, y, z, xAdd, zAdd;
	short room_number;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;

	if (!block && (x & WALL_MASK) == 512 && (z & WALL_MASK) == 512)
	{
		room_number = item->room_number;
		GetFloor(x, y, z, &room_number);
		r = &room[room_number];
		r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 1;
		return;
	}

	if (!item->pos.y_rot)
	{
		if (!block)
		{
			if ((z & WALL_MASK) < 512)
				z = (z & ~WALL_MASK) - 512;
			else
				z = (z & ~WALL_MASK) + 512;
		}

		xAdd = 0;
		zAdd = WALL_SIZE;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		if (!block)
		{
			if ((x & WALL_MASK) < 512)
				x = (x & ~WALL_MASK) - 512;
			else
				x = (x & ~WALL_MASK) + 512;
		}

		xAdd = WALL_SIZE;
		zAdd = 0;
	}
	else if (item->pos.y_rot == 0x8000)
	{
		if (!block)
		{
			if ((z & WALL_MASK) > 512)
				z |= WALL_MASK;
			else
				z &= ~WALL_MASK;

			z += 512;
		}

		xAdd = 0;
		zAdd = -WALL_SIZE;
	}
	else
	{
		if (!block)
		{
			if ((x & WALL_MASK) > 512)
				x |= WALL_MASK;
			else
				x &= ~WALL_MASK;

			x += 512;
		}

		xAdd = -WALL_SIZE;
		zAdd = 0;
	}

	room_number = item->room_number;
	GetFloor(x, y, z, &room_number);
	r = &room[room_number];
	r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 1;

	x += xAdd;
	z += zAdd;
	room_number = item->room_number;
	GetFloor(x, y, z, &room_number);
	r = &room[room_number];
	r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 1;
}

void InitialiseMovingBlock(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	ClearMovableBlockSplitters(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (item->status != ITEM_INVISIBLE)
		AlterFloorHeight(item, -WALL_SIZE);
}

void MovableBlock(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long h;
	short room_number;

	item = &items[item_number];

	if (item->flags & IFL_INVISIBLE)
	{
		AlterFloorHeight(item, WALL_SIZE);
		KillItem(item_number);
		return;
	}

	AnimateItem(item);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos < h)
		item->gravity_status = 1;
	else if (item->gravity_status)
	{
		item->pos.y_pos = h;
		item->gravity_status = 0;
		item->status = ITEM_DEACTIVATED;
		floor_shake_effect(item);
		SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
	}

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (item->status == ITEM_DEACTIVATED)
	{
		item->status = ITEM_INACTIVE;
		RemoveActiveItem(item_number);
		AlterFloorHeight(item, -WALL_SIZE);
		AdjustStopperFlag(item, item->item_flags[0] + 0x8000, 0);

		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
		TestTriggers(trigger_index, 1);
	}
}

void MovableBlockCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	ushort quadrant;
	short room_number;

	item = &items[item_number];

	if (!(input & IN_ACTION) || item->status == ITEM_ACTIVE || l->gravity_status || l->pos.y_pos != item->pos.y_pos)
		return;

	quadrant = ushort(l->pos.y_rot + 0x2000) >> 14;

	if (l->current_anim_state == AS_STOP)
	{
		if (lara.gun_status != LG_ARMLESS)
			return;

		switch (quadrant)
		{
		case NORTH:
			item->pos.y_rot = 0;
			break;

		case EAST:
			item->pos.y_rot = 0x4000;
			break;

		case SOUTH:
			item->pos.y_rot = -0x8000;
			break;

		case WEST:
			item->pos.y_rot = -0x4000;
			break;
		}

		if (!TestLaraPosition(MovingBlockBounds, item, l))
			return;

		room_number = l->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

		if (item->room_number != room_number)
			return;

		switch (quadrant)
		{
		case NORTH:
			l->pos.z_pos = (l->pos.z_pos & ~WALL_MASK) + WALL_SIZE - 100;
			break;

		case EAST:
			l->pos.x_pos = (l->pos.x_pos & ~WALL_MASK) + WALL_SIZE - 100;
			break;

		case SOUTH:
			l->pos.z_pos = (l->pos.z_pos & ~WALL_MASK) + 100;
			break;

		case WEST:
			l->pos.x_pos = (l->pos.x_pos & ~WALL_MASK) + 100;
			break;
		}

		l->pos.y_rot = item->pos.y_rot;
		l->goal_anim_state = AS_PPREADY;
		AnimateLara(l);

		if (l->current_anim_state == AS_PPREADY)
			lara.gun_status = LG_HANDSBUSY;

		return;
	}
	
	if (l->current_anim_state == AS_PPREADY && l->frame_number == anims[ANIM_PPREADY].frame_base + 19 && TestLaraPosition(MovingBlockBounds, item, l))
	{
		if (input & IN_FORWARD)
		{
			if (!TestBlockPush(item, WALL_SIZE, quadrant))
				return;

			item->goal_anim_state = 2;
			l->goal_anim_state = AS_PUSHBLOCK;
		}
		else if (input & IN_BACK)
		{
			if (!TestBlockPull(item, WALL_SIZE, quadrant))
				return;

			item->goal_anim_state = 3;
			l->goal_anim_state = AS_PULLBLOCK;
		}
		else
			return;

		AddActiveItem(item_number);
		AlterFloorHeight(item, WALL_SIZE);
		AdjustStopperFlag(item, item->item_flags[0], 1);
		item->status = ITEM_ACTIVE;
		AnimateItem(item);
		AnimateLara(l);
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
	}
}

void DrawMovableBlock(ITEM_INFO* item)
{
	if (item->status == ITEM_ACTIVE)
		DrawUnclippedItem(item);
	else
		DrawAnimatingItem(item);
}
