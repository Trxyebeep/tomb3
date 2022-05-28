#include "../tomb3/pch.h"
#include "draweffects.h"
#include "../3dsystem/3d_gen.h"
#include "../game/sphere.h"
#include "../3dsystem/hwinsert.h"
#include "game.h"
#include "../game/objects.h"
#include "../3dsystem/phd_math.h"

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
	long x1, y1, x2, y2, c0, c1;
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
			c0 = distances[nDs];
			c1 = distances[nDs + 1];
			nDs++;

			if (z < 32)
				continue;

			if (c0 > 255)
			{
				c0 = 511 - c0;

				if (c0 < 0)
					c0 = 0;
			}

			if (c1 > 255)
			{
				c1 = 511 - c1;

				if (c1 < 0)
					c1 = 0;
			}

			if (lr)
			{
				c0 >>= 1;
				c1 >>= 1;
			}

			if (ClipLine(x1, y1, x2, y2, w, h) &&
				x1 >= 0 && x1 <= w && y1 >= 0 && y1 <= h && x2 >= 0 && x2 <= w && y2 >= 0 && y2 <= h)
			{
				xStep = GlobalAlpha;
				GlobalAlpha = 0x70000000;
				z <<= W2V_SHIFT;
				HWI_InsertLine_Sorted(x1 - phd_winxmin, y1 - phd_winymin, x2 - phd_winxmin, y2 - phd_winymin, z, c0 | (c0 << 8), c1 | (c1 << 8));
				GlobalAlpha = xStep;
			}
		}

		nDs++;
	}

	phd_PopMatrix();
}

