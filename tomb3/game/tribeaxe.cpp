#include "../tomb3/pch.h"
#include "tribeaxe.h"
#include "box.h"
#include "../specific/game.h"
#include "effects.h"
#include "sound.h"
#include "objects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO tribeaxe_hit = { 0, 16, 265, 13 };

static uchar tribeaxe_hitframes[13][3] =
{
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{2, 12, 8},
	{8, 9, 32},
	{19, 28, 8},
	{0, 0, 0},
	{0, 0, 0},
	{7, 14, 8},
	{0, 0, 0},
	{15, 19, 32}
};

void TribeAxeControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* enemy;
	CREATURE_INFO* axe;
	AI_INFO info;
	short angle, head, tilt;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	axe = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	tilt = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != TRIBEAXE_DEATH)
		{
			if (item->current_anim_state == TRIBEAXE_WAIT1 || item->current_anim_state == TRIBEAXE_ATTACK4)
				item->anim_number = objects[item->object_number].anim_index + 21;
			else
				item->anim_number = objects[item->object_number].anim_index + 20;

			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = TRIBEAXE_DEATH;
		}
	}
	else
	{
		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);

		if (axe->enemy == lara_item && axe->hurt_by_lara && info.distance > 0x900000 && info.enemy_facing < 0x3000 && info.enemy_facing > -0x3000)
			axe->mood = ESCAPE_MOOD;

		CreatureMood(item, &info, 1);
		angle = CreatureTurn(item, axe->maximum_turn);

		if (info.ahead)
			head = info.angle;

		switch (item->current_anim_state)
		{
		case TRIBEAXE_WAIT1:
			axe->maximum_turn = 728;
			axe->flags = 0;

			if (axe->mood == BORED_MOOD)
			{
				axe->maximum_turn = 0;

				if (GetRandomControl() < 0x100)
					item->goal_anim_state = TRIBEAXE_WALK;
			}
			else if (axe->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = TRIBEAXE_WAIT1;
				else
					item->goal_anim_state = TRIBEAXE_RUN;
			}
			else if (item->item_flags[0])
			{
				item->item_flags[0] = 0;
				item->goal_anim_state = TRIBEAXE_WAIT2;
			}
			else if (info.ahead && info.distance < 0x718E4)
				item->goal_anim_state = TRIBEAXE_ATTACK4;
			else if (info.ahead && info.distance < 0x100000)
			{
				if (GetRandomControl() < 0x4000)
					item->goal_anim_state = TRIBEAXE_WALK;
				else
					item->goal_anim_state = TRIBEAXE_ATTACK4;
			}
			else if (info.ahead && info.distance < 0x400000)
				item->goal_anim_state = TRIBEAXE_WALK;
			else
				item->goal_anim_state = TRIBEAXE_RUN;

			break;

		case TRIBEAXE_WALK:
			axe->maximum_turn = 1638;
			axe->flags = 0;
			tilt = angle >> 3;

			if (axe->mood == BORED_MOOD)
			{
				axe->maximum_turn = 409;

				if (GetRandomControl() < 0x100)
				{
					if (GetRandomControl() < 0x2000)
						item->goal_anim_state = TRIBEAXE_WAIT1;
					else
						item->goal_anim_state = TRIBEAXE_WAIT2;
				}
			}
			else if (axe->mood==ESCAPE_MOOD)
				item->goal_anim_state = TRIBEAXE_RUN;
			else if (info.ahead && info.distance < 0x718E4)
			{
				if (GetRandomControl() < 0x2000)
					item->goal_anim_state = TRIBEAXE_WAIT1;
				else
					item->goal_anim_state = TRIBEAXE_WAIT2;
			}
			else if (info.distance > 0x400000)
				item->goal_anim_state = TRIBEAXE_RUN;

			break;

		case TRIBEAXE_RUN:
			axe->maximum_turn = 1092;
			axe->flags = 0;
			tilt = angle >> 2;

			if (axe->mood == BORED_MOOD)
			{
				axe->maximum_turn = 273;

				if (GetRandomControl() < 0x100)
				{
					if (GetRandomControl() < 0x4000)
						item->goal_anim_state = TRIBEAXE_WAIT1;
					else
						item->goal_anim_state = TRIBEAXE_WAIT2;
				}
			}
			else if (axe->mood == ESCAPE_MOOD && lara.target != item && info.ahead)
				item->goal_anim_state = TRIBEAXE_WAIT2;
			else if (info.bite || info.distance < 0x400000)
			{
				if (GetRandomControl() < 0x4000)
					item->goal_anim_state = TRIBEAXE_ATTACK6;
				else if (GetRandomControl() < 0x2000)
					item->goal_anim_state = TRIBEAXE_ATTACK5;
				else
					item->goal_anim_state = TRIBEAXE_WALK;
			}

			break;

		case TRIBEAXE_ATTACK2:
		case TRIBEAXE_ATTACK3:
		case TRIBEAXE_ATTACK4:
		case TRIBEAXE_ATTACK5:
		case TRIBEAXE_ATTACK6:
			item->item_flags[0] = 1;
			axe->maximum_turn = 728;
			axe->flags = item->frame_number - anims[item->anim_number].frame_base;
			enemy = axe->enemy;

			if (enemy == lara_item)
			{
				if (item->touch_bits & 0x2000 &&
					axe->flags >= tribeaxe_hitframes[item->current_anim_state][0] && axe->flags <= tribeaxe_hitframes[item->current_anim_state][1])
				{
					lara_item->hit_points -= tribeaxe_hitframes[item->current_anim_state][2];
					lara_item->hit_status = 1;

					for (int i = 0; i < tribeaxe_hitframes[item->current_anim_state][2]; i += 8)
						CreatureEffect(item, &tribeaxe_hit, DoBloodSplat);

					SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
				}
			}
			else if (enemy)
			{
				if (abs(enemy->pos.x_pos - item->pos.x_pos) < 512 &&
					abs(enemy->pos.y_pos - item->pos.y_pos) < 512 &&
					abs(enemy->pos.z_pos - item->pos.z_pos) < 512)
				{
					if (axe->flags >= tribeaxe_hitframes[item->current_anim_state][0] && axe->flags <= tribeaxe_hitframes[item->current_anim_state][1])
					{
						enemy->hit_points -= 2;
						enemy->hit_status = 1;
						CreatureEffect(item, &tribeaxe_hit, DoBloodSplat);
						SoundEffect(SFX_LARA_THUD, &item->pos, SFX_DEFAULT);
					}
				}
			}

			break;

		case TRIBEAXE_AIM3:
			axe->maximum_turn = 728;

			if (info.bite || info.distance < 0x718E4)
				item->goal_anim_state = TRIBEAXE_ATTACK3;
			else
				item->goal_anim_state = TRIBEAXE_WAIT2;

			break;

		case TRIBEAXE_WAIT2:
			axe->maximum_turn = 728;
			axe->flags = 0;

			if (axe->mood == BORED_MOOD)
			{
				axe->maximum_turn = 0;

				if (GetRandomControl() < 0x100)
					item->goal_anim_state = TRIBEAXE_WALK;
			}
			else if (axe->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
					item->goal_anim_state = TRIBEAXE_WAIT1;
				else
					item->goal_anim_state = TRIBEAXE_RUN;
			}
			else if (info.ahead && info.distance < 0x718E4)
			{
				if (GetRandomControl() < 0x800)
					item->goal_anim_state = TRIBEAXE_ATTACK2;
				else
					item->goal_anim_state = TRIBEAXE_AIM3;
			}
			else if (info.distance < 0x400000)
				item->goal_anim_state = TRIBEAXE_WALK;
			else
				item->goal_anim_state = TRIBEAXE_RUN;

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, head >> 1);
	CreatureJoint(item, 1, head >> 1);
	CreatureAnimation(item_number, angle, 0);
}
