#include "../tomb3/pch.h"
#include "traps.h"
#include "control.h"
#include "gameflow.h"
#include "objects.h"
#include "items.h"
#include "kayak.h"
#include "sound.h"
#include "../specific/smain.h"
#include "effects.h"
#include "../specific/game.h"
#include "draw.h"
#include "effect2.h"
#include "sphere.h"
#include "../3dsystem/phd_math.h"

static BITE_INFO teeth1a = { -23, 0, -1718, 0 };
static BITE_INFO teeth1b = { 71, 0, -1718, 1 };
static BITE_INFO teeth2a = { -23, 10, -1718, 0 };
static BITE_INFO teeth2b = { 71, 10, -1718, 1 };
static BITE_INFO teeth3a = { -23, -10, -1718, 0 };
static BITE_INFO teeth3b = { 71, -10, -1718, 1 };

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
#ifdef TROYESTUFF
			if (lara.water_status == LARA_CHEAT)
			{
				fx->counter = 0;
				KillEffect(fx_number);
				lara.burn = 0;
				return;
			}
#endif

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

void ControlSpikeWall(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h;
	short room_number;

	item = &items[item_number];

	if (TriggerActive(item) && item->status != ITEM_DEACTIVATED)
	{
		x = item->pos.x_pos + (16 * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
		y = item->pos.y_pos;
		z = item->pos.z_pos + (16 * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
		room_number = item->room_number;
		floor = GetFloor(x, y, z, &room_number);
		h = GetHeight(floor, x, y, z);

		if (item->pos.y_pos == h)
		{
			item->pos.x_pos = x;
			item->pos.z_pos = z;

			if (item->room_number != room_number)
				ItemNewRoom(item_number, room_number);

			SoundEffect(SFX_ROLLING_BALL, &item->pos, SFX_DEFAULT);
		}
		else
		{
			item->status = ITEM_DEACTIVATED;
			StopSoundEffect(SFX_ROLLING_BALL);
		}
	}

	if (item->touch_bits)
	{
		lara_item->hit_points -= 20;
		lara_item->hit_status = 1;
		DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos, 1, item->pos.y_rot, lara_item->room_number, 3);
		SoundEffect(SFX_LARA_GRABFEET, &item->pos, SFX_DEFAULT);
		item->touch_bits = 0;
	}
}

void ControlCeilingSpikes(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h;
	short room_number;

	item = &items[item_number];

	if (TriggerActive(item) && item->status != ITEM_DEACTIVATED)
	{
		x = item->pos.x_pos;
		y = item->pos.y_pos + (item->item_flags[0] != 1 ? 5 : 10);
		z = item->pos.z_pos;
		room_number = item->room_number;
		floor = GetFloor(x, y, z, &room_number);
		h = GetHeight(floor, x, y, z);

		if (h >= y + 1024)
		{
			item->pos.y_pos = y;

			if (item->room_number != room_number)
				ItemNewRoom(item_number, room_number);

			SoundEffect(SFX_ROLLING_BALL, &item->pos, SFX_DEFAULT);
		}
		else
		{
			item->status = ITEM_DEACTIVATED;
			StopSoundEffect(SFX_ROLLING_BALL);
		}
	}

	if (item->touch_bits)
	{
		lara_item->hit_points -= 20;
		lara_item->hit_status = 1;
		DoLotsOfBlood(lara_item->pos.x_pos, item->pos.y_pos + 768, lara_item->pos.z_pos, 1, item->pos.y_rot, lara_item->room_number, 3);
		SoundEffect(SFX_LARA_GRABFEET, &item->pos, SFX_DEFAULT);
		item->touch_bits = 0;
	}

	if (TriggerActive(item) && item->status != ITEM_DEACTIVATED && item->item_flags[0] == 1)
		AnimateItem(item);
}

void TriggerPendulumFlame(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long x, z;

	item = &items[item_number];
	x = lara_item->pos.x_pos - item->pos.x_pos;
	z = lara_item->pos.z_pos - item->pos.z_pos;

	if (x < -0x4000 || x > 0x4000 || z < -0x4000 || z > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR >> 1;
	sptr->sB = 0;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 28;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) - 16;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0x1F) - 16;
	sptr->Xvel = (GetRandomControl() & 0x3F) - 32;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0x3F) - 32;
	sptr->Friction = 4;
	sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;
	
	if (GetRandomControl() & 1)
	{
		sptr->Flags |= SF_ROTATE;
		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0x1F) - 16;
	}

	sptr->NodeNumber = 3;
	sptr->FxObj = (uchar)item_number;
	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	sptr->Width = (GetRandomControl() & 7) + 32;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

