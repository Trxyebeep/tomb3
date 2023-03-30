#include "../tomb3/pch.h"
#include "dog.h"
#include "box.h"
#include "../specific/game.h"
#include "../3dsystem/phd_math.h"
#include "effects.h"
#include "objects.h"
#include "control.h"
#include "lara.h"

static BITE_INFO dog_bite = { 0, 0, 100, 3 };

void InitialiseDog(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[item->object_number].anim_index + 8;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = DOG_STOP;
}

void DogControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* dog;
	AI_INFO info;
	long x, z, iDist;
	short angle, head, x_head, rnd, frame;
	static char DeathAnims[4] = { 20, 21, 22, 20 };

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	dog = (CREATURE_INFO*)item->data;
	angle = 0;
	head = 0;
	x_head = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != DOG_DEATH)
		{
			item->anim_number = objects[item->object_number].anim_index + DeathAnims[GetRandomControl() & 3];
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = DOG_DEATH;
		}
	}
	else
	{
		if (item->ai_bits)
			GetAITarget(dog);
		else
			dog->enemy = lara_item;

		CreatureAIInfo(item, &info);

		if (dog->enemy == lara_item)
			iDist = info.distance;
		else
		{
			x = lara_item->pos.x_pos - item->pos.x_pos;
			z = lara_item->pos.z_pos - item->pos.z_pos;
			phd_atan(z, x);
			iDist = SQUARE(x) + SQUARE(z);
		}

		if (info.ahead)
		{
			head = info.angle;
			x_head = info.x_angle;
		}

		GetCreatureMood(item, &info, 1);
		CreatureMood(item, &info, 1);

		if (dog->mood == BORED_MOOD)
			dog->maximum_turn >>= 1;

		angle = CreatureTurn(item, dog->maximum_turn);

		if (dog->hurt_by_lara || iDist < 0x900000 && !(item->ai_bits & MODIFY))
		{
			AlertAllGuards(item_number);
			item->ai_bits &= ~MODIFY;
		}

		rnd = (short)GetRandomControl();
		frame = item->frame_number - anims[item->anim_number].frame_base;

		switch (item->current_anim_state)
		{
		case DOG_EMPTY:
		case DOG_SLEEP:
			head = 0;
			x_head = 0;

			if (dog->mood != BORED_MOOD && item->ai_bits != MODIFY)
				item->goal_anim_state = DOG_STOP;
			else
			{
				dog->flags++;
				dog->maximum_turn = 0;

				if (dog->flags > 300 && rnd < 128)
					item->goal_anim_state = DOG_STOP;
			}

			break;

		case DOG_CROUCH:

			if (item->required_anim_state)
			{
				item->goal_anim_state = item->required_anim_state;
				break;
			}

		case DOG_STOP:
			dog->maximum_turn = 0;

			if (item->ai_bits & GUARD)
			{
				head = AIGuard(dog);

				if (!(GetRandomControl() & 0xFF))
				{
					if (item->current_anim_state == DOG_STOP)
						item->goal_anim_state = DOG_CROUCH;
					else
						item->goal_anim_state = DOG_STOP;
				}
			}
			else if (item->current_anim_state == DOG_CROUCH && rnd < 128)
				item->goal_anim_state = DOG_STOP;
			else if (item->ai_bits & PATROL1)
			{
				if (item->current_anim_state == DOG_STOP)
					item->goal_anim_state = DOG_WALK;
				else
					item->goal_anim_state = DOG_STOP;
			}
			else if (dog->mood == ESCAPE_MOOD)
			{
				if (lara.target == item || !info.ahead || item->hit_status)
				{
					item->required_anim_state = DOG_RUN;
					item->goal_anim_state = DOG_CROUCH;
				}
				else
					item->goal_anim_state = DOG_STOP;
			}
			else if (dog->mood == BORED_MOOD)
			{
				dog->flags = 0;
				dog->maximum_turn = 182;

				if (rnd < 256 && item->ai_bits & MODIFY && item->current_anim_state == DOG_STOP)
				{
					item->goal_anim_state = DOG_SLEEP;
					dog->flags = 0;
				}
				else if (rnd < 4096)
				{
					if (item->current_anim_state == DOG_STOP)
						item->goal_anim_state = DOG_WALK;
					else
						item->goal_anim_state = DOG_STOP;
				}
				else if (!(rnd & 0x1F))
					item->goal_anim_state = DOG_HOWL;
			}
			else
			{
				item->required_anim_state = DOG_RUN;

				if (item->current_anim_state == DOG_STOP)
					item->goal_anim_state = DOG_CROUCH;
			}

			break;

		case DOG_WALK:
			dog->maximum_turn = 546;

			if (item->ai_bits & PATROL1)
				item->goal_anim_state = DOG_WALK;
			else if (dog->mood == BORED_MOOD && rnd < 256)
				item->goal_anim_state = DOG_STOP;
			else
				item->goal_anim_state = DOG_STALK;

			break;

		case DOG_RUN:
			dog->maximum_turn = 1092;

			if (dog->mood == ESCAPE_MOOD)
			{
				if (lara.target != item && info.ahead)
					item->goal_anim_state = DOG_CROUCH;
			}
			else if (dog->mood == BORED_MOOD)
				item->goal_anim_state = DOG_CROUCH;
			else if (info.bite && info.distance < 0x100000)
				item->goal_anim_state = DOG_ATTACK1;
			else if (info.distance < 0x240000)
			{
				item->required_anim_state = DOG_STALK;
				item->goal_anim_state = DOG_CROUCH;
			}

			break;

		case DOG_STALK:
			dog->maximum_turn = 546;

			if (dog->mood == BORED_MOOD)
				item->goal_anim_state = DOG_CROUCH;
			else if (dog->mood == ESCAPE_MOOD)
				item->goal_anim_state = DOG_RUN;
			else if (info.bite && info.distance < 0x2C4E4)
			{
				item->goal_anim_state = DOG_ATTACK2;
				item->required_anim_state = DOG_STALK;
			}
			else if (info.distance > 0x240000 || item->hit_status)
				item->goal_anim_state = DOG_RUN;

			break;

		case DOG_ATTACK1:

			if (info.bite && item->touch_bits & 0x6648 && frame >= 4 && frame <= 14)
			{
				CreatureEffect(item, &dog_bite, DoBloodSplat);
				lara_item->hit_points -= 50;
				lara_item->hit_status = 1;
			}

			item->goal_anim_state = DOG_RUN;
			break;

		case DOG_HOWL:
			head = 0;
			x_head = 0;
			break;

		case DOG_ATTACK2:

			if (info.bite && item->touch_bits & 0x48 && (frame >= 9 && frame <= 12 || frame >= 22 && frame <= 25))
			{
				CreatureEffect(item, &dog_bite, DoBloodSplat);
				lara_item->hit_points -= 12;
				lara_item->hit_status = 1;
			}

			break;
		}
	}

	CreatureTilt(item, 0);
	CreatureJoint(item, 0, head);
	CreatureJoint(item, 1, x_head);
	CreatureAnimation(item_number, angle, 0);
}
