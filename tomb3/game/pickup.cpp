#include "../tomb3/pch.h"
#include "pickup.h"
#include "collide.h"
#include "objects.h"
#include "laramisc.h"
#include "larafire.h"
#include "laraflar.h"
#include "items.h"
#include "health.h"
#include "invfunc.h"
#include "gameflow.h"
#include "../specific/smain.h"
#include "effect2.h"
#include "sound.h"
#include "lara.h"
#ifdef TROYESTUFF
#include "../tomb3/tomb3.h"
#endif

static short PickUpBounds[12] = { -256, 256, -100, 100, -256, 256, -1820, 1820, 0, 0, 0, 0 };
static short PickUpBoundsUW[12] = { -512, 512, -512, 512, -512, 512, -8190, 8190, -8190, 8190, -8190, 8190 };
static PHD_VECTOR PickUpPosition = { 0, 0, -100 };
static PHD_VECTOR PickUpPositionUW = { 0, -200, -350 };

void PickUpCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short rotx, roty, rotz;

	item = &items[item_num];
	rotx = item->pos.x_rot;
	roty = item->pos.y_rot;
	rotz = item->pos.z_rot;
	item->pos.y_rot = l->pos.y_rot;
	item->pos.z_rot = 0;

	if (lara.water_status == LARA_ABOVEWATER || lara.water_status == LARA_WADE)
	{
		item->pos.x_rot = 0;

		if (TestLaraPosition(PickUpBounds, item, l))
		{
			if (l->current_anim_state == AS_PICKUP)
			{
				if (l->frame_number == anims[ANIM_PICKUP].frame_base + 42 ||
					l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 22 ||
					l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 20 &&
					item->object_number != FLARE_ITEM)
				{
					AddDisplayPickup(item->object_number);
					Inv_AddItem(item->object_number);

					if ((item->object_number == SECRET_ITEM1 || item->object_number == SECRET_ITEM2 || item->object_number == SECRET_ITEM3)
						&& ((savegame.secrets >> 1) & 1) + ((savegame.secrets >> 2) & 1) + (savegame.secrets & 1) >= 3)
						GF_ModifyInventory(CurrentLevel, 1);

					item->status = ITEM_INVISIBLE;

					if (item->object_number == ICON_PICKUP1_ITEM || item->object_number == ICON_PICKUP2_ITEM ||
						item->object_number == ICON_PICKUP3_ITEM || item->object_number == ICON_PICKUP4_ITEM)
					{
#ifdef RANDO_STUFF
						if (rando.levels[RANDOLEVEL].original_id == LV_CHAMBER)
#else
						if (CurrentLevel == LV_CHAMBER)
#endif
							KillItem(item_num);
						else
							level_complete = 1;
					}
					else
						RemoveDrawnItem(item_num);

					return;
				}
			}
			else if (l->current_anim_state == AS_FLAREPICKUP)
			{
				if ((l->anim_number == ANIM_DUCKPICKUPF && l->frame_number == anims[ANIM_DUCKPICKUPF].frame_base + 22 ||
					l->frame_number == anims[ANIM_PICKUPF].frame_base + 58)
					&& item->object_number == FLARE_ITEM && lara.gun_type != LG_FLARE)
				{
					lara.request_gun_type = LG_FLARE;
					lara.gun_type = LG_FLARE;
					InitialiseNewWeapon();
					lara.gun_status = LG_SPECIAL;
					lara.flare_age = (long)item->data & 0x7FFF;
					KillItem(item_num);
#ifdef TROYESTUFF
					return;
#endif
				}

#ifndef TROYESTUFF
				return;
#endif
			}
			else if (input & IN_ACTION && l->current_anim_state == AS_ALL4S)
				l->goal_anim_state = AS_DUCK;
			else if (input & IN_ACTION && !l->gravity_status &&
				(l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH ||
					l->current_anim_state == AS_DUCK && l->anim_number == ANIM_DUCKBREATHE) &&
				lara.gun_status == LG_ARMLESS && (lara.gun_type != LG_FLARE || item->object_number != FLARE_ITEM))
			{
				if (item->object_number == FLARE_ITEM)
				{
					l->goal_anim_state = AS_FLAREPICKUP;

					do AnimateLara(l); while (l->current_anim_state != AS_FLAREPICKUP);
				}
				else
				{
					AlignLaraPosition(&PickUpPosition, item, l);
					l->goal_anim_state = AS_PICKUP;
					AnimateLara(l);
				}

				if (lara.IsDucked)
					l->goal_anim_state = AS_DUCK;
				else
					l->goal_anim_state = AS_STOP;

				lara.gun_status = LG_HANDSBUSY;
			}
		}
	}
	else if (lara.water_status == LARA_UNDERWATER)
	{
		item->pos.x_rot = -4550;

		if (TestLaraPosition(PickUpBoundsUW, item, l))
		{
			if (l->current_anim_state == AS_PICKUP)
			{
				if (l->frame_number == anims[ANIM_PICKUP_UW].frame_base + 18)
				{
					if (item->object_number != FLARE_ITEM)
					{
						AddDisplayPickup(item->object_number);
						Inv_AddItem(item->object_number);

						if ((item->object_number == SECRET_ITEM1 || item->object_number == SECRET_ITEM2 || item->object_number == SECRET_ITEM3)
							&& ((savegame.secrets >> 1) & 1) + ((savegame.secrets >> 2) & 1) + (savegame.secrets & 1) >= 3)
							GF_ModifyInventory(CurrentLevel, 1);

						item->status = ITEM_INVISIBLE;
						RemoveDrawnItem(item_num);
#ifdef TROYESTUFF
						return;
#endif
					}
				}

#ifndef TROYESTUFF
				return;
#endif
			}
			else if (l->current_anim_state == AS_FLAREPICKUP)
			{
				if (l->frame_number == anims[ANIM_PICKUPF_UW].frame_base + 20 && item->object_number == FLARE_ITEM && lara.gun_type != LG_FLARE)
				{
					lara.request_gun_type = LG_FLARE;
					lara.gun_type = LG_FLARE;
					InitialiseNewWeapon();
					lara.gun_status = LG_SPECIAL;
					lara.flare_age = (long)item->data & 0x7FFF;
					draw_flare_meshes();
					KillItem(item_num);
#ifdef TROYESTUFF
					return;
#endif
				}

#ifndef TROYESTUFF
				return;
#endif
			}
			else if (input & IN_ACTION && l->current_anim_state == AS_TREAD && lara.gun_status == LG_ARMLESS && (lara.gun_type != LG_FLARE || item->object_number != FLARE_ITEM))
			{
				if (MoveLaraPosition(&PickUpPositionUW, item, l))
				{
					if (item->object_number == FLARE_ITEM)
					{
						l->fallspeed = 0;
						l->anim_number = ANIM_PICKUPF_UW;
						l->frame_number = anims[ANIM_PICKUPF_UW].frame_base;
						l->current_anim_state = AS_FLAREPICKUP;
						l->goal_anim_state = AS_TREAD;
					}
					else
					{
						l->goal_anim_state = AS_PICKUP;

						do AnimateLara(l); while (l->current_anim_state != AS_PICKUP);

						l->goal_anim_state = AS_TREAD;
					}
				}

#ifndef TROYESTUFF
				return;
#endif
			}
		}
	}

	item->pos.x_rot = rotx;
	item->pos.y_rot = roty;
	item->pos.z_rot = rotz;
}

