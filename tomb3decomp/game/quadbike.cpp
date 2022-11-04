#include "../tomb3/pch.h"
#include "quadbike.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "../specific/init.h"
#include "control.h"
#include "../3dsystem/phd_math.h"

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

void inject_quadbike(bool replace)
{
	INJECT(0x0045EB20, QuadBikeDraw, replace);
	INJECT(0x0045E7E0, InitialiseQuadBike, replace);
	INJECT(0x0045E9E0, GetOnQuadBike, replace);
}
