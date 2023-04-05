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
#include "../specific/smain.h"
#include "items.h"
#include "draw.h"
#include "lara2gun.h"
#include "lara1gun.h"
#include "laraflar.h"
#include "lara.h"
#include "invfunc.h"
#include "../specific/input.h"
#include "camera.h"
#include "savegame.h"

WEAPON_INFO weapons[11] =
{
	{
		0, 0, 0, 0,							//LG_UNARMED
		0, 0, 0, 0,
		0, 0, 0, 0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	},

	{
		-10920, 10920, -10920, 10920,		//LG_PISTOLS
		-30940, 10920, -14560, 14560,
		-10920, 30940, -14560, 14560,
		1820,
		1456,
		650,
		8192,
		1,
		9,
		3,
		0,
		SFX_LARA_FIRE
	},

	{
		-10920, 10920, -10920, 10920,		//LG_MAGNUMS
		-1820, 1820, -14560, 14560,
		0, 0, 0, 0,
		1820,
		728,
		650,
		8192,
		21,
		16,
		3,
		0,
		SFX_DESSERT_EAGLE_FIRE
	},

	{
		-10920, 10920, -10920, 10920,		//LG_UZIS
		-30940, 10920, -14560, 14560,
		-10920, 30940, -14560, 14560,
		1820,
		1456,
		650,
		8192,
		1,
		3,
		3,
		0,
		SFX_LARA_UZI_FIRE
	},

	{
		-10920, 10920, -10010, 10010,		//LG_SHOTGUN
		-14560, 14560, -11830, 11830,
		-14560, 14560, -11830, 11830,
		1820,
		0,
		500,
		8192,
		3,
		9,
		3,
		10,
		SFX_LARA_SHOTGUN
	},

	{
		-10920, 10920, -10010, 10010,		//LG_M16
		-14560, 14560, -11830, 11830,
		-14560, 14560, -11830, 11830,
		1820,
		1456,
		500,
		12288,
		4,
		0,
		3,
		16,
		0
	},

	{
		-10920, 10920, -10010, 10010,		//LG_ROCKET
		-14560, 14560, -11830, 11830,
		-14560, 14560, -11830, 11830,
		1820,
		1456,
		500,
		8192,
		30,
		0,
		2,
		12,
		SFX_BAZOOKA_FIRE
	},

	{
		-10920, 10920, -10010, 10010,		//LG_GRENADE
		-14560, 14560, -11830, 11830,
		-14560, 14560, -11830, 11830,
		1820,
		1456,
		500,
		8192,
		20,
		0,
		2,
		10,
		0
	},

	{
		-10920, 10920, -11830, 11830,		//LG_HARPOON
		-3640, 3640, -13650, 13650,
		-14560, 14560, -13650, 13650,
		1820,
		1456,
		500,
		8192,
		6,
		0,
		2,
		10,
		0
	},

	{
		0, 0, 0, 0,							//LG_FLARE
		0, 0, 0, 0,
		0, 0, 0, 0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	},

	{
		-5460, 5460, -10010, 10010,			//LG_SKIDOO
		-5460, 5460, -10010, 10010,
		-5460, 5460, -10010, 10010,
		1820,
		1456,
		400,
		8192,
		3,
		0,
		2,
		0,
		SFX_LARA_UZI_FIRE
	},
};

static short HoldStates[] =
{
	AS_WALK,
	AS_STOP,
	AS_POSE,
	AS_TURN_R,
	AS_TURN_L,
	AS_BACK,
	AS_FASTTURN,
	AS_STEPLEFT,
	AS_STEPRIGHT,
	AS_WADE,
	AS_PICKUP,
	AS_SWITCHON,
	AS_SWITCHOFF,
	AS_DUCK,
	-1
};

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
			ammo->ammo = 10000;
	}
	else if (weapon_type == LG_UZIS)
	{
		ammo = &lara.uzis;

		if (savegame.bonus_flag)
			ammo->ammo = 10000;
	}
	else if (weapon_type == LG_SHOTGUN)
	{
		ammo = &lara.shotgun;

		if (savegame.bonus_flag)
			ammo->ammo = 10000;
	}
	else if (weapon_type == LG_M16)
	{
		ammo = &lara.m16;

		if (savegame.bonus_flag)
			ammo->ammo = 10000;
	}
	else
	{
		ammo = &lara.pistols;
		ammo->ammo = 10000;
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

		if (abs(sphere->x) < r && abs(sphere->y) < r && sphere->z > r && SQUARE(sphere->x) + SQUARE(sphere->y) <= SQUARE(r))
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
		TriggerRicochetSpark(&dest, (angle >> 4) & 0xFFF, 16);
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
			TriggerRicochetSpark(&dest, (angle >> 4) & 0xFFF, 16);
		}
		else
			HitTarget(target, &dest, wep->damage);
	}

	return 1;
}