void DrawExplosionRings()
{
	DISPLAYMODE* dm;
	EXPLOSION_RING* ring;
	EXPLOSION_VERTS* vtx;
	EXPLOSION_VERTS* vtx2;
	PHDSPRITESTRUCT* sprite;
	PHD_VBUF v[4];
	PHD_VECTOR pos;
	PHDTEXTURESTRUCT tex;
	long* pZ;
	short* pXY;
	long* pZ2;
	short* pXY2;
	float zv;
	long w, h, rad, ang, r, g, b, x, y, z;
	long x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4, col1, col2, col3, col4;
	long Z[16];
	short XY[32];
	ushort u1, u2, v1, v2;
	char clipFlag;

	dm = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode[App.DXConfigPtr->nVMode];
	w = dm->w - 1;
	h = dm->h - 1;

	for (int i = 0; i < 6; i++)
	{
		ring = &ExpRings[i];

		if (!ring->on)
		{
			ring++;
			continue;
		}

		ring->life--;

		if (!ring->life)
		{
			ring->on = 0;
			ring++;
			continue;
		}

		phd_PushMatrix();
		phd_TranslateAbs(ring->x, ring->y, ring->z);
		phd_RotZ(ring->zrot << 4);
		phd_RotX(ring->xrot << 4);
		ring->radius += ring->speed;
		rad = ring->radius;
		vtx = ring->verts;
		pXY = XY;
		pZ = Z;

		for (int j = 0; j < 2; j++)
		{
			ang = (wibble & 0x3F) << 3;

			for (int k = 0; k < 8; k++)
			{
				vtx->x = short((rad * rcossin_tbl[ang << 1]) >> (W2V_SHIFT - 2));
				vtx->z = short((rad * rcossin_tbl[(ang << 1) + 1]) >> (W2V_SHIFT - 2));

				r = GetRandomDraw() & 0x1F;
				g = (GetRandomDraw() & 0x3F) + 224;
				b = (g >> 2) + (GetRandomDraw() & 0x3F);
				r = (r * ring->life) >> 5;
				g = (g * ring->life) >> 5;
				b = (b * ring->life) >> 5;
				vtx->rgb = r | (g << 8) | (b << 16);

				ang = (ang + 512) & 0xFFF;

				x = vtx->x;
				y = 0;
				z = vtx->z;

				pos.x = phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03];
				pos.y = phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13];
				pos.z = phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23];

				zv = f_persp / (float)pos.z;
				pos.x = short(float(pos.x * zv + f_centerx));
				pos.y = short(float(pos.y * zv + f_centery));

				pXY[0] = (short)pos.x;
				pXY[1] = (short)pos.y;
				pZ[0] = pos.z;

				vtx++;
				pXY += 2;
				pZ++;
			}

			rad >>= 1;
		}

		vtx = ring->verts;
		vtx2 = &ring->verts[8];
		pXY = XY;
		pZ = Z;
		pXY2 = &XY[16];
		pZ2 = &Z[8];

		x1 = *pXY++;
		y1 = *pXY++;
		z1 = *pZ++;
		x3 = *pXY2++;
		y3 = *pXY2++;
		z3 = *pZ2++;
		col1 = vtx->rgb;
		col3 = vtx2->rgb;
		sprite = &phdspriteinfo[objects[EXPLOSION1].mesh_index + 4 + ((wibble >> 4) & 3)];
		u1 = (sprite->offset << 8) & 0xFF00;
		v1 = sprite->offset & 0xFF00;
		u2 = ushort(u1 + sprite->width - App.nUVAdd);
		v2 = ushort(v1 + sprite->height - App.nUVAdd);
		u1 += (ushort)App.nUVAdd;
		v1 += (ushort)App.nUVAdd;

		for (int j = 0; j < 8; j++)
		{
			if (j == 7)
			{
				x2 = pXY[-16];
				y2 = pXY[-15];
				z2 = pZ[-8];
				x4 = pXY2[-16];
				y4 = pXY2[-15];
				z4 = pZ2[-8];
				col2 = vtx[-8].rgb;
				col4 = vtx->rgb;
			}
			else
			{
				x2 = *pXY++;
				y2 = *pXY++;
				z2 = *pZ++;
				x4 = *pXY2++;
				y4 = *pXY2++;
				z4 = *pZ2++;
				col2 = vtx->rgb;
				col4 = vtx2->rgb;
				vtx++;
				vtx2++;
			}

			if (((z1 + z2 + z3 + z4) >> 2) > phd_znear && (col1 || col2 || col3 || col4) &&
				x1 > -128 && x2 > -128 && x3 > -128 && x4 > -128 && x1 < w + 128 && x2 < w + 128 && x3 < w + 128 && x4 < w + 128 &&
				y1 > -128 && y2 > -128 && y3 > -128 && y4 > -128 && y1 < h + 128 && y2 < h + 128 && y3 < h + 128 && y4 < h + 128)
			{
				clipFlag = 0;

				if (x1 < phd_winxmin)
					clipFlag++;
				else if (x1 > phd_winxmax)
					clipFlag += 2;

				if (y1 < phd_winymin)
					clipFlag += 4;
				else if (y1 > phd_winxmax)
					clipFlag += 8;

				v[0].clip = clipFlag;
				v[0].xs = (float)x1;
				v[0].ys = (float)y1;
				v[0].zv = (float)z1;
				v[0].ooz = f_persp / (float)z1 * f_oneopersp;
				r = (col1 >> 3) & 0x1F;
				g = (col1 >> 11) & 0x1F;
				b = (col1 >> 19) & 0x1F;
				v[0].g = short(r << 10 | g << 5 | b);

				clipFlag = 0;

				if (x2 < phd_winxmin)
					clipFlag++;
				else if (x2 > phd_winxmax)
					clipFlag += 2;

				if (y2 < phd_winymin)
					clipFlag += 4;
				else if (y2 > phd_winxmax)
					clipFlag += 8;

				v[1].clip = clipFlag;
				v[1].xs = (float)x2;
				v[1].ys = (float)y2;
				v[1].zv = (float)z2;
				v[1].ooz = f_persp / (float)z2 * f_oneopersp;
				r = (col2 >> 3) & 0x1F;
				g = (col2 >> 11) & 0x1F;
				b = (col2 >> 19) & 0x1F;
				v[1].g = short(r << 10 | g << 5 | b);

				clipFlag = 0;

				if (x3 < phd_winxmin)
					clipFlag++;
				else if (x3 > phd_winxmax)
					clipFlag += 2;

				if (y3 < phd_winymin)
					clipFlag += 4;
				else if (y3 > phd_winxmax)
					clipFlag += 8;

				v[3].clip = clipFlag;
				v[3].xs = (float)x3;
				v[3].ys = (float)y3;
				v[3].zv = (float)z3;
				v[3].ooz = f_persp / (float)z3 * f_oneopersp;
				r = (col3 >> 3) & 0x1F;
				g = (col3 >> 11) & 0x1F;
				b = (col3 >> 19) & 0x1F;
				v[3].g = short(r << 10 | g << 5 | b);

				clipFlag = 0;

				if (x4 < phd_winxmin)
					clipFlag++;
				else if (x4 > phd_winxmax)
					clipFlag += 2;

				if (y4 < phd_winymin)
					clipFlag += 4;
				else if (y4 > phd_winxmax)
					clipFlag += 8;

				v[2].clip = clipFlag;
				v[2].xs = (float)x4;
				v[2].ys = (float)y4;
				v[2].zv = (float)z4;
				v[2].ooz = f_persp / (float)z4 * f_oneopersp;
			//	r = (col4 >> 3) & 0x1F;
			//	g = (col4 >> 11) & 0x1F;
			//	b = (col4 >> 19) & 0x1F;
			//	v[2].g = short(r << 10 | g << 5 | b);

				tex.u1 = u1;
				tex.u2 = u2;
				tex.u3 = u1;
				tex.u4 = u2;
				tex.v1 = v1;
				tex.v2 = v1;
				tex.v3 = v2;
				tex.v4 = v2;
				tex.tpage = sprite->tpage;
				tex.drawtype = 2;
				HWI_InsertGT4_Sorted(&v[0], &v[1], &v[2], &v[3], &tex, MID_SORT, 1);
			}

			x1 = x2;
			y1 = y2;
			z1 = z2;
			x3 = x4;
			y3 = y4;
			z3 = z4;
			col1 = col2;
			col3 = col4;
		}

		phd_PopMatrix();
	}
}

