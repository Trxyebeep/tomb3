#include "../tomb3/pch.h"
#include "effect2.h"
#include "objects.h"
#include "items.h"
#include "../specific/game.h"
#include "control.h"

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	if (number_dynamics != 32)
	{
		dl = &dynamics[number_dynamics];
		dl->x = x;
		dl->y = y;
		dl->z = z;
		dl->falloff = ushort(falloff << 8);
		dl->on = 1;
		dl->r = (uchar)r;
		dl->g = (uchar)g;
		dl->b = (uchar)b;
		number_dynamics++;
	}
}

void ClearDynamics()
{
	number_dynamics = 0;

	for (int i = 0; i < 32; i++)
		dynamics[i].on = 0;
}

void KillEverything()
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short item_number, nex, obj_num;

	item_number = next_item_active;

	while (item_number != NO_ITEM)
	{
		item = &items[item_number];
		nex = item->next_active;
		obj_num = item->object_number;

		if (obj_num != LARA && obj_num != FLARE && (obj_num < DOOR_TYPE1 || obj_num > DOOR_TYPE8) && item->active && !(item->flags & IFL_REVERSE))
			KillItem(item_number);

		item_number = nex;
	}

	item_number = next_fx_active;

	while (item_number != NO_ITEM)
	{
		fx = &effects[item_number];
		nex = fx->next_active;
		obj_num = fx->object_number;

		if (objects[obj_num].control && (obj_num != FLAME || fx->counter >= 0))
			KillEffect(item_number);

		item_number = nex;
	}

	for (int i = 0; i < 8; i++)
		lead_info[i].on = 0;

	KillEverythingFlag = 0;
}

void TriggerBreath(long x, long y, long z, long xv, long yv, long zv)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 32;
	sptr->dG = 32;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 32;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 37;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Friction = 0;
	sptr->Xvel = (short)xv;
	sptr->Yvel = (short)yv;
	sptr->Zvel = (short)zv;

	if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = 778;
	else
		sptr->Flags = 522;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->dWidth = (GetRandomControl() & 7) + 32;
	sptr->sWidth = sptr->dWidth >> 3;
	sptr->Width = sptr->sWidth;
	sptr->sHeight = sptr->sWidth;
	sptr->Height = sptr->sWidth;
	sptr->dHeight = sptr->dWidth;
}

void TriggerAlertLight(long x, long y, long z, long r, long g, long b, short angle, short rn)
{
	GAME_VECTOR s;
	GAME_VECTOR t;

	s.x = x;
	s.y = y;
	s.z = z;
	GetFloor(x, y, z, &rn);
	s.room_number = rn;
	t.x = x + (rcossin_tbl[angle << 1] << 1);
	t.y = y;
	t.z = z + (rcossin_tbl[(angle << 1) + 1] << 1);

	if (!LOS(&s, &t))
		TriggerDynamic(t.x, t.y, t.z, 8, r, g, b);
}

void inject_effect2(bool replace)
{
	INJECT(0x0042DE00, TriggerDynamic, replace);
	INJECT(0x0042DE60, ClearDynamics, replace);
	INJECT(0x0042D9A0, KillEverything, replace);
	INJECT(0x0042DCF0, TriggerBreath, replace);
	INJECT(0x0042BDA0, TriggerAlertLight, replace);
}