void HitTarget(ITEM_INFO* item, GAME_VECTOR* hitpos, long damage)
{
	if (item->hit_points > 0 && item->hit_points <= damage)
		savegame.kills++;

	item->hit_points -= (short)damage;
	item->hit_status = 1;

	if (item->data)
		((CREATURE_INFO*)item->data)->hurt_by_lara = 1;

	if (hitpos && item->object_number != TRIBEBOSS)
	{
		if (item->object_number == ROBOT_SENTRY_GUN)
		{
			for (int i = 0; i < 3; i++)
				TriggerRicochetSpark(hitpos, GetRandomControl() & 0xFFF, 0);

			SoundEffect(SFX_LARA_RICOCHET, &item->pos, SFX_DEFAULT);
		}
		else if (item->object_number != TARGETS)
			DoBloodSplat(hitpos->x, hitpos->y, hitpos->z, item->speed, item->pos.y_rot, item->room_number);
	}
}

void SmashItem(short item_number, long weapon_type)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number >= SMASH_WINDOW && item->object_number <= SMASH_OBJECT3)
		return SmashWindow(item_number);

	if (item->object_number == CARCASS || item->object_number == EXTRAFX6)
	{
		if (item->status != ITEM_ACTIVE)
		{
			item->status = ITEM_ACTIVE;
			AddActiveItem(item_number);
		}
	}
}

void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm)
{
	short speed, x, y;

	speed = winfo->aim_speed;

	if (arm->lock)
	{
		y = lara.target_angles[0];
		x = lara.target_angles[1];
	}
	else
	{
		x = 0;
		y = 0;
	}

	if (arm->y_rot >= y - speed && arm->y_rot <= speed + y)
		arm->y_rot = y;
	else if (arm->y_rot < y)
		arm->y_rot += speed;
	else
		arm->y_rot -= speed;

	if (arm->x_rot >= x - speed && arm->x_rot <= speed + x)
		arm->x_rot = x;
	else if (arm->x_rot < x)
		arm->x_rot += speed;
	else
		arm->x_rot -= speed;

	arm->z_rot = 0;
}

void find_target_point(ITEM_INFO* item, GAME_VECTOR* target)
{
	long x, y, z, c, s;
	short* bounds;

	bounds = GetBestFrame(item);
	x = (bounds[0] + bounds[1]) >> 1;
	y = bounds[2] + (bounds[3] - bounds[2]) / 3;
	z = (bounds[4] + bounds[5]) >> 1;
	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	target->x = item->pos.x_pos + ((z * s + x * c) >> W2V_SHIFT);
	target->y = item->pos.y_pos + y;
	target->z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);
	target->room_number = item->room_number;
}

void LaraTargetInfo(WEAPON_INFO* winfo)
{
	GAME_VECTOR start, target;
	short ang[2];

	if (!lara.target)
	{
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
		lara.target_angles[0] = 0;
		lara.target_angles[1] = 0;
		return;
	}

	start.x = lara_item->pos.x_pos;
	start.y = lara_item->pos.y_pos - 650;
	start.z = lara_item->pos.z_pos;
	start.room_number = lara_item->room_number;
	find_target_point(lara.target, &target);
	phd_GetVectorAngles(target.x - start.x, target.y - start.y, target.z - start.z, ang);
	ang[0] -= lara_item->pos.y_rot;
	ang[1] -= lara_item->pos.x_rot;

	if (LOS(&start, &target))
	{
		if (ang[0] >= winfo->lock_angles[0] && ang[0] <= winfo->lock_angles[1] &&
			ang[1] >= winfo->lock_angles[2] && ang[1] <= winfo->lock_angles[3])
		{
			lara.left_arm.lock = 1;
			lara.right_arm.lock = 1;
			lara.target_angles[0] = ang[0];
			lara.target_angles[1] = ang[1];
			return;
		}

		if (lara.left_arm.lock)
		{
			if (ang[0] < winfo->left_angles[0] || ang[0] > winfo->left_angles[1] ||
				ang[1] < winfo->left_angles[2] || ang[1] > winfo->left_angles[3])
				lara.left_arm.lock = 0;
		}

		if (lara.right_arm.lock)
		{
			if (ang[0] < winfo->right_angles[0] || ang[0] > winfo->right_angles[1] ||
				ang[1] < winfo->left_angles[2] || ang[1] > winfo->left_angles[3])
				lara.right_arm.lock = 0;
		}
	}
	else
	{
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
	}

	lara.target_angles[0] = ang[0];
	lara.target_angles[1] = ang[1];
}

