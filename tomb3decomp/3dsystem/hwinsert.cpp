#include "../tomb3/pch.h"
#include "hwinsert.h"

void HWI_InsertTrans8_Sorted(PHD_VBUF* buf, short shade)
{
	float z;
	long nPoints;
	char clip;

	clip = buf[0].clip | buf[1].clip | buf[2].clip | buf[3].clip | buf[4].clip | buf[5].clip | buf[6].clip | buf[7].clip;

	if (clip < 0 || (buf[0].clip & buf[1].clip & buf[2].clip & buf[3].clip & buf[4].clip & buf[5].clip & buf[6].clip & buf[7].clip) ||
		(buf[2].xs - buf[1].xs) * (buf->ys - buf[1].ys) - (buf[2].ys - buf[1].ys) * (buf->xs - buf[1].xs) < 0)
		return;

	for (int i = 0; i < 8; i++)
	{
		v_buffer[i].x = buf[i].xs;
		v_buffer[i].y = buf[i].ys;
		v_buffer[i].ooz = one / (buf[i].zv - 131072);
		v_buffer[i].vr = 0;
		v_buffer[i].vg = 0;
		v_buffer[i].vb = 0;
	}

	nPoints = 8;

	if (clip)
	{
		phd_leftfloat = (float)phd_winxmin;
		phd_topfloat = (float)phd_winymin;
		phd_rightfloat = float(phd_winxmin + phd_winwidth);
		phd_bottomfloat = float(phd_winymin + phd_winheight);
		nPoints = XYClipper(nPoints, v_buffer);
	}

	if (nPoints)
	{
		z = (buf[0].zv + buf[1].zv + buf[2].zv + buf[3].zv + buf[4].zv + buf[5].zv + buf[6].zv + buf[7].zv) * 0.125F - 131072;
		HWI_InsertPoly_Gouraud(nPoints, z, 0, 0, 0, 13);
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

	r0 = (v0->g >> 10) & 0x1F;
	r1 = (v1->g >> 10) & 0x1F;

	g0 = (v0->g >> 5) & 0x1F;
	g1 = (v1->g >> 5) & 0x1F;

	b0 = v0->g & 0x1F;
	b1 = v1->g & 0x1F;

	r = (r0 + r1) >> 1;
	g = (g0 + g1) >> 1;
	b = (b0 + b1) >> 1;
	dest->g = short(r << 10 | g << 5 | b);

	dest->u = (v0->u + v1->u) >> 1;
	dest->v = (v0->v + v1->v) >> 1;
}

void inject_hwinsert(bool replace)
{
	INJECT(0x0040A850, HWI_InsertTrans8_Sorted, replace);
	INJECT(0x00406880, SubdivideEdge, replace);
}
