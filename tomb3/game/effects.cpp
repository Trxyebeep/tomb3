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
#include "hair.h"
#include "target.h"
#include "invfunc.h"
#include "../specific/specific.h"
#include "sphere.h"
#include "footprnt.h"
#include "camera.h"
#include "savegame.h"
#include "../newstuff/LaraDraw.h"

void(*effect_routines[])(ITEM_INFO* item) =
{
	turn180_effect,
	floor_shake_effect,
	lara_normal_effect,
	LaraBubbles,
	finish_level_effect,
	FloodFX,
	ChandelierFX,
	RubbleFX,
	PistonFX,
	CurtainFX,
	SetChangeFX,
	ExplosionFX,
	lara_hands_free,
	flip_map_effect,
	draw_right_gun,
	draw_left_gun,
	shoot_right_gun,
	shoot_left_gun,
	swap_meshes_with_meshswap1,
	swap_meshes_with_meshswap2,
	swap_meshes_with_meshswap3,
	invisibility_on,
	invisibility_off,
	dynamic_light_on,
	dynamic_light_off,
	StatueFX,
	reset_hair,
	BoilerFX,
	AssaultReset,
	AssaultStop,
	AssaultStart,
	AssaultFinished,
	AddFootprint,
	AssaultPenalty4,
	QuadStart,
	QuadStop,
	QuadFinished,
	AssaultPenalty30,
	Exercise1_105,
	Exercise1_104,
	Exercise2_112,
	Exercise2_83,
	Exercise3_115,
	Exercise3_107,
	Exercise3_79,
	Exercise4_110,
	Exercise4_111,
	Exercise5_116,
	Exercise5_95,
	Exercise6_113,
	Exercise6_114,
	Exercise6_80,
	Exercise7_106,
	Exercise7_86,
	Exercise8_93,
	Exercise8_109Sp,
	Exercise8_87,
	ResetExercises,
	TubeTrain,
	RumbleNoShake
};

FX_INFO* effects;
OBJECT_VECTOR* sound_effects;
long number_sound_effects;

static long ExerciseNumber;

long QuadbikeLapTime;
long QuadbikeLapTimeDisplayTimer;
long assault_timer_display;
long assault_penalty_display_timer;
long assault_timer_active;
long assault_target_penalties;
long assault_penalties;
long assault_targets;

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
	GetLaraMeshPos(&p, LMX_HEAD);

	v.x = (GetRandomControl() & 7) - 4;
	v.y = (GetRandomControl() & 7) - 8;
	v.z = (GetRandomControl() & 0x7F) + 64;
	GetLaraMeshPos(&v, LMX_HEAD);

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
	GetLaraMeshPos(&pos, LMX_HEAD);

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

	S_CalculateLight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, &item->il);
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

	if (abs(x) < 0x4000 && abs(y) < 0x4000 && abs(z) < 0x4000)
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

void StatueFX(ITEM_INFO* item)
{
	SoundEffect(SFX_BOO_MUTE, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void SetChangeFX(ITEM_INFO* item)
{
	SoundEffect(SFX_TONK, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void ControlDingDong(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS)
	{
		SoundEffect(SFX_DOORBELL, &item->pos, SFX_DEFAULT);
		item->flags -= 0x3E00;
	}
}

void ControlLaraAlarm(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS)
		SoundEffect(SFX_BURGLAR_ALARM, &item->pos, SFX_DEFAULT);
}

void ControlBirdTweeter(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number == BIRD_TWEETER)
	{
		if (GetRandomControl() < 1024)
			SoundEffect(SFX_WALL_BLADES, &item->pos, SFX_DEFAULT);
	}
	else if (GetRandomControl() < 256)
		SoundEffect(SFX_DRIPS_REVERB, &item->pos, SFX_DEFAULT);
}

void DoChimeSound(ITEM_INFO* item)
{
	PHD_3DPOS pos;

	pos.x_pos = lara_item->pos.x_pos + ((item->pos.x_pos - lara_item->pos.x_pos) >> 6);
	pos.y_pos = lara_item->pos.y_pos + ((item->pos.y_pos - lara_item->pos.y_pos) >> 6);
	pos.z_pos = lara_item->pos.z_pos + ((item->pos.z_pos - lara_item->pos.z_pos) >> 6);
	SoundEffect(SFX_ALARM_1, &pos, SFX_DEFAULT);
}

void ControlClockChimes(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->timer)
	{
		if (item->timer % 60 == 59)
			DoChimeSound(item);

		item->timer--;

		if (!item->timer)
		{
			DoChimeSound(item);
			item->timer = -1;
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;
			item->flags &= ~IFL_CODEBITS;
		}
	}
}

void lara_hands_free(ITEM_INFO* item)
{
	lara.gun_status = LG_ARMLESS;
}

void flip_map_effect(ITEM_INFO* item)
{
	FlipMap();
}

