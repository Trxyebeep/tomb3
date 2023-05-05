#include "../tomb3/pch.h"
#include "laramisc.h"
#include "lara.h"
#include "laraclmb.h"
#include "larasurf.h"
#include "laraswim.h"
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
#include "items.h"
#include "larafire.h"
#include "../specific/file.h"
#include "camera.h"
#include "savegame.h"
#include "cinema.h"
#include "../specific/picture.h"
#include "effect2.h"
#include "../specific/input.h"
#include "../tomb3/tomb3.h"

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
	lara_as_duckroll,
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
	lara_col_duckroll,
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

static COLL_INFO lara_coll;
static COLL_INFO* mycoll = &lara_coll;
static short cheat_hit_points;

void LaraCheatGetStuff()
{
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

		if (objects[SAVEGAME_CRYSTAL_ITEM].loaded)
			Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);
	}

	lara.magnums.ammo = 1000;
	lara.uzis.ammo = 1000;
	lara.shotgun.ammo = 1000 * 6;
	lara.harpoon.ammo = 1000;
	lara.rocket.ammo = 1000;
	lara.grenade.ammo = 1000;
	lara.m16.ammo = 1000;
}

void LaraCheatyBits()
{
#ifdef _DEBUG
	uchar s;

	if (key_pressed(DIK_F1))
	{
		LaraCheatGetStuff();
		lara_item->hit_points = 1000;
	}

	if (key_pressed(DIK_F2))
#else
	if (tomb3.dozy && key_pressed(DIK_D) && key_pressed(DIK_O) && key_pressed(DIK_Z) && key_pressed(DIK_Y))
#endif
	{
		if (lara.skidoo == NO_ITEM)
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
	}

#ifdef _DEBUG
	if (key_pressed(DIK_F3) && !pictureFading)
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
#endif
}

