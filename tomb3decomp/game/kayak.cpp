#include "../tomb3/pch.h"
#include "kayak.h"
#include "objects.h"
#include "control.h"
#include "../specific/init.h"

void LaraRapidsDrown()
{
	lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 25;
	lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	lara_item->current_anim_state = 12;
	lara_item->goal_anim_state = 12;
	lara_item->gravity_status = 0;
	lara_item->hit_points = 0;
	lara_item->fallspeed = 0;
	lara_item->speed = 0;
	AnimateItem(lara_item);
	lara.extra_anim = 1;
	lara.gun_type = LG_UNARMED;
	lara.gun_status = LG_HANDSBUSY;
	lara.hit_direction = -1;
}

void KayakInitialise(short item_number)
{
	ITEM_INFO* item;
	KAYAKINFO* kayak;

	item = &items[item_number];
	kayak = (KAYAKINFO*)game_malloc(sizeof(KAYAKINFO), 0);
	item->data = kayak;
	kayak->Flags = 0;
	kayak->Rot = 0;
	kayak->Vel = 0;
	kayak->FallSpeedR = 0;
	kayak->FallSpeedL = 0;
	kayak->FallSpeedF = 0;
	kayak->OldPos = item->pos;

	for (int i = 0; i < 32; i++)
	{
		WakePts[i][0].life = 0;
		WakePts[i][1].life = 0;
	}
}

void inject_kayak(bool replace)
{
	INJECT(0x0043B390, LaraRapidsDrown, replace);
	INJECT(0x0043B410, KayakInitialise, replace);
}
