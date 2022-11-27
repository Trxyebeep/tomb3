#include "../tomb3/pch.h"
#include "items.h"

void InitialiseItemArray(short num_items)
{
	ITEM_INFO* item;

	item = &items[level_items];
	next_item_free = level_items;
	body_bag = NO_ITEM;
	next_item_active = NO_ITEM;

	for (int i = level_items + 1; i < num_items; i++)
	{
		item->next_item = i;
		item->active = 0;
		item->il.init = 0;
		item++;
	}

	item->next_item = NO_ITEM;
}

void inject_items(bool replace)
{
	INJECT(0x0043AA20, InitialiseItemArray, replace);
}
