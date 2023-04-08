#include "../tomb3/pch.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "draw.h"
#include "objects.h"
#include "control.h"

void InitInterpolate2(long frac, long rate)
{
	IM_rate = rate;
	IM_frac = frac;
	IMptr = &IMstack[384];
	memcpy(&IMstack[384], phd_mxptr, 48);
}

long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* meshp;
	short* frame;
	short* rot;
	short* extra_rot;
	long x, y, z, poppush;

	if (!item)
		return 0;

	if (WorldSpace)
	{
		x = item->pos.x_pos;
		y = item->pos.y_pos;
		z = item->pos.z_pos;
		phd_PushUnitMatrix();
		phd_SetTrans(0, 0, 0);
	}
	else
	{
		z = 0;
		y = 0;
		x = 0;
		phd_PushMatrix();
		phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	}

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	frame = GetBestFrame(item);
	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);
	obj = &objects[item->object_number];
	meshpp = &meshes[obj->mesh_index];
	meshp = *meshpp++;
	bone = &bones[obj->bone_index];

	phd_PushMatrix();
	phd_TranslateRel(meshp[0], meshp[1], meshp[2]);
	ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
	ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
	ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
	ptr->r = meshp[3];
	ptr++;
	phd_PopMatrix();

	extra_rot = (short*)item->data;

	for (int i = 0; i < obj->nmeshes - 1; i++, bone += 3)
	{
		poppush = *bone++;

		if (poppush & 1)
			phd_PopMatrix();

		if (poppush & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[0], bone[1], bone[2]);
		gar_RotYXZsuperpack(&rot, 0);

		if (poppush & 0x1C && extra_rot)
		{
			if (poppush & 8)
				phd_RotY(*extra_rot++);

			if (poppush & 4)
				phd_RotX(*extra_rot++);

			if (poppush & 0x10)
				phd_RotZ(*extra_rot++);
		}

		meshp = *meshpp++;
		phd_PushMatrix();
		phd_TranslateRel(meshp[0], meshp[1], meshp[2]);
		ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
		ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
		ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
		ptr->r = meshp[3];
		ptr++;
		phd_PopMatrix();
	}

	phd_PopMatrix();
	return obj->nmeshes;
}

long TestCollision(ITEM_INFO* item, ITEM_INFO* l)
{
	SPHERE* itemSpheres;
	SPHERE* laraSpheres;
	SPHERE slist_baddie[34];
	SPHERE slist_lara[34];
	PHD_VECTOR ip;
	PHD_VECTOR lp;
	ulong touch_bits;
	long nItemSpheres, nLaraSpheres, ir, lr;

	touch_bits = 0;
	nItemSpheres = GetSpheres(item, slist_baddie, 1);
	nLaraSpheres = GetSpheres(l, slist_lara, 1);

	for (int i = 0; i < nItemSpheres; i++)
	{
		itemSpheres = &slist_baddie[i];
		ir = itemSpheres->r;

		if (ir > 0)
		{
			ip.x = itemSpheres->x;
			ip.y = itemSpheres->y;
			ip.z = itemSpheres->z;

			for (int j = 0; j < nLaraSpheres; j++)
			{
				laraSpheres = &slist_lara[j];
				lr = laraSpheres->r;

				if (lr > 0)
				{
					lp.x = laraSpheres->x - ip.x;
					lp.y = laraSpheres->y - ip.y;
					lp.z = laraSpheres->z - ip.z;
					lr += ir;

					if (SQUARE(lp.x) + SQUARE(lp.y) + SQUARE(lp.z) < SQUARE(lr))
					{
						touch_bits |= 1 << i;
						break;
					}
				}
			}
		}
	}

	item->touch_bits = touch_bits;
	return touch_bits;
}

void GetJointAbsPosition(ITEM_INFO* item, PHD_VECTOR* pos, long joint)
{
	OBJECT_INFO* obj;
	long* mx;
	long* imx;
	long* bone;
	short* frm[2];
	short* extra_rotation;
	short* rot;
	short* rot2;
	long frac, rate, poppush;

	mx = phd_mxptr;
	imx = IMptr;
	obj = &objects[item->object_number];
	frac = GetFrames(item, frm, &rate);

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	extra_rotation = (short*)item->data;

	if (!extra_rotation)
		extra_rotation = null_rotations;

	bone = &bones[obj->bone_index];

	if (frac)
	{
		InitInterpolate2(frac, rate);
		rot = frm[0] + 9;
		rot2 = frm[1] + 9;
		phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		for (int i = 0; i < joint; i++)
		{
			poppush = *bone;

			if (poppush & 1)
				phd_PopMatrix_I();

			if (poppush & 2)
				phd_PushMatrix_I();

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (poppush & 0x1C)
			{
				if (poppush & 8)
					phd_RotY_I(*extra_rotation++);

				if (poppush & 4)
					phd_RotX_I(*extra_rotation++);

				if (poppush & 0x10)
					phd_RotZ_I(*extra_rotation++);
			}

			bone += 4;
		}

		phd_TranslateRel_I(pos->x, pos->y, pos->z);
		InterpolateMatrix();
	}
	else
	{
		phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
		rot = frm[0] + 9;
		gar_RotYXZsuperpack(&rot, 0);

		for (int i = 0; i < joint; i++)
		{
			poppush = *bone;

			if (poppush & 1)
				phd_PopMatrix();

			if (poppush & 2)
				phd_PushMatrix();

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot, 0);

			if (poppush & 0x1C)
			{
				if (poppush & 8)
					phd_RotY(*extra_rotation++);

				if (poppush & 4)
					phd_RotX(*extra_rotation++);

				if (poppush & 0x10)
					phd_RotZ(*extra_rotation++);
			}

			bone += 4;
		}

		phd_TranslateRel(pos->x, pos->y, pos->z);
	}

	pos->x = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
	pos->y = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
	pos->z = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
	phd_mxptr = mx;
	IMptr = imx;
}
