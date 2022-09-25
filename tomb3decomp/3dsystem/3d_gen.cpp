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

	if (ABS(phd_mxptr[M03]) > phd_zfar || ABS(phd_mxptr[M13]) > phd_zfar || ABS(phd_mxptr[M23]) > phd_zfar)
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
	fov /= 2;
	phd_persp = ((phd_winwidth / 2) * phd_cos(fov)) / phd_sin(fov);
	f_persp = (float)phd_persp;
	f_oneopersp = one / f_persp;
	f_perspoznear = f_persp / f_znear;
	LfAspectCorrection = (4.0F / 3.0F) / (float(phd_winwidth) / float(phd_winheight));
	InitZTable();
}

void phd_PushMatrix()
{
	memcpy(phd_mxptr + 12, phd_mxptr, 48);
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
		vbuf[i].g = 0x4210;

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
		objptr = calc_back_light(objptr);
		objptr = InsertObjectGT4(objptr + 1, objptr[0], BACK_SORT);
		objptr = InsertObjectGT3(objptr + 1, objptr[0], BACK_SORT);
		objptr = InsertObjectG4(objptr + 1, objptr[0], BACK_SORT);

		if (bFixSkyColour)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_CHAMBER)
#else
			if (CurrentLevel == LV_CHAMBER)
#endif
				triPtr = objptr + 4;
			else
				triPtr = objptr + 68;

			nTris = *objptr - 16;

			for (; nTris; nTris--)
			{
				triPtr[0] = short(BlackGouraudIndex << 8);
				triPtr += 4;
			}

			bFixSkyColour = 0;
		}

		objptr = InsertObjectG3(objptr + 1, objptr[0], BACK_SORT);
	}

	if (CurrentLevel == LV_GYM)
		S_InsertBackPolygon(0, (long)outsideBackgroundTop, phd_right + phd_winxmin, phd_winymax, 0x800000);
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
	phd_znear = znear << 14;
	phd_zfar = zfar << 14;
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

	if (!App.nRenderMode || App.nRenderMode == 1)
	{
		InsertObjectGT3 = HWI_InsertObjectGT3_Sorted;
		InsertObjectGT4 = HWI_InsertObjectGT4_Sorted;
		InsertObjectG3 = HWI_InsertObjectG3_Sorted;
		InsertObjectG4 = HWI_InsertObjectG4_Sorted;
		InsertFlatRect = HWI_InsertFlatRect_Sorted;
		InsertLine = HWI_InsertLine_Sorted;
		RoomInsertObjectGT3 = HWI_InsertObjectGT3_Sorted;
		RoomInsertObjectGT4 = HWI_InsertObjectGT4_Sorted;
		InsertSprite = HWI_InsertSprite_Sorted;
		InsertTrans8 = HWI_InsertTrans8_Sorted;
		InsertTransQuad = HWI_InsertTransQuad_Sorted;
	}
}

void phd_InitPolyList()
{
	/*
	dword_5D7AE8 = (int)&unk_4E34B0;	//seemingly unused, no problems so far
	dword_5D7AE4 = 0;
	dword_5A6AF0 = (int)&unk_4F6D60;
	*/
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

void inject_3dgen(bool replace)
{
	INJECT(0x00401AF0, phd_PutPolygons, replace);
	INJECT(0x004013E0, phd_RotX, replace);
	INJECT(0x00401490, phd_RotY, replace);
	INJECT(0x00401540, phd_RotZ, replace);
	INJECT(0x004015F0, phd_RotYXZ, replace);
	INJECT(0x004017D0, phd_RotYXZpack, replace);
	INJECT(0x004019C0, phd_TranslateRel, replace);
	INJECT(0x00401A70, phd_TranslateAbs, replace);
	INJECT(0x00402030, AlterFOV, replace);
	INJECT(0x004B4280, phd_PushMatrix, replace);
	INJECT(0x004B429E, phd_PushUnitMatrix, replace);
//	INJECT(----------, phd_PopMatrix, replace);
	INJECT(0x00402130, SetZNear, replace);
	INJECT(0x00402180, SetZFar, replace);
	INJECT(0x00401BF0, S_InsertRoom, replace);
	INJECT(0x00401CE0, calc_back_light, replace);
	INJECT(0x00401D20, S_InsertBackground, inject_rando ? 1 : replace);
	INJECT(0x004021A0, phd_InitWindow, replace);
	INJECT(0x00401EC0, phd_InitPolyList, replace);
	INJECT(0x00401F60, do_quickysorty, replace);
	INJECT(0x00401F20, phd_SortPolyList, replace);
	INJECT(0x00401350, phd_NormaliseVector, replace);
	INJECT(0x004012D0, phd_GetVectorAngles, replace);
	INJECT(0x00401000, phd_GenerateW2V, replace);
	INJECT(0x004011D0, phd_LookAt, replace);
}
