#pragma once
#include "../global/vars.h"

void inject_init(bool replace);

void ShutdownGame();

#define init_game_malloc	( (void(__cdecl*)()) 0x00485F60 )
#define game_malloc	( (void*(__cdecl*)(long, long)) 0x00485F90 )
#define game_free	( (void(__cdecl*)(long, long)) 0x00486010 )
#define init_water_table	( (void(__cdecl*)()) 0x00485AB0 )
#define CalculateWibbleTable	( (void(__cdecl*)()) 0x00486050 )
