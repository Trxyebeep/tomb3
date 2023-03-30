#pragma once
#include "../global/types.h"

void InitialiseDog(short item_number);
void DogControl(short item_number);

enum DOG_STATES
{
	DOG_EMPTY,
	DOG_STOP,
	DOG_WALK,
	DOG_RUN,
	DOG_JUMP,
	DOG_STALK,
	DOG_ATTACK1,
	DOG_HOWL,
	DOG_SLEEP,
	DOG_CROUCH,
	DOG_TURN,
	DOG_DEATH,
	DOG_ATTACK2
};
