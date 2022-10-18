#pragma once
#include "../global/vars.h"

void inject_init(bool replace);

void ShutdownGame();
void CalculateWibbleTable();
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();

#define init_game_malloc	( (void(__cdecl*)()) 0x00485F60 )
#define game_malloc	( (void*(__cdecl*)(long, long)) 0x00485F90 )
#define game_free	( (void(__cdecl*)(long, long)) 0x00486010 )
