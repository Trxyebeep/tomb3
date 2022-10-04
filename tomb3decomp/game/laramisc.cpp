#include "../tomb3/pch.h"
#include "laramisc.h"
#include "lara.h"
#include "laraclmb.h"
#include "larasurf.h"
#include "laraswim.h"
#include "laraanim.h"
#include "invfunc.h"
#include "objects.h"
#include "control.h"
#include "effects.h"
#include "collide.h"
#include "sound.h"
#include "../specific/specific.h"
#include "../3dsystem/phd_math.h"
#include "gameflow.h"
#include "../specific/smain.h"

void(*extra_control_routines[13])(ITEM_INFO* item, COLL_INFO* coll) =
{
	extra_as_breath,
	extra_void_func,
	extra_void_func,
	extra_as_sharkkill,
	extra_as_airlock,
	extra_as_gongbong,
	extra_as_dinokill,
	extra_void_func,
	extra_as_startanim,
	extra_void_func,
	extra_void_func,
	extra_as_trainkill,
	extra_as_rapidsdrown
};

void(*lara_control_routines[89])(ITEM_INFO* item, COLL_INFO* coll) =
{
	lara_as_walk,
	lara_as_run,
	lara_as_stop,
	lara_as_forwardjump,
	lara_void_func,
	lara_as_fastback,
	lara_as_turn_r,
	lara_as_turn_l,
	lara_as_death,
	lara_as_fastfall,
	lara_as_hang,
	lara_as_reach,
	lara_as_splat,
	lara_as_tread,
	lara_void_func,
	lara_as_compress,
	lara_as_back,
	lara_as_swim,
	lara_as_glide,
	lara_as_gymnast,
	lara_as_fastturn,
	lara_as_stepright,
	lara_as_stepleft,
	lara_void_func,
	lara_as_slide,
	lara_as_backjump,
	lara_as_rightjump,
	lara_as_leftjump,
	lara_as_upjump,
	lara_as_fallback,
	lara_as_hangleft,
	lara_as_hangright,
	lara_as_slideback,
	lara_as_surftread,
	lara_as_surfswim,
	lara_as_dive,
	lara_as_pushblock,
	lara_as_pushblock,
	lara_as_ppready,
	lara_as_pickup,
	lara_as_switchon,
	lara_as_switchon,
	lara_as_usekey,
	lara_as_usekey,
	lara_as_uwdeath,
	lara_void_func,
	lara_as_special,
	lara_as_surfback,
	lara_as_surfleft,
	lara_as_surfright,
	lara_void_func,
	lara_void_func,
	lara_as_swandive,
	lara_as_fastdive,
	lara_as_gymnast,
	lara_as_waterout,
	lara_as_climbstnc,
	lara_as_climbing,
	lara_as_climbleft,
	lara_as_climbend,
	lara_as_climbright,
	lara_as_climbdown,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_as_wade,
	lara_as_waterroll,
	lara_as_pickupflare,
	lara_void_func,
	lara_void_func,
	lara_as_deathslide,
	lara_as_duck,
#ifdef TROYESTUFF
	lara_as_duckroll,
#else
	lara_as_duck,
#endif
	lara_as_dash,
	lara_as_dashdive,
	lara_as_hang2,
	lara_as_monkeyswing,
	lara_as_monkeyl,
	lara_as_monkeyr,
	lara_as_monkey180,
	lara_as_all4s,
	lara_as_crawl,
	lara_as_hangturnl,
	lara_as_hangturnr,
	lara_as_all4turnl,
	lara_as_all4turnr,
	lara_as_crawlb,
	lara_void_func,
	lara_void_func
};

