#include "../tomb3/pch.h"
#include "larafire.h"
#include "objects.h"
#include "../specific/game.h"
#include "../3dsystem/3d_gen.h"
#include "sphere.h"
#include "control.h"
#include "gameflow.h"
#include "effects.h"
#include "triboss.h"
#include "effect2.h"
#include "sound.h"
#include "../3dsystem/phd_math.h"

long WeaponObject(long weapon_type)
{
	switch (weapon_type)
	{
	case LG_MAGNUMS:
		return MAGNUM;

	case LG_UZIS:
		return UZI;

	case LG_SHOTGUN:
		return SHOTGUN;

	case LG_M16:
		return M16;

	case LG_ROCKET:
		return ROCKET_GUN;

	case LG_GRENADE:
		return GRENADE_GUN;

	case LG_HARPOON:
		return HARPOON_GUN;

	default:
		return PISTOLS;
	}
}

long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* source, short* angles)
{
	AMMO_INFO* ammo;
	WEAPON_INFO* wep;
	SPHERE* sphere;
	PHD_3DPOS view;
	GAME_VECTOR start;
	GAME_VECTOR dest;
	SPHERE spheres[33];
	long nSpheres, best, bestdist, r, los, objLos, dx, dz;
	short room_number, obj_num, angle;

	if (weapon_type == LG_MAGNUMS)
	{
		ammo = &lara.magnums;

		if (savegame.bonus_flag)
			ammo->ammo = 1000;
	}
	else if (weapon_type == LG_UZIS)
	{
		ammo = &lara.uzis;

		if (savegame.bonus_flag)
			ammo->ammo = 1000;
	}
	else if (weapon_type == LG_SHOTGUN)
	{
		ammo = &lara.shotgun;

		if (savegame.bonus_flag)
			ammo->ammo = 1000;
	}
	else if (weapon_type == LG_M16)
	{
		ammo = &lara.m16;

		if (savegame.bonus_flag)
			ammo->ammo = 1000;
	}
	else
	{
		ammo = &lara.pistols;
		ammo->ammo = 1000;
	}

	if (ammo->ammo <= 0)
	{
		ammo->ammo = 0;
		return 0;
	}

	ammo->ammo--;
	wep = &weapons[weapon_type];
	view.x_pos = source->pos.x_pos;
	view.y_pos = source->pos.y_pos - wep->gun_height;
	view.z_pos = source->pos.z_pos;
	view.x_rot = short(wep->shot_accuracy * (GetRandomControl() - 0x4000) / 0x10000 + angles[1]);
	view.y_rot = short(wep->shot_accuracy * (GetRandomControl() - 0x4000) / 0x10000 + angles[0]);
	view.z_rot = 0;
	phd_GenerateW2V(&view);

	nSpheres = GetSpheres(target, spheres, 0);
	best = -1;
	bestdist = 0x7FFFFFFF;

	for (int i = 0; i < nSpheres; i++)
	{
		sphere = &spheres[i];
		r = sphere->r;

		if (ABS(sphere->x) < r && ABS(sphere->y) < r && sphere->z > r && SQUARE(sphere->x) + SQUARE(sphere->y) <= SQUARE(r))
		{
			if (sphere->z - r < bestdist)
			{
				bestdist = sphere->z - r;
				best = i;
			}
		}
	}

	lara.has_fired = 1;
	savegame.ammo_used++;
	start.x = view.x_pos;
	start.y = view.y_pos;
	start.z = view.z_pos;
	room_number = source->room_number;
	GetFloor(view.x_pos, view.y_pos, view.z_pos, &room_number);
	start.room_number = room_number;

	if (best < 0)
	{
		dest.x = start.x + ((wep->target_dist * phd_mxptr[M20]) >> W2V_SHIFT);
		dest.y = start.y + ((wep->target_dist * phd_mxptr[M21]) >> W2V_SHIFT);
		dest.z = start.z + ((wep->target_dist * phd_mxptr[M22]) >> W2V_SHIFT);
		los = LOS(&start, &dest);
		objLos = ObjectOnLOS(&start, &dest);

		if (objLos == NO_ITEM)
		{
			if (!los)
				Richochet(&dest);
		}
		else if (items[objLos].object_number == SMASH_OBJECT1 && CurrentLevel == LV_CRASH)
			Richochet(&dest);
		else
			SmashItem((short)objLos, weapon_type);

		return -1;
	}

	savegame.ammo_hit++;
	dest.x = start.x + ((bestdist * phd_mxptr[M20]) >> W2V_SHIFT);
	dest.y = start.y + ((bestdist * phd_mxptr[M21]) >> W2V_SHIFT);
	dest.z = start.z + ((bestdist * phd_mxptr[M22]) >> W2V_SHIFT);
	objLos = ObjectOnLOS(&start, &dest);

	if (objLos != NO_ITEM && (items[objLos].object_number != SMASH_OBJECT1 || CurrentLevel != LV_CRASH))
		SmashItem((short)objLos, weapon_type);

	obj_num = target->object_number;

	if (obj_num != SHIVA && obj_num != ARMY_WINSTON && obj_num != LON_BOSS && (obj_num != TRIBEBOSS || !TribeBossShieldOn))
		HitTarget(target, &dest, wep->damage);
	else if (obj_num == TRIBEBOSS)
		FindClosestShieldPoint(dest.x - ((dest.x - start.x) >> 5), dest.y - ((dest.y - start.y) >> 5), dest.z - ((dest.z - start.z) >> 5), target);
	else if (obj_num == ARMY_WINSTON || obj_num == LON_BOSS)
	{
		target->hit_status = 1;
		target->hit_points--;
		angle = (short)mGetAngle(lara_item->pos.z_pos, lara_item->pos.x_pos, target->pos.z_pos, target->pos.x_pos);
		TriggerRicochetSpark(&dest, (angle >> 4) & 0xFFF);
		SoundEffect(SFX_LARA_RICOCHET, &target->pos, SFX_DEFAULT);
	}
	else
	{
		dx = target->pos.x_pos - lara_item->pos.x_pos;
		dz = target->pos.z_pos - lara_item->pos.z_pos;
		angle = short(0x8000 - target->pos.y_rot + phd_atan(dz, dx));

		if (target->current_anim_state > 1 && target->current_anim_state < 5 && angle < 0x4000 && angle > -0x4000)
		{
			target->hit_status = 1;
			angle = (short)mGetAngle(lara_item->pos.z_pos, lara_item->pos.x_pos, target->pos.z_pos, target->pos.x_pos);
			TriggerRicochetSpark(&dest, (angle >> 4) & 0xFFF);
		}
		else
			HitTarget(target, &dest, wep->damage);
	}

	return 1;
}

void inject_larafire(bool replace)
{
	INJECT(0x0044AF50, WeaponObject, replace);
	INJECT(0x0044A890, FireWeapon, replace);
}
