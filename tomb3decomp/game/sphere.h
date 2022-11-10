#pragma once
#include "../global/vars.h"

void inject_sphere(bool replace);

void InitInterpolate2(long frac, long rate);
long GetSpheres(ITEM_INFO* item, SPHERE* ptr, long WorldSpace);
long TestCollision(ITEM_INFO* item, ITEM_INFO* l);

#define GetJointAbsPosition	( (void(__cdecl*)(ITEM_INFO*, PHD_VECTOR*, long)) 0x00468250 )
