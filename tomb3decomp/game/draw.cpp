#include "../tomb3/pch.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"
#include "objects.h"
#include "../3dsystem/scalespr.h"
#include "../specific/litesrc.h"
#include "../specific/function_stubs.h"
#include "effects.h"
#include "hair.h"
#include "lara.h"
#include "effect2.h"
#include "laraanim.h"

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

	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotY(angle);
	phd_mxptr = mx;
}

void phd_RotX_I(short angle)
{
	long* mx;

	mx = phd_mxptr;
	phd_mxptr = IMptr;
	phd_RotX(angle);
	phd_mxptr = mx;
}

void phd_RotZ_I(short angle)
{
	long* mx;

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
		phd_mxptr[M00] = (phd_mxptr[M00] + IMptr[M00]) >> 1;
		phd_mxptr[M01] = (phd_mxptr[M01] + IMptr[M01]) >> 1;
		phd_mxptr[M02] = (phd_mxptr[M02] + IMptr[M02]) >> 1;
		phd_mxptr[M03] = (phd_mxptr[M03] + IMptr[M03]) >> 1;
		phd_mxptr[M10] = (phd_mxptr[M10] + IMptr[M10]) >> 1;
		phd_mxptr[M11] = (phd_mxptr[M11] + IMptr[M11]) >> 1;
		phd_mxptr[M12] = (phd_mxptr[M12] + IMptr[M12]) >> 1;
		phd_mxptr[M13] = (phd_mxptr[M13] + IMptr[M13]) >> 1;
		phd_mxptr[M20] = (phd_mxptr[M20] + IMptr[M20]) >> 1;
		phd_mxptr[M21] = (phd_mxptr[M21] + IMptr[M21]) >> 1;
		phd_mxptr[M22] = (phd_mxptr[M22] + IMptr[M22]) >> 1;
		phd_mxptr[M23] = (phd_mxptr[M23] + IMptr[M23]) >> 1;
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

	frac = GetFrames(item, frmptr, &rate);

	if (!frac)
		return frmptr[0];

	bptr = interpolated_bounds;

	for (int i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
		*bptr = short(*frmptr[0] + (frac + (*frmptr[1] - *frmptr[0]) / rate));

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
				x = xv[l1] + ((z * ((xv[l2] - xv[l1]) >> 14) / div) << 14);
				y = yv[l1] + ((z * ((yv[l2] - yv[l1]) >> 14) / div) << 14);

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
			S_InsertBackPolygon(xmin, (long)outsideBackgroundTop, xmax, ymax, 0x800000);
		else
			S_InsertBackPolygon(xmin, ymin, xmax, ymax, 0x800000);
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
	S_LightRoom(r);

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

	if (!obj->loaded || obj->draw_routine == dummyDrawFunc)
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
		S_DrawSprite(obj->semi_transparent ? 0xB000000 : 0x9000000, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, obj->mesh_index - fx->frame_number, fx->shade, 0);
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
		if (IsJointUnderwater[0])
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
		if (IsJointUnderwater[0])
			S_SetupBelowWater(camera_underwater);
		else
			S_SetupAboveWater(camera_underwater);

		phd_PutPolygons_I(lara.mesh_ptrs[mesh], clip);
	}
}

