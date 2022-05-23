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

void inject_biggun(bool replace)
{
	INJECT(0x00410D00, BigGunInitialise, replace);
}
