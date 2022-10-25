#pragma once
#include "../global/vars.h"

void inject_savegame(bool replace);

void ModifyStartInfo(long level);

#define CreateSaveGameInfo	( (void(__cdecl*)()) 0x00461DD0 )
#define CreateStartInfo	( (void(__cdecl*)(long)) 0x00461B50 )
