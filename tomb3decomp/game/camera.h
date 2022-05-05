#pragma once
#include "../global/vars.h"

void inject_camera(bool replace);

void InitialiseCamera();
void MoveCamera(GAME_VECTOR* ideal, long speed);

#define CalculateCamera	( (void(__cdecl*)()) 0x004198FC )
#define mgLOS	( (long(__cdecl*)(GAME_VECTOR*, GAME_VECTOR*, long)) 0x0041A641 )
