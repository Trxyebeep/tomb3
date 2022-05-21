#include "../tomb3/pch.h"
#include "laraflar.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "objects.h"
#include "../specific/litesrc.h"
#include "../specific/game.h"
#include "effect2.h"
#include "lara.h"
#include "sound.h"
#include "items.h"
#include "control.h"

void DrawFlareInAir(ITEM_INFO* item)
{
	short* frame[2];
	long rate, clip, x, y, z;

	GetFrames(item, frame, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frame[0]);

	if (clip)
	{
		CalculateObjectLighting(item, frame[0]);
		phd_PutPolygons(meshes[objects[FLARE_ITEM].mesh_index], clip);

		if ((long)item->data & 0x8000)
		{
			phd_TranslateRel(-6, 6, 48);
			phd_RotX(-16380);
			S_CalculateStaticLight(2048);

			phd_PushUnitMatrix();
			phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

			phd_PushMatrix();
			phd_TranslateRel(-6, 6, 32);
			x = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
			y = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
			z = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
			phd_PopMatrix();

			phd_TranslateRel((GetRandomDraw() & 0x7F) - 64, (GetRandomDraw() & 0x7F) - 64, (GetRandomDraw() & 0x1FF) + 512);

			for (int i = 0; i < (GetRandomDraw() & 3) + 4; i++)
				TriggerFlareSparks(x, y, z, phd_mxptr[M03] >> W2V_SHIFT, phd_mxptr[M13] >> W2V_SHIFT, phd_mxptr[M23] >> W2V_SHIFT, i >> 2, 0);

			phd_PopMatrix();
		}
	}

	phd_PopMatrix();
}

long DoFlareLight(PHD_VECTOR* pos, long flare_age)
{
	long rnd, x, y, z, r, g, b, falloff;

	if (flare_age >= 900)
		return 0;

	rnd = GetRandomControl();
	x = pos->x + 8 * (rnd & 0xF);
	y = pos->y + ((rnd >> 1) & 0x78);
	z = pos->z + ((rnd >> 5) & 0x78);

	if (flare_age < 4)
	{
		r = (rnd & 3) + (flare_age << 1) + 20;
		g = ((rnd >> 4) & 3) + flare_age + 4;
		b = ((rnd >> 8) & 3) + (flare_age << 1);
		falloff = (rnd & 3) + (flare_age << 2) + 4;

		if (falloff > 16)
			falloff -= (rnd >> 12) & 3;

		TriggerDynamic(x, y, z, falloff, r, g, b);
	}
	else if (flare_age < 16)
	{
		r = (rnd & 7) + (flare_age >> 1) + 16;
		g = ((rnd >> 4) & 3) + (flare_age >> 1) + 8;
		b = ((rnd >> 8) & 3) + (flare_age >> 1) + 2;
		falloff = (rnd & 1) + flare_age + 2;
		TriggerDynamic(x, y, z, falloff, r, g, b);
	}
	else if (flare_age < 810)
	{
		r = (rnd & 7) + 24;
		g = ((rnd >> 4) & 3) + 16;
		b = ((rnd >> 8) & 4) + (((rnd >> 6) & 0x10) >> 2);
		TriggerDynamic(x, y, z, 16, r, g, b);
	}
	else if (flare_age < 876)
	{
		if (rnd > 0x2000)
		{
			r = (rnd & 7) + 24;
			g = ((rnd >> 4) & 3) + 16;
			b = ((rnd >> 8) & 4) + (((rnd >> 6) & 0x10) >> 2);
			TriggerDynamic(x, y, z, 16, r, g, b);
		}
		else
		{
			r = (GetRandomControl() & 7) + 24;
			g = (GetRandomControl() & 7) + 8;
			b = GetRandomControl() & 0xF;
			falloff = (GetRandomControl() & 6) + 8;
			TriggerDynamic(x, y, z, falloff, r, g, b);
			return 0;
		}
	}
	else
	{
		r = (GetRandomControl() & 7) + 24;
		g = (GetRandomControl() & 7) + 8;
		b = GetRandomControl() & 3;
		falloff = 16 - ((flare_age - 876) >> 1);
		TriggerDynamic(x, y, z, falloff, r, g, b);
		return rnd & 1;
	}

	return 1;
}

void DoFlareInHand(long flare_age)
{
	PHD_VECTOR pos;

	pos.x = 11;
	pos.y = 32;
	pos.z = 41;
	GetLaraHandAbsPosition(&pos, LEFT_HAND);
	lara.left_arm.flash_gun = (short)DoFlareLight(&pos, flare_age);

	if (lara.flare_age < 900)
	{
		lara.flare_age++;

		if (room[lara_item->room_number].flags & ROOM_UNDERWATER)
		{
			SoundEffect(SFX_LARA_FLARE_BURN, &lara_item->pos, SFX_WATER);

			if (GetRandomControl() < 0x4000)
				CreateBubble();
		}
		else
			SoundEffect(SFX_LARA_FLARE_BURN, &lara_item->pos, 0x2000000 | SFX_SETPITCH);
	}
	else if (lara.gun_status == LG_UNARMED)
		lara.gun_status = LG_UNDRAW;
}

void CreateFlare(long thrown)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long h;
	short item_num;

	item_num = CreateItem();

	if (item_num == NO_ITEM)
		return;

	item = &items[item_num];
	item->object_number = FLARE_ITEM;
	item->room_number = lara_item->room_number;
	pos.x = -16;
	pos.y = 32;
	pos.z = 42;
	GetLaraHandAbsPosition(&pos, LEFT_HAND);

	floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
	h = GetHeight(floor, pos.x, pos.y, pos.z);

	if (h < pos.y)
	{
		item->pos.x_pos = lara_item->pos.x_pos;
		item->pos.y_pos = pos.y;
		item->pos.z_pos = lara_item->pos.z_pos;
		item->pos.y_rot = lara_item->pos.y_rot;
		item->room_number = lara_item->room_number;
	}
	else
	{
		item->pos.x_pos = pos.x;
		item->pos.y_pos = pos.y;
		item->pos.z_pos = pos.z;

		if (thrown)
			item->pos.y_rot = lara_item->pos.y_rot;
		else
			item->pos.y_rot = lara_item->pos.y_rot - 0x2000;
	}

	InitialiseItem(item_num);
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->shade = -1;

	if (thrown)
	{
		item->speed = lara_item->speed + 50;
		item->fallspeed = lara_item->fallspeed - 50;
	}
	else
	{
		item->speed = lara_item->speed + 10;
		item->fallspeed = lara_item->fallspeed + 50;
	}

	if (DoFlareLight((PHD_VECTOR*)&item->pos, lara.flare_age))
		item->data = (void*)(lara.flare_age | 0x8000);
	else
		item->data = (void*)(lara.flare_age & 0x7FFF);

	AddActiveItem(item_num);
	item->status = ITEM_ACTIVE;
}

void inject_laraflar(bool replace)
{
	INJECT(0x0044BBD0, DrawFlareInAir, replace);
	INJECT(0x0044B940, DoFlareLight, replace);
	INJECT(0x0044BAF0, DoFlareInHand, replace);
	INJECT(0x0044BD90, CreateFlare, replace);
}
