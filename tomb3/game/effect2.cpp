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
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

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
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
			sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		}
		else
		{
			sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;
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
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}
	else
	{
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;
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
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

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
	sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
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
	sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
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
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

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

void TriggerRicochetSpark(GAME_VECTOR* pos, long angle, long size)
{
	SPARKS* sptr;
	long ang;
	uchar c;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 32;
	sptr->sB = 0;
	sptr->dR = 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 96;
	sptr->dB = 0;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->Life = 24;
	sptr->sLife = 24;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->x = pos->x;
	sptr->y = pos->y;
	sptr->z = pos->z;
	ang = ((GetRandomControl() & 0x7FF) + angle - 1024) & 0xFFF;
	sptr->Xvel = -rcossin_tbl[ang << 1] >> 1;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 768;
	sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 1;
	sptr->Friction = 1;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 3;
	sptr->Gravity = short(abs(sptr->Yvel >> 6) + (GetRandomControl() & 0x1F));
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomControl() & 1) + 1;
	sptr->Height = (GetRandomControl() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomControl() & 1) + 1;
	sptr->MaxYvel = 0;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	c = uchar((GetRandomControl() & 0x3F) + 128);
	sptr->sR = c;
	sptr->sG = c;
	sptr->sB = c;
	c >>= 1;
	sptr->dR = c;
	sptr->dG = c;
	sptr->dB = c;
	sptr->TransType = 3;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 16;
	sptr->Life = 28;
	sptr->sLife = 28;
	sptr->Dynamic = -1;
	sptr->x = pos->x;
	sptr->y = pos->y;
	sptr->z = pos->z;
	ang = ((GetRandomControl() & 0x7FF) + angle - 1023) & 0xFFF;
	sptr->Xvel = -rcossin_tbl[ang << 1] >> 1;
	sptr->Yvel = (GetRandomControl() & 0x1FF) - 384;
	sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 1;
	sptr->Friction = 33;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 3;
	sptr->Gravity = (GetRandomControl() & 7) + 4;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomControl() & 1) + 1;
	sptr->Height = (GetRandomControl() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomControl() & 1) + 1;
	sptr->MaxYvel = 0;
}

void TriggerBlood(long x, long y, long z, long angle, long num)
{
	SPARKS* sptr;
	long ang;
	short rad;

	for (int i = 0; i < num; i++)
	{
		sptr = &sparks[GetFreeSpark()];
		sptr->On = 1;

		if (gameflow.language == 2)
		{
			sptr->sR = 112;
			sptr->sG = 0;
			sptr->sB = 224;
			sptr->dR = 96;
			sptr->dG = 0;
			sptr->dB = 192;
		}
		else
		{
			sptr->sR = 224;
			sptr->sG = 0;
			sptr->sB = 32;
			sptr->dR = 192;
			sptr->dG = 0;
			sptr->dB = 24;
		}

		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 1;
		sptr->Dynamic = -1;
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		rad = GetRandomControl() & 0xF;
		ang = ((GetRandomControl() & 0x1F) + angle - 16) & 0xFFF;
		sptr->Xvel = -(rad * rcossin_tbl[ang << 1]) >> 5;
		sptr->Yvel = -128 - (GetRandomControl() & 0xFF);
		sptr->Zvel = (rad * rcossin_tbl[(ang << 1) + 1]) >> 5;
		sptr->Friction = 4;
		sptr->Flags = SF_BLOOD;
		sptr->Scalar = 3;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 31;
		sptr->Width = 2;
		sptr->sWidth = 2;
		sptr->Height = 2;
		sptr->sHeight = 2;
		sptr->dWidth = 2 - (GetRandomControl() & 1);
		sptr->dHeight = 2 - (GetRandomControl() & 1);
	}
}

