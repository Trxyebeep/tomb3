#include "../tomb3/pch.h"
#include "missile.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "sound.h"
#include "effect2.h"
#include "items.h"
#include "effects.h"
#include "traps.h"
#include "../specific/game.h"

void ControlMissile(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long h, c;
	short room_number;

	fx = &effects[fx_number];

	if (fx->object_number == DIVER_HARPOON && !(room[fx->room_number].flags & ROOM_UNDERWATER))
	{
		if (fx->pos.x_rot > -0x3000)
			fx->pos.x_rot -= 182;
	}

	fx->pos.x_pos += (((fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT) * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += (fx->speed * phd_sin(-fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.z_pos += (((fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT) * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos <= c)
	{
		if (fx->object_number == KNIFE || fx->object_number == DIVER_HARPOON)
			SoundEffect(fx->object_number == DIVER_HARPOON ? SFX_LARA_RICOCHET : SFX_SHIVA_SWORD_1, &fx->pos, SFX_DEFAULT);
		else if (fx->object_number == DRAGON_FIRE)
		{
			TriggerFireFlame(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, -1, 0);
			TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, 24, 31, 24, GetRandomControl() & 7);
#ifndef TROYESTUFF
			KillEffect(fx_number);
#endif
		}

#ifdef TROYESTUFF
		KillEffect(fx_number);
#endif
		return;
	}

	if (room_number != fx->room_number)
		EffectNewRoom(fx_number, room_number);

	if (fx->object_number == DRAGON_FIRE)
	{
		if (ItemNearLara(&fx->pos, 350))
		{
			lara_item->hit_points -= 3;
			lara_item->hit_status = 1;
			LaraBurn();
			return;
		}
	}
	else if (ItemNearLara(&fx->pos, 200))
	{
		if (fx->object_number == KNIFE || fx->object_number == DIVER_HARPOON)
		{
			lara_item->hit_points -= 50;
			SoundEffect(SFX_MACAQUE_CHATTER, &fx->pos, SFX_DEFAULT);
			KillEffect(fx_number);
		}

		lara_item->hit_status = 1;
		fx->pos.y_rot = lara_item->pos.y_rot;
		fx->speed = lara_item->speed;
		fx->counter = 0;
		fx->frame_number = 0;
	}

	if (fx->object_number == DIVER_HARPOON && room[fx->room_number].flags & ROOM_UNDERWATER)
	{
		if (!(wibble & 0xF))
			CreateBubble(&fx->pos, room_number, 8, 8);

		TriggerRocketSmoke(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, 64);
	}
	else if (fx->object_number == DRAGON_FIRE && !fx->counter--)
	{
		TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, 24, 31, 24, GetRandomControl() & 7);
		SoundEffect(SFX_SECURITY_GUARD_FIRE, &fx->pos, 0);
		KillEffect(fx_number);
	}
	else if (fx->object_number == KNIFE)
		fx->pos.z_rot += 5460;
}

void inject_missile(bool replace)
{
	INJECT(0x00454FB0, ControlMissile, replace);
}
