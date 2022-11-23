#include "../tomb3/pch.h"
#include "effects.h"
#include "lara.h"
#include "../specific/game.h"
#include "effect2.h"
#include "draw.h"
#include "sound.h"
#include "control.h"
#include "items.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "../specific/litesrc.h"
#include "../3dsystem/3d_gen.h"

void LaraBreath(ITEM_INFO* item)
{
	PHD_VECTOR p;
	PHD_VECTOR v;

	if (lara.water_status == LARA_UNDERWATER || lara_item->hit_points < 0)
		return;

	if (lara_item->current_anim_state == AS_STOP)
	{
		if (lara_item->frame_number < anims[ANIM_BREATH].frame_base + 30)
			return;
	}
	else if (lara_item->current_anim_state == AS_DUCK)
	{
		if (lara_item->frame_number < anims[ANIM_DUCKBREATHE].frame_base + 30)
			return;
	}
	else if (wibble < 128 || wibble > 192)
		return;

	p.x = 0;
	p.y = -4;
	p.z = 64;
	GetLaraHandAbsPosition(&p, LARA_HEAD);

	v.x = (GetRandomControl() & 7) - 4;
	v.y = (GetRandomControl() & 7) - 8;
	v.z = (GetRandomControl() & 0x7F) + 64;
	GetLaraHandAbsPosition(&v, LARA_HEAD);

	TriggerBreath(p.x, p.y, p.z, v.x - p.x, v.y - p.y, v.z - p.z);
}

long ItemNearLara(PHD_3DPOS* pos, long rad)
{
	short* bounds;
	long dx, dy, dz;

	dx = pos->x_pos - lara_item->pos.x_pos;
	dy = pos->y_pos - lara_item->pos.y_pos;
	dz = pos->z_pos - lara_item->pos.z_pos;

	if (dx >= -rad && dx <= rad && dz >= -rad && dz <= rad && dy >= -3072 && dy <= 3072 && SQUARE(dx) + SQUARE(dz) <= SQUARE(rad))
	{
		bounds = GetBoundsAccurate(lara_item);

		if (dy >= bounds[2] && dy <= bounds[3] + 100)
			return 1;
	}

	return 0;
}

void SoundEffects()
{
	OBJECT_VECTOR* sfx;

	for (int i = 0; i < number_sound_effects; i++)
	{
		sfx = &sound_effects[i];

		if (flip_status)
		{
			if (sfx->flags & 0x40)
				SoundEffect(sfx->data, (PHD_3DPOS*)sfx, 0);
		}
		else if (sfx->flags & 0x80)
			SoundEffect(sfx->data, (PHD_3DPOS*)sfx, 0);
	}

	if (flipeffect != -1)
		effect_routines[flipeffect](0);

	SOUND_EndScene();
}

void Richochet(GAME_VECTOR* pos)
{
	TriggerRicochetSpark(pos, (mGetAngle(pos->z, pos->x, lara_item->pos.z_pos, lara_item->pos.x_pos) >> 4) & 0xFFF, 16);
	SoundEffect(SFX_LARA_RICOCHET, (PHD_3DPOS*)pos, SFX_DEFAULT);
}

