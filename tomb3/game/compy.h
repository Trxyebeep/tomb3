#pragma once
#include "../global/types.h"

void InitialiseCompy(short item_number);
void CompyControl(short item_number);
void CarcassControl(short item_number);

enum COMPY_STATES
{
	COMPY_STOP,
	COMPY_RUN,
	COMPY_JUMP,
	COMPY_ATTACK,
	COMPY_DEATH
};

extern long compy_scared_timer;
extern long compys_attack_lara;
