#pragma once
#include "../global/vars.h"

void inject_camera(bool replace);

void InitialiseCamera();
long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push);
long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst);
void MoveCamera(GAME_VECTOR* ideal, long speed);
void ChaseCamera(ITEM_INFO* item);
void CombatCamera(ITEM_INFO* item);

#define CalculateCamera	( (void(__cdecl*)()) 0x004198FC )
