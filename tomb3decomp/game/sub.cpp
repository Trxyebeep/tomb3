#include "../tomb3/pch.h"
#include "sub.h"
#include "../specific/init.h"

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

void inject_sub(bool replace)
{
	INJECT(0x004685C0, SubInitialise, replace);
}
