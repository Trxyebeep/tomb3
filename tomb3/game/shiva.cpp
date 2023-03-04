#include "../tomb3/pch.h"
#include "shiva.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "box.h"
#include "effects.h"
#include "sound.h"

static BITE_INFO shiva_right = { 0, 0, 920, 22 };
static BITE_INFO shiva_left = { 0, 0, 920, 13 };

static void TriggerShivaSmoke(long x, long y, long z, long yt)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (yt)
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

	if (yt)
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

	if (yt)
	{
		sptr->Friction = 20;
		sptr->y += 32;
		sptr->Yvel >>= 4;
	}
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;

	if (yt)
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
	sptr->sWidth = sptr->Width >> 2;
	
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 3;
	sptr->sHeight = sptr->Height;
}

static void ShivaDamage(ITEM_INFO* item, CREATURE_INFO* shiva, long damage)
{
	if (!shiva->flags && item->touch_bits & 0x2400000)
	{
		lara_item->hit_points -= (short)damage;
		lara_item->hit_status = 1;
		CreatureEffect(item, &shiva_right, DoBloodSplat);
		shiva->flags = 1;
		SoundEffect(SFX_MACAQUE_ROLL, &item->pos, 0);
	}

	if (!shiva->flags && item->touch_bits & 0x2400)
	{
		lara_item->hit_points -= (short)damage;
		lara_item->hit_status = 1;
		CreatureEffect(item, &shiva_left, DoBloodSplat);
		shiva->flags = 1;
		SoundEffect(SFX_MACAQUE_ROLL, &item->pos, 0);
	}
}

void inject_shiva(bool replace)
{
	INJECT(0x00466E00, TriggerShivaSmoke, replace);
	INJECT(0x00467780, ShivaDamage, replace);
}
