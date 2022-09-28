#include "../tomb3/pch.h"
#include "triboss.h"
#include "objects.h"

short FindLizardManItemNumber(short room_number)
{
	ITEM_INFO* item;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (item->object_number == LIZARD_MAN && item->room_number == room_number)
			return i;
	}

	return NO_ITEM;
}

void inject_triboss(bool replace)
{
	INJECT(0x00471FB0, FindLizardManItemNumber, replace);
}
