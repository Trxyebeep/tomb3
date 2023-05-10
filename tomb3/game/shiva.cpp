#include "../tomb3/pch.h"
#include "shiva.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "box.h"
#include "effects.h"
#include "sound.h"
#include "lara.h"
#include "sphere.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "draw.h"
#include "../specific/output.h"
#include "../3dsystem/3d_gen.h"

static BITE_INFO shiva_right = { 0, 0, 920, 22 };
static BITE_INFO shiva_left = { 0, 0, 920, 13 };
static long effect_mesh;

static void TriggerShivaSmoke(long x, long y, long z, long yt)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (yt)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 208;
	}
	else
	{
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 64;
	sptr->Life = (GetRandomControl() & 0x1F) + 96;
	sptr->sLife = sptr->Life;

	if (yt)
		sptr->TransType = 2;
	else
		sptr->TransType = 3;

	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (yt)
	{
		sptr->Friction = 20;
		sptr->y += 32;
		sptr->Yvel >>= 4;
	}
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;

	if (yt)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else
	{
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}

	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width >> 2;
	
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 3;
	sptr->sHeight = sptr->Height;
}

static void ShivaDamage(ITEM_INFO* item, CREATURE_INFO* shiva, long damage)
{
	if (!shiva->flags && item->touch_bits & 0x2400000)
	{
		lara_item->hit_points -= (short)damage;
		lara_item->hit_status = 1;
		CreatureEffect(item, &shiva_right, DoBloodSplat);
		shiva->flags = 1;
		SoundEffect(SFX_MACAQUE_ROLL, &item->pos, 0);
	}

	if (!shiva->flags && item->touch_bits & 0x2400)
	{
		lara_item->hit_points -= (short)damage;
		lara_item->hit_status = 1;
		CreatureEffect(item, &shiva_left, DoBloodSplat);
		shiva->flags = 1;
		SoundEffect(SFX_MACAQUE_ROLL, &item->pos, 0);
	}
}

void InitialiseShiva(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number == SHIVA)
	{
		item->anim_number = objects[SHIVA].anim_index + 14;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = SHIVA_START;
		item->goal_anim_state = SHIVA_START;
	}

	item->status = ITEM_INACTIVE;
	item->mesh_bits = 0;
}

void ShivaControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* shiva;
	FLOOR_INFO* floor;
	AI_INFO info;
	PHD_VECTOR pos;
	long x, y, z;
	short lara_alive, torso_x, torso_y, head_y, angle, frame, base, rn;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	shiva = (CREATURE_INFO*)item->data;
	lara_alive = lara_item->hit_points > 0;
	torso_x = 0;
	torso_y = 0;
	head_y = 0;
	angle = 0;
	
	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != SHIVA_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + 22;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = SHIVA_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (shiva->mood == ESCAPE_MOOD)
		{
			shiva->target.x = lara_item->pos.x_pos;
			shiva->target.z = lara_item->pos.z_pos;
		}

		angle = CreatureTurn(item, shiva->maximum_turn);

		if (item->current_anim_state != SHIVA_START)
			item->mesh_bits = -1;

		switch (item->current_anim_state)
		{
		case SHIVA_WAIT:

			if (info.ahead)
				head_y = info.angle;

			if (shiva->flags < 0)
			{
				shiva->flags++;
				x = (GetRandomControl() & 0x5FF) + item->pos.x_pos - 768;
				y = item->pos.y_pos - (GetRandomControl() & 0x5FF);
				z = (GetRandomControl() & 0x5FF) + item->pos.z_pos - 768;
				TriggerShivaSmoke(x, y, z, 1);
			}
			else
			{
				if (shiva->flags == 1)
					shiva->flags = 0;

				shiva->maximum_turn = 0;

				if (shiva->mood == ESCAPE_MOOD)
				{
					rn = item->room_number;
					x = item->pos.x_pos + (WALL_SIZE * phd_sin(item->pos.y_rot + 0x8000) >> W2V_SHIFT);
					y = item->pos.y_pos;
					z = item->pos.z_pos + (WALL_SIZE * phd_cos(item->pos.y_rot + 0x8000) >> W2V_SHIFT);
					floor = GetFloor(x, y, z, &rn);

					if (shiva->flags || floor->box == 0x7FF || boxes[floor->box].overlap_index & 0x8000)
						item->goal_anim_state = SHIVA_WAIT_DEF;
					else
						item->goal_anim_state = SHIVA_WALKBACK;
				}
				else if (shiva->mood == BORED_MOOD)
				{
					if (GetRandomControl() < 1024)
						item->goal_anim_state = SHIVA_WALK;
				}
				else if (info.bite && info.distance < 0x190000)
				{
					item->goal_anim_state = SHIVA_PINCER;
					shiva->flags = 0;
				}
				else if (info.bite && info.distance < 0x1C6E39)
				{
					item->goal_anim_state = SHIVA_CHOPPER;
					shiva->flags = 0;
				}
				else if (item->hit_status && info.ahead)
				{
					item->goal_anim_state = SHIVA_WAIT_DEF;
					shiva->flags = 4;
				}
				else
					item->goal_anim_state = SHIVA_WALK;
			}

			break;

		case SHIVA_WALK:

			if (info.ahead)
				head_y = info.angle;

			shiva->maximum_turn = 728;

			if (shiva->mood == ESCAPE_MOOD)
				item->goal_anim_state = SHIVA_WAIT;
			else if (shiva->mood == BORED_MOOD)
				item->goal_anim_state = SHIVA_WAIT;
			else if (info.bite && info.distance < 0x1C6E39)
			{
				item->goal_anim_state = SHIVA_WAIT;
				shiva->flags = 0;
			}
			else if (item->hit_status)
			{
				item->goal_anim_state = SHIVA_WALK_DEF;
				shiva->flags = 4;
			}

			break;

		case SHIVA_WAIT_DEF:

			if (info.ahead)
				head_y = info.angle;

			shiva->maximum_turn = 0;

			if (item->hit_status || shiva->mood == ESCAPE_MOOD)
				shiva->flags = 4;

			if ((info.bite && info.distance < 0x1C6E39) || (item->frame_number == anims[item->anim_number].frame_base && !shiva->flags) || !info.ahead)
			{
				item->goal_anim_state = SHIVA_WAIT;
				shiva->flags = 0;
			}
			else if (shiva->flags)
				item->goal_anim_state = SHIVA_WAIT_DEF;

			if (item->frame_number == anims[item->anim_number].frame_base && shiva->flags > 1)
				shiva->flags -= 2;

			break;

		case SHIVA_WALK_DEF:

			if (info.ahead)
				head_y = info.angle;

			shiva->maximum_turn = 728;

			if (item->hit_status)
				shiva->flags = 4;

			if ((info.bite && info.distance < 0x190000) || (item->frame_number == anims[item->anim_number].frame_base && !shiva->flags))
			{
				item->goal_anim_state = SHIVA_WALK;
				shiva->flags = 0;
			}
			else if (shiva->flags)
				item->goal_anim_state = SHIVA_WALK_DEF;

			if (item->frame_number == anims[item->anim_number].frame_base)
				shiva->flags = 0;

			break;

		case SHIVA_START:
			shiva->maximum_turn = 0;

			if (shiva->flags)
				shiva->flags--;
			else
			{
				if (!item->mesh_bits)
					effect_mesh = 0;

				item->mesh_bits = (item->mesh_bits << 1) + 1;
				shiva->flags = 1;

				pos.x = 0;
				pos.y = 0;
				pos.z = 256;
				GetJointAbsPosition(item, &pos, effect_mesh++);
				TriggerExplosionSparks(pos.x, pos.y, pos.z, 2, 0, 0, item->room_number);
				TriggerShivaSmoke(pos.x, pos.y, pos.z, 1);
			}

			if (item->mesh_bits == 0x7FFFFFFF)
			{
				item->goal_anim_state = SHIVA_WAIT;
				effect_mesh = 0;
				shiva->flags = -45;
			}

			break;

		case SHIVA_PINCER:

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.x_angle;
				head_y = info.angle;
			}

			shiva->maximum_turn = 728;
			ShivaDamage(item, shiva, 150);

			break;

		case SHIVA_KILL:
			shiva->maximum_turn = 0;
			head_y = 0;
			torso_x = 0;
			torso_y = 0;
			frame = item->frame_number;
			base = anims[item->anim_number].frame_base;

			if (frame == base + 10 || frame == base + 21 || frame == base + 33)
			{
				CreatureEffect(item, &shiva_right, DoBloodSplat);
				CreatureEffect(item, &shiva_left, DoBloodSplat);
			}

			break;

		case SHIVA_CHOPPER:

			head_y = info.angle;
			torso_y = info.angle;

			if (info.x_angle > 0)
				torso_x = info.x_angle;

			shiva->maximum_turn = 728;
			ShivaDamage(item, shiva, 180);

			break;

		case SHIVA_WALKBACK:

			if (info.ahead)
				head_y = info.angle;

			shiva->maximum_turn = 728;

			if ((info.ahead && info.distance < 0x1C6E39) || (item->frame_number == anims[item->anim_number].frame_base && !shiva->flags))
				item->goal_anim_state = SHIVA_WAIT;
			else if (item->hit_status)
			{
				shiva->flags = 4;
				item->goal_anim_state = SHIVA_WAIT;
			}

			break;
		}
	}

	if (lara_alive && lara_item->hit_points <= 0)
	{
		CreatureKill(item, 18, SHIVA_KILL, EXTRA_YETIKILL);
		return;
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head_y - torso_y);
	CreatureJoint(item, 3, 0);
	CreatureAnimation(item_number, angle, 0);
}

