#include "../tomb3/pch.h"
#include "traps.h"
#include "control.h"
#include "gameflow.h"
#include "objects.h"
#include "items.h"
#include "kayak.h"
#include "sound.h"
#ifdef RANDO_STUFF
#include "../specific/smain.h"
#endif
#include "effects.h"
#include "../specific/game.h"
#include "draw.h"
#include "effect2.h"
#include "sphere.h"

void LaraBurn()
{
	FX_INFO* fx;
	short fxNum;

	if (!lara.burn)
	{
		fxNum = CreateEffect(lara_item->room_number);

		if (fxNum != NO_ITEM)
		{
			fx = &effects[fxNum];
			fx->frame_number = 1;
			fx->object_number = FLAME;
			fx->counter = -1;
			lara.burn = 1;
		}
	}
}

void LavaBurn(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	long h;
	short room_num;

	if (item->hit_points >= 0 && lara.water_status != LARA_CHEAT)
	{
		room_num = item->room_number;
		floor = GetFloor(item->pos.x_pos, 32000, item->pos.z_pos, &room_num);
		h = GetHeight(floor, item->pos.x_pos, 32000, item->pos.z_pos);

		if (item->floor == h)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_RAPIDS)
#else
			if (CurrentLevel == LV_RAPIDS)
#endif
				LaraRapidsDrown();
			else
			{
				item->hit_status = 1;
				item->hit_points = -1;

#ifdef RANDO_STUFF
				if (rando.levels[RANDOLEVEL].original_id == LV_AREA51 || rando.levels[RANDOLEVEL].original_id == LV_OFFICE)
#else
				if (CurrentLevel == LV_AREA51 || CurrentLevel == LV_OFFICE)
#endif
					lara.electric = 1;
				else
					LaraBurn();
			}
		}
	}
}

void SpikeControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

#ifdef RANDO_STUFF
	if (TriggerActive(item) && (rando.levels[RANDOLEVEL].original_id == LV_SHORE || rando.levels[RANDOLEVEL].original_id == LV_RAPIDS))
#else
	if (TriggerActive(item) && (CurrentLevel == LV_SHORE || CurrentLevel == LV_RAPIDS))
#endif
	{
		if (item->frame_number == anims[item->anim_number].frame_base)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_SHORE)
#else
			if (CurrentLevel == LV_SHORE)
#endif
				SoundEffect(SFX_SHIVA_SWORD_2, &item->pos, SFX_ALWAYS);
			else
				SoundEffect(SFX_LARA_GETOUT, &item->pos, SFX_ALWAYS);

		}

		AnimateItem(item);
	}
}

void PropellerControl(short item_number)
{
	ITEM_INFO * item;

	item = &items[item_number];

	if (!TriggerActive(item) || item->flags & IFL_INVISIBLE)
	{
		if (item->goal_anim_state != 1)
		{
			if (item->object_number == FAN)
				SoundEffect(SFX_UNDERWATER_FAN_STOP, &item->pos, SFX_WATER);

			item->goal_anim_state = 1;
		}
	}
	else
	{
		item->goal_anim_state = 0;

		if (item->touch_bits & 6)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_ROOFTOPS)
#else
			if (CurrentLevel == LV_ROOFTOPS)
#endif
			{
				lara_item->hit_points = -1;
				DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos,
					short(GetRandomControl() >> 10), item->pos.y_rot + 0x4000, lara_item->room_number, 5);
			}
			else
				lara_item->hit_points -= 200;

			lara_item->hit_status = 1;
			DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos,
				short(GetRandomControl() >> 10), item->pos.y_rot + 0x4000, lara_item->room_number, 3);

			if (item->object_number == SAW)
				SoundEffect(SFX_VERY_SMALL_WINCH, &item->pos, 0);
		}
		else if (item->object_number == SAW)
			SoundEffect(SFX_DRILL_BIT_1, &item->pos, SFX_DEFAULT);
		else if (item->object_number == FAN)
			SoundEffect(SFX_UNDERWATER_FAN_ON, &item->pos, SFX_WATER);
		else
			SoundEffect(SFX_SMALL_FAN_ON, &item->pos, SFX_DEFAULT);
	}

	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
	{
		RemoveActiveItem(item_number);

		if (item->object_number != SAW)
			item->collidable = 0;
	}
}

