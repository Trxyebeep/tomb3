#include "../tomb3/pch.h"
#include "quadbike.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "../specific/init.h"
#include "control.h"
#include "../3dsystem/phd_math.h"
#include "collide.h"
#include "laraflar.h"
#include "objects.h"
#include "gameflow.h"
#include "../specific/game.h"
#include "../specific/specific.h"
#include "lara1gun.h"
#include "effect2.h"
#include "missile.h"
#include "items.h"
#include "sound.h"
#include "laraanim.h"
#include "lara.h"

void QuadBikeDraw(ITEM_INFO* item)
{
	QUADINFO* quad;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* rot;
#ifdef TROYESTUFF
	short* rot2;
#endif
	long frac, rate, clip;

	frac = GetFrames(item, frm, &rate);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frm[0]);

	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	quad = (QUADINFO*)item->data;
	meshpp = &meshes[objects[item->object_number].mesh_index];
	bone = &bones[objects[item->object_number].bone_index];
	CalculateObjectLighting(item, frm[0]);

#ifdef TROYESTUFF
	if (frac)
	{
		InitInterpolate(frac, rate);
		phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
		rot = frm[0] + 9;
		rot2 = frm[1] + 9;
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PutPolygons_I(*meshpp++, clip);

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PutPolygons_I(*meshpp++, clip);
		phd_PopMatrix_I();

		phd_PushMatrix_I();

		phd_TranslateRel_I(bone[5], bone[6], bone[7]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PutPolygons_I(*meshpp++, clip);

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[9], bone[10], bone[11]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_RotX_I(quad->RearRot);
		phd_PutPolygons_I(*meshpp++, clip);
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[13], bone[14], bone[15]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_RotX_I(quad->RearRot);
		phd_PutPolygons_I(*meshpp++, clip);
		phd_PopMatrix_I();

		phd_PopMatrix_I();

		phd_PushMatrix_I();

		phd_TranslateRel_I(bone[17], bone[18], bone[19]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PutPolygons_I(*meshpp++, clip);

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[21], bone[22], bone[23]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_RotX_I(quad->FrontRot);
		phd_PutPolygons_I(*meshpp++, clip);
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[25], bone[26], bone[27]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_RotX_I(quad->FrontRot);
		phd_PutPolygons_I(*meshpp++, clip);
		phd_PopMatrix_I();

		phd_PopMatrix_I();
	}
	else
#endif
	{
		phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
		rot = frm[0] + 9;
		gar_RotYXZsuperpack(&rot, 0);
		phd_PutPolygons(*meshpp++, clip);

		phd_PushMatrix();
		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_PutPolygons(*meshpp++, clip);
		phd_PopMatrix();

		phd_PushMatrix();

		phd_TranslateRel(bone[5], bone[6], bone[7]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_PutPolygons(*meshpp++, clip);

		phd_PushMatrix();
		phd_TranslateRel(bone[9], bone[10], bone[11]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_RotX(quad->RearRot);
		phd_PutPolygons(*meshpp++, clip);
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[13], bone[14], bone[15]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_RotX(quad->RearRot);
		phd_PutPolygons(*meshpp++, clip);
		phd_PopMatrix();

		phd_PopMatrix();

		phd_PushMatrix();

		phd_TranslateRel(bone[17], bone[18], bone[19]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_PutPolygons(*meshpp++, clip);

		phd_PushMatrix();
		phd_TranslateRel(bone[21], bone[22], bone[23]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_RotX(quad->FrontRot);
		phd_PutPolygons(*meshpp++, clip);
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[25], bone[26], bone[27]);
		gar_RotYXZsuperpack(&rot, 0);
		phd_RotX(quad->FrontRot);
		phd_PutPolygons(*meshpp++, clip);
		phd_PopMatrix();

		phd_PopMatrix();
	}

	phd_PopMatrix();
}

void InitialiseQuadBike(short item_number)
{
	ITEM_INFO* item;
	QUADINFO* quad;

	item = &items[item_number];
	quad = (QUADINFO*)game_malloc(sizeof(QUADINFO), 0);
	item->data = quad;
	quad->Velocity = 0;
	quad->skidoo_turn = 0;
	quad->right_fallspeed = 0;
	quad->left_fallspeed = 0;
	quad->momentum_angle = item->pos.y_rot;
	quad->extra_rotation = 0;
	quad->track_mesh = 0;
	quad->pitch = 0;
	quad->Flags = 0;
}

static long GetOnQuadBike(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long dx, dy, dz, dist, h;
	ushort uang;
	short room_number, ang;

	item = &items[item_number];

	if (!(input & IN_ACTION) || item->flags & IFL_INVISIBLE || lara.gun_status != LG_ARMLESS || lara_item->gravity_status)
		return 0;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dy = abs(item->pos.y_pos - lara_item->pos.y_pos);
	dz = lara_item->pos.z_pos - item->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dy > 256 || dist > 170000)
		return 0;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < -32000)
		return 0;

	ang = (short)phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot;
	uang = lara_item->pos.y_rot - item->pos.y_rot;

	if (ang > -0x1FFE && ang < 0x5FFA)
	{
		if (uang <= 0x1FFE || uang >= 0x5FFA)
			return 0;
	}
	else
	{
		if (uang <= 0x9FF6 || uang >= 0xDFF2)
			return 0;
	}

	return 1;
}

void QuadBikeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	QUADINFO* quad;
	short ang, track;
	static char tunes[4] = { 9, 12, 4, 12 };

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	if (!GetOnQuadBike(item_number, coll))
		return ObjectCollision(item_number, l, coll);

	lara.skidoo = item_number;

	if (lara.gun_type == LG_FLARE)
	{
		CreateFlare(0);
		undraw_flare_meshes();
		lara.flare_control_left = 0;
		lara.gun_type = LG_ARMLESS;
		lara.request_gun_type = LG_ARMLESS;
	}

	lara.gun_status = LG_HANDSBUSY;
	item = &items[item_number];
	ang = (short)phd_atan(item->pos.z_pos - l->pos.z_pos, item->pos.x_pos - l->pos.x_pos) - item->pos.y_rot;

	if (ang > -0x1FFE && ang < 0x5FFA)
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 23;
		l->current_anim_state = 23;
		l->goal_anim_state = 23;
	}
	else
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 9;
		l->current_anim_state = 9;
		l->goal_anim_state = 9;
	}

	l->frame_number = anims[l->anim_number].frame_base;
	item->hit_points = 1;
	l->pos.x_pos = item->pos.x_pos;
	l->pos.y_pos = item->pos.y_pos;
	l->pos.z_pos = item->pos.z_pos;
	l->pos.y_rot = item->pos.y_rot;
	lara.head_y_rot = 0;
	lara.head_x_rot = 0;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	lara.hit_direction = -1;
	AnimateItem(l);

	if (CurrentLevel == LV_QUADBIKE)
	{
		track = tunes[GetRandomControl() & 3];

		if (track != cdtrack && IsAtmospherePlaying)
		{
			cdtrack = track;
			S_CDPlay(track, 0);
		}
	}

	quad = (QUADINFO*)item->data;
	quad->Revs = 0;
}

