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

void inject_train(bool replace)
{
	INJECT(0x0046D150, TestHeight, replace);
	INJECT(0x0046D1F0, TrainCollision, replace);
}