bool ClipLine(long& x1, long& y1, long& x2, long& y2, long w, long h)
{
	float clip;
	long xMax, xMin, yMax, yMin;

	xMax = phd_winxmin + phd_winxmax;
	xMin = phd_winxmin;
	yMax = phd_winymin + phd_winymax;
	yMin = phd_winymin;

	if (x1 < xMin && x2 < xMin || y1 < yMin && y2 < yMin)
		return 0;

	if (x1 > xMax && x2 > xMax || y1 > yMax && y2 > yMax)
		return 0;

	if (x1 > xMax)
	{
		clip = ((float)xMax - x2) / float(x1 - x2);
		x1 = xMax;
		y1 = long((y1 - y2) * clip + y2);
	}

	if (x2 > xMax)
	{
		clip = ((float)xMax - x1) / float(x2 - x1);
		x2 = xMax;
		y2 = long((y2 - y1) * clip + y1);
	}

	if (x1 < xMin)
	{
		clip = ((float)xMin - x1) / float(x2 - x1);
		x1 = xMin;
		y1 = long((y2 - y1) * clip + y1);
	}

	if (x2 < xMin)
	{
		clip = ((float)xMin - x2) / float(x1 - x2);
		x2 = xMin;
		y2 = long((y1 - y2) * clip + y2);
	}

	if (y1 > yMax)
	{
		clip = ((float)yMax - y2) / float(y1 - y2);
		y1 = yMax;
		x1 = long((x1 - x2) * clip + x2);
	}

	if (y2 > yMax)
	{
		clip = ((float)yMax - y1) / float(y2 - y1);
		y2 = yMax;
		x2 = long((x2 - x1) * clip + x1);
	}

	if (y1 < yMin)
	{
		clip = ((float)yMin - y1) / float(y2 - y1);
		y1 = yMin;
		x1 = long((x2 - x1) * clip + x1);
	}

	if (y2 < yMin)
	{
		clip = ((float)yMin - y2) / float(y1 - y2);
		y2 = yMin;
		x2 = long((x1 - x2) * clip + x2);
	}

	return 1;
}

