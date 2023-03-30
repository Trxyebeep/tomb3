#pragma once
#include "../global/types.h"

void InitialiseCamera();
long mgLOS(GAME_VECTOR* start, GAME_VECTOR* target, long push);
long CameraCollisionBounds(GAME_VECTOR* ideal, long push, long yfirst);
void MoveCamera(GAME_VECTOR* ideal, long speed);
void ChaseCamera(ITEM_INFO* item);
void CombatCamera(ITEM_INFO* item);
void LookCamera(ITEM_INFO* item);
void FixedCamera();
void CalculateCamera();

extern CAMERA_INFO camera;

extern GAME_VECTOR ForcedFixedCamera;
extern char UseForcedFixedCamera;