void SideFlameDetection(FX_INFO* fx, long length)
{
	short* bounds;
	long dx, dz, x, z, xs, xe, zs, ze;

	dx = lara_item->pos.x_pos - fx->pos.x_pos;
	dz = lara_item->pos.z_pos - fx->pos.z_pos;

	if (dx < -20480 || dx > 20480 || dz < -20480 || dz > 20480)
		return;

	switch (fx->pos.y_rot)
	{
	case 0:
		x = fx->pos.x_pos;
		z = fx->pos.z_pos + 512;
		xs = -256;
		xe = 256;
		zs = -length;
		ze = 0;
		break;

	case 0x4000:
		x = fx->pos.x_pos + 512;
		z = fx->pos.z_pos;
		xs = -length;
		xe = 0;
		zs = -256;
		ze = 256;
		break;

	case -0x4000:
		x = fx->pos.x_pos - 512;
		z = fx->pos.z_pos;
		xs = 0;
		xe = length;
		zs = -256;
		ze = 256;
		break;

	case -0x8000:
		x = fx->pos.x_pos;
		z = fx->pos.z_pos - 512;
		xs = -256;
		xe = 256;
		zs = 0;
		ze = length;
		break;

	default:
		x = 0;
		z = 0;
		xs = 0;
		xe = 0;
		zs = 0;
		ze = 0;
		break;
	}

	bounds = GetBoundsAccurate(lara_item);

	if (lara_item->pos.x_pos >= x + xs && lara_item->pos.x_pos <= x + xe &&
		lara_item->pos.z_pos >= z + zs && lara_item->pos.z_pos <= z + ze &&
		lara_item->pos.y_pos + bounds[2] <= fx->pos.y_pos + 128 &&
		lara_item->pos.y_pos + bounds[3] >= fx->pos.y_pos - 384)
	{
		if (fx->flag1 >= 18)
			LaraBurn();
		else
		{
			lara_item->hit_points -= 5;
			lara_item->hit_status = 1;
		}
	}
}

