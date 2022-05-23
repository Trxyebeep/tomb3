#include "../tomb3/pch.h"
#include "biggun.h"
#include "../specific/init.h"

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

void inject_biggun(bool replace)
{
	INJECT(0x00410D00, BigGunInitialise, replace);
	INJECT(0x00410E60, CanUseGun, replace);
}
