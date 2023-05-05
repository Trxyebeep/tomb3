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
#include "control.h"
#include "inventry.h"
#include "../specific/input.h"
#include "savegame.h"
#include "../tomb3/tomb3.h"

static short PickUpBounds[12] = { -256, 256, -100, 100, -256, 256, -1820, 1820, 0, 0, 0, 0 };
static short PickUpBoundsUW[12] = { -512, 512, -512, 512, -512, 512, -8190, 8190, -8190, 8190, -8190, 8190 };
static short PuzzleHoleBounds[12] = { -200, 200, 0, 0, 312, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short KeyHoleBounds[12] = { -200, 200, 0, 0, 312, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short Switch1Bounds[12] = { -220, 220, 0, 0, 292, 512, -1820, 1820, -5460, 5460, -1820, 1820 };
static short Switch2Bounds[12] = { -1024, 1024, -1024, 1024, -1024, 512, -14560, 14560, -14560, 14560, -14560, 14560 };
static PHD_VECTOR PickUpPosition = { 0, 0, -100 };
static PHD_VECTOR PickUpPositionUW = { 0, -200, -350 };
static PHD_VECTOR PuzzleHolePosition = { 0, 0, 327 };
static PHD_VECTOR KeyHolePosition = { 0, 0, 362 };
static PHD_VECTOR DetonatorPosition = { 0, 0, 0 };
static PHD_VECTOR SmallSwitchPosition = { 0, 0, 362 };
static PHD_VECTOR PushSwitchPosition = { 0, 0, 292 };
static PHD_VECTOR AirlockPosition = { 0, 0, 212 };
static PHD_VECTOR Switch2Position = { 0, 0, 108 };
static long pup_x, pup_y, pup_z;

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
				if ((l->frame_number == anims[ANIM_PICKUP].frame_base + 42 ||
					l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 22 ||
					l->frame_number == anims[ANIM_DUCKPICKUP].frame_base + 20) &&
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
						if (CurrentLevel == LV_CHAMBER)
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
					return;
				}
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
						return;
					}
				}
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
					return;
				}
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

	for (short n = item->carried_item; n != NO_ITEM; n = icon->carried_item)
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

	c <<= 3;

	if (item->object_number == SAVEGAME_CRYSTAL_ITEM)
	{
		item->pos.y_pos = item->item_flags[2] - abs(ang >> 4) - 64;

		if (tomb3.blue_crystal_light)
			TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 8, 0, c >> 2, c);
		else
			TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 8, 0, c, 0);

		dx = abs(item->pos.x_pos - lara_item->pos.x_pos);
		dy = abs(item->pos.y_pos - lara_item->pos.y_pos);
		dz = abs(item->pos.z_pos - lara_item->pos.z_pos);

		if (dx < 256 && dy < 1024 && dz < 256)
		{
			if (tomb3.psx_saving)
			{
				Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);
				SoundEffect(SFX_SAVE_CRYSTAL, &lara_item->pos, SFX_DEFAULT);
			}
			else
			{
				lara.poisoned = 0;
				lara_item->hit_points += 500;

				if (lara_item->hit_points > 1000)
					lara_item->hit_points = 1000;

				if (tomb3.psx_crystal_sfx)
					SoundEffect(SFX_SAVE_CRYSTAL, &lara_item->pos, SFX_DEFAULT);
				else
					SoundEffect(SFX_MENU_MEDI, &lara_item->pos, SFX_DEFAULT);
			}

			KillItem(item_number);
		}
	}
	else
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 8, 0, c, c >> 1);
}

void PuzzleHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long correct;

	item = &items[item_number];
	correct = 0;

	if (l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH)
	{
		if ((Inventory_Chosen == NO_ITEM && !(input & IN_ACTION)) || lara.gun_status != LG_ARMLESS ||
			l->gravity_status || !TestLaraPosition(PuzzleHoleBounds, &items[item_number], l))
			return;

		if (item->status != ITEM_INACTIVE)
		{
			if (l->pos.x_pos != pup_x || l->pos.y_pos != pup_y || l->pos.z_pos != pup_z)
			{
				pup_x = l->pos.x_pos;
				pup_y = l->pos.y_pos;
				pup_z = l->pos.z_pos;
				SoundEffect(SFX_LARA_NO, &l->pos, SFX_DEFAULT);
			}

			return;
		}

		if (Inventory_Chosen == NO_ITEM)
		{
			Display_Inventory(INV_KEYS_MODE);

			if (Inventory_Chosen == NO_ITEM && inv_keys_objects)
				return;

			if (Inventory_Chosen != NO_ITEM)
				pup_y = l->pos.y_pos - 1;
		}
		else
			pup_y = l->pos.y_pos - 1;

		switch (item->object_number)
		{
		case PUZZLE_HOLE1:

			if (Inventory_Chosen == PUZZLE_OPTION1)
			{
				Inv_RemoveItem(PUZZLE_OPTION1);
				correct = 1;
			}

			break;
		case PUZZLE_HOLE2:

			if (Inventory_Chosen == PUZZLE_OPTION2)
			{
				Inv_RemoveItem(PUZZLE_OPTION2);
				correct = 1;
			}

			break;
		case PUZZLE_HOLE3:

			if (Inventory_Chosen == PUZZLE_OPTION3)
			{
				Inv_RemoveItem(PUZZLE_OPTION3);
				correct = 1;
			}

			break;
		case PUZZLE_HOLE4:

			if (Inventory_Chosen == PUZZLE_OPTION4)
			{
				Inv_RemoveItem(PUZZLE_OPTION4);
				correct = 1;
			}

			break;
		}

		Inventory_Chosen = NO_ITEM;

		if (correct)
		{
			AlignLaraPosition(&PuzzleHolePosition, item, l);
			l->goal_anim_state = AS_USEPUZZLE;

			do AnimateLara(l); while (l->current_anim_state != AS_USEPUZZLE);

			l->goal_anim_state = AS_STOP;
			lara.gun_status = LG_HANDSBUSY;
			item->status = ITEM_ACTIVE;
			pup_x = l->pos.x_pos;
			pup_y = l->pos.y_pos;
			pup_z = l->pos.z_pos;
		}
		else if (l->pos.x_pos != pup_x || l->pos.y_pos != pup_y || l->pos.z_pos != pup_z)
		{
			SoundEffect(SFX_LARA_NO, &l->pos, SFX_DEFAULT);
			pup_x = l->pos.x_pos;
			pup_y = l->pos.y_pos;
			pup_z = l->pos.z_pos;
		}
	}
	else if (l->current_anim_state == AS_USEPUZZLE && TestLaraPosition(PuzzleHoleBounds, &items[item_number], l) &&
		l->frame_number == anims[ANIM_USEPUZZLE].frame_base + 80)
	{
		switch (item->object_number)
		{
		case PUZZLE_HOLE1:
			item->object_number = PUZZLE_DONE1;
			break;

		case PUZZLE_HOLE2:
			item->object_number = PUZZLE_DONE2;
			break;

		case PUZZLE_HOLE3:
			item->object_number = PUZZLE_DONE3;
			break;

		case PUZZLE_HOLE4:
			item->object_number = PUZZLE_DONE4;
			break;
		}

		item->anim_number = objects[item->object_number].anim_index;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = anims[item->anim_number].current_anim_state;
		item->goal_anim_state = item->current_anim_state;
		item->required_anim_state = 0;
		AddActiveItem(item_number);
		item->flags = IFL_CODEBITS;
		item->status = ITEM_ACTIVE;
		AnimateItem(item);
	}
}

void KeyHoleCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long correct;

	if (l->current_anim_state != AS_STOP || l->anim_number != ANIM_BREATH)
		return;

	item = &items[item_number];
	correct = 0;

	if ((Inventory_Chosen == NO_ITEM && !(input & IN_ACTION)) || lara.gun_status != LG_ARMLESS ||
		l->gravity_status || !TestLaraPosition(KeyHoleBounds, &items[item_number], l))
		return;

	if (item->status != ITEM_INACTIVE)
	{
		if (l->pos.x_pos != pup_x || l->pos.y_pos != pup_y || l->pos.z_pos != pup_z)
		{
			pup_x = l->pos.x_pos;
			pup_y = l->pos.y_pos;
			pup_z = l->pos.z_pos;
			SoundEffect(SFX_LARA_NO, &l->pos, SFX_DEFAULT);
		}

		return;
	}

	if (Inventory_Chosen == NO_ITEM)
	{
		Display_Inventory(INV_KEYS_MODE);

		if (Inventory_Chosen == NO_ITEM && inv_keys_objects)
			return;

		if (Inventory_Chosen != NO_ITEM)
			pup_y = l->pos.y_pos - 1;
	}
	else
		pup_y = l->pos.y_pos - 1;

	switch (item->object_number)
	{
	case KEY_HOLE1:

		if (Inventory_Chosen == KEY_OPTION1)
		{
			Inv_RemoveItem(KEY_OPTION1);
			correct = 1;
		}

		break;

	case KEY_HOLE2:

		if (Inventory_Chosen == KEY_OPTION2)
		{
			Inv_RemoveItem(KEY_OPTION2);
			correct = 1;
		}

		break;

	case KEY_HOLE3:

		if (Inventory_Chosen == KEY_OPTION3)
		{
			Inv_RemoveItem(KEY_OPTION3);
			correct = 1;
		}

		break;

	case KEY_HOLE4:

		if (Inventory_Chosen == KEY_OPTION4)
		{
			Inv_RemoveItem(KEY_OPTION4);
			correct = 1;
		}

		break;
	}

	Inventory_Chosen = -1;

	if (correct)
	{
		AlignLaraPosition(&KeyHolePosition, item, l);
		l->goal_anim_state = AS_USEKEY;

		do AnimateLara(l); while (l->current_anim_state != AS_USEKEY);

		l->goal_anim_state = AS_STOP;
		lara.gun_status = LG_HANDSBUSY;
		item->status = ITEM_ACTIVE;
		pup_x = l->pos.x_pos;
		pup_y = l->pos.y_pos;
		pup_z = l->pos.z_pos;
	}
	else if (l->pos.x_pos != pup_x || l->pos.y_pos != pup_y || l->pos.z_pos != pup_z)
	{
		pup_x = l->pos.x_pos;
		pup_y = l->pos.y_pos;
		pup_z = l->pos.z_pos;
		SoundEffect(SFX_LARA_NO, &l->pos, SFX_DEFAULT);
	}
}

void DetonatorCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	short x_rot, y_rot, z_rot;

	if (lara.extra_anim)
		return;

	item = &items[item_number];
	x_rot = item->pos.x_rot;
	y_rot = item->pos.y_rot;
	z_rot = item->pos.z_rot;
	item->pos.x_rot = 0;
	item->pos.y_rot = l->pos.y_rot;
	item->pos.z_rot = 0;
	
	if (item->status==ITEM_DEACTIVATED || !(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || l->gravity_status ||
		l->current_anim_state != AS_STOP || item->object_number == DETONATOR && !TestLaraPosition(PickUpBounds, item, l))
	{
		item->pos.x_rot = x_rot;
		item->pos.y_rot = y_rot;
		item->pos.z_rot = z_rot;
		ObjectCollision(item_number, l, coll);
		return;
	}

	if (Inventory_Chosen == NO_ITEM)
		Display_Inventory(INV_KEYS_MODE);

	if (Inventory_Chosen != KEY_OPTION2)
	{
		item->pos.x_rot = x_rot;
		item->pos.y_rot = y_rot;
		item->pos.z_rot = z_rot;
		ObjectCollision(item_number, l, coll);
		return;
	}

	Inv_RemoveItem(KEY_OPTION2);
	AlignLaraPosition(&DetonatorPosition, item, l);
	l->anim_number = objects[LARA_EXTRA].anim_index;
	l->frame_number = anims[l->anim_number].frame_base;
	l->current_anim_state = EXTRA_BREATH;

	if (item->object_number == DETONATOR)
		l->goal_anim_state = EXTRA_PLUNGER;

	AnimateItem(l);
	lara.extra_anim = 1;
	lara.gun_status = LG_HANDSBUSY;

	if (item->object_number == DETONATOR)
	{
		item->status = ITEM_ACTIVE;
		AddActiveItem(item_number);
	}
}

