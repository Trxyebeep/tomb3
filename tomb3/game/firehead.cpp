#include "../tomb3/pch.h"
#include "firehead.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"

static void TriggerFireHeadFlame(long x, long y, long z, long angle, long speed)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz>0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) + 192;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
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
	speed -= GetRandomControl() % ((speed >> 3) + 1);
	sptr->Xvel = short((GetRandomControl() & 0x7F) + ((speed * rcossin_tbl[angle]) >> 11) - 64);
	sptr->Yvel = (GetRandomControl() & 7) + 6;
	sptr->Zvel = short((GetRandomControl() & 0x7F) + ((speed * rcossin_tbl[angle + 1]) >> 11) - 64);
	sptr->Friction = 4;
	sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	
	if (GetRandomControl() & 1)
	{
		sptr->Flags |= SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x3F) - 32;
	}

	sptr->Gravity = -8 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	sptr->dWidth = uchar((speed >> 4) + (GetRandomControl() & 0xF));
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
}

void InitialiseFireHead(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[item->object_number].anim_index + 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = FIREHEAD_REAR;
	item->goal_anim_state = FIREHEAD_REAR;
}

void inject_firehead(bool replace)
{
	INJECT(0x0042FE30, TriggerFireHeadFlame, replace);
	INJECT(0x0042FAF0, InitialiseFireHead, replace);
}
