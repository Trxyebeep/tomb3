#include "../tomb3/pch.h"
#include "winston.h"
#include "box.h"
#include "sound.h"
#include "../specific/game.h"

void OldWinstonControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* winston;
	AI_INFO info;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	winston = (CREATURE_INFO*)item->data;
	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, winston->maximum_turn);

	if (item->current_anim_state == 1)
	{
		if ((info.distance > 0x240000 || !info.ahead) && item->goal_anim_state != 2)
		{
			item->goal_anim_state = 2;
			SoundEffect(SFX_WILARD_STAB, &item->pos, SFX_DEFAULT);
		}
	}
	else if (info.distance < 0x240000)
	{
		if (info.ahead)
		{
			item->goal_anim_state = 1;

			if (winston->flags & 1)
				winston->flags--;
		}
		else if (!(winston->flags & 1))
		{
			SoundEffect(SFX_WILARD_ODD_NOISE, &item->pos, SFX_DEFAULT);
			SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);
			winston->flags |= 1;
		}
	}

	if (item->touch_bits)
	{
		if (!(winston->flags & 2))
		{
			SoundEffect(SFX_LITTLE_SUB_LOOP, &item->pos, SFX_DEFAULT);
			SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);
			winston->flags |= 2;
		}
	}
	else if (winston->flags & 2)
		winston->flags -= 2;

	if (GetRandomDraw() < 256)
		SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);

	CreatureAnimation(item_number, angle, 0);
}

void inject_winston(bool replace)
{
	INJECT(0x00474110, OldWinstonControl, replace);
}
