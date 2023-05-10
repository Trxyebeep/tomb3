#include "../tomb3/pch.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "objects.h"
#include "../3dsystem/scalespr.h"
#include "../specific/litesrc.h"
#include "../specific/game.h"
#include "effects.h"
#include "hair.h"
#include "lara.h"
#include "effect2.h"
#include "control.h"
#include "sphere.h"
#include "../specific/draweffects.h"
#include "gameflow.h"
#include "laraelec.h"
#include "health.h"
#include "items.h"
#include "../specific/smain.h"
#include "../specific/input.h"
#include "camera.h"
#include "../newstuff/LaraDraw.h"
#include "../tomb3/tomb3.h"

static uchar EnemyWeapon[16] = { 0, 1, 129, 0, 1, 1,  1 };
static long bound_list[128];
static long bound_start;
static long bound_end;

long box_lines[12][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7} };
short null_rotations[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

long* IMptr;
long IM_rate;
long IM_frac;
long IMstack[768];

long number_draw_rooms;
short draw_rooms[100];
char IsJointUnderwater[15];
char GotJointPos[15];

long nPolyType;
long camera_underwater;
long mid_sort = 0;

long outside;
static long outside_top;
static long outside_left;
static long outside_right;
static long outside_bottom;

MESH_INFO* CurrentMesh;
long CurrentRoom;

static BITE_INFO EnemyBites[16] =	//bite_offsets enum
{
	{ 0, 192, 40, 13 },
	{ 0, 400, 64, 7 },
	{ 0, 300, 56, 7 },
	{ 0, 200, 106, 7 },
	{ 110, -30, -530, 2 },
	{ -110, -30, -530, 2 },
	{ 0, 300, 56, 7 }
};

void phd_PopMatrix_I()
{
	phd_PopMatrix();
	IMptr -= indices_count;
}

void phd_PushMatrix_I()
{
	phd_PushMatrix();
	memcpy(IMptr + indices_count, IMptr, 48);
	IMptr += indices_count;
}

void phd_RotY_I(short angle)
{
	long* mx;
	
	phd_RotY(angle);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotY(angle);
	phd_mxptr = mx;
}

void phd_RotX_I(short angle)
{
	long* mx;

	phd_RotX(angle);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotX(angle);
	phd_mxptr = mx;
}

void phd_RotZ_I(short angle)
{
	long* mx;

	phd_RotZ(angle);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotZ(angle);
	phd_mxptr = mx;
}

void phd_TranslateRel_I(long x, long y, long z)
{
	long* mx;

	phd_TranslateRel(x, y, z);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_TranslateRel(x, y, z);
	phd_mxptr = mx;
}

void phd_TranslateRel_ID(long x, long y, long z, long x2, long y2, long z2)
{
	long* mx;

	phd_TranslateRel(x, y, z);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_TranslateRel(x2, y2, z2);
	phd_mxptr = mx;
}

void phd_RotYXZ_I(short y, short x, short z)
{
	long* mx;

	phd_RotYXZ(y, x, z);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotYXZ(y, x, z);
	phd_mxptr = mx;
}

void phd_PutPolygons_I(short* objptr, long clip)
{
	phd_PushMatrix();
	InterpolateMatrix();
	phd_PutPolygons(objptr, clip);
	phd_PopMatrix();
}

void gar_RotYXZsuperpack(short** pprot, long skip)
{
	ushort* prot;
	long packed;

	while (skip)
	{
		prot = (ushort*)*pprot;

		if (*prot & 0xC000)
			*pprot += 1;
		else
			*pprot += 2;

		skip--;
	}

	prot = (ushort*)*pprot;

	switch (*prot >> 14)
	{
	case 0:
		packed = (*prot << 16) + *(prot + 1);
		phd_RotYXZpack(packed);
		(*pprot)++;
		break;

	case 1:
		phd_RotX(short((*prot & 1023) << 6));
		break;

	case 2:
		phd_RotY(short((*prot & 1023) << 6));
		break;

	default:
		phd_RotZ(short((*prot & 1023) << 6));
		break;
	}

	(*pprot)++;
}

void gar_RotYXZsuperpack_I(short** pprot1, short** pprot2, long skip)
{
	long* mx;

	gar_RotYXZsuperpack(pprot1, skip);
	mx = phd_mxptr;
	phd_mxptr = IMptr;
	gar_RotYXZsuperpack(pprot2, skip);
	phd_mxptr = mx;
}

void InitInterpolate(long frac, long rate)
{
	IM_rate = rate;
	IM_frac = frac;
	IMptr = IMstack;
	memcpy(IMstack, phd_mxptr, 48);
}

void InterpolateMatrix()
{
	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M00] += (IMptr[M00] - phd_mxptr[M00]) >> 1;
		phd_mxptr[M01] += (IMptr[M01] - phd_mxptr[M01]) >> 1;
		phd_mxptr[M02] += (IMptr[M02] - phd_mxptr[M02]) >> 1;
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 1;
		phd_mxptr[M10] += (IMptr[M10] - phd_mxptr[M10]) >> 1;
		phd_mxptr[M11] += (IMptr[M11] - phd_mxptr[M11]) >> 1;
		phd_mxptr[M12] += (IMptr[M12] - phd_mxptr[M12]) >> 1;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 1;
		phd_mxptr[M20] += (IMptr[M20] - phd_mxptr[M20]) >> 1;
		phd_mxptr[M21] += (IMptr[M21] - phd_mxptr[M21]) >> 1;
		phd_mxptr[M22] += (IMptr[M22] - phd_mxptr[M22]) >> 1;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M00] += (IMptr[M00] - phd_mxptr[M00]) >> 2;
		phd_mxptr[M01] += (IMptr[M01] - phd_mxptr[M01]) >> 2;
		phd_mxptr[M02] += (IMptr[M02] - phd_mxptr[M02]) >> 2;
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M10] += (IMptr[M10] - phd_mxptr[M10]) >> 2;
		phd_mxptr[M11] += (IMptr[M11] - phd_mxptr[M11]) >> 2;
		phd_mxptr[M12] += (IMptr[M12] - phd_mxptr[M12]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M20] += (IMptr[M20] - phd_mxptr[M20]) >> 2;
		phd_mxptr[M21] += (IMptr[M21] - phd_mxptr[M21]) >> 2;
		phd_mxptr[M22] += (IMptr[M22] - phd_mxptr[M22]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M00] = IMptr[M00] - ((IMptr[M00] - phd_mxptr[M00]) >> 2);
		phd_mxptr[M01] = IMptr[M01] - ((IMptr[M01] - phd_mxptr[M01]) >> 2);
		phd_mxptr[M02] = IMptr[M02] - ((IMptr[M02] - phd_mxptr[M02]) >> 2);
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M10] = IMptr[M10] - ((IMptr[M10] - phd_mxptr[M10]) >> 2);
		phd_mxptr[M11] = IMptr[M11] - ((IMptr[M11] - phd_mxptr[M11]) >> 2);
		phd_mxptr[M12] = IMptr[M12] - ((IMptr[M12] - phd_mxptr[M12]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M20] = IMptr[M20] - ((IMptr[M20] - phd_mxptr[M20]) >> 2);
		phd_mxptr[M21] = IMptr[M21] - ((IMptr[M21] - phd_mxptr[M21]) >> 2);
		phd_mxptr[M22] = IMptr[M22] - ((IMptr[M22] - phd_mxptr[M22]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

void InterpolateArmMatrix()
{
	phd_mxptr[M00] = phd_mxptr[M00 - 24];
	phd_mxptr[M01] = phd_mxptr[M01 - 24];
	phd_mxptr[M02] = phd_mxptr[M02 - 24];
	phd_mxptr[M10] = phd_mxptr[M10 - 24];
	phd_mxptr[M11] = phd_mxptr[M11 - 24];
	phd_mxptr[M12] = phd_mxptr[M12 - 24];
	phd_mxptr[M20] = phd_mxptr[M20 - 24];
	phd_mxptr[M21] = phd_mxptr[M21 - 24];
	phd_mxptr[M22] = phd_mxptr[M22 - 24];

	if (IM_rate == 2 || (IM_frac == 2 && IM_rate == 4))
	{
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
	}
	else if (IM_frac == 1)
	{
		phd_mxptr[M03] += (IMptr[M03] - phd_mxptr[M03]) >> 2;
		phd_mxptr[M13] += (IMptr[M13] - phd_mxptr[M13]) >> 2;
		phd_mxptr[M23] += (IMptr[M23] - phd_mxptr[M23]) >> 2;
	}
	else
	{
		phd_mxptr[M03] = IMptr[M03] - ((IMptr[M03] - phd_mxptr[M03]) >> 2);
		phd_mxptr[M13] = IMptr[M13] - ((IMptr[M13] - phd_mxptr[M13]) >> 2);
		phd_mxptr[M23] = IMptr[M23] - ((IMptr[M23] - phd_mxptr[M23]) >> 2);
	}
}

long GetFrames(ITEM_INFO* item, short* frm[], long* rate)
{
	ANIM_STRUCT* anim;
	long frame, size, frac, num;

	anim = &anims[item->anim_number];
	frm[0] = anim->frame_ptr;
	frm[1] = anim->frame_ptr;
	*rate = anim->interpolation & 0xFF;
	frame = item->frame_number - anim->frame_base;
	size = anim->interpolation >> 8;
	frm[0] += frame / *rate * size;
	frm[1] = frm[0] + size;
	frac = (frame % *rate);

	if (!frac)
		return 0;

	num = *rate * (frame / *rate + 1);

	if (num > anim->frame_end)
		*rate = *rate + anim->frame_end - num;

	return frac;
}

short* GetBestFrame(ITEM_INFO* item)
{
	short* frm[2];
	long frac, rate;

	frac = GetFrames(item, frm, &rate);

	if (frac > (rate >> 1))
		return frm[1];
	else
		return frm[0];
}

short* GetBoundsAccurate(ITEM_INFO* item)
{
	short* frmptr[2];
	short* bptr;
	long frac, rate;
	static short interpolated_bounds[6];

	frac = GetFrames(item, frmptr, &rate);

	if (!frac)
		return frmptr[0];

	bptr = interpolated_bounds;

	for (int i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
		*bptr = short(*frmptr[0] + (frac * (*frmptr[1] - *frmptr[0]) / rate));

	return interpolated_bounds;
}

void SetRoomBounds(short* door, long rn, ROOM_INFO* actualRoom)
{
	ROOM_INFO* r;
	PHD_VECTOR* v;
	PHD_VECTOR* lastV;
	static PHD_VECTOR vtxbuf[4];
	long x, y, z, tooNear, tooFar, tL, tR, tT, tB;

	r = &room[rn];

	if (r->left <= actualRoom->test_left && r->right >= actualRoom->test_right && r->top <= actualRoom->test_top && r->bottom >= actualRoom->test_bottom)
		return;

	tL = actualRoom->test_right;
	tR = actualRoom->test_left;
	tB = actualRoom->test_top;
	tT = actualRoom->test_bottom;
	door += 3;
	v = vtxbuf;
	tooNear = 0;
	tooFar = 0;

	for (int i = 0; i < 4; i++, v++, door += 3)
	{
		v->x = phd_mxptr[M00] * door[0] + phd_mxptr[M01] * door[1] + phd_mxptr[M02] * door[2] + phd_mxptr[M03];
		v->y = phd_mxptr[M10] * door[0] + phd_mxptr[M11] * door[1] + phd_mxptr[M12] * door[2] + phd_mxptr[M13];
		v->z = phd_mxptr[M20] * door[0] + phd_mxptr[M21] * door[1] + phd_mxptr[M22] * door[2] + phd_mxptr[M23];
		x = v->x;
		y = v->y;
		z = v->z;


		if (z <= 0)
			tooNear++;
		else
		{
			if (z > phd_zfar)
				tooFar++;

			z /= phd_persp;

			if (z)
			{
				x = x / z + phd_centerx;
				y = y / z + phd_centery;
			}
			else
			{
				if (x < 0)
					x = phd_left;
				else
					x = phd_right;

				if (y < 0)
					y = phd_top;
				else
					y = phd_bottom;
			}

			if (x - 1 < tL)
				tL = x - 1;

			if (x + 1 > tR)
				tR = x + 1;

			if (y - 1 < tT)
				tT = y - 1;

			if (y + 1 > tB)
				tB = y + 1;
		}
	}

	if (tooNear == 4 || (tooFar == 4 && !outside))
		return;

	if (tooNear > 0)
	{
		v = vtxbuf;
		lastV = &vtxbuf[3];

		for (int i = 0; i < 4; i++, lastV = v, v++)
		{
			if (lastV->z <= 0 == v->z <= 0)
				continue;

			if (v->x < 0 && lastV->x < 0)
				tL = 0;
			else if (v->x > 0 && lastV->x > 0)
				tR = phd_winxmax;
			else
			{
				tL = 0;
				tR = phd_winxmax;
			}

			if (v->y < 0 && lastV->y < 0)
				tT = 0;
			else if (v->y > 0 && lastV->y > 0)
				tB = phd_winymax;
			else
			{
				tT = 0;
				tB = phd_winymax;
			}
		}
	}

	if (tL < actualRoom->test_left)
		tL = actualRoom->test_left;

	if (tR > actualRoom->test_right)
		tR = actualRoom->test_right;

	if (tT < actualRoom->test_top)
		tT = actualRoom->test_top;

	if (tB > actualRoom->test_bottom)
		tB = actualRoom->test_bottom;

	if (tL >= tR || tT >= tB)
		return;

	if (r->bound_active & 2)
	{
		if (tL < r->test_left)
			r->test_left = (short)tL;

		if (tT < r->test_top)
			r->test_top = (short)tT;

		if (tR > r->test_right)
			r->test_right = (short)tR;

		if (tB > r->test_bottom)
			r->test_bottom = (short)tB;
	}
	else
	{
		bound_list[bound_end % 128] = rn;
		bound_end++;
		r->bound_active |= 2;
		r->test_left = (short)tL;
		r->test_right = (short)tR;
		r->test_top = (short)tT;
		r->test_bottom = (short)tB;
	}
}

void GetRoomBounds()
{
	ROOM_INFO* r;
	short* door;
	long rn, drn;

	while (bound_start != bound_end)
	{
		rn = bound_list[bound_start % 128];
		bound_start++;
		r = &room[rn];
		r->bound_active -= 2;
		mid_sort = (r->bound_active >> 8) + 1;

		if (r->test_left < r->left)
			r->left = r->test_left;

		if (r->test_top < r->top)
			r->top = r->test_top;

		if (r->test_right > r->right)
			r->right = r->test_right;

		if (r->test_bottom > r->bottom)
			r->bottom = r->test_bottom;

		if (!(r->bound_active & 1))
		{
			draw_rooms[number_draw_rooms] = (short)rn;
			number_draw_rooms++;
			r->bound_active |= 1;

			if (r->flags & ROOM_OUTSIDE)
				outside = ROOM_OUTSIDE;
		}

		if (r->flags & ROOM_OUTSIDE)
		{
			if (r->left < outside_left)
				outside_left = r->left;

			if (r->right > outside_right)
				outside_right = r->right;

			if (r->top < outside_top)
				outside_top = r->top;

			if (r->bottom > outside_bottom)
				outside_bottom = r->bottom;
		}

		phd_PushMatrix();
		phd_TranslateAbs(r->x, r->y, r->z);
		door = r->door;

		if (door)
		{
			for (drn = *door++; drn > 0; drn--)
			{
				rn = *door++;

				if (door[0] * (r->x + door[3] - w2v_matrix[M03]) +
					door[1] * (r->y + door[4] - w2v_matrix[M13]) +
					door[2] * (r->z + door[5] - w2v_matrix[M23]) < 0)
					SetRoomBounds(door, rn, r);

				door += 15;
			}
		}

		phd_PopMatrix();
	}
}

void ClipRoom(ROOM_INFO* r)
{
	long xv[8];
	long yv[8];
	long zv[8];
	long clip[8];
	long clip_room, x, y, z, xmin, xmax, ymin, ymax, l1, l2, div;

	xv[0] = 1024;
	xv[1] = (r->y_size << 10) - 1024;
	xv[2] = (r->y_size << 10) - 1024;
	xv[3] = 1024;
	xv[4] = 1024;
	xv[5] = (r->y_size << 10) - 1024;
	xv[6] = (r->y_size << 10) - 1024;
	xv[7] = 1024;

	yv[0] = r->maxceiling - r->y;
	yv[1] = r->maxceiling - r->y;
	yv[2] = r->maxceiling - r->y;
	yv[3] = r->maxceiling - r->y;
	yv[4] = r->minfloor - r->y;
	yv[5] = r->minfloor - r->y;
	yv[6] = r->minfloor - r->y;
	yv[7] = r->minfloor - r->y;

	zv[0] = 1024;
	zv[1] = 1024;
	zv[2] = (r->x_size << 10) - 1024;
	zv[3] = (r->x_size << 10) - 1024;
	zv[4] = 1024;
	zv[5] = 1024;
	zv[6] = (r->x_size << 10) - 1024;
	zv[7] = (r->x_size << 10) - 1024;

	clip_room = 0;

	for (int i = 0; i < 8; i++)
	{
		x = xv[i];
		y = yv[i];
		z = zv[i];

		xv[i] = x * phd_mxptr[M00] + y * phd_mxptr[M01] + z * phd_mxptr[M02] + phd_mxptr[M03];
		yv[i] = x * phd_mxptr[M10] + y * phd_mxptr[M11] + z * phd_mxptr[M12] + phd_mxptr[M13];
		zv[i] = x * phd_mxptr[M20] + y * phd_mxptr[M21] + z * phd_mxptr[M22] + phd_mxptr[M23];

		if (zv[i] > phd_zfar)
		{
			clip_room = 1;
			clip[i] = 1;
		}
		else
			clip[i] = 0;
	}

	if (!clip_room)
		return;

	xmin = 0x10000000;
	xmax = -0x10000000;
	ymin = 0x10000000;
	ymax = -0x10000000;

	for (int i = 0; i < 12; i++)
	{
		l1 = box_lines[i][0];
		l2 = box_lines[i][1];

		if (clip[l1] != clip[l2])
		{
			div = (zv[l2] - zv[l1]) >> W2V_SHIFT;

			if (div)
			{
				z = (phd_zfar - zv[l1]) >> W2V_SHIFT;
				x = xv[l1] + ((z * ((xv[l2] - xv[l1]) >> W2V_SHIFT) / div) << W2V_SHIFT);
				y = yv[l1] + ((z * ((yv[l2] - yv[l1]) >> W2V_SHIFT) / div) << W2V_SHIFT);

				if (x < xmin)
					xmin = x;

				if (x > xmax)
					xmax = x;

				if (y < ymin)
					ymin = y;

				if (y > ymax)
					ymax = y;
			}
			else
			{
				if (xv[l1] < xmin)
					xmin = xv[l1];

				if (xv[l2] < xmin)
					xmin = xv[l2];

				if (xv[l1] > xmax)
					xmax = xv[l1];

				if (xv[l2] > xmax)
					xmax = xv[l2];

				if (yv[l1] < ymin)
					ymin = yv[l1];

				if (yv[l2] < ymin)
					ymin = yv[l2];

				if (yv[l1] > ymax)
					ymax = yv[l1];

				if (yv[l2] > ymax)
					ymax = yv[l2];
			}
		}
	}

	xmin = phd_centerx + xmin / (phd_zfar / phd_persp);
	xmax = phd_centerx + xmax / (phd_zfar / phd_persp);
	ymin = phd_centery + ymin / (phd_zfar / phd_persp);
	ymax = phd_centery + ymax / (phd_zfar / phd_persp);

	if (xmin <= phd_right && ymin <= phd_bottom && xmax >= phd_left && ymax >= phd_top)
	{
		if (xmin < phd_left)
			xmin = phd_left;

		if (ymin < phd_top)
			ymin = phd_top;

		if (xmax > phd_right)
			xmax = phd_right;

		if (ymax > phd_bottom)
			ymax = phd_bottom;

		if (outside)
			S_InsertBackPolygon(xmin, (long)outsideBackgroundTop, xmax, ymax, 0);
		else
			S_InsertBackPolygon(xmin, ymin, xmax, ymax, 0);
	}
}

void PrintRooms(short current_room)
{
	ROOM_INFO* r;

	CurrentRoom = current_room;
	r = &room[current_room];

	if (r->flags & ROOM_UNDERWATER)
		S_SetupBelowWater(camera_underwater);
	else
		S_SetupAboveWater(camera_underwater);

	phd_TranslateAbs(r->x, r->y, r->z);
	phd_left = r->left;
	phd_right = r->right;
	phd_top = r->top;
	phd_bottom = r->bottom;

	if (outside > 0 && !(r->flags & ROOM_INSIDE))
		S_InsertRoom(r->data, 1);
	else
	{
		if (outside >= 0)
			ClipRoom(r);

		S_InsertRoom(r->data, 0);
	}
}

void DrawEffect(short fx_number)
{
	FX_INFO* fx;
	OBJECT_INFO* obj;

	fx = &effects[fx_number];
	obj = &objects[fx->object_number];

	if (!obj->loaded || obj->draw_routine == DrawDummyItem)
		return;

	if (fx->object_number == GLOW)
		S_DrawSprite((fx->pos.y_rot << 16) | (ushort)fx->pos.x_rot, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, obj->mesh_index, fx->shade, fx->frame_number);
	else if (obj->nmeshes >= 0)
	{
		phd_PushMatrix();
		phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

		if (phd_mxptr[M23] > phd_znear && phd_mxptr[M23] < phd_zfar)
		{
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			S_CalculateLight(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->room_number, 0);

			if (obj->nmeshes)
				phd_PutPolygons(meshes[obj->mesh_index], -1);
			else
				phd_PutPolygons(meshes[fx->frame_number], -1);
		}

		phd_PopMatrix();
	}
	else
		S_DrawSprite(SPR_ABS | SPR_SHADE | (obj->semi_transparent ? SPR_SEMITRANS : 0),
			fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, obj->mesh_index - fx->frame_number, fx->shade, 0);
}

void PrintObjects(short room_number)
{
	ROOM_INFO* r;
	MESH_INFO* mesh;
	ITEM_INFO* item;
	long clip;
	short item_num, obj_num;

	CurrentRoom = room_number;
	r = &room[room_number];

	if (r->flags & ROOM_UNDERWATER)
		S_SetupBelowWater(camera_underwater);
	else
		S_SetupAboveWater(camera_underwater);

	r->bound_active = 0;
	phd_PushMatrix();
	phd_TranslateAbs(r->x, r->y, r->z);
	phd_left = r->left;
	phd_right = r->right;
	phd_top = r->top;
	phd_bottom = r->bottom;

	for (int i = 0; i < r->num_meshes; i++)
	{
		mesh = &r->mesh[i];
		CurrentMesh = mesh;
		nPolyType = 4;

		if (static_objects[mesh->static_number].flags & 2)
		{
			phd_PushMatrix();
			phd_TranslateAbs(mesh->x, mesh->y, mesh->z);
			phd_RotY(mesh->y_rot);
			clip = S_GetObjectBounds(&static_objects[mesh->static_number].x_minp);

			if (clip)
			{
				S_CalculateStaticMeshLight(mesh->x, mesh->y, mesh->z, mesh->shade, mesh->shadeB, r);

				if (bObjectOn)
					phd_PutPolygons(meshes[static_objects[mesh->static_number].mesh_number], clip);
			}

			phd_PopMatrix();
		}
	}

	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax + 1;
	phd_bottom = phd_winymax + 1;
	nPolyType = 5;

	for (item_num = r->item_number; item_num != NO_ITEM; item_num = item->next_item)
	{
		item = &items[item_num];

		if (item->status != ITEM_INVISIBLE)
		{
			if (bAObjectOn)
			{
				obj_num = item->object_number;

				if (objects[obj_num].intelligent)
					nPolyType = 5;
				else if ((obj_num < PLAYER_1 || obj_num > PLAYER_10) && (obj_num < VEHICLE_EXTRA || obj_num > QUADBIKE))
					nPolyType = 4;
				else
					nPolyType = 5;

				objects[obj_num].draw_routine(item);
			}

			if (item->after_death < 32 && item->after_death > 0)
			{
				item->after_death++;

				if (item->after_death == 2 || item->after_death == 5 || item->after_death == 11 || item->after_death == 20 ||
					item->after_death == 27 || item->after_death == 32 || !(GetRandomDraw() & 7))
					DoLotsOfBloodD(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos, 0, short(GetRandomDraw() << 1), item->room_number, 1);
			}
		}
	}

	nPolyType = 6;

	for (int i = r->fx_number; i != NO_ITEM; i = effects[i].next_fx)
		DrawEffect(i);

	phd_PopMatrix();
	r->left = phd_winxmax;
	r->top = phd_winymax;
	r->bottom = 0;
	r->right = 0;
}

static void PutPolyLara(ITEM_INFO* item, char mesh, long clip)
{
	if (item->mesh_bits & (1 << mesh))
	{
		if (IsJointUnderwater[mesh])
			S_SetupBelowWater(camera_underwater);
		else
			S_SetupAboveWater(camera_underwater);

		phd_PutPolygons(lara.mesh_ptrs[mesh], clip);
	}
}

static void PutPolyLara_I(ITEM_INFO* item, char mesh, long clip)
{
	if (item->mesh_bits & (1 << mesh))
	{
		if (IsJointUnderwater[mesh])
			S_SetupBelowWater(camera_underwater);
		else
			S_SetupAboveWater(camera_underwater);

		phd_PutPolygons_I(lara.mesh_ptrs[mesh], clip);
	}
}

void DrawGunFlash(long gun_type, long clip)
{
	long y, z;

	switch (gun_type)
	{
	case LG_MAGNUMS:
		y = 215;
		z = 65;
		break;

	case LG_UZIS:
		y = 150;
		z = 50;
		break;

	case LG_SHOTGUN:
	case LG_FLARE:
		return;

	case LG_M16:
		phd_TranslateRel(0, 332, 96);
		phd_RotYXZ(0, -15470, ((GetRandomDraw() << 1) & 0x4000) + (GetRandomDraw() & 0xFFF) + 6144);
		S_CalculateStaticLight(600);
		phd_PutPolygons(meshes[objects[M16_FLASH].mesh_index], clip);
		S_DrawSprite(SPR_SEMITRANS | SPR_SCALE | SPR_TINT | SPR_BLEND_ADD | SPR_RGB(63, 48, 8), 0, 0, -65, objects[GLOW].mesh_index, 0, 192);
		return;

	default:
		y = 150;
		z = 40;
		break;
	}

	phd_TranslateRel(0, y, z);
	phd_RotYXZ(0, -16380, short(wibble << 8));
	S_CalculateStaticLight(600);
	phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);
	S_DrawSprite(SPR_SEMITRANS | SPR_SCALE | SPR_TINT | SPR_BLEND_ADD | SPR_RGB(63, 56, 8), 0, 0, 0, objects[GLOW].mesh_index, 0, 192);
}

void CalculateObjectLighting(ITEM_INFO* item, short* frame)
{
	PHD_VECTOR pos;
	long x, y, z;
	short room_number;

	if (item->shade >= 0)
		S_CalculateStaticMeshLight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->shade & 0x7FFF, item->shadeB, &room[item->room_number]);
	else if (GnGameMode == GAMEMODE_IN_CUTSCENE)
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
		GetJointAbsPosition(item, &pos, 0);
		room_number = item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_number);
		S_CalculateLight(pos.x, pos.y, pos.z, room_number, &item->il);
	}
	else if (item->object_number == TRAIN)
		S_CalculateLight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, &item->il);
	else
	{
		phd_PushUnitMatrix();
		phd_SetTrans(0, 0, 0);
		phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
		x = (phd_mxptr[M03] >> W2V_SHIFT) + item->pos.x_pos;
		y = (phd_mxptr[M13] >> W2V_SHIFT) + item->pos.y_pos;
		z = (phd_mxptr[M23] >> W2V_SHIFT) + item->pos.z_pos;
		phd_PopMatrix();
		S_CalculateLight(x, y, z, item->room_number, &item->il);
	}
}

