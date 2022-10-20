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
void InGameCinematicCamera();
void ControlCinematicPlayer(short item_number);
