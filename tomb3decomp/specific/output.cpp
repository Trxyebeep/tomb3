#include "../tomb3/pch.h"
#include "output.h"
#include "../3dsystem/3d_gen.h"
#include "transform.h"
#include "hwrender.h"
#include "../3dsystem/hwinsert.h"
#include "picture.h"
#include "workstubs.h"
#ifdef TROYESTUFF
#include "../game/health.h"
#include "dx.h"
#include "../game/objects.h"
#include "litesrc.h"
#include "draweffects.h"
#include "../tomb3/tomb3.h"
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

#ifdef TROYESTUFF
	if (tomb3.shadow_mode == SHADOW_PSX)
	{
		S_PrintSpriteShadow(size, box, item);
		return;
	}
#endif

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
long GetRenderScale(long unit)
{
	long w, h, x, y;

	w = 640;
	h = 480;
	x = (phd_winwidth > w) ? MulDiv(phd_winwidth, unit, w) : unit;
	y = (phd_winheight > h) ? MulDiv(phd_winheight, unit, h) : unit;
	return x < y ? x : y;
}

struct display_rots
{
	short obj_num;
	short rot_x;
	short rot_z;
};

display_rots rots[32] =
{
	{PICKUP_ITEM1, 0, 0},
	{PICKUP_ITEM2, 0, 0},

	{KEY_ITEM1, 0, 0},
	{KEY_ITEM2, 0, 0},
	{KEY_ITEM3, 0, 0},
	{KEY_ITEM4, 0, 0},

	{PUZZLE_ITEM1, 0, 0},
	{PUZZLE_ITEM2, 0, 0},
	{PUZZLE_ITEM3, 0, 0},
	{PUZZLE_ITEM4, 0, 0},

	{GUN_ITEM, 0, 0},
	{SHOTGUN_ITEM, 0x5000, -0x2000},
	{HARPOON_ITEM, -0x3000, 0},
	{ROCKET_GUN_ITEM, 0x1000, -0x2000},
	{GRENADE_GUN_ITEM, -0x3000, 0},
	{M16_ITEM, -0x3000, 0},
	{MAGNUM_ITEM, -0x2500, -0x2000},
	{UZI_ITEM, 0x4000, 0},
	{FLAREBOX_ITEM, 0, 0},

	{SG_AMMO_ITEM, 0xE38, 0},
	{MAG_AMMO_ITEM, 0x1000, 0},
	{UZI_AMMO_ITEM, -0x2000, 0},
	{HARPOON_AMMO_ITEM, 0xE38, 0},
	{M16_AMMO_ITEM, 0x1000, 0},
	{ROCKET_AMMO_ITEM, 0, 0},
	{GRENADE_AMMO_ITEM, 0, 0},

	{MEDI_ITEM, 0, 0},
	{BIGMEDI_ITEM, 0, 0},

	{ICON_PICKUP1_ITEM, 0, 0},
	{ICON_PICKUP2_ITEM, 0, 0},
	{ICON_PICKUP3_ITEM, 0, 0},
	{ICON_PICKUP4_ITEM, 0, 0},
};

long find_display_entry(short obj_num)
{
	for (int i = 0; i < 32; i++)
	{
		if (rots[i].obj_num == obj_num)
			return i;
	}

	return -1;
}

static void phd_PutPolygonsPickup(short* objptr, long clip)
{
	short* newPtr;

	phd_leftfloat = (float)phd_winxmin;
	phd_topfloat = (float)phd_winymin;
	phd_rightfloat = float(phd_winxmax + phd_winxmin + 1);
	phd_bottomfloat = float(phd_winymax + phd_winymin + 1);
	objptr += 4;
	newPtr = calc_object_vertices(objptr);

	if (newPtr)
	{
		newPtr = calc_vertice_light(newPtr, objptr);
		newPtr = InsertObjectGT4(newPtr + 1, *newPtr, MID_SORT);
		newPtr = InsertObjectGT3(newPtr + 1, *newPtr, MID_SORT);
		newPtr = InsertObjectG4(newPtr + 1, *newPtr, MID_SORT);
		InsertObjectG3(newPtr + 1, *newPtr, MID_SORT);
	}
}

