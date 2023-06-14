#include "../tomb3/pch.h"
#include "hwinsert.h"
#include "../specific/hwrender.h"
#include "../specific/file.h"
#include "../specific/texture.h"
#include "3d_gen.h"
#include "../specific/init.h"
#include "../specific/winmain.h"
#include "../specific/output.h"
#include "../game/draw.h"
#include "../game/control.h"
#include "../tomb3/tomb3.h"
#include "../game/gameflow.h"

static float UVTable[65536];
static VERTEX_INFO v_buffer[MAX_VINFO];

PHD_VBUF vbuf[MAX_VBUF];
TEXTUREBUCKET Buckets[MAX_BUCKETS];

long GlobalAlpha;
long nDrawnPoints;

static __inline bool CheckDrawType(long nDrawType)
{
	return nDrawType == DT_POLY_WGTA || nDrawType == DT_POLY_GA || nDrawType == DT_POLY_GTA ||
		nDrawType == DT_LINE_SOLID || nDrawType == DT_LINE_ALPHA;
}

static bool SetBufferPtrs(long** sort, short** info, long nDrawType, bool pass)
{
	if (CurrentTLVertex - VertexBuffer > MAX_TLVERTICES - 32)
		return 0;

	if (!App.lpDXConfig->bZBuffer || !bAlphaTesting)
	{
		*sort = sort3dptrbf;
		*info = info3dptrbf;
		sort[0][2] = nPolyType;
		surfacenumbf++;
		sort3dptrbf += 3;
		info3dptrbf += 5;
	}
	else if (pass || CheckDrawType(nDrawType))
	{
		*sort = sort3dptrfb;
		*info = info3dptrfb;
		sort[0][2] = nPolyType;
		surfacenumfb++;
		sort3dptrfb += 3;
		info3dptrfb += 5;
	}
	else
	{
		*sort = sort3dptrbf;
		*info = info3dptrbf;
		sort[0][2] = nPolyType;
		surfacenumbf++;
		sort3dptrbf += 3;
		info3dptrbf += 5;
	}

	return 1;
}

void SetFunctionPointers()
{
	InsertObjectGT3 = HWI_InsertObjectGT3_Sorted;
	InsertObjectGT4 = HWI_InsertObjectGT4_Sorted;
	InsertObjectG3 = HWI_InsertObjectG3_Sorted;
	InsertObjectG4 = HWI_InsertObjectG4_Sorted;
	InsertFlatRect = HWI_InsertFlatRect_Sorted;
	InsertLine = HWI_InsertLine_Sorted;

	InsertSprite = HWI_InsertSprite_Sorted;
	InsertTrans8 = HWI_InsertTrans8_Sorted;
	InsertTransQuad = HWI_InsertTransQuad_Sorted;
	InsertGourQuad = HWI_InsertGourQuad_Sorted;

	IsVisible = CheckVisible;
	IsInvisible = CheckInvisible;
}

bool CheckVisible(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2)
{
	return (v2->xs - v1->xs) * (v0->ys - v1->ys) - (v0->xs - v1->xs) * (v2->ys - v1->ys) > 0;
}

bool CheckInvisible(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2)
{
	return (v2->xs - v1->xs) * (v0->ys - v1->ys) - (v0->xs - v1->xs) * (v2->ys - v1->ys) < 0;
}

void SetCullCW()
{
	IsVisible = CheckVisible;
	IsInvisible = CheckInvisible;
}

void SetCullCCW()
{
	IsVisible = CheckInvisible;
	IsInvisible = CheckVisible;
}

void InitUVTable()
{
	for (int i = 0; i < 65536; i++)
		UVTable[i] = float(i + 1) * (1.0F / 65536.0F);
}

static long GETR(ulong col)
{
	long w, r;

	r = RGB_GETRED(col);
	r = ColorTable[r];

	if (bBlueEffect)
	{
		if (tomb3.custom_water_color || gameflow.force_water_color)
		{
			w = RGB_GETRED(water_color[CurrentLevel]);
			return (w * r) >> 8;
		}
		else
			return (128 * r) >> 8;
	}

	return r;
}

static long GETG(ulong col)
{
	long w, g;

	g = RGB_GETGREEN(col);
	g = ColorTable[g];

	if (bBlueEffect)
	{
		if (tomb3.custom_water_color || gameflow.force_water_color)
		{
			w = RGB_GETGREEN(water_color[CurrentLevel]);
			return (w * g) >> 8;
		}
		else
			return (224 * g) >> 8;
	}

	return g;
}

static long GETB(ulong col)
{
	long w, b;

	b = RGB_GETBLUE(col);
	b = ColorTable[b];

	if (bBlueEffect)
	{
		if (tomb3.custom_water_color || gameflow.force_water_color)
		{
			w = RGB_GETBLUE(water_color[CurrentLevel]);
			return (w * b) >> 8;
		}
	}

	return b;
}

static void PHD_VBUF_To_D3DTLVTX(PHD_VBUF* phdV, D3DTLVERTEX* v)
{
	long r, g, b;

	v->sx = phdV->xs;
	v->sy = phdV->ys;
	v->sz = f_a - f_boo * phdV->ooz;
	v->rhw = phdV->ooz;
	r = GETR(phdV->color);
	g = GETG(phdV->color);
	b = GETB(phdV->color);
	v->color = GlobalAlpha | (r << 16) | (g << 8) | b;
	v->specular = 0;
}

static void PHD_VBUF_To_D3DTLVTX_WITHUV(PHD_VBUF* phdV, D3DTLVERTEX* v, ushort* uv)
{
	long r, g, b;

	v->sx = phdV->xs;
	v->sy = phdV->ys;
	v->sz = f_a - f_boo * phdV->ooz;
	v->rhw = phdV->ooz;
	r = GETR(phdV->color);
	g = GETG(phdV->color);
	b = GETB(phdV->color);
	v->tu = UVTable[uv[0]];
	v->tv = UVTable[uv[1]];
	v->color = GlobalAlpha | (r << 16) | (g << 8) | b;
	v->specular = 0;
}

static void PHD_VBUF_To_VERTEX_INFO(PHD_VBUF* phdV, VERTEX_INFO* v)
{
	v->x = phdV->xs;
	v->y = phdV->ys;
	v->ooz = phdV->ooz;
	v->vr = GETR(phdV->color);
	v->vg = GETG(phdV->color);
	v->vb = GETB(phdV->color);

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
	{
		v->vr <<= 1;
		if (v->vr > 255) v->vr = 255;

		v->vg <<= 1;
		if (v->vg > 255) v->vg = 255;

		v->vb <<= 1;
		if (v->vb > 255) v->vb = 255;
	}
#endif
}

static void PHD_VBUF_To_VERTEX_INFO_WITHUV(PHD_VBUF* phdV, VERTEX_INFO* v, ushort* uv)
{
	v->x = phdV->xs;
	v->y = phdV->ys;
	v->ooz = phdV->ooz;
	v->vr = GETR(phdV->color);
	v->vg = GETG(phdV->color);
	v->vb = GETB(phdV->color);
	v->u = phdV->ooz * uv[0];
	v->v = phdV->ooz * uv[1];
}

static void PHD_VBUF_To_POINT_INFO(PHD_VBUF* v, POINT_INFO* point)
{
	point->xv = v->xv;
	point->yv = v->yv;
	point->zv = v->zv;
	point->ooz = v->ooz;
	point->xs = v->xs;
	point->ys = v->ys;
	point->vr = GETR(v->color);
	point->vg = GETG(v->color);
	point->vb = GETB(v->color);

#if (DIRECT3D_VERSION >= 0x900)
	if (tomb3.psx_contrast)
	{
		point->vr <<= 1;
		if (point->vr > 255) point->vr = 255;

		point->vg <<= 1;
		if (point->vg > 255) point->vg = 255;

		point->vb <<= 1;
		if (point->vb > 255) point->vb = 255;
	}
#endif
}

static void PHD_VBUF_To_POINT_INFO_WITHUV(PHD_VBUF* v, POINT_INFO* point, ushort* uv)
{
	point->xv = v->xv;
	point->yv = v->yv;
	point->zv = v->zv;
	point->ooz = v->ooz;
	point->xs = v->xs;
	point->ys = v->ys;
	point->vr = GETR(v->color);
	point->vg = GETG(v->color);
	point->vb = GETB(v->color);
	point->u = (float)uv[0];
	point->v = (float)uv[1];
}

long visible_zclip(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2)
{
	return (v2->xv * v0->zv - v0->xv * v2->zv) * v1->yv +
		(v0->xv * v2->yv - v2->xv * v0->yv) * v1->zv +
		(v0->yv * v2->zv - v2->yv * v0->zv) * v1->xv < 0;
}

