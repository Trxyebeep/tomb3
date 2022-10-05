#include "../tomb3/pch.h"
#include "inventry.h"
#include "objects.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/scalespr.h"
#include "../specific/frontend.h"
#include "../specific/output.h"
#include "draw.h"
#include "invfunc.h"

long GetDebouncedInput(long in)
{
	static long db;
	long i;

	i = ~db;
	db = in;
	return i & in;
}

void SelectMeshes(INVENTORY_ITEM* item)
{
	if (item->object_number == PASSPORT_OPTION)
	{
		if (item->current_frame < 4)
			item->drawn_meshes = 23;
		else if (item->current_frame <= 16)
			item->drawn_meshes = 87;
		else if (item->current_frame < 19)
			item->drawn_meshes = 95;
		else if (item->current_frame == 19)
			item->drawn_meshes = 91;
		else if (item->current_frame < 24)
			item->drawn_meshes = 123;
		else if (item->current_frame < 29)
			item->drawn_meshes = 59;
		else if (item->current_frame == 29)
			item->drawn_meshes = 19;
	}
	else if (item->object_number == MAP_OPTION)
	{
		if (!item->current_frame || item->current_frame >= 18)
			item->drawn_meshes = item->which_meshes;
		else
			item->drawn_meshes = -1;
	}
	else if (item->object_number != GAMMA_OPTION)
		item->drawn_meshes = -1;
}

long AnimateInventoryItem(INVENTORY_ITEM* item)
{
	if (item->current_frame == item->goal_frame)
	{
		SelectMeshes(item);
		return 0;
	}

	if (item->anim_count)
		item->anim_count--;
	else
	{
		item->anim_count = item->anim_speed;
		item->current_frame += item->anim_direction;

		if (item->current_frame >= item->frames_total)
			item->current_frame = 0;
		else if (item->current_frame < 0)
			item->current_frame = item->frames_total - 1;
	}

	SelectMeshes(item);
	return 1;
}

void DrawInventoryItem(INVENTORY_ITEM* item)
{
	INVENTORY_SPRITE** slist;
	INVENTORY_SPRITE* sp;
	OBJECT_INFO* obj;
	long* bone;
	short* frame;
	short* rot;
	long m, h, s, x, y, z, clip;
	short mesh, bit;

	nPolyType = 7;

	if (bInvItemsOff)
		return;

	if (item->object_number == MAP_OPTION)
	{
		s = savegame.timer / 30;
		m = -91 * (s % 3600) / 5;
		h = -91 * (s / 12) / 5;
		s = -1092 * (s % 60);
	}
	else
	{
		m = 0;
		h = 0;
		s = 0;
	}

	phd_TranslateRel(0, item->ytrans, item->ztrans);
	phd_RotYXZ(item->y_rot, item->x_rot, 0);

	if (item->object_number == GAMMA_OPTION)
		phd_RotYXZ(GlobeYRot, GlobeXRot, GlobeZRot);

	obj = &objects[item->object_number];

	if (!obj->loaded)
		return;

	if (obj->nmeshes < 0)
	{
		S_DrawSprite(0, 0, 0, 0, obj->mesh_index, 0, 0);
		return;
	}

	slist = item->sprlist;

	if (slist)
	{
		z = phd_mxptr[M23];
		x = phd_centerx + phd_mxptr[M03] / (z / phd_persp);
		y = phd_centery + phd_mxptr[M13] / (z / phd_persp);

		while (sp = *slist++)
		{
			if (z < phd_znear || z > phd_zfar)
				break;

			while (sp->shape)
			{
				switch (sp->shape)
				{
				case 1:
					S_DrawScreenSprite(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, static_objects[ALPHABET].mesh_number + sp->sprnum, 4096, 0);
					break;

				case 2:
					S_DrawScreenLine(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, sp->sprnum, sp->grdptr, 0);
					break;

				case 3:
					S_DrawScreenBox(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, sp->sprnum, sp->grdptr, 0);
					break;

				case 4:
					S_DrawScreenFBox(x + sp->x, y + sp->y, sp->z, sp->param1, sp->param2, sp->sprnum, sp->grdptr, 0);
					break;
				}

				s++;
			}
		}
	}

	frame = &obj->frame_base[item->current_frame * (anims[obj->anim_index].interpolation >> 8)];

	phd_PushMatrix();

	clip = S_GetObjectBounds(frame);

	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	phd_TranslateRel(frame[6], frame[7], frame[8]);
	rot = frame + 9;
	gar_RotYXZsuperpack(&rot, 0);
	bone = &bones[obj->bone_index];
	mesh = obj->mesh_index;
	bit = 1;

	if (item->object_number == GAMMA_OPTION)
		item->drawn_meshes = ~8;

	if (item->drawn_meshes & bit)
	{
		if (item->object_number == GAMMA_OPTION)
			Inv_GlobeLight(bit);

		phd_PutPolygons(meshes[mesh], clip);
	}

	for (int i = obj->nmeshes - 1; i > 0; i--, bone += 4)
	{
		mesh++;
		bit <<= 1;

		if (bone[0] & 1)
			phd_PopMatrix();

		if (bone[0] & 2)
			phd_PushMatrix();

		phd_TranslateRel(bone[1], bone[2], bone[3]);
		gar_RotYXZsuperpack(&rot, 0);

		if (item->object_number == MAP_OPTION)
		{
			if (i == 1)
			{
				phd_RotZ((short)s);
				item->misc_data[1] = item->misc_data[0];
				item->misc_data[0] = s;
			}

			if (i == 2)
				phd_RotZ((short)m);

			if (i == 3)
				phd_RotZ((short)h);
		}

		if (item->object_number == GAMMA_OPTION)
			Inv_GlobeLight(bit);

		if (item->drawn_meshes & bit)
			phd_PutPolygons(meshes[mesh], clip);
	}

	phd_PopMatrix();
}

void inject_inventry(bool replace)
{
	INJECT(0x00436FA0, GetDebouncedInput, replace);
	INJECT(0x004369C0, SelectMeshes, replace);
	INJECT(0x00436A50, AnimateInventoryItem, replace);
	INJECT(0x00436AE0, DrawInventoryItem, replace);
}