void LaraGetNewTarget(WEAPON_INFO* winfo)
{
	ITEM_INFO* item;
	ITEM_INFO* bestitem;
	GAME_VECTOR start, target;
	long x, y, z, dist, maxdist, maxdist2, bestdist;
	short ang[2];
	short bestyrot, item_number;

	item = 0;
	bestitem = 0;
	start.x = lara_item->pos.x_pos;
	start.y = lara_item->pos.y_pos - 650;
	start.z = lara_item->pos.z_pos;
	start.room_number = lara_item->room_number;
	bestyrot = 0x7FFF;
	bestdist = 0x7FFFFFFF;
	maxdist = winfo->target_dist;
	maxdist2 = SQUARE(maxdist);

	for (item_number = next_item_active; item_number != NO_ITEM; item_number = item->next_active)
	{
		if (item_number == NO_ITEM || item_number == lara.item_number)
			continue;

		item = &items[item_number];

		if (item->hit_points <= 0 || !objects[item->object_number].intelligent)
			continue;

		x = item->pos.x_pos - start.x;
		y = item->pos.y_pos - start.y;
		z = item->pos.z_pos - start.z;

		if (abs(x) > maxdist || abs(y) > maxdist || abs(z) > maxdist)
			continue;

		dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

		if (dist >= maxdist2)
			continue;

		find_target_point(item, &target);

		if (!LOS(&start, &target))
			continue;

		phd_GetVectorAngles(target.x - start.x, target.y - start.y, target.z - start.z, ang);
		ang[0] -= lara.torso_y_rot + lara_item->pos.y_rot;
		ang[1] -= lara.torso_x_rot + lara_item->pos.x_rot;

		if (ang[0] >= winfo->lock_angles[0] && ang[0] <= winfo->lock_angles[1] && ang[1] >= winfo->lock_angles[2] && ang[1] <= winfo->lock_angles[3])
		{
			if (abs(ang[0]) < bestyrot + 2730 && dist < bestdist)
			{
				bestdist = dist;
				bestyrot = abs(ang[0]);
				bestitem = item;
			}
		}
	}

	lara.target = bestitem;
	LaraTargetInfo(winfo);
}

void InitialiseNewWeapon()
{
	lara.right_arm.frame_number = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.lock = 0;
	lara.right_arm.flash_gun = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.left_arm.lock = 0;
	lara.left_arm.flash_gun = 0;

	lara.target = 0;
	
	switch (lara.gun_type)
	{
	case LG_PISTOLS:
	case LG_UZIS:
		lara.right_arm.frame_base = objects[PISTOLS].frame_base;
		lara.left_arm.frame_base = objects[PISTOLS].frame_base;

		if (lara.gun_status != LG_ARMLESS)
			draw_pistol_meshes(lara.gun_type);

		break;

	case LG_MAGNUMS:
	case LG_SHOTGUN:
	case LG_M16:
	case LG_ROCKET:
	case LG_GRENADE:
	case LG_HARPOON:
		lara.right_arm.frame_base = objects[WeaponObject(lara.gun_type)].frame_base;
		lara.left_arm.frame_base = lara.right_arm.frame_base;

		if (lara.gun_status != LG_ARMLESS)
			draw_shotgun_meshes(lara.gun_type);

		break;

	case LG_FLARE:
		lara.right_arm.frame_base = objects[FLARE].frame_base;
		lara.left_arm.frame_base = objects[FLARE].frame_base;

		if (lara.gun_status != LG_ARMLESS)
			draw_flare_meshes();

		break;

	default:
		lara.right_arm.frame_base = anims[lara_item->anim_number].frame_ptr;
		lara.left_arm.frame_base = lara.right_arm.frame_base;
		break;
	}
}