long FindBucket(DXTEXTURE* TPage)
{
	TEXTUREBUCKET* bucket;
	long nVtx, fullest;

	if (nDrawnPoints <= 2700)	//HACK: this seems to be a useless artifical limit (not sure though),
								//so instead of failing to draw, immediately go find fullest bucket, draw it, and use it.
								//TODO: make sure it's actually a useless limit and remove it, otherwise raise it.
	{
		for (int i = 0; i < MAX_BUCKETS; i++)
		{
			bucket = &Buckets[i];

			if (bucket->TPage == TPage && bucket->nVtx < (BUCKET_VERTS - BUCKET_EXTRA))
				return i;

			if (bucket->nVtx > (BUCKET_VERTS - BUCKET_EXTRA))
			{
				HWR_EnableZBuffer(1, 1);
				HWR_SetCurrentTexture(bucket->TPage);
#if (DIRECT3D_VERSION >= 0x900)
				DrawPrimitive(D3DPT_TRIANGLELIST, bucket->vtx, bucket->nVtx);
#else
				DrawPrimitive(D3DPT_TRIANGLELIST, D3DVT_TLVERTEX, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
#endif
				bucket->TPage = TPage;
				bucket->nVtx = 0;
				return i;
			}
		}
	}

	nVtx = 0;
	fullest = 0;

	for (int i = 0; i < MAX_BUCKETS; i++)
	{
		bucket = &Buckets[i];

		if (bucket->TPage == (DXTEXTURE*)-1)
		{
			bucket->TPage = TPage;
			return i;
		}

		if (bucket->nVtx > nVtx)
		{
			nVtx = bucket->nVtx;
			fullest = i;
		}
	}

	bucket = &Buckets[fullest];
	HWR_EnableZBuffer(1, 1);
	HWR_SetCurrentTexture(bucket->TPage);
#if (DIRECT3D_VERSION >= 0x900)
	DrawPrimitive(D3DPT_TRIANGLELIST, bucket->vtx, bucket->nVtx);
#else
	DrawPrimitive(D3DPT_TRIANGLELIST, D3DVT_TLVERTEX, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
#endif
	bucket->TPage = TPage;
	bucket->nVtx = 0;
	return fullest;
}

void DrawBuckets()
{
	TEXTUREBUCKET* bucket;

	for (int i = 0; i < MAX_BUCKETS; i++)
	{
		bucket = &Buckets[i];

		if (bucket->nVtx)
		{
			HWR_SetCurrentTexture(bucket->TPage);
#if (DIRECT3D_VERSION >= 0x900)
			DrawPrimitive(D3DPT_TRIANGLELIST, bucket->vtx, bucket->nVtx);
#else
			DrawPrimitive(D3DPT_TRIANGLELIST, D3DVT_TLVERTEX, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
#endif
		}
	}
}

void SubdivideEdge(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* dest)
{
	float zv;
	short r, g, b, r0, g0, b0, r1, g1, b1;

	dest->xv = (v1->xv + v0->xv) * 0.5F;
	dest->yv = (v0->yv + v1->yv) * 0.5F;
	dest->zv = (v0->zv + v1->zv) * 0.5F;

	if (dest->zv <= phd_znear)
		dest->clip = -128;
	else
	{
		zv = f_persp / dest->zv;
		dest->xs = dest->xv * zv + f_centerx;
		dest->ys = dest->yv * zv + f_centery;
		dest->ooz = zv * f_oneopersp;
		dest->clip = 0;

		if (dest->xs < phd_leftfloat)
			dest->clip++;
		else if (dest->xs > phd_rightfloat)
			dest->clip += 2;

		if (dest->ys < phd_topfloat)
			dest->clip += 4;
		else if (dest->ys > phd_bottomfloat)
			dest->clip += 8;
	}

	r0 = RGB_GETRED(v0->color);
	r1 = RGB_GETRED(v1->color);

	g0 = RGB_GETGREEN(v0->color);
	g1 = RGB_GETGREEN(v1->color);

	b0 = RGB_GETBLUE(v0->color);
	b1 = RGB_GETBLUE(v1->color);

	r = (r0 + r1) >> 1;
	g = (g0 + g1) >> 1;
	b = (b0 + b1) >> 1;
	dest->color = RGB_MAKE(r, g, b);

	dest->u = (v0->u + v1->u) >> 1;
	dest->v = (v0->v + v1->v) >> 1;
}

void SubdivideGT4(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num)
{
	PHDTEXTURESTRUCT tex;
	PHD_VBUF v12;
	PHD_VBUF v34;
	PHD_VBUF v14;
	PHD_VBUF v23;
	PHD_VBUF v13;

	if (!num)
	{
		HWI_InsertGT4_Poly(v1, v2, v3, v4, pTex, nSortType, double_sided);
		return;
	}

	tex.drawtype = pTex->drawtype;
	tex.tpage = pTex->tpage;
	v1->u = pTex->u1;
	v1->v = pTex->v1;
	v2->u = pTex->u2;
	v2->v = pTex->v2;
	v3->u = pTex->u3;
	v3->v = pTex->v3;
	v4->u = pTex->u4;
	v4->v = pTex->v4;
	SubdivideEdge(v1, v2, &v12);
	SubdivideEdge(v3, v4, &v34);
	SubdivideEdge(v1, v4, &v14);
	SubdivideEdge(v2, v3, &v23);
	SubdivideEdge(v1, v3, &v13);
	tex.u1 = v1->u;
	tex.v1 = v1->v;
	tex.u2 = v12.u;
	tex.v2 = v12.v;
	tex.u3 = v13.u;
	tex.v3 = v13.v;
	tex.v4 = v14.v;
	tex.u4 = v14.u;
	SubdivideGT4(v1, &v12, &v13, &v14, &tex, nSortType, double_sided, num - 1);
	tex.u1 = v12.u;
	tex.v1 = v12.v;
	tex.u2 = v2->u;
	tex.v2 = v2->v;
	tex.u3 = v23.u;
	tex.v3 = v23.v;
	tex.u4 = v13.u;
	tex.v4 = v13.v;
	SubdivideGT4(&v12, v2, &v23, &v13, &tex, nSortType, double_sided, num - 1);
	tex.u1 = v13.u;
	tex.v1 = v13.v;
	tex.u2 = v23.u;
	tex.v2 = v23.v;
	tex.u3 = v3->u;
	tex.v3 = v3->v;
	tex.u4 = v34.u;
	tex.v4 = v34.v;
	SubdivideGT4(&v13, &v23, v3, &v34, &tex, nSortType, double_sided, num - 1);
	tex.u1 = v14.u;
	tex.v1 = v14.v;
	tex.u2 = v13.u;
	tex.v2 = v13.v;
	tex.u3 = v34.u;
	tex.v3 = v34.v;
	tex.u4 = v4->u;
	tex.v4 = v4->v;
	SubdivideGT4(&v14, &v13, &v34, v4, &tex, nSortType, double_sided, num - 1);
}

void SubdivideGT3(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided, long num)
{
	PHD_VBUF v12;
	PHD_VBUF v23;
	PHD_VBUF v31;
	PHDTEXTURESTRUCT tex;

	if (!num)
	{
		HWI_InsertGT3_Poly(v1, v2, v3, pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, double_sided);
		return;
	}

	tex.drawtype = pTex->drawtype;
	tex.tpage = pTex->tpage;
	v1->u = pTex->u1;
	v1->v = pTex->v1;
	v2->u = pTex->u2;
	v2->v = pTex->v2;
	v3->u = pTex->u3;
	v3->v = pTex->v3;
	SubdivideEdge(v1, v2, &v12);
	SubdivideEdge(v2, v3, &v23);
	SubdivideEdge(v3, v1, &v31);
	tex.u1 = v1->u;
	tex.v1 = v1->v;
	tex.u2 = v12.u;
	tex.v2 = v12.v;
	tex.u3 = v31.u;
	tex.v3 = v31.v;
	SubdivideGT3(v1, &v12, &v31, &tex, nSortType, double_sided, num - 1);
	tex.u1 = v12.u;
	tex.v1 = v12.v;
	tex.u2 = v2->u;
	tex.v2 = v2->v;
	tex.u3 = v23.u;
	tex.v3 = v23.v;
	SubdivideGT3(&v12, v2, &v23, &tex, nSortType, double_sided, num - 1);
	tex.u1 = v31.u;
	tex.v1 = v31.v;
	tex.u2 = v12.u;
	tex.v2 = v12.v;
	tex.u3 = v23.u;
	tex.v3 = v23.v;
	tex.u4 = v3->u;
	tex.v4 = v3->v;
	SubdivideGT4(&v31, &v12, &v23, v3, &tex, nSortType, double_sided, num - 1);
}

short* HWI_InsertObjectGT3_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType)
{
	PHDTEXTURESTRUCT* pTex;
	long nDrawType;
	ushort double_sided;

	while (nFaces)
	{
		pTex = &phdtextinfo[pFaceInfo[3] & 0x7FFF];
		double_sided = (pFaceInfo[3] >> 0xF) & 1;

		if (pTex->drawtype > 1)
			nDrawType = DT_POLY_WGTA;
		else
			nDrawType = pTex->drawtype + DT_POLY_GT;

		if (nDrawType == DT_POLY_WGT || nDrawType == DT_POLY_WGTA)
			HWI_InsertGT3_Sorted(&vbuf[pFaceInfo[0]], &vbuf[pFaceInfo[1]], &vbuf[pFaceInfo[2]],
				pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, double_sided);
		else if (vbuf[pFaceInfo[0]].color || vbuf[pFaceInfo[1]].color || vbuf[pFaceInfo[2]].color)
			HWI_InsertGT3_Sorted(&vbuf[pFaceInfo[0]], &vbuf[pFaceInfo[1]], &vbuf[pFaceInfo[2]],
				pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, double_sided);
		else
			HWI_InsertObjectG3_Sorted(pFaceInfo, 1, nSortType);

		pFaceInfo += 4;
		nFaces--;
	}

	return pFaceInfo;
}

short* HWI_InsertObjectGT4_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType)
{
	PHDTEXTURESTRUCT* pTex;
	long nDrawType;
	ushort double_sided;

	while (nFaces)
	{
		pTex = &phdtextinfo[pFaceInfo[4] & 0x7FFF];
		double_sided = (pFaceInfo[4] >> 0xF) & 1;

		if (pTex->drawtype > 1)
			nDrawType = DT_POLY_WGTA;
		else
			nDrawType = pTex->drawtype + DT_POLY_GT;

		if (nDrawType == DT_POLY_WGT || nDrawType == DT_POLY_WGTA)
			HWI_InsertGT4_Sorted(&vbuf[pFaceInfo[0]], &vbuf[pFaceInfo[1]], &vbuf[pFaceInfo[2]], &vbuf[pFaceInfo[3]], pTex, nSortType, double_sided);
		else if (vbuf[pFaceInfo[0]].color || vbuf[pFaceInfo[1]].color || vbuf[pFaceInfo[2]].color || vbuf[pFaceInfo[3]].color)
			HWI_InsertGT4_Sorted(&vbuf[pFaceInfo[0]], &vbuf[pFaceInfo[1]], &vbuf[pFaceInfo[2]], &vbuf[pFaceInfo[3]], pTex, nSortType, double_sided);
		else
			HWI_InsertObjectG4_Sorted(pFaceInfo, 1, nSortType);

		pFaceInfo += 5;
		nFaces--;
	}

	return pFaceInfo;
}

short* HWI_InsertObjectG3_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType)
{
	PHD_VBUF* v0;
	PHD_VBUF* v1;
	PHD_VBUF* v2;
	POINT_INFO points[3];
	uchar* pC;
	long nPoints;
	float zdepth;

	while (nFaces)
	{
		v0 = &vbuf[pFaceInfo[0]];
		v1 = &vbuf[pFaceInfo[1]];
		v2 = &vbuf[pFaceInfo[2]];

		if (outside && nPolyType != 1 &&
			v0->zv == f_zfar && v1->zv == f_zfar && v2->zv == f_zfar &&
			v0->ys > outsideBackgroundTop && v1->ys > outsideBackgroundTop && v2->ys > outsideBackgroundTop)
		{
			pFaceInfo += 4;
			nFaces--;
			continue;
		}

		if (v0->clip & v1->clip & v2->clip)
		{
			pFaceInfo += 4;
			nFaces--;
			continue;
		}

		if ((v0->clip | v1->clip | v2->clip) < 0)
		{
			if (!visible_zclip(v0, v1, v2))
			{
				pFaceInfo += 4;
				nFaces--;
				continue;
			}

			PHD_VBUF_To_POINT_INFO(v0, &points[0]);
			PHD_VBUF_To_POINT_INFO(v1, &points[1]);
			PHD_VBUF_To_POINT_INFO(v2, &points[2]);

			nPoints = RoomZedClipper(3, points, v_buffer);

			if (nPoints)
			{
				phd_leftfloat = (float)phd_winxmin;
				phd_topfloat = (float)phd_winymin;
				phd_rightfloat = float(phd_winxmin + phd_winwidth);
				phd_bottomfloat = float(phd_winymin + phd_winheight);
				nPoints = XYGClipper(nPoints, v_buffer);
			}
		}
		else
		{
			if (IsInvisible(v0, v1, v2))
			{
				pFaceInfo += 4;
				nFaces--;
				continue;
			}

			PHD_VBUF_To_VERTEX_INFO(v0, &v_buffer[0]);
			PHD_VBUF_To_VERTEX_INFO(v1, &v_buffer[1]);
			PHD_VBUF_To_VERTEX_INFO(v2, &v_buffer[2]);
			nPoints = 3;

			if (v0->clip | v1->clip | v2->clip)
			{
				phd_leftfloat = (float)phd_winxmin;
				phd_topfloat = (float)phd_winymin;
				phd_rightfloat = float(phd_winxmin + phd_winwidth);
				phd_bottomfloat = float(phd_winymin + phd_winheight);
				nPoints = XYGClipper(nPoints, v_buffer);
			}
		}

		if (nPoints)
		{
			pC = &G_GouraudPalette[(pFaceInfo[3] >> 6) & 0x3FC];

			if (nSortType == MID_SORT)
				zdepth = (v0->zv + v1->zv + v2->zv) * 0.33333334F;
			else if (nSortType == FAR_SORT)
			{
				zdepth = v0->zv;

				if (v1->zv > zdepth)
					zdepth = v1->zv;

				if (v2->zv > zdepth)
					zdepth = v2->zv;
			}
			else
				zdepth = 1000000000;

			HWI_InsertPoly_Gouraud(nPoints, zdepth, pC[0], pC[1], pC[2], DT_POLY_G);
		}

		pFaceInfo += 4;
		nFaces--;
	}

	return pFaceInfo;
}

