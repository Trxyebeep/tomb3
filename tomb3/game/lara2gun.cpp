#include "../tomb3/pch.h"
#include "lara2gun.h"
#include "objects.h"
#include "larafire.h"
#include "sound.h"
#include "lara.h"
#include "effect2.h"
#include "../specific/game.h"
#include "../specific/input.h"
#include "control.h"
#include "camera.h"
#include "../newstuff/LaraDraw.h"

static PISTOL_DEF PistolTable[4]
{
	{ 0, 0, 0, 0 },
	{ PISTOLS, 4, 5, 13, 24 },
	{ MAGNUM, 7, 8, 15, 29 },
	{ UZI, 4, 5, 13, 24 }
};

void draw_pistol_meshes(long weapon_type)
{
	long obj;

	obj = WeaponObject(weapon_type);
	lara.mesh_ptrs[HAND_R] = meshes[objects[obj].mesh_index + HAND_R];
	lara.mesh_ptrs[THIGH_R] = meshes[objects[LARA].mesh_index + THIGH_R];

	if (weapon_type != LG_MAGNUMS)
	{
		lara.mesh_ptrs[HAND_L] = meshes[objects[obj].mesh_index + HAND_L];
		lara.mesh_ptrs[THIGH_L] = meshes[objects[LARA].mesh_index + THIGH_L];
	}
}

void undraw_pistol_mesh_right(long weapon_type)
{
	lara.mesh_ptrs[THIGH_R] = meshes[objects[WeaponObject(weapon_type)].mesh_index + THIGH_R];
	lara.mesh_ptrs[HAND_R] = meshes[objects[LARA].mesh_index + HAND_R];
}

void undraw_pistol_mesh_left(long weapon_type)
{
	if (weapon_type != LG_MAGNUMS)
	{
		lara.mesh_ptrs[THIGH_L] = meshes[objects[WeaponObject(weapon_type)].mesh_index + THIGH_L];
		lara.mesh_ptrs[HAND_L] = meshes[objects[LARA].mesh_index + HAND_L];
	}
}

void ready_pistols(long weapon_type)
{
	lara.gun_status = LG_READY;
	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.frame_number = 0;
	lara.left_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
	lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
}

void set_arm_info(LARA_ARM* arm, int frame)
{
	PISTOL_DEF* p;
	short anim_base;

	p = &PistolTable[lara.gun_type];
	anim_base = objects[p->ObjectNum].anim_index;

	if (frame < p->Draw1Anim)
		arm->anim_number = anim_base;
	else if (frame < p->Draw2Anim)
		arm->anim_number = anim_base + 1;
	else if (frame < p->RecoilAnim)
		arm->anim_number = anim_base + 2;
	else
		arm->anim_number = anim_base + 3;

	arm->frame_number = frame;
	arm->frame_base = anims[arm->anim_number].frame_ptr;
}

void draw_pistols(long weapon_type)
{
	PISTOL_DEF* p;
	short ani;

	ani = lara.left_arm.frame_number + 1;
	p = &PistolTable[lara.gun_type];

	if (ani < p->Draw1Anim || ani > p->RecoilAnim - 1)
		ani = p->Draw1Anim;
	else if (ani == p->Draw2Anim)
	{
		draw_pistol_meshes(weapon_type);
		SoundEffect(SFX_LARA_DRAW, &lara_item->pos, SFX_DEFAULT);
	}
	else if (ani == p->RecoilAnim - 1)
	{
		ready_pistols(weapon_type);
		ani = 0;
	}

	set_arm_info(&lara.right_arm, ani);
	set_arm_info(&lara.left_arm, ani);
}