void TriggerBloodD(long x, long y, long z, long angle, long num)
{
	SPARKS* sptr;
	long ang;
	short rad;

	for (int i = 0; i < num; i++)
	{
		sptr = &sparks[GetFreeSpark()];
		sptr->On = 1;

		if (gameflow.language == 2)
		{
			sptr->sR = 112;
			sptr->sG = 0;
			sptr->sB = 224;
			sptr->dR = 96;
			sptr->dG = 0;
			sptr->dB = 192;
		}
		else
		{
			sptr->sR = 224;
			sptr->sG = 0;
			sptr->sB = 32;
			sptr->dR = 192;
			sptr->dG = 0;
			sptr->dB = 24;
		}

		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 1;
		sptr->Dynamic = -1;
		sptr->x = (GetRandomDraw() & 0x1F) + x - 16;
		sptr->y = (GetRandomDraw() & 0x1F) + y - 16;
		sptr->z = (GetRandomDraw() & 0x1F) + z - 16;
		rad = GetRandomDraw() & 0xF;
		ang = ((GetRandomDraw() & 0x1F) + angle - 16) & 0xFFF;
		sptr->Xvel = -(rad * rcossin_tbl[ang << 1]) >> 5;
		sptr->Yvel = -128 - (GetRandomDraw() & 0xFF);
		sptr->Zvel = (rad * rcossin_tbl[(ang << 1) + 1]) >> 5;
		sptr->Friction = 4;
		sptr->Flags = SF_NONE;
		sptr->Scalar = 3;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomDraw() & 0x1F) + 31;
		sptr->Width = 2;
		sptr->sWidth = 2;
		sptr->Height = 2;
		sptr->sHeight = 2;
		sptr->dWidth = 2 - (GetRandomDraw() & 1);
		sptr->dHeight = 2 - (GetRandomDraw() & 1);
	}
}

void TriggerUnderwaterBlood(long x, long y, long z, long size)
{
	RIPPLE_STRUCT* ripple;
	long n;

	ripple = ripples;
	n = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		n++;

		if (n >= 16)
			return;
	}

	ripple->flags = 51;
	ripple->init = 1;
	ripple->life = (GetRandomControl() & 7) - 16;
	ripple->size = (uchar)size;
	ripple->x = (GetRandomControl() & 0x3F) + x - 32;
	ripple->y = y;
	ripple->z = (GetRandomControl() & 0x3F) + z - 32;
}

void TriggerUnderwaterBloodD(long x, long y, long z, long size)
{
	RIPPLE_STRUCT* ripple;
	long n;

	ripple = ripples;
	n = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		n++;

		if (n >= 16)
			return;
	}

	ripple->flags = 51;
	ripple->init = 1;
	ripple->life = (GetRandomDraw() & 7) - 16;
	ripple->size = (uchar)size;
	ripple->x = (GetRandomDraw() & 0x3F) + x - 32;
	ripple->y = y;
	ripple->z = (GetRandomDraw() & 0x3F) + z - 32;
}

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv, long smoke, long unused)
{
	SPARKS* sptr;
	SPARKS* smokeSpark;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dR = 255;
	sptr->dG = (GetRandomDraw() & 0x7F) + 64;
	sptr->dB = 192 - sptr->dG;
	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 5;
	sptr->Life = 10;
	sptr->sLife = 10;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomDraw() & 7) + x - 3;
	sptr->y = (GetRandomDraw() & 7) + y - 3;
	sptr->z = (GetRandomDraw() & 7) + z - 3;
	sptr->Xvel = short((GetRandomDraw() & 0xFF) + xv - 128);
	sptr->Yvel = short((GetRandomDraw() & 0xFF) + yv - 128);
	sptr->Zvel = short((GetRandomDraw() & 0xFF) + zv - 128);
	sptr->Friction = 34;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomDraw() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomDraw() & 1) + 1;
	sptr->Height = (GetRandomDraw() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomDraw() & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Flags = SF_SCALE;

	if (smoke)
	{
		smokeSpark = &sparks[GetFreeSpark()];
		smokeSpark->On = 1;
		smokeSpark->sR = sptr->dR >> 1;
		smokeSpark->sG = sptr->dG >> 1;
		smokeSpark->sB = sptr->dB >> 1;
		smokeSpark->dR = 32;
		smokeSpark->dG = 32;
		smokeSpark->dB = 32;
		smokeSpark->ColFadeSpeed = (GetRandomDraw() & 3) + 8;
		smokeSpark->FadeToBlack = 4;
		smokeSpark->TransType = 2;
		smokeSpark->Life = (GetRandomDraw() & 7) + 13;
		smokeSpark->sLife = smokeSpark->Life;
		smokeSpark->x = x + (xv >> 5);
		smokeSpark->y = y + (yv >> 5);
		smokeSpark->z = z + (zv >> 5);
		smokeSpark->extras = 0;
		smokeSpark->Dynamic = -1;
		smokeSpark->Xvel = short((GetRandomDraw() & 0x3F) + xv - 32);
		smokeSpark->Yvel = (short)yv;
		smokeSpark->Zvel = short((GetRandomDraw() & 0x3F) + zv - 32);
		smokeSpark->Friction = 4;

		if (GetRandomDraw() & 1)
		{
			smokeSpark->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
			smokeSpark->RotAng = GetRandomDraw() & 0xFFF;

			if (GetRandomDraw() & 1)
				smokeSpark->RotAdd = -16 - (GetRandomDraw() & 0xF);
			else
				smokeSpark->RotAdd = (GetRandomDraw() & 0xF) + 16;
		}
		else
			smokeSpark->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

		smokeSpark->Def = (uchar)objects[EXPLOSION1].mesh_index;
		smokeSpark->Scalar = 2;
		smokeSpark->Gravity = -8 - (GetRandomDraw() & 3);
		smokeSpark->MaxYvel = -4 - (GetRandomDraw() & 3);
		smokeSpark->dWidth = (GetRandomDraw() & 0xF) + 24;
		smokeSpark->sWidth = smokeSpark->dWidth >> 3;
		smokeSpark->Width = smokeSpark->dWidth >> 3;
		smokeSpark->dHeight = smokeSpark->dWidth;
		smokeSpark->sHeight = smokeSpark->dHeight >> 3;
		smokeSpark->Height = smokeSpark->dHeight >> 3;
	}
}

