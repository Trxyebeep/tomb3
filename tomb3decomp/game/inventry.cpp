#include "../tomb3/pch.h"
#include "inventry.h"
#include "objects.h"

long GetDebouncedInput(long in)
{
	static long db;
	long i;

	i = ~db;
	db = in;
	return i & in;
}

void SelectMeshes(INVENTORY_ITEM* item)
{
	if (item->object_number == PASSPORT_OPTION)
	{
		if (item->current_frame < 4)
			item->drawn_meshes = 23;
		else if (item->current_frame <= 16)
			item->drawn_meshes = 87;
		else if (item->current_frame < 19)
			item->drawn_meshes = 95;
		else if (item->current_frame == 19)
			item->drawn_meshes = 91;
		else if (item->current_frame < 24)
			item->drawn_meshes = 123;
		else if (item->current_frame < 29)
			item->drawn_meshes = 59;
		else if (item->current_frame == 29)
			item->drawn_meshes = 19;
	}
	else if (item->object_number == MAP_OPTION)
	{
		if (!item->current_frame || item->current_frame >= 18)
			item->drawn_meshes = item->which_meshes;
		else
			item->drawn_meshes = -1;
	}
	else if (item->object_number != GAMMA_OPTION)
		item->drawn_meshes = -1;
}

long AnimateInventoryItem(INVENTORY_ITEM* item)
{
	if (item->current_frame == item->goal_frame)
	{
		SelectMeshes(item);
		return 0;
	}

	if (item->anim_count)
		item->anim_count--;
	else
	{
		item->anim_count = item->anim_speed;
		item->current_frame += item->anim_direction;

		if (item->current_frame >= item->frames_total)
			item->current_frame = 0;
		else if (item->current_frame < 0)
			item->current_frame = item->frames_total - 1;
	}

	SelectMeshes(item);
	return 1;
}

void inject_inventry(bool replace)
{
	INJECT(0x00436FA0, GetDebouncedInput, replace);
	INJECT(0x004369C0, SelectMeshes, replace);
	INJECT(0x00436A50, AnimateInventoryItem, replace);
}
