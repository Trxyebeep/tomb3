#include "../tomb3/pch.h"
#include "transform.h"

void InitZTable()
{
	float zv;
	long z;

	z = 0;

	for (int i = 0; i < 20480; i++)
	{
		zv = float(z << W2V_SHIFT);
		z++;
		ZTable[i * 2] = f_persp / zv;
		ZTable[(i * 2) + 1] = f_oneopersp * f_persp / zv;
	}
}

short* calc_object_vertices(short* objptr)
{
	PHD_VBUF* buf;
	float fMidSort, zv, zT;
	long nVtx;
	char totalClip, clipFlag;

	if (App.lpZBuffer)
		fMidSort = 0;
	else
		fMidSort = float(mid_sort << (W2V_SHIFT + 8));

	totalClip = -1;
	buf = vbuf;
	objptr++;
	nVtx = *objptr++;
	m00 = float(phd_mxptr[M00]);
	m01 = float(phd_mxptr[M01]);
	m02 = float(phd_mxptr[M02]);
	m03 = float(phd_mxptr[M03]);
	m10 = float(phd_mxptr[M10]);
	m11 = float(phd_mxptr[M11]);
	m12 = float(phd_mxptr[M12]);
	m13 = float(phd_mxptr[M13]);
	m20 = float(phd_mxptr[M20]);
	m21 = float(phd_mxptr[M21]);
	m22 = float(phd_mxptr[M22]);
	m23 = float(phd_mxptr[M23]);

	for (; nVtx > 0; nVtx--)
	{
		buf->xv = m00 * float(objptr[0]) + m01 * float(objptr[1]) + m02 * float(objptr[2]) + m03;
		buf->yv = m10 * float(objptr[0]) + m11 * float(objptr[1]) + m12 * float(objptr[2]) + m13;
		zv = m20 * float(objptr[0]) + m21 * float(objptr[1]) + m22 * float(objptr[2]) + m23;
		buf->z = (long)zv;

		if (buf->z < phd_znear)
		{
			buf->zv = zv;
			clipFlag = -128;
		}
		else
		{
			if (buf->z < phd_zfar)
			{
				buf->zv = zv + fMidSort;
				zT = ZTable[((buf->z >> 14) << 1)];
				buf->xs = buf->xv * zT + f_centerx;
				buf->ys = buf->yv * zT + f_centery;
				buf->ooz = ZTable[((buf->z >> 14) << 1) + 1];
			}
			else
			{
				buf->zv = f_zfar;
				zv = f_persp / zv;
				buf->xs = buf->xv * zv + f_centerx;
				buf->ys = buf->yv * zv + f_centery;
				buf->ooz = zv * f_oneopersp;
			}

			clipFlag = 0;

			if (buf->xs < phd_leftfloat)
				clipFlag++;
			else if (buf->xs > phd_rightfloat)
				clipFlag += 2;

			if (buf->ys < phd_topfloat)
				clipFlag += 4;
			else if (buf->ys > phd_bottomfloat)
				clipFlag += 8;
		}

		buf->clip = clipFlag;
		totalClip &= clipFlag;
		buf++;
		objptr += 3;
	}

	if (totalClip)
		return 0;

	return objptr;
}

short* calc_object_verticesback(short* objptr)
{
	PHD_VBUF* buf;
	float fMidSort, x, y, z, zv, zT;
	long nVtx;
	char totalClip, clipFlag;

	if (App.lpZBuffer)
		fMidSort = 0;
	else
		fMidSort = float(mid_sort << (W2V_SHIFT + 8));

	outsideBackgroundTop = 0;
	totalClip = -1;
	buf = vbuf;
	objptr++;
	nVtx = *objptr++;
	m00 = float(phd_mxptr[M00]);
	m01 = float(phd_mxptr[M01]);
	m02 = float(phd_mxptr[M02]);
	m03 = float(phd_mxptr[M03]);
	m10 = float(phd_mxptr[M10]);
	m11 = float(phd_mxptr[M11]);
	m12 = float(phd_mxptr[M12]);
	m13 = float(phd_mxptr[M13]);
	m20 = float(phd_mxptr[M20]);
	m21 = float(phd_mxptr[M21]);
	m22 = float(phd_mxptr[M22]);
	m23 = float(phd_mxptr[M23]);

	for (; nVtx > 0; nVtx--)
	{
		x = float(objptr[0] << 5);
		y = float(objptr[1] << 5);
		z = float(objptr[2] << 5);
		buf->xv = m00 * x + m01 * y + m02 * z + m03;
		buf->yv = m10 * x + m11 * y + m12 * z + m13;
		zv = m20 * x + m21 * y + m22 * z + m23;
		buf->z = (long)zv;

		if (buf->z < phd_znear)
		{
			buf->zv = zv;
			clipFlag = -128;
		}
		else
		{
			if (buf->z < phd_zfar)
			{
				buf->zv = zv + fMidSort;
				zT = ZTable[((buf->z >> 14) << 1)];
				buf->xs = buf->xv * zT + f_centerx;
				buf->ys = buf->yv * zT + f_centery;
				buf->ooz = ZTable[((buf->z >> 14) << 1) + 1];
			}
			else
			{
				buf->zv = f_zfar;
				zv = f_persp / zv;
				buf->xs = buf->xv * zv + f_centerx;
				buf->ys = buf->yv * zv + f_centery;
				buf->ooz = zv * f_oneopersp;
			}

			if (buf->ys > outsideBackgroundTop && buf->ys > 0 && buf->ys < phd_winymin + phd_winymax &&
				buf->xs > 0 && buf->xs < phd_winxmin + phd_winxmax)
			{
				outsideBackgroundTop = buf->ys;

				if (outsideBackgroundTop > phd_winymin + phd_winymax)
					outsideBackgroundTop = float(phd_winymin + phd_winymax - 1);

				if (outsideBackgroundTop < 0)
					outsideBackgroundTop = 0;
			}

			clipFlag = 0;

			if (buf->xs < phd_leftfloat)
				clipFlag++;
			else if (buf->xs > phd_rightfloat)
				clipFlag += 2;

			if (buf->ys < phd_topfloat)
				clipFlag += 4;
			else if (buf->ys > phd_bottomfloat)
				clipFlag += 8;
		}

		buf->clip = clipFlag;
		totalClip &= clipFlag;
		buf++;
		objptr += 3;
	}

	if (totalClip)
		return 0;

	return objptr;
}

void inject_transform(bool replace)
{
	INJECT(0x0048D6A0, InitZTable, replace);
	INJECT(0x0048D6E0, calc_object_vertices, replace);
	INJECT(0x0048E050, calc_object_verticesback, replace);
}