void DrawShiva(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	PHD_VECTOR pos;
	short** mesh;
	short** swap;
	long* bone;
	short* frm[2];
	short* extra_rotation;
	short* rot;
	short* rot2;
	long frac, rate, clip, bit, lp;

	frac = GetFrames(item, frm, &rate);

	if (item->hit_points <= 0 && item->status != ITEM_ACTIVE && item->mesh_bits)
	{
		item->mesh_bits >>= 1;
		pos.x = 0;
		pos.y = 0;
		pos.z = 256;
		GetJointAbsPosition(item, &pos, effect_mesh++);
		TriggerShivaSmoke(pos.x, pos.y, pos.z, 1);
	}

	obj = &objects[item->object_number];

	if (obj->shadow_size)
		S_PrintShadow(obj->shadow_size, frm[0], item);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	clip = S_GetObjectBounds(frm[0]);

	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	CalculateObjectLighting(item, frm[0]);
	extra_rotation = (short*)item->data;
	bit = 1;

	mesh = &meshes[objects[SHIVA].mesh_index];
	swap = &meshes[objects[MESHSWAP1].mesh_index];
	bone = &bones[obj->bone_index];

	if (frac)
	{
		InitInterpolate(frac, rate);
		phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
		rot = frm[0] + 9;
		rot2 = frm[1] + 9;
		gar_RotYXZsuperpack_I(&rot, &rot2, 0);

		if (item->mesh_bits & bit)
			phd_PutPolygons_I(*mesh, clip);
		else
			phd_PutPolygons_I(*swap, clip);

		mesh++;
		swap++;

		for (lp = obj->nmeshes - 1; lp > 0; lp--)
		{
			if (bone[0] & 1)
				phd_PopMatrix_I();

			if (bone[0] & 2)
				phd_PushMatrix_I();

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack_I(&rot, &rot2, 0);

			if (extra_rotation && bone[0] & 0x1C)
			{
				if (bone[0] & 8)
					phd_RotY_I(*extra_rotation++);

				if (bone[0] & 4)
					phd_RotX_I(*extra_rotation++);

				if (bone[0] & 0x10)
					phd_RotZ_I(*extra_rotation++);
			}

			bit <<= 1;

			if (item->mesh_bits & bit)
				phd_PutPolygons_I(*mesh, clip);
			else if (item->hit_points > 0 || item->status == ITEM_ACTIVE || bit != 0x400 || item->carried_item == NO_ITEM)
				phd_PutPolygons_I(*swap, clip);

			bone += 4;
			mesh++;
			swap++;
		}
	}
	else
	{
		phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
		rot = frm[0] + 9;
		gar_RotYXZsuperpack(&rot, 0);

		if (item->mesh_bits & bit)
			phd_PutPolygons(*mesh, clip);
		else
			phd_PutPolygons(*swap, clip);

		mesh++;
		swap++;

		for (lp = obj->nmeshes - 1; lp > 0; lp--)
		{
			if (bone[0] & 1)
				phd_PopMatrix();

			if (bone[0] & 2)
				phd_PushMatrix();

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			gar_RotYXZsuperpack(&rot, 0);

			if (extra_rotation && bone[0] & 0x1C)
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
				phd_PutPolygons(*mesh, clip);
			else
				phd_PutPolygons(*swap, clip);

			bone += 4;
			mesh++;
			swap++;
		}
	}

	phd_PopMatrix();
}
