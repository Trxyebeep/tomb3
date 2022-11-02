#include "../tomb3/pch.h"
#include "quadbike.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"

void QuadBikeDraw(ITEM_INFO* item)
{
	QUADINFO* quad;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* rot;
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

	phd_PopMatrix();
}

void inject_quadbike(bool replace)
{
	INJECT(0x0045EB20, QuadBikeDraw, replace);
}
