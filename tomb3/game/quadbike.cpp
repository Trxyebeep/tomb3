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
#include "lara.h"
#include "effects.h"
#include "sphere.h"
#include "../specific/input.h"
#include "camera.h"
#include "savegame.h"
#include "../newstuff/LaraDraw.h"

static BITE_INFO quad_bites[6] =
{
	{-56, -32, -380, 0},
	{56, -32, -380, 0},
	{-8, 180, -48, 3},
	{8, 180, -48, 4},
	{90, 180, -32, 6},
	{-90, 180, -32, 7}
};

static char dont_exit_quad;
static char HandbrakeStarting;

void QuadBikeDraw(ITEM_INFO* item)
{
	QUADINFO* quad;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* rot;
	short* rot2;
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
	quad = (QUADINFO*)game_malloc(sizeof(QUADINFO));
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
		l->current_anim_state = QS_GETONL;
		l->goal_anim_state = QS_GETONL;
	}
	else
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 9;
		l->current_anim_state = QS_GETONR;
		l->goal_anim_state = QS_GETONR;
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
	SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
	SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
	lara.skidoo = NO_ITEM;
}

static long SkidooCheckGetOff()
{
	ITEM_INFO* item;
	QUADINFO* quad;
	PHD_VECTOR pos;

	item = &items[lara.skidoo];

	if ((lara_item->current_anim_state == QS_GETOFFR || lara_item->current_anim_state == QS_GETOFFL) &&
		lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		if (lara_item->current_anim_state == QS_GETOFFL)
			lara_item->pos.y_rot += 0x4000;
		else
			lara_item->pos.y_rot -= 0x4000;

		lara_item->anim_number = ANIM_STOP;
		lara_item->frame_number = anims[ANIM_STOP].frame_base;
		lara_item->current_anim_state = AS_STOP;
		lara_item->goal_anim_state = AS_STOP;
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

		if (lara_item->current_anim_state == QS_FALLOFF)
		{
			lara_item->anim_number = ANIM_FASTFALL;
			lara_item->frame_number = anims[ANIM_FASTFALL].frame_base;
			lara_item->current_anim_state = AS_FASTFALL;

			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetLaraMeshPos(&pos, LMX_HIPS);

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

		if (lara_item->current_anim_state == QS_FALLDEATH)
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
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -24 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 24;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

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

static long CanGetOff(long lr)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short angle, room_number;

	item = &items[lara.skidoo];

	if (lr >= 0)
		angle = item->pos.y_rot + 0x4000;
	else
		angle = item->pos.y_rot - 0x4000;

	x = item->pos.x_pos + ((512 * phd_sin(angle)) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos + ((512 * phd_cos(angle)) >> W2V_SHIFT);

	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (height_type != BIG_SLOPE && height_type != DIAGONAL &&
		h != NO_HEIGHT && abs(h - item->pos.y_pos) <= 512 &&
		c - item->pos.y_pos <= -762 && h - c >= 762)
		return 1;

	return 0;
}

static long GetCollisionAnim(ITEM_INFO* item, PHD_VECTOR* pos)
{
	long s, c, fb, lr;

	pos->x = item->pos.x_pos - pos->x;
	pos->z = item->pos.z_pos - pos->z;

	if (!pos->x && !pos->z)
		return 0;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	fb = (pos->x * s + pos->z * c) >> W2V_SHIFT;
	lr = (pos->x * c - pos->z * s) >> W2V_SHIFT;

	if (abs(fb) > abs(lr))
	{
		if (fb > 0)
			return 14;
		else
			return 13;
	}
	else
	{
		if (lr > 0)
			return 11;
		else
			return 12;
	}
}

static long DoDynamics(long height, long fallspeed, long* ypos)
{
	long bounce;

	if (height <= *ypos)
	{
		bounce = (height - *ypos) << 2;

		if (bounce < -80)
			bounce = -80;

		fallspeed += ((bounce - fallspeed) >> 3);

		if (*ypos > height)
			*ypos = height;
	}
	else
	{
		*ypos += fallspeed;

		if (*ypos <= height - 80)
				fallspeed += 6;
		else
		{
			*ypos = height;
			fallspeed = 0;
		}
	}

	return fallspeed;
}

static long DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos)	//from boat.cpp
{
	FLOOR_INFO* floor;
	long x, z, nX, nZ, oX, oZ, sX, sZ, h;
	short room_number;

	nX = newPos->x >> WALL_SHIFT;
	nZ = newPos->z >> WALL_SHIFT;
	oX = oldPos->x >> WALL_SHIFT;
	oZ = oldPos->z >> WALL_SHIFT;
	sX = newPos->x & WALL_MASK;
	sZ = newPos->z & WALL_MASK;

	if (nX == oX)
	{
		if (nZ == oZ)
		{
			item->pos.z_pos += (oldPos->z - newPos->z);
			item->pos.x_pos += (oldPos->x - newPos->x);
			return 0;
		}
		else if (nZ <= oZ)
		{
			item->pos.z_pos += WALL_SIZE - sZ;
			return item->pos.x_pos - newPos->x;
		}
		else
		{
			item->pos.z_pos -= 1 + sZ;
			return newPos->x - item->pos.x_pos;
		}
	}

	if (nZ == oZ)
	{
		if (nX <= oX)
		{
			item->pos.x_pos += WALL_SIZE - sX;
			return newPos->z - item->pos.z_pos;
		}
		else
		{
			item->pos.x_pos -= 1 + sX;
			return item->pos.z_pos - newPos->z;
		}
	}

	x = 0;
	z = 0;
	room_number = item->room_number;
	floor = GetFloor(oldPos->x, newPos->y, newPos->z, &room_number);
	h = GetHeight(floor, oldPos->x, newPos->y, newPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->z > oldPos->z)
			z = -1 - sZ;
		else
			z = WALL_SIZE - sZ;
	}

	room_number = item->room_number;
	floor = GetFloor(newPos->x, newPos->y, oldPos->z, &room_number);
	h = GetHeight(floor, newPos->x, newPos->y, oldPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->x > oldPos->x)
			x = -1 - sX;
		else
			x = WALL_SIZE - sX;
	}

	if (x && z)
	{
		item->pos.x_pos += x;
		item->pos.z_pos += z;
		return 0;
	}

	if (z)
	{
		item->pos.z_pos += z;

		if (z > 0)
			return item->pos.x_pos - newPos->x;
		else
			return newPos->x - item->pos.x_pos;
	}

	if (x)
	{
		item->pos.x_pos += x;

		if (x > 0)
			return newPos->z - item->pos.z_pos;
		else
			return item->pos.z_pos - newPos->z;
	}

	item->pos.x_pos += oldPos->x - newPos->x;
	item->pos.z_pos += oldPos->z - newPos->z;
	return 0;
}