static long CheckForHoldingState(long state)
{
	short* holds;

	if (lara.extra_anim)
		return 0;

	holds = HoldStates;

	while (*holds >= 0)
	{
		if (state == *holds)
			return 1;

		holds++;
	}

	return 0;
}

void LaraGun()
{
	AMMO_INFO* ammo;

	if (lara.left_arm.flash_gun > 0)
		lara.left_arm.flash_gun--;

	if (lara.right_arm.flash_gun > 0)
		lara.right_arm.flash_gun--;

	if (lara_item->hit_points <= 0)
		lara.gun_status = LG_ARMLESS;
	else if (lara.gun_status == LG_ARMLESS)
	{
		if (input & IN_DRAW)
			lara.request_gun_type = lara.last_gun_type;
		else if (input & IN_FLARE)
		{
			if (lara.gun_type == LG_FLARE)
				lara.gun_status = LG_UNDRAW;
			else if (Inv_RequestItem(FLAREBOX_ITEM))
				lara.request_gun_type = LG_FLARE;
		}

		if (lara.request_gun_type != lara.gun_type || input & IN_DRAW)
		{
			if (lara_item->current_anim_state == AS_DUCK && (lara.request_gun_type == LG_SHOTGUN || lara.request_gun_type == LG_M16 ||
				lara.request_gun_type == LG_ROCKET || lara.request_gun_type == LG_GRENADE || lara.request_gun_type == LG_HARPOON))
				return;

			if (lara.request_gun_type == LG_FLARE || lara.skidoo == NO_ITEM && (lara.request_gun_type == LG_HARPOON ||
				lara.water_status == LARA_ABOVEWATER || lara.water_status == LARA_WADE && lara.water_surface_dist > -weapons[lara.gun_type].gun_height))
			{
				if (lara.gun_type == LG_FLARE)
				{
					CreateFlare(0);
					undraw_flare_meshes();
					lara.flare_control_left = 0;
				}

				lara.gun_type = lara.request_gun_type;
				InitialiseNewWeapon();
				lara.gun_status = LG_DRAW;
				lara.right_arm.frame_number = 0;
				lara.left_arm.frame_number = 0;
			}
			else
			{
				lara.last_gun_type = lara.request_gun_type;

				if (lara.gun_type == LG_FLARE)
					lara.request_gun_type = LG_FLARE;
				else
					lara.gun_type = lara.request_gun_type;
			}
		}
	}
	else if (lara.gun_status == LG_READY)
	{
		if (input & IN_FLARE && Inv_RequestItem(FLAREBOX_ITEM))
			lara.request_gun_type = LG_FLARE;

		if ((input & IN_DRAW || lara.request_gun_type != lara.gun_type) || (lara.gun_type != LG_HARPOON && lara.water_status != LARA_ABOVEWATER &&
			(lara.water_status != LARA_WADE || lara.water_surface_dist < -weapons[lara.gun_type].gun_height)))
			lara.gun_status = LG_UNDRAW;
	}

	switch (lara.gun_status)
	{
	case LG_ARMLESS:

		if (lara.gun_type == LG_FLARE)
		{
			if (lara.skidoo != NO_ITEM || CheckForHoldingState(lara_item->current_anim_state))
			{
				if (!lara.flare_control_left)
				{
					lara.flare_control_left = 1;
					lara.left_arm.frame_number = 95;
					
				}
				else if (lara.left_arm.frame_number)
				{
					lara.left_arm.frame_number++;

					if (lara.left_arm.frame_number == 110)
						lara.left_arm.frame_number = 0;
				}
			}
			else
				lara.flare_control_left = 0;

			DoFlareInHand(lara.flare_age);
			set_flare_arm(lara.left_arm.frame_number);
		}

		break;

	case LG_HANDSBUSY:

		if (lara.gun_type == LG_FLARE)
		{
			lara.flare_control_left = lara.skidoo != NO_ITEM || CheckForHoldingState(lara_item->current_anim_state);
			DoFlareInHand(lara.flare_age);
			set_flare_arm(lara.left_arm.frame_number);
		}

		break;

	case LG_DRAW:

		if (lara.gun_type != LG_FLARE && lara.gun_type != LG_ARMLESS)
			lara.last_gun_type = lara.gun_type;

		switch (lara.gun_type)
		{
		case LG_PISTOLS:
		case LG_MAGNUMS:
		case LG_UZIS:

			if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA)
				camera.type = COMBAT_CAMERA;

			draw_pistols(lara.gun_type);
			break;

		case LG_SHOTGUN:
		case LG_M16:
		case LG_ROCKET:
		case LG_GRENADE:
		case LG_HARPOON:

			if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA)
				camera.type = COMBAT_CAMERA;

			draw_shotgun(lara.gun_type);
			break;

		case LG_FLARE:
			draw_flare();
			break;

		default:
			lara.gun_status = LG_ARMLESS;
			break;
		}

		break;

	case LG_UNDRAW:
		lara.mesh_ptrs[HEAD] = meshes[objects[LARA].mesh_index + HEAD];

		switch (lara.gun_type)
		{
		case LG_PISTOLS:
		case LG_MAGNUMS:
		case LG_UZIS:
			undraw_pistols(lara.gun_type);
			break;

		case LG_SHOTGUN:
		case LG_M16:
		case LG_ROCKET:
		case LG_GRENADE:
		case LG_HARPOON:
			undraw_shotgun(lara.gun_type);
			break;

		case LG_FLARE:
			undraw_flare();
			break;
		}

		break;

	case LG_READY:

		if (lara.pistols.ammo && input & IN_ACTION)
			lara.mesh_ptrs[HEAD] = meshes[objects[UZI].mesh_index + HEAD];
		else
			lara.mesh_ptrs[HEAD] = meshes[objects[LARA].mesh_index + HEAD];

		if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA)
			camera.type = COMBAT_CAMERA;

		if (input & IN_ACTION)
		{
			switch (lara.gun_type)
			{
			case LG_MAGNUMS:
				ammo = &lara.magnums;
				break;

			case LG_UZIS:
				ammo = &lara.uzis;
				break;

			case LG_SHOTGUN:
				ammo = &lara.shotgun;
				break;

			case LG_M16:
				ammo = &lara.m16;
				break;

			case LG_ROCKET:
				ammo = &lara.rocket;
				break;

			case LG_GRENADE:
				ammo = &lara.grenade;
				break;

			case LG_HARPOON:
				ammo = &lara.harpoon;
				break;

			default:
				ammo = &lara.pistols;
				break;
			}

			if (ammo->ammo <= 0)
			{
				ammo->ammo = 0;
				SoundEffect(SFX_LARA_EMPTY, &lara_item->pos, SFX_DEFAULT);

				if (Inv_RequestItem(GUN_ITEM))
					lara.request_gun_type = LG_PISTOLS;
				else
					lara.request_gun_type = LG_ARMLESS;

				break;
			}
		}

		switch (lara.gun_type)
		{
		case LG_PISTOLS:
		case LG_UZIS:
			PistolHandler(lara.gun_type);
			break;

		case LG_MAGNUMS:
		case LG_SHOTGUN:
		case LG_M16:
		case LG_ROCKET:
		case LG_GRENADE:
		case LG_HARPOON:
			RifleHandler(lara.gun_type);
			break;
		}

		break;

	case LG_SPECIAL:
		draw_flare();
		break;
	}
}

