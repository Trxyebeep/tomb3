#include "../tomb3/pch.h"
#include "train.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "collide.h"
#include "sphere.h"
#include "sound.h"
#include "objects.h"
#include "effects.h"
#include "lara.h"
#include "items.h"
#include "effect2.h"
#include "camera.h"
#include "../tomb3/tomb3.h"

static long TestHeight(ITEM_INFO* item, long x, long z, short* room_number)
{
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long s, c;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	pos.x = item->pos.x_pos + ((z * s + x * c) >> W2V_SHIFT);
	pos.y = ((x * phd_sin(item->pos.z_rot)) >> W2V_SHIFT) + (item->pos.y_pos - ((z * phd_sin(item->pos.x_rot)) >> W2V_SHIFT));
	pos.z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);
	*room_number = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, room_number);
	return GetHeight(floor, pos.x, pos.y, pos.z);
}

void TrainCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long x, y, z;

	item = &items[item_number];

	if (!TestBoundsCollide(item, l, coll->radius) || !TestCollision(item, l))
		return;

	SoundEffect(SFX_LARA_GENERAL_DEATH, &l->pos, SFX_ALWAYS);
	SoundEffect(SFX_LARA_FALLDEATH, &l->pos, SFX_ALWAYS);
	StopSoundEffect(SFX_TUBE_LOOP);
	l->anim_number = objects[LARA_EXTRA].anim_index;
	l->frame_number = anims[l->anim_number].frame_base;
	l->current_anim_state = EXTRA_TRAINKILL;
	l->goal_anim_state = EXTRA_TRAINKILL;
	l->hit_points = 0;
	l->pos.y_rot = item->pos.y_rot;
	l->gravity_status = 0;
	l->fallspeed = 0;
	l->speed = 0;
	AnimateItem(l);
	lara.extra_anim = 1;
	lara.gun_status = LG_HANDSBUSY;
	lara.gun_type = LG_UNARMED;
	lara.hit_direction = -1;
	lara.air = -1;
	item->item_flags[1] = 160;	//train speed
	x = l->pos.x_pos + (256 * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
	y = l->pos.y_pos - 512;
	z = l->pos.z_pos + (256 * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
	DoLotsOfBlood(x, y, z, 1024, item->pos.y_rot, l->room_number, 15);
}

void TrainControl(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long s, c, mid, front, x, y, z;
	static short speed = 260;
	short room_number;

	item = &items[item_number];

	if (tomb3.gold)
	{
		if (CurrentLevel == 4)
			speed = 200;
		else if (CurrentLevel == 5)
			speed = 100;
		else
			speed = 260;
	}

	if (!TriggerActive(item))
		return;

	if (!item->item_flags[0])
	{
		item->item_flags[0] = tomb3.gold ? speed : 260;
		item->item_flags[1] = tomb3.gold ? speed : 260;
	}

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	item->pos.x_pos += (item->item_flags[1] * s) >> W2V_SHIFT;
	item->pos.z_pos += (item->item_flags[1] * c) >> W2V_SHIFT;
	front = TestHeight(item, 0, 5120, &room_number);
	mid = TestHeight(item, 0, 0, &room_number);
	item->pos.y_pos = mid;

	if (item->pos.y_pos == NO_HEIGHT)
	{
		KillItem(item_number);
		return;
	}

	item->pos.y_pos -= 32;
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	item->pos.x_rot = short((mid - front) << 1);

	x = item->pos.x_pos + ((3072 * s) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos + ((3072 * c) >> W2V_SHIFT);
	TriggerDynamic(x, y, z, 16, 255, 255, 255);

	if (item->item_flags[1] == (tomb3.gold ? speed : 260))
	{
		SoundEffect(SFX_TUBE_LOOP, &item->pos, SFX_ALWAYS);
		return;
	}

	//not full speed means train hit lara
	item->item_flags[1] -= 48;

	if (item->item_flags[1] < 0)
		item->item_flags[1] = 0;

	if (!UseForcedFixedCamera)
	{
		ForcedFixedCamera.x = item->pos.x_pos + (0x2000 * s >> W2V_SHIFT);
		ForcedFixedCamera.z = item->pos.z_pos + (0x2000 * c >> W2V_SHIFT);
		room_number = item->room_number;
		floor = GetFloor(ForcedFixedCamera.x, item->pos.y_pos - 512, ForcedFixedCamera.z, &room_number);
		ForcedFixedCamera.y = GetHeight(floor, ForcedFixedCamera.x, item->pos.y_pos - 512, ForcedFixedCamera.z);
		ForcedFixedCamera.room_number = room_number;
		UseForcedFixedCamera = 1;
	}
}