void CalculateObjectLightingLara()
{
	PHD_VECTOR pos;
	short room_number;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	if (GnGameMode == GAMEMODE_IN_CUTSCENE)
	{
		GetJointAbsPosition(lara_item, &pos, TORSO);
		room_number = lara_item->room_number;
	}
	else
	{
		GetLaraMeshPos(&pos, LMX_TORSO);
		room_number = lara_item->room_number;
		GetFloor(pos.x, pos.y, pos.z, &room_number);
	}

	S_CalculateLight(pos.x, pos.y, pos.z, room_number, &lara_item->il);
}

static void SwapLaraWithCamera(bool lr)
{
	static long x, y, z;
	static short rn;

	if (!lr)
	{
		x = lara_item->pos.x_pos;
		y = lara_item->pos.y_pos;
		z = lara_item->pos.z_pos;
		rn = lara_item->room_number;

		lara_item->pos.x_pos = camera.pos.x;
		lara_item->pos.y_pos = camera.pos.y;
		lara_item->pos.z_pos = camera.pos.z;
		lara_item->room_number = camera.pos.room_number;
	}
	else
	{
		lara_item->pos.x_pos = x;
		lara_item->pos.y_pos = y;
		lara_item->pos.z_pos = z;
		lara_item->room_number = rn;
	}
}