void FlameControl(short fx_number)
{
	FX_INFO* fx;
	PHD_3DPOS pos;
	long rnd, x, y, z, angle, rad, s, c, dist, f2, wh;
	uchar xzoffs[16][2] =
	{
		{ 9, 9 },
		{ 24,9 },
		{ 40,9 },
		{ 55,9 },
		{ 9,24 },
		{ 24,24 },
		{ 40,24 },
		{ 55,24 },
		{ 9,40 },
		{ 24,40 },
		{ 40,40 },
		{ 55,40 },
		{ 9,55 },
		{ 24,55 },
		{ 40,55 },
		{ 55,55 }
	};

	fx = &effects[fx_number];
	rnd = GetRandomControl();
	rad = 0;

	if (fx->frame_number == BIG_FIRE)
	{
		if (!(wibble & 0xC))
		{
			TriggerFireFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 0);
			TriggerFireSmoke(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 0);
		}

		TriggerStaticFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, (GetRandomControl() & 0xF) + 96);
	}
	else if (fx->frame_number == SMALL_FIRE)
	{
		if (fx->counter >= 0)
		{
			angle = ((fx->pos.y_rot >> 4) & 4095) << 1;
			s = (288 * rcossin_tbl[angle]) >> (W2V_SHIFT - 2);
			c = (288 * rcossin_tbl[angle + 1]) >> (W2V_SHIFT - 2);

			TriggerStaticFlame(fx->pos.x_pos + s, fx->pos.y_pos - 192, fx->pos.z_pos + c, (GetRandomControl() & 15) + 32);

			if (!(wibble & 0x18))
			{
				TriggerFireFlame(fx->pos.x_pos + s, fx->pos.y_pos - 224, fx->pos.z_pos + c, -1, 1);

				if (!(wibble & 0x18))
					TriggerFireSmoke(fx->pos.x_pos + s, fx->pos.y_pos, fx->pos.z_pos + c, -1, 1);
			}
		}
		else
		{
			for (int i = 0; i < NUM_LARA_MESHES; i++)
			{
				if (!(wibble & 0xC))
				{
					fx->pos.x_pos = 0;
					fx->pos.y_pos = 0;
					fx->pos.z_pos = 0;
					GetJointAbsPosition(lara_item, (PHD_VECTOR*)&fx->pos, i);
					TriggerFireFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 255 - lara.BurnGreen);
				}
			}

			if (lara.BurnGreen)
				TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 13, (rnd >> 2) & 7, (rnd & 7) + 24, ((rnd >> 4) & 3) + 12);
			else
				TriggerDynamic(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 13, (rnd & 7) + 24, ((rnd >> 4) & 3) + 12, 0);

			if (lara_item->room_number != fx->room_number)
				EffectNewRoom(fx_number, lara_item->room_number);

			wh = GetWaterHeight(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->room_number);

			if (wh == NO_HEIGHT || fx->pos.y_pos <= wh || (room[fx->room_number].flags & ROOM_SWAMP &&
#ifdef RANDO_STUFF
				(rando.levels[RANDOLEVEL].original_id == LV_CITY ||
				rando.levels[RANDOLEVEL].original_id == LV_INDIABOSS ||
				rando.levels[RANDOLEVEL].original_id == LV_CHAMBER)))
#else
				(CurrentLevel == LV_CITY || CurrentLevel == LV_INDIABOSS || CurrentLevel == LV_CHAMBER)))
