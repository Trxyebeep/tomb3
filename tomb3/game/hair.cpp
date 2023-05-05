#include "../tomb3/pch.h"
#include "hair.h"
#include "objects.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "control.h"
#include "../specific/game.h"
#include "../3dsystem/phd_math.h"
#include "lara.h"
#include "effect2.h"

static PHD_3DPOS hair[7];
static PHD_VECTOR hvel[7];
static long wind = 0;
static long wind_angle = 2048;
static long dwind_angle = 2048;
static long first_hair;

void InitialiseHair()
{
	long* bone;

	bone = &bones[objects[HAIR].bone_index + 2];
	first_hair = 1;
	hair[0].x_rot = -0x4000;
	hair[0].y_rot = 0;

	for (int i = 1; i < 7; i++, bone += 4)
	{
		hair[i].x_pos = bone[1];
		hair[i].y_pos = bone[2];
		hair[i].z_pos = bone[3];
		hair[i].x_rot = -0x4000;
		hair[i].y_rot = 0;
		hair[i].z_rot = 0;
		hvel[i].x = 0;
		hvel[i].y = 0;
		hvel[i].z = 0;
	}
}

void DrawHair()
{
	short** meshpp;

	meshpp = &meshes[objects[HAIR].mesh_index];

	for (int i = 0; i < 6; i++)
	{
		phd_PushMatrix();
		phd_TranslateAbs(hair[i].x_pos, hair[i].y_pos, hair[i].z_pos);
		phd_RotY(hair[i].y_rot);
		phd_RotX(hair[i].x_rot);
		phd_PutPolygons(*meshpp++, 1);
		phd_PopMatrix();
	}
}

