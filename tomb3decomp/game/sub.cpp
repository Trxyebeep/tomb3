#include "../tomb3/pch.h"
#include "sub.h"
#include "../specific/init.h"
#include "control.h"
#include "collide.h"
#include "laraflar.h"
#include "objects.h"
#include "laraanim.h"

void SubInitialise(short item_number)
{
	ITEM_INFO* item;
	SUBINFO* sub;

	item = &items[item_number];
	sub = (SUBINFO*)game_malloc(sizeof(SUBINFO), 0);
	item->data = sub;
	sub->Rot = 0;
	sub->Vel = 0;
	sub->Flags = 2;
	sub->WeaponTimer = 0;

	for (int i = 0; i < 32; i++)
	{
		SubWakePts[i][0].life = 0;
		SubWakePts[i][1].life = 0;
	}
}

static long GetOnSub(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long dx, dy, dz, dist, h;
	short room_number;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || lara_item->gravity_status)
		return 0;

	item = &items[item_number];
	dy = lara_item->pos.y_pos - item->pos.y_pos + 128;

	if (ABS(dy) > 256)
		return 0;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 0x40000)
		return 0;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < -32000)
		return 0;

	return 1;
}

void SubCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	if (GetOnSub(item_number, coll))
	{
		lara.skidoo = item_number;
		lara.water_status = LARA_ABOVEWATER;

		if (lara.gun_type == LG_FLARE)
		{
			CreateFlare(0);
			undraw_flare_meshes();
			lara.flare_control_left = 0;
			lara.gun_type = LG_UNARMED;
			lara.request_gun_type = LG_UNARMED;
		}

		lara.gun_status = 1;
		item->hit_points = 1;
		l->pos.x_pos = item->pos.x_pos;
		l->pos.y_pos = item->pos.y_pos;
		l->pos.z_pos = item->pos.z_pos;
		l->pos.y_rot = item->pos.y_rot;

		if (l->current_anim_state == AS_SURFTREAD || l->current_anim_state == AS_SURFSWIM)
		{
			l->anim_number = objects[VEHICLE_ANIM].anim_index + 10;
			l->frame_number = anims[objects[UPV].anim_index + 10].frame_base;
		}
		else
		{
			l->anim_number = objects[VEHICLE_ANIM].anim_index + 13;
			l->frame_number = anims[objects[UPV].anim_index + 13].frame_base;
		}

		l->goal_anim_state = 8;
		l->current_anim_state = 8;
		AnimateItem(l);
	}
	else
	{
		item->pos.y_pos += 128;
		ObjectCollisionSub(item_number, l, coll);
		item->pos.y_pos -= 128;
	}
}

void inject_sub(bool replace)
{
	INJECT(0x004685C0, SubInitialise, replace);
	INJECT(0x00468780, GetOnSub, replace);
	INJECT(0x00468610, SubCollision, replace);
}
