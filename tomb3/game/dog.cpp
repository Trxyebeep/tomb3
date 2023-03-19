#include "../tomb3/pch.h"
#include "dog.h"

void InitialiseDog(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[item->object_number].anim_index + 8;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = DOG_STOP;
}

void inject_dog(bool replace)
{
	INJECT(0x00424070, InitialiseDog, replace);
}