void S_DrawWakeFX(ITEM_INFO* item)
{
	DISPLAYMODE* dm;
	WAKE_PTS* pt;
	PHD_VECTOR pos;
	long* pZ;
	short* pXY;
	uchar* pRGBs;
	float zv;
	long w, h, s, c, current, nw, s1, s2, s3, s4;
	long x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, c12, c34;
	long offsets[2][2];
	long Z[64];
	short XY[128];
	uchar rgbs[128];

	dm = &App.DeviceInfoPtr->DDInfo[App.DXConfigPtr->nDD].D3DInfo[App.DXConfigPtr->nD3D].DisplayMode[App.DXConfigPtr->nVMode];
	w = dm->w - 1;
	h = dm->h - 1;
	offsets[0][0] = -128;
	offsets[0][1] = 0;
	offsets[1][0] = 128;
	offsets[1][1] = 0;

	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);

	for (int i = 0; i < 2; i++)
	{
		pXY = XY;
		pZ = Z;
		pRGBs = rgbs;
		s = phd_sin(item->pos.y_rot);
		c = phd_cos(item->pos.y_rot);
		x1 = (item->pos.x_pos + ((offsets[i][1] * s + offsets[i][0] * c) >> W2V_SHIFT)) - lara_item->pos.x_pos;
		y1 = item->pos.y_pos - lara_item->pos.y_pos;
		z1 = (item->pos.z_pos + ((offsets[i][1] * c - offsets[i][0] * s) >> W2V_SHIFT)) - lara_item->pos.z_pos;

		pos.x = phd_mxptr[M00] * x1 + phd_mxptr[M01] * y1 + phd_mxptr[M02] * z1 + phd_mxptr[M03];
		pos.y = phd_mxptr[M10] * x1 + phd_mxptr[M11] * y1 + phd_mxptr[M12] * z1 + phd_mxptr[M13];
		pos.z = phd_mxptr[M20] * x1 + phd_mxptr[M21] * y1 + phd_mxptr[M22] * z1 + phd_mxptr[M23];

		zv = f_persp / (float)pos.z;
		pos.x = short(float(pos.x * zv + f_centerx));
		pos.y = short(float(pos.y * zv + f_centery));

		pXY[0] = (short)pos.x;
		pXY[1] = (short)pos.y;
		pXY += 2;
		*pZ++ = pos.z;
		pXY[0] = (short)pos.x;
		pXY[1] = (short)pos.y;
		pXY += 2;
		*pZ++ = pos.z;
		*pRGBs++ = 64;
		current = (CurrentStartWake - 1) & 0x1F;

		for (nw = 0; nw < 32; nw++)
		{
			pt = &WakePts[current][i];

			if (pt->life)
			{
				for (int k = 0; k < 2; k++)
				{
					x1 = pt->x[k] - lara_item->pos.x_pos;
					y1 = pt->y - lara_item->pos.y_pos;
					z1 = pt->z[k] - lara_item->pos.z_pos;
					pos.x = phd_mxptr[M00] * x1 + phd_mxptr[M01] * y1 + phd_mxptr[M02] * z1 + phd_mxptr[M03];
					pos.y = phd_mxptr[M10] * x1 + phd_mxptr[M11] * y1 + phd_mxptr[M12] * z1 + phd_mxptr[M13];
					pos.z = phd_mxptr[M20] * x1 + phd_mxptr[M21] * y1 + phd_mxptr[M22] * z1 + phd_mxptr[M23];

					zv = f_persp / (float)pos.z;
					pos.x = short(float(pos.x * zv + f_centerx));
					pos.y = short(float(pos.y * zv + f_centery));

					pXY[0] = (short)pos.x;
					pXY[1] = (short)pos.y;
					pXY += 2;
					*pZ++ = pos.z;
				}

				*pRGBs++ = pt->life;
			}
			else
			{
				*pZ = -5555;
				break;
			}

			current = (current - 1) & 0x1F;
		}

		if (!nw)
			break;

		pXY = XY;
		pZ = Z;
		pRGBs = rgbs;
		x1 = *pXY++;
		y1 = *pXY++;
		x2 = *pXY++;
		y2 = *pXY++;
		z1 = *pZ++;
		z2 = *pZ++;
		c12 = *pRGBs++;

		if (WakeShade < 16)
			c12 = (c12 * WakeShade) >> 4;

		for (;;)
		{
			z3 = *pZ++;

			if (z3 == -5555)
				break;

			x3 = *pXY++;
			y3 = *pXY++;
			x4 = *pXY++;
			y4 = *pXY++;
			z4 = *pZ++;
			c34 = *pRGBs++;

			if (WakeShade < 16)
				c34 = (c34 * WakeShade) >> 4;

			z1 = (z1 + z2 + z3 + z4) >> 2;

			if ((z1 & -0x4000) > 0x80000 &&
				x1 > -128 && x2 > -128 && x3 > -128 && x4 > -128 &&
				x1 < w + 128 && x2 < w + 128 && x3 < w + 128 && x4 < w + 128 &&
				y1 > -128 && y2 > -128 && y3 > -128 && y4 > -128 &&
				y1 < h + 128 && y2 < h + 128 && y3 < h + 128 && y4 < h + 128)
			{
				s1 = (c12 >> 2) | (8 * (c12 & -4 | (32 * (c12 & -4))));
				s2 = (c12 >> 1) | (16 * (c12 & -2 | (32 * (c12 & -2))));
				s3 = (c34 >> 1) | (16 * (c34 & -2 | (32 * (c34 & -2))));
				s4 = (c34 >> 2) | (8 * (c34 & -4 | (32 * (c34 & -4))));
				HWI_InsertAlphaSprite_Sorted(x1, y1, z1, s1, x2, y2, z2, s2, x3, y3, z3, s3, x4, y4, z4, s4, -1, 16, 1);
			}

			y1 = y4;
			x1 = x4;
			z1 = z4;
			x2 = x3;
			y2 = y3;
			z2 = z3;
			c12 = c34;
		}
	}

	phd_PopMatrix();
}

void inject_draweffects(bool replace)
{
	INJECT(0x00478600, LaraElectricDeath, replace);
	INJECT(0x00476CA0, DrawExplosionRings, replace);
	INJECT(0x00479510, ClipLine, replace);
	INJECT(0x0047F4C0, S_DrawWakeFX, replace);
}
