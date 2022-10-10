#include "../tomb3/pch.h"
#include "londboss.h"
#include "items.h"
#include "../specific/game.h"
#include "objects.h"

static void TriggerPlasmaBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle)
{
	FX_INFO* fx;
	short fx_num;

	fx_num = CreateEffect(room_number);

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		fx->speed = (GetRandomControl() & 0x1F) + 64;
		fx->pos.x_pos = pos->x;
		fx->pos.y_pos = pos->y;
		fx->pos.z_pos = pos->z;
		fx->pos.x_rot = 0x2000;
		fx->pos.y_rot = short(angle + GetRandomControl() + 0x4000);
		fx->object_number = EXTRAFX5;
		fx->fallspeed = 0;
		fx->flag1 = 1;
		fx->flag2 = type == 2;
	}
}

void inject_londboss(bool replace)
{
	INJECT(0x00451DE0, TriggerPlasmaBall, replace);
}
