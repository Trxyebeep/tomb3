#include "../tomb3/pch.h"
#include "flymaker.h"
#include "objects.h"
#include "items.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

static short FlyItem[3] = { NO_ITEM, NO_ITEM, NO_ITEM };

void FlyEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* fly;
	long nFly, dx, dz;
	short lp;

	item = &items[item_number];

	if (!item->active || item->hit_points <= 0 || item->timer <= 0)
		return;

	if (FlyItem[0] == NO_ITEM)
	{
		nFly = 0;

		for (lp = 0; lp < level_items; lp++)
		{
			fly = &items[lp];

			if (fly->object_number == MUTANT1 && fly->ai_bits & MODIFY)
			{
				FlyItem[nFly++] = lp;
				KillItem(lp);
			}

			if (nFly > 2)
				break;
		}

		return;
	}

	if (items[FlyItem[0]].status == ITEM_ACTIVE &&
		items[FlyItem[1]].status == ITEM_ACTIVE &&
		items[FlyItem[2]].status == ITEM_ACTIVE)
		return;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx > 32000 || dx < -32000 || dz > 32000 || dz < -32000 || SQUARE(dx) + SQUARE(dz) > 0x9000000)
		return;

	if (item->item_flags[0] > 0)
	{
		item->item_flags[0]--;
		return;
	}

	item->item_flags[0] = 255;
	item->timer -= 30;

	for (lp = 0; lp < 3; lp++)
	{
		if (!items[FlyItem[lp]].data && items[FlyItem[lp]].status != ITEM_ACTIVE)
			break;
	}

	if (lp > 2)
		return;

	fly = &items[FlyItem[lp]];
	fly->pos = item->pos;
	fly->hit_points = objects[fly->object_number].hit_points;
	fly->anim_number = objects[fly->object_number].anim_index;
	fly->frame_number = anims[fly->anim_number].frame_base;
	fly->current_anim_state = anims[fly->anim_number].current_anim_state;
	fly->goal_anim_state = fly->current_anim_state;
	fly->required_anim_state = 0;
	fly->data = 0;
	fly->mesh_bits = -1;
	fly->flags &= ~(IFL_CLEARBODY | IFL_INVISIBLE | 3);	//What's 3?
	fly->status = ITEM_ACTIVE;
	fly->collidable = 1;
	item->ai_bits = MODIFY;

	if (fly->active)
		RemoveActiveItem(FlyItem[lp]);

	AddActiveItem(FlyItem[lp]);
	ItemNewRoom(FlyItem[lp], item->room_number);
	EnableBaddieAI(FlyItem[lp], 1);
}
