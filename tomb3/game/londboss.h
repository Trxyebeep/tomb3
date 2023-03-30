#pragma once
#include "../global/types.h"

long KnockBackCollision(EXPLOSION_RING* ring);
void ControlLondBossPlasmaBall(short fx_number);
void ControlLaserBolts(short item_number);
void InitialiseLondonBoss(short item_number);
void LondonBossControl(short item_number);
void S_DrawLondonBoss(ITEM_INFO* item);

enum LONDONBOSSS_STATES
{
	LONDONBOSS_EMPTY,
	LONDONBOSS_STAND,
	LONDONBOSS_WALK,
	LONDONBOSS_RUN,
	LONDONBOSS_SUMMON,
	LONDONBOSS_BIGZAP,
	LONDONBOSS_DEATH,
	LONDONBOSS_LAUGH,
	LONDONBOSS_LILZAP,
	LONDONBOSS_VAULT2,
	LONDONBOSS_VAULT3,
	LONDONBOSS_VAULT4,
	LONDONBOSS_GODOWN
};

extern SHIELD_POINTS LondonBossShield[40];
extern EXPLOSION_RING KBRings[3];
