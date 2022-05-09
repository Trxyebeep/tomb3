#include "../tomb3/pch.h"
#include "transform.h"

static inline void phdMx2Flt()
{
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
}

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

void inject_transform(bool replace)
{
	INJECT(0x0048D6A0, InitZTable, replace);
	INJECT(0x0048D6E0, calc_object_vertices, replace);
}