void undraw_pistols(long weapon_type)
{
	PISTOL_DEF* p;
	short anil, anir;

	p = &PistolTable[lara.gun_type];
	anil = lara.left_arm.frame_number;

	if (lara.left_arm.frame_number >= p->RecoilAnim)
		anil = p->Draw1Anim2;
	else if (lara.left_arm.frame_number > 0 && lara.left_arm.frame_number < p->Draw1Anim)
	{
		lara.left_arm.x_rot -= lara.left_arm.x_rot / anil;
		lara.left_arm.y_rot -= lara.left_arm.y_rot / anil;
		anil--;
	}
	else if (!lara.left_arm.frame_number)
	{
		lara.left_arm.x_rot = 0;
		lara.left_arm.y_rot = 0;
		lara.left_arm.z_rot = 0;
		anil = p->RecoilAnim - 1;
	}
	else if (lara.left_arm.frame_number > p->Draw1Anim)
	{
		anil--;

		if (anil == p->Draw2Anim - 1)
		{
			undraw_pistol_mesh_left(weapon_type);
			SoundEffect(SFX_LARA_HOLSTER, &lara_item->pos, SFX_DEFAULT);
		}
	}

	set_arm_info(&lara.left_arm, anil);
	anir = lara.right_arm.frame_number;

	if (lara.right_arm.frame_number >= p->RecoilAnim)
		anir = p->Draw1Anim2;
	else if (lara.right_arm.frame_number > 0 && lara.right_arm.frame_number < p->Draw1Anim)
	{
		lara.right_arm.x_rot -= lara.right_arm.x_rot / anir;
		lara.right_arm.y_rot -= lara.right_arm.y_rot / anir;
		anir--;
	}
	else if (!lara.right_arm.frame_number)
	{
		lara.right_arm.x_rot = 0;
		lara.right_arm.y_rot = 0;
		lara.right_arm.z_rot = 0;
		anir = p->RecoilAnim - 1;
	}
	else if (lara.right_arm.frame_number > p->Draw1Anim)
	{
		anir--;

		if (anir == p->Draw2Anim - 1)
		{
			undraw_pistol_mesh_right(weapon_type);
			SoundEffect(SFX_LARA_HOLSTER, &lara_item->pos, SFX_DEFAULT);
		}
	}

	set_arm_info(&lara.right_arm, anir);

	if (anil == p->Draw1Anim && anir == p->Draw1Anim)
	{
		lara.gun_status = LG_ARMLESS;
		lara.left_arm.frame_number = 0;
		lara.right_arm.frame_number = 0;
		lara.target = 0;
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
	}

	if (!(input & IN_LOOK))
	{
		lara.head_x_rot = (lara.left_arm.x_rot + lara.right_arm.x_rot) >> 2;
		lara.head_y_rot = (lara.left_arm.y_rot + lara.right_arm.y_rot) >> 2;
		lara.torso_x_rot = lara.head_x_rot;
		lara.torso_y_rot = lara.head_y_rot;
	}
}

void AnimatePistols(long weapon_type)
{
	WEAPON_INFO* winfo;
	PISTOL_DEF* p;
	PHD_VECTOR pos;
	static long uzi_right = 0, uzi_left = 0;
	short sound_already, anir, anil;
	short angles[2];

	sound_already = 0;

	if (SmokeCountL)
	{
		switch (SmokeWeapon)
		{
		case LG_PISTOLS:
			pos.x = 4;
			pos.y = 128;
			pos.z = 40;
			break;

		case LG_MAGNUMS:
			pos.x = 16;
			pos.y = 160;
			pos.z = 56;
			break;

		case LG_UZIS:
			pos.x = 8;
			pos.y = 140;
			pos.z = 48;
			break;
		}

		GetLaraMeshPos(&pos, LMX_HAND_L);
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
	}

	if (SmokeCountR)
	{
		switch (SmokeWeapon)
		{
		case LG_PISTOLS:
			pos.x = -16;
			pos.y = 128;
			pos.z = 40;
			break;

		case LG_MAGNUMS:
			pos.x = -32;
			pos.y = 160;
			pos.z = 56;
			break;

		case LG_UZIS:
			pos.x = -16;
			pos.y = 140;
			pos.z = 48;
			break;
		}

		GetLaraMeshPos(&pos, LMX_HAND_R);
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
	}

	winfo = &weapons[weapon_type];
	p = &PistolTable[lara.gun_type];
	anir = lara.right_arm.frame_number;

	if (lara.right_arm.lock || input & IN_ACTION && !lara.target)
	{
		if (anir >= 0 && anir < p->Draw1Anim2)
			anir++;
		else if (anir == p->Draw1Anim2)
		{
			if (input & IN_ACTION)
			{
				if (weapon_type != LG_MAGNUMS)
				{
					angles[0] = lara.right_arm.y_rot + lara_item->pos.y_rot;
					angles[1] = lara.right_arm.x_rot;

					if (FireWeapon(weapon_type, lara.target, lara_item, angles))
					{
						SmokeCountR = 28;
						SmokeWeapon = weapon_type;
						TriggerGunShell(1, GUNSHELL, weapon_type);
						lara.right_arm.flash_gun = weapons[weapon_type].flash_time;
						SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | SFX_SETPITCH);
						SoundEffect(weapons[weapon_type].sample_num, &lara_item->pos, SFX_DEFAULT);
						sound_already = 1;

						if (weapon_type == LG_UZIS)
							uzi_right = 1;
					}
				}

				anir = p->RecoilAnim;
			}
			else if (uzi_right)
			{
				SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
				uzi_right = 0;
			}
		}
		else if (anir >= p->RecoilAnim)
		{
			if (weapon_type == LG_UZIS)
			{
				SoundEffect(weapons[LG_UZIS].sample_num, &lara_item->pos, SFX_DEFAULT);
				uzi_right = 1;
			}

			anir++;

			if (anir == p->RecoilAnim + winfo->recoil_frame)
				anir = p->Draw1Anim2;
		}
	}
	else
	{
		if (anir >= p->RecoilAnim)
			anir = p->Draw1Anim2;
		else if (anir > 0 && anir <= p->Draw1Anim2)
			anir--;

		if (uzi_right)
		{
			SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
			uzi_right = 0;
		}
	}

	set_arm_info(&lara.right_arm, anir);
	anil = lara.left_arm.frame_number;

	if (lara.left_arm.lock || input & IN_ACTION && !lara.target)
	{
		if (anil >= 0 && anil < p->Draw1Anim2)
			anil++;
		else if (anil == p->Draw1Anim2)
		{
			if (input & IN_ACTION)
			{
				angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
				angles[1] = lara.left_arm.x_rot;

				if (FireWeapon(weapon_type, lara.target, lara_item, angles))
				{
					if (weapon_type == LG_MAGNUMS)
					{
						SmokeCountR = 28;
						SmokeWeapon = 2;
						TriggerGunShell(1, GUNSHELL, 2);
						lara.right_arm.flash_gun = winfo->flash_time;
					}
					else
					{
						SmokeCountL = 28;
						SmokeWeapon = weapon_type;
						TriggerGunShell(0, GUNSHELL, weapon_type);
						lara.left_arm.flash_gun = winfo->flash_time;
					}

					if (!sound_already)
					{
						SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | SFX_SETPITCH);
						SoundEffect(winfo->sample_num, &lara_item->pos, SFX_DEFAULT);
					}

					if (weapon_type == LG_UZIS)
						uzi_left = 1;
				}

				anil = p->RecoilAnim;
			}
			else if (uzi_left)
			{
				SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
				uzi_left = 0;
			}
		}
		else if (anil >= p->RecoilAnim)
		{
			if (weapon_type == LG_UZIS)
			{
				SoundEffect(winfo->sample_num, &lara_item->pos, SFX_DEFAULT);
				uzi_left = 1;
			}

			anil++;

			if (anil == (p->RecoilAnim + winfo->recoil_frame))
				anil = p->Draw1Anim2;
		}
	}
	else
	{
		if (anil >= p->RecoilAnim)
			anil = p->Draw1Anim2;
		else if (anil > 0 && anil <= p->Draw1Anim2)
			anil--;

		if (uzi_left)
		{
			SoundEffect(winfo->sample_num + 1, &lara_item->pos, SFX_DEFAULT);
			uzi_left = 0;
		}
	}

	set_arm_info(&lara.left_arm, anil);
}

