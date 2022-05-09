#include "../tomb3/pch.h"
#include "output.h"
#include "../3dsystem/3d_gen.h"
#include "transform.h"

static short shadow[6 + (3 * 8)] =
{
	0, 0, 0,		//x, y, z
	32767, 1, 8,	//size, nPolys, nVtx
	0, 0, 0,		//8 vtx-> x, y, z
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};

void S_PrintShadow(short size, short* box, ITEM_INFO* item)
{
	long x0, x1, z0, z1, midX, midZ, xAdd, zAdd;

	x0 = box[0];
	x1 = box[1];
	z0 = box[4];
	z1 = box[5];
	midX = (x0 + x1) / 2;
	xAdd = (x1 - x0) * size / 1024;
	midZ = (z0 + z1) / 2;
	zAdd = (z1 - z0) * size / 1024;

	shadow[6] = short(midX - xAdd);
	shadow[8] = short(midZ + zAdd * 2);

	shadow[9] = short(midX + xAdd);
	shadow[11] = short(midZ + zAdd * 2);

	shadow[12] = short(midX + xAdd * 2);
	shadow[14] = short(midZ + zAdd);

	shadow[15] = short(midX + xAdd * 2);
	shadow[17] = short(midZ - zAdd);

	shadow[18] = short(midX + xAdd);
	shadow[20] = short(midZ - zAdd * 2);

	shadow[21] = short(midX - xAdd);
	shadow[23] = short(midZ - zAdd * 2);

	shadow[24] = short(midX - xAdd * 2);
	shadow[26] = short(midZ - zAdd);

	shadow[27] = short(midX - xAdd * 2);
	shadow[29] = short(midZ + zAdd);

	phd_leftfloat = float(phd_winxmin + phd_left);
	phd_topfloat = float(phd_winymin + phd_top);
	phd_rightfloat = float(phd_right + phd_winxmin + 1);
	phd_bottomfloat = float(phd_bottom + phd_winymin + 1);
	f_centerx = float(phd_winxmin + phd_centerx);
	f_centery = float(phd_winymin + phd_centery);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->floor, item->pos.z_pos);
	phd_RotY(item->pos.y_rot);

	if (calc_object_vertices(&shadow[4]))
		InsertTrans8(vbuf, 32);

	phd_PopMatrix();
}

void inject_output(bool replace)
{
	INJECT(0x0048A7B0, S_PrintShadow, replace);
}
