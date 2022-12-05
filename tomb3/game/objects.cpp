#include "../tomb3/pch.h"
#include "objects.h"
#include "collide.h"
#include "../specific/init.h"
#include "control.h"
#include "items.h"
#include "sound.h"
#include "missile.h"
#include "effects.h"
#include "../3dsystem/phd_math.h"

long OnDrawBridge(ITEM_INFO* item, long z, long x)
{
	long ix, iz;

	ix = item->pos.x_pos >> WALL_SHIFT;
	iz = item->pos.z_pos >> WALL_SHIFT;
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;

	if (!item->pos.y_rot && x == ix && (z == iz - 1 || z == iz - 2))
		return 1;

	if (item->pos.y_rot == 0x8000 && x == ix && (z == iz + 1 || z == iz + 2))
		return 1;

	if (item->pos.y_rot == 0x4000 && z == iz && (x == ix - 1 || x == ix - 2))
		return 1;

	if (item->pos.y_rot == -0x4000 && z == iz && (x == ix + 1 || x == ix + 2))
		return 1;

	return 0;
}

void DrawBridgeFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	if (item->current_anim_state == 1 && OnDrawBridge(item, z, x) && y <= item->pos.y_pos)
	{
		OnObject = 1;
		*h = item->pos.y_pos;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void DrawBridgeCeiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	if (item->current_anim_state == 1 && OnDrawBridge(item, z, x) && y > item->pos.y_pos)
		*c = item->pos.y_pos + 256;
}

void DrawBridgeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->current_anim_state)
		DoorCollision(item_number, l, coll);
}

void InitialiseLift(short item_number)
{
	ITEM_INFO* item;
	LIFT_INFO* lift;

	item = &items[item_number];
	lift = (LIFT_INFO*)game_malloc(sizeof(LIFT_INFO), 0);
	item->data = lift;
	lift->start_height = item->pos.y_pos;
	lift->wait_time = 0;
}

void LiftControl(short item_number)
{
	ITEM_INFO* item;
	LIFT_INFO* lift;
	short room_number;

	item = &items[item_number];
	lift = (LIFT_INFO*)item->data;

	if (TriggerActive(item))
	{
		if (item->pos.y_pos < lift->start_height + 5616)
		{
			if (lift->wait_time < 90)
			{
				item->goal_anim_state = 1;
				lift->wait_time++;
			}
			else
			{
				item->goal_anim_state = 0;
				item->pos.y_pos += 16;
			}
		}
		else
		{
			item->goal_anim_state = 1;
			lift->wait_time = 0;
		}
	}
	else if (item->pos.y_pos > lift->start_height + 16)
	{
		if (lift->wait_time < 90)
		{
			item->goal_anim_state = 1;
			lift->wait_time++;
		}
		else
		{
			item->goal_anim_state = 0;
			item->pos.y_pos -= 16;
		}
	}
	else
	{
		item->goal_anim_state = 1;
		lift->wait_time = 0;
	}

	AnimateItem(item);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}

void LiftFloorCeiling(ITEM_INFO* item, long x, long y, long z, long* h, long* c)
{
	long ix, iz, lx, lz, lh, lc, inside;

	ix = item->pos.x_pos >> WALL_SHIFT;
	iz = item->pos.z_pos >> WALL_SHIFT;
	lx = lara_item->pos.x_pos >> WALL_SHIFT;
	lz = lara_item->pos.z_pos >> WALL_SHIFT;
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;
	lh = item->pos.y_pos;
	lc = lh - 1280;
	inside = (x == ix || x + 1 == ix) && (z == iz || z - 1 == iz);	//is test point in lift range?
	*h = 0x7FFF;
	*c = -0x7FFF;

	if ((lx == ix || lx + 1 == ix) && (lz == iz || lz - 1 == iz))	//is lara in lift range?
	{
		if (!item->current_anim_state && lara_item->pos.y_pos < lh + 256 && lara_item->pos.y_pos > lc + 256)
		{
			if (inside)
			{
				*h = lh;
				*c = lc + 256;
			}
			else
			{
				*h = NO_HEIGHT;
				*c = 0x7FFF;
			}
		}
		else if (inside)
		{
			if (lara_item->pos.y_pos < lc + 256)
				*h = lc;
			else if (lara_item->pos.y_pos < lh + 256)
			{
				*h = lh;
				*c = lc + 256;
			}
			else
				*c = lh + 256;
		}
	}
	else if (inside)
	{
		if (y <= lc)
			*h = lc;
		else if (y < lh + 256)
		{
			if (item->current_anim_state)
			{
				*h = lh;
				*c = lc + 256;
			}
			else
			{
				*h = NO_HEIGHT;
				*c = 0x7FFF;
			}
		}
		else
			*c = lh + 256;
	}
}

void LiftFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long nH, c;

	LiftFloorCeiling(item, x, y, z, &nH, &c);

	if (nH < *h)
	{
		OnObject = 1;
		*h = nH;
	}
}

void LiftCeiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	long h, nC;

	LiftFloorCeiling(item, x, y, z, &h, &nC);

	if (nC > *c)
		*c = nC;
}

long GetOffset(ITEM_INFO* item, long x, long z)
{
	if (!item->pos.y_rot)
#ifdef TROYESTUFF			// Fixes bridge bug
		return ~x & 0x3FF;
#else
		return -x & 0x3FF;
#endif
	
	if (item->pos.y_rot == -0x8000)
		return x & 0x3FF;
	
	if (item->pos.y_rot == 0x4000)
		return z & 0x3FF;

#ifdef TROYESTUFF
	return ~z & 0x3FF;
#else
	return -z & 0x3FF;
#endif
}

void BridgeFlatFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	if (y <= item->pos.y_pos)
	{
		*h = item->pos.y_pos;
		height_type = WALL;
		OnObject = 1;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void BridgeFlatCeiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	if (y > item->pos.y_pos)
		*c = item->pos.y_pos + 256;
}

void BridgeTilt1Floor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (y <= level)
	{
		*h = level;
		height_type = WALL;
		OnObject = 1;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void BridgeTilt1Ceiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (y > level)
		*c = level + 256;
}

void BridgeTilt2Floor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);

	if (y <= level)
	{
		*h = level;
		height_type = WALL;
		OnObject = 1;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void BridgeTilt2Ceiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);

	if (level < y)
		*c = level + 256;
}

void ShutThatDoor(DOORPOS_DATA* d)
{
	FLOOR_INFO* floor;

	floor = d->floor;

	if (d->floor)
	{
		floor->box = -1;
		floor->ceiling = -127;
		floor->floor = -127;
		floor->index = 0;
		floor->sky_room = -1;
		floor->pit_room = -1;

		if (d->block != 2047)
			boxes[d->block].overlap_index |= 0x4000;
	}
}

void OpenThatDoor(DOORPOS_DATA* d)
{
	if (d->floor)
	{
		*d->floor = d->data;

		if (d->block != 2047)
			boxes[d->block].overlap_index &= ~0x4000;
	}
}

