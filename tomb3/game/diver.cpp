#include "../tomb3/pch.h"
#include "diver.h"
#include "control.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "missile.h"

long GetWaterSurface(long x, long y, long z, short room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;

	r = &room[room_number];
	floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];

	if (r->flags & ROOM_UNDERWATER)
	{
		while (floor->sky_room != 0xFF)
		{
			r = &room[floor->sky_room];

			if (!(r->flags & ROOM_UNDERWATER))
				return floor->ceiling << 8;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}
	else
	{
		while (floor->pit_room != 0xFF)
		{
			r = &room[floor->pit_room];

			if (r->flags & ROOM_UNDERWATER)
				return floor->floor << 8;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}

	return NO_HEIGHT;
}

void ControlGhostGasEmitter(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long dx, dz, size;

	item = &items[item_number];

	if (!TriggerActive(item) || wibble & 0xF)
		return;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 12;
	sptr->dG = 32;
	sptr->dB = 0;
	sptr->FadeToBlack = 32;
	sptr->ColFadeSpeed = (GetRandomControl() & 7) + 24;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 64;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1FF) + item->pos.x_pos - 256;
	sptr->y = item->pos.y_pos - (GetRandomControl() & 0xF) - 264;
	sptr->z = (GetRandomControl() & 0x3FF) + item->pos.z_pos - 512;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -1 - (GetRandomControl() & 1);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -4 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 4;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	size = (GetRandomControl() & 0x1F) + 96;
	sptr->Width = uchar(size >> 1);
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width;
	sptr->Height = uchar((size + (GetRandomControl() & 0x1F) + 32) >> 1);
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height;
}

static short Harpoon(long x, long y, long z, short speed, short ang, short room_number)
{
	FX_INFO* fx;
	short fx_num;

	fx_num = CreateEffect(room_number);

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		fx->pos.x_pos = x;
		fx->pos.y_pos = y;
		fx->pos.z_pos = z;
		fx->room_number = room_number;
		fx->pos.x_rot = 0;
		fx->pos.y_rot = ang;
		fx->pos.z_rot = 0;
		fx->speed = 150;
		fx->fallspeed = 0;
		fx->frame_number = 0;
		fx->object_number = DIVER_HARPOON;
		fx->shade = 0xE00;
		ShootAtLara(fx);
	}

	return fx_num;
}

void inject_diver(bool replace)
{
	INJECT(0x00423D70, GetWaterSurface, replace);
	INJECT(0x00423E80, ControlGhostGasEmitter, replace);
	INJECT(0x00423CF0, Harpoon, replace);
}