void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long sizerange)
{
	FX_INFO* fx;
	short rn, fxnum;

	rn = room_number;
	GetFloor(pos->x_pos, pos->y_pos, pos->z_pos, &room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
	{
		fxnum = CreateEffect(rn);

		if (fxnum != NO_ITEM)
		{
			fx = &effects[fxnum];
			fx->pos.x_pos = pos->x_pos;
			fx->pos.y_pos = pos->y_pos;
			fx->pos.z_pos = pos->z_pos;
			fx->speed = (GetRandomControl() & 0xFF) + 64;
			fx->object_number = BUBBLES1;
			fx->flag1 = (GetRandomControl() & 0x1F) + 32;
			fx->flag2 = 0;
			fx->frame_number = 0;
			TriggerBubble(pos->x_pos, pos->y_pos, pos->z_pos, size, sizerange, fxnum);
		}
	}
}

void LaraBubbles(ITEM_INFO* item)
{
	PHD_VECTOR pos;

	SoundEffect(SFX_LARA_BUBBLES, &item->pos, SFX_WATER);
	pos.x = 0;
	pos.y = -4;
	pos.z = 64;
	GetLaraHandAbsPosition(&pos, LARA_HEAD);

	for (int i = (GetRandomControl() & 3) + 2; i > 0; i--)
		CreateBubble((PHD_3DPOS*)&pos, item->room_number, 8, 8);
}

void ControlBubble1(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short room_number;

	fx = &effects[fx_number];
	fx->pos.y_rot += 1638;
	fx->pos.x_rot += 2366;
	fx->speed += fx->flag1;
	x = fx->pos.x_pos + ((3 * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT);
	y = fx->pos.y_pos - (fx->speed >> 8);
	z = fx->pos.z_pos + (phd_cos(fx->pos.x_rot) >> W2V_SHIFT);
	room_number = fx->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (y > h || !floor)
	{
		KillEffect(fx_number);
		return;
	}

	if (!(room[room_number].flags & ROOM_UNDERWATER))
	{
		SetupRipple(fx->pos.x_pos, room[fx->room_number].maxceiling, fx->pos.z_pos, -2 - (GetRandomControl() & 1), 1);
		KillEffect(fx_number);
		return;
	}

	c = GetCeiling(floor, x, y, z);

	if (c == NO_HEIGHT || y <= c)
	{
		KillEffect(fx_number);
		return;
	}

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, room_number);

	fx->pos.x_pos = x;
	fx->pos.y_pos = y;
	fx->pos.z_pos = z;
}

void Splash(ITEM_INFO* item)
{
	short room_number;

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
	{
		splash_setup.x = item->pos.x_pos;
		splash_setup.y = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);
		splash_setup.z = item->pos.z_pos;
		splash_setup.InnerXZoff = 32;
		splash_setup.InnerXZsize = 8;
		splash_setup.InnerYsize = -128;
		splash_setup.InnerXZvel = 320;
		splash_setup.InnerYvel = -40 * item->fallspeed;
		splash_setup.InnerGravity = 160;
		splash_setup.InnerFriction = 7;
		splash_setup.MiddleXZoff = 48;
		splash_setup.MiddleXZsize = 32;
		splash_setup.MiddleYsize = -64;
		splash_setup.MiddleXZvel = 480;
		splash_setup.MiddleYvel = -20 * item->fallspeed;
		splash_setup.MiddleGravity = 96;
		splash_setup.MiddleFriction = 8;
		splash_setup.OuterXZoff = 32;
		splash_setup.OuterXZsize = 128;
		splash_setup.OuterXZvel = 544;
		splash_setup.OuterFriction = 9;
		SetupSplash(&splash_setup);
	}
}

void WadeSplash(ITEM_INFO* item, long water, long depth)
{
	short* bounds;
	short room_number;

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (!(room[room_number].flags & ROOM_UNDERWATER))
		return;

	bounds = GetBestFrame(item);

	if (item->pos.y_pos + bounds[2] > water || item->pos.y_pos + bounds[3] < water)
		return;

	if (item->fallspeed > 0 && depth < 474 && !SplashCount)
	{
		splash_setup.x = item->pos.x_pos;
		splash_setup.y = water;
		splash_setup.z = item->pos.z_pos;
		splash_setup.InnerXZoff = 16;
		splash_setup.InnerXZsize = 12;
		splash_setup.InnerYsize = -96;
		splash_setup.InnerXZvel = 160;
		splash_setup.InnerGravity = 128;
		splash_setup.InnerYvel = -72 * item->fallspeed;
		splash_setup.InnerFriction = 7;
		splash_setup.MiddleXZoff = 24;
		splash_setup.MiddleXZsize = 24;
		splash_setup.MiddleYsize = -64;
		splash_setup.MiddleXZvel = 224;
		splash_setup.MiddleYvel = -36 * item->fallspeed;
		splash_setup.MiddleGravity = 72;
		splash_setup.MiddleFriction = 8;
		splash_setup.OuterXZoff = 32;
		splash_setup.OuterXZsize = 32;
		splash_setup.OuterXZvel = 272;
		splash_setup.OuterFriction = 9;
		SetupSplash(&splash_setup);
		SplashCount = 16;
	}
	else if (!(wibble & 0xF) && (!(GetRandomControl() & 0xF) || item->current_anim_state != AS_STOP))
		SetupRipple(item->pos.x_pos, water, item->pos.z_pos, -16 - (GetRandomControl() & 0xF), item->current_anim_state == AS_STOP);
}