static void DrawPickup(short obj_num)
{
	float fx, fy;
	long entry, x, y, z;
	short rotx, rotz;

	phd_LookAt(0, -1024, 0, 0, 0, 0, 0);

	LightCol[M00] = 3072;
	LightCol[M10] = 1680;	//sun
	LightCol[M20] = 640;

	LightCol[M01] = 1024;
	LightCol[M11] = 1024;	//spot
	LightCol[M21] = 1024;

	LightCol[M02] = 640;
	LightCol[M12] = 2432;	//dynamic
	LightCol[M22] = 4080;

	smcr = 64;
	smcg = 64;
	smcb = 64;

	x = 0x2000;
	y = -0x2000;
	z = 0x1800;
	LPos[0].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[0].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[0].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	x = -0x2000;
	y = -0x4000;
	z = 0x3000;
	LPos[1].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[1].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[1].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	x = 0;
	y = 0x2000;
	z = 0x3000;
	LPos[2].x = (x * w2v_matrix[M00] + y * w2v_matrix[M01] + z * w2v_matrix[M02]) >> W2V_SHIFT;
	LPos[2].y = (x * w2v_matrix[M10] + y * w2v_matrix[M11] + z * w2v_matrix[M12]) >> W2V_SHIFT;
	LPos[2].z = (x * w2v_matrix[M20] + y * w2v_matrix[M21] + z * w2v_matrix[M22]) >> W2V_SHIFT;

	PickupY += 728;
	entry = find_display_entry(obj_num);

	if (entry == -1)
	{
		rotx = 0;
		rotz = 0;
	}
	else
	{
		rotx = rots[entry].rot_x;
		rotz = rots[entry].rot_z;
	}

	phd_PushUnitMatrix();
	phd_mxptr[M03] = 0;
	phd_mxptr[M13] = 0;
	phd_mxptr[M23] = 1024 << W2V_SHIFT;
	phd_RotYXZ(PickupY, rotx, rotz);

	fy = f_centery;
	fx = f_centerx;
	f_centerx = float(phd_winxmin + phd_winxmax - ((50 * phd_persp) >> 8));
	f_centery = float(phd_winymax + phd_winymin - ((50 * phd_persp) >> 8));
	f_centerx += PickupX;
	phd_PutPolygonsPickup(meshes[objects[obj_num].mesh_index], 1);	
	f_centerx = fx;
	f_centery = fy;

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
	}
	else
	{
		HWR_EnableColorKey(0);
		HWR_EnableAlphaBlend(0);
		phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
		HWR_DrawPolyList(surfacenumbf, sort3d_bufferbf);
	}

#ifdef TROYESTUFF
	if (pickups[CurrentPickup].duration != -1)
	{
		if (App.lpZBuffer)
		{
			for (int i = 0; i < 6; i++)
			{
				Buckets[i].TPage = (DXTEXTURE*)-1;
				Buckets[i].nVtx = 0;
			}

			DX_ClearBuffers(8, 0);
			HWR_EnableColorKey(0);
			HWR_EnableAlphaBlend(0);
			HWR_EnableColorAddition(0);
			HWR_EnableZBuffer(1, 1);
		}
		else
			phd_InitPolyList();

		if (level_complete)
			InitialisePickUpDisplay();

		bBlueEffect = 0;
		DrawPickup(pickups[CurrentPickup].sprnum);

		if (App.lpZBuffer)
			DrawBuckets();
		else
		{
			phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
			HWR_DrawPolyListBF(surfacenumbf, sort3d_bufferbf);
		}
	}
#endif

	S_FadePicture();
	HWR_EndScene();
}

void S_LightRoom(ROOM_INFO* r)
{
	short* ptr;
	long level;

	if (!r->lighting)
		return;

	level = light_level[r->lighting];
	ptr = r->data;

	for (int i = (long)*ptr++; i > 0; i++, ptr += 6)
		((uchar*)ptr)[2] += uchar((level * (((uchar*)ptr)[3] & 0x1F)) >> 6);
}

void S_InsertBackPolygon(long xmin, long ymin, long xmax, long ymax, long col)
{
#ifdef TROYESTUFF
	InsertFlatRect(phd_winxmin + xmin, phd_winymin + ymin, phd_winxmin + xmax, phd_winymin + ymax, phd_zfar, 0);
#else
	InsertFlatRect(phd_winxmin + xmin, phd_winymin + ymin, phd_winxmin + xmax, phd_winymin + ymax, phd_zfar, inv_colours[0]);
#endif
}

