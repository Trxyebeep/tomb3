#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

void InitInterpolate2(long frac, long rate);

#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, long)) 0x00468250 )
#define TestCollision	( (long(__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00467E40 )
#define GetSpheres	( (long(__cdecl*)(ITEM_INFO*, SPHERE*, long)) 0x00467F70 )
