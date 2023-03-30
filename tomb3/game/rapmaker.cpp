#include "../tomb3/pch.h"
#include "rapmaker.h"
#include "objects.h"
#include "items.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

short RaptorItem[3] = { NO_ITEM, NO_ITEM, NO_ITEM };

void InitialiseRaptorEmitter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->item_flags[0] = 96 * (item_number & 3);
}

void RaptorEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* raptor;
	long nRaptor, x, z;
	short lp;

	item = &items[item_number];

	if (!item->active || item->timer <= 0)
		return;

	if (RaptorItem[0] == NO_ITEM)
	{
		nRaptor = 0;

		for (lp = 0; lp < level_items; lp++)
		{
			raptor = &items[lp];

			if (raptor->object_number == RAPTOR && raptor->ai_bits & MODIFY)
				RaptorItem[nRaptor++] = lp;

			if (nRaptor > 2)
				break;
		}

		return;
	}

	if (items[RaptorItem[0]].data && items[RaptorItem[1]].data && items[RaptorItem[2]].data)
		return;

	x = lara_item->pos.x_pos - item->pos.x_pos;
	z = lara_item->pos.z_pos - item->pos.z_pos;

	if (SQUARE(x) + SQUARE(z) < 0x1000000)
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
		if (!items[RaptorItem[lp]].data)
			break;
	}

	raptor = &items[RaptorItem[lp]];
	raptor->pos.x_pos = item->pos.x_pos;
	raptor->pos.y_pos = item->pos.y_pos;
	raptor->pos.z_pos = item->pos.z_pos;
	raptor->pos.x_rot = item->pos.x_rot;
	raptor->pos.y_rot = item->pos.y_rot;
	raptor->pos.z_rot = item->pos.z_rot;
	raptor->anim_number = objects[raptor->object_number].anim_index;
	raptor->frame_number = anims[raptor->anim_number].frame_base;
	raptor->current_anim_state = anims[raptor->anim_number].current_anim_state;
	raptor->goal_anim_state = raptor->current_anim_state;
	raptor->required_anim_state = 0;
	raptor->flags &= ~(IFL_CLEARBODY | IFL_INVISIBLE | 3);
	raptor->data = 0;
	raptor->hit_points = objects[raptor->object_number].hit_points;
	raptor->mesh_bits = -1;
	raptor->status = ITEM_ACTIVE;
	raptor->collidable = 1;

	if (raptor->active)
		RemoveActiveItem(RaptorItem[lp]);

	AddActiveItem(RaptorItem[lp]);
	ItemNewRoom(RaptorItem[lp], item->room_number);
	EnableBaddieAI(RaptorItem[lp], 1);
}
