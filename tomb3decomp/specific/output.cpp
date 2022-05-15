#include "../tomb3/pch.h"
#include "output.h"
#include "../3dsystem/3d_gen.h"
#include "transform.h"
#include "hwrender.h"
#include "../3dsystem/hwinsert.h"
#include "picture.h"
#ifdef TROYESTUFF
#include "../game/health.h"
#include "dx.h"
#include "../game/objects.h"
#endif

static short shadow[6 + (3 * 8)] =
{
	0, 0, 0,		//x, y, z
	32767, 1, 8,	//size, nPolys, nVtx
	0, 0, 0,		//8 vtx-> x, y, z
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};

void S_PrintShadow(short size, short* box, ITEM_INFO* item)
{
	long x0, x1, z0, z1, midX, midZ, xAdd, zAdd;

	x0 = box[0];
	x1 = box[1];
	z0 = box[4];
	z1 = box[5];
	midX = (x0 + x1) / 2;
	xAdd = (x1 - x0) * size / 1024;
	midZ = (z0 + z1) / 2;
	zAdd = (z1 - z0) * size / 1024;

	shadow[6] = short(midX - xAdd);
	shadow[8] = short(midZ + zAdd * 2);

	shadow[9] = short(midX + xAdd);
	shadow[11] = short(midZ + zAdd * 2);

	shadow[12] = short(midX + xAdd * 2);
	shadow[14] = short(midZ + zAdd);

	shadow[15] = short(midX + xAdd * 2);
	shadow[17] = short(midZ - zAdd);

	shadow[18] = short(midX + xAdd);
	shadow[20] = short(midZ - zAdd * 2);

	shadow[21] = short(midX - xAdd);
	shadow[23] = short(midZ - zAdd * 2);

	shadow[24] = short(midX - xAdd * 2);
	shadow[26] = short(midZ - zAdd);

	shadow[27] = short(midX - xAdd * 2);
	shadow[29] = short(midZ + zAdd);

	phd_leftfloat = float(phd_winxmin + phd_left);
	phd_topfloat = float(phd_winymin + phd_top);
	phd_rightfloat = float(phd_right + phd_winxmin + 1);
	phd_bottomfloat = float(phd_bottom + phd_winymin + 1);
	f_centerx = float(phd_winxmin + phd_centerx);
	f_centery = float(phd_winymin + phd_centery);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->floor, item->pos.z_pos);
	phd_RotY(item->pos.y_rot);

	if (calc_object_vertices(&shadow[4]))
		InsertTrans8(vbuf, 32);

	phd_PopMatrix();
}

void S_SetupAboveWater(long underwater)
{
	water_effect = underwater;
	shade_effect = 0;
	bBlueEffect = underwater;
}

void S_SetupBelowWater(long underwater)
{
	if (wet != underwater)
		wet = underwater;

	shade_effect = 1;
	water_effect = !underwater;
	bBlueEffect = 1;
}

#ifdef TROYESTUFF
static void DrawPickup(short obj_num)
{
	smcr = 128;
	smcg = 128;
	smcb = 128;

	LightCol[M00] = 3312;
	LightCol[M01] = 3312;
	LightCol[M02] = 0;

	LightCol[M10] = 1664;
	LightCol[M11] = 3312;
	LightCol[M12] = 0;

	LightCol[M20] = 0;
	LightCol[M21] = 3312;
	LightCol[M22] = 3072;

	LPos[0].x = 0x4000;
	LPos[0].y = -0x4000;
	LPos[0].z = 0x3000;
	LPos[1].x = -0x4000;
	LPos[1].y = -0x4000;
	LPos[1].z = 0x3000;
	LPos[2].x = 0;
	LPos[2].y = 0x2000;
	LPos[2].z = 0x3000;

	PickupY += 640;

	phd_PushUnitMatrix();
	phd_mxptr[M03] = (640 * long(float(phd_winxmax) / 512.0F) + PickupX) << W2V_SHIFT;
	phd_mxptr[M13] = 240 << W2V_SHIFT;
	phd_mxptr[M23] = 1024 << W2V_SHIFT;
	phd_RotY(PickupY);

	if (obj_num == SHOTGUN_ITEM)	//slightly nicer. todo: test every pickup, maybe make a table.
		phd_RotZ(0x2000);

	phd_PutPolygons(meshes[objects[obj_num].mesh_index], 1);
	phd_PopMatrix();
}
#endif

void S_OutputPolyList()
{
	if (App.lpZBuffer)
	{
		HWR_EnableColorKey(0);
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorAddition(0);
		HWR_EnableZBuffer(1, 1);
		HWR_SetCurrentTexture(0);

		if (bAlphaTesting)
		{
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
			DrawBuckets();
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
			phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
			HWR_DrawPolyListBF(surfacenumbf, sort3d_bufferbf);
			HWR_EnableZBuffer(0, 1);
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
			phd_SortPolyList(surfacenumfb, sort3d_bufferfb);
			HWR_DrawPolyListBF(surfacenumfb, sort3d_bufferfb);
		}
		else
		{
			DrawBuckets();
			phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
			HWR_DrawPolyListBF(surfacenumbf, sort3d_bufferbf);
		}

#ifdef TROYESTUFF
		for (int i = 0; i < 6; i++)
		{
			Buckets[i].TPage = (DXTEXTURE*)-1;
			Buckets[i].nVtx = 0;
		}

		DX_ClearBuffers(8, 0);
		HWR_EnableZBuffer(0, 1);

		if (level_complete)
			InitialisePickUpDisplay();

		if (pickups[CurrentPickup].duration != -1)
		{
			bBlueEffect = 0;
			DrawPickup(pickups[CurrentPickup].sprnum);
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
			DrawBuckets();
		}
#endif
	}
	else
	{
		HWR_EnableColorKey(0);
		HWR_EnableAlphaBlend(0);
		phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
		HWR_DrawPolyList(surfacenumbf, sort3d_bufferbf);
	}

	S_FadePicture();
	HWR_EndScene();
}

void inject_output(bool replace)
{
	INJECT(0x0048A7B0, S_PrintShadow, replace);
	INJECT(0x0048AB20, S_SetupAboveWater, replace);
	INJECT(0x0048AAC0, S_SetupBelowWater, replace);
	INJECT(0x0048A370, S_OutputPolyList, replace);
}