void draw_right_gun(ITEM_INFO* item)
{
	short* tmp;

	tmp = lara.mesh_ptrs[THIGH_R];
	lara.mesh_ptrs[THIGH_R] = meshes[objects[PISTOLS].mesh_index + THIGH_R];
	meshes[objects[PISTOLS].mesh_index + THIGH_R] = tmp;

	tmp = lara.mesh_ptrs[HAND_R];
	lara.mesh_ptrs[HAND_R] = meshes[objects[PISTOLS].mesh_index + HAND_R];
	meshes[objects[PISTOLS].mesh_index + HAND_R] = tmp;
}

void draw_left_gun(ITEM_INFO* item)
{
	short* tmp;

	tmp = lara.mesh_ptrs[THIGH_L];
	lara.mesh_ptrs[THIGH_L] = meshes[objects[PISTOLS].mesh_index + THIGH_L];
	meshes[objects[PISTOLS].mesh_index + THIGH_L] = tmp;

	tmp = lara.mesh_ptrs[HAND_L];
	lara.mesh_ptrs[HAND_L] = meshes[objects[PISTOLS].mesh_index + HAND_L];
	meshes[objects[PISTOLS].mesh_index + HAND_L] = tmp;
}

void shoot_right_gun(ITEM_INFO* item)
{
	lara.right_arm.flash_gun = 3;
}

void shoot_left_gun(ITEM_INFO* item)
{
	lara.left_arm.flash_gun = 3;
}

void swap_meshes_with_meshswap1(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short* tmp;

	obj = &objects[item->object_number];

	for (int i = 0; i < obj->nmeshes; i++)
	{
		tmp = meshes[obj->mesh_index + i];
		meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP1].mesh_index + i];
		meshes[objects[MESHSWAP1].mesh_index + i] = tmp;
	}
}

void swap_meshes_with_meshswap2(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short* tmp;

	obj = &objects[item->object_number];

	for (int i = 0; i < obj->nmeshes; i++)
	{
		tmp = meshes[obj->mesh_index + i];
		meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP2].mesh_index + i];
		meshes[objects[MESHSWAP2].mesh_index + i] = tmp;
	}
}

void swap_meshes_with_meshswap3(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short* tmp;

	obj = &objects[item->object_number];

	for (int i = 0; i < obj->nmeshes; i++)
	{
		tmp = meshes[obj->mesh_index + i];
		meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP3].mesh_index + i];

		if (item == lara_item)
			lara.mesh_ptrs[i] = meshes[objects[MESHSWAP3].mesh_index + i];

		meshes[objects[MESHSWAP3].mesh_index + i] = tmp;
	}
}

void invisibility_on(ITEM_INFO* item)
{
	item->status = ITEM_INVISIBLE;
}

void invisibility_off(ITEM_INFO* item)
{
	item->status = ITEM_ACTIVE;
}

void dynamic_light_on(ITEM_INFO* item)
{
	item->dynamic_light = 1;
}

void dynamic_light_off(ITEM_INFO* item)
{
	item->dynamic_light = 0;
}

void reset_hair(ITEM_INFO* item)
{
	InitialiseHair();
}

void AssaultStart(ITEM_INFO* item)
{
	savegame.timer = 0;
	assault_timer_active = 1;
	assault_timer_display = 1;
	assault_penalties = 0;
	assault_penalty_display_timer = 0;
	assault_targets = 7;
	ResetTargets();
	flipeffect = -1;
}

void AssaultStop(ITEM_INFO* item)
{
	assault_timer_active = 0;
	assault_timer_display = 1;
	flipeffect = -1;
}

void AssaultReset(ITEM_INFO* item)
{
	assault_timer_active = 0;
	assault_timer_display = 0;
	assault_penalties = 0;
	assault_penalty_display_timer = 0;
	flipeffect = -1;
}

void AssaultPenalty4(ITEM_INFO* item)
{
	if (assault_timer_active)
	{
		assault_penalty_display_timer = 120;
		assault_penalties += 240;	//actually 8 seconds lol
	}

	flipeffect = -1;
}

void AssaultPenalty30(ITEM_INFO* item)
{
	if (assault_timer_active)
	{
		assault_penalty_display_timer = 120;
		assault_penalties += 900;
	}

	flipeffect = -1;
}

void AssaultFinished(ITEM_INFO* item)
{
	if (assault_timer_active)
	{
		assault_penalty_display_timer = 300;
		assault_target_penalties = 300 * assault_targets;	//10 seconds for each target
		savegame.timer += assault_target_penalties + assault_penalties;

		if (savegame.timer > 0x1A5DD)
			savegame.timer = 0x1A5DD;

		AddAssaultTime(savegame.timer);
		assault_timer_active = 0;

		if (savegame.timer < 5400)
			S_CDPlay(95, 0);
	}

	flipeffect = -1;
}

void QuadStart(ITEM_INFO* item)
{
	if (lara.skidoo != NO_ITEM)
	{
		savegame.timer = 0;
		assault_timer_active = 1;
		assault_timer_display = 1;
	}

	flipeffect = -1;
}

void QuadStop(ITEM_INFO* item)
{
	if (lara.skidoo != NO_ITEM)
	{
		assault_timer_active = 0;
		assault_timer_display = 1;
	}

	flipeffect = -1;
}

