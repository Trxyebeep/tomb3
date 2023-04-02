#include "../tomb3/pch.h"
#include "LaraDraw.h"
#include "../3dsystem/3d_gen.h"
#include "../game/draw.h"
#include "../game/lara.h"
#include "../specific/output.h"
#include "../game/hair.h"
#include "../specific/game.h"
#include "../game/effect2.h"
#include "../game/control.h"
#include "../game/objects.h"

//Calculate Lara's matrices twice (once for drawing another for mesh pos getting) every draw loop
//instead of multiple times for every little thing.

static long lara_mesh_sweetness_table[NUM_LARA_MESHES] =
{
	HIPS,
	THIGH_L,
	CALF_L,
	FOOT_L,
	THIGH_R,
	CALF_R,
	FOOT_R,
	TORSO,
	HEAD,
	UARM_R,
	LARM_R,
	HAND_R,
	UARM_L,
	LARM_L,
	HAND_L
};

short* LaraShadowFrame;
long lara_matrices[indices_count * NUM_LARA_MESHES];
long lara_pos_matrices[indices_count * NUM_LARA_MESHES];
static bool FlareOn = 0;
static bool RightFlash = 0;
static bool LeftFlash = 0;

#define StoreMatrix(mx)\
{\
memcpy(mx, phd_mxptr, indices_count * sizeof(long));\
mx += indices_count;\
}

static void mInterpolateArmMatrix(long* mx)
{
	phd_mxptr[M00] = mx[M00];
	phd_mxptr[M01] = mx[M01];
	phd_mxptr[M02] = mx[M02];
	phd_mxptr[M10] = mx[M10];
	phd_mxptr[M11] = mx[M11];
	phd_mxptr[M12] = mx[M12];
	phd_mxptr[M20] = mx[M20];
	phd_mxptr[M21] = mx[M21];
	phd_mxptr[M22] = mx[M22];

	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

static void CalcLaraMatricesN(short* frame, long* bone, long flag)
{
	long* mx;
	short* rot;
	short* rot2;
	short gun;

	if (flag)
		mx = lara_pos_matrices;
	else
		mx = lara_matrices;

	rot = frame + 9;

	phd_PushMatrix();

	if (flag)
	{
		phd_mxptr[M03] = 0;
		phd_mxptr[M13] = 0;
		phd_mxptr[M23] = 0;
	}
	else
		phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);

	phd_PushMatrix();
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);

	phd_PushMatrix();
	phd_TranslateRel(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);

	phd_TranslateRel(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);

	phd_TranslateRel(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);

	phd_TranslateRel(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);

	phd_TranslateRel(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack(&rot, 0);
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);

	if (lara.weapon_item != NO_ITEM && lara.gun_type == LG_M16 &&
		(!items[lara.weapon_item].current_anim_state || items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 7);
	}
	else
		gar_RotYXZsuperpack(&rot, 0);

	phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	StoreMatrix(mx);

	phd_PushMatrix();
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	rot2 = rot;
	gar_RotYXZsuperpack(&rot2, 6);
	phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	StoreMatrix(mx);
	phd_PopMatrix();

	if (lara.gun_status == LG_READY || lara.gun_status == LG_FLARE || lara.gun_status == LG_DRAW || lara.gun_status == LG_UNDRAW)
		gun = lara.gun_type;
	else
		gun = LG_UNARMED;

	switch (gun)
	{
	case LG_UNARMED:
	case LG_FLARE:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			gar_RotYXZsuperpack(&rot, 11);
		}
		else
			gar_RotYXZsuperpack(&rot, 0);

		StoreMatrix(mx);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		if (lara.gun_type == LG_FLARE && lara.left_arm.flash_gun)
			FlareOn = 1;

		if (GnGameMode == GAMEMODE_IN_CUTSCENE)
		{
			if (lara.left_arm.flash_gun)
				LeftFlash = 1;

			if (lara.right_arm.flash_gun)
				RightFlash = 1;
		}

		break;

	case LG_PISTOLS:
	case LG_UZIS:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		StoreMatrix(mx);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		StoreMatrix(mx);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		if (lara.left_arm.flash_gun)
			LeftFlash = 1;

		if (lara.right_arm.flash_gun)
			RightFlash = 1;

		break;

	case LG_MAGNUMS:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		StoreMatrix(mx);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];

		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		StoreMatrix(mx);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		if (lara.left_arm.flash_gun)
			LeftFlash = 1;

		if (lara.right_arm.flash_gun)
			RightFlash = 1;

		break;

	case LG_SHOTGUN:
	case LG_M16:
	case LG_ROCKET:
	case LG_GRENADE:
	case LG_HARPOON:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		StoreMatrix(mx);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		if (lara.right_arm.flash_gun)
			RightFlash = 1;

		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
	LaraShadowFrame = frame;
}