static void SkidooBaddieCollision(ITEM_INFO* quad)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	short* doors;
	long dx, dy, dz;
	short roomies[16];
	short room_count, item_number;

	room_count = 1;
	roomies[0] = quad->room_number;
	doors = room[quad->room_number].door;

	if (doors)
	{
		for (int i = *doors++; i > 0; i--, doors += 16)
		{
			roomies[room_count] = *doors;
			room_count++;
		}
	}

	for (int i = 0; i < room_count; i++)
	{
		for (item_number = room[roomies[i]].item_number; item_number != NO_ITEM; item_number = item->next_item)
		{
			item = &items[item_number];

			if (item->collidable && item->status != ITEM_INVISIBLE && item != lara_item && item != quad)
			{
				obj = &objects[item->object_number];

				if (obj->collision && (obj->intelligent || item->object_number == AVALANCHE))
				{
					dx = quad->pos.x_pos - item->pos.x_pos;
					dy = quad->pos.y_pos - item->pos.y_pos;
					dz = quad->pos.z_pos - item->pos.z_pos;

					if (dx > -2048 && dx < 2048 && dz > -2048 && dz < 2048 && dy > -2048 && dy < 2048 && TestBoundsCollide(item, quad, 500))
					{
						if (item->object_number == AVALANCHE)
						{
							if (item->current_anim_state == 1)
							{
								lara_item->hit_status = 1;
								lara_item->hit_points -= 100;
							}

							continue;
						}

						DoLotsOfBlood(item->pos.x_pos, quad->pos.y_pos - 256, item->pos.z_pos, quad->speed, quad->pos.y_rot, item->room_number, 3);
						item->hit_points = 0;
					}
				}
			}
		}
	}
}