void DrawRooms(short current_room)
{
	ROOM_INFO* r;
	OBJECT_INFO* obj;
	short* rot;
	bool fx;

	fx = 0;
	CurrentRoom = current_room;
	r = &room[current_room];
	r->test_left = 0;
	r->test_top = 0;
	r->test_right = phd_winxmax;
	r->test_bottom = phd_winymax;
	phd_left = 0;
	phd_top = 0;
	phd_right = phd_winxmax;
	phd_bottom = phd_winymax;
	outside = r->flags & ROOM_OUTSIDE;
	camera_underwater = r->flags & ROOM_UNDERWATER;
	r->bound_active = 2;
	bound_list[0] = current_room;
	bound_start = 0;
	bound_end = 1;
	number_draw_rooms = 0;

	if (outside)
	{
		outside_top = 0;
		outside_left = 0;
		outside_right = phd_winxmax;
		outside_bottom = phd_winymax;
	}
	else
	{
		outside_left = phd_winxmax;
		outside_top = phd_winymax;
		outside_bottom = 0;
		outside_right = 0;
	}

	GetRoomBounds();
	mid_sort = 0;

	if (outside)
	{
		phd_top = outside_top;
		phd_left = outside_left;
		phd_right = outside_right;
		phd_bottom = outside_bottom;
		obj = &objects[HORIZON];

		if (obj->loaded)
		{
			S_SetupAboveWater(camera_underwater);
			phd_PushMatrix();
			phd_mxptr[M03] = 0;
			phd_mxptr[M13] = 0;
			phd_mxptr[M23] = 0;
			rot = anims[obj->anim_index].frame_ptr + 9;
			gar_RotYXZsuperpack(&rot, 0);
			S_InitialisePolyList(1);
			S_InsertBackground(meshes[obj->mesh_index]);
			phd_PopMatrix();
		}
		else
		{
			S_InitialisePolyList(1);
			outside = -1;
		}
	}
	else
		S_InitialisePolyList(1);

	obj = &objects[LARA];

	if (obj->loaded && !(lara_item->flags & IFL_INVISIBLE))
	{
		if (room[lara_item->room_number].flags & ROOM_UNDERWATER)
			S_SetupBelowWater(camera_underwater);
		else
			S_SetupAboveWater(camera_underwater);

		mid_sort = room[lara_item->room_number].bound_active >> 8;

		if (mid_sort)
			mid_sort--;

		nPolyType = 2;

		if (bLaraOn)
			NewDrawLara(lara_item);
	}

	if (bRoomOn)
	{
		nPolyType = 3;

		for (int i = 0; i < number_draw_rooms; i++)
			PrintRooms(draw_rooms[i]);
	}

	for (int i = 0; i < number_draw_rooms; i++)
		PrintObjects(draw_rooms[i]);

	if (bEffectOn)
	{
		SwapLaraWithCamera(0);

		nPolyType = 6;
		S_DrawSparks();
		S_DrawSplashes();
		S_DrawBat();

		if (tomb3.gold)
			fx = 0;
		else
			fx = GF_Snow != 0;

		if (fx)
			DoSnow();

		if (tomb3.gold)
			fx = CurrentLevel == LV_JUNGLE || CurrentLevel == LV_ROOFTOPS;
		else
			fx = GF_Rain != 0;

		if (fx)
			DoRain();

		S_DrawFootPrints();

		if (tomb3.gold)
			fx = 0;
		else
			fx = GF_WaterParts != 0;

		if (fx)
			DoUwEffect();

		SwapLaraWithCamera(1);
	}

	if (lara.electric)
	{
		if (lara.electric < 16)
			lara.electric++;

		UpdateElectricityPoints();
		LaraElectricDeath(0, lara_item);
		LaraElectricDeath(1, lara_item);
	}
}