void TriggerGunSmoke(long x, long y, long z, long xv, long yv, long zv, long initial, long weapon, long shade)
{
	SPARKS* sptr;
	uchar size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = uchar(shade << 2);
	sptr->dG = uchar(shade << 2);
	sptr->dB = uchar(shade << 2);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = uchar(32 - (initial << 4));
	sptr->Life = (GetRandomControl() & 3) + 40;
	sptr->sLife = sptr->Life;

	if ((weapon == LG_PISTOLS || weapon == LG_MAGNUMS || weapon == LG_UZIS) && sptr->dR > 64)
	{
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (initial)
	{
		sptr->Xvel = short((GetRandomControl() & 0x3FF) + xv - 512);
		sptr->Yvel = short((GetRandomControl() & 0x3FF) + yv - 512);
		sptr->Zvel = short((GetRandomControl() & 0x3FF) + zv - 512);
	}
	else
	{
		sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Yvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
	}

	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
			sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	sptr->Gravity = -2 - (GetRandomControl() & 1);
	sptr->MaxYvel = -2 - (GetRandomControl() & 1);

	size = (GetRandomControl() & 7) - ((weapon == LG_ROCKET || weapon == LG_GRENADE) ? 0 : 12) + 24;

	if (initial)
	{
		sptr->Width = size >> 1;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = (size + 4) << 1;
	}
	else
	{
		sptr->Width = size >> 2;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = size;
	}

	if (initial)
	{
		sptr->Height = size >> 1;
		sptr->sHeight = sptr->Width;
		sptr->dHeight = (size + 4) << 1;
	}
	else
	{
		sptr->Height = size >> 2;
		sptr->sHeight = sptr->Width;
		sptr->dHeight = size;
	}
}

void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number)
{
	SPARKS* sptr;
	SP_DYNAMIC* pDL;
	long dx, dz, i;
	uchar extras_table[4];
	uchar r, g, b;

	extras_table[0] = 0;
	extras_table[1] = 4;
	extras_table[2] = 7;
	extras_table[3] = 10;
	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;

	if (uw == 1)
	{
		sptr->sG = (GetRandomControl() & 0x3F) + 128;
		sptr->sB = 32;
		sptr->dR = 192;
		sptr->dG = (GetRandomControl() & 0x1F) + 64;
		sptr->dB = 0;
	}
	else
	{
		sptr->sG = (GetRandomControl() & 0xF) + 32;
		sptr->sB = 0;
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;
	}

	if (uw == 1)
	{
		sptr->ColFadeSpeed = 7;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 7) + 16;
		sptr->sLife = sptr->Life;
		sptr->RoomNumber = (uchar)room_number;
	}
	else
	{
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 16;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 7) + 24;
		sptr->sLife = sptr->Life;
	}

	sptr->Dynamic = (char)dynamic;
	sptr->extras = uchar(extras | ((extras_table[extras] + (GetRandomControl() & 7) - 4) << 3));

	if (dynamic == -2)
	{
		for (i = 0; i < 32; i++)
		{
			pDL = &spark_dynamics[i];

			if (!pDL->On)
			{
				pDL->On = 1;
				pDL->Falloff = 4;

				if (uw == 1)
					pDL->Flags = 2;
				else
					pDL->Flags = 1;

				sptr->Dynamic = (char)i;
				break;
			}
		}

		if (i == 32)
			sptr->Dynamic = -1;
	}

	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = (GetRandomControl() & 0xFFF) - 2048;
	sptr->Yvel = (GetRandomControl() & 0xFFF) - 2048;
	sptr->Zvel = (GetRandomControl() & 0xFFF) - 2048;

	if (dynamic != -2 || uw == 1)
	{
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	}
	else
	{
		sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
		sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
		sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	}

	if (uw == 1)
		sptr->Friction = 17;
	else
		sptr->Friction = 51;

	if (GetRandomControl() & 1)
	{
		if (uw == 1)
			sptr->Flags = SF_UNWATER | SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0xFF) + 128;
	}
	else if (uw == 1)
		sptr->Flags = SF_UNWATER | SF_ALTDEF | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Width = (GetRandomControl() & 0xF) + 40;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width << 1;
	sptr->Height = sptr->Width + (GetRandomControl() & 7) + 8;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height << 1;

	if (uw == 2)
	{
		r = sptr->sR;
		g = sptr->sG;
		b = sptr->sB;
		sptr->sR = b;
		sptr->sG = r;
		sptr->sB = g;
		r = sptr->dR;
		g = sptr->dG;
		b = sptr->dB;
		sptr->dR = b;
		sptr->dG = r;
		sptr->dB = g;
		sptr->Flags |= 0x2000;
	}
	else if (extras)
		TriggerExplosionSmoke(x, y, z, uw);
	else
		TriggerExplosionSmokeEnd(x, y, z, uw);
}

