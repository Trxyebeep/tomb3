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

void TriggerFireFlame(long x, long y, long z, long body_part, long type)
{
	SPARKS* sptr;
	long dx, dz;
	uchar size;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (type == 2)
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = sptr->sR;
		sptr->sB = (GetRandomControl() & 0x3F) - 64;
	}
	else if (type == 254)
	{
		sptr->sR = 48;
		sptr->sG = 255;
		sptr->sB = (GetRandomControl() & 0x1F) + 48;
		sptr->dR = 32;
		sptr->dG = (GetRandomControl() & 0x3F) - 64;
		sptr->dB = (GetRandomControl() & 0x3F) + 128;
	}
	else
	{
		sptr->sR = 255;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = 48;
	}

	if (type != 254)
	{
		sptr->dR = (GetRandomControl() & 0x3F) - 64;
		sptr->dB = 32;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
	}

	if (body_part == -1)
	{
		if (type == 2 || type == 255 || type == 254)
		{
			sptr->FadeToBlack = 6;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 5;
			sptr->Life = (type < 254 ? 0 : 8) + (GetRandomControl() & 3) + 16;
			sptr->sLife = sptr->Life;
		}
		else
		{
			sptr->FadeToBlack = 8;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 20;
			sptr->Life = (GetRandomControl() & 7) + 40;
			sptr->sLife = sptr->Life;
		}
	}
	else
	{
		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 3) + 28;
		sptr->sLife = sptr->Life;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;

	if (body_part != -1)
	{
		sptr->x = (GetRandomControl() & 0x1F) - 16;
		sptr->y = 0;
		sptr->z = (GetRandomControl() & 0x1F) - 16;
	}
	else
	{
		if (!type || type == 1)
		{
			sptr->x = (GetRandomControl() & 0x1F) + x - 16;
			sptr->y = y;
			sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		}
		else if (type >= 254)
		{
			sptr->x = (GetRandomControl() & 0x3F) + x - 32;
			sptr->y = y;
			sptr->z = (GetRandomControl() & 0x3F) + z - 32;
		}
		else
		{
			sptr->x = (GetRandomControl() & 0xF) + x - 8;
			sptr->y = y;
			sptr->z = (GetRandomControl() & 0xF) + z - 8;
		}
	}

	if (type == 2)
	{
		sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Yvel = -1024 - (GetRandomControl() & 0x1FF);
		sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Friction = 68;
	}
	else
	{
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

		if (type == 1)
			sptr->Friction = 51;
		else
			sptr->Friction = 5;
	}

	if (GetRandomControl() & 1)
	{
		if (body_part == -1)
		{
			sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
			sptr->Flags = 538;
			sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		}
		else
		{
			sptr->Flags = 602;
			sptr->FxObj = (uchar)body_part;
			sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
			sptr->MaxYvel = -24 - (GetRandomControl() & 7);
		}

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (body_part == -1)
	{
		sptr->Flags = 522;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}
	else
	{
		sptr->Flags = 586;
		sptr->FxObj = (uchar)body_part;
		sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
		sptr->MaxYvel = -24 - (GetRandomControl() & 7);
	}

	sptr->Scalar = 2;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;

	if (!type)
		size = (GetRandomControl() & 0x1F) + 128;
	else if (type == 1)
		size = (GetRandomControl() & 0x1F) + 64;
	else if (type < 254)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		size = (GetRandomControl() & 0x1F) + 32;
	}
	else
		size = (GetRandomControl() & 0xF) + 48;

	sptr->sWidth = size;
	sptr->Width = size;
	sptr->sHeight = size;
	sptr->Height = size;

	if (type == 2)
	{
		sptr->dWidth = size >> 2;
		sptr->dHeight = size >> 2;
	}
	else
	{
		sptr->dWidth = size >> 4;
		sptr->dHeight = size >> 4;
	}
}