long DrawPhaseGame()
{
	CalcLaraMatrices(0);
	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	CalcLaraMatrices(1);
	phd_PopMatrix();

	SetLaraUnderwaterNodes();

	DrawRooms(camera.pos.room_number);
	DrawGameInfo(1);
	S_OutputPolyList();
	camera.number_frames = S_DumpScreen();
	S_AnimateTextures(camera.number_frames);
	return camera.number_frames;
}

void DrawAnimatingItem(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	BITE_INFO* bite;
	PHD_VECTOR pos;
	GAME_VECTOR src;
	GAME_VECTOR dest;
	short** meshpp;
	long* bone;
	short* frmptr[2];
	short* extra_rotation;
	short* rot1;
	short* rot2;
	long frac, rate, clip, bit;
	short rnd;

	frac = GetFrames(item, frmptr, &rate);
	obj = &objects[item->object_number];

	if (obj->shadow_size)
		S_PrintShadow(obj->shadow_size, frmptr[0], item);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frmptr[0]);

	if (!clip)
	{
		if (item->clear_body && item->hit_points <= 0 && item->after_death >= 32)
			KillItem(item - items);

		phd_PopMatrix();
		return;
	}

	CalculateObjectLighting(item, frmptr[0]);

	if (item->data)
		extra_rotation = (short*)item->data;
	else
		extra_rotation = null_rotations;

	meshpp = &meshes[obj->mesh_index];
	bone = &bones[obj->bone_index];
	bit = 1;

	if (frac)
	{
		InitInterpolate(frac, rate);
		phd_TranslateRel_ID(frmptr[0][6], frmptr[0][7], frmptr[0][8], frmptr[1][6], frmptr[1][7], frmptr[1][8]);
		rot1 = frmptr[0] + 9;
		rot2 = frmptr[1] + 9;
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);

		if (item->mesh_bits & bit)
			phd_PutPolygons_I(*meshpp, clip);

		meshpp++;

		for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp++)
		{
			if (bone[0] & 1)
				phd_PopMatrix_I();

			if (bone[0] & 2)
				phd_PushMatrix_I();

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot1, &rot2, 0);

			if (bone[0] & 0x1C)
			{
				if (bone[0] & 8)
					phd_RotY_I(*extra_rotation++);

				if (bone[0] & 4)
					phd_RotX_I(*extra_rotation++);

				if (bone[0] & 16)
					phd_RotZ_I(*extra_rotation++);
			}

			bit <<= 1;

			if (item->mesh_bits & bit)
				phd_PutPolygons_I(*meshpp, clip);

			if (item->fired_weapon && i == EnemyBites[obj->bite_offset].mesh_num - 1)
			{
				bite = &EnemyBites[obj->bite_offset];
				rnd = (short)GetRandomDraw();
				phd_PushMatrix_I();
				phd_TranslateRel_I(bite->x, bite->y, bite->z);
				phd_RotYXZ_I(0, -16380, (rnd << 14) + (rnd >> 2) - 4096);
				InterpolateMatrix();
				S_DrawSprite(SPR_SEMITRANS | SPR_BLEND_ADD | SPR_TINT | SPR_SCALE | SPR_RGB(63, 56, 8), 0, 0, 0, objects[GLOW].mesh_index, 0, 192);
				S_CalculateStaticLight(600);

				if (EnemyWeapon[obj->bite_offset] & 1)
					phd_PutPolygons(meshes[objects[M16_FLASH].mesh_index], clip);
				else
					phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);

				phd_PopMatrix_I();
				item->fired_weapon--;
			}

			if (i == (EnemyBites[obj->bite_offset].mesh_num - 1) && EnemyWeapon[obj->bite_offset] & 0x80)
			{
				if (item->hit_points > 0 || item->frame_number != anims[item->anim_number].frame_end)
				{
					bite = &EnemyBites[obj->bite_offset + 1];
					pos.x = bite->x;
					pos.y = bite->y;
					pos.z = bite->z;
					GetJointAbsPosition(item, &pos, bite->mesh_num);
					src.x = pos.x;
					src.y = pos.y;
					src.z = pos.z;
					src.room_number = item->room_number;

					pos.x = bite->x;
					pos.y = bite->y << 5;
					pos.z = bite->z;
					GetJointAbsPosition(item, &pos, bite->mesh_num);
					dest.x = pos.x;
					dest.y = pos.y;
					dest.z = pos.z;

					LOS(&src, &dest);
					S_DrawLaserBeam(&src, &dest, 255, 2, 3);
				}
			}
		}
	}
	else
	{
		phd_TranslateRel(frmptr[0][6], frmptr[0][7], frmptr[0][8]);
		rot1 = frmptr[0] + 9;
		gar_RotYXZsuperpack(&rot1, 0);

		if (item->mesh_bits & bit)
			phd_PutPolygons(*meshpp, clip);

		meshpp++;

		for (int i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp++)
		{
			if (bone[0] & 1)
				phd_PopMatrix();

			if (bone[0] & 2)
				phd_PushMatrix();

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot1, 0);

			if (bone[0] & 0x1C)
			{
				if (bone[0] & 8)
					phd_RotY(*extra_rotation++);

				if (bone[0] & 4)
					phd_RotX(*extra_rotation++);

				if (bone[0] & 0x10)
					phd_RotZ(*extra_rotation++);
			}

			bit <<= 1;

			if (item->mesh_bits & bit)
				phd_PutPolygons(*meshpp, clip);

			if (item->fired_weapon && i == EnemyBites[obj->bite_offset].mesh_num - 1)
			{
				bite = &EnemyBites[obj->bite_offset];
				rnd = (short)GetRandomDraw();
				phd_PushMatrix();
				phd_TranslateRel(bite->x, bite->y, bite->z);

				if (item->object_number == ROBOT_SENTRY_GUN)
					phd_RotYXZ(0, 32760, (rnd << 14) + (rnd >> 2) - 4096);
				else
					phd_RotYXZ(0, -16380, (rnd << 14) + (rnd >> 2) - 4096);

				S_DrawSprite(SPR_SEMITRANS | SPR_BLEND_ADD | SPR_TINT | SPR_SCALE | SPR_RGB(63, 56, 8), 0, 0, 0, objects[GLOW].mesh_index, 0, 192);
				S_CalculateStaticLight(600);

				if (EnemyWeapon[obj->bite_offset] & 1)
					phd_PutPolygons(meshes[objects[M16_FLASH].mesh_index], clip);
				else
					phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);

				phd_PopMatrix();
				item->fired_weapon--;
			}

			if (i == (EnemyBites[obj->bite_offset].mesh_num - 1) && EnemyWeapon[obj->bite_offset] & 0x80)
			{
				bite = &EnemyBites[obj->bite_offset + 1];
				pos.x = bite->x;
				pos.y = bite->y;
				pos.z = bite->z;
				GetJointAbsPosition(item, &pos, bite->mesh_num);
				src.x = pos.x;
				src.y = pos.y;
				src.z = pos.z;
				src.room_number = item->room_number;

				pos.x = bite->x;
				pos.y = bite->y << 5;
				pos.z = bite->z;
				GetJointAbsPosition(item, &pos, bite->mesh_num);
				dest.x = pos.x;
				dest.y = pos.y;
				dest.z = pos.z;

				LOS(&src, &dest);
				S_DrawLaserBeam(&src, &dest, 255, 2, 3);
			}
		}
	}

	phd_PopMatrix();
}

void DrawDummyItem(ITEM_INFO* item)
{

}