void TriggerExplosionSmoke(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 144;
	sptr->sG = 144;
	sptr->sB = 144;
	sptr->dR = 64;
	sptr->dG = 64;
	sptr->dB = 64;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 3;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 10;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
	sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
	sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
		sptr->Friction = 2;
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 3;
	sptr->sHeight = sptr->Height;
}

void TriggerExplosionSmokeEnd(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (uw)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 208;
	}
	else
	{
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 64;
	sptr->Life = (GetRandomControl() & 0x1F) + 96;
	sptr->sLife = sptr->Life;

	if (uw)
		sptr->TransType = 2;
	else
		sptr->TransType = 3;

	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
	{
		sptr->Friction = 20;
		sptr->Yvel >>= 4;
		sptr->y += 32;
	}
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	if ((GetRandomControl() & 1) != 0)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;

	if (uw)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else
	{
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}

	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 3;
	sptr->sHeight = sptr->Height;
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
	INJECT(0x0042A680, TriggerRicochetSpark, replace);
	INJECT(0x0042C7E0, TriggerBlood, replace);
	INJECT(0x0042C950, TriggerBloodD, replace);
	INJECT(0x0042D110, TriggerUnderwaterBlood, replace);
	INJECT(0x0042D180, TriggerUnderwaterBloodD, replace);
	INJECT(0x0042A8B0, TriggerFlareSparks, replace);
	INJECT(0x0042B4F0, TriggerGunSmoke, replace);
	INJECT(0x0042AB80, TriggerExplosionSparks, replace);
	INJECT(0x0042B130, TriggerExplosionSmoke, replace);
	INJECT(0x0042AF20, TriggerExplosionSmokeEnd, replace);
}