static void QuadbikeExplode(ITEM_INFO* item)
{
	if (room[item->room_number].flags & ROOM_UNDERWATER)
		TriggerUnderwaterExplosion(item);
	else
	{
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);

		for (int i = 0; i < 3; i++)
			TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
	}

	ExplodingDeath(lara.skidoo, -2, 1);
	KillItem(lara.skidoo);
	item->status = ITEM_DEACTIVATED;
	SoundEffect(SFX_EXPLOSION1, 0, 0);
	SoundEffect(SFX_EXPLOSION2, 0, 0);
	lara.skidoo = NO_ITEM;
}

static long SkidooCheckGetOff()
{
	ITEM_INFO* item;
	QUADINFO* quad;
	PHD_VECTOR pos;

	item = &items[lara.skidoo];

	if ((lara_item->current_anim_state == 10 || lara_item->current_anim_state == 24) &&
		lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		if (lara_item->current_anim_state == 24)
			lara_item->pos.y_rot += 0x4000;
		else
			lara_item->pos.y_rot -= 0x4000;

		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = anims[ANIM_STOP].frame_base;
		lara_item->current_anim_state = 2;
		lara_item->goal_anim_state = 2;
		lara_item->pos.x_pos -= (512 * phd_sin(lara_item->pos.y_rot)) >> W2V_SHIFT;
		lara_item->pos.z_pos -= (512 * phd_cos(lara_item->pos.y_rot)) >> W2V_SHIFT;
		lara_item->pos.x_rot = 0;
		lara_item->pos.z_rot = 0;
		lara.skidoo = NO_ITEM;
		lara.gun_status = LG_ARMLESS;
	}
	else if (lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		quad = (QUADINFO*)item->data;

		if (lara_item->current_anim_state == 20)
		{
			lara_item->anim_number = ANIM_FASTFALL;
			lara_item->frame_number = anims[ANIM_FASTFALL].frame_base;
			lara_item->current_anim_state = AS_FASTFALL;
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetLaraHandAbsPosition(&pos, LARA_HIPS);
			lara_item->pos.x_pos = pos.x;
			lara_item->pos.y_pos = pos.y;
			lara_item->pos.z_pos = pos.z;
			lara_item->gravity_status = 1;
			lara_item->fallspeed = item->fallspeed;
			lara_item->pos.x_rot = 0;
			lara_item->pos.z_rot = 0;
			lara_item->hit_points = 0;
			lara.gun_status = LG_ARMLESS;
			item->flags |= IFL_INVISIBLE;
			return 0;
		}

		if (lara_item->current_anim_state == 19)
		{
			lara_item->goal_anim_state = 8;
			lara_item->fallspeed = 154;
			lara_item->speed = 0;
			quad->Flags |= 0x80;
			return 0;
		}
	}

	return 1;
}

