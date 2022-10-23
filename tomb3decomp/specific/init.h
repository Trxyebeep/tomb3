#pragma once
#include "../global/vars.h"

void inject_init(bool replace);

void ShutdownGame();
void CalculateWibbleTable();
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
void init_game_malloc();
void* game_malloc(long size, long type);
void game_free(long size, long type);
