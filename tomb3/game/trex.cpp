#include "../tomb3/pch.h"
#include "trex.h"
#include "box.h"
#include "objects.h"
#include "collide.h"
#include "lara.h"
#include "../specific/game.h"
#include "effects.h"
#include "../3dsystem/phd_math.h"
#include "draw.h"
#include "control.h"

static BITE_INFO trex_hit = { 0, 32, 64, 13 };

void DinoControl(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	ITEM_INFO* candidate;
	CREATURE_INFO* rex;
	AI_INFO info;
	long dist, best_dist, x, y, z;
	short angle, n;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	rex = (CREATURE_INFO*)item->data;
	angle = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state == DINO_STOP)
			item->goal_anim_state = DINO_DEATH;
		else
			item->goal_anim_state = DINO_STOP;
	}
	else
	{
		target = 0;
		best_dist = 0x7FFFFFFF;
		GetNearByRooms(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 4096, 0, item->room_number);

		for (int i = 0; i < number_draw_rooms; i++)
		{
			for (n = room[draw_rooms[i]].item_number; n != NO_ITEM; n = candidate->next_item)
			{
				candidate = &items[n];

				if ((candidate->object_number == FLARE_ITEM || candidate->object_number == RAPTOR) &&
					candidate->hit_points && candidate->status == ITEM_ACTIVE)
				{
					x = (candidate->pos.x_pos - item->pos.x_pos) >> 6;
					y = (candidate->pos.y_pos - item->pos.y_pos) >> 6;
					z = (candidate->pos.z_pos - item->pos.z_pos) >> 6;
					dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

					if (dist < best_dist)
					{
						target = candidate;
						best_dist = dist;
					}
				}
			}
		}

		rex->enemy = target;

		if (rex->hurt_by_lara)
			rex->enemy = lara_item;

		CreatureAIInfo(item, &info);
		GetCreatureMood(item, &info, 1);

		if (!item->item_flags[0] && !item->item_flags[1] && rex->enemy == lara_item)
			rex->mood = BORED_MOOD;

		CreatureMood(item, &info, 1);

		if (rex->mood == BORED_MOOD)
			rex->maximum_turn >>= 1;

		angle = CreatureTurn(item, rex->maximum_turn);

		if (item->touch_bits)
			lara_item->hit_points -= item->current_anim_state == DINO_RUN ? 10 : 1;

		rex->flags = rex->mood != ESCAPE_MOOD && !info.ahead && info.enemy_facing > -0x4000 && info.enemy_facing < 0x4000;

		if (!rex->flags && info.distance > 0x225510 && info.distance < 0x1000000 && info.bite)
			rex->flags = 1;

		if (lara.gun_type != LG_FLARE && (lara_item->current_anim_state == AS_STOP || lara_item->current_anim_state == AS_DUCK) &&
			lara_item->goal_anim_state == lara_item->current_anim_state && !item->hit_status)
		{
			if (item->item_flags[0] > 0)
				item->item_flags[0]--;
		}
		else
		{
			item->item_flags[0] = 120;
			item->item_flags[1] = 3;
		}

		switch (item->current_anim_state)
		{
		case DINO_STOP:

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;
			else if (rex->mood == BORED_MOOD || rex->flags)
				item->goal_anim_state = 2;
			else if (rex->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead && !item->hit_status)
				{
					if (GetRandomControl() < 256)
						item->goal_anim_state = DINO_ROAR;
					else
						item->goal_anim_state = DINO_STOP;
				}
				else
					item->goal_anim_state = DINO_RUN;
			}
			else if (info.distance < 0x225510 && info.bite)
			{
				if (item->item_flags[0])
					item->goal_anim_state = DINO_ATTACK2;
				else if (GetRandomControl() & 1)
				{
					if (item->item_flags[1])
						item->goal_anim_state = DINO_LONGROARSTART;
				}
				else if (item->item_flags[1])
					item->goal_anim_state = DINO_SNIFFSTART;
			}
			else
				item->goal_anim_state = DINO_RUN;

			break;

		case DINO_WALK:
			rex->maximum_turn = 364;

			if (rex->mood != BORED_MOOD || !rex->flags)
				item->goal_anim_state = DINO_STOP;
			else if (info.ahead && GetRandomControl() < 0x100)
			{
				item->required_anim_state = DINO_ROAR;
				item->goal_anim_state = DINO_STOP;
			}

			break;

		case DINO_RUN:
			rex->maximum_turn = 728;

			if (info.distance < 0x1900000 && info.bite)
				item->goal_anim_state = DINO_STOP;
			else if (rex->flags)
				item->goal_anim_state = DINO_STOP;
			else if (rex->mood == ESCAPE_MOOD || !info.ahead || GetRandomControl() >= 256)
			{
				if (rex->mood == BORED_MOOD)
					item->goal_anim_state = DINO_STOP;
				else if (rex->mood == ESCAPE_MOOD && lara.target != item && info.ahead)
					item->goal_anim_state = DINO_STOP;
			}
			else
			{
				item->required_anim_state = DINO_ROAR;
				item->goal_anim_state = DINO_STOP;
			}

			break;

		case DINO_ROAR:
			rex->maximum_turn = 0;
			break;

		case DINO_ATTACK2:
			rex->maximum_turn = 364;

			if (rex->enemy == lara_item)
			{
				if (item->touch_bits & 0x3000)
				{
					CreatureEffect(item, &trex_hit, DoBloodSplat);
					lara_item->hit_points -= 10000;
					lara_item->hit_status = 1;
					item->goal_anim_state = DINO_KILL;
					rex->maximum_turn = 0;
					CreatureKill(item, 11, DINO_KILL, EXTRA_DINOKILL);
				}
			}
			else if (rex->enemy && item->frame_number == anims[item->anim_number].frame_base + 20)
			{
				x = abs(rex->enemy->pos.x_pos - ((objects[TREX].pivot_length * phd_sin(item->pos.y_rot)) >> W2V_SHIFT) - item->pos.x_pos);
				y = abs(rex->enemy->pos.y_pos - item->pos.y_pos);
				z = abs(rex->enemy->pos.z_pos - ((objects[TREX].pivot_length * phd_cos(item->pos.y_rot)) >> W2V_SHIFT) - item->pos.z_pos);

				if (x < 0x718E4 && y < 0x718E4 && z < 0x718E4)
				{
					if (rex->enemy->object_number == FLARE_ITEM)
						rex->enemy->hit_points = 0;
					else
					{
						rex->enemy->hit_points -= 50;
						rex->enemy->hit_status = 1;
						CreatureEffect(item, &trex_hit, DoBloodSplat);
					}
				}
			}

			if (!(GetRandomControl() & 3))
				item->required_anim_state = DINO_WALK;

			break;

		case DINO_KILL:
			rex->maximum_turn = 0;
			CreatureEffect(item, &trex_hit, DoBloodSplat);
			break;

		case DINO_LONGROARSTART:
			rex->maximum_turn = 0;

			if (item->item_flags[1] > 0 && item->frame_number == anims[item->anim_number].frame_base)
			{
				item->item_flags[1]--;

				if (rex->enemy && rex->enemy->object_number == FLARE_ITEM)
				{
					rex->enemy->hit_points = 0;
					item->item_flags[1]--;
				}
			}

			item->goal_anim_state = DINO_LONGROAREND;
			break;

		case DINO_SNIFFSTART:
			rex->maximum_turn = 0;

			if (item->item_flags[1] > 0 && item->frame_number == anims[item->anim_number].frame_base)
			{
				item->item_flags[1]--;

				if (rex->enemy && rex->enemy->object_number == FLARE_ITEM)
				{
					rex->enemy->hit_points = 0;
					item->item_flags[1] = 0;
				}
			}

			break;

		case DINO_SNIFFMID:
			rex->maximum_turn = 0;

			if (item->frame_number == anims[item->anim_number].frame_base)
			{
				if (GetRandomControl() & 1 && item->item_flags[1] && !item->item_flags[0])
				{
					item->goal_anim_state = DINO_SNIFFMID;

					if (item->item_flags[1] > 0)
						item->item_flags[1] --;
				}
				else
					item->goal_anim_state = DINO_SNIFFEND;
			}

			break;
		}
	}

	CreatureAnimation(item_number, angle, 0);

	if (item->hit_points > 0)
		item->collidable = 1;
	else
		item->collidable = 0;
}
