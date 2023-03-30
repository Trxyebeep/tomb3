#pragma once
#include "../global/types.h"

void InitialisePrisoner(short item_number);
void PrisonerControl(short item_number);

enum BOB_STATES
{
	BOB_EMPTY,
	BOB_STOP,
	BOB_WALK,
	BOB_PUNCH2,
	BOB_AIM2,
	BOB_WAIT,
	BOB_AIM1,
	BOB_AIM0,
	BOB_PUNCH1,
	BOB_PUNCH0,
	BOB_RUN,
	BOB_DEATH,
	BOB_CLIMB3,
	BOB_CLIMB1,
	BOB_CLIMB2,
	BOB_FALL3
};