void(*lara_collision_routines[89])(ITEM_INFO* item, COLL_INFO* coll) =
{
	lara_col_walk,
	lara_col_run,
	lara_col_stop,
	lara_col_forwardjump,
	lara_col_land,
	lara_col_fastback,
	lara_col_turn_r,
	lara_col_turn_l,
	lara_col_death,
	lara_col_fastfall,
	lara_col_hang,
	lara_col_reach,
	lara_col_splat,
	lara_col_tread,
	lara_col_land,
	lara_col_compress,
	lara_col_back,
	lara_col_swim,
	lara_col_glide,
	lara_col_pushblock,
	lara_col_land,
	lara_col_stepright,
	lara_col_stepleft,
	lara_col_roll2,
	lara_col_slide,
	lara_col_backjump,
	lara_col_rightjump,
	lara_col_leftjump,
	lara_col_upjump,
	lara_col_fallback,
	lara_col_hangleft,
	lara_col_hangright,
	lara_col_slideback,
	lara_col_surftread,
	lara_col_surfswim,
	lara_col_dive,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_uwdeath,
	lara_col_roll,
	lara_void_func,
	lara_col_surfback,
	lara_col_surfleft,
	lara_col_surfright,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_swandive,
	lara_col_fastdive,
	lara_col_pushblock,
	lara_col_pushblock,
	lara_col_climbstnc,
	lara_col_climbing,
	lara_col_climbleft,
	lara_void_func,
	lara_col_climbright,
	lara_col_climbdown,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_col_wade,
	lara_col_waterroll,
	lara_col_pushblock,
	lara_void_func,
	lara_void_func,
	lara_void_func,
	lara_col_duck,
#ifdef TROYESTUFF
	lara_col_duckroll,
#else
	lara_col_duck,
#endif
	lara_col_dash,
	lara_col_dashdive,
	lara_col_hang2,
	lara_col_monkeyswing,
	lara_col_monkeyl,
	lara_col_monkeyr,
	lara_col_monkey180,
	lara_col_all4s,
	lara_col_crawl,
	lara_col_hangturnlr,
	lara_col_hangturnlr,
	lara_col_all4turnl,
	lara_col_all4turnl,
	lara_col_crawlb,
	lara_void_func,
	lara_col_crawl2hang
};

void LaraCheatGetStuff()
{
#ifdef TROYESTUFF
	if (objects[M16_ITEM].loaded)
		Inv_AddItem(M16_ITEM);

	if (objects[SHOTGUN_ITEM].loaded)
		Inv_AddItem(SHOTGUN_ITEM);

	if (objects[UZI_ITEM].loaded)
		Inv_AddItem(UZI_ITEM);

	if (objects[MAGNUM_ITEM].loaded)
		Inv_AddItem(MAGNUM_ITEM);

	if (objects[GUN_ITEM].loaded)
		Inv_AddItem(GUN_ITEM);

	if (objects[ROCKET_GUN_ITEM].loaded)
		Inv_AddItem(ROCKET_GUN_ITEM);

	if (objects[GRENADE_GUN_ITEM].loaded)
		Inv_AddItem(GRENADE_GUN_ITEM);

	if (objects[HARPOON_ITEM].loaded)
		Inv_AddItem(HARPOON_ITEM);

	for (int i = KEY_ITEM1; i <= KEY_ITEM4; i++)
	{
		if (objects[i].loaded && !Inv_RequestItem(i))
			Inv_AddItem(i);
	}

	for (int i = PUZZLE_ITEM1; i <= PUZZLE_ITEM4; i++)
	{
		if (objects[i].loaded && !Inv_RequestItem(i))
			Inv_AddItem(i);
	}

	for (int i = PICKUP_ITEM1; i <= PICKUP_ITEM2; i++)
	{
		if (objects[i].loaded && !Inv_RequestItem(i))
			Inv_AddItem(i);
	}

	for (int i = 0; i < 1; i++)
	{
		if (objects[FLAREBOX_ITEM].loaded)
			Inv_AddItem(FLAREBOX_ITEM);

		if (objects[MEDI_ITEM].loaded)
			Inv_AddItem(MEDI_ITEM);

		if (objects[BIGMEDI_ITEM].loaded)
			Inv_AddItem(BIGMEDI_ITEM);
	}

	lara.magnums.ammo = 1000;
	lara.uzis.ammo = 1000;
	lara.shotgun.ammo = 1000 * 6;
	lara.harpoon.ammo = 1000;
	lara.rocket.ammo = 1000;
	lara.grenade.ammo = 1000;
	lara.m16.ammo = 1000;
#else
	Inv_AddItem(M16_ITEM);
	Inv_AddItem(SHOTGUN_ITEM);
	Inv_AddItem(UZI_ITEM);
	Inv_AddItem(MAGNUM_ITEM);
	Inv_AddItem(GUN_ITEM);
	Inv_AddItem(ROCKET_GUN_ITEM);
	Inv_AddItem(GRENADE_GUN_ITEM);

	for (int i = 0; i < 1; i++)
	{
		Inv_AddItem(FLAREBOX_ITEM);
		Inv_AddItem(MEDI_ITEM);
		Inv_AddItem(BIGMEDI_ITEM);
	}

	lara.magnums.ammo = 250;
	lara.uzis.ammo = 1000;
	lara.shotgun.ammo = 300;
	lara.m16.ammo = 300;
	lara.grenade.ammo = 1000;
#endif
}

