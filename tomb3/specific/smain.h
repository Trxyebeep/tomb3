#pragma once
#include "../global/types.h"

void inject_smain(bool replace);

bool S_LoadSettings();
void S_SaveSettings();
void CheckCheatMode();
long TitleSequence();
long GameMain();

extern long HiResFlag;
extern long title_loaded;
extern char exit_message[128];
extern bool inject_rando;

#ifdef RANDO_STUFF
struct rando_level
{
	uchar original_id;
	uchar current_id;
	bool hasRain;
	bool hasSnow;
	bool freezingWater;
	char nSecrets;
};

struct rando_info
{
	uchar nLevels;
	rando_level levels[21];
};

extern rando_info rando;
#define RANDOLEVEL	CurrentLevel
#endif
