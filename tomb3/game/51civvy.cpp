#include "../tomb3/pch.h"
#include "51civvy.h"
#include "effect2.h"
#include "../specific/game.h"
#include "control.h"
#include "gameflow.h"
#include "../specific/smain.h"

static void TriggerFenceSparks(long x, long y, long z, long kill)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sG;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 16;
	sptr->dB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = sptr->sB >> 2;
	sptr->dG = sptr->sB >> 1;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = ((GetRandomControl() & 0xFF) - 128) << 1;
	sptr->Yvel = short((GetRandomControl() & 0xF) - (kill << 5) - 8);
	sptr->Zvel = ((GetRandomControl() & 0xFF) - 128) << 1;
	sptr->Friction = 4;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = uchar(kill + 1);
	sptr->sWidth = 1;
	sptr->dWidth = 1;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 0xF) + 16;
}

void ControlElectricFence(short item_number)
{
	ITEM_INFO* item;
	long dx, dz, x, z, ex, ey, ez, xa, za, xs, zs, num, num2, oex, oez;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -20480 || dx > 20480 || dz < -20480 || dz > 20480)
		return;

	if (!item->pos.y_rot)
	{
		x = item->pos.x_pos + 512;
		z = item->pos.z_pos + 512;
		ex = x - 992;
		ez = item->pos.z_pos + 256;
		xa = 0x7FF;
		za = 0;
		xs = 1056;
		zs = 128;
	}
	else if (item->pos.y_rot == -0x4000)
	{
		x = item->pos.x_pos - 512;
		z = item->pos.z_pos + 512;
		ex = x + 256;
		ez = z - 992;
		xa = 0;
		za = 0x7FF;
		xs = 128;
		zs = 1056;
	}
	else if (item->pos.y_rot == -0x8000)
	{
		x = item->pos.x_pos - 512;
		z = item->pos.z_pos - 512;
		ex = x - 992;
		ez = item->pos.z_pos - 256;
		xa = 2047;
		za = 0;
		xs = 1056;
		zs = 128;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		x = item->pos.x_pos + 512;
		z = item->pos.z_pos - 512;
		ex = x - 256;
		ez = z - 992;
		xa = 0;
		za = 2047;
		xs = 128;
		zs = 1056;
	}
	else
	{
		x = 0;
		z = 0;
		ex = 0;
		ez = 0;
		xa = 0;
		za = 0;
		xs = 0;
		zs = 0;
	}

	if (!(GetRandomControl() & 0x3F))
	{
		if (xa)
			ex += GetRandomControl() & xa;
		else
			ez += GetRandomControl() & za;

#ifdef RANDO_STUFF
		if (rando.levels[RANDOLEVEL].original_id == LV_OFFICE)
#else
		if (CurrentLevel == LV_OFFICE)
#endif
			ey = item->pos.y_pos - (GetRandomControl() & 0x1F);
		else
			ey = item->pos.y_pos - (GetRandomControl() & 0x7FF) - (GetRandomControl() & 0x3FF);

		num = (GetRandomControl() & 3) + 3;

		for (int i = 0; i < num; i++)
		{
			TriggerFenceSparks(ex, ey, ez, 0);

			if (xa)
				ex += (xa & GetRandomControl() & 7) - 4;
			else
				ez += (za & GetRandomControl() & 7) - 4;

			ey += (GetRandomControl() & 7) - 4;
		}
	}

#ifdef RANDO_STUFF
	if (lara.electric || rando.levels[RANDOLEVEL].original_id == LV_OFFICE ||
#else
	if (lara.electric || CurrentLevel == LV_OFFICE ||
#endif
		lara_item->pos.x_pos < x - xs || lara_item->pos.x_pos > x + xs ||
		lara_item->pos.z_pos < z - zs || lara_item->pos.z_pos > z + zs ||
		lara_item->pos.y_pos > item->pos.y_pos + 32 || lara_item->pos.y_pos < item->pos.y_pos - 3072)
		return;

	oex = ex;
	oez = ez;
	num = (GetRandomControl() & 0xF) + 3;

	for (int i = 0; i < num; i++)
	{
		if (xa)
			ex = (GetRandomControl() & 0x1FF) + lara_item->pos.x_pos - 256;
		else
			ez = (GetRandomControl() & 0x1FF) + lara_item->pos.z_pos - 256;

		ey = lara_item->pos.y_pos - GetRandomControl() % 768;
		num2 = (GetRandomControl() & 3) + 6;

		for (int j = 0; j < num2; j++)
		{
			TriggerFenceSparks(ex, ey, ez, 1);

			if (xa)
				ex += (xa & GetRandomControl() & 7) - 4;
			else
				ez += (za & GetRandomControl() & 7) - 4;

			ey += (GetRandomControl() & 7) - 4;
		}

		ex = oex;
		ez = oez;
	}

	lara.electric = 1;
	lara_item->hit_points = 0;
}

void inject_civvy(bool replace)
{
	INJECT(0x0040F080, TriggerFenceSparks, replace);
	INJECT(0x0040ECA0, ControlElectricFence, inject_rando ? 1 : replace);
}
