#include "../tomb3/pch.h"
#include "3d_gen.h"
#include "../specific/transform.h"
#include "../specific/litesrc.h"
#include "phd_math.h"
#include "scalespr.h"
#include "../game/gameflow.h"
#include "../specific/output.h"
#include "hwinsert.h"
#include "../specific/game.h"
#include "../specific/smain.h"
#include "../specific/init.h"
#include "../game/control.h"
#include "../game/draw.h"
#include "../specific/hwrender.h"
#include "../tomb3/tomb3.h"

void (*InsertLine)(long x1, long y1, long x2, long y2, long z, long c0, long c1);
short* (*InsertObjectGT4)(short* pFaceInfo, long nFaces, sort_type nSortType);
short* (*InsertObjectGT3)(short* pFaceInfo, long nFaces, sort_type nSortType);
short* (*InsertObjectG4)(short* pFaceInfo, long nFaces, sort_type nSortType);
short* (*InsertObjectG3)(short* pFaceInfo, long nFaces, sort_type nSortType);
void (*InsertFlatRect)(long x1, long y1, long x2, long y2, long zdepth, long col);
void (*InsertTrans8)(PHD_VBUF* buf, short shade);
void (*InsertSprite)(long zdepth, long x1, long y1, long x2, long y2, long nSprite, ulong shade, ulong shade1, long nDrawType, long offset);
void (*InsertTransQuad)(long x, long y, long w, long h, long z);
void (*InsertGourQuad)(long x0, long y0, long x1, long y1, long z, ulong c0, ulong c1, ulong c2, ulong c3, bool add);
bool (*IsVisible)(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2);
bool (*IsInvisible)(PHD_VBUF* v0, PHD_VBUF* v1, PHD_VBUF* v2);

float outsideBackgroundTop;
long BlackGouraudIndex = 0;
bool bFixSkyColour = 0;

long sort3d_bufferbf[MAX_SORTLISTS];
short info3d_bufferbf[MAX_SORTLISTS];
long* sort3dptrbf;
short* info3dptrbf;
long surfacenumbf;

long sort3d_bufferfb[MAX_SORTLISTS];
short info3d_bufferfb[MAX_SORTLISTS];
long* sort3dptrfb;
short* info3dptrfb;
long surfacenumfb;

long* phd_mxptr;
long w2v_matrix[indices_count];
long matrix_stack[40 * indices_count];

float one = 33554432.0F;
static float LfAspectCorrection = 0;

PHD_VECTOR CamPos;
PHD_VECTOR CamRot;

float f_centerx;
float f_centery;
float f_znear;
float f_zfar;
float f_persp;
float f_oneopersp;
float f_perspoznear;
float f_oneoznear;
float f_a;
float f_b;
float f_boo;

long phd_centerx;
long phd_centery;
long phd_znear;
long phd_zfar;
long phd_persp;

RECT phd_WindowRect;
float phd_leftfloat;
float phd_topfloat;
float phd_rightfloat;
float phd_bottomfloat;
long phd_winwidth;
long phd_winheight;
long phd_right;
long phd_left;
long phd_bottom;
long phd_top;
long phd_scrwidth;
long phd_scrheight;
long phd_viewdist;
short phd_winxmin;
short phd_winxmax;
short phd_winymin;
short phd_winymax;

void phd_PutPolygons(short* objptr, long clip)
{
	short* newPtr;

	phd_leftfloat = (float)phd_winxmin;
	phd_topfloat = (float)phd_winymin;
	phd_rightfloat = float(phd_winxmax + phd_winxmin + 1);
	phd_bottomfloat = float(phd_winymax + phd_winymin + 1);
	f_centerx = float(phd_winxmin + phd_centerx);
	f_centery = float(phd_winymin + phd_centery);
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

void phd_RotX(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}
}

void phd_RotY(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}
}

void phd_RotZ(short angle)
{
	long sin, cos, mx1, mx2;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M01] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M11] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M21] = mx2 >> W2V_SHIFT;
	}
}

