#pragma once
#include "../global/vars.h"

void inject_smain(bool replace);

bool S_LoadSettings();
void CheckCheatMode();

#define S_SaveSettings	( (void(__cdecl*)()) 0x0048C8C0 )

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
extern bool inject_rando;
#define RANDOLEVEL	CurrentLevel
#endif
