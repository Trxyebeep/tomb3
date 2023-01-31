#pragma once
#include "../global/vars.h"

void inject_tonyboss(bool replace);

void ControlTonyFireBall(short fx_number);
void InitialiseTonyBoss(short item_number);
void TonyBossControl(short item_number);
void S_DrawTonyBoss(ITEM_INFO* item);

enum TONYBOSS_STATES
{
	TONYBOSS_WAIT,
	TONYBOSS_RISE,
	TONYBOSS_FLOAT,
	TONYBOSS_ZAPP,
	TONYBOSS_ROCKZAPP,
	TONYBOSS_BIGBOOM,
	TONYBOSS_DEATH
};
