#include "../tomb3/pch.h"
#include "missile.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "sound.h"
#include "effect2.h"
#include "items.h"
#include "effects.h"
#include "traps.h"
#include "../specific/game.h"
#include "draw.h"
#include "lara.h"
#include "../3dsystem/3d_gen.h"

void ControlMissile(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long h, c;
	short room_number;

	fx = &effects[fx_number];

	if (fx->object_number == DIVER_HARPOON && !(room[fx->room_number].flags & ROOM_UNDERWATER))
	{
		if (fx->pos.x_rot > -0x3000)
			fx->pos.x_rot -= 182;
	}

	fx->pos.x_pos += (((fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT) * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += (fx->speed * phd_sin(-fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.z_pos += (((fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT) * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos <= c)
	{
		if (fx->object_number == KNIFE || fx->object_number == DIVER_HARPOON)
			SoundEffect(fx->object_number == DIVER_HARPOON ? SFX_LARA_RICOCHET : SFX_SHIVA_SWORD_1, &fx->pos, SFX_DEFAULT);
		else if (fx->object_number == DRAGON_FIRE)
		{
			TriggerFireFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 0);
			TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, 24, 255, 192, GetRandomControl() & 0x3F);
		}

		KillEffect(fx_number);
		return;
	}

	if (room_number != fx->room_number)
		EffectNewRoom(fx_number, room_number);

	if (fx->object_number == DRAGON_FIRE)
	{
		if (ItemNearLara(&fx->pos, 350))
		{
			lara_item->hit_points -= 3;
			lara_item->hit_status = 1;
			LaraBurn();
			return;
		}
	}
	else if (ItemNearLara(&fx->pos, 200))
	{
		if (fx->object_number == KNIFE || fx->object_number == DIVER_HARPOON)
		{
			lara_item->hit_points -= 50;
			SoundEffect(SFX_MACAQUE_CHATTER, &fx->pos, SFX_DEFAULT);
			KillEffect(fx_number);
		}

		lara_item->hit_status = 1;
		fx->pos.y_rot = lara_item->pos.y_rot;
		fx->speed = lara_item->speed;
		fx->counter = 0;
		fx->frame_number = 0;
	}

	if (fx->object_number == DIVER_HARPOON && room[fx->room_number].flags & ROOM_UNDERWATER)
	{
		if (!(wibble & 0xF))
			CreateBubble(&fx->pos, room_number, 8, 8);

		TriggerRocketSmoke(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, 64);
	}
	else if (fx->object_number == DRAGON_FIRE && !fx->counter--)
	{
		TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, 24, 255, 192, GetRandomControl() & 0x3F);
		SoundEffect(SFX_SECURITY_GUARD_FIRE, &fx->pos, 0);
		KillEffect(fx_number);
	}
	else if (fx->object_number == KNIFE)
		fx->pos.z_rot += 5460;
}

void ShootAtLara(FX_INFO* fx)
{
	short* bounds;
	long x, z, dx, dy, dz;

	dx = lara_item->pos.x_pos - fx->pos.x_pos;
	dy = lara_item->pos.y_pos - fx->pos.y_pos;
	dz = lara_item->pos.z_pos - fx->pos.z_pos;

	bounds = GetBoundsAccurate(lara_item);
	x = bounds[3] + 3 * (bounds[2] - bounds[3]) / 4 + dy;
	z = phd_sqrt(SQUARE(dx) + SQUARE(dz));

	fx->pos.x_rot = -(short)phd_atan(z, x);
	fx->pos.y_rot = (short)phd_atan(dz, dx);
	fx->pos.x_rot += short((GetRandomControl() - 0x4000) / 64);
	fx->pos.y_rot += short((GetRandomControl() - 0x4000) / 64);
}

long ExplodingDeath(short item_number, long mesh_bits, short counter)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FX_INFO* fx;
	long* bone;
	short* frame;
	short* rot;
	short* extra_rotation;
	long bit, lp;
	short obj_num, fx_number;

	item = &items[item_number];
	obj = &objects[item->object_number];
	bone = &bones[obj->bone_index];
	frame = GetBestFrame(item);
	extra_rotation = (short*)item->data;

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);

	bit = 1;

	if (mesh_bits & bit && item->mesh_bits & bit)
	{
		obj_num = item->object_number;

		if (!(GetRandomControl() & 3) || obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT1 || obj_num == SMASH_OBJECT2 || obj_num == SMASH_OBJECT3)
		{
			fx_number = CreateEffect(item->room_number);

			if (fx_number != NO_ITEM)
			{
				fx = &effects[fx_number];
				fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
				fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
				fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
				fx->room_number = item->room_number;
				fx->pos.y_rot = short((GetRandomControl() << 1) + 0x8000);
				fx->speed = (short)GetRandomControl() >> 8;
				fx->fallspeed = (short)-GetRandomControl() >> 8;

				if (obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT1 || obj_num == SMASH_OBJECT2 ||
					obj_num == SMASH_OBJECT3 || obj_num == MUTANT2 || obj_num == QUADBIKE)
					fx->counter = 0;
				else
					fx->counter = (counter << 2) | GetRandomControl() & 3;

				fx->object_number = BODY_PART;
				fx->frame_number = obj->mesh_index;
			}

			item->mesh_bits -= bit;
		}
	}

	for (lp = 1; lp < obj->nmeshes; lp++)
	{
		if (bone[0] & 1)
			phd_PopMatrix();

		if (bone[0] & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rot, 0);

		if (item->data && bone[0] & 0x1C)
		{
			if (bone[0] & 8)
				phd_RotY(*extra_rotation++);

			if (bone[0] & 4)
				phd_RotX(*extra_rotation++);

			if (bone[0] & 0x10)
				phd_RotZ(*extra_rotation++);
		}

		bit <<= 1;

		if (mesh_bits & bit && item->mesh_bits & bit)
		{
			obj_num = item->object_number;

			if (!(GetRandomControl() & 3) || obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT1 || obj_num == SMASH_OBJECT2 || obj_num == SMASH_OBJECT3)
			{
				fx_number = CreateEffect(item->room_number);

				if (fx_number != NO_ITEM)
				{
					fx = &effects[fx_number];
					fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
					fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
					fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
					fx->room_number = item->room_number;
					fx->pos.y_rot = short((GetRandomControl() << 1) + 0x8000);
					fx->speed = (short)GetRandomControl() >> 8;
					fx->fallspeed = (short)-GetRandomControl() >> 8;

					if (obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT1 || obj_num == SMASH_OBJECT2 ||
						obj_num == SMASH_OBJECT3 || obj_num == MUTANT2 || obj_num == QUADBIKE)
						fx->counter = 0;
					else
						fx->counter = (counter << 2) | GetRandomControl() & 3;

					fx->object_number = BODY_PART;
					fx->frame_number = short(obj->mesh_index + lp);
					fx->shade = 0x4210;
				}

				item->mesh_bits -= bit;
			}
		}

		bone += 4;
	}

	phd_PopMatrix();
	return !(item->mesh_bits & (0x7FFFFFFF >> (31 - obj->nmeshes)));
}