void QuadFinished(ITEM_INFO* item)
{
	if (lara.skidoo != NO_ITEM && assault_timer_active)
	{
		if (savegame.timer > 0x1A5DD)
			savegame.timer = 0x1A5DD;

		QuadbikeLapTime = savegame.timer;
		QuadbikeLapTimeDisplayTimer = 300;
		AddQuadbikeTime(savegame.timer);
		assault_timer_active = 0;
	}

	flipeffect = -1;
}

void Exercise1_105(ITEM_INFO* item)
{
	if (!ExerciseNumber)
	{
		S_CDPlay(108, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise1_104(ITEM_INFO* item)
{
	if (ExerciseNumber == 1)
	{
		S_CDPlay(107, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise2_112(ITEM_INFO* item)
{
	if (ExerciseNumber == 2)
	{
		S_CDPlay(115, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise2_83(ITEM_INFO* item)
{
	if (ExerciseNumber == 3)
	{
		S_CDPlay(86, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise3_115(ITEM_INFO* item)
{
	if (ExerciseNumber == 4)
	{
		S_CDPlay(118, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise3_107(ITEM_INFO* item)
{
	if (ExerciseNumber == 5)
	{
		S_CDPlay(110, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise3_79(ITEM_INFO* item)
{
	if (ExerciseNumber == 6)
	{
		S_CDPlay(82, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise4_110(ITEM_INFO* item)
{
	if (ExerciseNumber == 7)
	{
		S_CDPlay(113, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise4_111(ITEM_INFO* item)
{
	if (ExerciseNumber == 8)
	{
		S_CDPlay(114, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise5_116(ITEM_INFO* item)
{
	if (ExerciseNumber == 9)
	{
		S_CDPlay(119, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise5_95(ITEM_INFO* item)
{
	if (ExerciseNumber == 10)
	{
		S_CDPlay(98, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise6_113(ITEM_INFO* item)
{
	if (ExerciseNumber == 11)
	{
		S_CDPlay(116, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise6_114(ITEM_INFO* item)
{
	if (ExerciseNumber == 12)
	{
		S_CDPlay(117, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise6_80(ITEM_INFO* item)
{
	if (ExerciseNumber == 13)
	{
		S_CDPlay(83, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise7_106(ITEM_INFO* item)
{
	if (ExerciseNumber == 14)
	{
		S_CDPlay(109, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise7_86(ITEM_INFO* item)
{
	if (ExerciseNumber == 15)
	{
		S_CDPlay(89, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise8_93(ITEM_INFO* item)
{
	if (ExerciseNumber == 16)
	{
		S_CDPlay(96, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise8_109Sp(ITEM_INFO* item)
{
	if (ExerciseNumber == 17 && lara.water_status == LARA_SURFACE)
	{
		S_CDPlay(112, 0);
		ExerciseNumber++;
	}

	flipeffect = -1;
}

void Exercise8_87(ITEM_INFO* item)
{
	if (ExerciseNumber == 18)
	{
		S_CDPlay(90, 0);
		ExerciseNumber = 0;
	}

	flipeffect = -1;
}

void ResetExercises(ITEM_INFO* item)
{
	ExerciseNumber = 0;
	flipeffect = -1;
}

void TubeTrain(ITEM_INFO* item)
{
	camera.bounce = -350;
	flipeffect = -1;
}

void RumbleNoShake(ITEM_INFO* item)
{
	SoundEffect(SFX_RUMBLE_NEXTDOOR, 0, SFX_DEFAULT);
	flipeffect = -1;
}

void BaddieBiteEffect(ITEM_INFO* item, BITE_INFO* bite)
{
	PHD_VECTOR pos;

	pos.x = bite->x;
	pos.y = bite->y;
	pos.z = bite->z;
	GetJointAbsPosition(item, &pos, bite->mesh_num);
	DoBloodSplat(pos.x, pos.y, pos.z, item->speed, item->pos.y_rot, item->room_number);
}

short DoBloodSplat(long x, long y, long z, short speed, short ang, short room_number)
{
	if (room[room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterBlood(x, y, z, GetRandomControl() & 7);
	else
		TriggerBlood(x, y, z, ang >> 4, (GetRandomControl() & 7) + 6);

	return -1;
}

short DoBloodSplatD(long x, long y, long z, short speed, short ang, short room_number)
{
	if (room[room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterBloodD(x, y + 64, z, GetRandomDraw() & 7);
	else
		TriggerBloodD(x, y, z, ang >> 4, (GetRandomDraw() & 7) + 6);

	return -1;
}

void DoLotsOfBlood(long x, long y, long z, short speed, short direction, short room_num, long num)
{
	for (; num > 0; num--)
		DoBloodSplat(x - (GetRandomControl() << 9) / 0x8000 + 256, y - (GetRandomControl() << 9) / 0x8000 + 256,
			z - (GetRandomControl() << 9) / 0x8000 + 256, speed, direction, room_num);
}

void DoLotsOfBloodD(long x, long y, long z, short speed, short direction, short room_num, long num)
{
	for (; num > 0; num--)
		DoBloodSplatD(x - (GetRandomDraw() << 9) / 0x8000 + 256, y, z - (GetRandomDraw() << 9) / 0x8000 + 256, speed, direction, room_num);
}
