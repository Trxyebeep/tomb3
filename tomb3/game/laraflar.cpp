#include "../tomb3/pch.h"
#include "laraflar.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "objects.h"
#include "../specific/litesrc.h"
#include "../specific/game.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "sound.h"
#include "items.h"
#include "control.h"
#include "invfunc.h"
#include "larafire.h"
#include "../3dsystem/phd_math.h"
#include "savegame.h"
#include "../newstuff/LaraDraw.h"

#define FLARE_AGE		900
#define FLARE_DYING		(FLARE_AGE - 90)
#define FLARE_END		(FLARE_AGE - 24)

void DrawFlareInAir(ITEM_INFO* item)
{
	short* frame[2];
	long rate, clip, x, y, z;

	GetFrames(item, frame, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos - GetBoundsAccurate(item)[3], item->pos.z_pos);
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
			phd_SetTrans(0, 0, 0);
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

	if (flare_age >= FLARE_AGE)
		return 0;

	rnd = GetRandomControl();
	x = pos->x + ((rnd & 0xF) << 3);
	y = pos->y + ((rnd >> 1) & 0x78);
	z = pos->z + ((rnd >> 5) & 0x78);

	if (flare_age < 4)
	{
		r = (rnd & 0x1F) + (flare_age << 4) + 160;
		g = ((rnd >> 4) & 0x1F) + (flare_age << 3) + 32;
		b = ((rnd >> 8) & 0x1F) + (flare_age << 4);
		falloff = (rnd & 3) + (flare_age << 2) + 4;

		if (falloff > 16)
			falloff -= (rnd >> 12) & 3;
	}
	else if (flare_age < 16)
	{
		r = (rnd & 0x3F) + (flare_age << 2) + 128;
		g = ((rnd >> 4) & 0x1F) + (flare_age << 2) + 64;
		b = ((rnd >> 8) & 0x1F) + (flare_age << 2) + 16;
		falloff = (rnd & 1) + flare_age + 2;
	}
	else if (flare_age < FLARE_DYING)
	{
		r = (rnd & 0x3F) + 192;
		g = ((rnd >> 4) & 0x1F) + 128;
		b = ((rnd >> 8) & 0x20) + (((rnd >> 6) & 0x10) << 1);
		falloff = 16;
	}
	else if (flare_age < FLARE_END)
	{
		if (rnd > 0x2000)
		{
			r = (rnd & 0x3F) + 192;
			g = ((rnd >> 4) & 0x1F) + 64;
			b = ((rnd >> 8) & 0x20) + (((rnd >> 6) & 0x10) << 1);
			falloff = 16;
		}
		else
		{
			r = (GetRandomControl() & 0x3F) + 192;
			g = (GetRandomControl() & 0x3F) + 64;
			b = GetRandomControl() & 0x7F;
			falloff = (GetRandomControl() & 6) + 8;
			TriggerDynamic(x, y, z, falloff, r, g, b);
			return 0;
		}
	}
	else
	{
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x3F) + 64;
		b = GetRandomControl() & 0x1F;
		falloff = 16 - ((flare_age - FLARE_END) >> 1);
		TriggerDynamic(x, y, z, falloff, r, g, b);
		return rnd & 1;
	}

	TriggerDynamic(x, y, z, falloff, r, g, b);
	return 1;
}

