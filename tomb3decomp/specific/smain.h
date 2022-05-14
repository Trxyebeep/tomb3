#pragma once
#include "../global/vars.h"

void inject_smain(bool replace);

bool S_LoadSettings();

#ifdef RANDO_STUFF
struct rando_level
{
	uchar original_id;
	uchar current_id;
};

struct rando_info
{
	uchar nLevels;
	rando_level levels[20];
};

extern rando_info rando;
#endif