void BossDropIcon(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* icon;

	item = &items[item_number];

	for (short n = item->carried_item; n != NO_ITEM; n = icon->carried_item)	//uhm..
	{
		icon = &items[n];
		icon->pos.x_pos = item->pos.x_pos;
		icon->pos.y_pos = item->pos.y_pos;
		icon->pos.z_pos = item->pos.z_pos;
		ItemNewRoom(n, item->room_number);
		AddActiveItem(n);
		item->status = ITEM_ACTIVE;
	}
}

void AnimatingPickUp(short item_number)
{
	ITEM_INFO* item;
	long ang, c, dx, dy, dz;

	item = &items[item_number];

	if (item->status == ITEM_INVISIBLE || item->flags & IFL_CLEARBODY)
		return;

	if (item->object_number == SAVEGAME_CRYSTAL_ITEM && !item->item_flags[1])
	{
		item->item_flags[1] = 1;
		item->item_flags[2] = (short)item->pos.y_pos;
	}

	item->pos.y_rot += 1024;
	item->item_flags[0] = (item->item_flags[0] + 1) & 0x3F;
	ang = rcossin_tbl[item->item_flags[0] << 7];
	c = abs(ang >> 7);

	if (c > 31)
		c = 31;

	if (item->object_number == SAVEGAME_CRYSTAL_ITEM)
	{
		item->pos.y_pos = item->item_flags[2] - abs(ang >> 4) - 64;
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 8, 0, c, 0);
		dx = abs(item->pos.x_pos - lara_item->pos.x_pos);
		dy = abs(item->pos.y_pos - lara_item->pos.y_pos);
		dz = abs(item->pos.z_pos - lara_item->pos.z_pos);

		if (dx < 256 && dy < 1024 && dz < 256)
		{
			lara.poisoned = 0;
			lara_item->hit_points += 500;

			if (lara_item->hit_points > 1000)
				lara_item->hit_points = 1000;
			
#ifdef TROYESTUFF
			if (tomb3.psx_crystal_sfx)
				SoundEffect(SFX_SAVE_CRYSTAL, &lara_item->pos, SFX_DEFAULT);
			else
#endif
				SoundEffect(SFX_MENU_MEDI, &lara_item->pos, SFX_DEFAULT);
			
			KillItem(item_number);
		}
	}
	else
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 8, 0, c, c >> 1);
}

void inject_pickup(bool replace)
{
	INJECT(0x0045BC00, PickUpCollision, inject_rando ? 1 : replace);
	INJECT(0x0045CDE0, BossDropIcon, replace);
	INJECT(0x0045CE70, AnimatingPickUp, replace);
}
