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

void inject_hwinsert(bool replace)
{
	INJECT(0x0040A850, HWI_InsertTrans8_Sorted, replace);
}