short* HWI_InsertObjectG4_Sorted(short* pFaceInfo, long nFaces, sort_type nSortType)
{
	PHD_VBUF* v0;
	PHD_VBUF* v1;
	PHD_VBUF* v2;
	PHD_VBUF* v3;
	POINT_INFO points[4];
	uchar* pC;
	float zdepth;
	long nPoints;

	while (nFaces)
	{
		v0 = &vbuf[pFaceInfo[0]];
		v1 = &vbuf[pFaceInfo[1]];
		v2 = &vbuf[pFaceInfo[2]];
		v3 = &vbuf[pFaceInfo[3]];

		if (outside && nPolyType != 1 &&
			v0->zv == f_zfar && v1->zv == f_zfar && v2->zv == f_zfar && v3->zv == f_zfar &&
			v0->ys > outsideBackgroundTop && v1->ys > outsideBackgroundTop && v2->ys > outsideBackgroundTop && v3->ys > outsideBackgroundTop)
		{
			pFaceInfo += 5;
			nFaces--;
			continue;
		}

		if (v0->clip & v1->clip & v2->clip & v3->clip)
		{
			pFaceInfo += 5;
			nFaces--;
			continue;
		}

		if ((v0->clip | v1->clip | v2->clip | v3->clip) < 0)
		{
			if (!visible_zclip(v0, v1, v2))
			{
				pFaceInfo += 5;
				nFaces--;
				continue;
			}

			PHD_VBUF_To_POINT_INFO(v0, &points[0]);
			PHD_VBUF_To_POINT_INFO(v1, &points[1]);
			PHD_VBUF_To_POINT_INFO(v2, &points[2]);
			PHD_VBUF_To_POINT_INFO(v3, &points[3]);
			nPoints = RoomZedClipper(4, points, v_buffer);

			if (nPoints)
			{
				phd_leftfloat = (float)phd_winxmin;
				phd_topfloat = (float)phd_winymin;
				phd_rightfloat = float(phd_winxmin + phd_winwidth);
				phd_bottomfloat = float(phd_winymin + phd_winheight);
				nPoints = XYGClipper(nPoints, v_buffer);
			}
		}
		else
		{
			if (IsInvisible(v0, v1, v2))
			{
				pFaceInfo += 5;
				nFaces--;
				continue;
			}

			PHD_VBUF_To_VERTEX_INFO(v0, &v_buffer[0]);
			PHD_VBUF_To_VERTEX_INFO(v1, &v_buffer[1]);
			PHD_VBUF_To_VERTEX_INFO(v2, &v_buffer[2]);
			PHD_VBUF_To_VERTEX_INFO(v3, &v_buffer[3]);	//actually survived the inline
			nPoints = 4;

			if (v0->clip | v1->clip | v2->clip | v3->clip)
			{
				phd_leftfloat = (float)phd_winxmin;
				phd_topfloat = (float)phd_winymin;
				phd_rightfloat = float(phd_winxmin + phd_winwidth);
				phd_bottomfloat = float(phd_winymin + phd_winheight);
				nPoints = XYGClipper(nPoints, v_buffer);
			}
		}

		if (nPoints)
		{
			pC = &G_GouraudPalette[(pFaceInfo[4] >> 6) & 0x3FC];

			if (nSortType == MID_SORT)
				zdepth = (v0->zv + v1->zv + v2->zv + v3->zv) * 0.25F;
			else if (nSortType == FAR_SORT)
			{
				zdepth = v0->zv;

				if (v1->zv > zdepth)
					zdepth = v1->zv;

				if (v2->zv > zdepth)
					zdepth = v2->zv;

				if (v3->zv > zdepth)
					zdepth = v3->zv;
			}
			else
				zdepth = 1000000000;

			HWI_InsertPoly_Gouraud(nPoints, zdepth, pC[0], pC[1], pC[2], DT_POLY_G);
		}

		pFaceInfo += 5;
		nFaces--;
	}

	return pFaceInfo;
}

void HWI_InsertFlatRect_Sorted(long x1, long y1, long x2, long y2, long zdepth, long col)
{
	TEXTUREBUCKET* bucket;
	D3DTLVERTEX* v;
	long* sort;
	short* info;
	float z;
	long nBucket;

	if (x2 <= x1 || y2 <= y1)
		return;

	if (x1 < phd_winxmin)
		x1 = phd_winxmin;

	if (y1 < phd_winymin)
		y1 = phd_winymin;

	if (x2 > phd_winwidth + phd_winxmin)
		x2 = phd_winwidth + phd_winxmin;

	if (y2 > phd_winheight + phd_winymin)
		x2 = phd_winheight + phd_winymin;

	z = one / (float)zdepth;

	if (App.lpDXConfig->bZBuffer)
	{
		nBucket = FindBucket(0);

		if (nBucket == -1)
			return;

		bucket = &Buckets[nBucket];
		v = &bucket->vtx[bucket->nVtx];

		v->sx = (float)x1;
		v->sy = (float)y1;
		v->rhw = z;
		v->sz = f_a - v->rhw * f_boo;
		v->color = col;
		v->specular = 0;

		v = &bucket->vtx[bucket->nVtx + 1];
		v->sx = (float)x2;
		v->sy = (float)y1;
		v->rhw = z;
		v->sz = f_a - v->rhw * f_boo;
		v->color = col;
		v->specular = 0;

		v = &bucket->vtx[bucket->nVtx + 2];
		v->sx = (float)x2;
		v->sy = (float)y2;
		v->rhw = z;
		v->sz = f_a - v->rhw * f_boo;
		v->color = col;
		v->specular = 0;

		v = &bucket->vtx[bucket->nVtx + 3];
		v->sx = (float)x1;
		v->sy = (float)y1;
		v->rhw = z;
		v->sz = f_a - v->rhw * f_boo;
		v->color = col;
		v->specular = 0;

		v = &bucket->vtx[bucket->nVtx + 4];
		v->sx = (float)x2;
		v->sy = (float)y2;
		v->rhw = z;
		v->sz = f_a - v->rhw * f_boo;
		v->color = col;
		v->specular = 0;

		v = &bucket->vtx[bucket->nVtx + 5];
		v->sx = (float)x1;
		v->sy = (float)y2;
		v->rhw = z;
		v->sz = f_a - v->rhw * f_boo;
		v->color = col;
		v->specular = 0;

		bucket->nVtx += 6;
		nDrawnPoints += 2;
	}
	else
	{
		if (!SetBufferPtrs(&sort, &info, 0, 0))
			return;

		sort[0] = (long)info;
		sort[1] = zdepth;
		info[0] = DT_POLY_G;
		info[1] = 0;
		info[2] = 4;
		v = CurrentTLVertex;
		*((D3DTLVERTEX**)(info + 3)) = v;

		v[0].sx = (float)x1;
		v[0].sy = (float)y1;
		v[0].rhw = z;
		v[0].sz = f_a - v[0].rhw * f_boo;
		v[0].color = col;
		v[0].specular = 0;

		v[1].sx = (float)x2;
		v[1].sy = (float)y1;
		v[1].rhw = z;
		v[1].sz = f_a - v[1].rhw * f_boo;
		v[1].color = col;
		v[1].specular = 0;

		v[2].sx = (float)x2;
		v[2].sy = (float)y2;
		v[2].rhw = z;
		v[2].sz = f_a - v[2].rhw * f_boo;
		v[2].color = col;
		v[2].specular = 0;

		v[3].sx = (float)x1;
		v[3].sy = (float)y2;
		v[3].rhw = z;
		v[3].sz = f_a - v[3].rhw * f_boo;
		v[3].color = col;
		v[3].specular = 0;

		CurrentTLVertex = v + 4;
	}
}