void WaterFall(short item_number)
{
	ITEM_INFO* item;
	long dx, dy, dz, ang, s, c;

	item = &items[item_number];
	dx = item->pos.x_pos - lara_item->pos.x_pos;
	dy = item->pos.y_pos - lara_item->pos.y_pos;
	dz = item->pos.z_pos - lara_item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dy < -0x4000 || dy > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	S_CalculateLight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, &item->il);	//why
	ang = (item->pos.y_rot >> 3) & 0x1FFE;
	s = (544 * rcossin_tbl[ang]) >> 12;
	c = (544 * rcossin_tbl[ang + 1]) >> 12;

	if (!(wibble & 0xC))
		TriggerWaterfallMist(item->pos.x_pos + s, item->pos.y_pos, item->pos.z_pos + c, item->pos.y_rot >> 4);

	SoundEffect(SFX_WATERFALL_LOOP, &item->pos, SFX_DEFAULT);
}

void finish_level_effect(ITEM_INFO* item)
{
	level_complete = 1;
}

void turn180_effect(ITEM_INFO* item)
{
	item->pos.y_rot += 0x8000;
	item->pos.x_rot = -item->pos.x_rot;
}

void floor_shake_effect(ITEM_INFO* item)
{
	long x, y, z;

	x = item->pos.x_pos - camera.pos.x;
	y = item->pos.y_pos - camera.pos.y;
	z = item->pos.z_pos - camera.pos.z;

	if ((abs(x) < 0x4000) && (abs(y) < 0x4000) && (abs(z) < 0x4000))
		camera.bounce = 100 * ((SQUARE(x) + SQUARE(y) + SQUARE(z)) / 256 - 0x100000) / 0x100000;
}

void lara_normal_effect(ITEM_INFO* item)
{
	item->anim_number = ANIM_STOP;
	item->frame_number = anims[ANIM_STOP].frame_base;
	item->current_anim_state = AS_STOP;
	item->goal_anim_state = AS_STOP;
	camera.type = CHASE_CAMERA;
	AlterFOV(14560);
}

void BoilerFX(ITEM_INFO* item)
{
	SoundEffect(SFX_WILARD_FOOT_STEPS, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void FloodFX(ITEM_INFO* item)
{
	PHD_3DPOS pos;

	if (fliptimer > 120)
		flipeffect = -1;
	else
	{
		pos.x_pos = lara_item->pos.x_pos;

		if (fliptimer >= 30)
			pos.y_pos = camera.target.y + 100 * (fliptimer - 30);
		else
			pos.y_pos = camera.target.y + 100 * (30 - fliptimer);

		pos.z_pos = lara_item->pos.z_pos;
		SoundEffect(SFX_RESERVOIR_FLUSH, &pos, SFX_DEFAULT);
	}

	fliptimer++;
}

void RubbleFX(ITEM_INFO* item)
{
	SoundEffect(SFX_LARA_KNEES_SHUFFLE, 0, SFX_DEFAULT);
	camera.bounce = -350;
	flipeffect = -1;
}

void ChandelierFX(ITEM_INFO* item)
{
	SoundEffect(SFX_SMALL_DOOR_SUBWAY, 0, SFX_DEFAULT);
	fliptimer++;

	if (fliptimer > 30)
		flipeffect = -1;
}

void ExplosionFX(ITEM_INFO* item)
{
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	camera.bounce = -75;
	flipeffect = -1;
}

void PistonFX(ITEM_INFO* item)
{
	SoundEffect(SFX_LONDON_MERCENARY_DEATH, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void CurtainFX(ITEM_INFO* item)
{
	SoundEffect(SFX_CLEANER_LOOP, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void inject_effects(bool replace)
{
	INJECT(0x0042E630, LaraBreath, replace);
	INJECT(0x0042E170, ItemNearLara, replace);
	INJECT(0x0042E200, SoundEffects, replace);
	INJECT(0x0042E270, Richochet, replace);
	INJECT(0x0042E4F0, CreateBubble, replace);
	INJECT(0x0042E5C0, LaraBubbles, replace);
	INJECT(0x0042E750, ControlBubble1, replace);
	INJECT(0x0042E8C0, Splash, replace);
	INJECT(0x0042E9F0, WadeSplash, replace);
	INJECT(0x0042EBB0, WaterFall, replace);
	INJECT(0x0042ECD0, finish_level_effect, replace);
	INJECT(0x0042ECE0, turn180_effect, replace);
	INJECT(0x0042ED00, floor_shake_effect, replace);
	INJECT(0x0042EDA0, lara_normal_effect, replace);
	INJECT(0x0042EDE0, BoilerFX, replace);
	INJECT(0x0042EE00, FloodFX, replace);
	INJECT(0x0042EEA0, RubbleFX, replace);
	INJECT(0x0042EED0, ChandelierFX, replace);
	INJECT(0x0042EF00, ExplosionFX, replace);
	INJECT(0x0042EF30, PistonFX, replace);
	INJECT(0x0042EF50, CurtainFX, replace);
}