void LaraCheat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_item->hit_points = 1000;
	LaraUnderWater(item, coll);

	if (input & IN_ACTION)
		TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 31, 255, 255, 255);

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

	LaraCheatyBits();
	lara.last_pos.x = lara_item->pos.x_pos;
	lara.last_pos.y = lara_item->pos.y_pos;
	lara.last_pos.z = lara_item->pos.z_pos;

	if (lara.gun_status == LG_HANDSBUSY && lara_item->current_anim_state == AS_STOP && lara_item->goal_anim_state == AS_STOP &&
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

	if (lara.skidoo == NO_ITEM && !lara.extra_anim)
	{
		WadeSplash(lara_item, wh, wd);

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
				if (wd != NO_HEIGHT && abs(hfw) < 256)
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

			if (hfw <= 256)
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

		if (lara_item->flags & IFL_INVISIBLE)
		{
			lara.death_count++;
			return;
		}
	}

	switch (lara.water_status)
	{
	case LARA_ABOVEWATER:
	case LARA_WADE:

		if (room[lara_item->room_number].flags & ROOM_SWAMP && lara.water_surface_dist < -775)
		{
			if (lara_item->hit_points >= 0)
			{
				lara.air -= 6;

				if (lara.air < 0)
				{
					lara.air = -1;
					lara_item->hit_points -= 10;
				}
			}
		}
		else if ((lara.skidoo == NO_ITEM || items[lara.skidoo].object_number != UPV) && lara.air < 1800 && lara_item->hit_points >= 0)
		{
			lara.air += 10;

			if (lara.air > 1800)
				lara.air = 1800;
		}

		LaraAboveWater(lara_item, mycoll);

		if (GF_Cold)
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

		if (GF_Cold)
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

		if (GF_Cold)
			ExposureMeter--;

		break;

	case LARA_CHEAT:

		if (GF_Cold)
			ExposureMeter = 600;

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

void AnimateLara(ITEM_INFO* item)
{
	ANIM_STRUCT* anim;
	short* cmd;
	long speed;
	ushort type;

	item->frame_number++;
	anim = &anims[item->anim_number];

	if (anim->number_changes > 0 && GetChange(item, anim))
	{
		anim = &anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;
	}

	if (item->frame_number > anim->frame_end)
	{
		if (anim->number_commands)
		{
			cmd = &commands[anim->command_index];

			for (int i = anim->number_commands; i > 0; i--)
			{
				switch (*cmd++)
				{
				case COMMAND_MOVE_ORIGIN:
					TranslateItem(item, cmd[0], cmd[1], cmd[2]);
					cmd += 3;
					break;

				case COMMAND_JUMP_VELOCITY:
					item->fallspeed = cmd[0];
					item->speed = cmd[1];
					item->gravity_status = 1;

					if (lara.calc_fallspeed)
					{
						item->fallspeed = lara.calc_fallspeed;
						lara.calc_fallspeed = 0;
					}

					cmd += 2;
					break;

				case COMMAND_ATTACK_READY:

					if (lara.gun_status != LG_SPECIAL)
						lara.gun_status = LG_ARMLESS;

					break;

				case COMMAND_SOUND_FX:
				case COMMAND_EFFECT:
					cmd += 2;
					break;
				}
			}
		}

		item->anim_number = anim->jump_anim_num;
		item->frame_number = anim->jump_frame_num;
		anim = &anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;
	}

	if (anim->number_commands > 0)
	{
		cmd = &commands[anim->command_index];

		for (int i = anim->number_commands; i > 0; i--)
		{
			switch (*cmd++)
			{
			case COMMAND_MOVE_ORIGIN:
				cmd += 3;
				break;

			case COMMAND_JUMP_VELOCITY:
				cmd += 2;
				break;

			case COMMAND_SOUND_FX:

				if (item->frame_number == cmd[0])
				{
					type = cmd[1] & 0xC000;

					if (type == SFX_LANDANDWATER ||
						(type == SFX_LANDONLY && (lara.water_surface_dist >= 0 || lara.water_surface_dist == NO_HEIGHT)) ||
						(type == SFX_WATERONLY && lara.water_surface_dist < 0 &&
							lara.water_surface_dist != NO_HEIGHT && !(room[item->room_number].flags & ROOM_SWAMP)))
						SoundEffect(cmd[1] & 0x3FFF, &item->pos, SFX_ALWAYS);
				}

				cmd += 2;
				break;

			case COMMAND_EFFECT:

				if (item->frame_number == cmd[0])
				{
					FXType = cmd[1] & 0xC000;
					effect_routines[cmd[1] & 0x3FFF](item);
				}

				cmd += 2;
				break;
			}
		}
	}

	if (!item->gravity_status)
	{
		if (lara.water_status == LARA_WADE && room[item->room_number].flags & ROOM_SWAMP)
		{
			speed = anim->velocity >> 1;

			if (anim->acceleration)
				speed += (anim->acceleration * (item->frame_number - anim->frame_base)) >> 2;
		}
		else
		{
			speed = anim->velocity;

			if (anim->acceleration)
				speed += anim->acceleration * (item->frame_number - anim->frame_base);
		}

		item->speed = speed >> 16;
	}
	else
	{
		if (room[item->room_number].flags & ROOM_SWAMP)
		{
			item->speed -= item->speed >> 3;

			if (abs(item->speed) < 8)
			{
				item->speed = 0;
				item->gravity_status = 0;
			}

			if (item->fallspeed > 128)
				item->fallspeed >>= 1;

			item->fallspeed -= item->fallspeed >> 2;

			if (item->fallspeed < 4)
				item->fallspeed = 4;
		}
		else
		{
			speed = anim->velocity + anim->acceleration * (item->frame_number - anim->frame_base - 1);
			item->speed -= speed >> 16;
			speed += anim->acceleration;
			item->speed += speed >> 16;
			item->fallspeed += item->fallspeed < 128 ? 6 : 1;
		}

		item->pos.y_pos += item->fallspeed;
	}

	item->pos.x_pos += (item->speed * phd_sin(lara.move_angle)) >> W2V_SHIFT;
	item->pos.z_pos += (item->speed * phd_cos(lara.move_angle)) >> W2V_SHIFT;
}

void UseItem(short object_number)
{
	if (object_number < 0 || object_number >= NUMBER_OBJECTS)
		return;

	switch (object_number)
	{
	case GUN_ITEM:
	case GUN_OPTION:
		lara.request_gun_type = LG_PISTOLS;
		break;

	case SHOTGUN_ITEM:
	case SHOTGUN_OPTION:
		lara.request_gun_type = LG_SHOTGUN;
		break;

	case MAGNUM_ITEM:
	case MAGNUM_OPTION:
		lara.request_gun_type = LG_MAGNUMS;
		break;

	case UZI_ITEM:
	case UZI_OPTION:
		lara.request_gun_type = LG_UZIS;
		break;

	case HARPOON_ITEM:
	case HARPOON_OPTION:
		lara.request_gun_type = LG_HARPOON;
		break;

	case M16_ITEM:
	case M16_OPTION:
		lara.request_gun_type = LG_M16;
		break;

	case ROCKET_GUN_ITEM:
	case ROCKET_OPTION:
		lara.request_gun_type = LG_ROCKET;
		break;

	case GRENADE_GUN_ITEM:
	case GRENADE_OPTION:
		lara.request_gun_type = LG_GRENADE;
		break;

	case MEDI_ITEM:
	case MEDI_OPTION:

		if (lara_item->hit_points > 0 && lara_item->hit_points < 1000 || lara.poisoned)
		{
			lara.poisoned = 0;
			lara_item->hit_points += 500;

			if (lara_item->hit_points > 1000)
				lara_item->hit_points = 1000;

			Inv_RemoveItem(MEDI_ITEM);
			SoundEffect(SFX_MENU_MEDI, 0, 2);
			savegame.health_used++;
		}

		break;

	case BIGMEDI_ITEM:
	case BIGMEDI_OPTION:

		if (lara_item->hit_points > 0 && lara_item->hit_points < 1000 || lara.poisoned)
		{
			lara.poisoned = 0;
			lara_item->hit_points += 1000;

			if (lara_item->hit_points > 1000)
				lara_item->hit_points = 1000;

			Inv_RemoveItem(BIGMEDI_ITEM);
			SoundEffect(SFX_MENU_MEDI, 0, 2);
			savegame.health_used += 2;
		}

		break;

	case FLAREBOX_ITEM:
	case FLAREBOX_OPTION:
		lara.request_gun_type = LG_FLARE;
		break;
	}
}

void ControlLaraExtra(short item_number)
{
	AnimateItem(&items[item_number]);
}

void InitialiseLaraLoad(short item_number)
{
	lara.item_number = item_number;
	lara_item = &items[item_number];
}

void InitialiseLara(long type)
{
	ITEM_INFO* item;
	short* tmp;

	item = lara_item;
	item->collidable = 0;
	item->data = &lara;
	item->hit_points = 1000;

	lara.hit_direction = -1;
	lara.skidoo = NO_ITEM;
	lara.weapon_item = NO_ITEM;
	lara.flare_control_left = 0;
	lara.flare_control_right = 0;
	lara.extra_anim = 0;
	lara.look = 1;
	lara.burn = 0;
	lara.BurnGreen = 0;
	lara.calc_fallspeed = 0;
	lara.climb_status = 0;
	lara.pose_count = 0;
	lara.hit_frame = 0;
	lara.air = 1800;
	lara.dive_count = 0;
	lara.death_count = 0;
	lara.current_active = 0;
	lara.spaz_effect_count = 0;
	lara.flare_age = 0;
	lara.back_gun = 0;
	lara.flare_frame = 0;
	lara.water_surface_dist = 100;
	lara.last_pos.x = item->pos.x_pos;
	lara.last_pos.y = item->pos.y_pos;
	lara.last_pos.z = item->pos.z_pos;
	lara.spaz_effect = 0;
	lara.mesh_effects = 0;
	lara.target = 0;
	lara.turn_rate = 0;
	lara.move_angle = 0;
	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.head_z_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.torso_z_rot = 0;
	lara.right_arm.flash_gun = 0;
	lara.left_arm.flash_gun = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.poisoned = 0;
	lara.creature = 0;
	lara.electric = 0;

	if (type == 1 && GF_LaraStartAnim)
	{
		lara.gun_status = LG_HANDSBUSY;
		lara.water_status = LARA_ABOVEWATER;
		item->anim_number = objects[LARA_EXTRA].anim_index;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 0;
		item->goal_anim_state = (short)GF_LaraStartAnim;
		AnimateLara(item);
		lara.extra_anim = 1;
		camera.type = CINEMATIC_CAMERA;
		cine_frame = 0;
		cinematic_pos = item->pos;
	}
	else if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		lara.water_status = LARA_UNDERWATER;
		item->fallspeed = 0;
		item->anim_number = ANIM_TREAD;
		item->frame_number = anims[ANIM_TREAD].frame_base;
		item->current_anim_state = AS_TREAD;
		item->goal_anim_state = AS_TREAD;
	}
	else
	{
		lara.water_status = LARA_ABOVEWATER;
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->current_anim_state = AS_STOP;
		item->goal_anim_state = AS_STOP;
	}

	if (type == 4)
	{
		for (int i = 0; i < 15; i++)
		{
			meshes[objects[LARA].mesh_index + i] = meshes[objects[LARA_SKIN].mesh_index + i];
			lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + i];
		}

		tmp = lara.mesh_ptrs[THIGH_L];
		lara.mesh_ptrs[THIGH_L] = meshes[objects[PISTOLS].mesh_index + THIGH_L];
		meshes[objects[PISTOLS].mesh_index + THIGH_L] = tmp;

		tmp = lara.mesh_ptrs[THIGH_R];
		lara.mesh_ptrs[THIGH_R] = meshes[objects[PISTOLS].mesh_index + THIGH_R];
		meshes[objects[PISTOLS].mesh_index + THIGH_R] = tmp;

		lara.gun_status = LG_ARMLESS;
	}
	else
		InitialiseLaraInventory(CurrentLevel);

	DashTimer = 120;
	ExposureMeter = 600;
}

