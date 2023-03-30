#pragma once
#include "../global/types.h"

void ControlWillbossPlasmaBall(short fx_number);
void InitialiseWillBoss(short item_number);
void WillBossControl(short item_number);
void S_DrawWillBoss(ITEM_INFO* item);

enum WILLBOSS_STATES
{
	WILLBOSS_STOP,
	WILLBOSS_WALK,
	WILLBOSS_LUNGE,
	WILLBOSS_BIGKILL,
	WILLBOSS_STUNNED,
	WILLBOSS_KNOCKOUT,
	WILLBOSS_GETUP,
	WILLBOSS_WALKATAK1,
	WILLBOSS_WALKATAK2,
	WILLBOSS_180,
	WILLBOSS_SHOOT
};

extern SHIELD_POINTS WillBossShield[40];
