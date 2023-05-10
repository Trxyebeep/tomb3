#include "../tomb3/pch.h"
#include "monkey.h"
#include "box.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "effects.h"
#include "../specific/game.h"
#include "items.h"
#include "draw.h"
#include "../specific/output.h"
#include "../3dsystem/3d_gen.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

static BITE_INFO monkey_hit = { 10, 10, 11, 13 };

void InitialiseMonkey(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[MONKEY].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = MONKEY_SIT;
	item->goal_anim_state = MONKEY_SIT;
}

void MonkeyControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	ITEM_INFO* pickup;
	CREATURE_INFO* monkey;
	CREATURE_INFO* slot;
	AI_INFO info;
	long dx, dz, iDist, lp;
	short angle, tilt, torso_y;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	monkey = (CREATURE_INFO*)item->data;
	angle = 0;
	tilt = 0;
	torso_y = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != MONKEY_DEATH)
		{
			item->mesh_bits = -1;
			item->anim_number = objects[MONKEY].anim_index + 14;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_DEATH;
		}
	}
	else
	{
		GetAITarget(monkey);

		if (monkey->hurt_by_lara)
			monkey->enemy = lara_item;

		if (item->ai_bits == MODIFY)
		{
			if (item->carried_item == NO_ITEM)
				item->mesh_bits = 0xFFFF6F6F;
			else
				item->mesh_bits = 0xFFFF6E6F;
		}
		else if (item->carried_item == NO_ITEM)
			item->mesh_bits = -1;
		else
			item->mesh_bits = 0xFFFFFEFF;

		CreatureAIInfo(item, &info);

		if (!monkey->hurt_by_lara && monkey->enemy == lara_item && objects[TIGER].loaded)
			monkey->enemy = 0;

		if (monkey->enemy == lara_item)
			iDist = info.distance;
		else
		{
			dx = lara_item->pos.x_pos - item->pos.x_pos;
			dz = lara_item->pos.z_pos - item->pos.z_pos;
			phd_atan(dz, dx);
			iDist = SQUARE(dx) * SQUARE(dz);
		}

		GetCreatureMood(item, &info, 1);

		if (lara.skidoo != NO_ITEM)
			monkey->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, monkey->maximum_turn);

		enemy = monkey->enemy;
		monkey->enemy = lara_item;

		if (item->hit_status)
			AlertAllGuards(item_number);

		monkey->enemy = enemy;

		switch (item->current_anim_state)
		{
		case MONKEY_WALK:
			monkey->maximum_turn = 1274;

			if (item->ai_bits & PATROL1)
				item->goal_anim_state = MONKEY_WALK;
			else if (monkey->mood == ESCAPE_MOOD)
				item->goal_anim_state = MONKEY_RUN;
			else if (monkey->mood == BORED_MOOD)
			{
				if (GetRandomControl() < 256)
					item->goal_anim_state = MONKEY_SIT;
			}
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = MONKEY_STAND;
			else
				item->goal_anim_state = MONKEY_STAND;

			break;

		case MONKEY_STAND:
			monkey->flags = 0;
			monkey->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				AIGuard(monkey);

				if (!(GetRandomControl() & 0xF))
				{
					if (GetRandomControl() & 1)
						item->goal_anim_state = MONKEY_ANGRY;
					else
						item->goal_anim_state = MONKEY_SIT;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = MONKEY_WALK;
			else if (monkey->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = MONKEY_STAND;
				else
					item->goal_anim_state = MONKEY_RUN;
			}
			else if (monkey->mood == BORED_MOOD)
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (!(GetRandomControl() & 0xF))
					item->goal_anim_state = MONKEY_WALK;
				else if (!(GetRandomControl() & 0xF))
				{
					if (GetRandomControl() & 1)
						item->goal_anim_state = MONKEY_ANGRY;
					else
						item->goal_anim_state = MONKEY_SIT;
				}
			}
			else if (item->ai_bits & FOLLOW && (monkey->reached_goal || iDist > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = MONKEY_SIT;
				else
					item->goal_anim_state = MONKEY_RUN;
			}
			else if (info.bite && info.distance < 0x1C639)
			{
				if (lara_item->pos.y_pos >= item->pos.y_pos)
					item->goal_anim_state = MONKEY_ATAK_LOW;
				else
					item->goal_anim_state = MONKEY_ATAK_HIGH;
			}
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = MONKEY_ATAK_JUMP;
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = MONKEY_WALK;
			else if (info.distance < 0x718E4 && enemy != lara_item && enemy &&
				enemy->object_number != AI_PATROL1 && enemy->object_number != AI_PATROL2 &&
				abs(item->pos.y_pos - enemy->pos.y_pos) < 256)
				item->goal_anim_state = MONKEY_PICKUP;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = MONKEY_ROLL;
			else
				item->goal_anim_state = MONKEY_RUN;

			break;

		case MONKEY_RUN:
			monkey->maximum_turn = 2002;
			tilt = angle / 2;

			if (item->ai_bits & GUARD)
				item->goal_anim_state = MONKEY_STAND;
			else if (monkey->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = MONKEY_STAND;
			}
			else if (item->ai_bits & FOLLOW && (monkey->reached_goal || iDist > 0x400000))
				item->goal_anim_state = MONKEY_STAND;
			else if (monkey->mood == BORED_MOOD)
				item->goal_anim_state = MONKEY_ROLL;
			else if (info.distance < 0x718E4)
				item->goal_anim_state = MONKEY_STAND;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = MONKEY_ROLL;

			break;

		case MONKEY_PICKUP:
			monkey->reached_goal = 1;

			if (!enemy)
				break;

			if ((enemy->object_number == MEDI_ITEM || enemy->object_number == KEY_ITEM4) && item->frame_number == anims[item->anim_number].frame_base + 12)
			{
				if (enemy->room_number == NO_ROOM || enemy->status == ITEM_INVISIBLE || enemy->flags & IFL_CLEARBODY)
				{
					monkey->enemy = 0;
					break;
				}

				item->carried_item = enemy - items;
				RemoveDrawnItem(item->carried_item);
				enemy->room_number = NO_ROOM;
				enemy->carried_item = NO_ITEM;

				for (lp = 0; lp < MAX_LOT; lp++)
				{
					slot = &baddie_slots[lp];

					if (slot->item_num != NO_ITEM && slot->item_num != item_number && slot->enemy == enemy)
						slot->enemy = 0;
				}

				monkey->enemy = 0;

				if (item->ai_bits != MODIFY)
					item->ai_bits |= AMBUSH | MODIFY;
			}
			else if (enemy->object_number == AI_AMBUSH && item->frame_number == anims[item->anim_number].frame_base + 12)
			{
				item->ai_bits = 0;
				pickup = &items[item->carried_item];
				pickup->pos.x_pos = item->pos.x_pos;
				pickup->pos.y_pos = item->pos.y_pos;
				pickup->pos.z_pos = item->pos.z_pos;
				ItemNewRoom(item->carried_item, item->room_number);
				item->carried_item = NO_ITEM;
				pickup->ai_bits = GUARD;
				monkey->enemy = 0;
			}
			else
			{
				monkey->maximum_turn = 0;

				if (abs(info.angle) < 1274)
					item->pos.y_rot += info.angle;
				else if (info.angle < 0)
					item->pos.y_rot -= 1274;
				else
					item->pos.y_rot += 1274;
			}

			break;

		case MONKEY_SIT:
			monkey->flags = 0;
			monkey->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				AIGuard(monkey);

				if (!(GetRandomControl() & 0xF))
				{
					if (GetRandomControl() & 1)
						item->goal_anim_state = MONKEY_SCRATCH;
					else
						item->goal_anim_state = MONKEY_EAT;
				}
			}
			else if (item->ai_bits & PATROL1)
				item->goal_anim_state = MONKEY_WALK;
			else if (monkey->mood == ESCAPE_MOOD)
				item->goal_anim_state = MONKEY_STAND;
			else if (monkey->mood == BORED_MOOD)
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (!(GetRandomControl() & 0xF))
					item->goal_anim_state = MONKEY_WALK;
				else if (!(GetRandomControl() & 0xF))
				{
					if (GetRandomControl() & 1)
						item->goal_anim_state = MONKEY_SCRATCH;
					else
						item->goal_anim_state = MONKEY_EAT;
				}
			}
			else if (item->ai_bits & FOLLOW && (monkey->reached_goal || iDist > 0x400000))
			{
				if (item->required_anim_state)
					item->goal_anim_state = item->required_anim_state;
				else if (info.ahead)
					item->goal_anim_state = MONKEY_SIT;
				else
					item->goal_anim_state = MONKEY_STAND;
			}
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = MONKEY_STAND;
			else if (info.bite && info.distance < 0x718E4)
				item->goal_anim_state = MONKEY_WALK;
			else
				item->goal_anim_state = MONKEY_STAND;

			break;

		case MONKEY_ATAK_LOW:

			if (info.ahead)
				torso_y = info.angle;

			monkey->maximum_turn = 0;

			if (abs(info.angle) < 1274)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 1274;
			else
				item->pos.y_rot += 1274;

			if (enemy == lara_item)
			{
				if (!monkey->flags && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 40;
					lara_item->hit_status = 1;
					CreatureEffect(item, &monkey_hit, DoBloodSplat);
					monkey->flags = 1;
				}
			}
			else if (!monkey->flags && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 256 && abs(enemy->pos.y_pos - item->pos.y_pos) <= 256 && abs(enemy->pos.z_pos - item->pos.z_pos) < 256)
				{
					enemy->hit_points -= 20;
					enemy->hit_status = 1;
					monkey->flags = 1;
					CreatureEffect(item, &monkey_hit, DoBloodSplat);
				}
			}

			break;

		case MONKEY_ATAK_HIGH:

			if (info.ahead)
				torso_y = info.angle;

			monkey->maximum_turn = 0;

			if (abs(info.angle) < 1274)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 1274;
			else
				item->pos.y_rot += 1274;

			if (enemy == lara_item)
			{
				if (!monkey->flags && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 40;
					lara_item->hit_status = 1;
					CreatureEffect(item, &monkey_hit, DoBloodSplat);
					monkey->flags = 1;
				}
			}
			else if (!monkey->flags && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 256 && abs(enemy->pos.y_pos - item->pos.y_pos) <= 256 && abs(enemy->pos.z_pos - item->pos.z_pos) < 256)
				{
					enemy->hit_points -= 20;
					enemy->hit_status = 1;
					monkey->flags = 1;
					CreatureEffect(item, &monkey_hit, DoBloodSplat);
				}
			}

		//	break;	//idk if fallthrough here is on purpose..

		case MONKEY_ATAK_JUMP:

			if (info.ahead)
				torso_y = info.angle;

			monkey->maximum_turn = 0;

			if (abs(info.angle) < 1274)
				item->pos.y_rot += info.angle;
			else if (info.angle < 0)
				item->pos.y_rot -= 1274;
			else
				item->pos.y_rot += 1274;

			if (enemy == lara_item)
			{
				if (monkey->flags != 1 && item->touch_bits & 0x2400)
				{
					lara_item->hit_points -= 50;
					lara_item->hit_status = 1;
					CreatureEffect(item, &monkey_hit, DoBloodSplat);
					monkey->flags = 1;
				}
			}
			else if (monkey->flags != 1 && enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 256 && abs(enemy->pos.y_pos - item->pos.y_pos) <= 256 && abs(enemy->pos.z_pos - item->pos.z_pos) < 256)
				{
					enemy->hit_points -= 25;
					enemy->hit_status = 1;
					monkey->flags = 1;
					CreatureEffect(item, &monkey_hit, DoBloodSplat);
				}
			}

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);

	if (item->current_anim_state >= MONKEY_CLIMB4)
	{
		monkey->maximum_turn = 0;
		CreatureAnimation(item_number, angle, 0);
	}
	else
	{
		switch (CreatureVault(item_number, angle, 2, 128))
		{
		case -4:
			monkey->maximum_turn = 0;
			item->anim_number = objects[MONKEY].anim_index + 20;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_DOWN4;
			break;

		case -3:
			monkey->maximum_turn = 0;
			item->anim_number = objects[MONKEY].anim_index + 21;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_DOWN3;
			break;

		case -2:
			monkey->maximum_turn = 0;
			item->anim_number = objects[MONKEY].anim_index + 22;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_DOWN2;
			break;

		case 2:
			monkey->maximum_turn = 0;
			item->anim_number = objects[MONKEY].anim_index + 19;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_CLIMB2;
			break;

		case 3:
			monkey->maximum_turn = 0;
			item->anim_number = objects[MONKEY].anim_index + 18;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_CLIMB3;
			break;

		case 4:
			monkey->maximum_turn = 0;
			item->anim_number = objects[MONKEY].anim_index + 17;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = MONKEY_CLIMB4;
			break;
		}
	}
}

void DrawMonkey(ITEM_INFO* item)
{
	OBJECT_INFO* obj;
	short** mesh;
	short** swap;
	long* bone;
	short* frm[2];
	short* extra_rotation;
	short* rot;
	short* rot2;
	long frac, rate, clip, bit, lp;

	obj = &objects[item->object_number];
	frac = GetFrames(item, frm, &rate);

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

	mesh = &meshes[objects[MONKEY].mesh_index];

	if (item->ai_bits == MODIFY)
		swap = &meshes[objects[MESHSWAP3].mesh_index];
	else
		swap = &meshes[objects[MESHSWAP2].mesh_index];

	bone = &bones[obj->bone_index];
	bit = 1;

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
