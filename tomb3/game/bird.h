#pragma once
#include "../global/types.h"

void InitialiseVulture(short item_number);
void VultureControl(short item_number);

enum BIRD_STATES
{
	BIRD_EMPTY,
	BIRD_FLY,
	BIRD_STOP,
	BIRD_GLIDE,
	BIRD_FALL,
	BIRD_DEATH,
	BIRD_ATTACK,
	BIRD_EAT
};