#endif
			{
				SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &fx->pos, SFX_DEFAULT);
				lara_item->hit_points -= 7;
				lara_item->hit_status = 1;
			}
			else
			{
				fx->counter = 0;
				KillEffect(fx_number);
				lara.burn = 0;
			}

			return;
		}
	}
	else if (fx->frame_number == JET_FIRE)
	{
		if (fx->flag1)
			fx->flag1--;
		else
		{
			fx->flag1 = (GetRandomControl() & 3) + 8;
			f2 = GetRandomControl() & 0x3F;

			if (fx->flag2 == f2)
				f2 = (f2 + 13) & 0x3F;

			fx->flag2 = (short)f2;
		}

		x = (xzoffs[fx->flag2 & 7][0] << 4) - 512;
		z = (xzoffs[fx->flag2 & 7][1] << 4) - 512;

		if (!(wibble & 4))
			TriggerFireFlame(fx->pos.x_pos + x, fx->pos.y_pos, fx->pos.z_pos + z, -1, 2);

		x = (xzoffs[(fx->flag2 >> 3) + 8][0] << 4) - 512;
		z = (xzoffs[(fx->flag2 >> 3) + 8][1] << 4) - 512;

		if (wibble & 4)
			TriggerFireFlame(fx->pos.x_pos + x, fx->pos.y_pos, fx->pos.z_pos + z, -1, 2);
	}
	else
	{
		angle = (fx->pos.y_rot >> 3) & 0x1FFE;
		rad = (rnd & 0xFF) + 512;
		s = (rad * rcossin_tbl[angle]) >> (W2V_SHIFT - 2);
		c = (rad * rcossin_tbl[angle + 1]) >> (W2V_SHIFT - 2);

		if (fx->flag2)
		{
			if (wibble & 4)
				TriggerSideFlame(fx->pos.x_pos + s, fx->pos.y_pos, fx->pos.z_pos + c, (angle - 4096) & 0x1FFF, (!(GetRandomControl() & 7)) ? 1 : 0, 1);

			fx->flag2--;
		}
		else
		{
			if (fx->flag1)
			{
				if (wibble & 4)
				{
					if (fx->flag1 > 112)
						TriggerSideFlame(fx->pos.x_pos + s, fx->pos.y_pos, fx->pos.z_pos + c, (angle + 4096) & 0x1FFE, (129 - fx->flag1) >> 1, 0);
					else if (fx->flag1 < 18)
						TriggerSideFlame(fx->pos.x_pos + s, fx->pos.y_pos, fx->pos.z_pos + c, (angle + 4096) & 0x1FFE, (fx->flag1 >> 1) + 1, 0);
					else
						TriggerSideFlame(fx->pos.x_pos + s, fx->pos.y_pos, fx->pos.z_pos + c, (angle + 4096) & 0x1FFE, 9, 0);
				}

				fx->flag1 -= 2;
			}
			else
			{
				fx->flag1 = 128;

#ifdef RANDO_STUFF
				if (rando.levels[RANDOLEVEL].original_id == LV_RAPIDS)
#else
				if (CurrentLevel == LV_RAPIDS)
#endif
					fx->flag2 = 120;
				else
					fx->flag2 = 60;
			}
		}
	}

	x = fx->pos.x_pos + ((rnd & 0xF) << 5);
	y = fx->pos.y_pos + ((rnd & 0xF0) << 1);
	z = fx->pos.z_pos + ((rnd >> 3) & 0x1E0);

	if (fx->frame_number == SIDE_FIRE)
	{
		if (fx->flag2)
			rad = 0;
		else if (fx->flag1 < 18)
			rad = 2048;
		else if (fx->flag1 < 64)
			rad = 2048;
		else
			rad = (128 - fx->flag1) << 5;

		angle = (((fx->pos.y_rot >> 3) & 0xFFFE) - 4096) & 0x1FFE;
		s = (rad * rcossin_tbl[angle]) >> (W2V_SHIFT - 2);
		c = (rad * rcossin_tbl[angle + 1]) >> (W2V_SHIFT - 2);
		TriggerDynamic(x + s, y, z + c, fx->flag2 ? 6 : 13,
			(rnd & 7) + 24, ((rnd >> 4) & 3) + 12, 0);
	}
	else
		TriggerDynamic(x, y, z, 16 - (fx->frame_number << 2), (rnd & 7) + 24, ((rnd >> 4) & 3) + 12, 0);

	SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &fx->pos, SFX_DEFAULT);

	if (fx->counter)
		fx->counter--;
	else if (fx->frame_number == SIDE_FIRE)
	{
		if (!lara.burn && rad)
			SideFlameDetection(fx, rad);
	}
	else if (fx->frame_number != SMALL_FIRE)
	{
		pos.x_pos = fx->pos.x_pos;
		pos.y_pos = fx->pos.y_pos;
		pos.z_pos = fx->pos.z_pos;

		if (ItemNearLara(&pos, 600))
		{
			x = lara_item->pos.x_pos - pos.x_pos;
			z = lara_item->pos.z_pos - pos.z_pos;
			dist = SQUARE(x) + SQUARE(z);
			lara_item->hit_points -= 5;
			lara_item->hit_status = 1;

			if (dist < 202500)
			{
				fx->counter = 100;
				LaraBurn();
			}
		}
	}
}

void inject_traps(bool replace)
{
	INJECT(0x0046FAE0, LaraBurn, replace);
	INJECT(0x0046FB30, LavaBurn, inject_rando ? 1 : replace);
	INJECT(0x0046E340, SpikeControl, inject_rando ? 1 : replace);
	INJECT(0x0046D340, PropellerControl, inject_rando ? 1 : replace);
	INJECT(0x0046F1E0, SideFlameDetection, replace);
	INJECT(0x0046F370, FlameControl, inject_rando ? 1 : replace);
}