void TriggerFireSmoke(long x, long y, long z, long body_part, long type)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 32;
	sptr->dG = 32;
	sptr->dB = 32;

	if (body_part == -1)
	{
		if (type == 255)
		{
			sptr->FadeToBlack = 8;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 16;
			sptr->Life = (GetRandomControl() & 7) + 28;
			sptr->sLife = sptr->Life;
		}
		else
		{
			sptr->FadeToBlack = 16;
			sptr->ColFadeSpeed = (GetRandomControl() & 7) + 32;
			sptr->Life = (GetRandomControl() & 0xF) + 57;
			sptr->sLife = sptr->Life;
		}
	}
	else
	{
		sptr->FadeToBlack = 12;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
		sptr->Life = (GetRandomControl() & 3) + 20;
		sptr->sLife = sptr->Life;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = y - (GetRandomControl() & 0x7F) - 256;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -16 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dWidth = (GetRandomControl() & 0x3F) + 64;
	sptr->sWidth = sptr->dWidth >> 2;
	sptr->Width = sptr->sWidth;
	sptr->sHeight = sptr->sWidth;
	sptr->Height = sptr->sWidth;
	sptr->dHeight = sptr->dWidth;
}

void TriggerStaticFlame(long x, long y, long z, long size)
{
	SPARKS* sptr;
	long dx, dz;
	uchar s;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = (GetRandomControl() & 0x3F) + 128;
	sptr->sB = 64;
	sptr->dR = sptr->sR;
	sptr->dG = sptr->sG;
	sptr->dB = 64;
	sptr->ColFadeSpeed = 1;
	sptr->FadeToBlack = 0;
	sptr->Life = 2;
	sptr->sLife = 2;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 7) + x - 4;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 7) + z - 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Friction = 0;
	sptr->Zvel = 0;
	sptr->Yvel = 0;
	sptr->Xvel = 0;
	sptr->Flags = 522;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 2;
	s = (uchar)size;
	sptr->sWidth = s;
	sptr->Width = s;
	sptr->dWidth = s;
	sptr->sHeight = s;
	sptr->Height = s;
	sptr->dHeight = s;
}

void TriggerSideFlame(long x, long y, long z, long angle, long speed, long pilot)
{
	SPARKS* sptr;
	long dx, dz, rad, s, c;
	uchar size;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 0x80;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 28;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (pilot)
		rad = (speed << 7) + (GetRandomControl() & 0x1F);
	else
		rad = (speed << 8) + (GetRandomControl() & 0x1FF);

	s = (rad * rcossin_tbl[angle]) >> (W2V_SHIFT - 3);
	c = (rad * rcossin_tbl[angle + 1]) >> (W2V_SHIFT - 3);
	sptr->Xvel = short((GetRandomControl() & 0x7F) + s - 64);
	sptr->Yvel = -6 - (GetRandomControl() & 7);
	sptr->Zvel = short((GetRandomControl() & 0x7F) + c - 64);
	sptr->Friction = 4;
	sptr->Flags = 522;
	sptr->Gravity = -8 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	size = (GetRandomControl() & 0x1F) + 128;

	if (pilot)
		size >>= 2;

	sptr->dWidth = size;
	sptr->sWidth = size >> 1;
	sptr->Width = size >> 1;
	sptr->sHeight = size >> 1;
	sptr->Height = size >> 1;
	sptr->dHeight = size;
}

void TriggerRocketSmoke(long x, long y, long z, long c)
{
	SPARKS* sptr;
	uchar size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = uchar(c + 64);
	sptr->dG = uchar(c + 64);
	sptr->dB = uchar(c + 64);
	sptr->FadeToBlack = 12;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -4 - (GetRandomControl() & 3);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	size = (GetRandomControl() & 7) + 32;
	sptr->dWidth = size;
	sptr->sWidth = size >> 2;
	sptr->Width = size >> 2;
	sptr->sHeight = size >> 2;
	sptr->Height = size >> 2;
	sptr->dHeight = size;
}

void inject_effect2(bool replace)
{
	INJECT(0x0042DE00, TriggerDynamic, replace);
	INJECT(0x0042DE60, ClearDynamics, replace);
	INJECT(0x0042D9A0, KillEverything, replace);
	INJECT(0x0042DCF0, TriggerBreath, replace);
	INJECT(0x0042BDA0, TriggerAlertLight, replace);
	INJECT(0x0042B780, TriggerFireFlame, replace);
	INJECT(0x0042B2F0, TriggerFireSmoke, replace);
	INJECT(0x0042D640, TriggerStaticFlame, replace);
	INJECT(0x0042BBC0, TriggerSideFlame, replace);
	INJECT(0x0042C670, TriggerRocketSmoke, replace);
}