void LaraCheatyBits()
{
#ifdef TROYESTUFF
	uchar s;

	if (key_pressed(DIK_F1))
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

	if (key_pressed(DIK_F2))
	{
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LARA_CHEAT)
		{
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->current_anim_state = AS_SWIM;
			lara_item->goal_anim_state = AS_SWIM;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 0;
			lara_item->fallspeed = 0;
			lara.water_status = LARA_CHEAT;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
		}
	}

	if (key_pressed(DIK_F3))
		FinishLevelCheat = 1;

	if (key_pressed(DIK_F4))
	{
		s = 0;

		for (int i = 0; i < LevelSecrets[CurrentLevel]; i++)	//assume no secret triggers have bigger IDs than the total
			s |= 1 << i;

		if (savegame.secrets != s)
		{
			S_CDPlay(122, 0);
			savegame.secrets = s;
		}
	}
#else
	if (!gameflow.cheat_enable && !gameflow.dozy_cheat_enabled)
		return;

	if (input & IN_D)
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

	if (input & IN_C)
	{
		lara_item->pos.y_pos -= 128;

		if (lara.water_status != LARA_CHEAT)
		{
			lara_item->anim_number = ANIM_SWIMCHEAT;
			lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
			lara_item->current_anim_state = AS_SWIM;
			lara_item->goal_anim_state = AS_SWIM;
			lara.water_status = LARA_CHEAT;
			lara_item->gravity_status = 0;
			lara_item->pos.x_rot = 5460;
			lara_item->fallspeed = 30;
			lara.air = 1800;
			lara.death_count = 0;
			lara.torso_x_rot = 0;
			lara.torso_y_rot = 0;
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
			cheat_hit_points = lara_item->hit_points;
		}
	}
#endif
}

void LaraCheat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_item->hit_points = 1000;
	LaraUnderWater(item, coll);

	if (input & IN_WALK && !(input & IN_LOOK))
	{
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		lara.water_status = LARA_ABOVEWATER;
		item->pos.x_rot = 0;
		item->pos.z_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.gun_status = LG_ARMLESS;
#ifndef TROYESTUFF
		LaraInitialiseMeshes(1);
		lara.mesh_effects = 0;
		lara_item->hit_points = cheat_hit_points;
#endif
	}
}

void LaraInitialiseMeshes(long level_number)
{
	START_INFO* start;

	start = &savegame.start[level_number];

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + i] = meshes[objects[LARA_SKIN].mesh_index + i];

	if (start->gun_type != LG_UNARMED)
	{
		if (start->gun_type == LG_MAGNUMS)
		{
			lara.mesh_ptrs[THIGH_L] = meshes[objects[MAGNUM].mesh_index + THIGH_L];
			lara.mesh_ptrs[THIGH_R] = meshes[objects[MAGNUM].mesh_index + THIGH_R];
		}
		else if (start->gun_type == LG_UZIS)
		{
			lara.mesh_ptrs[THIGH_L] = meshes[objects[UZI].mesh_index + THIGH_L];
			lara.mesh_ptrs[THIGH_R] = meshes[objects[UZI].mesh_index + THIGH_R];
		}
		else
		{
			lara.mesh_ptrs[THIGH_L] = meshes[objects[PISTOLS].mesh_index + THIGH_L];
			lara.mesh_ptrs[THIGH_R] = meshes[objects[PISTOLS].mesh_index + THIGH_R];
		}
	}

	if (start->gun_type == LG_FLARE)
		lara.mesh_ptrs[HAND_L] = meshes[objects[FLARE].mesh_index + HAND_L];

	if (start->gun_type == LG_M16)
		lara.back_gun = M16;
	else if (start->gun_type == LG_HARPOON)
		lara.back_gun = HARPOON_GUN;
	else if (start->gun_type == LG_ROCKET)
		lara.back_gun = ROCKET_GUN;
	else if (start->gun_type == LG_GRENADE)
		lara.back_gun = GRENADE_GUN;
	else if(start->got_shotgun)
		lara.back_gun = SHOTGUN;
	else if (start->got_m16)
		lara.back_gun = M16;
	else if (start->got_rocket)
		lara.back_gun = ROCKET_GUN;
	else if (start->got_grenade)
		lara.back_gun = GRENADE_GUN;
	else if (start->got_harpoon)
		lara.back_gun = HARPOON_GUN;
}