void InitialiseDoor(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;
	ROOM_INFO* r;
	ROOM_INFO* b;
	long dx, dy;
	short box_number, room_number, two_room;

	item = &items[item_number];
	door = (DOOR_DATA*)game_malloc(sizeof(DOOR_DATA), 31);
	item->data = door;
	dx = 0;
	dy = 0;

	if (!item->pos.y_rot)
		dx = -1;
	else if (item->pos.y_rot == -32768)
		dx = 1;
	else if (item->pos.y_rot == 16384)
		dy = -1;
	else
		dy = 1;

	r = &room[item->room_number];
	door->d1.floor = &r->floor[r->x_size * (dy + ((item->pos.x_pos - r->x) >> WALL_SHIFT)) + dx + ((item->pos.z_pos - r->z) >> WALL_SHIFT)];
	room_number = GetDoor(door->d1.floor);

	if (room_number == 255)
		box_number = door->d1.floor->box;
	else
	{
		b = &room[room_number];
		box_number = b->floor[(((item->pos.z_pos - b->z) >> 10) + dx) + (((item->pos.x_pos - b->x) >> 10) + dy) * b->x_size].box;
	}

	door->d1.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
	door->d1.data = *door->d1.floor;

	if (r->flipped_room == -1)
		door->d1flip.floor = 0;
	else
	{
		r = &room[r->flipped_room];
		door->d1flip.floor = &r->floor[(((item->pos.z_pos - r->z) >> 10) + dx) + (((item->pos.x_pos - r->x) >> 10) + dy) * r->x_size];
		room_number = GetDoor(door->d1flip.floor);

		if (room_number == 255)
			box_number = door->d1flip.floor->box;
		else
		{
			b = &room[room_number];
			box_number = b->floor[(((item->pos.z_pos - b->z) >> 10) + dx) + (((item->pos.x_pos - b->x) >> 10) + dy) * b->x_size].box;
		}

		door->d1flip.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
		door->d1flip.data = *door->d1flip.floor;
	}

	two_room = GetDoor(door->d1.floor);
	ShutThatDoor(&door->d1);
	ShutThatDoor(&door->d1flip);

	if (two_room == 255)
	{
		door->d2.floor = 0;
		door->d2flip.floor = 0;
	}
	else
	{
		r = &room[two_room];
		door->d2.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
		room_number = GetDoor(door->d2.floor);

		if (room_number == 255)
			box_number = door->d2.floor->box;
		else
		{
			b = &room[room_number];
			box_number = b->floor[((item->pos.z_pos - b->z) >> 10) + ((item->pos.x_pos - b->x) >> 10) * b->x_size].box;
		}

		door->d2.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
		door->d2.data = *door->d2.floor;

		if (r->flipped_room == -1)
			door->d2flip.floor = 0;
		else
		{
			r = &room[r->flipped_room];
			door->d2flip.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
			room_number = GetDoor(door->d2flip.floor);

			if (room_number == 255)
				box_number = door->d2flip.floor->box;
			else
			{
				b = &room[room_number];
				box_number = b->floor[((item->pos.z_pos - b->z) >> 10) + ((item->pos.x_pos - b->x) >> 10) * b->x_size].box;
			}

			door->d2flip.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
			door->d2flip.data = *door->d2flip.floor;
		}

		ShutThatDoor(&door->d2);
		ShutThatDoor(&door->d2flip);
		room_number = item->room_number;
		ItemNewRoom(item_number, two_room);
		item->room_number = room_number;
	}
}

void DoorControl(short item_number)
{
	ITEM_INFO* item;
	DOOR_DATA* door;

	item = &items[item_number];
	door = (DOOR_DATA*)item->data;

	if (TriggerActive(item))
	{
		if (item->current_anim_state)
		{
			OpenThatDoor(&door->d1);
			OpenThatDoor(&door->d2);
			OpenThatDoor(&door->d1flip);
			OpenThatDoor(&door->d2flip);
		}
		else
			item->goal_anim_state = 1;
	}
	else
	{
		if (item->current_anim_state == 1)
			item->goal_anim_state = 0;
		else
		{
			ShutThatDoor(&door->d1);
			ShutThatDoor(&door->d2);
			ShutThatDoor(&door->d1flip);
			ShutThatDoor(&door->d2flip);
		}
	}

	AnimateItem(item);
}

