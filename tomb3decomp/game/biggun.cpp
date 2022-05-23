#include "../tomb3/pch.h"
#include "biggun.h"
#include "../specific/init.h"
#include "laraflar.h"
#include "laraanim.h"
#include "objects.h"
#include "collide.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/output.h"

void BigGunInitialise(short item_number)
{
	ITEM_INFO* item;
	BIGGUNINFO* gun;

	item = &items[item_number];
	gun = (BIGGUNINFO*)game_malloc(sizeof(BIGGUNINFO), 0);
	item->data = gun;
	gun->FireCount = 0;
	gun->Flags = 0;
	gun->RotX = 30;
	gun->RotY = 0;
	gun->StartRotY = item->pos.y_rot;
}

static long CanUseGun(ITEM_INFO* gun, ITEM_INFO* l)
{
	long dx, dz, dist;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || l->gravity_status)
		return 0;

	dx = l->pos.x_pos - gun->pos.x_pos;
	dz = l->pos.z_pos - gun->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 30000)
		return 0;

	return 1;
}

void BigGunCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	BIGGUNINFO* gun;

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	item = &items[item_number];

	if (CanUseGun(item, l))
	{
		lara.skidoo = item_number;

		if (lara.gun_type == LG_FLARE)
		{
			CreateFlare(0);
			undraw_flare_meshes();
			lara.flare_control_left = 0;
			lara.gun_type = LG_UNARMED;
			lara.request_gun_type = LG_UNARMED;
		}

		lara.gun_status = LG_HANDSBUSY;
		l->pos = item->pos;
		l->anim_number = objects[VEHICLE_ANIM].anim_index;
		l->frame_number = anims[objects[BIGGUN].anim_index].frame_base;
		l->current_anim_state = 0;
		l->goal_anim_state = 0;
		gun = (BIGGUNINFO*)item->data;
		gun->Flags = 0;
		gun->RotX = 30;
	}
	else
		ObjectCollision(item_number, l, coll);
}

void BigGunDraw(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short** meshpp;
	long* bone;
	short* frm[2];
	short* rot1;
	short* rot2;
	long frac, rate, clip;

	frac = GetFrames(item, frm, &rate);
	obj = &objects[item->object_number];

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	clip = S_GetObjectBounds(frm[0]);

	if (clip)
	{
		CalculateObjectLighting(item, frm[0]);
		meshpp = &meshes[obj->mesh_index];
		bone = &bones[obj->bone_index];

		if (frac)
		{
			InitInterpolate(frac, rate);
			rot1 = frm[0] + 9;
			rot2 = frm[1] + 9;
			phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);

			gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
			phd_PutPolygons_I(*meshpp++, clip);

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
			phd_PutPolygons_I(*meshpp++, clip);

			phd_TranslateRel_I(bone[5], bone[6], bone[7]);
			gar_RotYXZsuperpack_I(&rot1, &rot2, 0);
			phd_PutPolygons_I(*meshpp++, clip);
		}
		else
		{
			phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
			rot1 = frm[0] + 9;

			gar_RotYXZsuperpack(&rot1, 0);
			phd_PutPolygons(*meshpp++, clip);

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot1, 0);
			phd_PutPolygons(*meshpp++, clip);

			phd_TranslateRel(bone[5], bone[6], bone[7]);
			gar_RotYXZsuperpack(&rot1, 0);
			phd_PutPolygons(*meshpp++, clip);
		}
	}

	phd_PopMatrix();
}

void inject_biggun(bool replace)
{
	INJECT(0x00410D00, BigGunInitialise, replace);
	INJECT(0x00410E60, CanUseGun, replace);
	INJECT(0x00410D50, BigGunCollision, replace);
	INJECT(0x00410EC0, BigGunDraw, replace);
}