void LaraControl(short item_number)
{
	long room_water_state, wd, wh, hfw, dx, dy, dz;

#ifndef TROYESTUFF
	if (gameflow.cheat_enable || gameflow.dozy_cheat_enabled)
#endif
		LaraCheatyBits();

	lara.last_pos.x = lara_item->pos.x_pos;
	lara.last_pos.y = lara_item->pos.y_pos;
	lara.last_pos.z = lara_item->pos.z_pos;

	if (lara.gun_status == 1 && lara_item->current_anim_state == AS_STOP && lara_item->goal_anim_state == AS_STOP &&
		lara_item->anim_number == ANIM_BREATH && !lara.extra_anim && !lara_item->gravity_status)
		lara.gun_status = LG_ARMLESS;

	if (lara_item->current_anim_state != AS_DASH && DashTimer < 120)
		DashTimer++;

	lara.IsDucked = 0;
	room_water_state = room[lara_item->room_number].flags & (ROOM_UNDERWATER | ROOM_SWAMP);
	wd = GetWaterDepth(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, lara_item->room_number);
	wh = GetWaterHeight(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, lara_item->room_number);

	if (wh == NO_HEIGHT)
		hfw = NO_HEIGHT;
	else
		hfw = lara_item->pos.y_pos - wh;

	lara.water_surface_dist = -hfw;

	if (lara.skidoo == NO_ITEM)
	{
		WadeSplash(lara_item, wh, wd);

		if (lara.skidoo == NO_ITEM && !lara.extra_anim)
		{
			switch (lara.water_status)
			{
			case LARA_ABOVEWATER:

				if (hfw == NO_HEIGHT || hfw < 256)
					break;

				if (wd > 474 && !(room_water_state & ROOM_SWAMP))
				{
					if (room_water_state)
					{
						lara.air = 1800;
						lara.water_status = LARA_UNDERWATER;
						lara_item->gravity_status = 0;
						lara_item->pos.y_pos += 100;
						UpdateLaraRoom(lara_item, 0);
						StopSoundEffect(SFX_LARA_FALL);

						if (lara_item->current_anim_state == AS_SWANDIVE)
						{
							lara_item->pos.x_rot = -8190;
							lara_item->goal_anim_state = AS_DIVE;
							AnimateLara(lara_item);
							lara_item->fallspeed <<= 1;
						}
						else if (lara_item->current_anim_state == AS_FASTDIVE)
						{
							lara_item->pos.x_rot = -15470;
							lara_item->goal_anim_state = AS_DIVE;
							AnimateLara(lara_item);
							lara_item->fallspeed <<= 1;
						}
						else
						{
							lara_item->pos.x_rot = -8190;
							lara_item->anim_number = 112;
							lara_item->frame_number = anims[112].frame_base;
							lara_item->current_anim_state = AS_DIVE;
							lara_item->goal_anim_state = AS_SWIM;
							lara_item->fallspeed = 3 * lara_item->fallspeed / 2;
						}

						lara.torso_x_rot = 0;
						lara.torso_y_rot = 0;
						lara.head_x_rot = 0;
						lara.head_y_rot = 0;
						Splash(lara_item);
					}
				}
				else if (hfw > 256)
				{
					lara.water_status = LARA_WADE;

					if (!lara_item->gravity_status)
						lara_item->goal_anim_state = AS_STOP;
					else if (room_water_state & ROOM_SWAMP)
					{
						if (lara_item->current_anim_state == AS_SWANDIVE || lara_item->current_anim_state == AS_FASTDIVE)
							lara_item->pos.y_pos = wh + 1000;

						lara_item->anim_number = ANIM_WADE;
						lara_item->frame_number = anims[ANIM_WADE].frame_base;
						lara_item->current_anim_state = AS_WADE;
						lara_item->goal_anim_state = AS_WADE;
					}
				}

				break;

			case LARA_UNDERWATER:

				if (!room_water_state)
				{
					if (wd != NO_HEIGHT && ABS(hfw) < 256)
					{
						lara_item->anim_number = 114;
						lara_item->frame_number = anims[114].frame_base;
						lara_item->current_anim_state = AS_SURFTREAD;
						lara_item->goal_anim_state = AS_SURFTREAD;
						lara.water_status = LARA_SURFACE;
						lara_item->pos.y_pos += 1 - hfw;
						lara_item->fallspeed = 0;
						lara.dive_count = 11;
						lara_item->pos.z_rot = 0;
						lara_item->pos.x_rot = 0;
						lara.torso_y_rot = 0;
						lara.torso_x_rot = 0;
						lara.head_y_rot = 0;
						lara.head_x_rot = 0;
						UpdateLaraRoom(lara_item, -381);
						SoundEffect(SFX_LARA_BREATH, &lara_item->pos, SFX_ALWAYS);
					}
					else
					{
						lara_item->anim_number = ANIM_FALLDOWN;
						lara_item->frame_number = anims[ANIM_FALLDOWN].frame_base;
						lara_item->current_anim_state = AS_FORWARDJUMP;
						lara_item->goal_anim_state = AS_FORWARDJUMP;
						lara.water_status = LARA_ABOVEWATER;
						lara_item->speed = lara_item->fallspeed / 4;
						lara_item->fallspeed = 0;
						lara_item->gravity_status = 1;
						lara_item->pos.x_rot = 0;
						lara_item->pos.z_rot = 0;
						lara.torso_x_rot = 0;
						lara.torso_y_rot = 0;
						lara.head_x_rot = 0;
						lara.head_y_rot = 0;
					}
				}

				break;

			case LARA_SURFACE:

				if (!room_water_state)
				{
					if (hfw <= 256)
					{
						lara.water_status = LARA_ABOVEWATER;
						lara_item->anim_number = ANIM_FALLDOWN;
						lara_item->frame_number = anims[ANIM_FALLDOWN].frame_base;
						lara_item->current_anim_state = AS_FORWARDJUMP;
						lara_item->goal_anim_state = AS_FORWARDJUMP;
						lara_item->gravity_status = 1;
						lara_item->speed = lara_item->fallspeed / 4;
					}
					else
					{
						lara.water_status = LARA_WADE;
						lara_item->anim_number = ANIM_BREATH;
						lara_item->frame_number = anims[ANIM_BREATH].frame_base;
						lara_item->current_anim_state = AS_STOP;
						lara_item->goal_anim_state = AS_WADE;
						AnimateItem(lara_item);
					}

					lara_item->fallspeed = 0;
					lara_item->pos.x_rot = 0;
					lara_item->pos.z_rot = 0;
					lara.torso_x_rot = 0;
					lara.torso_y_rot = 0;
					lara.head_x_rot = 0;
					lara.head_y_rot = 0;
				}

				break;

			case LARA_WADE:

				camera.target_elevation = -4004;

				if (hfw < 256)
				{
					lara.water_status = 0;

					if (lara_item->current_anim_state == AS_WADE)
						lara_item->goal_anim_state = AS_RUN;
				}
				else if (hfw > 730 && !(room_water_state & ROOM_SWAMP))
				{
					lara.water_status = LARA_SURFACE;
					lara_item->pos.y_pos += 1 - hfw;

					if (lara_item->current_anim_state == AS_BACK)
					{
						lara_item->anim_number = 140;
						lara_item->frame_number = anims[140].frame_base;
						lara_item->current_anim_state = AS_SURFBACK;
						lara_item->goal_anim_state = AS_SURFBACK;
					}
					else if (lara_item->current_anim_state == AS_STEPRIGHT)
					{
						lara_item->anim_number = 144;
						lara_item->frame_number = anims[144].frame_base;
						lara_item->current_anim_state = AS_SURFRIGHT;
						lara_item->goal_anim_state = AS_SURFRIGHT;
					}
					else if (lara_item->current_anim_state == AS_STEPLEFT)
					{
						lara_item->anim_number = 143;
						lara_item->frame_number = anims[143].frame_base;
						lara_item->current_anim_state = AS_SURFLEFT;
						lara_item->goal_anim_state = AS_SURFLEFT;
					}
					else
					{
						lara_item->anim_number = 116;
						lara_item->frame_number = anims[116].frame_base;
						lara_item->current_anim_state = AS_SURFSWIM;
						lara_item->goal_anim_state = AS_SURFSWIM;
					}

					lara_item->gravity_status = 0;
					lara_item->fallspeed = 0;
					lara.dive_count = 0;
					lara_item->pos.x_rot = 0;
					lara_item->pos.z_rot = 0;
					lara.torso_x_rot = 0;
					lara.torso_y_rot = 0;
					lara.head_x_rot = 0;
					lara.head_y_rot = 0;
					UpdateLaraRoom(lara_item, -381);
				}

				break;
			}
		}
	}

	//S_SetReverbType call here

	if (lara_item->hit_points > 0)
	{
		if (GF_NoFloor && lara_item->pos.y_pos >= GF_NoFloor)
		{
			lara_item->hit_points = -1;
			lara.death_count = 270;
		}
	}
	else
	{
		lara_item->hit_points = -1;

		if (!lara.death_count)
			S_CDStop();

		lara.death_count++;

		if (lara_item->flags & 0x100)
		{
			lara.death_count++;
			return;
		}
	}

	switch (lara.water_status)
	{
	case LARA_ABOVEWATER:
	case LARA_WADE:

		if (!(room[lara_item->room_number].flags & ROOM_SWAMP) || lara.water_surface_dist >= -775)
		{
			if (lara.skidoo == NO_ITEM && items[NO_ITEM].object_number != UPV)	//?!?!?!?!?!
				lara.air = 1800;
		}
		else if (lara_item->hit_points >= 0)
		{
			lara.air -= 6;

			if (lara.air < 0)
			{
				lara.air = -1;
				lara_item->hit_points -= 10;
			}
		}

		LaraAboveWater(lara_item, mycoll);

#ifdef RANDO_STUFF
		if (rando.levels[RANDOLEVEL].freezingWater)
#else
		if (CurrentLevel == LV_ANTARC || CurrentLevel == LV_MINES)
#endif
		{
			if (lara.water_status == LARA_WADE)
				ExposureMeter--;
			else 
			{
				ExposureMeter++;

				if (ExposureMeter > 600)
					ExposureMeter = 600;
			}
		}

		break;

	case LARA_UNDERWATER:

		if (lara_item->hit_points >= 0)
		{
			lara.air--;

			if (lara.air < 0)
			{
				lara.air = -1;
				lara_item->hit_points -= 5;
			}
		}

		LaraUnderWater(lara_item, mycoll);

#ifdef RANDO_STUFF
		if (rando.levels[RANDOLEVEL].freezingWater)
#else
		if (CurrentLevel == LV_ANTARC || CurrentLevel == LV_MINES)
#endif
			ExposureMeter -= 2;

		break;

	case LARA_SURFACE:

		if (lara_item->hit_points >= 0)
		{
			lara.air += 10;

			if (lara.air > 1800)
				lara.air = 1800;
		}

		LaraSurface(lara_item, mycoll);

#ifdef RANDO_STUFF
		if (rando.levels[RANDOLEVEL].freezingWater)
#else
		if (CurrentLevel == LV_ANTARC || CurrentLevel == LV_MINES)
#endif
			ExposureMeter--;

		break;

	case LARA_CHEAT:

		if (gameflow.cheat_enable || gameflow.dozy_cheat_enabled)
			LaraCheat(lara_item, mycoll);

		break;
	}

	if (ExposureMeter < 0)
	{
		ExposureMeter = -1;
		lara_item->hit_points -= 10;
	}

	dx = lara_item->pos.x_pos - lara.last_pos.x;
	dy = lara_item->pos.y_pos - lara.last_pos.y;
	dz = lara_item->pos.z_pos - lara.last_pos.z;
	savegame.distance_travelled += phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
}

void inject_laramisc(bool replace)
{
	INJECT(0x0044C630, LaraCheatGetStuff, replace);
	INJECT(0x0044D1A0, LaraCheatyBits, replace);
	INJECT(0x0044CFC0, LaraCheat, replace);
	INJECT(0x0044D060, LaraInitialiseMeshes, replace);
	INJECT(0x0044C6F0, LaraControl, inject_rando ? 1 : replace);
}
