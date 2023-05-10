#include "../tomb3/pch.h"
#include "transform.h"
#include "../3dsystem/phd_math.h"
#include "init.h"
#include "../3dsystem/hwinsert.h"
#include "../game/effect2.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"
#include "output.h"
#include "../game/control.h"
#include "../game/draw.h"

float ZTable[40960];
static float m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23;

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

	if (App.lpDXConfig->bZBuffer)
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
				zT = ZTable[((buf->z >> W2V_SHIFT) << 1)];
				buf->xs = buf->xv * zT + f_centerx;
				buf->ys = buf->yv * zT + f_centery;
				buf->ooz = ZTable[((buf->z >> W2V_SHIFT) << 1) + 1];
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

	if (App.lpDXConfig->bZBuffer)
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
				zT = ZTable[((buf->z >> W2V_SHIFT) << 1)];
				buf->xs = buf->xv * zT + f_centerx;
				buf->ys = buf->yv * zT + f_centery;
				buf->ooz = ZTable[((buf->z >> W2V_SHIFT) << 1) + 1];
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

short* calc_roomvert(short* objptr, long far_clip)
{
	ROOM_INFO* r;
	PHD_VBUF* buf;
	DYNAMIC* dl;
	float fMidSort, fx, fy, fz, zv, zv2;
	long nVtx, x, y, z, cR, cG, cB, dx, dy, dz, dist, falloff, rnd, shade, shimmer, abs;
	uchar random;
	char clipFlag;

	r = &room[CurrentRoom];

	if (App.lpDXConfig->bZBuffer)
		fMidSort = 0;
	else
		fMidSort = float(mid_sort << (W2V_SHIFT + 8));

	nVtx = *objptr++;
	buf = vbuf;
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
		cR = ((objptr[5] & 0x7C00) >> 10) << 3;
		cG = ((objptr[5] & 0x3E0) >> 5) << 3;
		cB = (objptr[5] & 0x1F) << 3;
		buf->color = RGB_MAKE(cR, cG, cB);

		x = objptr[0];
		y = objptr[1];
		z = objptr[2];
		fx = (float)objptr[0];
		fy = (float)objptr[1];
		fz = (float)objptr[2];
		buf->xv = m00 * fx + m01 * fy + m02 * fz + m03;
		buf->yv = m10 * fx + m11 * fy + m12 * fz + m13;
		zv = m20 * fx + m21 * fy + m22 * fz + m23;
		buf->zv = zv;

		if (zv < f_znear)
			buf->clip = -128;
		else
		{
			clipFlag = 0;
			buf->zv += fMidSort;
			zv2 = f_persp / zv;

			if (zv > farz << W2V_SHIFT)
				buf->zv = f_zfar;

			buf->ooz = zv2 * f_oneopersp;
			cR = RGB_GETRED(buf->color);
			cG = RGB_GETGREEN(buf->color);
			cB = RGB_GETBLUE(buf->color);

			for (int j = 0; j < number_dynamics; j++)
			{
				dl = &dynamics[j];

				if (dl->on)
				{
					dx = (dl->x - r->x - x) >> 4;
					dy = (dl->y - r->y - y) >> 4;
					dz = (dl->z - r->z - z) >> 4;
					dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

					if (dist < 0x10000)
					{
						dist = phd_sqrt(dist);
						falloff = dl->falloff >> 5;

						if (dist < falloff)
						{
							if (!dist)
								dist++;

							dist = falloff - dist;
							cR += (dist * dl->r) / falloff;
							cG += (dist * dl->g) / falloff;
							cB += (dist * dl->b) / falloff;
						}
					}
				}
			}

			rnd = (((x + r->x) >> 6) + ((y + r->y) >> 6) + ((z + r->z) >> 7)) & 0xFC;

			if (objptr[4] & 0x2000)
			{
				random = WaterTable[r->MeshEffect][rnd & 0x3F].random;
				buf->yv += float(WaterTable[r->MeshEffect][(random + (wibble >> 2)) & 0x3F].choppy << W2V_SHIFT);
				shade = -WaterTable[r->MeshEffect][(random + (wibble >> 2)) & 0x3F].choppy >> 1;

				cR += shade;
				cG += shade;
				cB += shade;
			}

			if (objptr[4] & 0x4000)
			{
				random = WaterTable[r->MeshEffect][rnd & 0x3F].random;
				shimmer = WaterTable[r->MeshEffect][(random + (wibble >> 2)) & 0x3F].shimmer;
				abs = WaterTable[r->MeshEffect][(random + (wibble >> 2)) & 0x3F].abs;
				shade = (shimmer + abs) << 3;

				cR += shade;
				cG += shade;
				cB += shade;
			}

			if (cR > 255)
				cR = 255;

			if (cG > 255)
				cG = 255;

			if (cB > 255)
				cB = 255;

			if (cR < 0)
				cR = 0;

			if (cG < 0)
				cG = 0;

			if (cB < 0)
				cB = 0;

			buf->color = RGB_MAKE(cR, cG, cB);

			buf->xs = zv2 * buf->xv + f_centerx;
			buf->ys = zv2 * buf->yv + f_centery;

			if (water_effect && objptr[4] >= 0)
				buf->ys += wibble_table[(((long)buf->xs + wibble) >> 3) & 0x1F];

			if (buf->xs < phd_leftfloat)
				clipFlag++;
			else if (buf->xs > phd_rightfloat)
				clipFlag += 2;

			if (buf->ys < phd_topfloat)
				clipFlag += 4;
			else if (buf->ys > phd_bottomfloat)
				clipFlag += 8;

			buf->clip = clipFlag;
		}

		if (zv > distanceFogValue << W2V_SHIFT)
		{
			z = (long)zv;

			if (z > farz << W2V_SHIFT)
				z = farz << W2V_SHIFT;

			cR = RGB_GETRED(buf->color);
			cG = RGB_GETGREEN(buf->color);
			cB = RGB_GETBLUE(buf->color);
			shade = 2048 - ((z - (distanceFogValue << W2V_SHIFT)) >> 16);
			cR = (cR * shade) / 2048;
			cG = (cG * shade) / 2048;
			cB = (cB * shade) / 2048;
			buf->color = RGB_MAKE(cR, cG, cB);
		}

		objptr += 6;
		buf++;
	}

	return objptr;
}
