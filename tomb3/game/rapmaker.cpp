#include "../tomb3/pch.h"
#include "rapmaker.h"

void InitialiseRaptorEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[0] = 96 * (item_number & 3);
}

void inject_rapmaker(bool replace)
{
	INJECT(0x00461010, InitialiseRaptorEmitter, replace);
}