void phd_RotYXZ(short y, short x, short z)
{
	long sin, cos, mx1, mx2;

	if (y)
	{
		sin = phd_sin(y);
		cos = phd_cos(y);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	if (x)
	{
		sin = phd_sin(x);
		cos = phd_cos(x);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	if (z)
	{
		sin = phd_sin(z);
		cos = phd_cos(z);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M01] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M11] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M21] = mx2 >> W2V_SHIFT;
	}
}

void phd_RotYXZpack(long angles)
{
	long sin, cos, mx1, mx2;
	short angle;

	angle = (angles >> 10) & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] - sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] + sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] - sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] + sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] - sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] + sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	angle = (angles >> 20) & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M01] + sin * phd_mxptr[M02];
		mx2 = cos * phd_mxptr[M02] - sin * phd_mxptr[M01];
		phd_mxptr[M01] = mx1 >> W2V_SHIFT;
		phd_mxptr[M02] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M11] + sin * phd_mxptr[M12];
		mx2 = cos * phd_mxptr[M12] - sin * phd_mxptr[M11];
		phd_mxptr[M11] = mx1 >> W2V_SHIFT;
		phd_mxptr[M12] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M21] + sin * phd_mxptr[M22];
		mx2 = cos * phd_mxptr[M22] - sin * phd_mxptr[M21];
		phd_mxptr[M21] = mx1 >> W2V_SHIFT;
		phd_mxptr[M22] = mx2 >> W2V_SHIFT;
	}

	angle = angles & 0x3FF;
	angle <<= 6;

	if (angle)
	{
		sin = phd_sin(angle);
		cos = phd_cos(angle);

		mx1 = cos * phd_mxptr[M00] + sin * phd_mxptr[M01];
		mx2 = cos * phd_mxptr[M01] - sin * phd_mxptr[M00];
		phd_mxptr[M00] = mx1 >> W2V_SHIFT;
		phd_mxptr[M01] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M10] + sin * phd_mxptr[M11];
		mx2 = cos * phd_mxptr[M11] - sin * phd_mxptr[M10];
		phd_mxptr[M10] = mx1 >> W2V_SHIFT;
		phd_mxptr[M11] = mx2 >> W2V_SHIFT;

		mx1 = cos * phd_mxptr[M20] + sin * phd_mxptr[M21];
		mx2 = cos * phd_mxptr[M21] - sin * phd_mxptr[M20];
		phd_mxptr[M20] = mx1 >> W2V_SHIFT;
		phd_mxptr[M21] = mx2 >> W2V_SHIFT;
	}
}

long phd_TranslateRel(long x, long y, long z)
{
	phd_mxptr[M03] += x * phd_mxptr[M00] + y * phd_mxptr[M01] + z * phd_mxptr[M02];
	phd_mxptr[M13] += x * phd_mxptr[M10] + y * phd_mxptr[M11] + z * phd_mxptr[M12];
	phd_mxptr[M23] += x * phd_mxptr[M20] + y * phd_mxptr[M21] + z * phd_mxptr[M22];

	if (abs(phd_mxptr[M03]) > phd_zfar || abs(phd_mxptr[M13]) > phd_zfar || abs(phd_mxptr[M23]) > phd_zfar)
		return 0;

	return 1;
}

void phd_TranslateAbs(long x, long y, long z)
{
	long fx, fy, fz;

	fx = x - w2v_matrix[M03];
	fy = y - w2v_matrix[M13];
	fz = z - w2v_matrix[M23];
	phd_mxptr[M03] = fx * phd_mxptr[M00] + fy * phd_mxptr[M01] + fz * phd_mxptr[M02];
	phd_mxptr[M13] = fx * phd_mxptr[M10] + fy * phd_mxptr[M11] + fz * phd_mxptr[M12];
	phd_mxptr[M23] = fx * phd_mxptr[M20] + fy * phd_mxptr[M21] + fz * phd_mxptr[M22];
}

void AlterFOV(short fov)
{
	long fov_width;

	fov /= 2;

	if (tomb3.psx_fov)
		fov_width = phd_winheight * 320 / 200;
	else
		fov_width = phd_winheight * 320 / 240;

	LfAspectCorrection = 1.0F;
	phd_persp = (fov_width / 2) * phd_cos(fov) / phd_sin(fov);

	f_persp = (float)phd_persp;
	f_oneopersp = one / f_persp;
	f_perspoznear = f_persp / f_znear;
	InitZTable();
}