void SmashWindow(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	BOX_INFO* box;
	long sector;

	item = &items[item_number];
	r = &room[item->room_number];
	sector = ((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT);
	box = &boxes[r->floor[sector].box];

	if (box->overlap_index & 0x8000)
		box->overlap_index &= ~0x4000;

	if (item->object_number == SMASH_WINDOW)
		SoundEffect(SFX_METAL_SHUTTERS_SMASH, &item->pos, SFX_DEFAULT);
	else
	{
		SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
		SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
		item->collidable = 0;
	}

	item->mesh_bits = 0xFFFE;
	item->collidable = 0;
	ExplodingDeath(item_number, 0xFEFE, 0);
	item->flags |= IFL_INVISIBLE;

	if (item->status == ITEM_ACTIVE)
		RemoveActiveItem(item_number);

	item->status = ITEM_DEACTIVATED;
}

void InitialiseWindow(short item_number)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;

	item = &items[item_number];
	item->flags = 0;
	item->mesh_bits = 1;
	r = &room[item->room_number];
	floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];

	if (boxes[floor->box].overlap_index & 0x8000)
		boxes[floor->box].overlap_index |= 0x4000;
}

void WindowControl(short item_number)
{
	ITEM_INFO* item;
	long speed;

	item = &items[item_number];

	if (item->flags & IFL_INVISIBLE)
		return;

	if (lara.skidoo != NO_ITEM)
	{
		if (!ItemNearLara(&item->pos, 512))
			return;
	}
	else
	{
		if (!item->touch_bits)
			return;

		item->touch_bits = 0;
		speed = (lara_item->speed * phd_cos(lara_item->pos.y_rot - item->pos.y_rot)) >> W2V_SHIFT;

		if (abs(speed) < 50)
			return;
	}

	SmashWindow(item_number);
}

void GeneralControl(short item_number)
{
	ITEM_INFO* item;
	short room_number;

	item = &items[item_number];

	if (TriggerActive(item))
		item->goal_anim_state = 1;
	else
		item->goal_anim_state = 0;

	AnimateItem(item);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	if (item->status == ITEM_DEACTIVATED)
	{
		RemoveActiveItem(item_number);
		item->flags |= IFL_INVISIBLE;
	}
}

void DetonatorControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	AnimateItem(item);

	if (item->frame_number - anims[item->anim_number].frame_base == 80)
	{
		camera.bounce = -150;
		SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	}

	if (item->status == ITEM_DEACTIVATED)
		RemoveActiveItem(item_number);
}

void ControlAnimating_1_4(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
		AnimateItem(item);
}

void inject_objects(bool replace)
{
	INJECT(0x00459330, OnDrawBridge, replace);
	INJECT(0x004593F0, DrawBridgeFloor, replace);
	INJECT(0x00459450, DrawBridgeCeiling, replace);
	INJECT(0x00459490, DrawBridgeCollision, replace);
	INJECT(0x004594C0, InitialiseLift, replace);
	INJECT(0x00459500, LiftControl, replace);
	INJECT(0x004595E0, LiftFloorCeiling, replace);
	INJECT(0x00459760, LiftFloor, replace);
	INJECT(0x004597A0, LiftCeiling, replace);
	INJECT(0x00459840, GetOffset, replace);
	INJECT(0x004597E0, BridgeFlatFloor, replace);
	INJECT(0x00459820, BridgeFlatCeiling, replace);
	INJECT(0x00459880, BridgeTilt1Floor, replace);
	INJECT(0x004598E0, BridgeTilt1Ceiling, replace);
	INJECT(0x00459920, BridgeTilt2Floor, replace);
	INJECT(0x00459980, BridgeTilt2Ceiling, replace);
	INJECT(0x00458DC0, ShutThatDoor, replace);
	INJECT(0x00458E10, OpenThatDoor, replace);
	INJECT(0x00458E50, InitialiseDoor, replace);
	INJECT(0x00459260, DoorControl, replace);
	INJECT(0x00458C20, SmashWindow, replace);
	INJECT(0x00458B90, InitialiseWindow, replace);
	INJECT(0x00458D20, WindowControl, replace);
	INJECT(0x004599C0, GeneralControl, replace);
	INJECT(0x00459A50, DetonatorControl, replace);
	INJECT(0x00459AD0, ControlAnimating_1_4, replace);
}