void SideFlameEmitterControl(short item_number)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short fxNum;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->data)
		{
			KillEffect(short((ulong)item->data) - 1);
			item->data = 0;
		}
	}
	else if (!item->data)
	{
		fxNum = CreateEffect(item->room_number);

		if (fxNum != NO_ITEM)
		{
			fx = &effects[fxNum];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->pos.y_rot = item->pos.y_rot;
			fx->frame_number = SIDE_FIRE;
			fx->object_number = FLAME;
			fx->flag1 = 0;
			fx->flag2 = 0;
			fx->counter = 0;
		}

		item->data = (void*)(fxNum + 1);
	}
}

void FlameEmitterControl(short item_number)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short fxNum;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->data)
		{
			KillEffect(short((ulong)item->data) - 1);
			item->data = 0;
		}
	}
	else if (!item->data)
	{
		fxNum = CreateEffect(item->room_number);

		if (fxNum != NO_ITEM)
		{
			fx = &effects[fxNum];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->pos.y_rot = item->pos.y_rot;
			fx->frame_number = BIG_FIRE;
			fx->object_number = FLAME;
			fx->counter = 0;
		}

		item->data = (void*)(fxNum + 1);
	}
}

void FlameEmitter2Control(short item_number)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short fxNum;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->data)
		{
			KillEffect(short((ulong)item->data) - 1);
			item->data = 0;
		}
	}
	else if (!item->data)
	{
		fxNum = CreateEffect(item->room_number);

		if (fxNum != NO_ITEM)
		{
			fx = &effects[fxNum];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->pos.y_rot = item->pos.y_rot;
			fx->frame_number = SMALL_FIRE;
			fx->object_number = FLAME;
			fx->counter = 0;
		}

		item->data = (void*)(fxNum + 1);
	}
}

void FlameEmitter3Control(short item_number)
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short fxNum;

	item = &items[item_number];

	if (!TriggerActive(item))
	{
		if (item->data)
		{
			KillEffect(short((ulong)item->data) - 1);
			item->data = 0;
		}
	}
	else if (!item->data)
	{
		fxNum = CreateEffect(item->room_number);

		if (fxNum != NO_ITEM)
		{
			fx = &effects[fxNum];
			fx->pos.x_pos = item->pos.x_pos;
			fx->pos.y_pos = item->pos.y_pos;
			fx->pos.z_pos = item->pos.z_pos;
			fx->pos.y_rot = item->pos.y_rot;
			fx->frame_number = JET_FIRE;
			fx->object_number = FLAME;
			fx->flag1 = 0;
			fx->flag2 = GetRandomControl() & 0x3F;
			fx->counter = 0;
		}

		item->data = (void*)(fxNum + 1);
	}
}

