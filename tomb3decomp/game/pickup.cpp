#include "../tomb3/pch.h"
#include "pickup.h"
#include "collide.h"
#include "laraanim.h"
#include "objects.h"
#include "laramisc.h"
#include "larafire.h"
#include "laraflar.h"
#include "items.h"
#include "health.h"
#include "invfunc.h"
#include "gameflow.h"
#ifdef RANDO_STUFF
#include "../specific/smain.h"
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
				}

				return;	//move to after KillItem^
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
					}
				}

				return;	//move to after RemoveDrawnItem^
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
				}

				return;	//move to after KillItem^
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

				return;	//remove
			}
		}
	}

	item->pos.x_rot = rotx;
	item->pos.y_rot = roty;
	item->pos.z_rot = rotz;
}

void inject_pickup(bool replace)
{
	INJECT(0x0045BC00, PickUpCollision, inject_rando ? 1 : replace);
}