void HWI_InsertLine_Sorted(long x1, long y1, long x2, long y2, long z, long c0, long c1)
{
	D3DTLVERTEX* v;
	long* sort;
	short* info;

	if (!SetBufferPtrs(&sort, &info, 0, 1))
		return;

	sort[0] = (long)info;
	sort[1] = z;

	if (GlobalAlpha == 0xDEADBEEF)
	{
		info[0] = DT_LINE_ALPHA;
		GlobalAlpha = 0xFF000000;
	}
	else
		info[0] = DT_LINE_SOLID;

	info[1] = 0;
	info[2] = 2;
	v = CurrentTLVertex;
	*((D3DTLVERTEX**)(info + 3)) = v;

	v[0].sx = float(phd_winxmin + x1);
	v[0].sy = float(phd_winymin + y1);
	v[0].rhw = one / (float)z;
	v[0].sz = f_a - v[0].rhw * f_boo;
	v[0].color = GlobalAlpha | c0;
	v[0].specular = 0;

	v[1].sx = float(phd_winxmin + x2);
	v[1].sy = float(phd_winymin + y2);
	v[1].rhw = one / (float)z;
	v[1].sz = f_a - v[1].rhw * f_boo;
	v[1].color = GlobalAlpha | c1;
	v[1].specular = 0;

	CurrentTLVertex = v + 2;
}

void HWI_InsertSprite_Sorted(long zdepth, long x1, long y1, long x2, long y2, long nSprite, ulong shade, ulong shade1, long nDrawType, long offset)
{
	PHDSPRITESTRUCT* sprite;
	VERTEX_INFO* vtx;
	float z, u1, v1, u2, v2;
	long nPoints;
	bool blueEffect;

	if (x2 <= x1 || y2 <= y1 || x2 <= 0 || y2 <= 0 || x1 >= phd_winxmax || y1 >= phd_winymax)
		return;

	blueEffect = bBlueEffect;
	bBlueEffect = 0;

	x1 += phd_winxmin;
	y1 += phd_winymin;
	x2 += phd_winxmin;
	y2 += phd_winymin;

	if (zdepth < phd_znear)
		zdepth = phd_znear;

	if (zdepth >= phd_zfar)
		return;

	sprite = &phdspriteinfo[nSprite];
	z = one / (float)zdepth;
	u1 = float((sprite->offset << 8) & 0xFF00);
	v1 = float(sprite->offset & 0xFF00);
	u2 = (u1 + sprite->width - (float)App.nUVAdd) * z;
	v2 = (v1 + sprite->height - (float)App.nUVAdd) * z;
	u1 = (u1 + (float)App.nUVAdd) * z;
	v1 = (v1 + (float)App.nUVAdd) * z;

	v1 += offset << W2V_SHIFT;
	v2 += offset << W2V_SHIFT;

	vtx = v_buffer;
	nPoints = 4;

	vtx->x = (float)x1;
	vtx->y = (float)y1;
	vtx->ooz = z;
	vtx->vr = GETR(shade);
	vtx->vg = GETG(shade);
	vtx->vb = GETB(shade);
	vtx->u = u1;
	vtx->v = v1;
	vtx++;

	vtx->x = (float)x2;
	vtx->y = (float)y1;
	vtx->ooz = z;
	vtx->vr = GETR(shade);
	vtx->vg = GETG(shade);
	vtx->vb = GETB(shade);
	vtx->u = u2;
	vtx->v = v1;
	vtx++;

	if (shade1 != -1)
		shade = shade1;

	vtx->x = (float)x2;
	vtx->y = (float)y2;
	vtx->ooz = z;
	vtx->vr = GETR(shade);
	vtx->vg = GETG(shade);
	vtx->vb = GETB(shade);
	vtx->u = u2;
	vtx->v = v2;
	vtx++;

	vtx->x = (float)x1;
	vtx->y = (float)y2;
	vtx->ooz = z;
	vtx->vr = GETR(shade);
	vtx->vg = GETG(shade);
	vtx->vb = GETB(shade);
	vtx->u = u1;
	vtx->v = v2;

	if (x1 < phd_winxmin || x2 > phd_winxmin + phd_winwidth || y1 < phd_winymin || y2 > phd_winymin + phd_winheight)
	{
		phd_leftfloat = (float)phd_winxmin;
		phd_rightfloat = float(phd_winxmin + phd_winwidth);
		phd_topfloat = (float)phd_winymin;
		phd_bottomfloat = float(phd_winymin + phd_winheight);
		nPoints = RoomXYGUVClipper(nPoints, v_buffer);
	}

	if (nPoints)
		HWI_InsertClippedPoly_Textured(nPoints, (float)zdepth, nDrawType, sprite->tpage);

	bBlueEffect = blueEffect;
}

void HWI_InsertTrans8_Sorted(PHD_VBUF* buf, short shade)
{
	float z;
	long nPoints, nVtx;
	char clipO, clipA;

	nVtx = 8;
	clipO = 0;
	clipA = -1;

	for (int i = 0; i < nVtx; i++)
	{
		clipO |= buf[i].clip;
		clipA &= buf[i].clip;
	}

	if (clipO < 0 || clipA || IsInvisible(&buf[0], &buf[1], &buf[2]))
		return;

	for (int i = 0; i < nVtx; i++)
	{
		v_buffer[i].x = buf[i].xs;
		v_buffer[i].y = buf[i].ys;
		v_buffer[i].ooz = one / (buf[i].zv - 131072);
		v_buffer[i].vr = 0;
		v_buffer[i].vg = 0;
		v_buffer[i].vb = 0;
	}

	nPoints = nVtx;

	if (clipO)
	{
		phd_leftfloat = (float)phd_winxmin;
		phd_topfloat = (float)phd_winymin;
		phd_rightfloat = float(phd_winxmin + phd_winwidth);
		phd_bottomfloat = float(phd_winymin + phd_winheight);
		nPoints = XYClipper(nPoints, v_buffer);
	}

	if (nPoints)
	{
		z = 0;

		for (int i = 0; i < nVtx; i++)
			z += buf[i].zv;

		z = z * 0.125F - 131072;
		HWI_InsertPoly_Gouraud(nPoints, z, 0, 0, 0, DT_POLY_GA);
	}
}

void HWI_InsertTransQuad_Sorted(long x, long y, long w, long h, long z)
{
	D3DTLVERTEX* v;
	long* sort;
	short* info;
	float zv;

	if (!SetBufferPtrs(&sort, &info, 0, 1))
		return;

	sort[0] = (long)info;
	sort[1] = z;
	info[0] = DT_POLY_GA;
	info[1] = 0;
	info[2] = 4;

	v = CurrentTLVertex;
	*((D3DTLVERTEX**)(info + 3)) = CurrentTLVertex;
	zv = one / (float)z;

	v[0].sx = (float)x;
	v[0].sy = (float)y;
	v[0].sz = f_a - zv * f_boo;
	v[0].rhw = zv;
	v[0].color = 0x50003FFF;

	v[1].sx = float(x + w);
	v[1].sy = (float)y;
	v[1].sz = f_a - zv * f_boo;
	v[1].rhw = zv;
	v[1].color = 0x50003FFF;

	v[2].sx = float(x + w);
	v[2].sy = float(y + h);
	v[2].sz = f_a - zv * f_boo;
	v[2].rhw = zv;
	v[2].color = 0x50003F1F;

	v[3].sx = (float)x;
	v[3].sy = float(y + h);
	v[3].sz = f_a - zv * f_boo;
	v[3].rhw = zv;
	v[3].color = 0x50003F1F;
	CurrentTLVertex = v + 4;
}

void HWI_InsertGourQuad_Sorted(long x0, long y0, long x1, long y1, long z, ulong c0, ulong c1, ulong c2, ulong c3, bool add)
{
	D3DTLVERTEX* v;
	long* sort;
	short* info;
	float zv;

	if (!SetBufferPtrs(&sort, &info, 0, 1))
		return;

	sort[0] = (long)info;
	sort[1] = z;
	info[0] = add ? DT_POLY_GTA : DT_POLY_GA;
	info[1] = 0;
	info[2] = 4;

	v = CurrentTLVertex;
	*((D3DTLVERTEX**)(info + 3)) = CurrentTLVertex;
	zv = one / (float)z;

	v[0].sx = (float)x1;
	v[0].sy = (float)y0;
	v[0].sz = f_a - zv * f_boo;
	v[0].rhw = zv;
	v[0].color = c1;

	v[1].sx = (float)x1;
	v[1].sy = (float)y1;
	v[1].sz = f_a - zv * f_boo;
	v[1].rhw = zv;
	v[1].color = c2;

	v[2].sx = (float)x0;
	v[2].sy = (float)y1;
	v[2].sz = f_a - zv * f_boo;
	v[2].rhw = zv;
	v[2].color = c3;

	v[3].sx = (float)x0;
	v[3].sy = (float)y0;
	v[3].sz = f_a - zv * f_boo;
	v[3].rhw = zv;
	v[3].color = c0;
	CurrentTLVertex = v + 4;
}

void HWI_InsertGT4_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHD_VBUF* v4, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided)
{
	float zv;

	if (App.lpDXConfig->bZBuffer || nPolyType != 3 && nPolyType != 4)
	{
		HWI_InsertGT4_Poly(v1, v2, v3, v4, pTex, nSortType, double_sided);
		return;
	}

	zv = v1->zv;

	if (zv < v2->zv)
		zv = v2->zv;

	if (zv < v3->zv)
		zv = v3->zv;

	if (zv < v4->zv)
		zv = v4->zv;

	if (zv < 0x1F40000)
		SubdivideGT4(v1, v2, v3, v4, pTex, nSortType, double_sided, 2);
	else if (zv < 0x36B0000)
		SubdivideGT4(v1, v2, v3, v4, pTex, nSortType, double_sided, 1);
	else
		HWI_InsertGT4_Poly(v1, v2, v3, v4, pTex, nSortType, double_sided);
}

