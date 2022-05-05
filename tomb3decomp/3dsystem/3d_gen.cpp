#include "../tomb3/pch.h"
#include "3d_gen.h"
#include "../specific/transform.h"
#include "../specific/litesrc.h"

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

void inject_3dgen(bool replace)
{
	INJECT(0x00401AF0, phd_PutPolygons, replace);
}