void PistolHandler(long weapon_type)
{
	WEAPON_INFO* winfo;
	PHD_VECTOR pos;
	long r, g, b;

	winfo = &weapons[weapon_type];
	
	if (input & IN_ACTION)
		LaraTargetInfo(winfo);
	else
		lara.target = 0;

	if (!lara.target)
		LaraGetNewTarget(winfo);

	AimWeapon(winfo, &lara.left_arm);
	AimWeapon(winfo, &lara.right_arm);

	if (lara.left_arm.lock && lara.right_arm.lock)
	{
		lara.torso_x_rot = (lara.left_arm.x_rot + lara.right_arm.x_rot) >> 2;
		lara.torso_y_rot = (lara.left_arm.y_rot + lara.right_arm.y_rot) >> 2;

		if (camera.old_type != LOOK_CAMERA)
		{
			lara.head_x_rot = lara.torso_x_rot;
			lara.head_y_rot = lara.torso_y_rot;
		}
	}
	else if (lara.left_arm.lock && !lara.right_arm.lock)
	{
		lara.torso_x_rot = lara.left_arm.x_rot >> 1;
		lara.torso_y_rot = lara.left_arm.y_rot >> 1;

		if (camera.old_type != LOOK_CAMERA)
		{
			lara.head_x_rot = lara.torso_x_rot;
			lara.head_y_rot = lara.torso_y_rot;
		}
	}
	else if (!lara.left_arm.lock && lara.right_arm.lock)
	{
		lara.torso_x_rot = lara.right_arm.x_rot >> 1;
		lara.torso_y_rot = lara.right_arm.y_rot >> 1;

		if (camera.old_type != LOOK_CAMERA)
		{
			lara.head_x_rot = lara.torso_x_rot;
			lara.head_y_rot = lara.torso_y_rot;
		}
	}

	AnimatePistols(weapon_type);

	if (lara.left_arm.flash_gun || lara.right_arm.flash_gun)
	{
		pos.x = (GetRandomControl() & 0xFF) - 128;
		pos.y = (GetRandomControl() & 0x7F) - 63;
		pos.z = (GetRandomControl() & 0xFF) - 128;

		if (lara.left_arm.flash_gun)
			GetLaraMeshPos(&pos, LMX_HAND_L);
		else
			GetLaraMeshPos(&pos, LMX_HAND_R);

		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 128;
		b = GetRandomControl() & 0x3F;
		TriggerDynamic(pos.x, pos.y, pos.z, 10, r, g, b);
	}
}
