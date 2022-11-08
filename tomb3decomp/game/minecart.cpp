#include "../tomb3/pch.h"
#include "minecart.h"
#include "../specific/init.h"

void MineCartInitialise(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->data = game_malloc(sizeof(CARTINFO), 0);
}

void inject_minecart(bool replace)
{
	INJECT(0x00453930, MineCartInitialise, replace);
}