void phd_PushMatrix()
{
	memcpy(phd_mxptr + indices_count, phd_mxptr, indices_count * sizeof(long));
	phd_mxptr += indices_count;
}

void phd_PushUnitMatrix()
{
	phd_mxptr += indices_count;
	phd_mxptr[M00] = 1 << W2V_SHIFT;
	phd_mxptr[M01] = 0;
	phd_mxptr[M02] = 0;
	phd_mxptr[M10] = 0;
	phd_mxptr[M11] = 1 << W2V_SHIFT;
	phd_mxptr[M12] = 0;
	phd_mxptr[M20] = 0;
	phd_mxptr[M21] = 0;
	phd_mxptr[M22] = 1 << W2V_SHIFT;
}

void phd_SetTrans(long x, long y, long z)
{
	phd_mxptr[M03] = x << W2V_SHIFT;
	phd_mxptr[M13] = y << W2V_SHIFT;
	phd_mxptr[M23] = z << W2V_SHIFT;
}

void phd_PopMatrix()
{
	phd_mxptr -= indices_count;
}

void SetZNear(long znear)
{
	float z;

	phd_znear = znear;
	f_znear = (float)znear;
	f_oneoznear = one / f_znear;
	f_perspoznear = f_persp / f_znear;
	z = f_zfar * f_znear * 0.990F / (f_znear - f_zfar);
	f_b = -z;
	f_boo = -z / one;
	f_a = 0.005F - z / f_znear;
}

void SetZFar(long zfar)
{
	float z;

	phd_zfar = zfar;
	f_zfar = (float)zfar;
	z = f_zfar * f_znear * 0.990F / (f_znear - f_zfar);
	f_b = -z;
	f_boo = -z / one;
	f_a = 0.005F - z / f_znear;
}

void S_InsertRoom(short* objptr, long out)
{
	phd_leftfloat = (float)(phd_winxmin + phd_left);
	phd_topfloat = (float)(phd_winymin + phd_top);
	phd_rightfloat = float(phd_right + phd_winxmin + 1);
	phd_bottomfloat = float(phd_bottom + phd_winymin + 1);
	f_centerx = float(phd_winxmin + phd_centerx);
	f_centery = float(phd_winymin + phd_centery);
	objptr = calc_roomvert(objptr, out ? 0 : 16);
	objptr = InsertObjectGT4(objptr + 1, objptr[0], FAR_SORT);
	objptr = InsertObjectGT3(objptr + 1, objptr[0], FAR_SORT);
	ins_room_sprite(objptr + 1, objptr[0]);
}

short* calc_back_light(short* objptr)
{
	long nNormals;

	nNormals = *objptr;
	objptr++;

	if (nNormals > 0)
		objptr += 3 * nNormals;
	else if (nNormals < 0)
	{
		nNormals = -nNormals;
		objptr += nNormals;
	}

	for (int i = 0; i < nNormals; i++)
		vbuf[i].color = 0x808080;

	return objptr;
}

void S_InsertBackground(short* objptr)
{
	short* triPtr;
	long nTris;

	nPolyType = 1;
	phd_leftfloat = float(phd_winxmin + phd_left);
	phd_topfloat = float(phd_winymin + phd_top);
	phd_rightfloat = float(phd_right + phd_winxmin + 1);
	phd_bottomfloat = float(phd_bottom + phd_winymin + 1);
	f_centerx = float(phd_winxmin + phd_centerx);
	f_centery = float(phd_winymin + phd_centery);
	objptr += 4;
	objptr = calc_object_verticesback(objptr);
	outsideBackgroundTop = 0;

	if (objptr)
	{
		HWR_InitGamma(10.0F);
		objptr = calc_back_light(objptr);
		objptr = InsertObjectGT4(objptr + 1, objptr[0], BACK_SORT);
		objptr = InsertObjectGT3(objptr + 1, objptr[0], BACK_SORT);
		objptr = InsertObjectG4(objptr + 1, objptr[0], BACK_SORT);

		if (bFixSkyColour)
		{
			if (CurrentLevel == LV_CHAMBER)
				triPtr = objptr + 4;
			else
				triPtr = objptr + 68;

			nTris = *objptr - 16;

			for (; nTris > 0; nTris--)
			{
				triPtr[0] = short(BlackGouraudIndex << 8);
				triPtr += 4;
			}

			bFixSkyColour = 0;
		}

		InsertObjectG3(objptr + 1, objptr[0], BACK_SORT);
		HWR_InitGamma(GammaOption);
	}

	if (CurrentLevel == LV_GYM)
		S_InsertBackPolygon(0, (long)outsideBackgroundTop, phd_right + phd_winxmin, phd_winymax, 0);
}

