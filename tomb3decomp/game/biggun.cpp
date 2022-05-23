#include "../tomb3/pch.h"
#include "biggun.h"
#include "../specific/init.h"
#include "laraflar.h"
#include "laraanim.h"
#include "objects.h"
#include "collide.h"

void BigGunInitialise(short item_number)
{
	ITEM_INFO* item;
	BIGGUNINFO* gun;

	item = &items[item_number];
	gun = (BIGGUNINFO*)game_malloc(sizeof(BIGGUNINFO), 0);
	item->data = gun;
	gun->FireCount = 0;
	gun->Flags = 0;
	gun->RotX = 30;
	gun->RotY = 0;
	gun->StartRotY = item->pos.y_rot;
}

static long CanUseGun(ITEM_INFO* gun, ITEM_INFO* l)
{
	long dx, dz, dist;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || l->gravity_status)
		return 0;

	dx = l->pos.x_pos - gun->pos.x_pos;
	dz = l->pos.z_pos - gun->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 30000)
		return 0;

	return 1;
}

void BigGunCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	BIGGUNINFO* gun;

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	item = &items[item_number];

	if (CanUseGun(item, l))
	{
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
		l->pos = item->pos;
		l->anim_number = objects[VEHICLE_ANIM].anim_index;
		l->frame_number = anims[objects[BIGGUN].anim_index].frame_base;
		l->current_anim_state = 0;
		l->goal_anim_state = 0;
		gun = (BIGGUNINFO*)item->data;
		gun->Flags = 0;
		gun->RotX = 30;
	}
	else
		ObjectCollision(item_number, l, coll);
}

void inject_biggun(bool replace)
{
	INJECT(0x00410D00, BigGunInitialise, replace);
	INJECT(0x00410E60, CanUseGun, replace);
	INJECT(0x00410D50, BigGunCollision, replace);
}
