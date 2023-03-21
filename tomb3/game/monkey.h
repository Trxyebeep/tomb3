#pragma once
#include "../global/vars.h"

void InitialiseMonkey(short item_number);
void MonkeyControl(short item_number);
void DrawMonkey(ITEM_INFO* item);

void inject_monkey(bool replace);

enum MONKEY_STATES
{
	MONKEY_EMPTY,
	MONKEY_STOP,
	MONKEY_WALK,
	MONKEY_STAND,
	MONKEY_RUN,
	MONKEY_PICKUP,
	MONKEY_SIT,
	MONKEY_EAT,
	MONKEY_SCRATCH,
	MONKEY_ROLL,
	MONKEY_ANGRY,
	MONKEY_DEATH,
	MONKEY_ATAK_LOW,
	MONKEY_ATAK_HIGH,
	MONKEY_ATAK_JUMP,
	MONKEY_CLIMB4,
	MONKEY_CLIMB3,
	MONKEY_CLIMB2,
	MONKEY_DOWN4,
	MONKEY_DOWN3,
	MONKEY_DOWN2
};