void DrawLaraInt(ITEM_INFO* item, short* frame1, short* frame2, long frac, long rate)
{
	OBJECT_INFO* obj;
	PHD_VECTOR pos;
	long* bone;
	long* bgBone;
	short* rot1;
	short* rot2;
	short* rot12;
	short* rot22;
	long mx[12];
	long clip, flare_on, x, y, z;
	short gun_type;

	obj = &objects[item->object_number];
	flare_on = 0;

	if (lara.skidoo == NO_ITEM)
		S_PrintShadow(obj->shadow_size, GetBoundsAccurate(item), item);

	memcpy(mx, phd_mxptr, sizeof(mx));

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frame1);

	if (!clip)
	{
		if (GnGameMode == 3)
		{
			phd_PopMatrix();
			return;
		}

		clip = -1;
	}

	phd_PushMatrix();
	CalculateObjectLightingLara();
	bone = &bones[obj->bone_index];
	rot1 = frame1 + 9;
	rot2 = frame2 + 9;

	InitInterpolate(frac, rate);
	phd_TranslateRel_ID(frame1[6], frame1[7], frame1[8], frame2[6], frame2[7], frame2[8]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, HIPS, clip);

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, THIGH_L, clip);

	phd_TranslateRel_I(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, CALF_L, clip);

	phd_TranslateRel_I(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, FOOT_L, clip);
	phd_PopMatrix_I();

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, THIGH_R, clip);

	phd_TranslateRel_I(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, CALF_R, clip);

	phd_TranslateRel_I(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
	PutPolyLara_I(item, FOOT_R, clip);
	phd_PopMatrix_I();

	phd_TranslateRel_I(bone[25], bone[26], bone[27]);

	if (lara.weapon_item != NO_ITEM && lara.gun_type == LG_M16 &&
		(!items[lara.weapon_item].current_anim_state || items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 4))
	{
		rot1 = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot2 = rot1;
		gar_RotYXZsuperpack_I(&rot1, &rot2, 7);
	}
	else
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);

	phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	PutPolyLara_I(item, TORSO, clip);

	phd_PushMatrix_I();
	phd_TranslateRel_I(bone[53], bone[54], bone[55]);
	rot12 = rot1;
	rot22 = rot2;
	gar_RotYXZsuperpack_I(&rot12, &rot22, 6);
	phd_RotYXZ_I(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	PutPolyLara_I(item, HEAD, clip);

	memcpy(phd_mxptr, mx, sizeof(mx));
	DrawHair();
	phd_PopMatrix_I();

	if (lara.back_gun)
	{
		bgBone = &bones[objects[lara.back_gun].bone_index];
		phd_PushMatrix_I();
		phd_TranslateRel_I(bgBone[53], bgBone[54], bgBone[55]);
		rot12 = objects[lara.back_gun].frame_base + 9;
		rot22 = rot12;
		gar_RotYXZsuperpack_I(&rot12, &rot22, 14);
		phd_PutPolygons_I(meshes[objects[lara.back_gun].mesh_index + HEAD], clip);
		phd_PopMatrix_I();
	}

	gun_type = LG_UNARMED;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_SPECIAL || lara.gun_status == LG_DRAW || lara.gun_status == LG_UNDRAW)
		gun_type = lara.gun_type;

	switch (gun_type)
	{
	case LG_UNARMED:
	case LG_FLARE:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, UARM_R, clip);

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, LARM_R, clip);

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, HAND_R, clip);
		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot1 = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			rot2 = rot1;
			gar_RotYXZsuperpack_I(&rot1, &rot2, 11);
		}
		else
			gar_RotYXZsuperpack_I(&rot1, &rot2, 0);

		PutPolyLara_I(item, UARM_L, clip);

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, LARM_L, clip);

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, HAND_L, clip);

		if (lara.gun_type == LG_FLARE && lara.left_arm.flash_gun)
		{
			phd_TranslateRel_I(11, 32, 80);
			phd_RotX_I(-16380);
			flare_on = 1;
		}

		break;

	case LG_PISTOLS:
	case LG_UZIS:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		InterpolateArmMatrix();
		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot1 = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot1, 8);
		PutPolyLara(item, UARM_R, clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, LARM_R, clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, HAND_R, clip);

		if (lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		InterpolateArmMatrix();
		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot1 = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot1, 11);
		PutPolyLara(item, UARM_L, clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, LARM_L, clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, HAND_L, clip);

		if (lara.left_arm.flash_gun)
			DrawGunFlash(gun_type, clip);

		if (lara.right_arm.flash_gun)
		{
			memcpy(phd_mxptr, mx, sizeof(mx));
			DrawGunFlash(gun_type, clip);
		}

		break;

	case LG_MAGNUMS:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		InterpolateArmMatrix();
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot1 = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot1, 8);
		PutPolyLara(item, UARM_R, clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, LARM_R, clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, HAND_R, clip);

		if (lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		InterpolateArmMatrix();
		phd_RotYXZ_I(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot1 = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot1, 11);
		PutPolyLara(item, UARM_L, clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, LARM_L, clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot1, 0);
		PutPolyLara(item, HAND_L, clip);

		if (lara.left_arm.flash_gun)
			DrawGunFlash(gun_type, clip);

		if (lara.right_arm.flash_gun)
		{
			memcpy(phd_mxptr, mx, sizeof(mx));
			DrawGunFlash(gun_type, clip);
		}

		break;

	case LG_SHOTGUN:
	case LG_M16:
	case LG_ROCKET:
	case LG_GRENADE:
	case LG_HARPOON:
		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		rot2 = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		rot1 = rot2;
		gar_RotYXZsuperpack_I(&rot1, &rot2, 8);
		PutPolyLara_I(item, UARM_R, clip);

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, LARM_R, clip);

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, HAND_R, clip);

		if (lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix_I();

		phd_PushMatrix_I();
		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, UARM_L, clip);

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, LARM_L, clip);

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
		PutPolyLara_I(item, HAND_L, clip);

		if (lara.right_arm.flash_gun)
		{
			memcpy(phd_mxptr, mx, sizeof(mx));
			DrawGunFlash(gun_type, clip);
		}

		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();

	if (flare_on)
	{
		pos.x = 8;
		pos.y = 36;
		pos.z = 32;
		GetLaraHandAbsPosition(&pos, LEFT_HAND);
		x = pos.x;
		y = pos.y;
		z = pos.z;
		pos.x = 8;
		pos.y = 36;
		pos.z = (GetRandomDraw() & 0xFF) + 1024;
		GetLaraHandAbsPosition(&pos, LEFT_HAND);

		for (int i = 0; i < (GetRandomDraw() & 3) + 4; i++)
			TriggerFlareSparks(x, y, z, pos.x - x, pos.y - y, pos.z - z, i >> 2, 0);
	}
}