void InitialiseLaraInventory(long level)
{
	START_INFO* start;
	long ammo;

	Inv_RemoveAllItems();
	start = &savegame.start[level];

	if (GF_RemoveWeapons)
	{
		start->got_pistols = 0;
		start->got_magnums = 0;
		start->got_uzis = 0;
		start->got_shotgun = 0;
		start->got_m16 = 0;
		start->got_rocket = 0;
		start->got_grenade = 0;
		start->got_harpoon = 0;
		start->gun_type = LG_UNARMED;
		start->gun_status = LG_ARMLESS;
		GF_RemoveWeapons = 0;
	}

	if (GF_RemoveAmmo)
	{
		start->pistol_ammo = 0;
		start->magnum_ammo = 0;
		start->uzi_ammo = 0;
		start->shotgun_ammo = 0;
		start->m16_ammo = 0;
		start->rocket_ammo = 0;
		start->harpoon_ammo = 0;
		start->grenade_ammo = 0;
		start->num_flares = 0;
		start->num_big_medis = 0;
		start->num_medis = 1;
		GF_RemoveAmmo = 0;
	}

	Inv_AddItem(MAP_CLOSED);
	lara.pistols.ammo = 1000;

	if (start->got_pistols)
		Inv_AddItem(GUN_ITEM);

	if (start->got_magnums)
	{
		Inv_AddItem(MAGNUM_ITEM);
		lara.magnums.ammo = start->magnum_ammo;
		GlobalItemReplace(MAGNUM_ITEM, MAG_AMMO_ITEM);
	}
	else
	{
		ammo = start->magnum_ammo / 10;

		for (int i = 0; i < ammo; i++)
			Inv_AddItem(MAG_AMMO_ITEM);

		lara.magnums.ammo = 0;
	}

	if (start->got_uzis)
	{
		Inv_AddItem(UZI_ITEM);
		lara.uzis.ammo = start->uzi_ammo;
		GlobalItemReplace(UZI_ITEM, UZI_AMMO_ITEM);
	}
	else
	{
		ammo = start->uzi_ammo / 40;

		for (int i = 0; i < ammo; i++)
			Inv_AddItem(UZI_AMMO_ITEM);

		lara.uzis.ammo = 0;
	}

	if (start->got_shotgun)
	{
		Inv_AddItem(SHOTGUN_ITEM);
		lara.shotgun.ammo = start->shotgun_ammo;
		GlobalItemReplace(SHOTGUN_ITEM, SG_AMMO_ITEM);
	}
	else
	{
		ammo = start->shotgun_ammo / 12;

		for (int i = 0; i < ammo; i++)
			Inv_AddItem(SG_AMMO_ITEM);

		lara.shotgun.ammo = 0;
	}

	if (start->got_rocket)
	{
		Inv_AddItem(ROCKET_GUN_ITEM);
		lara.rocket.ammo = start->rocket_ammo;
		GlobalItemReplace(ROCKET_GUN_ITEM, ROCKET_AMMO_ITEM);
	}
	else
	{
		ammo = start->rocket_ammo;

		for (int i = 0; i < ammo; i++)
			Inv_AddItem(ROCKET_AMMO_ITEM);

		lara.rocket.ammo = 0;
	}

	if (start->got_grenade)
	{
		Inv_AddItem(GRENADE_GUN_ITEM);
		lara.grenade.ammo = start->grenade_ammo;
		GlobalItemReplace(GRENADE_GUN_ITEM, GRENADE_AMMO_ITEM);
	}
	else
	{
		ammo = start->grenade_ammo / 2;

		for (int i = 0; i < ammo; i++)
			Inv_AddItem(GRENADE_AMMO_ITEM);

		lara.grenade.ammo = 0;
	}

	if (start->got_m16)
	{
		Inv_AddItem(M16_ITEM);
		lara.m16.ammo = start->m16_ammo;
		GlobalItemReplace(M16_ITEM, M16_AMMO_ITEM);
	}
	else
	{
		ammo = start->m16_ammo / 60;

		for (int i = 0; i < ammo; i++)
			Inv_AddItem(M16_AMMO_ITEM);

		lara.m16.ammo = 0;
	}

	if (start->got_harpoon)
	{
		Inv_AddItem(HARPOON_ITEM);
		lara.harpoon.ammo = start->harpoon_ammo;
		GlobalItemReplace(HARPOON_ITEM, HARPOON_AMMO_ITEM);
	}
	else
	{
		lara.harpoon.ammo = 0;
		ammo = start->harpoon_ammo / 3;

		for (int i = 0; i < ammo; i++)
		{
			if (!i)
				Inv_AddItem(HARPOON_AMMO_ITEM);

			lara.harpoon.ammo += 3;
		}
	}

	for (int i = 0; i < start->num_flares; i++)
		Inv_AddItem(FLARE_ITEM);

	for (int i = 0; i < start->num_medis; i++)
		Inv_AddItem(MEDI_ITEM);

	for (int i = 0; i < start->num_big_medis; i++)
		Inv_AddItem(BIGMEDI_ITEM);

	if (!tomb3.gold)
	{
		for (int i = 0; i < start->num_sgcrystals; i++)
			Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);
	}

	if (start->num_icon1)
		Inv_AddItem(ICON_PICKUP1_ITEM);

	if (start->num_icon2)
		Inv_AddItem(ICON_PICKUP2_ITEM);

	if (start->num_icon3)
		Inv_AddItem(ICON_PICKUP3_ITEM);

	if (start->num_icon4)
		Inv_AddItem(ICON_PICKUP4_ITEM);

	lara.gun_status = LG_ARMLESS;
	lara.last_gun_type = start->gun_type;
	lara.gun_type = lara.last_gun_type;
	lara.request_gun_type = lara.last_gun_type;
	LaraInitialiseMeshes(level);
	InitialiseNewWeapon();
}