void phd_InitWindow(long x, long y, long w, long h, long znear, long zfar, long fov, long sw, long sh)
{
	phd_winxmin = (short)x;
	phd_winxmax = short(w - 1);
	phd_winymin = (short)y;
	phd_winymax = short(h - 1);
	phd_winwidth = w;
	phd_winheight = h;
	phd_scrwidth = sw;
	phd_scrheight = sh;
	phd_centerx = w / 2;
	phd_centery = h / 2;
	phd_viewdist = zfar;
	phd_znear = znear << W2V_SHIFT;
	phd_zfar = zfar << W2V_SHIFT;
	f_centerx = float(w / 2);
	f_centery = float(h / 2);
	AlterFOV(short(182 * fov));
	SetZNear(phd_znear);
	SetZFar(phd_zfar);
	InitZTable();
	phd_right = phd_winxmax;
	phd_bottom = phd_winymax;
	phd_left = 0;
	phd_top = 0;
	phd_mxptr = matrix_stack;
	phd_WindowRect.left = phd_winxmin;
	phd_WindowRect.bottom = phd_winymin + phd_winheight;
	phd_WindowRect.top = phd_winymin;
	phd_WindowRect.right = phd_winxmin + phd_winwidth;

	SetFunctionPointers();
}

void phd_InitPolyList()
{
	surfacenumbf = 0;
	surfacenumfb = 0;
	info3dptrbf = info3d_bufferbf;
	sort3dptrbf = sort3d_bufferbf;
	info3dptrfb = info3d_bufferfb;
	sort3dptrfb = sort3d_bufferfb;
	CurrentTLVertex = VertexBuffer;
}

void do_quickysorty(long left, long right, long* list)
{
	long* lList;
	long* rList;
	long swp[3];
	long l, r, z;

	l = left;
	r = right;
	z = list[3 * ((right + l) / 2) + 1];

	do
	{
		while (l < right && list[3 * l + 1] > z)
			l++;

		while (r > left && list[3 * r + 1] < z)
			r--;

		if (l <= r)
		{
			lList = &list[3 * l];
			rList = &list[3 * r];

			swp[0] = lList[0];
			swp[1] = lList[1];
			swp[2] = lList[2];

			lList[0] = rList[0];
			lList[1] = rList[1];
			lList[2] = rList[2];

			rList[0] = swp[0];
			rList[1] = swp[1];
			rList[2] = swp[2];

			l++;
			r--;
		}

	} while (l <= r);

	if (r > left)
		do_quickysorty(left, r, list);

	if (l < right)
		do_quickysorty(l, right, list);
}

void phd_SortPolyList(long count, long* list)
{
	long* l;

	if (!count)
		return;

	l = list;

	for (int i = 0; i < count; i++)
	{
		l[1] += i;
		l += 3;
	}

	do_quickysorty(0, count - 1, list);
}

void phd_NormaliseVector(long x, long y, long z, long* dest)
{
	long s;

	if (x || y || z)
	{
		while ((short)x != x || (short)y != y || (short)z != z)
		{
			x >>= 2;
			y >>= 2;
			z >>= 2;
		}

		s = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
		dest[0] = (x << W2V_SHIFT) / s;
		dest[1] = (y << W2V_SHIFT) / s;
		dest[2] = (z << W2V_SHIFT) / s;
	}
	else
	{
		dest[0] = 0;
		dest[1] = 0;
		dest[2] = 0;
	}
}