void DrawLara(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	PHD_VECTOR pos;
	long* bone;
	long* bgBone;
	short* frm[2];
	short* frame;
	short* rot;
	short* rot2;
	long mx[12];
	long l, r, t, b, frac, rate, clip, flare_on, x, y, z;
	short spaz, gun_type;

	l = phd_left;
	r = phd_right;
	t = phd_top;
	b = phd_bottom;
	flare_on = 0;
	phd_top = 0;
	phd_left = 0;
	phd_bottom = phd_winymax;
	phd_right = phd_winxmax;

	if (!GotJointPos[FOOT_L])
		GetLaraHandAbsPosition(&pos, LEFT_FOOT);

	if (!GotJointPos[FOOT_R])
		GetLaraHandAbsPosition(&pos, RIGHT_FOOT);

	if (!GotJointPos[HAND_L])
		GetLaraHandAbsPosition(&pos, LEFT_HAND);

	if (!GotJointPos[HAND_R])
		GetLaraHandAbsPosition(&pos, RIGHT_HAND);

	if (!GotJointPos[HEAD])
		GetLaraHandAbsPosition(&pos, LARA_HEAD);

	frac = GetFrames(item, frm, &rate);

	if (lara.hit_direction < 0 && frac)
	{
		DrawLaraInt(item, frm[0], frm[1], frac, rate);
		phd_left = l;
		phd_right = r;
		phd_top = t;
		phd_bottom = b;
		return;
	}

	obj = &objects[item->object_number];

	if (lara.hit_direction < 0)
		frame = frm[0];
	else
	{
		switch (lara.hit_direction)
		{
		case NORTH:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKF;
			else
				spaz = ANIM_SPAZ_FORWARD;

			break;

		case EAST:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKR;
			else
				spaz = ANIM_SPAZ_RIGHT;

			break;

		case SOUTH:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKB;
			else
				spaz = ANIM_SPAZ_BACK;

			break;

		default:

			if (lara.IsDucked)
				spaz = ANIM_SPAZ_DUCKL;
			else
				spaz = ANIM_SPAZ_LEFT;

			break;
		}

		frame = &anims[spaz].frame_ptr[lara.hit_frame * (anims[spaz].interpolation >> 8)];
	}

	if (lara.skidoo == NO_ITEM || lara.extra_anim && item->current_anim_state == 12)
		S_PrintShadow(obj->shadow_size, frame, item);

	memcpy(mx, phd_mxptr, sizeof(mx));
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frame);

	if (!clip)
	{
		if (GnGameMode == 3)
		{
			phd_PopMatrix();
			return;
		}

		clip = -1;
	}

	phd_PushMatrix();
	CalculateObjectLightingLara();
	bone = &bones[obj->bone_index];
	rot = frame + 9;
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, HIPS, clip);

	phd_PushMatrix();
	phd_TranslateRel(bone[1], bone[2], bone[3]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, THIGH_L, clip);

	phd_TranslateRel(bone[5], bone[6], bone[7]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, CALF_L, clip);

	phd_TranslateRel(bone[9], bone[10], bone[11]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, FOOT_L, clip);
	phd_PopMatrix();

	phd_PushMatrix();
	phd_TranslateRel(bone[13], bone[14], bone[15]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, THIGH_R, clip);

	phd_TranslateRel(bone[17], bone[18], bone[19]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, CALF_R, clip);

	phd_TranslateRel(bone[21], bone[22], bone[23]);
	gar_RotYXZsuperpack(&rot, 0);
	PutPolyLara(item, FOOT_R, clip);
	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);

	if (lara.weapon_item != NO_ITEM && lara.gun_type == LG_M16 &&
		(!items[lara.weapon_item].current_anim_state || items[lara.weapon_item].current_anim_state == 2 || items[lara.weapon_item].current_anim_state == 4))
	{
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 7);
	}
	else
		gar_RotYXZsuperpack(&rot, 0);

	phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
	PutPolyLara(item, TORSO, clip);

	phd_PushMatrix();
	phd_TranslateRel(bone[53], bone[54], bone[55]);
	rot2 = rot;
	gar_RotYXZsuperpack(&rot2, 6);
	phd_RotYXZ(lara.head_y_rot, lara.head_x_rot, lara.head_z_rot);
	PutPolyLara(item, HEAD, clip);

	memcpy(phd_mxptr, mx, sizeof(mx));
	DrawHair();
	phd_PopMatrix();

	if (lara.back_gun)
	{
		bgBone = &bones[objects[lara.back_gun].bone_index];
		phd_PushMatrix();
		phd_TranslateRel(bgBone[53], bgBone[54], bgBone[55]);
		rot2 = objects[lara.back_gun].frame_base + 9;
		gar_RotYXZsuperpack(&rot2, 14);
		phd_PutPolygons(meshes[objects[lara.back_gun].mesh_index + 14], clip);
		phd_PopMatrix();
	}

	gun_type = LG_UNARMED;

	if (lara.gun_status == LG_READY || lara.gun_status == LG_SPECIAL || lara.gun_status == LG_DRAW || lara.gun_status == LG_UNDRAW)
		gun_type = lara.gun_type;

	switch (gun_type)
	{
	case LG_UNARMED:
	case LG_FLARE:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, UARM_R, clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_R, clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_R, clip);

		if (GnGameMode == 3 && lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);

		if (lara.flare_control_left)
		{
			rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
				(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
			gar_RotYXZsuperpack(&rot, 11);
		}
		else
			gar_RotYXZsuperpack(&rot, 0);

		PutPolyLara(item, UARM_L, clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_L, clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_L, clip);

		if (lara.gun_type == LG_FLARE && lara.left_arm.flash_gun)
		{
			flare_on = 1;
			DrawGunFlash(LG_FLARE, clip);
		}

		if (GnGameMode == 3)
		{
			if (lara.left_arm.flash_gun)
				DrawGunFlash(LG_PISTOLS, clip);

			if (lara.right_arm.flash_gun)
			{
				memcpy(phd_mxptr, mx, sizeof(mx));
				DrawGunFlash(LG_PISTOLS, clip);
			}
		}

		break;

	case LG_PISTOLS:
	case LG_UZIS:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];
		phd_RotYXZ(lara.right_arm.y_rot, lara.right_arm.x_rot, lara.right_arm.z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		PutPolyLara(item, UARM_R, clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_R, clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_R, clip);

		if (lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];
		phd_RotYXZ(lara.left_arm.y_rot, lara.left_arm.x_rot, lara.left_arm.z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		PutPolyLara(item, UARM_L, clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_L, clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_L, clip);

		if (lara.left_arm.flash_gun)
			DrawGunFlash(gun_type, clip);

		if (lara.right_arm.flash_gun)
		{
			memcpy(phd_mxptr, mx, sizeof(mx));
			DrawGunFlash(gun_type, clip);
		}

		break;

	case LG_MAGNUMS:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];
		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.right_arm.frame_base[(anims[lara.right_arm.anim_number].interpolation >> 8) *
			(lara.right_arm.frame_number - anims[lara.right_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		PutPolyLara(item, UARM_R, clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_R, clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_R, clip);

		if (lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		phd_mxptr[M00] = phd_mxptr[-2 * indices_count + M00];
		phd_mxptr[M01] = phd_mxptr[-2 * indices_count + M01];
		phd_mxptr[M02] = phd_mxptr[-2 * indices_count + M02];
		phd_mxptr[M10] = phd_mxptr[-2 * indices_count + M10];
		phd_mxptr[M11] = phd_mxptr[-2 * indices_count + M11];
		phd_mxptr[M12] = phd_mxptr[-2 * indices_count + M12];
		phd_mxptr[M20] = phd_mxptr[-2 * indices_count + M20];
		phd_mxptr[M21] = phd_mxptr[-2 * indices_count + M21];
		phd_mxptr[M22] = phd_mxptr[-2 * indices_count + M22];
		phd_RotYXZ(lara.torso_y_rot, lara.torso_x_rot, lara.torso_z_rot);
		rot = &lara.left_arm.frame_base[(anims[lara.left_arm.anim_number].interpolation >> 8) *
			(lara.left_arm.frame_number - anims[lara.left_arm.anim_number].frame_base) + 9];
		gar_RotYXZsuperpack(&rot, 11);
		PutPolyLara(item, UARM_L, clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_L, clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_L, clip);

		if (lara.left_arm.flash_gun)
			DrawGunFlash(LG_MAGNUMS, clip);

		if (lara.right_arm.flash_gun)
		{
			memcpy(phd_mxptr, mx, sizeof(mx));
			DrawGunFlash(LG_MAGNUMS, clip);
		}

		break;

	case LG_SHOTGUN:
	case LG_M16:
	case LG_ROCKET:
	case LG_GRENADE:
	case LG_HARPOON:
		phd_PushMatrix();
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		rot = &lara.right_arm.frame_base[lara.right_arm.frame_number * (anims[lara.right_arm.anim_number].interpolation >> 8) + 9];
		gar_RotYXZsuperpack(&rot, 8);
		PutPolyLara(item, UARM_R, clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_R, clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_R, clip);

		if (lara.right_arm.flash_gun)
			memcpy(mx, phd_mxptr, sizeof(mx));

		phd_PopMatrix();

		phd_PushMatrix();
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, UARM_L, clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, LARM_L, clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		gar_RotYXZsuperpack(&rot, 0);
		PutPolyLara(item, HAND_L, clip);

		if (lara.right_arm.flash_gun)
		{
			memcpy(phd_mxptr, mx, sizeof(mx));
			DrawGunFlash(gun_type, clip);
		}

		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	phd_PopMatrix();
	phd_left = l;
	phd_right = r;
	phd_top = t;
	phd_bottom = b;

	if (flare_on)
	{
		pos.x = 8;
		pos.y = 36;
		pos.z = 32;
		GetLaraHandAbsPosition(&pos, 0);
		x = pos.x;
		y = pos.y;
		z = pos.z;
		pos.x = 8;
		pos.y = 36;
		pos.z = (GetRandomDraw() & 0xFF) + 1024;
		GetLaraHandAbsPosition(&pos, 0);

		for (int i = 0; i < (GetRandomDraw() & 3) + 4; i++)
			TriggerFlareSparks(x, y, z, pos.x - x, pos.y - y, pos.z - z, i >> 2, 0);
	}
}

void inject_draw(bool replace)
{
	INJECT(0x00429390, phd_PopMatrix_I, replace);
	INJECT(0x004293C0, phd_PushMatrix_I, replace);
	INJECT(0x004293F0, phd_RotY_I, replace);
	INJECT(0x00429430, phd_RotX_I, replace);
	INJECT(0x00429470, phd_RotZ_I, replace);
	INJECT(0x004294B0, phd_TranslateRel_I, replace);
	INJECT(0x00429500, phd_TranslateRel_ID, replace);
	INJECT(0x00429550, phd_RotYXZ_I, replace);
	INJECT(0x00429690, phd_PutPolygons_I, replace);
	INJECT(0x004295E0, gar_RotYXZsuperpack, replace);
	INJECT(0x004295A0, gar_RotYXZsuperpack_I, replace);
	INJECT(0x00429350, InitInterpolate, replace);
	INJECT(0x004296C0, InterpolateMatrix, replace);
	INJECT(0x00429930, InterpolateArmMatrix, replace);
	INJECT(0x00429DB0, GetFrames, replace);
	INJECT(0x00429ED0, GetBestFrame, replace);
	INJECT(0x00429E50, GetBoundsAccurate, replace);
	INJECT(0x00425590, SetRoomBounds, replace);
	INJECT(0x004253C0, GetRoomBounds, replace);
	INJECT(0x00425910, ClipRoom, replace);
	INJECT(0x00424FE0, PrintRooms, replace);
	INJECT(0x00425D10, DrawEffect, replace);
	INJECT(0x004250A0, PrintObjects, replace);
	INJECT(0x00427E20, DrawLaraInt, replace);
	INJECT(0x004265E0, DrawLara, replace);
}