void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!(input & IN_ACTION) || item->status != ITEM_INACTIVE || item->flags & IFL_INVISIBLE || lara.gun_status != LG_ARMLESS ||
		l->gravity_status || l->current_anim_state != AS_STOP || l->anim_number != ANIM_BREATH || !TestLaraPosition(Switch1Bounds, item, l))
		return;

	lara.head_x_rot = 0;
	lara.head_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	l->pos.y_rot = item->pos.y_rot;

	if (item->object_number == SMALL_SWITCH)
		AlignLaraPosition(&SmallSwitchPosition, item, l);
	else if (item->object_number == PUSH_SWITCH)
		AlignLaraPosition(&PushSwitchPosition, item, l);
	else if (item->object_number == AIRLOCK_SWITCH)
		AlignLaraPosition(&AirlockPosition, item, l);

	if (item->current_anim_state == 1)
	{
		l->current_anim_state = AS_SWITCHON;

		if (item->object_number == SMALL_SWITCH)
			l->anim_number = objects[LARA].anim_index + ANIM_GENERATORSW_OFF;
		else if (item->object_number == PUSH_SWITCH)
			l->anim_number = objects[LARA].anim_index + ANIM_ONEHANDPUSHSW;
		else if (item->object_number == AIRLOCK_SWITCH)
		{
			l->anim_number = objects[LARA_EXTRA].anim_index;
			l->frame_number = anims[l->anim_number].frame_base;
			l->current_anim_state = EXTRA_BREATH;
			l->goal_anim_state = EXTRA_AIRLOCK;
			AnimateItem(l);
			lara.extra_anim = 1;
		}
		else
			l->anim_number = objects[LARA].anim_index + ANIM_SWITCHOFF;

		item->goal_anim_state = 0;
	}
	else
	{
		l->current_anim_state = AS_SWITCHOFF;

		if (item->object_number == SMALL_SWITCH)
			l->anim_number = objects[LARA].anim_index + ANIM_GENERATORSW_ON;
		else if (item->object_number == PUSH_SWITCH)
			l->anim_number = objects[LARA].anim_index + ANIM_ONEHANDPUSHSW;
		else if (item->object_number == AIRLOCK_SWITCH)
		{
			l->anim_number = objects[LARA_EXTRA].anim_index;
			l->frame_number = anims[l->anim_number].frame_base;
			l->current_anim_state = EXTRA_BREATH;
			l->goal_anim_state = EXTRA_AIRLOCK;
			AnimateItem(l);
			lara.extra_anim = 1;
		}
		else
			l->anim_number = objects[LARA].anim_index + ANIM_SWITCHON;

		item->goal_anim_state = 1;
	}

	if (!lara.extra_anim)
	{
		l->frame_number = anims[l->anim_number].frame_base;
		l->goal_anim_state = AS_STOP;
	}

	lara.gun_status = LG_HANDSBUSY;
	item->status = ITEM_ACTIVE;
	AddActiveItem(item_number);
	AnimateItem(item);
}

void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!(input & IN_ACTION) || item->status != ITEM_INACTIVE || lara.water_status != LARA_UNDERWATER || lara.gun_status != LG_ARMLESS
		|| l->current_anim_state != AS_TREAD)
		return;

	if (!TestLaraPosition(Switch2Bounds, item, l))
		return;

	if (item->current_anim_state != 1 && item->current_anim_state)
		return;

	if (MoveLaraPosition(&Switch2Position, item, l))
	{
		l->fallspeed = 0;
		l->goal_anim_state = AS_SWITCHON;

		do AnimateLara(l); while (l->current_anim_state != AS_SWITCHON);

		l->goal_anim_state = AS_TREAD;
		lara.gun_status = LG_HANDSBUSY;
		item->status = ITEM_ACTIVE;
		
		if (item->current_anim_state == 1)
			item->goal_anim_state = 0;
		else
			item->goal_anim_state = 1;

		AddActiveItem(item_number);
		AnimateItem(item);
	}
}

void SwitchControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->flags |= IFL_CODEBITS;

	if (!TriggerActive(item) && !(item->flags & IFL_INVISIBLE))
	{
		item->goal_anim_state = 1;
		item->timer = 0;
	}

	AnimateItem(item);
}

long SwitchTrigger(short item_number, short timer)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_DEACTIVATED)
	{
		if (!item->current_anim_state && timer > 0)
		{
			item->timer = timer;
			item->status = ITEM_ACTIVE;

			if (timer != 1)
				item->timer *= 30;
		}
		else
		{
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;

			if (item->item_flags[0])
				item->flags |= IFL_INVISIBLE;
		}

		return 1;
	}

	if (item->flags & IFL_INVISIBLE)
		return 1;

	return 0;
}

long KeyTrigger(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_ACTIVE && lara.gun_status != LG_HANDSBUSY)
	{
		item->status = ITEM_DEACTIVATED;
		return 1;
	}

	return 0;
}

long PickupTrigger(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status == ITEM_INVISIBLE)
	{
		item->status = ITEM_DEACTIVATED;
		return 1;
	}

	return 0;
}