void phd_GetVectorAngles(long x, long y, long z, short* angles)
{
	short atan;

	angles[0] = (short)phd_atan(z, x);

	while ((short)x != x || (short)y != y || (short)z != z)
	{
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}

	atan = (short)phd_atan(phd_sqrt(SQUARE(x) + SQUARE(z)), y);

	if ((y > 0 && atan > 0) || (y < 0 && atan < 0))
		atan = -atan;

	angles[1] = atan;
}

void phd_GenerateW2V(PHD_3DPOS* viewPos)
{
	long sx, cx, sy, cy, sz, cz;

	phd_mxptr = matrix_stack;
	sx = phd_sin(viewPos->x_rot);
	cx = phd_cos(viewPos->x_rot);
	sy = phd_sin(viewPos->y_rot);
	cy = phd_cos(viewPos->y_rot);
	sz = phd_sin(viewPos->z_rot);
	cz = phd_cos(viewPos->z_rot);

	w2v_matrix[M00] = TRIGMULT3(sx, sy, sz) + TRIGMULT2(cy, cz);
	w2v_matrix[M01] = TRIGMULT2(cx, sz);
	w2v_matrix[M02] = TRIGMULT3(sx, cy, sz) - TRIGMULT2(sy, cz);
	phd_mxptr[M00] = w2v_matrix[M00];
	phd_mxptr[M01] = w2v_matrix[M01];
	phd_mxptr[M02] = w2v_matrix[M02];

	w2v_matrix[M10] = TRIGMULT3(sx, sy, cz) - TRIGMULT2(cy, sz);
	w2v_matrix[M11] = TRIGMULT2(cx, cz);
	w2v_matrix[M12] = TRIGMULT3(sx, cy, cz) + TRIGMULT2(sy, sz);
	phd_mxptr[M10] = w2v_matrix[M10];
	phd_mxptr[M11] = w2v_matrix[M11];
	phd_mxptr[M12] = w2v_matrix[M12];

	w2v_matrix[M20] = TRIGMULT2(cx, sy);
	w2v_matrix[M21] = -sx;
	w2v_matrix[M22] = TRIGMULT2(cx, cy);
	phd_mxptr[M20] = w2v_matrix[M20];
	phd_mxptr[M21] = w2v_matrix[M21];
	phd_mxptr[M22] = w2v_matrix[M22];

	w2v_matrix[M03] = viewPos->x_pos;;
	w2v_matrix[M13] = viewPos->y_pos;
	w2v_matrix[M23] = viewPos->z_pos;
	phd_mxptr[M03] = w2v_matrix[M03];
	phd_mxptr[M13] = w2v_matrix[M13];
	phd_mxptr[M23] = w2v_matrix[M23];

	w2v_matrix[M10] = long(LfAspectCorrection * float(phd_mxptr[M10]));
	w2v_matrix[M11] = long(LfAspectCorrection * float(phd_mxptr[M11]));
	w2v_matrix[M12] = long(LfAspectCorrection * float(phd_mxptr[M12]));
	phd_mxptr[M10] = w2v_matrix[M10];
	phd_mxptr[M11] = w2v_matrix[M11];
	phd_mxptr[M12] = w2v_matrix[M12];
}

void phd_LookAt(long sx, long sy, long sz, long tx, long ty, long tz, short roll)
{
	PHD_3DPOS viewPos;
	long dx, dy, dz, val;
	short angles[2];

	phd_GetVectorAngles(tx - sx, ty - sy, tz - sz, angles);
	viewPos.x_pos = sx;
	viewPos.y_pos = sy;
	viewPos.z_pos = sz;
	viewPos.x_rot = angles[1];
	viewPos.y_rot = angles[0];
	viewPos.z_rot = roll;
	dx = sx - tx;
	dy = sy - ty;
	dz = sz - tz;
	val = (long)sqrtl(SQUARE(dx) + SQUARE(dz));
	CamRot.x = (mGetAngle(0, 0, val, dy) >> 4) & 0xFFF;
	CamRot.y = (mGetAngle(sz, sx, tz, tx) >> 4) & 0xFFF;
	CamRot.z = 0;
	CamPos.x = sx;
	CamPos.y = sy;
	CamPos.z = sz;
	phd_GenerateW2V(&viewPos);
}
