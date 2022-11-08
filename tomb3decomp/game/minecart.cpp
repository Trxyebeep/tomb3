#include "../tomb3/pch.h"
#include "minecart.h"
#include "../specific/init.h"
#include "collide.h"
#include "sphere.h"
#include "control.h"
#include "laraflar.h"
#include "../specific/game.h"
#include "objects.h"
#include "../specific/specific.h"
#include "../3dsystem/phd_math.h"

void MineCartInitialise(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->data = game_malloc(sizeof(CARTINFO), 0);
}

static long GetInMineCart(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	long dx, dz, dist, h;
	short room_number;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || l->gravity_status ||
		!TestBoundsCollide(item, l, coll->radius) || !TestCollision(item, l))
		return 0;

	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 200000)
		return 0;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < -32000)
		return 0;

	return 1;
}

void MineCartCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	CARTINFO* cart;
	short ang;

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	item = &items[item_number];

	if (!GetInMineCart(item, l, coll))
		return ObjectCollision(item_number, l, coll);

	lara.skidoo = item_number;

	if (lara.gun_type == LG_FLARE)
	{
		CreateFlare(0);
		undraw_flare_meshes();
		lara.flare_control_left = 0;
		lara.gun_type = LG_UNARMED;
		lara.request_gun_type = LG_UNARMED;
	}

	lara.gun_status = LG_HANDSBUSY;
	ang = short(mGetAngle(item->pos.x_pos, item->pos.z_pos, l->pos.x_pos, l->pos.z_pos) - item->pos.y_rot);

	if (ang > -0x1FFE && ang < 0x5FFA)
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 46;
	else
		l->anim_number = objects[VEHICLE_ANIM].anim_index;

	l->frame_number = anims[l->anim_number].frame_base;
	l->current_anim_state = 0;
	l->goal_anim_state = 0;
	l->pos = item->pos;

	cart = (CARTINFO*)item->data;
	cart->Flags = 0;
	cart->Speed = 0;
	cart->YVel = 0;
	cart->Gradient = 0;
	S_CDPlay(12, 0);
}

static long CanGetOut(long lr)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short ang, room_number;

	item = &items[lara.skidoo];

	if (lr < 0)
		ang = item->pos.y_rot + 0x4000;
	else
		ang = item->pos.y_rot - 0x4000;

	x = item->pos.x_pos - ((330 * phd_sin(ang)) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos - ((330 * phd_cos(ang)) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type != BIG_SLOPE && height_type != DIAGONAL && h != NO_HEIGHT && abs(h) > 512)
	{
		c = GetCeiling(floor, x, y, z);

		if (c - item->pos.y_pos <= -762 && h - c >= 762)
			return 1;
	}

	return 0;
}

void inject_minecart(bool replace)
{
	INJECT(0x00453930, MineCartInitialise, replace);
	INJECT(0x00453AB0, GetInMineCart, replace);
	INJECT(0x00453960, MineCartCollision, replace);
	INJECT(0x00454C00, CanGetOut, replace);
}