static void CalcLaraMatrices_I(short* frame1, short* frame2, long frac, long rate, long* bone, long flag)
{
	long* mx;
	long* arms;
	short* rot;
	short* rot2;
	short* rotcopy;
	short* rot2copy;
	short gun;

	if (flag)
		mx = lara_pos_matrices;
	else
		mx = lara_matrices;

	phd_PushMatrix();

	if (flag)
	{
		phd_mxptr[M03] = 0;
		phd_mxptr[M13] = 0;
		phd_mxptr[M23] = 0;
	}
	else
		phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	phd_RotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);
	arms = phd_mxptr;

	phd_PushMatrix();
	rot = frame1 + 9;
	rot2 = frame2 + 9;
	InitInterpolate(frac, rate);
	phd_TranslateRel_ID(frame1[6], frame1[7], frame1[8], frame2[6], frame2[7], frame2[8]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_TranslateRel_I(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_TranslateRel_I(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_TranslateRel_I(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_TranslateRel_I(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack_I(&rot, &rot2, 0);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();
	phd_PopMatrix_I();

	phd_TranslateRel_I(bone[25], bone[26], bone[27]);

	if (lara.weapon_item != NO_ITEM && lara.gun_type == LG_M16 &&
		(!items[lara.weapon_item].current_anim_state || items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot2 = rot;
		gar_RotYXZsuperpack_I(&rot, &rot2, 7);
	}
	else
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

	phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[53], bone[54], bone[55]);
	rotcopy = rot;
	rot2copy = rot2;
	gar_RotYXZsuperpack_I(&rotcopy, &rot2copy, 6);
	phd_RotYXZ_I(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	phd_PushMatrix();
	InterpolateMatrix();
	StoreMatrix(mx);
	phd_PopMatrix();
	phd_PopMatrix_I();

	if (lara.gun_status == LG_READY || lara.gun_status == LG_SPECIAL || lara.gun_status == LG_DRAW || lara.gun_status == LG_UNDRAW)
		gun = lara.gun_type;
	else
		gun = LG_UNARMED;

	switch (gun)
	{
	case LG_UNARMED:
	case LG_FLARE:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			rot2 = rot;
			gar_RotYXZsuperpack_I(&rot, &rot2, 11);
		}
		else
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		if (lara.gun_type == LG_FLARE && lara.left_arm.flash_gun)
			FlareOn = 1;

		if (GnGameMode == GAMEMODE_IN_CUTSCENE)
		{
			if (lara.left_arm.flash_gun)
				LeftFlash = 1;

			if (lara.right_arm.flash_gun)
				RightFlash = 1;
		}

		break;

	case LG_PISTOLS:
	case LG_UZIS:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		StoreMatrix(mx);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		InterpolateArmMatrix();
		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		StoreMatrix(mx);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		if (lara.left_arm.flash_gun)
			LeftFlash = 1;

		if (lara.right_arm.flash_gun)
			RightFlash = 1;

		break;

	case LG_MAGNUMS:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		StoreMatrix(mx);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		mInterpolateArmMatrix(arms);
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		StoreMatrix(mx);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);
		
		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		StoreMatrix(mx);

		if (lara.left_arm.flash_gun)
			LeftFlash = 1;

		if (lara.right_arm.flash_gun)
			RightFlash = 1;

		break;

	case LG_SHOTGUN:
	case LG_M16:
	case LG_ROCKET:
	case LG_GRENADE:
	case LG_HARPOON:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		rot2 = &lara.right_arm.frame_base[lara.right_arm.frame_number *
			(anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot = rot2;
		gar_RotYXZsuperpack_I(&rot, &rot2, 8);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);
		phd_PushMatrix();
		InterpolateMatrix();
		StoreMatrix(mx);
		phd_PopMatrix();

		if (lara.right_arm.flash_gun)
			RightFlash = 1;

		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
}

void CalcLaraMatrices(long flag)
{
	long* bone;
	short* frame;
	short* frmptr[2];
	long rate, frac;
	short spaz;

	FlareOn = 0;
	RightFlash = 0;
	LeftFlash = 0;
	bone = &bones[objects[lara_item->object_number].bone_index];
	frac = GetFrames(lara_item, frmptr, &rate);

	if (lara.hit_direction < 0)
	{
		if (frac)
		{
			LaraShadowFrame = GetBoundsAccurate(lara_item);
			CalcLaraMatrices_I(frmptr[0], frmptr[1], frac, rate, bone, flag);
			return;
		}
	}

	if (lara.hit_direction < 0)
		frame = *frmptr;
	else
	{
		if (lara.hit_direction == 0)
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKF : ANIM_SPAZ_FORWARD;
		else if (lara.hit_direction == 1)
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKR : ANIM_SPAZ_RIGHT;
		else if (lara.hit_direction == 2)
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKB : ANIM_SPAZ_BACK;
		else
			spaz = lara.IsDucked ? ANIM_SPAZ_DUCKL : ANIM_SPAZ_LEFT;

		frame = &anims[spaz].frame_ptr[lara.hit_frame * (anims[spaz].interpolation >> 8)];
	}

	CalcLaraMatricesN(frame, bone, flag);
}

void GetLaraMeshPos(PHD_VECTOR* pos, long node)
{
	phd_PushMatrix();
	phd_mxptr[M00] = lara_pos_matrices[node * indices_count + M00];
	phd_mxptr[M01] = lara_pos_matrices[node * indices_count + M01];
	phd_mxptr[M02] = lara_pos_matrices[node * indices_count + M02];
	phd_mxptr[M03] = lara_pos_matrices[node * indices_count + M03];
	phd_mxptr[M10] = lara_pos_matrices[node * indices_count + M10];
	phd_mxptr[M11] = lara_pos_matrices[node * indices_count + M11];
	phd_mxptr[M12] = lara_pos_matrices[node * indices_count + M12];
	phd_mxptr[M13] = lara_pos_matrices[node * indices_count + M13];
	phd_mxptr[M20] = lara_pos_matrices[node * indices_count + M20];
	phd_mxptr[M21] = lara_pos_matrices[node * indices_count + M21];
	phd_mxptr[M22] = lara_pos_matrices[node * indices_count + M22];
	phd_mxptr[M23] = lara_pos_matrices[node * indices_count + M23];
	phd_TranslateRel(pos->x, pos->y, pos->z);
	pos->x = lara_item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
	pos->y = lara_item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
	pos->z = lara_item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
	phd_PopMatrix();
}

void SetLaraUnderwaterNodes()
{
	PHD_VECTOR pos;
	long bit;
	short room_num;

	bit = 0;

	for (int i = 14; i >= 0; i--)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetLaraMeshPos(&pos, i);

		if (lara_mesh_sweetness_table[i] == TORSO)
			pos.y -= 64;

		room_num = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_num);
		IsJointUnderwater[i] = room[room_num].flags & ROOM_UNDERWATER;
	}
}

void ResetLaraUnderwaterNodes()
{
	for (int i = 14; i >= 0; i--)
		IsJointUnderwater[i] = 0;
}

void NewDrawLara(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	PHD_VECTOR pos;
	short** meshpp;
	long* bone;
	short* rot;
	long top, bottom, left, right, x, y, z;
	bool blue;

	top = phd_top;
	bottom = phd_bottom;
	left = phd_left;
	right = phd_right;
	phd_top = 0;
	phd_left = 0;
	phd_bottom = phd_winymax;
	phd_right = phd_winxmax;
	phd_PushMatrix();
	obj = &objects[item->object_number];

	if (lara.skidoo == NO_ITEM)
		S_PrintShadow(obj->shadow_size, LaraShadowFrame, item);

	CalculateObjectLightingLara();
	blue = bBlueEffect;

	for (int i = 0; i < 15; i++)
	{
		if (!(item->mesh_bits & 1 << lara_mesh_sweetness_table[i]))
			continue;

		phd_mxptr[M00] = lara_matrices[i * indices_count + M00];
		phd_mxptr[M01] = lara_matrices[i * indices_count + M01];
		phd_mxptr[M02] = lara_matrices[i * indices_count + M02];
		phd_mxptr[M03] = lara_matrices[i * indices_count + M03];
		phd_mxptr[M10] = lara_matrices[i * indices_count + M10];
		phd_mxptr[M11] = lara_matrices[i * indices_count + M11];
		phd_mxptr[M12] = lara_matrices[i * indices_count + M12];
		phd_mxptr[M13] = lara_matrices[i * indices_count + M13];
		phd_mxptr[M20] = lara_matrices[i * indices_count + M20];
		phd_mxptr[M21] = lara_matrices[i * indices_count + M21];
		phd_mxptr[M22] = lara_matrices[i * indices_count + M22];
		phd_mxptr[M23] = lara_matrices[i * indices_count + M23];

		bBlueEffect = IsJointUnderwater[i];
		phd_PutPolygons(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], 0);
	}

	phd_PopMatrix();
	DrawHair();

	if (lara.back_gun && item->mesh_bits & 1 << lara_mesh_sweetness_table[LMX_TORSO])
	{
		phd_PushMatrix();
		phd_mxptr[M00] = lara_matrices[indices_count * LMX_TORSO + M00];
		phd_mxptr[M01] = lara_matrices[indices_count * LMX_TORSO + M01];
		phd_mxptr[M02] = lara_matrices[indices_count * LMX_TORSO + M02];
		phd_mxptr[M03] = lara_matrices[indices_count * LMX_TORSO + M03];
		phd_mxptr[M10] = lara_matrices[indices_count * LMX_TORSO + M10];
		phd_mxptr[M11] = lara_matrices[indices_count * LMX_TORSO + M11];
		phd_mxptr[M12] = lara_matrices[indices_count * LMX_TORSO + M12];
		phd_mxptr[M13] = lara_matrices[indices_count * LMX_TORSO + M13];
		phd_mxptr[M20] = lara_matrices[indices_count * LMX_TORSO + M20];
		phd_mxptr[M21] = lara_matrices[indices_count * LMX_TORSO + M21];
		phd_mxptr[M22] = lara_matrices[indices_count * LMX_TORSO + M22];
		phd_mxptr[M23] = lara_matrices[indices_count * LMX_TORSO + M23];
		obj = &objects[lara.back_gun];
		bone = &bones[obj->bone_index];
		meshpp = &meshes[obj->mesh_index];
		meshpp += TORSO * 2;
		phd_TranslateRel(bone[53], bone[54], bone[55]);
		rot = objects[lara.back_gun].frame_base + 9;
		gar_RotYXZsuperpack(&rot, 14);
		bBlueEffect = IsJointUnderwater[LMX_TORSO];
		phd_PutPolygons(*meshpp, -1);
		phd_PopMatrix();
	}

	bBlueEffect = blue;

	if (FlareOn)
	{
		pos.x = 8;
		pos.y = 36;
		pos.z = 32;
		GetLaraMeshPos(&pos, LMX_HAND_L);

		x = pos.x;
		y = pos.y;
		z = pos.z;

		pos.x = 8;
		pos.y = 36;
		pos.z = (GetRandomDraw() & 0xFF) + 1024;
		GetLaraMeshPos(&pos, LMX_HAND_L);

		for (int i = 0; i < (GetRandomDraw() & 3) + 4; i++)
			TriggerFlareSparks(x, y, z, pos.x - x, pos.y - y, pos.z - z, i >> 2, 0);
	}

	if (LeftFlash)
	{
		phd_PushMatrix();
		phd_mxptr[M00] = lara_matrices[indices_count * LMX_HAND_L + M00];
		phd_mxptr[M01] = lara_matrices[indices_count * LMX_HAND_L + M01];
		phd_mxptr[M02] = lara_matrices[indices_count * LMX_HAND_L + M02];
		phd_mxptr[M03] = lara_matrices[indices_count * LMX_HAND_L + M03];
		phd_mxptr[M10] = lara_matrices[indices_count * LMX_HAND_L + M10];
		phd_mxptr[M11] = lara_matrices[indices_count * LMX_HAND_L + M11];
		phd_mxptr[M12] = lara_matrices[indices_count * LMX_HAND_L + M12];
		phd_mxptr[M13] = lara_matrices[indices_count * LMX_HAND_L + M13];
		phd_mxptr[M20] = lara_matrices[indices_count * LMX_HAND_L + M20];
		phd_mxptr[M21] = lara_matrices[indices_count * LMX_HAND_L + M21];
		phd_mxptr[M22] = lara_matrices[indices_count * LMX_HAND_L + M22];
		phd_mxptr[M23] = lara_matrices[indices_count * LMX_HAND_L + M23];

		if (GnGameMode == GAMEMODE_IN_CUTSCENE)
			DrawGunFlash(LG_PISTOLS, 0);
		else
			DrawGunFlash(lara.gun_type, 0);

		phd_PopMatrix();
	}

	if (RightFlash)
	{
		phd_PushMatrix();
		phd_mxptr[M00] = lara_matrices[indices_count * LMX_HAND_R + M00];
		phd_mxptr[M01] = lara_matrices[indices_count * LMX_HAND_R + M01];
		phd_mxptr[M02] = lara_matrices[indices_count * LMX_HAND_R + M02];
		phd_mxptr[M03] = lara_matrices[indices_count * LMX_HAND_R + M03];
		phd_mxptr[M10] = lara_matrices[indices_count * LMX_HAND_R + M10];
		phd_mxptr[M11] = lara_matrices[indices_count * LMX_HAND_R + M11];
		phd_mxptr[M12] = lara_matrices[indices_count * LMX_HAND_R + M12];
		phd_mxptr[M13] = lara_matrices[indices_count * LMX_HAND_R + M13];
		phd_mxptr[M20] = lara_matrices[indices_count * LMX_HAND_R + M20];
		phd_mxptr[M21] = lara_matrices[indices_count * LMX_HAND_R + M21];
		phd_mxptr[M22] = lara_matrices[indices_count * LMX_HAND_R + M22];
		phd_mxptr[M23] = lara_matrices[indices_count * LMX_HAND_R + M23];

		if (GnGameMode == GAMEMODE_IN_CUTSCENE)
			DrawGunFlash(LG_PISTOLS, 0);
		else
			DrawGunFlash(lara.gun_type, 0);

		phd_PopMatrix();
	}

	phd_top = top;
	phd_bottom = bottom;
	phd_left = left;
	phd_right = right;
}
