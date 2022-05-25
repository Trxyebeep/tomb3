#pragma once
#include "../global/vars.h"

void inject_cinema(bool replace);

long DrawPhaseCinematic();
void InitialiseGenPlayer(short item_number);

#define InGameCinematicCamera	( (void(__cdecl*)()) 0x0041B2A0 )
