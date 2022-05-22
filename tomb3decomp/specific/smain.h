#pragma once
#include "../global/vars.h"

void inject_smain(bool replace);

bool S_LoadSettings();
void CheckCheatMode();

#ifdef RANDO_STUFF
struct rando_level
{
	uchar original_id;
	uchar current_id;
	bool hasRain;
	bool hasSnow;
	bool freezingWater;
};

struct rando_info
{
	uchar nLevels;
	rando_level levels[21];
};

extern rando_info rando;
#define RANDOLEVEL	(CurrentLevel - 1)
#endif
