#include "../tomb3/pch.h"
#include "3d_gen.h"
#include "../specific/transform.h"
#include "../specific/litesrc.h"
#include "phd_math.h"

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
}
