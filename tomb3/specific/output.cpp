#include "../tomb3/pch.h"
#include "output.h"
#include "../3dsystem/3d_gen.h"
#include "transform.h"
#include "hwrender.h"
#include "../3dsystem/hwinsert.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "../newstuff/Picture2.h"
#else
#include "picture.h"
#endif
#include "dxshell.h"
#include "display.h"
#include "time.h"
#include "game.h"
#include "../3dsystem/phd_math.h"
#include "dd.h"
#include "file.h"
#include "winmain.h"
#include "../game/gameflow.h"
#include "../game/draw.h"
#include "../game/inventry.h"
#include "../game/control.h"
#include "../game/health.h"
#include "../game/objects.h"
#include "litesrc.h"
#include "draweffects.h"
#include "../tomb3/tomb3.h"

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

long framedump;
long water_effect;
bool bBlueEffect;

void S_PrintShadow(short size, short* box, ITEM_INFO* item)
{
	long x0, x1, z0, z1, midX, midZ, xAdd, zAdd;

	if (tomb3.shadow_mode == SHADOW_PSX && GnGameMode != GAMEMODE_IN_CUTSCENE)
	{
		S_PrintSpriteShadow(size, box, item);
		return;
	}

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
	bBlueEffect = underwater;
}

void S_SetupBelowWater(long underwater)
{
	water_effect = !underwater;
	bBlueEffect = 1;
}

long GetFixedScale(long unit)	//some things require fixed scale to look proper all the time. mostly effects, like snow and rain etc.
{
	long w, h, x, y;

	w = 640;
	h = 480;
	x = (phd_winwidth > w) ? MulDiv(phd_winwidth, unit, w) : unit;
	y = (phd_winheight > h) ? MulDiv(phd_winheight, unit, h) : unit;
	return x < y ? x : y;
}

long GetRenderScale(long unit)	//User selected scale
{
	long w, h, x, y;

	w = long(640.0F / (VidSizeLocked ? tomb3.INV_Scale : tomb3.GUI_Scale));
	h = long(480.0F / (VidSizeLocked ? tomb3.INV_Scale : tomb3.GUI_Scale));
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

static void GetRots(short obj_num, short& x, short& z)
{
	for (int i = 0; i < 32; i++)
	{
		if (rots[i].obj_num == obj_num)
		{
			x = rots[i].rot_x;
			z = rots[i].rot_z;
			return;
		}
	}

	x = 0;
	z = 0;
}

static void SetPickupLight()
{
	long x, y, z;

	//ambient
	smcr = 64;
	smcg = 64;
	smcb = 64;

	//colors
	LightCol[M00] = 3072;
	LightCol[M10] = 1680;	//sun
	LightCol[M20] = 640;

	LightCol[M01] = 1024;
	LightCol[M11] = 1024;	//spot
	LightCol[M21] = 1024;

	LightCol[M02] = 640;
	LightCol[M12] = 2432;	//dynamic
	LightCol[M22] = 4080;

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
	{
		for (int i = 0; i < indices_count; i++)
			LightCol[i] >>= 1;

		smcr >>= 1;
		smcg >>= 1;
		smcb >>= 1;
	}
#endif

	//positions
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
}

static void phd_PutPolygonsPickup(short* objptr, float x, float y)
{
	short* newPtr;
	float fx, fy;

	fx = f_centerx;
	fy = f_centery;
	f_centerx = x;
	f_centery = y;

	phd_leftfloat = (float)phd_winxmin;
	phd_topfloat = (float)phd_winymin;
	phd_rightfloat = float(phd_winxmax + phd_winxmin + 1);
	phd_bottomfloat = float(phd_winymax + phd_winymin + 1);
	objptr += 4;
	newPtr = calc_object_vertices(objptr);

	if (newPtr)
	{
		SetPickupLight();
		newPtr = calc_vertice_light(newPtr, objptr);
		newPtr = InsertObjectGT4(newPtr + 1, *newPtr, MID_SORT);
		newPtr = InsertObjectGT3(newPtr + 1, *newPtr, MID_SORT);
		newPtr = InsertObjectG4(newPtr + 1, *newPtr, MID_SORT);
		InsertObjectG3(newPtr + 1, *newPtr, MID_SORT);
	}

	f_centerx = fx;
	f_centery = fy;
}

static void DrawPickup(short obj_num)
{
	float x, y;
	short rotx, rotz;

	x = float(phd_winxmin + phd_winxmax - GetFixedScale(75));
	y = float(phd_winymax + phd_winymin - GetFixedScale(75));
	GetRots(obj_num, rotx, rotz);

	phd_LookAt(0, -1024, 0, 0, 0, 0, 0);

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 1024);
	phd_RotYXZ(PickupY, rotx, rotz);
	phd_PutPolygonsPickup(meshes[objects[obj_num].mesh_index], x + PickupX, y);	
	phd_PopMatrix();
}