void DoProperDetection(short item_number, long x, long y, long z, long xv, long yv, long zv)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long ceiling, height, oldtype, oldonobj, oldheight, bs, yang, xs;
	short room_number;

	item = &items[item_number];

	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	oldheight = GetHeight(floor, x, y, z);
	oldtype = height_type;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	height = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos >= height)
	{
		bs = 0;

		if ((oldtype == BIG_SLOPE || oldtype == DIAGONAL) && oldheight < height)
		{
			yang = (ushort)item->pos.y_rot;

			if (tiltyoff < 0)
			{
				if (yang >= 0x8000)
					bs = 1;
			}
			else if (tiltyoff > 0)
			{
				if (yang <= 0x8000)
					bs = 1;
			}

			if (tiltxoff < 0)
			{
				if (yang >= 0x4000 && yang <= 0xC000)
					bs = 1;
			}
			else if (tiltxoff > 0)
			{
				if (yang <= 0x4000 || yang >= 0xC000)
					bs = 1;
			}
		}

		if (y > height + 32 && !bs && ((item->pos.x_pos ^ x) & ~WALL_MASK || (item->pos.z_pos ^ z) & ~WALL_MASK))
		{
			xs = (item->pos.x_pos ^ x) & ~WALL_MASK && (item->pos.z_pos ^ z) & ~WALL_MASK ? abs(x - item->pos.x_pos) < abs(z - item->pos.z_pos) : 1;
			item->pos.y_rot = (item->pos.x_pos ^ x) & ~WALL_MASK && xs ? -item->pos.y_rot : -0x8000 - item->pos.y_rot;
			item->pos.x_pos = x;
			item->pos.y_pos = y;
			item->pos.z_pos = z;
			item->speed >>= 1;
		}
		else if (oldtype != BIG_SLOPE && oldtype != DIAGONAL)
		{
			if (item->fallspeed > 0)
			{
				if (item->fallspeed > 16)
				{
					if (item->object_number == GRENADE)
						item->fallspeed = (item->fallspeed >> 1) - item->fallspeed;
					else
					{
						item->fallspeed = -(item->fallspeed >> 2);

						if (item->fallspeed < -100)
							item->fallspeed = -100;
					}
				}
				else
				{
					item->fallspeed = 0;

					if (item->object_number == GRENADE)
					{
						item->speed--;
						item->required_anim_state = 1;
						item->pos.x_rot = 0;
					}
					else
						item->speed -= 3;

					if (item->speed < 0)
						item->speed = 0;
				}
			}

			item->pos.y_pos = height;
		}
		else
		{
			item->speed -= item->speed >> 2;

			if (tiltyoff < 0 && abs(tiltyoff) - abs(tiltxoff) >= 2)
			{
				if ((ushort)item->pos.y_rot > 0x8000)
				{
					item->pos.y_rot = -1 - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed -= short(tiltyoff << 1);

						if ((ushort)item->pos.y_rot > 0x4000 && (ushort)item->pos.y_rot < 0xC000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0x4000)
								item->pos.y_rot = 0x4000;
						}
						else if ((ushort)item->pos.y_rot < 0x4000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0x4000)
								item->pos.y_rot = 0x4000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltyoff > 0 && abs(tiltyoff) - abs(tiltxoff) >= 2)
			{
				if ((ushort)item->pos.y_rot < 0x8000)
				{
					item->pos.y_rot = -1 - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += short(tiltyoff << 1);

						if ((ushort)item->pos.y_rot > 0xC000 || (ushort)item->pos.y_rot < 0x4000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0xC000)
								item->pos.y_rot = -0x4000;
						}
						else if ((ushort)item->pos.y_rot < 0xC000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0xC000)
								item->pos.y_rot = -0x4000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltxoff < 0 && abs(tiltxoff) - abs(tiltyoff) >= 2)
			{
				if ((ushort)item->pos.y_rot > 0x4000 && (ushort)item->pos.y_rot < 0xC000)
				{
					item->pos.y_rot = 0x7FFF - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed -= short(tiltxoff << 1);

						if ((ushort)item->pos.y_rot < 0x8000)
						{
							item->pos.y_rot -= 0x4000;

							if ((ushort)item->pos.y_rot > 61440)
								item->pos.y_rot = 0;
						}
						else
						{
							item->pos.y_rot += 0x4000;

							if ((ushort)item->pos.y_rot < 0x4000)
								item->pos.y_rot = 0;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltxoff > 0 && abs(tiltxoff) - abs(tiltyoff) >= 2)
			{
				if ((ushort)item->pos.y_rot > 0xC000 || (ushort)item->pos.y_rot < 0x4000)
				{
					item->pos.y_rot = 0x7FFF - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += short(tiltxoff << 1);

						if ((ushort)item->pos.y_rot > 0x8000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0x8000)
								item->pos.y_rot = -0x8000;
						}
						else if ((ushort)item->pos.y_rot < 0x8000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0x8000)
								item->pos.y_rot = -0x8000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltyoff < 0 && tiltxoff < 0)
			{
				if ((ushort)item->pos.y_rot > 0x6000 && (ushort)item->pos.y_rot < 0xE000)
				{
					item->pos.y_rot = -0x4001 - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed -= short(tiltxoff + tiltyoff);

						if ((ushort)item->pos.y_rot > 0x2000 && (ushort)item->pos.y_rot < 0xA000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0x2000)
								item->pos.y_rot = 0x2000;
						}
						else if (item->pos.y_rot != 0x2000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0x2000)
								item->pos.y_rot = 0x2000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltyoff < 0 && tiltxoff > 0)
			{
				if ((ushort)item->pos.y_rot > 0xA000 || (ushort)item->pos.y_rot < 0x2000)
				{
					item->pos.y_rot = 0x3FFF - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += short(tiltxoff - tiltyoff);

						if ((ushort)item->pos.y_rot < 0xE000 && (ushort)item->pos.y_rot > 0x6000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0x6000)
								item->pos.y_rot = 0x6000;
						}
						else if (item->pos.y_rot != 0x6000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0x6000)
								item->pos.y_rot = 0x6000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltyoff > 0 && tiltxoff > 0)
			{
				if ((ushort)item->pos.y_rot > 0xE000 || (ushort)item->pos.y_rot < 0x6000)
				{
					item->pos.y_rot = -0x4001 - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += short(tiltxoff + tiltyoff);

						if ((ushort)item->pos.y_rot < 0x2000 || (ushort)item->pos.y_rot > 0xA000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0xA000)
								item->pos.y_rot = -0x6000;
						}
						else if ((ushort)item->pos.y_rot != 0xA000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0xA000)
								item->pos.y_rot = -0x6000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}
			else if (tiltyoff > 0 && tiltxoff < 0)
			{
				if ((ushort)item->pos.y_rot > 0x2000 && (ushort)item->pos.y_rot < 0xA000)
				{
					item->pos.y_rot = 0x3FFF - item->pos.y_rot;

					if (item->fallspeed > 0)
						item->fallspeed = -(item->fallspeed >> 1);
				}
				else
				{
					if (item->speed < 32)
					{
						item->speed += short(tiltyoff - tiltxoff);

						if ((ushort)item->pos.y_rot < 0x6000 || (ushort)item->pos.y_rot > 0xE000)
						{
							item->pos.y_rot -= 0x1000;

							if ((ushort)item->pos.y_rot < 0xE000)
								item->pos.y_rot = -0x2000;
						}
						else if ((ushort)item->pos.y_rot != 0xE000)
						{
							item->pos.y_rot += 0x1000;

							if ((ushort)item->pos.y_rot > 0xE000)
								item->pos.y_rot = -0x2000;
						}
					}

					item->fallspeed = item->fallspeed > 0 ? -(item->fallspeed >> 1) : 0;
				}
			}

			item->pos.x_pos = x;
			item->pos.y_pos = y;
			item->pos.z_pos = z;
		}
	}
	else
	{
		if (yv >= 0)
		{
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
			height = GetHeight(floor, item->pos.x_pos, y, item->pos.z_pos);

			oldonobj = OnObject;

			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (item->pos.y_pos >= height && oldonobj)
			{
				if (item->fallspeed > 0)
				{
					if (item->fallspeed > 16)
					{
						if (item->object_number == GRENADE)
							item->fallspeed = (item->fallspeed >> 1) - item->fallspeed;
						else
						{
							item->fallspeed = -(item->fallspeed >> 2);

							if (item->fallspeed < -100)
								item->fallspeed = -100;
						}
					}
					else
					{
						item->fallspeed = 0;

						if (item->object_number == GRENADE)
						{
							item->speed--;
							item->required_anim_state = 1;
							item->pos.x_rot = 0;
						}
						else
							item->speed -= 3;

						if (item->speed < 0)
							item->speed = 0;
					}
				}

				item->pos.y_pos = height;
			}
		}

		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		ceiling = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (item->pos.y_pos < ceiling)
		{
			if (y < ceiling && ((item->pos.x_pos ^ x) & ~WALL_MASK || (item->pos.z_pos ^ z) & ~WALL_MASK))
			{
				item->pos.y_rot = (item->pos.x_pos ^ x) & ~WALL_MASK ? -item->pos.y_rot : -0x8000 - item->pos.y_rot;

				if (item->object_number == GRENADE)
					item->speed -= item->speed >> 3;
				else
					item->speed >>= 1;

				item->pos.x_pos = x;
				item->pos.y_pos = y;
				item->pos.z_pos = z;
			}
			else
				item->pos.y_pos = ceiling;

			if (item->fallspeed < 0)
				item->fallspeed = -item->fallspeed;
		}
	}

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}
