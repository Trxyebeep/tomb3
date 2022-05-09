#include "../tomb3/pch.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"

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
}
