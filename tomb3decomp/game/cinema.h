#pragma once
#include "../global/vars.h"

void inject_cinema(bool replace);

long DrawPhaseCinematic();
void InitialiseGenPlayer(short item_number);
void InitCinematicRooms();
long GetCinematicRoom(long x, long y, long z);
void LaraControlCinematic(short item_number);
void InitialisePlayer1(short item_number);
void UpdateLaraGuns();
void CalculateCinematicCamera();

#define InGameCinematicCamera	( (void(__cdecl*)()) 0x0041B2A0 )
