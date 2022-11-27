#include "../tomb3/pch.h"
#include "items.h"
#include "effect2.h"

void InitialiseItemArray(short num_items)
{
	ITEM_INFO* item;

	item = &items[level_items];
	next_item_free = (short)level_items;
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

void KillItem(short item_num)
{
	ITEM_INFO* item;
	short linknum;

	DetatchSpark(item_num, 128);
	item = &items[item_num];
	item->active = 0;
	item->really_active = 0;

	if (next_item_active == item_num)
		next_item_active = item->next_active;
	else
	{
		for (linknum = next_item_active; linknum != NO_ITEM; linknum = items[linknum].next_active)
		{
			if (items[linknum].next_active == item_num)
			{
				items[linknum].next_active = item->next_active;
				break;
			}
		}
	}

	if (item->room_number != 255)
	{
		linknum = room[item->room_number].item_number;

		if (linknum == item_num)
			room[item->room_number].item_number = item->next_item;
		else
		{
			for (; linknum != NO_ITEM; linknum = items[linknum].next_item)
			{
				if (items[linknum].next_item == item_num)
				{
					items[linknum].next_item = item->next_item;
					break;
				}
			}
		}
	}

	if (item == lara.target)
		lara.target = 0;

	if (item_num < level_items)
		item->flags |= IFL_CLEARBODY;
	else
	{
		item->next_item = next_item_free;
		next_item_free = item_num;
	}
}

short CreateItem()
{
	short item_number;

	item_number = next_item_free;

	if (item_number != NO_ITEM)
	{
		items[item_number].flags = 0;
		items[item_number].il.init = 0;
		next_item_free = items[item_number].next_item;
	}

	return item_number;
}

void inject_items(bool replace)
{
	INJECT(0x0043AA20, InitialiseItemArray, replace);
	INJECT(0x0043AA90, KillItem, replace);
	INJECT(0x0043ABE0, CreateItem, replace);
}