void HWI_InsertGT3_Sorted(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, ushort* uv1, ushort* uv2, ushort* uv3, sort_type nSortType, ushort double_sided)
{
	float zv;

	if (App.lpDXConfig->bZBuffer || nPolyType != 3 && nPolyType != 4)
	{
		HWI_InsertGT3_Poly(v1, v2, v3, pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, double_sided);
		return;
	}

	zv = v1->zv;

	if (zv < v2->zv)
		zv = v2->zv;

	if (zv < v3->zv)
		zv = v3->zv;

	if (zv < 0x1F40000)
		SubdivideGT3(v1, v2, v3, pTex, nSortType, double_sided, 2);
	else if (zv < 0x36B0000)
		SubdivideGT3(v1, v2, v3, pTex, nSortType, double_sided, 1);
	else
		HWI_InsertGT3_Poly(v1, v2, v3, pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, double_sided);
}

void HWI_InsertGT3_Poly(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2, PHDTEXTURESTRUCT* pTex, ushort* uv0, ushort* uv1, ushort* uv2, sort_type nSortType, ushort double_sided)
{
	PHD_VBUF* swapvtx;
	D3DTLVERTEX* v;
	TEXTUREBUCKET* bucket;
	POINT_INFO points[3];
	long* sort;
	ushort* swapuv;
	short* info;
	float zdepth;
	long nPoints, nDrawType, nBucket;

	if (outside && nPolyType != 1 &&
		v0->zv == f_zfar && v1->zv == f_zfar && v2->zv == f_zfar &&
		v0->ys > outsideBackgroundTop && v1->ys > outsideBackgroundTop && v2->ys > outsideBackgroundTop)
		return;

	if ((v0->clip | v2->clip | v1->clip) < 0)
	{
		PHD_VBUF_To_POINT_INFO_WITHUV(v0, &points[0], uv0);	//these 3 calls are originally inlined
		PHD_VBUF_To_POINT_INFO_WITHUV(v1, &points[1], uv1);
		PHD_VBUF_To_POINT_INFO_WITHUV(v2, &points[2], uv2);
		nPoints = RoomZedClipper(3, points, v_buffer);

		if (!nPoints)
			return;

	drawtextured:
		phd_leftfloat = (float)phd_winxmin;
		phd_rightfloat = float(phd_winxmin + phd_winwidth);
		phd_topfloat = (float)phd_winymin;
		phd_bottomfloat = float(phd_winymin + phd_winheight);
		nPoints = RoomXYGUVClipper(nPoints, v_buffer);

		if (nPoints)
		{
			if (nSortType == MID_SORT)
				zdepth = (v0->zv + v1->zv + v2->zv) * 0.33333334F;
			else if (nSortType == FAR_SORT)
			{
				zdepth = v0->zv;

				if (zdepth < v1->zv)
					zdepth = v1->zv;

				if (zdepth < v2->zv)
					zdepth = v2->zv;
			}
			else
				zdepth = 1000000000;

			if (pTex->drawtype == 3)
				nDrawType = DT_POLY_COLSUB;
			else if (pTex->drawtype > 1)
				nDrawType = DT_POLY_WGTA;
			else
				nDrawType = pTex->drawtype + DT_POLY_GT;

			if (nPolyType == 1)
			{
				if (v0->ys > outsideBackgroundTop)
					outsideBackgroundTop = v0->ys;

				if (v1->ys > outsideBackgroundTop)
					outsideBackgroundTop = v1->ys;

				if (v2->ys > outsideBackgroundTop)
					outsideBackgroundTop = v2->ys;
			}

			HWI_InsertClippedPoly_Textured(nPoints, zdepth, nDrawType, pTex->tpage);
		}

		return;
	}

	if (IsInvisible(v0, v1, v2))
	{
		if (!double_sided)
			return;

		swapvtx = v1;
		v1 = v2;
		v2 = swapvtx;

		swapuv = uv1;
		uv1 = uv2;
		uv2 = swapuv;
	}

	if (v0->clip | v1->clip | v2->clip)
	{
		PHD_VBUF_To_VERTEX_INFO_WITHUV(v0, &v_buffer[0], uv0);	//these 3 calls are originally inlined
		PHD_VBUF_To_VERTEX_INFO_WITHUV(v1, &v_buffer[1], uv1);
		PHD_VBUF_To_VERTEX_INFO_WITHUV(v2, &v_buffer[2], uv2);
		nPoints = 3;
		goto drawtextured;
	}

	if (pTex->drawtype == 3)
		nDrawType = DT_POLY_COLSUB;
	else if (pTex->drawtype > 1)
		nDrawType = DT_POLY_WGTA;
	else
		nDrawType = pTex->drawtype + DT_POLY_GT;

	if (nPolyType == 1)
	{
		if (v0->ys > outsideBackgroundTop)
			outsideBackgroundTop = v0->ys;

		if (v1->ys > outsideBackgroundTop)
			outsideBackgroundTop = v1->ys;

		if (v2->ys > outsideBackgroundTop)
			outsideBackgroundTop = v2->ys;
	}

	if (App.lpDXConfig->bZBuffer && nDrawType != DT_POLY_WGTA && nDrawType != DT_POLY_WGT && nDrawType != DT_POLY_COLSUB)
	{
		nBucket = FindBucket(TexturePtrs[pTex->tpage]);

		if (nBucket != -1)
		{
			nDrawnPoints++;
			bucket = &Buckets[nBucket];

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX_WITHUV(v0, v, uv0);	//inlined
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX_WITHUV(v1, v, uv1);	//inlined
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX_WITHUV(v2, v, uv2);	//the one call that actually survived!!!!!!!!
			bucket->nVtx++;
		}
	}
	else
	{
		if (nSortType == MID_SORT)
			zdepth = (v0->zv + v1->zv + v2->zv) * 0.33333334F;
		else if (nSortType == FAR_SORT)
		{
			zdepth = v0->zv;

			if (zdepth < v1->zv)
				zdepth = v1->zv;

			if (zdepth < v2->zv)
				zdepth = v2->zv;
		}
		else
			zdepth = 1000000000;

		if (!SetBufferPtrs(&sort, &info, nDrawType, 0))
			return;

		sort[0] = (long)info;
		sort[1] = (long)zdepth;
		info[0] = (short)nDrawType;
		info[1] = pTex->tpage;
		info[2] = 3;
		v = CurrentTLVertex;
		*((D3DTLVERTEX**)(info + 3)) = v;
		PHD_VBUF_To_D3DTLVTX_WITHUV(v0, v, uv0);	//all 3 originally inlined
		v++;
		PHD_VBUF_To_D3DTLVTX_WITHUV(v1, v, uv1);
		v++;
		PHD_VBUF_To_D3DTLVTX_WITHUV(v2, v, uv2);
		v++;
		CurrentTLVertex = v;
	}
}

void HWI_InsertGT4_Poly(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, PHDTEXTURESTRUCT* pTex, sort_type nSortType, ushort double_sided)
{
	PHD_VBUF* swap;
	D3DTLVERTEX* v;
	TEXTUREBUCKET* bucket;
	long* sort;
	short* info;
	float zdepth;
	long nDrawType, nBucket;

	if (outside && nPolyType != 1 &&
		v0->zv == f_zfar && v1->zv == f_zfar && v2->zv == f_zfar && v3->zv == f_zfar &&
		v0->ys > outsideBackgroundTop && v1->ys > outsideBackgroundTop && v2->ys > outsideBackgroundTop && v3->ys > outsideBackgroundTop)
		return;

	if (v0->clip & v1->clip & v2->clip & v3->clip)
		return;

	if ((v0->clip | v1->clip | v2->clip | v3->clip) < 0)
	{
		if (double_sided)
		{
			if (visible_zclip(v0, v1, v2))
			{
				HWI_InsertGT3_Poly(v0, v1, v2, pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, 0);
				HWI_InsertGT3_Poly(v0, v2, v3, pTex, &pTex->u1, &pTex->u3, &pTex->u4, nSortType, 0);
			}

			swap = v0;
			v0 = v2;
			v2 = swap;

			if (visible_zclip(v0, v1, v2))
			{
				HWI_InsertGT3_Poly(v0, v1, v2, pTex, &pTex->u3, &pTex->u2, &pTex->u1, nSortType, 0);
				HWI_InsertGT3_Poly(v0, v2, v3, pTex, &pTex->u3, &pTex->u1, &pTex->u4, nSortType, 0);
			}
		}
		else
		{
			if (visible_zclip(v0, v1, v2))
			{
				HWI_InsertGT3_Poly(v0, v1, v2, pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, 0);
				HWI_InsertGT3_Poly(v0, v2, v3, pTex, &pTex->u1, &pTex->u3, &pTex->u4, nSortType, 0);
			}
		}

		return;
	}

	if (IsInvisible(v0, v1, v2))
	{
		if (!double_sided)
			return;

		swap = v0;
		v0 = v2;
		v2 = swap;

		if (v0->clip | v1->clip | v2->clip | v3->clip)
		{
			HWI_InsertGT3_Poly(v0, v1, v2, pTex, &pTex->u3, &pTex->u2, &pTex->u1, nSortType, 0);
			HWI_InsertGT3_Poly(v0, v2, v3, pTex, &pTex->u3, &pTex->u1, &pTex->u4, nSortType, 0);
			return;
		}
	}
	else
		double_sided = 0;

	if (v0->clip | v1->clip | v2->clip | v3->clip)
	{
		HWI_InsertGT3_Poly(v0, v1, v2, pTex, &pTex->u1, &pTex->u2, &pTex->u3, nSortType, double_sided);
		HWI_InsertGT3_Poly(v0, v2, v3, pTex, &pTex->u1, &pTex->u3, &pTex->u4, nSortType, double_sided);
		return;
	}

	if (pTex->drawtype == 3)
		nDrawType = DT_POLY_COLSUB;
	else if (pTex->drawtype > 1)
		nDrawType = DT_POLY_WGTA;
	else
		nDrawType = pTex->drawtype + DT_POLY_GT;

	if (nPolyType == 1)
	{
		if (v0->ys > outsideBackgroundTop)
			outsideBackgroundTop = v0->ys;

		if (v1->ys > outsideBackgroundTop)
			outsideBackgroundTop = v1->ys;

		if (v2->ys > outsideBackgroundTop)
			outsideBackgroundTop = v2->ys;

		if (v3->ys > outsideBackgroundTop)
			outsideBackgroundTop = v3->ys;
	}

	if (App.lpDXConfig->bZBuffer && nDrawType != DT_POLY_WGTA && nDrawType != DT_POLY_WGT && nDrawType != DT_POLY_COLSUB)
	{
		nBucket = FindBucket(TexturePtrs[pTex->tpage]);

		if (nBucket == -1)
			return;

		bucket = &Buckets[nBucket];
		nDrawnPoints += 2;

		if (double_sided)
		{
			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v0, v);
			v->tu = UVTable[pTex->u3];
			v->tv = UVTable[pTex->v3];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v1, v);
			v->tu = UVTable[pTex->u2];
			v->tv = UVTable[pTex->v2];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v2, v);
			v->tu = UVTable[pTex->u1];
			v->tv = UVTable[pTex->v1];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v0, v);
			v->tu = UVTable[pTex->u3];
			v->tv = UVTable[pTex->v3];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v2, v);
			v->tu = UVTable[pTex->u1];
			v->tv = UVTable[pTex->v1];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v3, v);
			v->tu = UVTable[pTex->u4];
			v->tv = UVTable[pTex->v4];
			bucket->nVtx++;
		}
		else
		{
			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v0, v);
			v->tu = UVTable[pTex->u1];
			v->tv = UVTable[pTex->v1];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v1, v);
			v->tu = UVTable[pTex->u2];
			v->tv = UVTable[pTex->v2];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v2, v);
			v->tu = UVTable[pTex->u3];
			v->tv = UVTable[pTex->v3];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v0, v);
			v->tu = UVTable[pTex->u1];
			v->tv = UVTable[pTex->v1];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v2, v);
			v->tu = UVTable[pTex->u3];
			v->tv = UVTable[pTex->v3];
			bucket->nVtx++;

			v = &bucket->vtx[bucket->nVtx];
			PHD_VBUF_To_D3DTLVTX(v3, v);
			v->tu = UVTable[pTex->u4];
			v->tv = UVTable[pTex->v4];
			bucket->nVtx++;
		}
	}
	else
	{
		if (nSortType == MID_SORT)
			zdepth = (v0->zv + v1->zv + v2->zv + v3->zv) * 0.25F;
		else if (nSortType == FAR_SORT)
		{
			zdepth = v0->zv;

			if (zdepth < v1->zv)
				zdepth = v1->zv;

			if (zdepth < v2->zv)
				zdepth = v2->zv;

			if (zdepth < v3->zv)
				zdepth = v3->zv;
		}
		else
			zdepth = 1000000000;

		if (!SetBufferPtrs(&sort, &info, nDrawType, 0))
			return;

		sort[0] = (long)info;
		sort[1] = (long)zdepth;
		info[0] = (short)nDrawType;
		info[1] = pTex->tpage;
		info[2] = 4;

		v = CurrentTLVertex;
		*((D3DTLVERTEX**)(info + 3)) = v;

		if (double_sided)
		{
			PHD_VBUF_To_D3DTLVTX(v0, v);
			v->tu = UVTable[pTex->u3];
			v->tv = UVTable[pTex->v3];
			v++;

			PHD_VBUF_To_D3DTLVTX(v1, v);
			v->tu = UVTable[pTex->u2];
			v->tv = UVTable[pTex->v2];
			v++;

			PHD_VBUF_To_D3DTLVTX(v2, v);
			v->tu = UVTable[pTex->u1];
			v->tv = UVTable[pTex->v1];
			v++;
		}
		else
		{
			PHD_VBUF_To_D3DTLVTX(v0, v);
			v->tu = UVTable[pTex->u1];
			v->tv = UVTable[pTex->v1];
			v++;

			PHD_VBUF_To_D3DTLVTX(v1, v);
			v->tu = UVTable[pTex->u2];
			v->tv = UVTable[pTex->v2];
			v++;

			PHD_VBUF_To_D3DTLVTX(v2, v);
			v->tu = UVTable[pTex->u3];
			v->tv = UVTable[pTex->v3];
			v++;
		}

		PHD_VBUF_To_D3DTLVTX(v3, v);
		v->tu = UVTable[pTex->u4];
		v->tv = UVTable[pTex->v4];
		v++;

		CurrentTLVertex = v;
	}
}