static long SkidooDynamics(ITEM_INFO* item)
{
	QUADINFO* quad;
	FLOOR_INFO* floor;
	PHD_VECTOR oldPos, newPos;
	PHD_VECTOR flPos, frPos, blPos, brPos, mlPos, mrPos, bmlPos, bmrPos, fmlPos, fmrPos;
	PHD_VECTOR flPos2, bmlPos2, mlPos2, fmlPos2, blPos2, frPos2, bmrPos2, mrPos2, fmrPos2, brPos2;
	long front_left, front_right, back_left, back_right, mid_left, mid_right, bm_left, bm_right, fm_left, fm_right;
	long front_left2, bm_left2, mid_left2, fm_left2, back_left2, front_right2, bm_right2, mid_right2, fm_right2, back_right2;
	long h, speed, slip, anim, dx, dz;
	short vel, ang, room_number, shift, shift2;

	dont_exit_quad = 0;
	quad = (QUADINFO*)item->data;
	oldPos.x = item->pos.x_pos;
	oldPos.y = item->pos.y_pos;
	oldPos.z = item->pos.z_pos;

	front_left = TestHeight(item, 550, -260, &flPos);
	front_right = TestHeight(item, 550, 260, &frPos);
	back_left = TestHeight(item, -550, -260, &blPos);
	back_right = TestHeight(item, -550, 260, &brPos);
	mid_left = TestHeight(item, 0, -260, &mlPos);
	mid_right = TestHeight(item, 0, 260, &mrPos);
	bm_left = TestHeight(item, 275, -260, &bmlPos);
	bm_right = TestHeight(item, 275, 260, &bmrPos);
	fm_left = TestHeight(item, -275, -260, &fmlPos);
	fm_right = TestHeight(item, -275, 260, &fmrPos);

	if (blPos.y > back_left)
		blPos.y = back_left;

	if (brPos.y > back_right)
		brPos.y = back_right;

	if (flPos.y > front_left)
		flPos.y = front_left;

	if (frPos.y > front_right)
		frPos.y = front_right;

	if (fmlPos.y > fm_left)
		fmlPos.y = fm_left;

	if (fmrPos.y > fm_right)
		fmrPos.y = fm_right;

	if (bmlPos.y > bm_left)
		bmlPos.y = bm_left;

	if (bmrPos.y > bm_right)
		bmrPos.y = bm_right;

	if (mlPos.y > mid_left)
		mlPos.y = mid_left;

	if (mrPos.y > mid_right)
		mrPos.y = mid_right;

	if (item->pos.y_pos <= item->floor - 256)
		item->pos.y_rot += short(quad->extra_rotation + quad->skidoo_turn);
	else
	{
		if (quad->skidoo_turn < -364)
			quad->skidoo_turn += 364;
		else if (quad->skidoo_turn > 364)
			quad->skidoo_turn -= 364;
		else
			quad->skidoo_turn = 0;

		item->pos.y_rot += short(quad->extra_rotation + quad->skidoo_turn);
		vel = short(546 - (quad->Velocity >> 8));
		ang = item->pos.y_rot - quad->momentum_angle;

		if (!(input & IN_ACTION) && quad->Velocity > 0)
			vel += vel >> 2;

		if (ang < -273)
		{
			if (ang >= -27300)
				quad->momentum_angle -= vel;
			else
				quad->momentum_angle = item->pos.y_rot + 27300;
		}
		else if (ang > 273)
		{
			if (ang <= 27300)
				quad->momentum_angle += vel;
			else
				quad->momentum_angle = item->pos.y_rot - 27300;
		}
		else
			quad->momentum_angle = item->pos.y_rot;
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos < h)
		speed = item->speed;
	else
		speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;

	item->pos.x_pos += (speed * phd_sin(quad->momentum_angle)) >> W2V_SHIFT;
	item->pos.z_pos += (speed * phd_cos(quad->momentum_angle)) >> W2V_SHIFT;

	slip = (100 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	
	if (abs(slip) > 50)
	{
		dont_exit_quad = 1;

		if (slip > 0)
			slip -= 10;
		else
			slip += 10;

		item->pos.x_pos -= (slip * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.z_pos -= (slip * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	}

	slip = (50 * phd_sin(item->pos.z_rot)) >> W2V_SHIFT;

	if (abs(slip) > 25)
	{
		dont_exit_quad = 1;
		item->pos.x_pos += (slip * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.z_pos -= (slip * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	}

	newPos.x = item->pos.x_pos;
	newPos.z = item->pos.z_pos;

	if (!(item->flags & IFL_INVISIBLE))
		SkidooBaddieCollision(item);

	shift = 0;

	front_left2 = TestHeight(item, 550, -260, &flPos2);

	if (front_left2 < flPos.y - 256)
		shift = (short)DoShift(item, &flPos2, &flPos);

	bm_left2 = TestHeight(item, 275, -260, &bmlPos2);

	if (bm_left2 < bmlPos.y - 256)
		DoShift(item, &bmlPos2, &bmlPos);

	mid_left2 = TestHeight(item, 0, -260, &mlPos2);

	if (mid_left2 < mlPos.y - 256)
		DoShift(item, &mlPos2, &mlPos);

	fm_left2 = TestHeight(item, -275, -260, &fmlPos2);

	if (fm_left2 < fmlPos.y - 256)
		DoShift(item, &fmlPos2, &fmlPos);

	back_left2 = TestHeight(item, -550, -260, &blPos2);

	if (back_left2 < blPos.y - 256)
	{
		shift2 = (short)DoShift(item, &blPos2, &blPos);

		if ((shift2 > 0 && shift >= 0) || (shift2 < 0 && shift <= 0))
			shift += shift2;
	}

	front_right2 = TestHeight(item, 550, 260, &frPos2);

	if (front_right2 < frPos.y - 256)
	{
		shift2 = (short)DoShift(item, &frPos2, &frPos);

		if ((shift2 > 0 && shift >= 0) || (shift2 < 0 && shift <= 0))
			shift += shift2;
	}

	bm_right2 = TestHeight(item, 275, 260, &bmrPos2);

	if (bm_right2 < bmrPos.y - 256)
		DoShift(item, &bmrPos2, &bmrPos);

	mid_right2 = TestHeight(item, 0, 260, &mrPos2);

	if (mid_right2 < mrPos.y - 256)
		DoShift(item, &mrPos2, &mrPos);

	fm_right2 = TestHeight(item, -275, 260, &fmrPos2);

	if (fm_right2 < fmrPos.y - 256)
		DoShift(item, &fmrPos2, &fmrPos);

	back_right2 = TestHeight(item, -550, 260, &brPos2);

	if (back_right2 < brPos.y - 256)
	{
		shift2 = (short)DoShift(item, &brPos2, &brPos);

		if ((shift2 > 0 && shift >= 0) || (shift2 < 0 && shift <= 0))
			shift += shift2;
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < item->pos.y_pos - 256)
		DoShift(item, (PHD_VECTOR*)&item->pos, &oldPos);

	quad->extra_rotation = shift;
	anim = GetCollisionAnim(item, &newPos);

	if (anim)
	{
		dx = item->pos.x_pos - oldPos.x;
		dz = item->pos.z_pos - oldPos.z;
		speed = (dx * phd_sin(quad->momentum_angle) + dz * phd_cos(quad->momentum_angle)) >> W2V_SHIFT;
		speed <<= 8;

		if (&items[lara.skidoo] == item && quad->Velocity == 0xA000 && speed < 0x9FF6)
		{
			lara_item->hit_points -= short((0xA000 - speed) >> 7);
			lara_item->hit_status = 1;
		}

		if (quad->Velocity > 0 && speed < quad->Velocity)
			quad->Velocity = speed < 0 ? 0 : speed;
		else if (quad->Velocity < 0 && speed > quad->Velocity)
			quad->Velocity = speed > 0 ? 0 : speed;

		if (quad->Velocity < -0x3000)
			quad->Velocity = -0x3000;
	}

	return anim;
}

static void AnimateQuadBike(ITEM_INFO* item, long hitWall, long killed)
{
	QUADINFO* quad;
	short state;

	quad = (QUADINFO*)item->data;
	state = lara_item->current_anim_state;

	if (item->pos.y_pos != item->floor && state != QS_FALL && state != QS_LAND && state != QS_FALLOFF && !killed)
	{
		if (quad->Velocity < 0)
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 6;
		else
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 25;

		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
		lara_item->current_anim_state = 8;
		lara_item->goal_anim_state = 8;
	}
	else if (hitWall &&
		state != QS_HITFRONT && state != QS_HITBACK && state != QS_HITLEFT && state != QS_HITRIGHT && state != QS_FALLOFF &&
		quad->Velocity > 0x3555 && !killed)
	{
		switch (hitWall)
		{
		case 13:
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 12;
			lara_item->current_anim_state = QS_HITFRONT;
			lara_item->goal_anim_state = QS_HITFRONT;
			break;

		case 14:
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 11;
			lara_item->current_anim_state = QS_HITBACK;
			lara_item->goal_anim_state = QS_HITBACK;
			break;

		case 11:
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 14;
			lara_item->current_anim_state = QS_HITLEFT;
			lara_item->goal_anim_state = QS_HITLEFT;
			break;

		default:
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 13;
			lara_item->current_anim_state = QS_HITRIGHT;
			lara_item->goal_anim_state = QS_HITRIGHT;
			break;
		}

		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
		SoundEffect(SFX_QUAD_FRONT_IMPACT, &item->pos, SFX_DEFAULT);
	}
	else
	{
		switch (lara_item->current_anim_state)
		{
		case QS_DRIVE:

			if (killed)
			{
				if (quad->Velocity <= 0x5000)
					lara_item->goal_anim_state = QS_BIKEDEATH;
				else
					lara_item->goal_anim_state = QS_FALLDEATH;
			}
			else if (!(quad->Velocity & 0xFFFFFF00) && !(input & (IN_JUMP | IN_ACTION)))
				lara_item->goal_anim_state = QS_STOP;
			else if (input & IN_LEFT && !HandbrakeStarting)
				lara_item->goal_anim_state = QS_TURNL;
			else if (input & IN_RIGHT && !HandbrakeStarting)
				lara_item->goal_anim_state = QS_TURNR;
			else if (input & IN_JUMP)
			{
				if (quad->Velocity <= 0x6AAA)
					lara_item->goal_anim_state = QS_SLOW;
				else
					lara_item->goal_anim_state = QS_BRAKE;
			}

			break;

		case 2:

			if (!(quad->Velocity & 0xFFFFFF00))
				lara_item->goal_anim_state = QS_STOP;
			else if (input & IN_RIGHT)
			{
				lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 20;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->current_anim_state = QS_TURNR;
				lara_item->goal_anim_state = QS_TURNR;
			}
			else if (!(input & IN_LEFT))
				lara_item->goal_anim_state = QS_DRIVE;

			break;

		case 5:
		case 6:
		case 18:

			if (!(quad->Velocity & 0xFFFFFF00))
				lara_item->goal_anim_state = QS_STOP;
			else if (input & IN_LEFT)
				lara_item->goal_anim_state = QS_TURNL;
			else if (input & IN_RIGHT)
				lara_item->goal_anim_state = QS_TURNR;

			break;

		case 8:

			if (item->pos.y_pos == item->floor)
				lara_item->goal_anim_state = QS_LAND;
			else if (item->fallspeed > 240 && CurrentLevel != LV_GYM)
				quad->Flags |= 0x40;

			break;

		case 11:
		case 12:
		case 13:
		case 14:

			if (input & (IN_JUMP | IN_ACTION))
				lara_item->goal_anim_state = QS_DRIVE;

			break;

		case 15:

			if (killed)
			{
				lara_item->goal_anim_state = QS_BIKEDEATH;
				break;
			}

			if (input & IN_ROLL && !quad->Velocity && !dont_exit_quad)
			{
				if (input & IN_RIGHT && CanGetOff(1))
					lara_item->goal_anim_state = QS_GETOFFR;
				else if (input & IN_LEFT && CanGetOff(-1))
					lara_item->goal_anim_state = QS_GETOFFL;
			}
			else if (input & (IN_JUMP | IN_ACTION))
				lara_item->goal_anim_state = QS_DRIVE;

			break;

		case 22:

			if (!(quad->Velocity & 0xFFFFFF00))
				lara_item->goal_anim_state = QS_STOP;
			else if (input & IN_LEFT)
			{
				lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 3;
				lara_item->frame_number = anims[lara_item->anim_number].frame_base;
				lara_item->current_anim_state = QS_TURNL;
				lara_item->goal_anim_state = QS_TURNL;
			}
			else if (!(input & IN_RIGHT))
				lara_item->goal_anim_state = QS_DRIVE;

			break;
		}
	}

	if (room[item->room_number].flags & (ROOM_UNDERWATER | ROOM_SWAMP))
	{
		lara_item->goal_anim_state = QS_FALLOFF;
		lara_item->hit_points = 0;
		QuadbikeExplode(item);
	}
}

static long UserControl(ITEM_INFO* item, long height, long* pitch)
{
	QUADINFO* quad;
	long revs;
	static char CanHandbrakeStart;

	quad = (QUADINFO*)item->data;

	if (!quad->Velocity && !(input & IN_SPRINT) && !CanHandbrakeStart)
		CanHandbrakeStart = 1;
	else if (quad->Velocity)
		CanHandbrakeStart = 0;

	if (!(input & IN_SPRINT))
		HandbrakeStarting = 0;

	if (!HandbrakeStarting)
	{
		if (quad->Revs > 16)
		{
			quad->Velocity += quad->Revs >> 4;
			quad->Revs -= quad->Revs >> 3;
		}
		else
			quad->Revs = 0;
	}

	if (item->pos.y_pos < height - 256)
	{
		if (quad->EngineRevs < 0xA000)
			quad->EngineRevs += (0xA000 - quad->EngineRevs) >> 3;
	}
	else
	{
		if (!quad->Velocity && input & IN_LOOK)
			LookUpDown();

		if (quad->Velocity > 0)
		{
			if (input & IN_SPRINT && !HandbrakeStarting && quad->Velocity > 0x3000)
			{
				if (input & IN_LEFT)
				{
					quad->skidoo_turn -= 500;

					if (quad->skidoo_turn < -0x5B0)
						quad->skidoo_turn = -0x5B0;
				}
				else if (input & IN_RIGHT)
				{
					quad->skidoo_turn += 500;

					if (quad->skidoo_turn > 0x5B0)
						quad->skidoo_turn = 0x5B0;
				}
			}
			else
			{
				if (input & IN_LEFT)
				{
					quad->skidoo_turn -= 455;

					if (quad->skidoo_turn < -910)
						quad->skidoo_turn = -910;
				}
				else if (input & IN_RIGHT)
				{
					quad->skidoo_turn += 455;

					if (quad->skidoo_turn > 910)
						quad->skidoo_turn = 910;
				}
			}
		}
		else if (quad->Velocity < 0)
		{
			if (input & IN_SPRINT && !HandbrakeStarting && quad->Velocity < -0x2800)
			{
				if (input & IN_RIGHT)
				{
					quad->skidoo_turn -= 500;

					if (quad->skidoo_turn < -0x5B0)
						quad->skidoo_turn = -0x5B0;
				}
				else if (input & IN_LEFT)
				{
					quad->skidoo_turn += 500;

					if (quad->skidoo_turn > 0x5B0)
						quad->skidoo_turn = 0x5B0;
				}
			}
			else
			{
				if (input & IN_RIGHT)
				{
					quad->skidoo_turn -= 455;

					if (quad->skidoo_turn < -910)
						quad->skidoo_turn = -910;
				}
				else if (input & IN_LEFT)
				{
					quad->skidoo_turn += 455;

					if (quad->skidoo_turn > 910)
						quad->skidoo_turn = 910;
				}
			}
		}

		if (input & IN_JUMP)
		{
			if (input & IN_SPRINT && (CanHandbrakeStart || HandbrakeStarting))
			{
				HandbrakeStarting = 1;
				quad->Revs -= 512;

				if (quad->Revs < -0x3000)
					quad->Revs = -0x3000;
			}
			else if (quad->Velocity > 0)
				quad->Velocity -= 640;
			else  if (quad->Velocity > -0x3000)
				quad->Velocity -= 768;
		}
		else if (input & IN_ACTION)
		{
			if (input & IN_SPRINT && (CanHandbrakeStart || HandbrakeStarting))
			{
				HandbrakeStarting = 1;
				quad->Revs += 512;

				if (quad->Revs >= 0xA000)
					quad->Revs = 0xA000;
			}
			else if (quad->Velocity < 0xA000)
			{
				if (quad->Velocity < 0x4000)
					quad->Velocity += ((0x4800 - quad->Velocity) >> 3) + 8;
				else if (quad->Velocity < 0x7000)
					quad->Velocity += ((0x7800 - quad->Velocity) >> 4) + 4;
				else
					quad->Velocity += ((0xA000 - quad->Velocity) >> 3) + 2;
			}
			else
				quad->Velocity = 0xA000;
		}
		else if (quad->Velocity > 256)
			quad->Velocity -= 256;
		else if (quad->Velocity < -256)
			quad->Velocity += 256;
		else
			quad->Velocity = 0;

		if (HandbrakeStarting && quad->Revs && !(input & IN_JUMP | IN_ACTION))
		{
			if (quad->Revs > 8)
				quad->Revs -= quad->Revs >> 3;
			else
				quad->Revs = 0;
		}

		item->speed = short(quad->Velocity >> 8);

		if (quad->EngineRevs > 0x7000)
			quad->EngineRevs = -0x2000;

		revs = 0;	//originally uninitialized

		if (quad->Velocity < 0)
			revs = abs(quad->Revs) + abs(quad->Velocity >> 1);
		else if (quad->Velocity < 0x7000)
			revs = abs(quad->Revs) + 0x8800 * quad->Velocity / 0x7000 - 0x2000;
		else if (quad->Velocity <= 0xA000)
			revs = abs(quad->Revs) + 0x9800 * (quad->Velocity - 0x7000) / 0x3000 - 0x2800;
		else
			revs += abs(quad->Revs);

		quad->EngineRevs += (revs - quad->EngineRevs) >> 3;
	}

	*pitch = quad->EngineRevs;
	return 0;
}

long QuadBikeControl()
{
	ITEM_INFO* item;
	QUADINFO* quad;
	FLOOR_INFO* floor;
	PHD_VECTOR flPos, frPos, pos;
	long front_left, front_right;
	long killed, hitWall, h, driving, pitch, smokeVel;
	short room_number, rot, xrot, zrot, state;
	static uchar ExhaustSmokeVel;

	item = &items[lara.skidoo];
	quad = (QUADINFO*)item->data;
	hitWall = SkidooDynamics(item);
	killed = 0;
	pitch = 0;	//originally uninitialized

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	front_left = TestHeight(item, 550, -260, &flPos);
	front_right = TestHeight(item, 550, 260, &frPos);

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	TestTriggers(trigger_index, 0);

	if (lara_item->hit_points <= 0)
	{
		killed = 1;
		input &= ~(IN_FORWARD | IN_BACK | IN_LEFT | IN_RIGHT);
	}

	if (quad->Flags)
	{
		driving = front_right;	//what
		hitWall = 0;
	}
	else
	{
		switch (lara_item->current_anim_state)
		{
		case QS_GETONR:
		case QS_GETOFFR:
		case QS_GETONL:
		case QS_GETOFFL:
			driving = -1;
			hitWall = 0;
			break;

		default:
			driving = UserControl(item, h, &pitch);
			break;
		}
	}

	if (quad->Velocity || quad->Revs)
	{
		 quad->pitch = pitch;

		 if (quad->pitch < -0x8000)
			 quad->pitch = -0x8000;
		 else if (quad->pitch > 0xA000)
			 quad->pitch = 0xA000;

		 SoundEffect(SFX_QUAD_MOVE, &item->pos, (quad->pitch << 8) + 0x1000004);
	}
	else
	{
		if (driving != -1)
			SoundEffect(SFX_QUAD_IDLE, &item->pos, SFX_DEFAULT);

		quad->pitch = 0;
	}

	item->floor = h;
	rot = short(quad->Velocity >> 2);
	quad->FrontRot -= rot;
	quad->RearRot -= short(rot + (quad->Revs >> 3));
	quad->left_fallspeed = DoDynamics(front_left, quad->left_fallspeed, &flPos.y);
	quad->right_fallspeed = DoDynamics(front_right, quad->right_fallspeed, &frPos.y);
	item->fallspeed = (short)DoDynamics(h, item->fallspeed, &item->pos.y_pos);

	h = (flPos.y + frPos.y) >> 1;
	xrot = (short)phd_atan(550, item->pos.y_pos - h);
	zrot = (short)phd_atan(260, h - flPos.y);
	item->pos.x_rot += (xrot - item->pos.x_rot) >> 1;
	item->pos.z_rot += (zrot - item->pos.z_rot) >> 1;

	if (!(quad->Flags & 0x80))
	{
		if (room_number != item->room_number)
		{
			ItemNewRoom(lara.skidoo, room_number);
			ItemNewRoom(lara.item_number, room_number);
		}

		lara_item->pos.x_pos = item->pos.x_pos;
		lara_item->pos.y_pos = item->pos.y_pos;
		lara_item->pos.z_pos = item->pos.z_pos;
		lara_item->pos.x_rot = item->pos.x_rot;
		lara_item->pos.y_rot = item->pos.y_rot;
		lara_item->pos.z_rot = item->pos.z_rot;
		AnimateQuadBike(item, hitWall, killed);
		AnimateItem(lara_item);
		item->anim_number = lara_item->anim_number + objects[QUADBIKE].anim_index - objects[VEHICLE_ANIM].anim_index;
		item->frame_number = lara_item->frame_number + anims[item->anim_number].frame_base - anims[lara_item->anim_number].frame_base;
		camera.target_elevation = -5460;

		if (quad->Flags & 0x40 && item->pos.y_pos == item->floor)
		{
			ExplodingDeath(lara.item_number, -1, 1);
			lara_item->hit_points = 0;
			lara_item->flags |= IFL_INVISIBLE;
			QuadbikeExplode(item);
			return 0;
		}
	}

	state = lara_item->current_anim_state;

	if (state != QS_GETONR && state != QS_GETONL && state != QS_GETOFFR && state != QS_GETOFFL)
	{
		for (int i = 0; i < 2; i++)
		{
			pos.x = quad_bites[i].x;
			pos.y = quad_bites[i].y;
			pos.z = quad_bites[i].z;
			GetJointAbsPosition(item, &pos, quad_bites[i].mesh_num);
			rot = item->pos.y_rot + (!i ? 0x9000 : 0x7000);

			if (item->speed > 32)
			{
				if (item->speed < 64)
					TriggerExhaustSmoke(pos.x, pos.y, pos.z, rot, 64 - item->speed, 1);
			}
			else
			{
				if (ExhaustSmokeVel < 16)
				{
					smokeVel = ((GetRandomControl() & 7) + (GetRandomControl() & 0x10) + 2 * ExhaustSmokeVel) << 7;
					ExhaustSmokeVel++;
				}
				else if (HandbrakeStarting)
					smokeVel = (abs(quad->Revs) >> 2) + ((GetRandomControl() & 7) << 7);
				else if (GetRandomControl() & 3)
					smokeVel = 0;
				else
					smokeVel = ((GetRandomControl() & 0xF) + (GetRandomControl() & 0x10)) << 7;

				TriggerExhaustSmoke(pos.x, pos.y, pos.z, rot, smokeVel, 0);
			}
		}
	}
	else
	{
		if (CurrentLevel == LV_GYM)
		{
			savegame.timer = 0;
			assault_timer_active = 0;
			assault_timer_display = 0;
		}

		ExhaustSmokeVel = 0;
	}

	return SkidooCheckGetOff();
}
