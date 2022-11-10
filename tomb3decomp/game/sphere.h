#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

void InitInterpolate2(long frac, long rate);
long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace);

#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, long)) 0x00468250 )
#define TestCollision	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00467E40 )