void HWI_InsertClippedPoly_Textured(long nPoints, float zdepth, long nDrawType, long nTPage)
{
	VERTEX_INFO* vtxbuf;
	D3DTLVERTEX* v;
	TEXTUREBUCKET* bucket;
	long* sort;
	short* info;
	float z;
	long nBucket, nVtx;

	vtxbuf = v_buffer;

	if (App.lpDXConfig->bZBuffer && nDrawType != DT_POLY_WGTA && nDrawType != DT_POLY_WGT && nDrawType != DT_POLY_COLSUB)
	{
		nBucket = FindBucket(TexturePtrs[nTPage]);

		if (nBucket == -1)
			return;

		for (int i = 0; i < 3; i++, vtxbuf++)
		{
			bucket = &Buckets[nBucket];
			nVtx = bucket->nVtx;
			v = &bucket->vtx[nVtx];
			v->sx = vtxbuf->x;
			v->sy = vtxbuf->y;
			v->sz = f_a - f_boo * vtxbuf->ooz;
			v->rhw = vtxbuf->ooz;
			v->color = GlobalAlpha | (vtxbuf->vr << 16) | (vtxbuf->vg << 8) | vtxbuf->vb;
			v->specular = 0;
			z = (1.0F / 65536.0F) / vtxbuf->ooz;
			v->tu = vtxbuf->u * z;
			v->tv = vtxbuf->v * z;
			bucket->nVtx++;
		}

		vtxbuf--;
		nPoints -= 3;
		nDrawnPoints++;

		if (nPoints)
		{
			nDrawnPoints += nPoints;

			for (int i = nPoints; i; i--)
			{
				bucket = &Buckets[nBucket];
				nVtx = bucket->nVtx;

				v = &bucket->vtx[nVtx];
				v->sx = v_buffer->x;
				v->sy = v_buffer->y;
				v->sz = f_a - f_boo * v_buffer->ooz;
				v->rhw = v_buffer->ooz;
				v->color = GlobalAlpha | (v_buffer->vr << 16) | (v_buffer->vg << 8) | v_buffer->vb;
				v->specular = 0;
				z = (1.0F / 65536.0F) / v_buffer->ooz;
				v->tu = v_buffer->u * z;
				v->tv = v_buffer->v * z;

				v = &bucket->vtx[nVtx + 1];
				v->sx = vtxbuf->x;
				v->sy = vtxbuf->y;
				v->sz = f_a - f_boo * vtxbuf->ooz;
				v->rhw = vtxbuf->ooz;
				v->color = GlobalAlpha | (vtxbuf->vr << 16) | (vtxbuf->vg << 8) | vtxbuf->vb;
				v->specular = 0;
				z = (1.0F / 65536.0F) / vtxbuf->ooz;
				v->tu = vtxbuf->u * z;
				v->tv = vtxbuf->v * z;

				vtxbuf++;
				v = &bucket->vtx[nVtx + 2];
				v->sx = vtxbuf->x;
				v->sy = vtxbuf->y;
				v->sz = f_a - f_boo * vtxbuf->ooz;
				v->rhw = vtxbuf->ooz;
				v->color = GlobalAlpha | (vtxbuf->vr << 16) | (vtxbuf->vg << 8) | vtxbuf->vb;
				v->specular = 0;
				z = (1.0F / 65536.0F) / vtxbuf->ooz;
				v->tu = vtxbuf->u * z;
				v->tv = vtxbuf->v * z;

				bucket->nVtx += 3;
			}
		}
	}
	else
	{
		if (!SetBufferPtrs(&sort, &info, nDrawType, 0))
			return;

		sort[0] = (long)info;
		sort[1] = (long)zdepth;
		info[0] = (short)nDrawType;
		info[1] = (short)nTPage;
		info[2] = (short)nPoints;
		v = CurrentTLVertex;
		*((D3DTLVERTEX**)(info + 3)) = v;

		for (int i = nPoints; i; i--, v++, vtxbuf++)
		{
			v->sx = vtxbuf->x;
			v->sy = vtxbuf->y;
			v->sz = f_a - f_boo * vtxbuf->ooz;
			v->rhw = vtxbuf->ooz;
			v->color = GlobalAlpha | (vtxbuf->vr << 16) | (vtxbuf->vg << 8) | vtxbuf->vb;
			v->specular = 0;
			z = (1.0F / 65536.0F) / vtxbuf->ooz;
			v->tu = vtxbuf->u * z;
			v->tv = vtxbuf->v * z;
		}

		CurrentTLVertex = v;
	}
}

void HWI_InsertPoly_Gouraud(long nPoints, float zdepth, long r, long g, long b, long nDrawType)
{
	VERTEX_INFO* vtx;
	TEXTUREBUCKET* bucket;
	D3DTLVERTEX* v;
	long* sort;
	short* info;
	ulong maxCol;
	long nBucket;

	vtx = v_buffer;
	maxCol = nDrawType != DT_POLY_GA ? 0xFFFFFFFF : 0x80FFFFFF;

	if (App.lpDXConfig->bZBuffer && nDrawType != DT_POLY_GA)
	{
		nBucket = FindBucket(0);

		if (nBucket == -1)
			return;

		bucket = &Buckets[nBucket];

		for (int i = 0; i < 3; i++)
		{
			v = &bucket->vtx[bucket->nVtx];
			v->sx = vtx->x;
			v->sy = vtx->y;
			v->sz = f_a - f_boo * vtx->ooz;
			v->rhw = vtx->ooz;
			vtx->vr = (vtx->vr * r) >> 8;
			vtx->vg = (vtx->vg * g) >> 8;
			vtx->vb = (vtx->vb * b) >> 8;
			v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;
			v->specular = 0;
			vtx++;
			bucket->nVtx++;
		}

		nDrawnPoints++;
		nPoints -= 3;
		vtx--;

		if (nPoints)
		{
			nDrawnPoints += nPoints;

			for (int i = 0; i < nPoints; i++)
			{
				v = &bucket->vtx[bucket->nVtx];
				v->sx = v_buffer->x;
				v->sy = v_buffer->y;
				v->sz = f_a - f_boo * v_buffer->ooz;
				v->rhw = v_buffer->ooz;
				v->color = (0xFF000000 | (v_buffer->vr << 16) | (v_buffer->vg << 8) | v_buffer->vb) & maxCol;
				v->specular = 0;

				v = &bucket->vtx[bucket->nVtx + 1];
				v->sx = vtx->x;
				v->sy = vtx->y;
				v->sz = f_a - f_boo * vtx->ooz;
				v->rhw = vtx->ooz;
				v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;
				v->specular = 0;

				vtx++;
				vtx->vr = (vtx->vr * r) >> 8;
				vtx->vg = (vtx->vg * g) >> 8;
				vtx->vb = (vtx->vb * b) >> 8;
				v = &bucket->vtx[bucket->nVtx + 2];
				v->sx = vtx->x;
				v->sy = vtx->y;
				v->sz = f_a - f_boo * vtx->ooz;
				v->rhw = vtx->ooz;
				v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;
				v->specular = 0;

				bucket->nVtx += 3;
			}
		}
	}
	else
	{
		if (!SetBufferPtrs(&sort, &info, nDrawType, 0))
			return;

		sort[0] = (long)info;
		sort[1] = (long)zdepth;
		info[0] = (short)nDrawType;
		info[1] = 0;
		info[2] = (short)nPoints;
		v = CurrentTLVertex;
		*((D3DTLVERTEX**)(info + 3)) = v;

		for (int i = nPoints; i; i--, v++, vtx++)
		{
			v->sx = vtx->x;
			v->sy = vtx->y;
			v->rhw = vtx->ooz;
			v->sz = f_a - f_boo * v->rhw;
			vtx->vr = (vtx->vr * r) >> 8;
			vtx->vg = (vtx->vg * g) >> 8;
			vtx->vb = (vtx->vb * b) >> 8;
			v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;
			v->specular = 0;
		}

		CurrentTLVertex = v;
	}
}