void DartsControl(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, z, speed;
	short room_num;

	item = &items[item_number];

	if (item->touch_bits)
	{
		lara_item->hit_points -= 25;
		lara_item->hit_status = 1;
		lara.poisoned += 160;
		DoBloodSplat(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, lara_item->speed, lara_item->pos.y_rot, lara_item->room_number);
		KillItem(item_number);
	}
	else
	{
		x = item->pos.x_pos;
		z = item->pos.z_pos;
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
		item->pos.x_pos += (speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.y_pos -= (item->speed * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
		item->pos.z_pos += (speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
		room_num = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);

		if (item->room_number != room_num)
			ItemNewRoom(item_number, room_num);

		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (item->pos.y_pos >= item->floor)
		{
			for (int i = 0; i < 4; i++)
				TriggerDartSmoke(x, item->pos.y_pos, z, 0, 0, 1);

			KillItem(item_number);
		}
	}
}

void DartEmitterControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* dart;
	long x, z, xLimit, zLimit, xv, zv, rnd;
	short num;

	item = &items[item_number];

	if (item->active)
	{
		if (item->timer > 0)
		{
			item->timer--;
			return;
		}

		item->timer = 24;
	}

	num = CreateItem();

	if (num == NO_ITEM)
		return;

	x = 0;
	z = 0;
	dart = &items[num];
	dart->object_number = DARTS;
	dart->room_number = item->room_number;

	if (!item->pos.y_rot)
		z = 512;
	else if (item->pos.y_rot == 0x4000)
		x = 512;
	else if (item->pos.y_rot == -0x4000)
		x = -512;
	else if (item->pos.y_rot == -0x8000)
		z = -512;

	dart->pos.x_pos = item->pos.x_pos + x;
	dart->pos.y_pos = item->pos.y_pos - 512;
	dart->pos.z_pos = item->pos.z_pos + z;
	InitialiseItem(num);
	dart->pos.x_rot = 0;
	dart->pos.y_rot = item->pos.y_rot + 0x8000;
	dart->speed = 256;
	xLimit = 0;
	zLimit = 0;

	if (x)
		xLimit = abs(x << 1) - 1;
	else
		zLimit = abs(z << 1) - 1;

	for (int i = 0; i < 5; i++)
	{
		rnd = -GetRandomControl();

		if (z >= 0)
			zv = zLimit & rnd;
		else
			zv = -(zLimit & rnd);

		if (x >= 0)
			xv = xLimit & rnd;
		else
			xv = -(xLimit & rnd);

		TriggerDartSmoke(dart->pos.x_pos, dart->pos.y_pos, dart->pos.z_pos, xv, zv, 0);
	}

	AddActiveItem(num);
	dart->status = ITEM_ACTIVE;
	SoundEffect(SFX_BLOWPIPE_NATIVE_BLOW, &dart->pos, SFX_DEFAULT);
}

void FallingCeiling(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	short room_number;

	item = &items[item_number];

	if (!item->current_anim_state)
	{
		item->gravity_status = 1;
		item->goal_anim_state = 1;
	}
	else if (item->current_anim_state == 1 && item->touch_bits)
	{
		lara_item->hit_points -= 300;
		lara_item->hit_status = 1;
	}

	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
		RemoveActiveItem(item_number);
	else
	{
		room_number = item->room_number;
		floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);

		if (item->current_anim_state == 1 && item->pos.y_pos >= item->floor)
		{
			item->pos.y_pos = item->floor;
			item->goal_anim_state = 2;
			item->gravity_status = 0;
			item->fallspeed = 0;
		}
	}
}

void TeethTrap(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item))
	{
		item->goal_anim_state = 1;

		if (item->touch_bits && item->current_anim_state == 1)
		{
			lara_item->hit_points -= 400;
			lara_item->hit_status = 1;
			BaddieBiteEffect(item, &teeth1a);
			BaddieBiteEffect(item, &teeth1b);
			BaddieBiteEffect(item, &teeth2a);
			BaddieBiteEffect(item, &teeth2b);
			BaddieBiteEffect(item, &teeth3a);
			BaddieBiteEffect(item, &teeth3b);
		}
	}
	else
		item->goal_anim_state = 0;

	AnimateItem(item);
}

void FallingBlockCeiling(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long dy;

	dy = item->pos.y_pos - (item->object_number == FALLING_PLANK ? 1024 : 512);

	if (y > dy)
	{
		if (!item->current_anim_state || item->current_anim_state == 1)
			*h = dy + 256;
	}
}

void FallingBlockFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long dy;

	dy = item->pos.y_pos - (item->object_number == FALLING_PLANK ? 1024 : 512);

	if (y <= dy)
	{
		if (!item->current_anim_state || item->current_anim_state == 1)
		{
			*h = dy;
			OnObject = 1;
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
	INJECT(0x0046D500, ControlSpikeWall, replace);
	INJECT(0x0046D650, ControlCeilingSpikes, replace);
	INJECT(0x0046FBD0, TriggerPendulumFlame, replace);
	INJECT(0x0046EF40, SideFlameEmitterControl, replace);
	INJECT(0x0046EFF0, FlameEmitterControl, replace);
	INJECT(0x0046F090, FlameEmitter2Control, replace);
	INJECT(0x0046F130, FlameEmitter3Control, replace);
	INJECT(0x0046EDD0, DartsControl, replace);
	INJECT(0x0046EC10, DartEmitterControl, replace);
	INJECT(0x0046EB20, FallingCeiling, replace);
	INJECT(0x0046EA50, TeethTrap, replace);
	INJECT(0x0046EA00, FallingBlockCeiling, replace);
	INJECT(0x0046E9B0, FallingBlockFloor, replace);
}