void DoFlareInHand(long flare_age)
{
	PHD_VECTOR pos;

	pos.x = 11;
	pos.y = 32;
	pos.z = 41;
	GetLaraMeshPos(&pos, LMX_HAND_L);
	lara.left_arm.flash_gun = (short)DoFlareLight(&pos, flare_age);

	if (lara.flare_age < FLARE_AGE)
	{
		lara.flare_age++;

		if (room[lara_item->room_number].flags & ROOM_UNDERWATER)
		{
			SoundEffect(SFX_LARA_FLARE_BURN, &lara_item->pos, SFX_WATER);

			if (GetRandomControl() < 0x4000)
				CreateBubble((PHD_3DPOS*)&pos, lara_item->room_number, 8, 8);
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
	GetLaraMeshPos(&pos, LMX_HAND_L);

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

void set_flare_arm(long frame)
{
	short anim;

	if (frame < 1)
		anim = objects[FLARE].anim_index;
	else if (frame < 33)
		anim = objects[FLARE].anim_index + 1;
	else if (frame < 72)
		anim = objects[FLARE].anim_index + 2;
	else if (frame < 95)
		anim = objects[FLARE].anim_index + 3;
	else
		anim = objects[FLARE].anim_index + 4;

	lara.left_arm.anim_number = anim;
	lara.left_arm.frame_base = anims[anim].frame_ptr;
}

void draw_flare_meshes()
{
	lara.mesh_ptrs[HAND_L] = meshes[objects[FLARE].mesh_index + HAND_L];
}

void undraw_flare_meshes()
{
	lara.mesh_ptrs[HAND_L] = meshes[objects[LARA].mesh_index + HAND_L];
}

void ready_flare()
{
	lara.gun_status = LG_ARMLESS;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.target = 0;
}

void draw_flare()
{
	short ani;

	if (lara_item->current_anim_state == AS_FLAREPICKUP || lara_item->current_anim_state == AS_PICKUP)
	{
		DoFlareInHand(lara.flare_age);
		lara.flare_control_left = 0;
		lara.left_arm.frame_number = 93;
		set_flare_arm(93);
		return;
	}

	lara.flare_control_left = 1;
	ani = lara.left_arm.frame_number + 1;

	if (ani < 33 || ani > 94)
		ani = 33;
	else if (ani == 46)
	{
		draw_flare_meshes();

		if (!savegame.bonus_flag)
			Inv_RemoveItem(FLAREBOX_ITEM);
	}
	else if (ani >= 72 && ani <= 93)
	{
		if (ani == 72)
		{
			if (room[lara_item->room_number].flags & ROOM_UNDERWATER)
				SoundEffect(SFX_LARA_FLARE_IGNITE, &lara_item->pos, SFX_WATER);
			else
				SoundEffect(SFX_LARA_FLARE_IGNITE, &lara_item->pos, SFX_DEFAULT);

			lara.flare_age = 0;
		}

		DoFlareInHand(lara.flare_age);
	}
	else if (ani == 94)
	{
		ready_flare();
		ani = 0;
		DoFlareInHand(lara.flare_age);
	}

	lara.left_arm.frame_number = ani;
	set_flare_arm(lara.left_arm.frame_number);
}

void undraw_flare()
{
	short ani, ani2;

	lara.flare_control_left = 1;
	ani = lara.left_arm.frame_number;
	ani2 = lara.flare_frame;

	if (lara_item->goal_anim_state == AS_STOP && lara.skidoo == NO_ITEM)
	{
		if (lara_item->anim_number == ANIM_BREATH)
		{
			lara_item->anim_number = ANIM_THROWFLARE;
			ani2 = ani + anims[ANIM_THROWFLARE].frame_base;
			lara.flare_frame = ani2;
			lara_item->frame_number = ani2;
		}

		if (lara_item->anim_number == ANIM_THROWFLARE)
		{
			lara.flare_control_left = 0;

			if (ani2 >= anims[ANIM_THROWFLARE].frame_base + 31)
			{
				lara.request_gun_type = lara.last_gun_type;
				lara.gun_type = lara.last_gun_type;
				lara.gun_status = LG_ARMLESS;
				InitialiseNewWeapon();
				lara.target = 0;
				lara.right_arm.lock = 0;
				lara.left_arm.lock = 0;
				lara_item->anim_number = ANIM_STOP;
				lara.flare_frame = anims[ANIM_STOP].frame_base;
				lara_item->frame_number = lara.flare_frame;
				lara_item->current_anim_state = AS_STOP;
				lara_item->goal_anim_state = AS_STOP;
				return;
			}

			lara.flare_frame = ani2 + 1;
		}
	}
	else if (lara_item->current_anim_state == AS_STOP && lara.skidoo == NO_ITEM)
	{
		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = anims[ANIM_STOP].frame_base;
	}

	if (!ani)
		ani = 1;
	else if (ani >= 72 && ani < 95)
	{
		ani++;

		if (ani == 94)
			ani = 1;
	}
	else if (ani >= 1 && ani < 33)
	{
		ani++;

		if (ani == 21)
		{
			CreateFlare(1);
			undraw_flare_meshes();
		}
		else if (ani == 33)
		{
			ani = 0;
			lara.request_gun_type = lara.last_gun_type;
			lara.gun_type = lara.last_gun_type;
			lara.gun_status = LG_ARMLESS;
			InitialiseNewWeapon();
			lara.flare_control_left = 0;
			lara.target = 0;
			lara.right_arm.lock = 0;
			lara.left_arm.lock = 0;
			lara.flare_frame = 0;
		}
	}
	else if (ani >= 95 && ani < 110)
	{
		ani++;

		if (ani == 109)
			ani = 1;
	}

	if (ani >= 1 && ani < 21)
		DoFlareInHand(lara.flare_age);

	lara.left_arm.frame_number = ani;
	set_flare_arm(ani);
}

void FlareControl(short item_number)
{
	ITEM_INFO* item;
	long x, y, z, xv, zv, age;

	item = &items[item_number];

	if (room[item->room_number].flags & ROOM_SWAMP)
	{
		KillItem(item_number);
		return;
	}

	if (item->fallspeed)
	{
		item->pos.x_rot += 546;
		item->pos.z_rot += 910;
	}
	else
	{
		item->pos.x_rot = 0;
		item->pos.z_rot = 0;
	}

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	xv = (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	zv = (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.x_pos += xv;
	item->pos.z_pos += zv;

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		item->fallspeed += (5 - item->fallspeed) >> 1;
		item->speed += (5 - item->speed) >> 1;
	}
	else
		item->fallspeed += 6;

	item->pos.y_pos += item->fallspeed;
	DoProperDetection(item_number, x, y, z, xv, item->fallspeed, zv);
	age = (long)item->data & 0x7FFF;

	if (age < FLARE_AGE)
		age++;
	else if (!item->fallspeed && !item->speed)
	{
		KillItem(item_number);
		return;
	}

	if (DoFlareLight((PHD_VECTOR*)&item->pos, age))
	{
		age |= 0x8000;

		if (room[item->room_number].flags & ROOM_UNDERWATER)
		{
			SoundEffect(SFX_LARA_FLARE_BURN, &item->pos, SFX_WATER);

			if (GetRandomControl() < 0x4000)
				CreateBubble(&item->pos, item->room_number, 8, 8);
		}
		else
			SoundEffect(SFX_LARA_FLARE_BURN, &item->pos, 0x2000000 | SFX_SETPITCH);
	}

	item->data = (void*)age;
}