void HWI_InsertPoly_GouraudRGB(long nPoints, float zdepth, long nDrawType)
{
	VERTEX_INFO* vtx;
	TEXTUREBUCKET* bucket;
	D3DTLVERTEX* v;
	long* sort;
	short* info;
	ulong maxCol;
	long nBucket;

	maxCol = (nDrawType == DT_POLY_GA || nDrawType == DT_POLY_GTA) ? 0x80FFFFFF : 0xFFFFFFFF;
	vtx = v_buffer;

	if (App.lpDXConfig->bZBuffer && nDrawType != DT_POLY_GA && nDrawType != DT_POLY_GTA)
	{
		nBucket = FindBucket(0);

		if (nBucket == -1)
			return;

		bucket = &Buckets[nBucket];

		for (int i = 0; i < 3; i++)
		{
			v = &bucket->vtx[bucket->nVtx];
			v->sx = vtx->x;
			v->sy = vtx->y;
			v->sz = f_a - f_boo * vtx->ooz;
			v->rhw = vtx->ooz;
			v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;
			vtx++;
			bucket->nVtx++;
		}

		nDrawnPoints++;
		nPoints -= 3;
		vtx--;

		if (nPoints)
		{
			nDrawnPoints += nPoints;

			for (int i = 0; i < nPoints; i++)
			{
				v = &bucket->vtx[bucket->nVtx];
				v->sx = v_buffer->x;
				v->sy = v_buffer->y;
				v->sz = f_a - f_boo * v_buffer->ooz;
				v->rhw = v_buffer->ooz;
				v->color = (0xFF000000 | (v_buffer->vr << 16) | (v_buffer->vg << 8) | v_buffer->vb) & maxCol;

				v = &bucket->vtx[bucket->nVtx + 1];
				v->sx = vtx->x;
				v->sy = vtx->y;
				v->sz = f_a - f_boo * vtx->ooz;
				v->rhw = vtx->ooz;
				v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;

				vtx++;
				v = &bucket->vtx[bucket->nVtx + 2];
				v->sx = vtx->x;
				v->sy = vtx->y;
				v->sz = f_a - f_boo * vtx->ooz;
				v->rhw = vtx->ooz;
				v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;

				bucket->nVtx += 3;
			}
		}
	}
	else
	{
		if (!SetBufferPtrs(&sort, &info, nDrawType, 0))
			return;

		sort[0] = (long)info;
		sort[1] = (long)zdepth;
		info[0] = (short)nDrawType;
		info[1] = 0;
		info[2] = (short)nPoints;
		v = CurrentTLVertex;
		*((D3DTLVERTEX**)(info + 3)) = v;

		for (int i = nPoints; i; i--, v++, vtx++)
		{
			v->sx = vtx->x;
			v->sy = vtx->y;
			v->sz = f_a - f_boo * vtx->ooz;
			v->rhw = vtx->ooz;
			v->color = (0xFF000000 | (vtx->vr << 16) | (vtx->vg << 8) | vtx->vb) & maxCol;
		}

		CurrentTLVertex = v;
	}
}

void HWI_InsertAlphaSprite_Sorted(long x1, long y1, long z1, long shade1, long x2, long y2, long z2, long shade2,
	long x3, long y3, long z3, long shade3, long x4, long y4, long z4, long shade4,
	long nSprite, long nDrawtype, long double_sided)
{
	PHDSPRITESTRUCT* sprite;
	VERTEX_INFO* vtx;
	VERTEX_INFO swap;
	float u1, v1, u2, v2, zdepth;
	long nPoints;
	bool blueEffect;

	blueEffect = bBlueEffect;
	bBlueEffect = 0;
	sprite = 0;	//new line: compiler complains that sprite is possibly uninitialized at the HWI_InsertClippedPoly_Textured call below
				//which is not possible anyway, so 0 is fine

	if (nSprite != -1)
	{
		sprite = &phdspriteinfo[nSprite];
		u1 = float((sprite->offset << 8) & 0xFF00);
		v1 = float(sprite->offset & 0xFF00);
		u2 = u1 + sprite->width - float(App.nUVAdd);
		v2 = v1 + sprite->height - float(App.nUVAdd);
		u1 += float(App.nUVAdd);
		v1 += float(App.nUVAdd);
	}

	vtx = v_buffer;

	vtx->x = (float)x1;
	vtx->y = (float)y1;
	vtx->ooz = one / (float)z1;
	vtx->vr = GETR(shade1);
	vtx->vg = GETG(shade1);
	vtx->vb = GETB(shade1);
	vtx++;

	vtx->x = (float)x2;
	vtx->y = (float)y2;
	vtx->ooz = one / (float)z2;
	vtx->vr = GETR(shade2);
	vtx->vg = GETG(shade2);
	vtx->vb = GETB(shade2);
	vtx++;

	vtx->x = (float)x3;
	vtx->y = (float)y3;
	vtx->ooz = one / (float)z3;
	vtx->vr = GETR(shade3);
	vtx->vg = GETG(shade3);
	vtx->vb = GETB(shade3);
	vtx++;

	vtx->x = (float)x4;
	vtx->y = (float)y4;
	vtx->ooz = one / (float)z4;
	vtx->vr = GETR(shade4);
	vtx->vg = GETG(shade4);
	vtx->vb = GETB(shade4);

	if (nSprite != -1)
	{
		vtx = v_buffer;

		if (!double_sided)
		{
			vtx->u = u1 * vtx->ooz;
			vtx->v = v1 * vtx->ooz;
			vtx++;

			vtx->u = u2 * vtx->ooz;
			vtx->v = v1 * vtx->ooz;
			vtx++;

			vtx->u = u2 * vtx->ooz;
			vtx->v = v2 * vtx->ooz;
			vtx++;

			vtx->u = u1 * vtx->ooz;
			vtx->v = v2 * vtx->ooz;
		}
		else
		{
			vtx->u = u1 * vtx->ooz;
			vtx->v = v1 * vtx->ooz;
			vtx++;

			vtx->u = u1 * vtx->ooz;
			vtx->v = v2 * vtx->ooz;
			vtx++;

			vtx->u = u2 * vtx->ooz;
			vtx->v = v2 * vtx->ooz;
			vtx++;

			vtx->u = u2 * vtx->ooz;
			vtx->v = v1 * vtx->ooz;
		}
	}

	//Change this to support phd_vbuf :>
	if (nSprite == -1 && double_sided &&
		long((v_buffer[0].y - v_buffer[1].y) * (v_buffer[2].x - v_buffer[1].x) - (v_buffer[0].x - v_buffer[1].x) * (v_buffer[2].y - v_buffer[1].y)) < 0)
	{
		swap = v_buffer[0];
		v_buffer[0] = v_buffer[2];
		v_buffer[2] = swap;
	}

	phd_leftfloat = (float)phd_winxmin;
	phd_rightfloat = float(phd_winxmin + phd_winwidth);
	phd_topfloat = (float)phd_winymin;
	phd_bottomfloat = float(phd_winymin + phd_winheight);
	nPoints = 4;

	if (nSprite == -1)
		nPoints = XYGClipper(nPoints, v_buffer);
	else
		nPoints = RoomXYGUVClipper(nPoints, v_buffer);

	if (nPoints)
	{
		zdepth = float((z1 + z2 + z3 + z4) >> 2);

		if (nSprite == -1)
			HWI_InsertPoly_GouraudRGB(nPoints, zdepth, nDrawtype);
		else
			HWI_InsertClippedPoly_Textured(nPoints, zdepth, nDrawtype, sprite->tpage);
	}

	bBlueEffect = blueEffect;
}

long RoomZedClipper(long n, POINT_INFO* in, VERTEX_INFO* out)
{
	POINT_INFO* last;
	POINT_INFO* pIn;
	float lastZ, inZ, dz;
	long nPoints;

	pIn = in;
	last = &in[n - 1];

	for (nPoints = 0; n--; last = pIn++)
	{
		inZ = f_znear - pIn->zv;
		lastZ = f_znear - last->zv;

		if (((*(long*)&lastZ) | (*(long*)&inZ)) >= 0)
			continue;

		if (((*(long*)&lastZ) ^ (*(long*)&inZ)) < 0)
		{
			dz = inZ / (last->zv - pIn->zv);
			out->x = ((last->xv - pIn->xv) * dz + pIn->xv) * f_perspoznear + f_centerx;
			out->y = ((last->yv - pIn->yv) * dz + pIn->yv) * f_perspoznear + f_centery;
			out->ooz = f_oneoznear;
			out->u = ((last->u - pIn->u) * dz + pIn->u) * f_oneoznear;
			out->v = ((last->v - pIn->v) * dz + pIn->v) * f_oneoznear;
			out->vr = long((last->vr - pIn->vr) * dz + pIn->vr);
			out->vg = long((last->vg - pIn->vg) * dz + pIn->vg);
			out->vb = long((last->vb - pIn->vb) * dz + pIn->vb);
			out++;
			nPoints++;
		}

		if ((*(long*)&inZ) < 0)
		{
			out->x = pIn->xs;
			out->y = pIn->ys;
			out->ooz = pIn->ooz;
			out->u = pIn->u * pIn->ooz;
			out->v = pIn->v * pIn->ooz;
			out->vr = pIn->vr;
			out->vg = pIn->vg;
			out->vb = pIn->vb;
			out++;
			nPoints++;
		}
	}

	if (nPoints < 3)
		nPoints = 0;

	return nPoints;
}