void HairControl(long in_cutscene)
{
	FLOOR_INFO* floor;
	long* bone;
	short* rot[2];
	short* frm[2];
	short* frame;
	short* rotation;
	short* objptr;
	SPHERE sphere[5];
	PHD_VECTOR pos;
	long water_level, x, y, z, h, dist, frac, rate;
	short spaz, room_number;

	if (lara.hit_direction < 0)
	{
		frame = GetBestFrame(lara_item);
		frac = GetFrames(lara_item, frm, &rate);
	}
	else
	{
		switch (lara.hit_direction)
		{
		case NORTH:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKF;
			else
				spaz = ANIM_SPAZ_FORWARD;

			break;

		case EAST:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKR;
			else
				spaz = ANIM_SPAZ_RIGHT;

			break;

		case SOUTH:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKB;
			else
				spaz = ANIM_SPAZ_BACK;

			break;

		default:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKL;
			else
				spaz = ANIM_SPAZ_LEFT;

			break;
		}

		frame = &anims[spaz].frame_ptr[lara.hit_frame * (anims[spaz].interpolation >> 8)];
		frm[0] = frame;
		frac = 0;
	}

	phd_PushUnitMatrix();
	phd_SetTrans(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	if (frac)
	{
		InitInterpolate(frac, rate);
		rot[0] = frm[0] + 9;
		rot[1] = frm[1] + 9;
		bone = &bones[objects[LARA].bone_index];
		phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
		gar_RotYXZsuperpack_I(&rot[0], &rot[1], 0);

		phd_PushMatrix_I();
		objptr = lara.mesh_ptrs[HIPS];
		phd_TranslateRel_I(objptr[0], objptr[1], objptr[2]);
		InterpolateMatrix();
		sphere[0].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[0].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[0].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[0].r = objptr[3];
		phd_PopMatrix_I();

		phd_TranslateRel_I(bone[25], bone[26], bone[27]);

		if (lara.weapon_item != NO_ITEM && lara.gun_type == LG_M16 &&
			(items[lara.weapon_item].current_anim_state == 0 ||
				items[lara.weapon_item].current_anim_state == 2 ||
				items[lara.weapon_item].current_anim_state == 4))
		{
			rot[0] = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
			rot[1] = rot[0];
			gar_RotYXZsuperpack_I(&rot[0], &rot[1], 7);
		}
		else
			gar_RotYXZsuperpack_I(&rot[0], &rot[1], 6);

		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);

		phd_PushMatrix_I();
		objptr = lara.mesh_ptrs[TORSO];
		phd_TranslateRel_I(objptr[0], objptr[1], objptr[2]);
		InterpolateMatrix();
		sphere[1].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[1].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[1].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[1].r = objptr[3];
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack_I(&rot[0], &rot[1], 0);

		objptr = lara.mesh_ptrs[UARM_R];
		phd_TranslateRel_I(objptr[0], objptr[1], objptr[2]);
		InterpolateMatrix();
		sphere[3].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[3].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[3].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[3].r = 3 * objptr[3] / 2;
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack_I(&rot[0], &rot[1], 2);
		objptr = lara.mesh_ptrs[UARM_L];
		phd_TranslateRel_I(objptr[0], objptr[1], objptr[2]);
		InterpolateMatrix();
		sphere[4].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[4].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[4].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[4].r = 3 * objptr[3] / 2;
		phd_PopMatrix_I();

		phd_TranslateRel_I(bone[53], bone[54], bone[55]);
		gar_RotYXZsuperpack_I(&rot[0], &rot[1], 2);
		phd_RotYXZ_I(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);

		phd_PushMatrix_I();
		objptr = lara.mesh_ptrs[HEAD];
		phd_TranslateRel_I(objptr[0], objptr[1], objptr[2]);
		InterpolateMatrix();
		sphere[2].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[2].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[2].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[2].r = objptr[3];
		phd_PopMatrix_I();

		phd_TranslateRel_I(0, -23, -55);
		InterpolateMatrix();
		pos.x = phd_mxptr[M03] >> W2V_SHIFT;
		pos.y = phd_mxptr[M13] >> W2V_SHIFT;
		pos.z = phd_mxptr[M23] >> W2V_SHIFT;
	}
	else
	{
		rotation = frame + 9;
		bone = &bones[objects[LARA].bone_index];
		phd_TranslateRel(frame[6], frame[7], frame[8]);
		gar_RotYXZsuperpack(&rotation, 0);

		phd_PushMatrix();
		objptr = lara.mesh_ptrs[HIPS];
		phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
		sphere[0].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[0].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[0].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[0].r = objptr[3];
		phd_PopMatrix();

		phd_TranslateRel(bone[25], bone[26], bone[27]);

		if (lara.weapon_item != NO_ITEM && lara.gun_type == LG_M16 &&
			(items[lara.weapon_item].current_anim_state == 0 ||
				items[lara.weapon_item].current_anim_state == 2 ||
				items[lara.weapon_item].current_anim_state == 4))
		{
			rotation = lara.right_arm.frame_base + lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9;
			gar_RotYXZsuperpack(&rotation, 7);
		}
		else
			gar_RotYXZsuperpack(&rotation, 6);

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);

		phd_PushMatrix();
		objptr = lara.mesh_ptrs[TORSO];
		phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
		sphere[1].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[1].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[1].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[1].r = objptr[3];
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack(&rotation, 0);

		objptr = lara.mesh_ptrs[UARM_R];
		phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
		sphere[3].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[3].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[3].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[3].r = 3 * objptr[3] / 2;
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack(&rotation, 2);
		objptr = lara.mesh_ptrs[UARM_L];
		phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
		sphere[4].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[4].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[4].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[4].r = 3 * objptr[3] / 2;
		phd_PopMatrix();

		phd_TranslateRel(bone[53], bone[54], bone[55]);
		gar_RotYXZsuperpack(&rotation, 2);
		phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);

		phd_PushMatrix();
		objptr = lara.mesh_ptrs[HEAD];
		phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
		sphere[2].x = phd_mxptr[M03] >> W2V_SHIFT;
		sphere[2].y = phd_mxptr[M13] >> W2V_SHIFT;
		sphere[2].z = phd_mxptr[M23] >> W2V_SHIFT;
		sphere[2].r = objptr[3];
		phd_PopMatrix();

		phd_TranslateRel(0, -23, -55);
		pos.x = phd_mxptr[M03] >> W2V_SHIFT;
		pos.y = phd_mxptr[M13] >> W2V_SHIFT;
		pos.z = phd_mxptr[M23] >> W2V_SHIFT;
	}

	phd_PopMatrix();

	bone = &bones[objects[HAIR].bone_index];

	if (first_hair)
	{
		first_hair = 0;
		hair[0].x_pos = pos.x;
		hair[0].y_pos = pos.y;
		hair[0].z_pos = pos.z;

		for (int i = 0; i < 6; i++)
		{
			phd_PushUnitMatrix();
			phd_SetTrans(hair[i].x_pos, hair[i].y_pos, hair[i].z_pos);
			phd_RotYXZ(hair[i].y_rot, hair[i].x_rot, 0);
			phd_TranslateRel(bone[1], bone[2], bone[3]);
			hair[i + 1].x_pos = phd_mxptr[M03] >> W2V_SHIFT;
			hair[i + 1].y_pos = phd_mxptr[M13] >> W2V_SHIFT;
			hair[i + 1].z_pos = phd_mxptr[M23] >> W2V_SHIFT;
			phd_PopMatrix();
		}

		SmokeWindX = 0;
		SmokeWindZ = 0;
		wind = 0;
		wind_angle = 2048;
		dwind_angle = 2048;
		return;
	}

	hair[0].x_pos = pos.x;
	hair[0].y_pos = pos.y;
	hair[0].z_pos = pos.z;
	room_number = lara_item->room_number;

	if (in_cutscene)
		water_level = NO_HEIGHT;
	else
	{
		x = lara_item->pos.x_pos + (frame[0] + frame[1]) / 2;
		y = lara_item->pos.y_pos + (frame[3] + frame[2]) / 2;
		z = lara_item->pos.z_pos + (frame[5] + frame[4]) / 2;
		water_level = GetWaterHeight(x, y, z, room_number);
	}

	wind += (GetRandomControl() & 7) - 3;

	if (wind <= -2)
		wind++;
	else if (wind >= 9)
		wind--;

	dwind_angle = (dwind_angle + (((GetRandomControl() & 0x3F) - 32) << 1)) & 0x1FFE;

	if (dwind_angle < 1024)
		dwind_angle += (1024 - dwind_angle) << 1;
	else if (dwind_angle > 3072)
		dwind_angle -= (dwind_angle - 3072) << 1;

	wind_angle = (wind_angle + ((dwind_angle - wind_angle) >> 3)) & 0x1FFE;
	SmokeWindX = (wind * rcossin_tbl[wind_angle]) >> 12;
	SmokeWindZ = (wind * rcossin_tbl[wind_angle + 1]) >> 12;

	for (int i = 1; i < 7; i++, bone += 4)
	{
		hvel[0].x = hair[i].x_pos;
		hvel[0].y = hair[i].y_pos;
		hvel[0].z = hair[i].z_pos;

		if (in_cutscene)
			h = 32767;
		else
		{
			floor = GetFloor(hair[i].x_pos, hair[i].y_pos, hair[i].z_pos, &room_number);
			h = GetHeight(floor, hair[i].x_pos, hair[i].y_pos, hair[i].z_pos);
		}

		hair[i].x_pos += 3 * hvel[i].x / 4;
		hair[i].y_pos += 3 * hvel[i].y / 4;
		hair[i].z_pos += 3 * hvel[i].z / 4;

		if (lara.water_status == LARA_ABOVEWATER && room[room_number].flags & ROOM_NOT_INSIDE)
		{
			hair[i].x_pos += SmokeWindX;
			hair[i].z_pos += SmokeWindZ;
		}

		if (water_level == NO_HEIGHT || hair[i].y_pos < water_level)
		{
			hair[i].y_pos += 10;

			if (water_level != NO_HEIGHT && hair[i].y_pos > water_level)
				hair[i].y_pos = water_level;
		}

		if (hair[i].y_pos > h)
		{
			hair[i].x_pos = pos.x;

			if (hair[i].y_pos - h <= 256)	//snap to floor if it goes below, no more than 1 click to avoid hairection when going through corners
				hair[i].y_pos = h;

			hair[i].z_pos = pos.z;
		}

		for (int j = 0; j < 5; j++)
		{
			x = hair[i].x_pos - sphere[j].x;
			y = hair[i].y_pos - sphere[j].y;
			z = hair[i].z_pos - sphere[j].z;
			dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

			if (dist < SQUARE(sphere[j].r))
			{
				dist = phd_sqrt(dist);

				if (!dist)
					dist = 1;

				hair[i].x_pos = sphere[j].x + x * sphere[j].r / dist;
				hair[i].y_pos = sphere[j].y + y * sphere[j].r / dist;
				hair[i].z_pos = sphere[j].z + z * sphere[j].r / dist;
			}
		}

		dist = phd_sqrt(SQUARE(hair[i].z_pos - hair[i - 1].z_pos) + SQUARE(hair[i].x_pos - hair[i - 1].x_pos));
		hair[i - 1].y_rot = (short)phd_atan(hair[i].z_pos - hair[i - 1].z_pos, hair[i].x_pos - hair[i - 1].x_pos);
		hair[i - 1].x_rot = (short)-phd_atan(dist, hair[i].y_pos - hair[i - 1].y_pos);

		phd_PushUnitMatrix();
		phd_SetTrans(hair[i - 1].x_pos, hair[i - 1].y_pos, hair[i - 1].z_pos);
		phd_RotYXZ(hair[i - 1].y_rot, hair[i - 1].x_rot, 0);

		if (i == 6)
			phd_TranslateRel(bone[-3], bone[-2], bone[-1]);
		else
			phd_TranslateRel(bone[1], bone[2], bone[3]);

		hair[i].x_pos = phd_mxptr[M03] >> W2V_SHIFT;
		hair[i].y_pos = phd_mxptr[M13] >> W2V_SHIFT;
		hair[i].z_pos = phd_mxptr[M23] >> W2V_SHIFT;
		hvel[i].x = hair[i].x_pos - hvel[0].x;
		hvel[i].y = hair[i].y_pos - hvel[0].y;
		hvel[i].z = hair[i].z_pos - hvel[0].z;
		phd_PopMatrix();
	}
}