static long TestHeight(ITEM_INFO* item, long x, long z, PHD_VECTOR* pos)
{
	FLOOR_INFO* floor;
	long s, c;
	short room_number;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	pos->x = item->pos.x_pos + ((z * c + x * s) >> W2V_SHIFT);
	pos->y = item->pos.y_pos + ((z * phd_sin(item->pos.z_rot)) >> W2V_SHIFT) - ((x * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
	pos->z = item->pos.z_pos + ((x * c - z * s) >> W2V_SHIFT);

	room_number = item->room_number;
	floor = GetFloor(pos->x, pos->y, pos->z, &room_number);
	c = GetCeiling(floor, pos->x, pos->y, pos->z);

	if (pos->y < c || c == NO_HEIGHT)
		return NO_HEIGHT;

	return GetHeight(floor, pos->x, pos->y, pos->z);
}

static void TriggerExhaustSmoke(long x, long y, long z, short angle, long speed, long moving)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	if (moving)
	{
		sptr->dR = uchar((96 * speed) >> 5);
		sptr->dG = uchar((96 * speed) >> 5);
		sptr->dB = uchar((128 * speed) >> 5);
	}
	else
	{
		sptr->dR = 96;
		sptr->dG = 96;
		sptr->dB = 128;
	}

	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 4;
	sptr->Life = uchar((GetRandomControl() & 3) - (speed >> 12) + 20);
	sptr->sLife = sptr->Life;

	if (sptr->Life < 9)
	{
		sptr->Life = 9;
		sptr->sLife = 9;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) + ((speed * phd_sin(angle)) >> 16) - 128;
	sptr->Yvel = -8 - (GetRandomControl() & 7);
	sptr->Zvel = (GetRandomControl() & 0xFF) + ((speed * phd_cos(angle)) >> 16) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = 538;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -24 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 24;
	}
	else
		sptr->Flags = 522;

	sptr->Scalar = 2;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dWidth = uchar((GetRandomControl() & 7) + (speed >> 7) + 32);
	sptr->sWidth = sptr->dWidth >> 1;
	sptr->Width = sptr->dWidth >> 1;
	sptr->dHeight = sptr->dWidth;
	sptr->sHeight = sptr->dHeight >> 1;
	sptr->Height = sptr->dHeight >> 1;
}

void inject_quadbike(bool replace)
{
	INJECT(0x0045EB20, QuadBikeDraw, replace);
	INJECT(0x0045E7E0, InitialiseQuadBike, replace);
	INJECT(0x0045E9E0, GetOnQuadBike, replace);
	INJECT(0x0045E830, QuadBikeCollision, replace);
	INJECT(0x0045F3C0, QuadbikeExplode, replace);
	INJECT(0x0045F490, SkidooCheckGetOff, replace);
	INJECT(0x0045F640, TestHeight, replace);
	INJECT(0x00460BD0, TriggerExhaustSmoke, replace);
}