static inline void DoClip(VERTEX_INFO* vtx, VERTEX_INFO* v1, VERTEX_INFO* v2, float clipper)
{
	vtx->ooz = (v2->ooz - v1->ooz) * clipper + v1->ooz;
	vtx->u = (v2->u - v1->u) * clipper + v1->u;
	vtx->v = (v2->v - v1->v) * clipper + v1->v;
	vtx->vr = long((v2->vr - v1->vr) * clipper + v1->vr);
	vtx->vg = long((v2->vg - v1->vg) * clipper + v1->vg);
	vtx->vb = long((v2->vb - v1->vb) * clipper + v1->vb);
}

long RoomXYGUVClipper(long n, VERTEX_INFO* in)
{
	VERTEX_INFO* v1;
	VERTEX_INFO* v2;
	static VERTEX_INFO output[8];
	float clipper;
	long nPoints;

	v2 = &in[n - 1];
	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		v2 = &in[i];

		if (v1->x < phd_leftfloat)
		{
			if (v2->x < phd_leftfloat)
				continue;

			clipper = (phd_leftfloat - v2->x) / (v1->x - v2->x);
			DoClip(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_leftfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else if (v1->x > phd_rightfloat)
		{
			if (v2->x > phd_rightfloat)
				continue;

			clipper = (phd_rightfloat - v2->x) / (v1->x - v2->x);
			DoClip(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_rightfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}

		if (v2->x < phd_leftfloat)
		{
			clipper = (phd_leftfloat - v2->x) / (v1->x - v2->x);
			DoClip(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_leftfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else if (v2->x > phd_rightfloat)
		{
			clipper = (phd_rightfloat - v2->x) / (v1->x - v2->x);
			DoClip(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_rightfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else
		{
			output[nPoints].x = v2->x;
			output[nPoints].y = v2->y;
			output[nPoints].ooz = v2->ooz;
			output[nPoints].u = v2->u;
			output[nPoints].v = v2->v;
			output[nPoints].vr = v2->vr;
			output[nPoints].vg = v2->vg;
			output[nPoints].vb = v2->vb;
			nPoints++;
		}
	}

	if (nPoints < 3)
		return 0;

	n = nPoints;
	v2 = &output[n - 1];
	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		v2 = &output[i];

		if (v1->y < phd_topfloat)
		{
			if (v2->y < phd_topfloat)
				continue;

			clipper = (phd_topfloat - v2->y) / (v1->y - v2->y);
			DoClip(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_topfloat;
			nPoints++;
		}
		else if (v1->y > phd_bottomfloat)
		{
			if (v2->y > phd_bottomfloat)
				continue;

			clipper = (phd_bottomfloat - v2->y) / (v1->y - v2->y);
			DoClip(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_bottomfloat;
			nPoints++;
		}

		if (v2->y < phd_topfloat)
		{
			clipper = (phd_topfloat - v2->y) / (v1->y - v2->y);
			DoClip(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_topfloat;
			nPoints++;
		}
		else if (v2->y > phd_bottomfloat)
		{
			clipper = (phd_bottomfloat - v2->y) / (v1->y - v2->y);
			DoClip(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_bottomfloat;
			nPoints++;
		}
		else
		{
			in[nPoints].x = v2->x;
			in[nPoints].y = v2->y;
			in[nPoints].ooz = v2->ooz;
			in[nPoints].u = v2->u;
			in[nPoints].v = v2->v;
			in[nPoints].vr = v2->vr;
			in[nPoints].vg = v2->vg;
			in[nPoints].vb = v2->vb;
			nPoints++;
		}
	}

	if (nPoints < 3)
		nPoints = 0;

	return nPoints;
}

static inline void DoClipNoUV(VERTEX_INFO* vtx, VERTEX_INFO* v1, VERTEX_INFO* v2, float clipper)
{
	vtx->ooz = (v2->ooz - v1->ooz) * clipper + v1->ooz;
	vtx->vr = long((v2->vr - v1->vr) * clipper + v1->vr);
	vtx->vg = long((v2->vg - v1->vg) * clipper + v1->vg);
	vtx->vb = long((v2->vb - v1->vb) * clipper + v1->vb);
}

long XYGClipper(long n, VERTEX_INFO* in)
{
	VERTEX_INFO* v1;
	VERTEX_INFO* v2;
	static VERTEX_INFO output[8];
	float clipper;
	long nPoints;

	v2 = &in[n - 1];
	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		v2 = &in[i];

		if (v1->x < phd_leftfloat)
		{
			if (v2->x < phd_leftfloat)
				continue;

			clipper = (phd_leftfloat - v2->x) / (v1->x - v2->x);
			DoClipNoUV(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_leftfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else if (v1->x > phd_rightfloat)
		{
			if (v2->x > phd_rightfloat)
				continue;

			clipper = (phd_rightfloat - v2->x) / (v1->x - v2->x);
			DoClipNoUV(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_rightfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}

		if (v2->x < phd_leftfloat)
		{
			clipper = (phd_leftfloat - v2->x) / (v1->x - v2->x);
			DoClipNoUV(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_leftfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else if (v2->x > phd_rightfloat)
		{
			clipper = (phd_rightfloat - v2->x) / (v1->x - v2->x);
			DoClipNoUV(&output[nPoints], v2, v1, clipper);
			output[nPoints].x = phd_rightfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else
		{
			output[nPoints].x = v2->x;
			output[nPoints].y = v2->y;
			output[nPoints].ooz = v2->ooz;
			output[nPoints].vr = v2->vr;
			output[nPoints].vg = v2->vg;
			output[nPoints].vb = v2->vb;
			nPoints++;
		}
	}

	if (nPoints < 3)
		return 0;

	n = nPoints;
	v2 = &output[n - 1];
	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		v2 = &output[i];

		if (v1->y < phd_topfloat)
		{
			if (v2->y < phd_topfloat)
				continue;

			clipper = (phd_topfloat - v2->y) / (v1->y - v2->y);
			DoClipNoUV(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_topfloat;
			nPoints++;
		}
		else if (v1->y > phd_bottomfloat)
		{
			if (v2->y > phd_bottomfloat)
				continue;

			clipper = (phd_bottomfloat - v2->y) / (v1->y - v2->y);
			DoClipNoUV(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_bottomfloat;
			nPoints++;
		}

		if (v2->y < phd_topfloat)
		{
			clipper = (phd_topfloat - v2->y) / (v1->y - v2->y);
			DoClipNoUV(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_topfloat;
			nPoints++;
		}
		else if (v2->y > phd_bottomfloat)
		{
			clipper = (phd_bottomfloat - v2->y) / (v1->y - v2->y);
			DoClipNoUV(&in[nPoints], v2, v1, clipper);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_bottomfloat;
			nPoints++;
		}
		else
		{
			in[nPoints].x = v2->x;
			in[nPoints].y = v2->y;
			in[nPoints].ooz = v2->ooz;
			in[nPoints].vr = v2->vr;
			in[nPoints].vg = v2->vg;
			in[nPoints].vb = v2->vb;
			nPoints++;
		}
	}

	if (nPoints < 3)
		nPoints = 0;

	return nPoints;
}

long XYClipper(long n, VERTEX_INFO* in)
{
	VERTEX_INFO* v1;
	VERTEX_INFO* v2;
	VERTEX_INFO output[20];
	float clipper;
	long nPoints;

	v2 = &in[n - 1];
	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		v2 = &in[i];

		if (v1->x < phd_leftfloat)
		{
			if (v2->x < phd_leftfloat)
				continue;

			clipper = (phd_leftfloat - v2->x) / (v1->x - v2->x);
			output[nPoints].x = phd_leftfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else if (v1->x > phd_rightfloat)
		{
			if (v2->x > phd_rightfloat)
				continue;

			clipper = (phd_rightfloat - v2->x) / (v1->x - v2->x);
			output[nPoints].x = phd_rightfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}

		if (v2->x < phd_leftfloat)
		{
			clipper = (phd_leftfloat - v2->x) / (v1->x - v2->x);
			output[nPoints].x = phd_leftfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else if (v2->x > phd_rightfloat)
		{
			clipper = (phd_rightfloat - v2->x) / (v1->x - v2->x);
			output[nPoints].x = phd_rightfloat;
			output[nPoints].y = (v1->y - v2->y) * clipper + v2->y;
			nPoints++;
		}
		else
		{
			output[nPoints].x = v2->x;
			output[nPoints].y = v2->y;
			nPoints++;
		}
	}

	if (nPoints < 3)
		return 0;

	n = nPoints;
	v2 = &output[n - 1];
	nPoints = 0;

	for (int i = 0; i < n; i++)
	{
		v1 = v2;
		v2 = &output[i];

		if (v1->y < phd_topfloat)
		{
			if (v2->y < phd_topfloat)
				continue;

			clipper = (phd_topfloat - v2->y) / (v1->y - v2->y);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_topfloat;
			nPoints++;
		}
		else if (v1->y > phd_bottomfloat)
		{
			if (v2->y > phd_bottomfloat)
				continue;

			clipper = (phd_bottomfloat - v2->y) / (v1->y - v2->y);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_bottomfloat;
			nPoints++;
		}

		if (v2->y < phd_topfloat)
		{
			clipper = (phd_topfloat - v2->y) / (v1->y - v2->y);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_topfloat;
			nPoints++;
		}
		else if (v2->y > phd_bottomfloat)
		{
			clipper = (phd_bottomfloat - v2->y) / (v1->y - v2->y);
			in[nPoints].x = (v1->x - v2->x) * clipper + v2->x;
			in[nPoints].y = phd_bottomfloat;
			nPoints++;
		}
		else
		{
			in[nPoints].x = v2->x;
			in[nPoints].y = v2->y;
			nPoints++;
		}
	}

	if (nPoints < 3)
		nPoints = 0;

	return nPoints;
}
