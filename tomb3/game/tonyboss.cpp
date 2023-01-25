#include "../tomb3/pch.h"
#include "tonyboss.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "sphere.h"
#include "items.h"

static void TriggerTonyFlame(short item_number, long node)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long dx, dz;

	item = &items[item_number];
	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;

	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = (uchar)node;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

static void TriggerFireBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle, long speed)
{
	FX_INFO* fx;
	PHD_VECTOR fxpos;
	long fallspeed;
	short fxNum;

	if (!type)
	{
		fxpos.x = 0;
		fxpos.y = 0;
		fxpos.z = 0;
		GetJointAbsPosition(item, &fxpos, 10);
		angle = item->pos.y_rot;
		fallspeed = -16;
		speed = 0;
	}
	else if (type == 1)
	{
		fxpos.x = 0;
		fxpos.y = 0;
		fxpos.z = 0;
		GetJointAbsPosition(item, &fxpos, 13);
		angle = item->pos.y_rot;
		fallspeed = -16;
		speed = 0;
	}
	else if (type == 2)
	{
		fxpos.x = 0;
		fxpos.y = 0;
		fxpos.z = 0;
		GetJointAbsPosition(item, &fxpos, 13);
		speed = 160;
		fallspeed = -32 - (GetRandomControl() & 7);
	}
	else if (type == 3)
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed = 0;
		fallspeed = (GetRandomControl() & 3) + 4;
	}
	else if (type == 4)
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed += (GetRandomControl() & 3);
		angle = short(GetRandomControl() << 1);
		fallspeed = (GetRandomControl() & 3) - 2;
	}
	else if (type == 5)
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed = (GetRandomControl() & 7) + 48;
		angle += (GetRandomControl() & 0x1FFF) + 28672;
		fallspeed = -16 - (GetRandomControl() & 0xF);
	}
	else
	{
		fxpos.x = pos->x;
		fxpos.y = pos->y;
		fxpos.z = pos->z;
		speed = (GetRandomControl() & 0x1F) + 32;
		angle = short(GetRandomControl() << 1);
		fallspeed = -32 - (GetRandomControl() & 0x1F);
	}

	fxNum = CreateEffect(room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = fxpos.x;
	fx->pos.y_pos = fxpos.y;
	fx->pos.z_pos = fxpos.z;
	fx->pos.y_rot = angle;
	fx->object_number = TONYFIREBALL;
	fx->speed = (short)speed;
	fx->fallspeed = (short)fallspeed;
	fx->flag1 = (short)type;
	fx->flag2 = (GetRandomControl() & 3) + 1;

	if (type == 5)
		fx->flag2 <<= 1;
	else if (type == 2)
		fx->flag2 = 0;
}

void inject_tonyboss(bool replace)
{
	INJECT(0x0046C460, TriggerTonyFlame, replace);
	INJECT(0x0046C640, TriggerFireBall, replace);
}