long S_GetObjectBounds(short* box)
{
	long* v;
	long vtx[8][3];
	long xmin, xmax, ymin, ymax, zmin, zmax, nZ, x, y, z;

	if (phd_mxptr[M23] >= phd_zfar && !outside)
		return 0;

	objbcnt++;
	xmin = box[0];
	xmax = box[1];
	ymin = box[2];
	ymax = box[3];
	zmin = box[4];
	zmax = box[5];

	vtx[0][0] = xmin;
	vtx[0][1] = ymin;
	vtx[0][2] = zmin;

	vtx[1][0] = xmax;
	vtx[1][1] = ymin;
	vtx[1][2] = zmin;

	vtx[2][0] = xmax;
	vtx[2][1] = ymax;
	vtx[2][2] = zmin;

	vtx[3][0] = xmin;
	vtx[3][1] = ymax;
	vtx[3][2] = zmin;

	vtx[4][0] = xmin;
	vtx[4][1] = ymin;
	vtx[4][2] = zmax;

	vtx[5][0] = xmax;
	vtx[5][1] = ymin;
	vtx[5][2] = zmax;

	vtx[6][0] = xmax;
	vtx[6][1] = ymax;
	vtx[6][2] = zmax;

	vtx[7][0] = xmin;
	vtx[7][1] = ymax;
	vtx[7][2] = zmax;
	
	xmin = 0x3FFFFFFF;
	xmax = -0x3FFFFFFF;
	ymin = 0x3FFFFFFF;
	ymax = -0x3FFFFFFF;
	v = &vtx[0][0];
	nZ = 0;

	for (int i = 0; i < 8; i++)
	{
		z = v[0] * phd_mxptr[M20] + v[1] * phd_mxptr[M21] + v[2] * phd_mxptr[M22] + phd_mxptr[M23];

		if (z > phd_znear && z < phd_zfar)
		{
			nZ++;
			z /= phd_persp;
			x = (v[0] * phd_mxptr[M00] + v[1] * phd_mxptr[M01] + v[2] * phd_mxptr[M02] + phd_mxptr[M03]) / z;

			if (x < xmin)
				xmin = x;

			if (x > xmax)
				xmax = x;

			y = (v[0] * phd_mxptr[M10] + v[1] * phd_mxptr[M11] + v[2] * phd_mxptr[M12] + phd_mxptr[M13]) / z;

			if (y < ymin)
				ymin = y;

			if (y > ymax)
				ymax = y;
		}

		v += 3;
	}

	xmin += phd_centerx;
	xmax += phd_centerx;
	ymin += phd_centery;
	ymax += phd_centery;

	if (nZ < 8 || xmin < 0 || ymin < 0 || xmax > phd_winxmax || ymax > phd_winymax)
		return -1;

	if (xmin > phd_right || ymin > phd_bottom || xmax < phd_left || ymax < phd_top)
		return 0;

	return 1;
}

void mCalcPoint(long x, long y, long z, long* result)
{
	x -= w2v_matrix[M03];
	y -= w2v_matrix[M13];
	z -= w2v_matrix[M23];
	result[0] = (w2v_matrix[M00] * x + w2v_matrix[M01] * y + w2v_matrix[M02] * z) >> 14;
	result[1] = (w2v_matrix[M10] * x + w2v_matrix[M11] * y + w2v_matrix[M12] * z) >> 14;
	result[2] = (w2v_matrix[M20] * x + w2v_matrix[M21] * y + w2v_matrix[M22] * z) >> 14;
}

void ProjectPCoord(long x, long y, long z, long* result, long cx, long cy, long fov)
{
	if (z > 0)
	{
		result[0] = cx + x * fov / z;
		result[1] = cy + y * fov / z;
		result[2] = z;
	}
	else if (z < 0)
	{
		result[0] = cx - x * fov / z;
		result[1] = cy - y * fov / z;
		result[2] = z;
	}
	else
	{
		result[0] = cx + x * fov;
		result[1] = cy + y * fov;
		result[2] = z;
	}
}

long S_DumpCine()
{
	static long nf = 0;

	if (framedump)
	{
		nf++;
		SaveDDBuffer(App.lpFrontBuffer);
		return 1;
	}

	return 0;
}

void inject_output(bool replace)
{
	INJECT(0x0048A7B0, S_PrintShadow, replace);
	INJECT(0x0048AB20, S_SetupAboveWater, replace);
	INJECT(0x0048AAC0, S_SetupBelowWater, replace);
	INJECT(0x0048A370, S_OutputPolyList, replace);
	INJECT(0x0048A9B0, S_LightRoom, replace);
	INJECT(0x0048A760, S_InsertBackPolygon, replace);
	INJECT(0x0048A4C0, S_GetObjectBounds, replace);
	INJECT(0x0048AC20, mCalcPoint, replace);
	INJECT(0x0048ACC0, ProjectPCoord, replace);
	INJECT(0x0048A2A0, S_DumpCine, replace);
}
