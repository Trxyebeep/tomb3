#pragma once
#include "../global/vars.h"

void inject_dog(bool replace);

void InitialiseDog(short item_number);

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