void ControlBodyPart(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long h, c, lp;
	short room_number;

	fx = &effects[fx_number];
	fx->pos.x_rot += 910;
	fx->pos.z_rot += 1820;
	fx->fallspeed += 3;
	fx->pos.x_pos += (fx->speed * phd_sin(fx->pos.y_rot)) >> (W2V_SHIFT + 2);
	fx->pos.y_pos += fx->fallspeed;
	fx->pos.z_pos += (fx->speed * phd_cos(fx->pos.y_rot)) >> (W2V_SHIFT + 2);

	if (!(wibble & 0xC))
	{
		if (fx->counter & 1)
			TriggerFireFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx_number, 0);

		if (fx->counter & 2)
			TriggerFireSmoke(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 0);
	}

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos < c)
	{
		fx->pos.y_pos = c;
		fx->fallspeed = -fx->fallspeed;
	}

	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h)
	{
		if (fx->counter & 3)
		{
			for (lp = 0; lp < 3; lp++)
			{
				if (fx->counter & 1)
					TriggerFireFlame(fx->pos.x_pos, h, fx->pos.z_pos, -1, 0);

				if (fx->counter & 2)
					TriggerFireSmoke(fx->pos.x_pos, h, fx->pos.z_pos, -1, 0);
			}

			SoundEffect(SFX_EXPLOSION1, &fx->pos, SFX_DEFAULT);
		}

		KillEffect(fx_number);
		return;
	}

	if (ItemNearLara(&fx->pos, fx->counter & ~3))
	{
		lara_item->hit_points -= fx->counter >> 2;
		lara_item->hit_status = 1;

		if (fx->counter & 3)
		{
			for (lp = 0; lp < 3; lp++)
			{
				if (fx->counter & 1)
					TriggerFireFlame(fx->pos.x_pos, h, fx->pos.z_pos, -1, 0);

				if (fx->counter & 2)
					TriggerFireSmoke(fx->pos.x_pos, h, fx->pos.z_pos, -1, 0);
			}

			SoundEffect(SFX_EXPLOSION1, &fx->pos, SFX_DEFAULT);
		}

		KillEffect(fx_number);
	}

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, room_number);
}
