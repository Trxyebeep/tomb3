#pragma once
#include "../global/vars.h"

void inject_londboss(bool replace);

long KnockBackCollision(EXPLOSION_RING* ring);
void ControlLondBossPlasmaBall(short fx_number);
void ControlLaserBolts(short item_number);
void InitialiseLondonBoss(short item_number);
void LondonBossControl(short item_number);

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