static void OutputPickupDisplay()
{
	DXClearBuffers(8, 0);

	if (App.lpDXConfig->bZBuffer)
	{
		for (int i = 0; i < MAX_BUCKETS; i++)
		{
			Buckets[i].TPage = (DXTEXTURE*)-1;
			Buckets[i].nVtx = 0;
		}

#if (DIRECT3D_VERSION < 0x900)
		HWR_EnableColorKey(0);
#endif
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorAddition(0);
		HWR_EnableZBuffer(1, 1);
	}

	phd_InitPolyList();

	if (level_complete)
		InitialisePickUpDisplay();

	bBlueEffect = 0;
	DrawPickup(pickups[CurrentPickup].sprnum);

	if (App.lpDXConfig->bZBuffer)
	{
		if (bAlphaTesting)
		{
#if (DIRECT3D_VERSION >= 0x900)
			SetRenderState(D3DRS_ALPHATESTENABLE, 0);
			DrawBuckets();
			SetRenderState(D3DRS_ALPHATESTENABLE, 1);
#else
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
			DrawBuckets();
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
#endif
			phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
			HWR_DrawPolyListBF(surfacenumbf, sort3d_bufferbf);
			HWR_EnableZBuffer(0, 1);
#if (DIRECT3D_VERSION >= 0x900)
			SetRenderState(D3DRS_ALPHATESTENABLE, 0);
#else
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
#endif
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
#if (DIRECT3D_VERSION < 0x900)
		HWR_EnableColorKey(0);
#endif
		HWR_EnableAlphaBlend(0);
		phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
		HWR_DrawPolyList(surfacenumbf, sort3d_bufferbf);
	}
}

void S_OutputPolyList()
{
	if (App.lpDXConfig->bZBuffer)
	{
#if (DIRECT3D_VERSION < 0x900)
		HWR_EnableColorKey(0);
#endif
		HWR_EnableAlphaBlend(0);
		HWR_EnableColorAddition(0);
		HWR_EnableZBuffer(1, 1);
		HWR_SetCurrentTexture(0);

		if (bAlphaTesting)
		{
#if (DIRECT3D_VERSION >= 0x900)
			SetRenderState(D3DRS_ALPHATESTENABLE, 0);
			DrawBuckets();
			SetRenderState(D3DRS_ALPHATESTENABLE, 1);
#else
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
			DrawBuckets();
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
#endif
			phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
			HWR_DrawPolyListBF(surfacenumbf, sort3d_bufferbf);
			HWR_EnableZBuffer(0, 1);
#if (DIRECT3D_VERSION >= 0x900)
			SetRenderState(D3DRS_ALPHATESTENABLE, 0);
#else
			SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);
#endif
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
#if (DIRECT3D_VERSION < 0x900)
		HWR_EnableColorKey(0);
#endif
		HWR_EnableAlphaBlend(0);
		phd_SortPolyList(surfacenumbf, sort3d_bufferbf);
		HWR_DrawPolyList(surfacenumbf, sort3d_bufferbf);
	}

	if (pickups[CurrentPickup].duration != -1 && !Inventory_Displaying)
		OutputPickupDisplay();

	S_FadePicture();
	HWR_EndScene();
}

void S_InsertBackPolygon(long xmin, long ymin, long xmax, long ymax, long col)
{
	InsertFlatRect(phd_winxmin + xmin, phd_winymin + ymin, phd_winxmin + xmax, phd_winymin + ymax, phd_zfar, col);
}

long S_GetObjectBounds(short* box)
{
	long* v;
	long vtx[8][3];
	long xmin, xmax, ymin, ymax, zmin, zmax, nZ, x, y, z;

	if (phd_mxptr[M23] >= phd_zfar && !outside)
		return 0;

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
	result[0] = (w2v_matrix[M00] * x + w2v_matrix[M01] * y + w2v_matrix[M02] * z) >> W2V_SHIFT;
	result[1] = (w2v_matrix[M10] * x + w2v_matrix[M11] * y + w2v_matrix[M12] * z) >> W2V_SHIFT;
	result[2] = (w2v_matrix[M20] * x + w2v_matrix[M21] * y + w2v_matrix[M22] * z) >> W2V_SHIFT;
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
	if (!framedump)
		return 0;

#if (DIRECT3D_VERSION < 0x900)
	DXSaveScreen(App.FrontBuffer);
#endif
	return 1;
}

void S_InitialiseScreen(long type)
{
	if (type > 0)
		TempVideoRemove();

	HWR_InitState();
}

void ScreenPartialDump()
{
	DXUpdateFrame(1, &phd_WindowRect);
}

long S_DumpScreen()
{
	long nFrames;

	if (framedump)
		nFrames = TICKS_PER_FRAME;
	else
		nFrames = SyncTicks(TICKS_PER_FRAME);

	ScreenPartialDump();
	return nFrames;
}

void ScreenClear(bool a)
{
	DXClearBuffers(2, 0);
}

void S_ClearScreen()
{
	ScreenClear(0);
}

void AnimateTextures(long n)
{
	PHDTEXTURESTRUCT tex;
	short* range;
	static long comp;
	long nFrames;
	short nRanges, nRangeFrames;

	nFrames = 5 * TICKS_PER_FRAME / 2;

	for (comp += n; comp > nFrames; comp -= nFrames)
	{
		nRanges = *aranges;
		range = aranges + 1;

		for (int i = 0; i < nRanges; i++)
		{
			nRangeFrames = *range++;

			tex = phdtextinfo[range[0]];

			while (nRangeFrames > 0)
			{
				phdtextinfo[range[0]] = phdtextinfo[range[1]];
				range++;
				nRangeFrames--;
			}

			phdtextinfo[range[0]] = tex;
			range++;
		}
	}
}

void S_AnimateTextures(long n)
{
	AnimateTextures(n);
}

void S_InitialisePolyList(bool clearBackBuffer)
{
	ulong flags;

	nPolyType = 0;

	if (GtFullScreenClearNeeded)
	{
#if (DIRECT3D_VERSION < 0x900)
		DXCheckForLostSurfaces();
#endif
		DD_SpinMessageLoop(0);
#if (DIRECT3D_VERSION < 0x900)
		DXDoFlipWait();
#endif
		DXClearBuffers(3, 0);
		GtFullScreenClearNeeded = 0;
		clearBackBuffer = 0;
	}

	flags = 256;

	if (clearBackBuffer || HWConfig.nFillMode < D3DFILL_SOLID)
		flags |= 2;

	flags |= 8;

	DXClearBuffers(flags, 0);
	HWR_BeginScene();
	phd_InitPolyList();
}
