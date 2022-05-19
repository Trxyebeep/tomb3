#include "../tomb3/pch.h"
#include "draweffects.h"
#include "../3dsystem/3d_gen.h"
#include "../game/sphere.h"
#include "../3dsystem/hwinsert.h"

void LaraElectricDeath(long lr, ITEM_INFO* item)
{
	DISPLAYMODE* dm;
	PHD_VECTOR pos;
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	PHD_VECTOR point;
	short* points;
	float zv;
	long w, h, nDs, mesh1, mesh2, x, y, z, xStep, yStep, zStep, pX, pY, pZ;
	long x1, y1, x2, y2, col;
	long coords[600];
	short distances[200];

	/**/
	item = lara_item;//remove this line to fix the bug where electricity always appears on Lara. Core used lara_item instead of item inside the entire func.
	/**/
	dm = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode[App.DXConfigPtr->nVMode];
	w = dm->w - 1;
	h = dm->h - 1;
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	nDs = 0;

	for (int i = 0; i < 14; i++)
	{
		mesh1 = lara_meshes[2 * i];
		mesh2 = lara_meshes[2 * i + 1];
		points = &electricity_points[(5 * i) & 0xF][0];

		pos1.y = 0;

		if (lr)
		{
			pos1.x = -48;
			pos1.z = -48;
		}
		else
		{
			pos1.x = 48;
			pos1.z = 48;
		}

		GetJointAbsPosition(item, &pos1, mesh1);
		pos2.y = 0;

		if (!lara_last_points[i] || i == 13)
		{
			if (lr)
			{
				pos2.z = -48;
				pos2.x = -48;
			}
			else
			{
				pos2.x = 48;
				pos2.z = 48;
			}

			if (i == 13)
				pos2.y = -64;
		}
		else
		{
			pos2.x = 0;
			pos2.z = 0;
		}

		GetJointAbsPosition(item, &pos2, mesh2);
		pos1.x -= item->pos.x_pos;
		pos1.y -= item->pos.y_pos;
		pos1.z -= item->pos.z_pos;
		pos2.x -= item->pos.x_pos;
		pos2.y -= item->pos.y_pos;
		pos2.z -= item->pos.z_pos;
		x = pos1.x;
		y = pos1.y;
		z = pos1.z;
		xStep = (pos2.x - pos1.x) >> 2;
		yStep = (pos2.y - pos1.y) >> 2;
		zStep = (pos2.z - pos1.z) >> 2;

		for (int j = 0; j < 5; j++)
		{
			if (j == 4 && !lara_last_points[i])
				break;

			if (j == 4 && lara_last_points[i])
			{
				pos.x = pos2.x;
				pos.y = pos2.y;
				pos.z = pos2.z;
			}
			else
			{
				pos.x = x;
				pos.y = y;
				pos.z = z;
			}

			if (!j || j == 4)
				distances[nDs] = 0;
			else
			{
				point.x = *points++;
				point.y = *points++;
				point.z = *points++;
				points += 3;	//dont need vels

				if (lr)
				{
					pos.x -= point.x >> 3;
					pos.y -= point.y >> 3;
					pos.z -= point.z >> 3;
				}
				else
				{
					pos.x += point.x >> 3;
					pos.y += point.y >> 3;
					pos.z += point.z >> 3;
				}

				pX = ABS(point.x);
				pY = ABS(point.y);
				pZ = ABS(point.z);

				if (pY > pX)
					pX = pY;

				if (pZ > pX)
					pX = pZ;

				distances[nDs] = (short)pX;
			}

			pX = phd_mxptr[M00] * pos.x + phd_mxptr[M01] * pos.y + phd_mxptr[M02] * pos.z + phd_mxptr[M03];
			pY = phd_mxptr[M10] * pos.x + phd_mxptr[M11] * pos.y + phd_mxptr[M12] * pos.z + phd_mxptr[M13];
			pZ = phd_mxptr[M20] * pos.x + phd_mxptr[M21] * pos.y + phd_mxptr[M22] * pos.z + phd_mxptr[M23];
			zv = f_persp / (float)pZ;
			coords[3 * nDs] = short(float(pX * zv + f_centerx));
			coords[3 * nDs + 1] = short(float(pY * zv + f_centery));
			coords[3 * nDs + 2] = pZ >> W2V_SHIFT;
			nDs++;
			x += xStep;
			y += yStep;
			z += zStep;
		}
	}

	nDs = 0;

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < lara_line_counts[i]; j++)
		{
			x1 = coords[3 * nDs];
			y1 = coords[3 * nDs + 1];
			z = coords[3 * nDs + 2];
			x2 = coords[3 * nDs + 3];
			y2 = coords[3 * nDs + 4];
			col = distances[nDs];
			nDs++;

			if (z < 32)
				continue;

			if (col > 255)
			{
				col = 511 - col;

				if (col < 0)
					col = 0;
			}

			if (lr)
				col >>= 1;

			if (ClipLine(x1, y1, x2, y2, w, h) &&
				x1 >= 0 && x1 <= w && y1 >= 0 && y1 <= h && x2 >= 0 && x2 <= w && y2 >= 0 && y2 <= h)
			{
				xStep = GlobalAlpha;
				GlobalAlpha = 0x70000000;
				HWI_InsertLine_Sorted(x1 - phd_winxmin, y1 - phd_winymin, x2 - phd_winxmin, y2 - phd_winymin, z << W2V_SHIFT, col | (col << 8));
				GlobalAlpha = xStep;
			}
		}

		nDs++;
	}

	phd_PopMatrix();
}

void inject_draweffects(bool replace)
{
	INJECT(0x00478600, LaraElectricDeath, replace);
}
